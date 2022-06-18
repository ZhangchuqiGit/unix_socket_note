
#include "tcp_client_call.h"

#include <climits>		/* for OPEN_MAX */
#include <sys/epoll.h>


/** 所谓的IO复用，就是同时等待多个文件描述符就绪，以系统调用的形式提供。
 如果所有文件描述符都没有就绪的话，该系统调用阻塞，否则调用返回，允许用户进行后续的操作。
 常见的 IO复用 技术有 select, poll, epoll 以及 kqueue 等等。
 其中 epoll 为 Linux 独占，而 kqueue 则在许多 UNIX系统上存在，包括 macOS。
 kqueue()在易用性上可能比 select()和 poll()更好一些。
 epoll()效率要比单独使用 poll()函数高很多，非常适合监听大量的描述符，
 许多高性能的服务器都使用 epoll()。        **/

/**
epoll工作原理
epoll同样只告知那些就绪的文件描述符，而且当我们调用epoll_wait()获得就绪文件描述符时，
返回的不是实际的描述符，而是一个代表就绪描述符数量的值，
你只需要去epoll指定的一个数组中依次取得相应数量的文件描述符即可，
这里使用了内存映射（mmap）技术，这样便彻底省掉了这些文件描述符在系统调用时复制的开销。
另一个本质的改进在于epoll采用基于事件的就绪通知方式，在select/poll中，
进程只有在调用一定的方法后，内核才对所有监视的文件描述符进行扫描，
而epoll事先通过epoll_ctl()来注册一个文件描述符，一旦基于某个文件描述符就绪时，
内核会采用类似callback的回调机制，迅速激活这个文件描述符，
当进程调用epoll_wait()时便得到通知。  **/

/********************* I/O口复用 *************************/

/* I/O 复用 高级轮询技术 epoll */
void tcp_service_echo_IO_epoll(const int &listen_fd)
{
	int retval; // 函数返回值
	int accept_fd; // 新(连接成功)套接字描述符
	char buf[MAXLINE];
/*******************************************************************/
	/* 创建 epoll 模型，指定 fd 监听数量的最大值，返回描述符 epoll_fd */
	int epoll_fd = epoll_create(MAX_listen);
	err_sys(epoll_fd, __LINE__, "epoll_create()");

	struct epoll_event reg_event{};// 注册 epoll 模型
	reg_event.data.fd = listen_fd;	// 操作目标，是需要监听的（套接字）描述符
	reg_event.events = EPOLLIN;	// 感兴趣的事件：EPOLLIN 可读

	/* 注册管理 epoll 模型，事件注册函数，比如 添加描述符 EPOLL_CTL_ADD 等 */
	retval = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &reg_event);
	err_sys(retval, __LINE__, "epoll_ctl():listen_fd");

	struct epoll_event Ready_event[MAX_listen]; // 就绪事件列表
	/* -1 indicates available entry */
	for (auto & event : Ready_event) event.data.fd = -1;
	int num_ready; // 就绪描述符的数量
	while (true)
	{
		/* 等待I/O事件发生：指定毫秒为单位的最大等待时间 */
		num_ready = epoll_wait(epoll_fd, Ready_event, MAX_listen, 500);
		err_sys(num_ready, __LINE__, "epoll_wait()");
/*******************************************************************/
		for (int i = 0; i < num_ready; ++i)
		{
/*******************************************************************/
			if (Ready_event[i].data.fd == listen_fd) // new client connection
			{
				debug_line(__LINE__, (char *) __FUNCTION__);
				/* ---- accept () ---- */
				accept_fd = Accept(listen_fd, AF_UNSPEC);
				/* 文件控制操作: 将客户套接字描述符设置为非阻塞 */
				retval = fcntl(accept_fd, F_SETFL, O_NONBLOCK);
				err_sys(retval, __LINE__, "fcntl()");
				/* 感兴趣的事件：EPOLLIN 可读 + 边沿触发方式工作 */
				reg_event.data.fd = accept_fd;	// 操作目标，是需要监听的（套接字）描述符
				reg_event.events = EPOLLIN | EPOLLET; // 感兴趣的事件
				/* 注册管理 epoll 模型，事件注册函数 */
				retval = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &reg_event);
				err_sys(retval, __LINE__, "epoll_ctl():accept_fd");
				std::cout << "client fd : " << accept_fd << std::endl;
			}
/*******************************************************************/
				/* check all clients for data */
			else if(Ready_event[i].events & EPOLLIN) // 可读事件
			{
				if((accept_fd = Ready_event[i].data.fd) < 0) continue;
				/* socket is readable */
				debug_line(__LINE__, (char *)__FUNCTION__);
				retval = Read(accept_fd, buf, sizeof(buf));
				if (retval <= 0) {
					if (retval < 0) {
						if(errno == ECONNRESET)
							std::cerr<<"Connection reset by peer"<<std::endl;
						err_sys(retval, __LINE__, "Read()");
					}
					// TCP connection closed (TCP 断开)
					/* ---- close () ---- */
					retval = close(accept_fd);
					err_sys(retval, __LINE__, "close()");
					Ready_event[i].data.fd = -1;
					std::cerr<<"客户端["<<i<<"]("<<accept_fd<<")终止"<<std::endl;
				}
				else {
					if (if_run("time\n", buf)) {
						std::cout << "接收客户端: time" << std::endl;
						bzero(buf, sizeof(buf)); // 清 0
/* Return the current time and put it in *TIMER if timer is not nullptr */
						time_t ticks = time(nullptr);
						/* 格式化串"%a.bs" :
		对于 a，表示如果字符串长度小于a,那么右对齐左边补空格，若大于a则原样输出不限制
		对于 b，表示如果字符串长度超过b，那么只取前b个    */
						snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
					} else {
						if (buf[retval-1] == '\n') buf[retval-1] = '\0';
						std::cout << "接收客户端: " << buf << std::endl;
					}
					retval = Write(accept_fd, buf, strlen(buf));
					err_sys(retval, __LINE__, "Write()");
				}
			}
		}
	}
}

