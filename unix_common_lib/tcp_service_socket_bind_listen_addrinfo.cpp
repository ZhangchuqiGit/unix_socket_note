//
// Created by zcq on 2021/4/2.
//

#include "tcp_service_socket_bind_listen_addrinfo.h"

int tcp_socket_bind_listen(const char *host, const char *service,
						   socklen_t *addrlen,
						   const int &family, const int &socktype,
						   const int &protocol, const int &flags)
{
	int retval; // 函数返回值
	int listen_fd; // 套接字描述符

	/* ---- 配置 ---- */
	struct addrinfo *result = Get_addrinfo( host, service, family,
											socktype, protocol, flags);
	struct addrinfo	*saveTOfree = result;

	do {
		/* ---- socket () ---- */
		listen_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (listen_fd < 0) continue; /* error, try next one */

		/* ---- 允许地址的立即重用，端口不占用 ---- */
		int option = 1;
		retval=setsockopt(listen_fd, SOL_SOCKET/* 通用选项 */,
						  SO_REUSEADDR/*地址立即重用*/, &option, sizeof(option) );
		err_ret(retval, __LINE__,"setsockopt()");

		/* ---- 允许端口的立即重用 ---- */
//		retval=setsockopt(listen_fd, SOL_SOCKET/*通用选项*/,
//						  SO_REUSEPORT/*端口立即重用*/, &option, sizeof(option) );
//		err_sys(retval, __LINE__,"setsockopt()");

		/* ---- 修改 接收 缓冲区 大小 ---- */
#ifdef Receiver_MAXBuf_mode
		int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	retval=setsockopt(listen_fd, SOL_SOCKET/*通用选项*/,
						 SO_RCVBUF/* 接收缓冲区 */,
						 &receiver_buf, sizeof(receiver_buf) );
	err_sys(retval, __LINE__,"setsockopt()");
#endif
		/* ---- bind () ---- */
		if ( bind(listen_fd, result->ai_addr, result->ai_addrlen) == 0 ) break;

		/* ---- close () ---- */
		err_sys( close(listen_fd), __LINE__, "close()");

	}	/* ai_next 指向下一个 addrinfo 结构体的指针 */
	while ( (result = result->ai_next) != nullptr );

	if (result == nullptr)	/* errno set from final socket() or bind() */
		err_sys(-1, __LINE__, "%s():\thost:%s;\tservice:%s",
				__func__ , host, service );

	/* ---- listen () ---- */
	retval=listen(listen_fd, MAX_listen/*最大连接数*/ );
	err_sys(retval, __LINE__, "listen()");

#if 1
	std::cout << "位掩码 \t \tai_flags: \t" 		<< result->ai_flags 	<< std::endl
			  << "协议族 \t \tai_family: \t" 	<< result->ai_family 	<< std::endl
			  << "socket类型 \tai_socktype: \t"	<< result->ai_socktype 	<< std::endl
			  << "协议类型 \tai_protocol: \t" 	<< result->ai_protocol 	<< std::endl;
	if (result->ai_canonname != nullptr)
		std::cout << "主机名称 \tai_canonname: \t"
				  << (char *)result->ai_canonname << std::endl;
#endif
	if (addrlen) *addrlen = result->ai_addrlen;	/* Length of socket address. */
	freeaddrinfo(saveTOfree); // 释放结构体链表指向的任何动态存储空间
	return(listen_fd);
}

int tcp_socket_bind_listen(std::string_view host, std::string_view service,
						   socklen_t *addrlen,
						   const int &family, const int &socktype,
						   const int &protocol, const int &flags)
{
	return tcp_socket_bind_listen((const char *)host.data(),
								  (const char *)service.data(),
								  addrlen, family, socktype, protocol, flags);
}

