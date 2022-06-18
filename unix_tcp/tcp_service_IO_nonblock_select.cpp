
#include "zcq_header.h"


#include <sys/select.h>

#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
//#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read

#include <iostream>
//using namespace std;


#if 0
int main(int argc, char **argv) {
	tcp_service_IO_nonblock_select();
}
#endif

/********************* I/O口复用 *************************/

void tcp_service_IO_nonblock_select()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
	int retval; // 函数返回值
	int listen_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	err_sys(listen_fd, __LINE__, "socket()");

	/* ---- 允许地址的立即重用，端口不占用 ---- */
	int option = 1;
	retval=setsockopt(listen_fd, SOL_SOCKET/* 通用选项 */,
					  SO_REUSEADDR/*地址立即重用*/, &option, sizeof(option) );
	err_sys(retval, __LINE__,"setsockopt()");

	/* ---- 允许端口的立即重用 ---- */
	retval=setsockopt(listen_fd, SOL_SOCKET/*通用选项*/,
					  SO_REUSEPORT/*端口立即重用*/, &option, sizeof(option) );
	err_sys(retval, __LINE__,"setsockopt()");

#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	ret_value=setsockopt(socket_listen_fd, SOL_SOCKET/*通用选项*/,
						 SO_RCVBUF/* 接收缓冲区 */,
						 &receiver_buf, sizeof(receiver_buf) );
	err_sys(ret_value, __LINE__,"setsockopt()");
#endif

	/* ---- 配置 ip 地址 端口 信息 ---- */
	struct sockaddr_in server_address{};
	bzero(&server_address, sizeof(server_address)); // 清 0
	server_address.sin_family		=AF_INET; // IPv4
	server_address.sin_addr.s_addr	=htonl(INADDR_ANY); // Internet address
	server_address.sin_port			=htons(PORT_service); // 端口号

	/* ---- bind () ---- */
	retval=bind(listen_fd, (struct sockaddr *)(&server_address), sizeof(server_address) );
	err_sys(retval, __LINE__, "bind()");

	/* ---- listen () ---- */
	retval=listen(listen_fd, MAX_listen/*最大连接数*/ );
	err_sys(retval, __LINE__, "listen()");

/************************** 设置 I/O 非阻塞 **************************/
	_debug_log_info("设置 I/O 非阻塞  listen_fd: %d", listen_fd)
	int flags = Fcntl(listen_fd, F_GETFL, 0); 			// 获取 I/O 状态
	Fcntl(listen_fd, F_SETFL, flags | O_NONBLOCK); 	// 设置 I/O 非阻塞
/*******************************************************************/
//	/* ---- Signal () ---- */
//	// SIGCHLD : 父进程 捕捉 子进程 的状态信号。
//	Signal(SIGCHLD);	/* must call waitpid() */
/********************* I/O口复用 *************************/
	int maxfd = listen_fd;			/* initialize */
	int client_max_index = -1;				/* index into client[] array */
	fd_set read_set; // 鉴别事件的标志位集合
	FD_ZERO(&read_set); // 初始化集合:所有位均关闭
	FD_SET(listen_fd, &read_set); // 设置 socket_listen_fd 位
	fd_set rs; // 鉴别事件的标志位集合
	int client[FD_SETSIZE];
	for (int & i : client)
		i = -1;			/* -1 indicates available entry */
