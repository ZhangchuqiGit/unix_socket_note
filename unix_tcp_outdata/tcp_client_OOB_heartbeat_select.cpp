
#include "zcq_header.h"

void tcp_client_OOB_heartbeat_pselect()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int sock_fd; // 套接字描述符
	int retval; // 函数返回值

	/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
	/* ---- 配置 ---- */
	/* ---- connect () ---- */
	sock_fd = tcp_socket_connect(HOST_Service_IPv4,	PORT_service_STR,
								 AF_INET, SOCK_STREAM );

	/** 心搏机制：周期信号轮询对端，无响应即不存活 */
	heartbeat_client(sock_fd, 1, 5); // 带外数据 引发 SIGURG 信号

	char sendline[MAXLINE], recvline[MAXLINE];
	fd_set read_set, rset;
	FD_ZERO(&read_set); // 初始化集合:所有位均关闭
	FD_SET(sock_fd, &read_set);
	FD_SET(STDIN_FILENO, &read_set);
	uint8_t stdineof = 0;
	while (true) {
		rset = read_set;

		/* 设置超时: pselect() 等待描述符变为可读
			   信号掩码: 若收到该信号，中断返回 0；否则返回 -1(error)  */
		retval = select(sock_fd + 1, &rset,
						nullptr, nullptr, nullptr);
		if (retval < 0) {
			if (errno == EINTR) continue; // Interrupted system call
			err_sys(retval, __LINE__, "fd_read_pselect_timeo()");
		}
		if (FD_ISSET(sock_fd, &rset)) {	/* socket is readable */
			retval = Read(sock_fd, recvline, sizeof(recvline)-1);
			if (retval > 0) {
				recvline[retval] = 0;
				std::cout << "Data: " << recvline << std::endl;
			} else if (retval == 0) {
				if (stdineof == 1) break; 		// normal termination
				err_quit(-1, __LINE__, "server terminated prematurely");
			}
		}
		if (FD_ISSET(STDIN_FILENO, &rset)) {  /* input is readable */
			retval = Read(STDIN_FILENO, sendline, sizeof(sendline)-1);
			if (retval == 0) {
				alarm(0);			/* turn off heartbeat */
				stdineof = 1;
				FD_CLR(STDIN_FILENO, &read_set);
				shutdown(sock_fd, SHUT_WR);	/* send FIN */
				continue;
			}
			Write(sock_fd, sendline, strlen(sendline));
		}
	}
	/* ---- close () ---- */
	err_sys(close(sock_fd), __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}

