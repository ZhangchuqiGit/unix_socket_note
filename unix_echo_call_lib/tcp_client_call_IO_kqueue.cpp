
#include "tcp_client_call.h"

#include <sys/types.h>
//#include <event.h>
#include <ctime> // <sys/time.h>
#include <event2/event.h>
#include <fcntl.h>
#include <kqueue/sys/event.h> // sudo apt install -y libkqueue\*

/** 所谓的IO复用，就是同时等待多个文件描述符就绪，以系统调用的形式提供。
 如果所有文件描述符都没有就绪的话，该系统调用阻塞，否则调用返回，允许用户进行后续的操作。
 常见的 IO复用 技术有 select, poll, epoll 以及 kqueue 等等。
 其中 epoll 为 Linux 独占，而 kqueue 则在许多 UNIX系统上存在，包括 macOS。
 kqueue()在易用性上可能比 select()和 poll()更好一些。
 epoll()效率要比单独使用 poll()函数高很多，非常适合监听大量的描述符，
 许多高性能的服务器都使用 epoll()。        **/


/********************* I/O口复用 *************************/

/* I/O 复用 高级轮询技术 kqueue */
void tcp_client_call_kqueue(FILE *fp, const int &sock_fd)
{
	/* 判断文件是否存在 及 文件类型（文件、设备、管道等等） */
	int is_file = is_file_fp(fp); // 获取文件属性信息
	int retval; // 函数返回值
	int8_t stdineof = 0;
	char buf[MAXLINE];
	std::string strbuf;
	int FILE_fp = fileno(fp); // 转化文件描述符
	err_sys(FILE_fp, __LINE__, "fileno(fp)");
/*******************************************************************/
	struct kevent kev[2]; /** 所关注的事件 **/
	EV_SET(&kev[0],				/* 初始化 事件列表 kevent{} 的便利宏 */
		   FILE_fp,						/** 监听描述符 */
		   EVFILT_READ/*描述符可读*/,	/** 监听类型 */
		   EV_ADD/*添加事件*/, 			/** 操作方式 */
		   0,						/** 第二种操作方式 */
		   0,						/** 用户数据 */
		   nullptr						/** 用户数据标识符 */
	);
	EV_SET(&kev[1],				/* 初始化 事件列表 kevent{} 的便利宏 */
		   sock_fd,						/** 监听描述符 */
		   EVFILT_READ/*描述符可读*/,	/** 监听类型 */
		   EV_ADD/*添加事件*/, 			/** 操作方式 */
		   0,						/** 第二种操作方式 */
		   0,						/** 用户数据 */
		   nullptr						/** 用户数据标识符 */
	);
	int	kq = kqueue(); // 事件描述符
	err_sys(kq, __LINE__, "kqueue()");
	struct timespec	ts{};
	bzero(&ts, sizeof(ts)); // 清 0
	/* kevent()既用于 注册 所关注的事件，也用于 监视 所关注的事件 是否已发生。*/
	kevent(kq,							/** 监听描述符 */
		   kev, 2,	 		/** 监视 事件列表 */
		   nullptr, 0, /** 就绪 事件列表 */
		   &ts 							/** 超时设置 */
	); // 注册事件
	int event_num; // eventlist 已经触发的就绪事件的数目
	while (true)
	{
		/* kevent()既用于 注册 所关注的事件，也用于 监视 所关注的事件 是否已发生。*/
		event_num = kevent(kq,								/** 监听描述符 */
						   nullptr, 0, 	/** 监视 事件列表 */
						   kev, 2,	 				/** 就绪 事件列表 */
						   nullptr					/** 超时设置 */
		); // 监视事件，nullptr：阻塞进程
		err_sys(event_num, __LINE__, "kevent()");
/*******************************************************************/
		for (int i = 0; i < event_num; ++i)
		{
			if (kev[i].ident == (uintptr_t)FILE_fp) /* input is readable */
			{
				debug_line(__LINE__, (char *)__FUNCTION__);
				retval = Read(FILE_fp, buf, sizeof(buf));
				err_sys(retval, __LINE__, "Read()");
				if (if_quit_echo(buf) ) // end (FIN) !
				{
					debug_line(__LINE__, (char *)__FUNCTION__);
					retval = 0;
//					return;
				}
				if (retval == 0 || (is_file == 4 && retval == kev[i].data) )
				{
					debug_line(__LINE__, (char *)__FUNCTION__);
					retval = shutdown(sock_fd, SHUT_WR/*关闭写*/);
					err_sys(retval, __LINE__, "shutdown()");
					kev[i].flags = EV_DELETE;/*删除事件*/
					kevent(kq, &kev[i], 1,
						   nullptr, 0, &ts); /* remove kevent */
					std::cout << "客户端：关闭发送功能，等待接收中" << std::endl;
					stdineof = 1; // 当前客户端：关闭发送功能，等待接收中
					continue;
				}
				if (stdineof != 1) { /* 当前客户端 发送功能 */
					retval = Write(sock_fd, buf, strlen(buf));
					err_sys(retval, __LINE__, "Write()");
					if (buf[retval-1] == '\n') buf[retval-1] = '\0';
					std::cout << "发送: " << buf << std::endl;
				}
			}
			if (kev[i].ident == (uintptr_t)sock_fd) /* socket is readable */
			{
				retval = Read(sock_fd, buf, sizeof(buf));
				err_sys(retval, __LINE__, "Read()");
				if (retval == 0) // TCP connection closed (TCP 断开)
				{
					debug_line(__LINE__, (char *)__FUNCTION__);
					kev[i].ident = -1;
					/* ---- close () ---- */
					retval=close(sock_fd);
					err_sys(retval, __LINE__, "客户端 close()");
					if (stdineof == 1) std::cerr << "服务器断开" << std::endl;
					else err_sys(-1, __LINE__, "服务器异常");
					return;
				} else {
					if (buf[retval-1] == '\n') buf[retval-1] = '\0';
					std::cout << "接收: " << buf << std::endl;
				}
			}
		}
	}
}


