//
// Created by zcq on 2021/4/3.
//

#include "udp_service_socket_bind_addrinfo.h"


int udp_socket_bind_config( struct sockaddr *sa_p, 	socklen_t *sa_len,
							const char *host, 		const char *service,
							const int &family, 		const int &socktype,
							const int &protocol, 	const int &flags )
{
	int ret_value; // 函数返回值
	int sockfd; // 套接字描述符
	/* ---- 配置 ---- */
	struct addrinfo *result = Get_addrinfo( host, service, family,
											socktype, protocol, flags);
	struct addrinfo	*saveTOfree = result;

	do {
		/* ---- socket () ---- */
//		sockfd = socket(family, socktype, protocol);
		sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (sockfd < 0) continue;

		/* ---- 允许地址的立即重用，端口不占用 ---- */
		int option = 1;
		ret_value=setsockopt(sockfd, SOL_SOCKET/* 通用选项 */,
							 SO_REUSEADDR/*地址立即重用*/, &option, sizeof(option) );
		err_sys(ret_value, __LINE__,"setsockopt()");
#if 0
		/* ---- 允许端口的立即重用 ---- */
		ret_value=setsockopt(sockfd, SOL_SOCKET/*通用选项*/,
							 SO_REUSEPORT/*端口立即重用*/, &option, sizeof(option) );
		err_sys(ret_value, __LINE__,"setsockopt()");
#endif
#ifdef Receiver_MAXBuf_mode
		/* ---- 修改 接收 缓冲区 大小 ---- */
	int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	ret_value=setsockopt(socket_fd, SOL_SOCKET/*通用选项*/,
						 SO_RCVBUF/* 接收缓冲区 */,
						 &receiver_buf, sizeof(receiver_buf) );
	err_sys(ret_value, __LINE__,"setsockopt()");
#endif

		/* ---- bind () ---- */
		if (bind(sockfd, result->ai_addr, result->ai_addrlen) == 0) break;/* success */

		/* ---- close () ---- */
		err_sys( close(sockfd), __LINE__, "close()");

	}	/* ai_next 指向下一个 addrinfo 结构体的指针 */
	while ( (result = result->ai_next) != nullptr );

	if (result == nullptr)	/* errno set from final connect() */
		err_sys(-1, __LINE__, "%s():\thost:%s;\tservice:%s",
				__func__ , host, service );

	/* copy family and port : struct sockaddr{} */
	if (sa_p != nullptr) memcpy(sa_p, result->ai_addr, result->ai_addrlen);
	if (sa_len != nullptr) *sa_len = result->ai_addrlen;

#if 1
	std::cout << "位掩码 \t \tai_flags: \t" 		<< result->ai_flags 	<< std::endl
			  << "协议族 \t \tai_family: \t" 	<< result->ai_family 	<< std::endl
			  << "socket类型 \tai_socktype: \t"	<< result->ai_socktype 	<< std::endl
			  << "协议类型 \tai_protocol: \t" 	<< result->ai_protocol 	<< std::endl;
	if (result->ai_canonname != nullptr)
		std::cout << "主机名称 \tai_canonname: \t"
				  << (char *)result->ai_canonname << std::endl;
	else std::cout << "主机 ip: " << addr_net_to_ptr(result->ai_addr) << std::endl;
#endif
	freeaddrinfo(saveTOfree); // 释放结构体链表指向的任何动态存储空间
	return(sockfd);
}



