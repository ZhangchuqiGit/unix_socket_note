//
// Created by zcq on 2021/4/2.
//

#include "service_accept.h"


/* ---- accept () 若 被系统中断、返回前连接终止，自己重启 ---- */
int Accept(const int &listen_fd, const int & family)
{
	int accept_fd; // 新(连接成功)套接字描述符
	socklen_t valsize;  // sizeof(struct sockaddr) = 16
	uint8_t num = 0;			// 重启次数
	while (true)
	{
		switch (family)
		{
			case AF_INET: {
				struct sockaddr_in sai_IPv4{}; /* Internet socket address. */
				valsize = INET_ADDRSTRLEN; // sizeof(struct sockaddr_in) = 16
				accept_fd = accept(listen_fd, (struct sockaddr *) (&sai_IPv4), &valsize);
			}
				break;
			case AF_INET6: {
				struct sockaddr_in6 sai_IPv6{}; /* IPv6 socket address. */
				valsize = sizeof(sai_IPv6); // sizeof(struct sockaddr_in6) = 28
//				valsize = INET6_ADDRSTRLEN; // sizeof(struct sockaddr_in6) = 28
				accept_fd = accept(listen_fd, (struct sockaddr *) (&sai_IPv6), &valsize);
			}
				break;
//			case AF_LOCAL: {
			case AF_UNIX: {
				struct sockaddr_un sau{};
				valsize = sizeof(sau); // sizeof(struct sockaddr_un) = 110
//				valsize = _SS_SIZE; // sizeof(struct sockaddr_un) = 110
				accept_fd = accept(listen_fd, (struct sockaddr *) (&sau), &valsize);
			}
				break;
			default: {
				struct sockaddr_storage	sas{};
				valsize = _SS_SIZE; // sizeof(struct sockaddr_storage) = 128
				accept_fd = accept(listen_fd, (struct sockaddr *) (&sas), &valsize);
			}
				break;
		}
		if (accept_fd < 0){
			if (errno == EINTR) {
				if (++num >= 10) err_ret(accept_fd, __LINE__,
										 "accept():若被系统中断，自己重启 num >= 10"); // 重启次数
				continue; 							// 若被系统中断，自己重启
			}
			err_sys(accept_fd, __LINE__, "accept()");
		}
		else break;
	}
#if 1
	/* 获取 与某个套接字 关联的 本地/对方 协议地址 */
	getIP_addr_port(accept_fd);
#endif
	return accept_fd; // 新(连接成功)套接字描述符
}

/* ---- accept () 若 被系统中断、返回前连接终止，自己重启 ---- */
int Accept(const int &listen_fd, struct sockaddr *servaddr)
{
	int mode;
	if (servaddr == nullptr) mode = AF_INET;
	else mode = servaddr->sa_family;
	return Accept(listen_fd, mode); // 新(连接成功)套接字描述符
}

/* ---- accept () 非阻塞 select() ---- */
int Accept_nunblock_select(const int &listen_fd, const int & family)
{
	struct sockaddr_in sai_IPv4{}; 	/* Internet socket address. */
	struct sockaddr_in6 sai_IPv6{}; /* IPv6 socket address. */
	struct sockaddr_un sau{}; 		/* AF_LOCAL / AF_UNIX socket address */
	struct sockaddr_storage	sas{};
	struct sockaddr *service_addr;
	socklen_t valsize = 0;
	switch (family)
	{
		case AF_INET:
			valsize = INET_ADDRSTRLEN; // sizeof(struct sockaddr_in) = 16
			service_addr = (struct sockaddr *)(&sai_IPv4);
			break;
		case AF_INET6:
//			valsize = INET6_ADDRSTRLEN; // sizeof(struct sockaddr_in6) = 28
			valsize = sizeof(sai_IPv6); // sizeof(struct sockaddr_in6) = 28
			service_addr = (struct sockaddr *)(&sai_IPv6);
			break;
//			case AF_LOCAL:
		case AF_UNIX:
//			valsize = _SS_SIZE; // sizeof(struct sockaddr_un) = 110
			valsize = sizeof(sau); // sizeof(struct sockaddr_un) = 110
			service_addr = (struct sockaddr *)(&sau);
			break;
		default:
			valsize = _SS_SIZE; // sizeof(struct sockaddr_storage) = 128
			service_addr = (struct sockaddr *)(&sas);
			break;
	}
	uint8_t num = 0;			// 重启次数
	int accept_fd; 			// 新(连接成功)套接字描述符
	while (true)
	{
		accept_fd = accept(listen_fd,
						   (struct sockaddr *)(&service_addr),
						   &valsize);
		if (accept_fd < 0) {
			if (errno == EINTR) {
				if (++num >= 10) return -1; 			// 重启次数
				continue; 							// 若被系统中断，自己重启
			}
			if (errno == EWOULDBLOCK) return -2;	// 客户终止连接，源自 Berkeley
			if (errno == ECONNABORTED) return -3;	// 客户终止连接，源自 POSIX
			if (errno == EPROTO) return -4;			// 客户终止连接，源自 SVR4
			err_sys(accept_fd, __FILE__,
					__func__, __LINE__, "accept()");
		}
		else break;
	}
	/* 获取 与某个套接字 关联的 本地/对方 协议地址 */
	getIP_addr_port(accept_fd);

	/************************** 设置 I/O 非阻塞 **************************/
	_debug_log_info("设置 I/O 非阻塞  accept_fd: %d", accept_fd)
	int flags = Fcntl(accept_fd, F_GETFL, 0); 			// 获取 I/O 状态
	Fcntl(accept_fd, F_SETFL, flags | O_NONBLOCK); 	// 设置 I/O 非阻塞

	return accept_fd; // 新(连接成功)套接字描述符
}