/********************* I/O口复用 *************************/
/* I/O口复用
---------------------------------------------------------------------------
select()
	通过设置或者检查存放在数据结构 fd_set 中的标志位的来鉴别事件。Fd_set是一个输入输出参数，用户通过设置(FD_SET)相应的标志位标识关注的描述符，经内核拷贝到内核空间；内核根据输入fd_set 扫描对应的描述符，新建另一fd_set标识活跃的描述符，扫描完毕后将含有活跃描述符的fd_set 拷贝到用户空间。用户检查(FD_ISSET)内核输出的fd_set 确定活跃的描述符。（进程的fd的分配都是从3开始的，0、1、2已经用作标准输入，输出和错误？而fd_set的每一位对应一个fd。）
---------------------------------------------------------------------------
进程管理最大文件描述符
select 由宏定义FD_SETSIZE决定，用一个unsigned long数组表示。
 	一般FD_SETSIZE定义位1024，可以修改FD_SETSIZE来改变select管理描述符的数量。
poll 基于链表存储，无上限，但受内存上限限制
epoll 同 poll。
---------------------------------------------------------------------------
效率
select 内核和用户空间使用内核拷贝的方式交互数据，无论内核和用户空间，
 	都需要轮询整个fd_set,当随管理的fd增加时，效率会呈线性下降。
poll 同 select
epoll 没有内核拷贝，而且只返回就绪的fd。在侦听大量不活跃的fd时，效率比较高。
 	但在侦听少量活跃的fd时，性能不如前两者。因为epoll使用了复杂算法。
---------------------------------------------------------------------------
IO多路复用是指内核一旦发现过程指定的一个或者多个IO条件筹备读取，它就告诉该过程。
IO多路复用实用如下场合：
    当客户解决多个描述符时（个别是交互式输出和网络套接口），必须应用I/O复用。
    当一个客户同时解决多个套接口时，而这种状况是可能的，但很少呈现。
    如果一个TCP服务器既要解决监听套接口，又要解决已连贯套接口，个别也要用到I/O复用。
    如果一个服务器即要解决TCP，又要解决UDP，个别要应用I/O复用。
    如果一个服务器要解决多个服务或多个协定，个别要应用I/O复用。     */

/** FreeBSD 引入了 kqueue 接口。
 本接口允许进程向内核注册描述所关注 kqueue事件的事件过滤器。
 事件 除了与 select所关注类似的文件I/O和超时外，
 还有异步I/O、文件修改通知（例如文件被删除或修改时发出的通知）、
 进程跟踪（例如进程调用 exit或 fork时发出的通知）和信号处理。     **/