/* 数据分为两种，一种是带内数据，一种是带外数据。带内数据就是我们平常传输或者说是口头叫的数据。
 许多的传输层都具有带外数据（也称为 经加速数据 ）的概念，希望迅速的通知给对端。
 这里的迅速是指这种通知应该在已经排队了的带内数据之前发送。也就是说，带外数据拥有更高的优先级。
 带外数据不要求再启动一个连接进行传输，而是使用已有的连接进行传输。
 UDP没有实现带外数据（是个极端哦～）。
 TCP中telnet,rlogin,ftp等应用（除了这样的远程非活跃应用之外，几乎很少有使用到带外数据的地方）
--------------------------------------------------------------------------------
 TCP包首部：
	紧急字段URG：	当URG=1时，告诉系统此报文段中有紧急数据，应尽快传送。
	紧急指针：	指出本报文段中紧急数据的后一个字节的序号，即指出带外数据字节在正常字节流中的位置。
--------------------------------------------------------------------------------
1 发送端 ：
    带外字节会被标记为 OOB
    紧急数据是插在正常数据流中进行传输的。
    一个紧急指针 只指向 一个字节的 带外数据的 后一个字节的位置。
    		要发送数据 0，1，2，如果只发送一个字节的带外数据 X，
    	那么发送缓冲区就是（0，1，2，X，3），紧急指针 指向 X 的后面，即 3 的位置。
    		发送多个字节的带外数据（X，Y，Z），那么发送缓冲区就是（0，1，2，X，Y，Z），
		紧急指针指向 Z 的后面。
    		假如由于发送窗口的关系，导致该发送缓冲区中的数据（0，1，2，3，X，4，5，6）分多次发送。
    	如：发送窗口是 4，带外数据是 X，分为两个包发送，
    	第一个包传送4个字节（0，1，2，3），接收端记下接受的字节数并且发现紧急指针指向的紧急数据
    	下一个包传送3个字节（4，5，6）。
    即使数据的流动会因为TCP的流量控制而停止，紧急通知却总是 无障碍的发送 到对端TCP。
--------------------------------------------------------------------------------
2 接受端 ：
	一旦有一个新的紧急指针到达，不论由紧急指针指向的实际数据字节是否已经到达接受端，
 以下两个动作都会发生 ：
    1) 当接受到一个设置了URG标志的分节时，接受端检查紧急指针，确定它是否指向新的带外数据，
    比如：前面发送了两个包，只有第一个才会通知接受进程有新的带外数据到达。
    2) 当有新的紧急指针到达时，接受进程被通知到。首先，内核会给接受套接字的属主进程发送SIGURG信号，
    前提是接受进程调用了 fcntl()或者 ioctl()为这个套接字建立了属主，
    并且该属主进程为该信号建立了信号处理函数。
    只有一个OOB标记，如果新的OOB字节在旧的OOB字节之前到达，旧的OOB字节就会被丢弃。
    3) 当由紧急指针指向的实际数据字节到达接受端TCP时，数据字节会有两个存储地区：
    一个是和普通数据(带内区域)一样的 在线留存，
    另外一个是 独立的单字节 带外缓冲区，接受进程从这个单字节带外缓冲区读入数据的唯一方法是指定MSG_OOB 调用 recv()，recvfrom()，recvmsg()。
    如果放在和普通数据一起的带内区域，接受进程就得通过检查该连接的 带外标记OOB 来获悉
    何时访问带这个数据字节。
    默认情况下将带外数据字节放入独立的单字节带外缓冲区内。通过套接字选项 SO_OOBINLINE 在线留存。
--------------------------------------------------------------------------------
3 会发生的一些错误：
 		如果接受进程请求读入数据（通过 MSG_OOB 标志），但是对端并没有发送任何带外数据，
	读入操作将返回EINVAL。
    	在接受进程已被告知对端发送了一个带外字节（SIGURG 或 select(),poll()等）的前提下，
   	如果接受进程试图读入该字节，但是该字节尚未到达，读入操作返回 EWOULDBLOCK。
    接受进程此时做的就是从缓冲区中读入数据，腾出空间，以允许对端TCP发送出那个带外字节。
    	如果接受进程试图多次读入同一个带外字节，读入操作返回 EINVAL。
    	如果开启了套接字选项 SO_OOBINLINE 在线留存，
    接受进程如果还是通过 MSG_OOB 读入带外数据，读入操作将返回 EINVAL 。
--------------------------------------------------------------------------------
总结：带外数据概念实际上时向接收端传送三个不同的信息：
（1）发送端进入紧急模式这个事实。
 接收进程得以通知这个事实的手段不外乎 SIGURG信号 或 select(),poll()等调用。
 本通知在发送进程发送带外字节后由发送端TCP立即发送，
 即使往接收端的任何数据发送因流量控制而停止了，TCP仍然发送带外字节通知。
 本通知可能导致接收端进入某种特殊处理模式，以处理接收的任何后继数据。
（2）带外字节的位置，也就是它相对于来自发送端的其余数据的发送位置：带外标记。
（3）带外字节的实际值。既然TCP是一个不解释应用进程所发送数据的字节流协议，带外字节就可以是任何8位值。

 对于TCP的紧急模式，可以认为 URG标志是通知（信息1），紧急指针是带外标记（信息2），数据字节是其本身（信息3）。
 与这个带外数据概念相关的问题有：
	每个连接只有一个TCP紧急指针；
	每个连接只有一个带外标记；
	每个连接只有一个单字节的带外缓冲区（该缓冲区只有在数据非在线读入时才需考虑）。
 		如果带外数据时在线读入的，那么当心的带外数据到达时，先前的带外字节字节并未丢失，
 		不过他们的标记却因此被新的标记取代而丢失了。

	带外数据的一个常见的用途体现在rlogin程序中。
 当客户中断运行在服务器主机上的程序时，服务器需要告知客户丢弃所有已在服务器排队的输出，
 因为已经排队等着从服务器发送到客户的输出最多有一个窗口的大小。服务器向客户发送一个特殊字节，
 告知后者清刷所有这些输出（在客户看来是输入），这个特殊字节就作为带外数据发送。
 客户收到由带外数据引发的SIGURG信号后，就从套接字中读入直到碰到带外数据发送。
 客户收到由带外数据引发的SIGURG信号后，就从套接字中读入直到碰到带外标记，并丢弃到标记之前的所有数据。
 这种情形下即使服务器相继地快速发送多个带外字节，客户也不受影响，
 因为客户只是读到最后一个标记为止，并丢弃所有读入的数据。        */
