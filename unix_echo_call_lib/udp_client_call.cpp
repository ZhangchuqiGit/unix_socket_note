
#include "udp_client_call.h"


void udp_client_call(const int & socket_fd,
					 const struct sockaddr *cli_addr,
					 const socklen_t & sizeof_addr,
					 const time_t &seconds )
{
	std::cout << "------ udp_client_call() : 'quit' is exit : " << std::endl;
#if (UDP_Call==0 )
	/* 传递互动信息 */ 	/* 传递文本串，计算两数之和 */
	udp_client_call_msg(socket_fd, cli_addr, sizeof_addr );
#elif UDP_Call==1
	/* 使用 SIGALRM 为 recvfrom 设置超时 */
	udp_client_call_timeo_SIGALRM(socket_fd, cli_addr, sizeof_addr, seconds);
#elif UDP_Call==2
	/* 使用 select 为 recvfrom 设置超时: 等待描述符变为可读 */
	udp_client_call_timeo_select(socket_fd, cli_addr, sizeof_addr, seconds);
#elif UDP_Call==3
	/* 使用 通用套接字选项 SO_RCVTIMEO（接收超时） 为 recvfrom 设置超时 */
	udp_client_call_timeo_SockOptions(socket_fd, cli_addr, sizeof_addr, seconds);
#elif UDP_Call==4
/* 	在信号处理函数中进行非局部转移
 	增加 可靠性： 	1、超时和重传：处理丢失数据；
 				 	2、序列号：验证应答 是否匹配。    */
	udp_client_call_rtt(socket_fd, cli_addr, sizeof_addr, seconds);
#endif
	std::cout << "------ udp_client_call() : exit ! " << std::endl;
}

void udp_client_call_r(const int & socket_fd,
					   const struct sockaddr *cli_addr,
					   const socklen_t & sizeof_addr,
					   const time_t &seconds)
{
	switch (cli_addr->sa_family) {
		case AF_INET:
		case AF_INET6:
			udp_client_call(socket_fd, (struct sockaddr *) &cli_addr,
							sizeof_addr, seconds);
			break;
//		case AF_LOCAL: {
		case AF_UNIX: {
			struct sockaddr_un sau{}; /* address of an AF_LOCAL(AF_UNIX) socket. */
			size_t len = sizeof(sau);
			bzero(&sau, len);
			memmove(&sau, &cli_addr, sizeof_addr);
			udp_client_call(socket_fd, (struct sockaddr *) &sau, len, seconds);
		}
			break;
		default:
			break;
	}
}


/* 传递互动信息 */
void udp_client_call_msg(const int & socket_fd,
						 const struct sockaddr * serv_addr,
						 const socklen_t & sizeof_addr)
{
	socklen_t len_size = sizeof_addr;
	int retval; // 函数返回值
	char sendline[MAXLINE];
	char recvline[MAXLINE];
//	fgetc(fp); // 从终端捕获 '\n'，丢弃
	while (true)
	{
//		debug_line(__LINE__,(char *)__FUNCTION__);
		if ( if_quit_0(sendline, "quit") ) break;
		std::cout << "you input:\t";

		retval = Read(STDIN_FILENO, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "get_Line()");
		retval = strlen(sendline);
		if (sendline[retval - 1] == '\n') sendline[retval - 1] = '\0';
		std::cout << "sendto:\t\t" << sendline << std::endl;

//		retval = sendto(socket_fd, sendline, strlen(sendline), MSG_WAITALL,
		retval = sendto(socket_fd, sendline, strlen(sendline), 0,
						(struct sockaddr *)serv_addr, sizeof_addr);
		err_sys(retval, __LINE__, "sendto()");

		memset(recvline, 0 , sizeof(recvline));
//		retval = recvfrom(socket_fd, recvline, sizeof(recvline), MSG_WAITALL,
		retval = recvfrom(socket_fd, recvline, sizeof(recvline), 0,
						  (struct sockaddr *)serv_addr, &len_size);
		if (retval < 0) {
			if (errno == EINTR) std::cerr << "Interrupted system call" << std::endl;
				//				fprintf(stderr, "socket timeout\n");
			else err_sys(retval, __LINE__, "recvfrom()");
		}
		else if (retval == 0) break;
		else {
			if (recvline[retval-1] == '\n') recvline[retval-1] = '\0';
			std::cout << "recvfrom:\t" << recvline << std::endl;
		}
	}
}
