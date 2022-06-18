//
// Created by zcq on 2021/3/31.
//

#include "tcp_client_socket_connect_addrinfo.h"


int Connect(const int &sock_fd, const struct sockaddr *sa_ptr)
{
	socklen_t len = 0;
	switch (sa_ptr->sa_family)
	{
		case AF_INET:
			len = sizeof(struct sockaddr_in);
			break;
		case AF_INET6:
			len = sizeof(struct sockaddr_in6);
			break;
		case AF_UNIX:
			len = sizeof(struct sockaddr_un);
			break;
		default:
			err_sys(-1, __LINE__, "%s()", __func__);
			break;
	}
	return connect(sock_fd, sa_ptr, len);
}

/* 使用 SIGALRM 为 connect() 设置超时 */
static void connect_alarm(int signo) /* 信号处理函数 */
{
	// 只是简单返回，实现中断 connect()
}

/* 使用 SIGALRM 为 connect() 设置超时 */
int connect_timeo(const int &sock_fd, const struct sockaddr *sa_ptr,
				  const int &second)
{
	int retval; // 函数返回值

	/** 设置 信号处理程序 */
	__sighandler_t sigfunc = Signal(SIGALRM, connect_alarm); // 返回旧信号处理程序

	/* unsigned int alarm (unsigned int seconds); 在几秒钟内，该过程将得到 SIGALRM。
	 * 如果 seconds 为零，则取消任何当前计划的警报。
	 * 如果 seconds 非零，首次设置警报；再次调用时，则返回即将警报的剩余秒数。
	 * 没有返回值指示错误，但是可以将”errno”设置为 0 并在调用“alarm”后检查其值。 */
	errno = 0;
	if (alarm(second) != 0) err_msg(-1, __LINE__, "alarm()");
	if ((retval = Connect(sock_fd, sa_ptr)) < 0) {
		close(sock_fd);
		if (errno == EINTR) errno = ETIMEDOUT; // Connection timed out
	}
	alarm(0); /* 取消任何当前计划的警报 */

	/** 恢复 先前的信号处理程序 */
	Signal(SIGALRM, sigfunc);

	return(retval);
}

/* blocking connect() */
int tcp_socket_connect(std::string_view host, std::string_view service,
					   const int &family, const int &socktype,
					   const int &protocol, const int &flags)
{
	int sock_fd; // 套接字描述符
	/* ---- 配置 ---- */
	struct addrinfo *result = Get_addrinfo( host, service, family,
											socktype, protocol, flags);
	struct addrinfo	*saveTOfree = result;

	do {
		/* ---- socket () ---- */
//		sock_fd = socket(family, socktype, protocol);
		sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (sock_fd < 0) continue;

		/* ---- connect () ---- */
		if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == 0) break;

		/* ---- close () ---- */
		err_sys( close(sock_fd), __LINE__, "close()");

	}	/* ai_next 指向下一个 addrinfo 结构体的指针 */
	while ( (result = result->ai_next) != nullptr );

	if (result == nullptr)	/* errno set from final connect() */
		err_sys(-1, __LINE__, "%s():\thost:%s;\tservice:%s",
				__func__ , host.data(), service.data() );
#if 0
	std::cout << "位掩码 \t \tai_flags: \t" 		<< result->ai_flags 	<< std::endl
			  << "协议族 \t \tai_family: \t" 	<< result->ai_family 	<< std::endl
			  << "socket类型 \tai_socktype: \t"	<< result->ai_socktype 	<< std::endl
			  << "协议类型 \tai_protocol: \t" 	<< result->ai_protocol 	<< std::endl;
	if (result->ai_canonname != nullptr)
		std::cout << "主机名称 \tai_canonname: \t"
				  << (char *)result->ai_canonname << std::endl;
	/* 获取 本地/对方 与某个套接字 关联的 IP地址 和 端口号。 */
	getIP_addr_port(sock_fd, result->ai_family);
	/* gethostbyname2_r()：用 域名 或者 主机名 获取 主机 IP 信息。 */
	err_sys( Get_host_byname2_r(result->ai_canonname, result->ai_family),
			 __LINE__, "Get_host_byname2_r()");
#endif
	freeaddrinfo(saveTOfree); // 释放结构体链表指向的任何动态存储空间
	return(sock_fd);
}

/* 使用 非阻塞 I/O 复用 select() 为 connect() 设置超时 */
int connect_nonb_timeo(const int &sock_fd, const struct sockaddr *sa_ptr,
					   const socklen_t &sa_len, const int &second)
{
/************************** 设置 I/O 非阻塞 **************************/
	_debug_log_info("设置 I/O 非阻塞")
	int flag_fcntl = Fcntl(sock_fd, F_GETFL, 0); 		// 获取 I/O 状态
	Fcntl(sock_fd, F_SETFL, flag_fcntl | O_NONBLOCK); 	// 设置 I/O 非阻塞

/***************************** connect ******************************/
	int retval; // 函数返回值
	retval = connect(sock_fd, sa_ptr, sa_len);
	if (retval == 0) {						// connect completed immediately
		Fcntl(sock_fd, F_SETFL, flag_fcntl);// restore file status flags
		return 0;
	}
	if (retval < 0)	if (errno != EINPROGRESS/* 现在正在进行中  */) return(-1);

/***** Do whatever we want while the connect is taking place. ******/
	fd_set readset, writeset;	// 鉴别事件的标志位集合
	FD_ZERO(&readset); 	// 初始化集合:所有位均关闭
	FD_SET(sock_fd, &readset);
	writeset = readset;

	struct timeval tval{};
	tval.tv_sec = second;
	tval.tv_usec = 0;

	/* select() 设置或者检查存放在数据结构 fd_set 中的标志位来鉴别事件 */
	retval=select(sock_fd+1, &readset, &writeset, nullptr,
				  (second > 0)? &tval : nullptr);
	err_sys(retval, __FILE__, __func__, __LINE__, "select()");
	if (retval == 0) {
		close(sock_fd);
		errno = ETIMEDOUT; /* Connection timed out */
		return(-2);
	}

	int	error = 0;
	if (FD_ISSET(sock_fd, &readset) || FD_ISSET(sock_fd, &writeset)) {
		socklen_t error_len = sizeof(error);
		if (getsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
					   SO_ERROR/* 获得待处理错误 */, &error, &error_len) < 0)
			return(-3);	/* 挂起错误 */
	} else {
		err_quit(-1, __FILE__, __func__, __LINE__,
				 "select() error: sock_fd not set");
	}

	Fcntl(sock_fd, F_SETFL, flag_fcntl);// restore file status flags

	if (error) {
		close(sock_fd);
		errno = error;
		return(-4);
	}

	return(0);
}

