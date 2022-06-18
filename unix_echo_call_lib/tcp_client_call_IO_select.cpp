
#include "tcp_client_call.h"

#include <sys/select.h>

/** 所谓的IO复用，就是同时等待多个文件描述符就绪，以系统调用的形式提供。
 如果所有文件描述符都没有就绪的话，该系统调用阻塞，否则调用返回，允许用户进行后续的操作。
 常见的 IO复用 技术有 select, poll, epoll 以及 kqueue 等等。
 其中 epoll 为 Linux 独占，而 kqueue 则在许多 UNIX系统上存在，包括 macOS。
 kqueue()在易用性上可能比 select()和 poll()更好一些。
 epoll()效率要比单独使用 poll()函数高很多，非常适合监听大量的描述符，
 许多高性能的服务器都使用 epoll()。        **/

/** #define __FD_SETSIZE  1024   表示 select 最多同时监听 1024 个 fd **/

/********************* I/O口复用 *************************/

/* I/O 复用 */
void tcp_client_call_select(FILE *fp, const int &sock_fd)
{
	/* int fd = fileno(FILE *fp) 转化文件描述符 */
	char buf[MAXLINE];
	int	retval;
	int8_t stdineof=0;
/*******************************************************************/
	fd_set readset; // 鉴别事件的标志位集合
	int max_fdfp = std::max( fileno(fp)/* 转化文件描述符 */, sock_fd);
	std::cin.get();
	while (true)
	{
		FD_ZERO(&readset); // 初始化集合:所有位均关闭
		FD_SET(fileno(fp)/* 转化文件描述符 */, &readset); // 打开 file_fp 位
		FD_SET(sock_fd, &readset); // 打开 sock_fd 位
		/* select() 设置或者检查存放在数据结构 fd_set 中的标志位来鉴别事件 */
		retval=select(max_fdfp+1, &readset,
					  nullptr, nullptr, nullptr);
		if (retval < 0) {
			if (errno == EINTR) continue;
			err_sys(retval, __LINE__, "select()");
		}
/*******************************************************************/
		if (FD_ISSET(fileno(fp)/* 转化文件描述符 */, &readset)) /* input is readable */
		{
			retval = get_Line(fp, buf, sizeof(buf));
			err_sys(retval, __LINE__, "getLine()");
			retval = strlen(buf);
			if (buf[retval-1] == '\n') buf[retval-1] = '\0';
			if ( if_quit_0(buf, "quit") ) // end (FIN) !
			{
				/*	直接关闭在 socket FD 上打开的 全部 或 部分 连接。close()还要看链接数
				 int shutdown (int fd, int how);
				 确定要关闭的内容 how :
					 SHUT_RD   关闭 读
					 SHUT_WR   关闭 写
					 SHUT_RDWR 关闭 读写
   				Returns 0 on success, -1 for errors.  */
				retval = shutdown(sock_fd, SHUT_WR/*关闭 写*/); /* send FIN */
				err_sys(retval, __LINE__, "shutdown()");
				stdineof = 1;
				std::cout << "当前客户端：关闭发送功能，等待接收中" << std::endl;
				// 关闭 file_fp 位
				FD_CLR(fileno(fp)/* 转化文件描述符 */, &readset);
				continue;
			}
			if (stdineof != 1) {
				retval = Write(sock_fd, buf, strlen(buf));
				err_sys(retval, __LINE__, "Write()");
				std::cout << "发送 : " << buf << std::endl;
			}
		}
/*******************************************************************/
		if ( FD_ISSET(sock_fd, &readset) ) /* socket is readable */
		{
			retval = Read(sock_fd, buf, sizeof buf);
			err_sys(retval, __LINE__, "Read()");
			if (retval == 0) // TCP connection closed (TCP 断开)
			{
				retval = shutdown(sock_fd, SHUT_RDWR); /* send FIN */
				err_sys(retval, __LINE__, "shutdown()");
				FD_CLR(sock_fd, &readset);
				if (stdineof == 1) return; /* 正常终止*/
				std::cerr << "服务器断开" << std::endl;
				return;
			}
			if (buf[retval-1] == '\n') buf[retval-1] = '\0';
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
    如果一个服务器要解决多个服务或多个协定，个别要应用I/O复用。
---------------------------------------------------------------------------*/

/********************* I/O口复用 *************************/
/*  select()函数
原型如下：
1 int select(int fdsp1, fd_set *readfds, fd_set *writefds, fd_set *errorfds, const struct timeval *timeout);

各个参数含义如下：
    int fdsp1:最大描述符值 + 1
    fd_set *readfds:	对可读感兴趣的描述符集
    fd_set *writefds:	对可写感兴趣的描述符集
    fd_set *errorfds:	对出错感兴趣的描述符集

    struct timeval *timeout:	超时时间
    （注意：对于linux系统，此参数没有const限制，
    每次select调用完毕timeout的值都被修改为剩余时间，而unix系统则不会改变timeout值）

select函数会在发生以下情况时返回：
    readfds 集合中有描述符 可读
    writefds 集合中有描述符 可写
    errorfds 集合中有描述符 遇到错误条件
    指定的超时时间 timeout 到了

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
*/
