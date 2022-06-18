
#include "zcq_header.h"

#include <climits>		/* for OPEN_MAX */
#include <poll.h>

#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
//#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read
#include <ctime>

#include <iostream>
//using namespace std;


#if 0
int main(int argc, char **argv) {
	tcp_service_IO_poll_host_serv_addrinfo();
}
#endif

/********************* I/O口复用 *************************/

void tcp_service_IO_poll_addrinfo()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
	int retval; // 函数返回值
	int listen_fd; // 套接字描述符

	listen_fd = tcp_socket_bind_listen(nullptr, PORT_service_STR );
//	listen_fd = tcp_socket_bind_listen(nullptr, (char *)PORT_service_STR,
//									   nullptr, (int)AF_INET6);

/*******************************************************************/
//	/* ---- Signal () ---- */
//	// SIGCHLD : 父进程 捕捉 子进程 的状态信号。
//	Signal(SIGCHLD);	/* must call waitpid() */
/********************* I/O口复用 *************************/
	int client_max_index = 0;		/* max index into client[] array */
//	int8_t stdineof[FD_SETSIZE];
	struct pollfd client[FD_SETSIZE];
/* 设置 POLLIN 有普通数据或者优先数据可读，
 * 设置 POLLOUT | POLLWRBAND 等价 select()可写事件 */
	client[0].fd = listen_fd;
	client[0].events = POLLIN | POLLPRI | POLLOUT | POLLWRBAND;
	for (int i = 1; i < FD_SETSIZE; ++i){
		client[i].fd = -1; /* -1 indicates available entry */
//		stdineof[i]=0;
	}
	int num_ready = 0; // 就绪描述符的数量
/*******************************************************************/
	int accept_fd; // 新(连接成功)套接字描述符
	char buf[MAXLINE];
	while (true)
	{
		/* poll() 设置或者检查存放在数据结构 pollfd 中的标志位来鉴别事件 */
		num_ready = poll(client, client_max_index+1, INFTIM);
		// 返回就绪描述符的数量
		err_sys(num_ready, __LINE__, "poll()");
/*******************************************************************/
		/* new client connection */
		if (client[0].revents & (POLLIN|POLLPRI)/* 有普通或优先或紧急数据可读 */) {
			/* ---- accept () ---- */
			accept_fd = Accept(listen_fd, AF_UNSPEC);
			int i;
			for (i=1; i < FD_SETSIZE; ++i) {
				if (client[i].fd < 0) {
					client[i].fd = accept_fd;    /* save descriptor */
					break;
				}
			}
			if (i >= FD_SETSIZE ) {
				err_quit(-1, __LINE__, "too many clients !");
			}
			client[i].events = POLLIN | POLLPRI | POLLOUT | POLLWRBAND;
			/* max index in client[] array */
			if (i > client_max_index) client_max_index = i;
			std::cout << "client fd : "
					  << accept_fd << std::endl;
			debug_line(__LINE__, (char *)__FUNCTION__);
		}
/*******************************************************************/
		/* check all clients for data */
		for (int i = 1; i <= client_max_index; ++i) {
			if (client[i].fd < 0) continue;
			/* socket is readable */
			if (client[i].revents & (POLLIN|POLLPRI|POLLERR))
			{
				debug_line(__LINE__, (char *)__FUNCTION__);
				retval = Read(client[i].fd, buf, sizeof(buf));
				err_sys(retval, __LINE__, "Read()");
				if (retval == 0) // TCP connection closed (TCP 断开)
				{
					/* ---- close () ---- */
					retval = close(client[i].fd);
					err_sys(retval, __LINE__, "close()");
					std::cerr << "客户端[" << i << "]("
							  << client[i].fd << ")终止" << std::endl;
					client[i].fd = -1;
				}
				else {
					if (buf[retval-1] == '\n') buf[retval-1] = '\0';
					std::cout << "接收客户端 : " << buf << std::endl;
					if ( if_run("time", buf) ) {
						bzero(buf, sizeof(buf)); // 清 0
/* Return the current time and put it in *TIMER if timer is not nullptr */
						time_t ticks = time(nullptr);
						/* 格式化串"%a.bs" :
		对于 a，表示如果字符串长度小于a,那么右对齐左边补空格，若大于a则原样输出不限制
		对于 b，表示如果字符串长度超过b，那么只取前b个    */
						snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks) );
					}
					retval = Write(client[i].fd, buf, strlen(buf));
					err_sys(retval, __LINE__, "Write()");
				}
			}
			/* no more readable descriptors */
			if (--num_ready <= 0) break; // 就绪描述符的数量
		}
/*******************************************************************/
	}
/* ---- close () ---- */
	retval=close(listen_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}


/********************* I/O口复用 *************************/
/* I/O口复用
---------------------------------------------------------------------------
linux c++ select/poll/epoll 的个人见解
Select
	通过设置或者检查存放在数据结构 fd_set 中的标志位的来鉴别事件。Fd_set是一个输入输出参数，用户通过设置(FD_SET)相应的标志位标识关注的描述符，经内核拷贝到内核空间；内核根据输入fd_set 扫描对应的描述符，新建另一fd_set标识活跃的描述符，扫描完毕后将含有活跃描述符的fd_set 拷贝到用户空间。用户检查(FD_ISSET)内核输出的fd_set 确定活跃的描述符。（进程的fd的分配都是从3开始的，0、1、2已经用作标准输入，输出和错误？而fd_set的每一位对应一个fd。）
Poll
	poll与select相似，poll用结构体标识一个fd，将关注的事件合并用events位标识。用户将关注的fd对应的结构体添加到数组中，并从用户空间拷贝到内核空间。内核轮询数组标识的fd，如有就绪fd这设置对应结构体的revents标志位，并继续遍历，最后将结构体数组拷贝到用户空间，用户检查revents标识并进行处理。如遍历完仍未有fd就绪，则挂起当前进程，直到再次被调度，被唤醒后再次遍历结构数组。采用“水平触发”的侦测方式，如检测到fd就绪后，没处理，那么下次epoll时会再次报告该fd。
Epoll
	Epoll在用户空间和内核使用mmap方式传递数据（epoll_ctl函数添加关注的fd），避免了复制开销。Epoll使用就绪通知的模式，内核将就绪的fd添加到就绪队列中并返回，epoll_wait收到的都是就绪的fd。支持两种侦测方式，“边沿触发”和“边沿触发”，当采用“水平触发”（默认）的侦测方式，有poll相同的问题。
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
---------------------------------------------------------------------------
（二）poll()函数
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
events	revents			POLLIN:        有普通数据或者优先数据可读
events	revents			POLLRDNORM:    有普通数据可读
events	revents			POLLRDBAND:    有优先数据可读
events	revents			POLLPRI:       有紧急数据可读

events	revents			POLLOUT:       有普通数据可写
events	revents			POLLWRNORM:    有普通数据可写
events	revents			POLLWRBAND:    有紧急数据可写

		revents			POLLERR:       有 错误 发生
		revents			POLLHUP:       有描述符 挂起 事件发生
		revents			POLLNVAL:      描述符 非法
---------------------------------------------------------------------------
POLLIN | POLLPRI 等价与 select() 的可读事件；
POLLOUT | POLLWRBAND 等价与 select() 的可写事件；
POLLIN 等价与 POLLRDNORM | POLLRDBAND，而 POLLOUT 等价于 POLLWRBAND。
如果你对一个描述符的可读事件和可写事件以及错误等事件均感兴趣那么你应该都进行相应的设置。
 */