/********************* I/O口复用 *************************/
/* I/O口复用
---------------------------------------------------------------------------
Select()
	通过设置或者检查存放在数据结构 fd_set 中的标志位的来鉴别事件。Fd_set是一个输入输出参数，用户通过设置(FD_SET)相应的标志位标识关注的描述符，经内核拷贝到内核空间；内核根据输入fd_set 扫描对应的描述符，新建另一fd_set标识活跃的描述符，扫描完毕后将含有活跃描述符的fd_set 拷贝到用户空间。用户检查(FD_ISSET)内核输出的fd_set 确定活跃的描述符。（进程的fd的分配都是从3开始的，0、1、2已经用作标准输入，输出和错误？而fd_set的每一位对应一个fd。）
---------------------------------------------------------------------------
进程管理最大文件描述符
Select 由宏定义FD_SETSIZE决定，用一个unsigned long数组表示。
 	一般FD_SETSIZE定义位1024，可以修改FD_SETSIZE来改变select管理描述符的数量。
Poll 基于链表存储，无上限，但受内存上限限制
Epoll 同 poll。
---------------------------------------------------------------------------
效率
Select 内核和用户空间使用内核拷贝的方式交互数据，无论内核和用户空间，
 	都需要轮询整个fd_set,当随管理的fd增加时，效率会呈线性下降。
Poll 同 select
Epoll 没有内核拷贝，而且只返回就绪的fd。在侦听大量不活跃的fd时，效率比较高。
 	但在侦听少量活跃的fd时，性能不如前两者。因为epoll使用了复杂算法。
---------------------------------------------------------------------------
IO多路复用是指内核一旦发现过程指定的一个或者多个IO条件筹备读取，它就告诉该过程。
IO多路复用实用如下场合：
    当客户解决多个描述符时（个别是交互式输出和网络套接口），必须应用I/O复用。
    当一个客户同时解决多个套接口时，而这种状况是可能的，但很少呈现。
    如果一个TCP服务器既要解决监听套接口，又要解决已连贯套接口，个别也要用到I/O复用。
    如果一个服务器即要解决TCP，又要解决UDP，个别要应用I/O复用。
    如果一个服务器要解决多个服务或多个协定，个别要应用I/O复用。
---------------------------------------------------------------------------*/

