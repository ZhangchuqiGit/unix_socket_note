//
// Created by zcq on 2021/3/31.
//

#ifndef __TCP_CLIENT_SOCKET_CONNECT_ADDRINFO_H
#define __TCP_CLIENT_SOCKET_CONNECT_ADDRINFO_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //for inet_addr

#include <iostream>
//using namespace std;

int Connect(const int &sock_fd, const struct sockaddr *sa_ptr);

/* 使用 SIGALRM 为 connect() 设置超时 */
int connect_timeo(const int &sock_fd, const struct sockaddr *sa_ptr,
				  const int &second);

/* blocking connect() */
int tcp_socket_connect(std::string_view host, std::string_view service,
					   const int &family = AF_UNSPEC,
		/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
		/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
				   const int &socktype = SOCK_STREAM,
					   const int &protocol = IPPROTO_IP,
					   const int &flags =
					   AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV);
/*	AI_CANONNAME	0x0002	ai_canonname 返回 主机的 规范名称
	AI_NUMERICHOST	0x0004	表明 host 只是 IP地址串，不是主机名(域名)
	AI_NUMERICSERV	0x0400	表明 service 只是 端口号，不是服务名*/

/* 使用 非阻塞 I/O 复用 select() 为 connect() 设置超时 */
int connect_nonb_timeo(const int &sock_fd, const struct sockaddr *sa_ptr,
					   const socklen_t &sa_len, const int &second=3);


#endif //__TCP_CLIENT_SOCKET_CONNECT_ADDRINFO_H