/********************* I/O口复用 *************************/
/* 	#include <kqueue/sys/event.h> // sudo apt install -y libkqueue\*
	#include <sys/types.h>, <event.h>, <sys/time.h>
---------------------------------------------------------------------------
 返回一个新的 kq 事件描述符，用于后续的 kevent()调用中。
 如果创建内核事件队列时出错，返回 -1 并设置 errno。
int kqueue (void);
---------------------------------------------------------------------------
 kevent()既用于 注册 所关注的事件，也用于 监视 所关注的事件 是否已发生。
int kevent (int 				  kq,
 			const struct kevent   *changelist, 	 int nchanges,
			struct kevent 		  *eventlist, 	 int nevents,
			const struct timespec *timeout );
参数：
 kq：监听描述符
 changelist 和 nchanges：监视事件列表，这两个参数给出对所关注事件做出的更改，
 	若无更改则分别取值 NULL和 0。如果 nchanges不为0，
 	kevent()就执行 changelist 数组中所请求的每个事件过滤器更改。
 eventlist 和 nevents：已经就绪的事件列表，
 	其条件已经触发的任何事件（包括刚在 changelist 中增设的那些事件）
 	由 kevent()通过 eventlist 参数返回，
 	它指向一个由 nevents个元素构成的 kevent{}结构数组。
 timeout：超时设置。NULL 阻塞进程，非0值 timespec 指定明确的超时值，
 	0值 timespec 执行非阻塞事件检查。
 	注意，kevent()使用的 timespec{}结构不同于 select()使用的 timeval{}结构，
 	前者的分辨率为 纳秒，后者的分辨率为 微秒。
返回：eventlist 中已经触发的事件数目，0 表示超时，出错返回 -1 并设置 errno。
---------------------------------------------------------------------------
	EV_SET() 用于 初始化 监视列表 kevent{} 的 便利 宏
#define EV_SET (kevp_, a, b, c, d, e, f) do {	\
				struct kevent *kevp = (kevp_);	\
				(kevp)->ident  = (a);			\
				(kevp)->filter = (b);			\
				(kevp)->flags  = (c);			\
				(kevp)->fflags = (d);			\
				(kevp)->data   = (e);			    \
				(kevp)->udata  = (f);			\
			} while(0)
---------------------------------------------------------------------------
struct kevent {
	uintptr_t ident;		监听描述符

	short	  filter;		监听类型
 			如 EVFILT_READ=读，EVFILT_WRITE=写，EVFILT_TIMER=定时器事件，
 			   EVFILT_SIGNAL=信号，EVFILT_USER=用户自定义事件

	u_short   flags; 		操作方式
 			如 EV_ADD 添加，EV_DELETE 删除，EV_ENABLE 激活，EV_DISABLE 不激活

	u_int 	  fflags;		第二种 操作方式，NOTE_TRIGGER 立即激活等等
	intptr_t  data;			用户数据，socket 里面它是可读写的数据长度
	void	  *udata;		不透明的用户数据标识符，
			指针类型的数据，可以携带任何想携带的附加数据。比如 对象 class
};
---------------------------------------------------------------------------
 filter 监听类型	说明
 EVFILT_READ 	描述符可读。
 EVFILT_WRITE	描述符可写。
 EVFILT_EMPTY 	在写入缓冲区中没有剩余数据时返回。
 EVFILT_AIO		异步IO事件。事件不会直接通过 kevent()注册，而是通过 异步IO 系统调用。
-------------------------
 EVFILT_VNODE 	文件修改和删除事件：
 	NOTE_ATTRIB 		文件的属性已更改。
 	NOTE_CLOSE 			文件描述符已关闭。关闭的文件描述符 没有 写访问权限。
	NOTE_CLOSE_WRITE 	文件描述符已关闭。关闭的文件描述符 具有 写访问权限。
 	NOTE_DELETE	  		描述符所引用的文件调用了 unlink()系统调用。
 	NOTE_EXTEND	  		对于常规文件，描述符引用的文件已扩展。
 					对目录 重命名操作、添加 或 删除 目录条目。
 					对 子目录 更改名称时，不会报告 NOTE_EXTEND 事件。
	NOTE_LINK	   		文件上的链接数已更改，如在描述符引用的目录内创建或删除了子目录。
	NOTE_OPEN	     	描述符引用的文件已打开。
	NOTE_WRITE	    	在描述符所引用的文件上发生 写 操作。
	NOTE_READ	      	在描述符所引用的文件上发生 读 操作。
	NOTE_RENAME	     	描述符引用的文件已重命名。
	NOTE_REVOKE	    	通过 revoke(2)撤消了对该文件的访问，或者卸载了基础文件系统。
-------------------------
 EVFILT_PROC 	进程已调用 exit()、fork()、exec()事件：
	NOTE_EXIT 	该过程已调用 exit()退出。退出状态将存储在数据 data中。
	NOTE_FORK	该进程已调用 fork()创建子进程。
	NOTE_EXEC	该进程已调用 exec类()执行 shell 命令。
	NOTE_TRACK	在 fork()调用中遵循一个过程。
 			父进程使用与原始事件相同的 fflag 注册一个新的kevent来监视子进程。
 			子进程将用 fflags 设置 NOTE_CHILD，并在数据中设置父PID来发出事件信号。
 			如果父进程未能注册新的事件（通常是由于资源限制），
 			它将发出一个在 fflags 中设置 NOTE_TRACKERR 的事件，
 			而子进程将不会发出 NOTE_CHILD 事件的信号。
-------------------------
 EVFILT_PROCDESC 	使用 fork 创建的进程描述符作为 fflags 的标识符和要监视的事件进行监视，
 			并在关联的进程执行一个或多个请求的事件时返回。要监视的事件是：
	NOTE_EXIT   该过程已退出。退出状态将存储在数据 data中。
-------------------------
 EVFILT_SIGNAL 		收到信号。
 			这与 signal()和 sigaction()设施共存，并且具有较低的优先级。
 			即使信号已被标记为 SIG_IGN，过滤器也会记录所有将信号发送到进程的尝试，
 			但 SIGCHLD 信号除外，如果忽略该信号，过滤器将不会记录该信号。
 			事件通知在正常的信号传递处理之后发生。
 			data 返回自上次调用 kevent()以来信号发生的次数。
 			该过滤器自动在内部设置 EV_CLEAR (flags 操作方式)标志。
-------------------------
 EVFILT_TIMER 		建立一个由 ident 标识的任意 计时器。
 			添加计时器时，数据 data 指定触发计时器的时间（对于NOTE_ABSTIME）或 超时时间。
 			除非指定了 EV_ONESHOT (flags 操作方式) 或 NOTE_ABSTIME，
 			否则计时器将是定期的。返回时，数据包含自上次调用 kevent()以来超时已过期的次数。
 			对于非单调计时器，此过滤器会在内部自动设置 EV_CLEAR (flags 操作方式)标志。
	过滤器在 fflags 参数中接受以下标志：
 			NOTE_SECONDS	   data	以秒为单位。
			NOTE_MSECONDS	   data	is in milliseconds.(默认毫秒)
			NOTE_USECONDS	   data	is in microseconds.
			NOTE_NSECONDS	   data	is in nanoseconds.
			NOTE_ABSTIME	  指定的到期时间是绝对值。
-------------------------
 EVFILT_USER	 建立一个由 ident 标识的用户事件，该事件与任何内核机制都不相关，
 			由用户级代码触发。fflags 的低24位可用于用户定义的标志，并可使用以下方式进行操作：
			NOTE_FFNOP	     	忽略 fflags。
			NOTE_FFAND	    	按位与 fflags。
			NOTE_FFOR	     	按位或 fflags.
			NOTE_FFCOPY	    Copy fflags.
			NOTE_FFCTRLMASK    Control mask for fflags.
			NOTE_FFLAGSMASK    User defined flag mask for fflags.
			返回时，fflags 在低24位中包含用户定义的标志。
---------------------------------------------------------------------------
 flags 操作方式(更改) 说明
 EV_ADD 		将事件添加到队列中。重新添加现有事件将修改原始事件的参数，
 			并且不会导致重复条目。添加事件会自动将其启用，除非被 EV_DISABLE 标志覆盖。
 EV_DISABLE	  	禁用该事件，但不删除。
 EV_ENABLE 		重新启用先前的禁用事件。
 EV_DISPATCH  	交付事件后 立即 禁用该事件。
 EV_DELETE	  	从队列中删除事件。附加到文件描述符的事件将在描述符的最后关闭时自动删除。
 EV_RECEIPT	 	该标志对于对 kqueue()进行批量更改而不耗尽任何未决事件很有用。
 			当作为输入传递时，它将强制始终返回 EV_ERROR。成功添加过滤器后，数据字段将为零。
 			请注意，如果遇到此标志，并且事件列表中没有剩余空间可容纳 EV_ERROR 事件，
 			则后续的更改将不会得到处理。
 EV_ONESHOT	  	触发一次后删除事件。
 EV_CLEAR	 	用户检索到事件后，将重置其状态。某些过滤器可能会在内部自动设置此标志。
-------------------------
 flags 操作方式(返回) 说明
 EV_EOF	 	发生 EOF 条件。
 EV_ERROR 	发生错误，errno 值 存放在 kevent{} 成员 data 中
---------------------------------------------------------------------------
 */