/********************* I/O口复用 *************************/
/*	epoll()函数
---------------------------------------------------------------------------
 // 创建 epoll 模型，指定 fd 监听数量的最大值，返回描述符 epoll_fd，
int epoll_create (int size);
参数：size 指定 fd 监听数量的最大值，注意是 fd 个数，不是 fd 最大值。
返回：成功返回 描述符 epoll_fd，失败返回 -1，见 errno。
备注：创建 epoll 句柄会占用一个 fd 值，在使用完 epoll 后，
 	必须调用 close() 关闭，否则可能导致 fd 被耗尽。
--------------------------------
int epoll_create1 (int flags);
---------------------------------------------------------------------------
 // 注册管理 epoll 模型，事件注册函数，比如 添加描述符 等：
int epoll_ctl(int epoll_fd, int op, int fd, struct epoll_event *event);
参数：
 epoll_fd：epoll_create()返回的描述符。
 op：操作常量，用三个宏表示表示 动作：
				EPOLL_CTL_ADD 	注册新的 fd 到 epfd()中
				EPOLL_CTL_MOD 	修改已经注册的 fd 的监听事件
				EPOLL_CTL_DEL 	从 epfd 中删除一个 fd
 fd：操作目标，是需要监听的（套接字）描述符。
 event：数据结构，感兴趣的事件以及任何关联的用户数据。
 返回：成功返回 0，错误返回 -1，见 errno。
---------------------------------------------------------------------------
 event{} 是设定监听事件的结构体，数据结构如下：
typedef union epoll_data
{
  	void 	 *ptr;
  	int 	 fd;  	//操作目标，是需要监听的（套接字）描述符
  	uint32_t u32;
  	uint64_t u64;
} epoll_data_t;
struct epoll_event
{
  	uint32_t 	 events;	感兴趣的事件
	epoll_data_t data;		用户数据
} __EPOLL_PACKED;
---------------------------------------------------------------------------
 感兴趣的事件 events 可以是以下几个宏的集合：
EPOLLIN ：表示监听的文件描述符 可读（包括对端 socket 正常关闭）；
EPOLLOUT：表示监听的文件描述符 可写；
EPOLLPRI：表示监听的文件描述符 有紧急的数据 可读（这里应该表示有 带外数据 到来）；
EPOLLERR：表示监听的文件描述符 发生错误；
EPOLLHUP：表示监听的文件描述符 被挂断；
EPOLLONESHOT：事件只监听一次，当监听完这次事件之后，就会把这个 fd 从 epoll 的队列中删除。
		如果还需要继续监听这个 socket 的话，需要再次把这个 fd 加入到 epoll 队列里。
EPOLLET： 将 epoll 设为 边缘触发(Edge Triggered)模式，
		这是相对于 水平触发(Level Triggered)来说的。
-----------------------------
 关于 边缘触发(ET, Edge Triggered)、水平触发(LT, Level Triggered)两种工作模式：
 	边缘触发 ET 是缺省的工作方式，并且同时支持 block 和 no-block socket。
在这种做法中，内核告诉你一个文件描述符是否就绪了，然后你可以对这个就绪的 fd 进行 IO 操作。
如果你不作任何操作，内核还是会继续通知你的，所以，这种模式编程出错误可能性要小一点。
传统的 select/poll 都是这种模型的代表。
 	水平触发 LT 是高速工作方式，只支持 no-block socket。
在这种模式下，当描述符从未就绪变为就绪时，内核通过 epoll 告诉你。
然后它会假设你知道文件描述符已经就绪，并且不会再为那个文件描述符发送更多的就绪通知
（仅仅发送一次），直到你做了某些操作导致那个文件描述符不再为就绪状态了
（如你在发送、接收 或 接收请求，或者发送接收的数据少于一定量时导致了一个 EWOULDBLOCK 错误）。
 但是请注意，如果一直不对这个 fd 作 IO操作（从而导致它再次变成未就绪），
内核不会发送更多的通知（only once），不过在 TCP 协议中，
ET 模式的加速效用仍需要更多的 bench mark 确认。
-----------------------------
 ET 和 LT 的区别：
 	LT 事件不会丢弃，只要 有数据 可以让用户读，则不断的通知你。
 	LT 模式是 只要 有事件、有数据 没有处理 就会一直通知下去的。
	LT 模式下只要某个 fd 处于 readable/writable 状态，
 无论什么时候进行 epoll_wait() 都会返回该 fd；
 	ET 则只在事件 发生之时 通知。
 	ET 则只在高低电平变换时（即状态从1到0或者0到1）触发。
 	ET 模式下只有某个 fd 从 unreadable 变为 readable 或从 unwritable 变为 writable 时，
 epoll_wait() 才会返回该 fd。
 	ET 模式仅当 状态发生变化 的时候才获得通知，这里所谓的状态的变化
 并不包括缓冲区中还有未处理的数据，也就是说，如果要采用 ET 模式，
 需要一直 read/write 直到出错为止，很多人反映为什么采用 ET 模式只接收了一部分数据
 就再也得不到通知了，大多因为这样。
-----------------------------
在 epoll 的 ET 模式下，正确的读写方式为：
         读：只要可读，就一直读，直到返回 0，或者 errno = EAGAIN
         写：只要可写，就一直写，直到数据发送完，或者 errno = EAGAIN
---------------------------------------------------------------------------
 // 等待一个 epoll 实例上 I/O 事件发生：
int epoll_wait (int epoll_fd, struct epoll_event *event, int maxevents,
 				int timeout );
参数：
 epoll_fd：epoll_create()返回的描述符。
 event：数据结构，已触发的事件以及任何关联的用户数据。
 maxevents：要求的最大事件数。(监听数量的最大值)
 timeout：指定毫秒为单位的最大等待时间（ 0 会立即返回，-1 会永久阻塞）。
返回：成功返回 需要处理的 事件数目，返回 0 表示已超时，错误返回 -1，见 errno。
-----------------------------
int epoll_pwait(int epoll_fd, struct epoll_event *events, int maxevents,
 				int timeout, const sigset_t *sigmask );
 与 epoll_wait 等待相同，但是线程的信号掩码暂时并用作为参数提供的原子替换。
---------------------------------------------------------------------------*/