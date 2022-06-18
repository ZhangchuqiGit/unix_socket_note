
#include "tcp_client_call.h"

#include <climits>		/* for OPEN_MAX */
#include <poll.h>

/** 所谓的IO复用，就是同时等待多个文件描述符就绪，以系统调用的形式提供。
 如果所有文件描述符都没有就绪的话，该系统调用阻塞，否则调用返回，允许用户进行后续的操作。
 常见的 IO复用 技术有 select, poll, epoll 以及 kqueue 等等。
 其中 epoll 为 Linux 独占，而 kqueue 则在许多 UNIX系统上存在，包括 macOS。
 kqueue()在易用性上可能比 select()和 poll()更好一些。
 epoll()效率要比单独使用 poll()函数高很多，非常适合监听大量的描述符，
 许多高性能的服务器都使用 epoll()。        **/


/********************* I/O口复用 *************************/

/* I/O 复用 */
void tcp_client_call_poll(FILE *fp, const int &sock_fd)
{
	/* int fd = fileno(FILE *fp) 转化文件描述符 */
	char buf[MAXLINE];
	int retval; // 函数返回值
	int8_t stdineof=0;
/*******************************************************************/
	struct pollfd client[2];
/* 设置 有普通数据或者优先数据可读，设置 POLLOUT | POLLWRBAND 等价 select()可写事件 */
	client[0].fd = fileno(fp)/* 转化文件描述符 */;
	client[0].events =  POLLIN | POLLPRI ;
/* 设置 有普通数据或者优先数据可读，设置 POLLOUT | POLLWRBAND 等价 select()可写事件 */
	client[1].fd = sock_fd;
	client[1].events = POLLIN | POLLPRI | POLLOUT | POLLWRBAND;
	while (true)
	{
		/* poll() 设置或者检查存放在数据结构 pollfd 中的标志位来鉴别事件 */
		retval = poll(client, 2, INFTIM);
		// 返回就绪描述符的数量
		err_sys(retval, __FILE__, __func__, __LINE__, "poll()");
/*******************************************************************/
		/* input is readable */
		if (client[0].revents & (POLLIN | POLLPRI | POLLERR) )
		{
			debug_line(__LINE__, (char *)__FUNCTION__);
			retval = get_Line(fp, buf, sizeof(buf));
			err_sys(retval, __LINE__, "getLine()");
			if (if_quit_echo(buf) ) // end (FIN) !
			{
				debug_line(__LINE__, (char *)__FUNCTION__);
				/*	直接关闭在 socket FD 上打开的 全部 或 部分 连接。close()还要看链接数
					 int shutdown (int fd, int how);
					 确定要关闭的内容 how :
						 SHUT_RD   关闭 读
						 SHUT_WR   关闭 写
						 SHUT_RDWR 关闭 读写
					   Returns 0 on success, -1 for errors.  */
				// send FIN
				retval = shutdown(sock_fd, SHUT_WR/*关闭写*/);
				err_sys(retval, __LINE__, "shutdown()");
				client[0].fd = -1;
				client[1].events = POLLIN | POLLPRI;//有普通或优先或紧急数据可读
				std::cout << "客户端：关闭发送功能，等待接收中" << std::endl;
				stdineof = 1; // 当前客户端：关闭发送功能，等待接收中
				continue;
			}
			if (stdineof != 1) { /* 当前客户端 发送功能 */
				debug_line(__LINE__, (char *)__FUNCTION__);
				retval = Write(sock_fd, buf, strlen(buf));
				err_sys(retval, __LINE__, "Write()");
//				std::cout << "发送 : " << buf ;
				std::cout << "发送 : " << buf << std::endl;
			}
		}
/*******************************************************************/
		/* socket is readable */
		if (client[1].revents & (POLLIN|POLLPRI|POLLERR) )
		{
			debug_line(__LINE__, (char *)__FUNCTION__);
			retval = Read(sock_fd, buf, sizeof buf);
			err_sys(retval, __LINE__, "Read()");
			if (retval == 0) // TCP connection closed (TCP 断开)
			{
				debug_line(__LINE__, (char *)__FUNCTION__);
				/* ---- close () ---- */
				retval=close(sock_fd);
				err_sys(retval, __LINE__, "客户端 close()");
				debug_line(__LINE__, (char *)__FUNCTION__);
				client[1].fd = -1;
				if (stdineof == 1) std::cerr << "服务器断开" << std::endl;
				else std::cerr << "服务器异常" << std::endl;
				return;
			}
			else
//			std::cout << "接收 : " << buf;
			std::cout << "接收 : " << buf << std::endl;
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
/* 	poll()函数
原型如下：
1 int poll(struct pollfd *fdarray, unsigned long nfds, int timeout);

各参数含义如下：
struct pollfd *fdarray:		一个结构体，用来保存各个描述符的相关状态。
unsigned long nfds:			fdarray 数组的大小，即里面包含有效成员的数量。
int timeout:				设定的超时时间。（以毫秒为单位）
---------------------------------------------------------------------------
参数 timeout 的设置如下：
	INFTIM (-1):     wait forever
	0   :            return immediately, do not block
	>0     :         wait specified number of milliseconds
---------------------------------------------------------------------------
poll()函数 返回值 ：
-1：有错误产生
0：超时时间到，而且没有描述符有状态变化
>0：有状态变化的描述符个数
---------------------------------------------------------------------------
着重讲 fdarray 数组，因为这是它和select()函数主要的不同的地方：
pollfd 的结构如下：
	struct pollfd {
	    int fd;            要检查的描述符
	    short events;      fd 感兴趣 的事件 / 监听的事件
	    short revents;     fd 发生 的事件 / poll()返回时描述符的返回状态
	 };
---------------------------------------------------------------------------
其实poll()和select()函数要处理的问题是相同的，只不过是不同组织在几乎相同时刻同时推出的，因此才同时保留了下来。select()函数把可读描述符、可写描述符、错误描述符分在了三个集合里，这三个集合都是用bit位来标记一个描述符，一旦有若干个描述符状态发生变化，那么它将被置位，而其他没有发生变化的描述符的bit位将被clear，也就是说select()的readset、writeset、errorset是一个value-result类型，通过它们传值，而也通过它们返回结果。这样的一个坏处是每次重新select 的时候对集合必须重新赋值。而poll()函数则与select()采用的方式不同，它通过一个结构数组保存各个描述符的状态，每个结构体第一项fd代表描述符，第二项代表要监听的事件，也就是感兴趣的事件，而第三项代表poll()返回时描述符的返回状态。合法状态如下：
---------------------------------------------------------------------------
监听事件	返回状态			合法状态
events	revents			POLLIN:        	有普通数据或者优先数据可读
events	revents			POLLRDNORM:    	有普通数据可读
events	revents			POLLRDBAND:    	有优先数据可读
events	revents			POLLPRI:       	有紧急数据可读

events	revents			POLLOUT:       	有普通数据或者优先数据可写，不会导致阻塞
events	revents			POLLWRNORM:    	有普通数据可写，不会导致阻塞
events	revents			POLLWRBAND:    	有优先数据可写，不会导致阻塞

 						POLLMSG:		SIGPOLL / SIGIO 消息可用。默认会导致进程退出。
									文件描述符准备就绪, 可以开始进行 I/O 操作。

		revents			POLLERR:       	有 错误 事件发生
		revents			POLLHUP:       	有 挂起 事件发生
		revents			POLLNVAL:      	指定的(文件)描述符 非法
---------------------------------------------------------------------------
POLLIN | POLLPRI 等价 select() 的可读事件；
POLLOUT | POLLWRBAND 等价 select() 的可写事件；
POLLIN 等价 POLLRDNORM | POLLRDBAND。
如果你对一个描述符的可读事件和可写事件以及错误等事件均感兴趣那么你应该都进行相应的设置。
 */