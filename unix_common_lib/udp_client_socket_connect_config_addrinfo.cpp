//
// Created by zcq on 2021/4/3.
//

#include "udp_client_socket_connect_config_addrinfo.h"


int udp_socket_config( struct sockaddr *sa_p, 	socklen_t *sa_len,
					   const char *host, 		const char *service,
					   const int &family, 		const int &socktype,
					   const int &protocol, 	const int &flags )
{
	int sockfd; // 套接字描述符
	/* ---- 配置 ---- */
	struct addrinfo *result = Get_addrinfo( host, service, family,
											socktype, protocol, flags);
	struct addrinfo	*saveTOfree = result;

	do {
		/* ---- socket () ---- */
//		sockfd = socket(family, socktype, protocol);
		sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (sockfd >= 0) break; /* success */

	}	/* ai_next 指向下一个 addrinfo 结构体的指针 */
	while ( (result = result->ai_next) != nullptr );

	if (result == nullptr)	/* errno set from final connect() */
		err_sys(-1, __LINE__, "%s():\thost:%s;\tservice:%s",
				__func__ , host, service );

	/* copy family and port : struct sockaddr{} */
	memcpy(sa_p, result->ai_addr, result->ai_addrlen);
	if (sa_len != nullptr) *sa_len = result->ai_addrlen;

#if 1
	std::cout << "位掩码 \t \tai_flags: \t" 		<< result->ai_flags 	<< std::endl
			  << "协议族 \t \tai_family: \t" 	<< result->ai_family 	<< std::endl
			  << "socket类型 \tai_socktype: \t"	<< result->ai_socktype 	<< std::endl
			  << "协议类型 \tai_protocol: \t" 	<< result->ai_protocol 	<< std::endl;
	if (result->ai_canonname != nullptr)
		std::cout << "对方主机名称 ai_canonname: \t"
				  << (char *)result->ai_canonname << std::endl;
	std::cout << "对方主机 ip: " << addr_net_to_ptr(result->ai_addr) << std::endl;
#endif
	freeaddrinfo(saveTOfree); // 释放结构体链表指向的任何动态存储空间
	return(sockfd);
}


int udp_socket_connect_config( struct sockaddr *sa_p, 	socklen_t *sa_len,
							   const char *host, 		const char *service,
							   const int &family, 		const int &socktype,
							   const int &protocol, 	const int &flags )
{
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

		/* ---- connect () ---- */
		if (connect(sockfd, result->ai_addr, result->ai_addrlen) == 0) break;

		/* ---- close () ---- */
		err_sys( close(sockfd), __LINE__, "close()");

	}	/* ai_next 指向下一个 addrinfo 结构体的指针 */
	while ( (result = result->ai_next) != nullptr );

	if (result == nullptr)	/* errno set from final connect() */
		err_sys(-1, __LINE__, "%s():\thost:%s;\tservice:%s",
				__func__ , host, service );

	memcpy(sa_p, result->ai_addr, result->ai_addrlen);
	if (sa_len != nullptr) *sa_len = result->ai_addrlen;
#if 1
	std::cout << "位掩码 \t \tai_flags: \t" 		<< result->ai_flags 	<< std::endl
			  << "协议族 \t \tai_family: \t" 	<< result->ai_family 	<< std::endl
			  << "socket类型 \tai_socktype: \t"	<< result->ai_socktype 	<< std::endl
			  << "协议类型 \tai_protocol: \t" 	<< result->ai_protocol 	<< std::endl;
	if (result->ai_canonname != nullptr)
		std::cout << "主机名称 \tai_canonname: \t"
				  << (char *)result->ai_canonname << std::endl;
	std::cout << "ip: " << addr_net_to_ptr(result->ai_addr) << std::endl;
#endif
	freeaddrinfo(saveTOfree); // 释放结构体链表指向的任何动态存储空间
	return(sockfd);
}



