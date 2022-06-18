
#include "udp_client_call.h"


void call_alarm(int signo) /* alarm()设置警报 信号处理函数 */
{
	// 只是简单返回，实现中断 connect()
}
/* 使用 SIGALRM 为 recvfrom 设置超时 */
void udp_client_call_timeo_SIGALRM(const int & socket_fd,
								   const struct sockaddr * serv_addr,
								   const socklen_t & sizeof_addr,
								   const time_t &seconds )
{
	socklen_t len_size = sizeof_addr;
	int retval; // 函数返回值
	char sendline[MAXLINE];
	char recvline[MAXLINE];
//	fgetc(fp); // 从终端捕获 '\n'，丢弃

/** 设置 信号处理程序 */
//	__sighandler_t sigfunc = Signal(SIGALRM, call_alarm); // 返回旧信号处理程序
	Signal(SIGALRM, call_alarm);

	while (true)
	{
//		debug_line(__LINE__,(char *)__FUNCTION__);
		if ( if_quit_0(sendline, "quit") ) break;
		std::cout << "you input:\t";

		retval = get_Line(stdin, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "get_Line()");
		retval = strlen(sendline);
		if (sendline[retval-1] == '\n') sendline[retval-1] = '\0';
		std::cout << "sendto:\t\t" << sendline << std::endl;

		retval = sendto(socket_fd, sendline, strlen(sendline), MSG_WAITALL,
						(struct sockaddr *)serv_addr, sizeof_addr);
		err_sys(retval, __LINE__, "sendto()");

		alarm(3); /******** 设置警报 */
		memset(recvline, 0 , sizeof(recvline));
		retval = recvfrom(socket_fd, recvline, sizeof(recvline), MSG_WAITALL,
						  (struct sockaddr *)serv_addr, &len_size);
		if (retval < 0) {
			if (errno == EINTR) std::cerr << "socket timeout" << std::endl;
//				fprintf(stderr, "socket timeout\n");
			else err_sys(retval, __LINE__, "recvfrom()");
		}
		else if (retval == 0) break;
		else {
			alarm(0); /********* 取消任何当前计划的警报 */
			if (recvline[retval-1] == '\n') recvline[retval-1] = '\0';
			std::cout << "recvfrom:\t" << recvline << std::endl;
		}
	}
	alarm(0); /* 取消任何当前计划的警报 */
}


/* 使用 select 为 recvfrom 设置超时: 等待描述符变为可读 */
void udp_client_call_timeo_select(const int & socket_fd,
								  const struct sockaddr * serv_addr,
								  const socklen_t & sizeof_addr,
								  const time_t &seconds)
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

		retval = get_Line(stdin, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "get_Line()");
		retval = strlen(sendline);
		if (sendline[retval-1] == '\n') sendline[retval-1] = '\0';
		std::cout << "sendto:\t\t" << sendline << std::endl;

		retval = sendto(socket_fd, sendline, strlen(sendline), MSG_WAITALL,
						(struct sockaddr *)serv_addr, sizeof_addr);
		err_sys(retval, __LINE__, "sendto()");

		/* 设置超时: select() 等待描述符变为可读 */
		if (fd_read_select_timeo(socket_fd, seconds) == 0)
			std::cerr << "socket timeout" << std::endl;
			//				fprintf(stderr, "socket timeout\n");
		else {
			memset(recvline, 0 , sizeof(recvline));
			retval = recvfrom(socket_fd, recvline, sizeof(recvline),
							  MSG_WAITALL,
							  (struct sockaddr *)serv_addr, &len_size);
			err_sys(retval, __LINE__, "recvfrom()");
			if (retval == 0) {
//				debug_line(__LINE__,(char *)__FUNCTION__);
				/** 广播点，UDP fd 不需要关闭 **/
				close(socket_fd);
				break;
			};
			if (recvline[retval-1] == '\n') recvline[retval-1] = '\0';
			std::cout << "recvfrom:\t" << recvline << std::endl;
		}
	}
}


/* 使用 通用套接字选项 SO_RCVTIMEO（接收超时） 为 recvfrom 设置超时 */
void udp_client_call_timeo_SockOptions(const int & socket_fd,
									   const struct sockaddr * serv_addr,
									   const socklen_t & sizeof_addr,
									   const time_t &seconds)
{
	socklen_t len_size = sizeof_addr;
	int retval; // 函数返回值
	char sendline[MAXLINE];
	char recvline[MAXLINE];
//	fgetc(fp); // 从终端捕获 '\n'，丢弃

	/** ---- 允许端口的接收超时 ---- **/
	struct timeval tv{};
	tv.tv_sec = seconds;
	tv.tv_usec = 0;
	retval=setsockopt(socket_fd, SOL_SOCKET/*通用选项*/,
					  SO_RCVTIMEO/*接收超时*/, &tv, sizeof(tv) );
	err_sys(retval, __LINE__,"setsockopt()");
	while (true)
	{
//		debug_line(__LINE__,(char *)__FUNCTION__);
		if ( if_quit_0(sendline, "quit") ) break;
		std::cout << "you input:\t";

		retval = get_Line(stdin, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "get_Line()");
		retval = strlen(sendline);
		if (sendline[retval-1] == '\n') sendline[retval-1] = '\0';
		std::cout << "sendto:\t\t" << sendline << std::endl;

		retval = sendto(socket_fd, sendline, strlen(sendline), MSG_WAITALL,
						(struct sockaddr *)serv_addr, sizeof_addr);
		err_sys(retval, __LINE__, "sendto()");

		memset(recvline, 0 , sizeof(recvline));
		retval = recvfrom(socket_fd, recvline, sizeof(recvline), 0,
						  (struct sockaddr *)serv_addr, &len_size);
		if (retval <= 0) {
			if (retval == 0) {
//				debug_line(__LINE__,(char *)__FUNCTION__);
				/** 广播点，UDP fd 不需要关闭 **/
				break;
			}
			if (errno == EWOULDBLOCK /* 端口接收超时 */) {
				std::cerr << "socket timeout" << std::endl;
//				fprintf(stderr, "socket timeout\n");
				continue;
			}
			err_sys(retval, __LINE__, "recvfrom()");
		}
		if (recvline[retval-1] == '\n') recvline[retval-1] = '\0';
		std::cout << "recvfrom:\t" << recvline << std::endl;
	}
}