//	for (int i = 0; i < FD_SETSIZE; ++i)
//		client[i] = -1;			/* -1 indicates available entry */
	int accept_fd; // 新(连接成功)套接字描述符
	int num_ready = 0; // 就绪描述符的数量
	char buf[MAXLINE];
	while (true)
	{
		rs = read_set;
		/* select() 设置或者检查存放在数据结构 fd_set 中的标志位来鉴别事件 */
		num_ready=select(maxfd+1, &rs, nullptr,  nullptr, nullptr);
		// 返回就绪描述符的数量
		err_sys(num_ready, __LINE__, "select()");
/*******************************************************************/
		/* new client connection */
		if (FD_ISSET(listen_fd, &rs)) {
			/* ---- accept () 非阻塞 select() ---- */
			accept_fd = Accept_nunblock_select(listen_fd, server_address.sin_family);
			if (accept_fd >= 0) {
				/* for select() */
				if (accept_fd > maxfd) maxfd = accept_fd;
				int i;
				for (i = 0; i < FD_SETSIZE; ++i) {
					if (client[i] < 0) {
						client[i] = accept_fd;   /* save descriptor */
						break;
					}
				}
				if (i >= FD_SETSIZE) {
					err_quit(-1, __LINE__, "too many clients !");
				}
				FD_SET(accept_fd, &read_set);    // 设置 socket_accept_fd 位
				/* max index in client[] array */
				if (i > client_max_index) client_max_index = i;
				std::cout << "client fd : " << accept_fd << std::endl;
			}
		}
/*******************************************************************/
		/* check all clients for data */
		for (int i = 0; i <= client_max_index; ++i) {
			if (client[i] < 0) continue;
			if (FD_ISSET(client[i], &rs)) /* socket is readable */
			{
				retval = Read(client[i], buf, sizeof(buf));
				err_sys(retval, __LINE__, "Read()");
				if (retval == 0) // TCP connection closed (TCP 断开)
				{
					std::cerr << "客户端[" << i << "]("
							  << client[i] << ")终止" << std::endl;
					/* ---- close () ---- */
					err_sys(close(client[i]), __LINE__, "close()");
					FD_CLR(client[i], &read_set); // 关闭 sockfd_t 位
					client[i] = -1;
				}
				else {
					if (retval > 1 && buf[retval-1] == '\n') buf[retval-1] = '\0';
					std::cout << "接收客户端: " << buf << std::endl;
					if (if_run("time", buf)) {
						bzero(buf, sizeof(buf)); // 清 0
/* Return the current time and put it in *TIMER if timer is not nullptr */
						time_t ticks = time(nullptr);
						/* 格式化串"%a.bs" :
		对于 a，表示如果字符串长度小于a,那么右对齐左边补空格，若大于a则原样输出不限制
		对于 b，表示如果字符串长度超过b，那么只取前b个    */
						snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
					}
					retval = Write(client[i], buf, strlen(buf));
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
（一）select()函数
原型如下：
1 int select(int fdsp1, fd_set *readfds, fd_set *writefds, fd_set *errorfds, const struct timeval *timeout);

各个参数含义如下：
    int fdsp1:最大描述符值 + 1
    fd_set *readfds:对可读感兴趣的描述符集
    fd_set *writefds:对可写感兴趣的描述符集
    fd_set *errorfds:对出错感兴趣的描述符集
    struct timeval *timeout:超时时间（注意：对于linux系统，此参数没有const限制，每次select调用完毕timeout的值都被修改为剩余时间，而unix系统则不会改变timeout值）

select函数会在发生以下情况时返回：
    readfds集合中有描述符可读
    writefds集合中有描述符可写
    errorfds集合中有描述符遇到错误条件
    指定的超时时间timeout到了

当select返回时，描述符集合将被修改以指示哪些个描述符正处于可读、可写或有错误状态。可以用FD_ISSET宏对描述符进行测试以找到状态变化的描述符。如果select因为超时而返回的话，所有的描述符集合都将被清空。
select函数返回状态发生变化的描述符总数。返回0意味着超时。失败则返回-1并设置errno。可能出现的错误有：EBADF（无效描述符）、EINTR（因终端而返回）、EINVAL（nfds或timeout取值错误）。

设置描述符集合通常用如下几个宏定义：
1 FD_ZERO(fd_set *fdset);               清除fdset中的所有位
2 FD_SET(int fd, fd_set *fdset);        在fd_set中打开fd的位
3 FD_CLR(int fd, fd_set *fdset);        在fd_set中关闭fd的位
4 int FD_ISSET(int fd, fd_set *fdset);  fdset中fd的作用是什么？

如:
1 fd_set read_set;
2 FD_ZERO(&read_set);                   初始化集合：所有位均关闭
3 FD_SET(1, &read_set);                 开启fd 1
4 FD_SET(4, &read_set);                 为fd 4开启位
5 FD_SET(5, &read_set);                 开启fd 5

当select返回的时候，rset位都将被置0，除了那些有变化的fd位。

当发生如下情况时认为是可读的：
1.socket的receive buffer中的字节数大于socket的receive buffer的low-water mark属性值。
 （low-water mark值类似于分水岭，当receive buffer中的字节数小于low-water mark值的时候，认为socket还不可读，只有当receive buffer中的字节数达到一定量的时候才认为socket可读）
2.连接半关闭（读关闭，即收到对端发来的FIN包）
3.发生变化的描述符是被动套接字，而连接的三路握手完成的数量大于0，即有新的TCP连接建立
4.描述符发生错误，如果调用read系统调用读套接字的话会返回-1。

当发生如下情况时认为是可写的：
1.socket的send buffer中的字节数大于socket的send buffer的low-water mark属性值以及socket已经连接或者不需要连接（如UDP）。
2.写半连接关闭，调用write函数将产生SIGPIPE
3.描述符发生错误，如果调用write系统调用写套接字的话会返回-1。

注意：
1.select默认能处理的描述符数量是有上限的，为FD_SETSIZE的大小。
2.对于timeout参数，如果置为NULL，则表示wait forever；若timeout->tv_sec = timeout->tv_usec = 0，则表示do not wait at all；否则指定等待时间。
3.如果使用select处理多个套接字，那么需要使用一个数组（也可以是其他结构）来记录各个描述符的状态。而使用poll则不需要，下面看poll函数。
---------------------------------------------------------------------------
 */