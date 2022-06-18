//
// Created by zcq on 2021/4/2.
//

#ifndef __TCP_SERVICE_SOCKET_BIND_LISTEN_ADDRINFO_H
#define __TCP_SERVICE_SOCKET_BIND_LISTEN_ADDRINFO_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //for inet_addr

#include <iostream>
//using namespace std;


int tcp_socket_bind_listen(std::string_view host ,
						   std::string_view service = PORT_service_STR,
						   socklen_t *addrlen = nullptr,
						   const int &family = AF_UNSPEC,
						   const int &socktype = SOCK_STREAM,
						   const int &protocol = IPPROTO_IP,
						   const int &flags =
						   AI_PASSIVE | AI_NUMERICHOST | AI_NUMERICSERV);

/*	AI_PASSIVE		0x0001	套接字被动打开，通常用于 服务器 server 的 socket()
							仅当 hostname=NULL 时起作用；若设置了该标志，
							则查询结果中的IP地址是通配地址；
							若未设置该标志，则IP地址是环回地址。
	AI_NUMERICHOST	0x0004	表明 host 只是 IP地址串，不是主机名(域名)
	AI_NUMERICSERV	0x0400	表明 service 只是 端口号，不是服务名*/

int tcp_socket_bind_listen(const char *host = nullptr,
						   const char *service = PORT_service_STR,
						   socklen_t *addrlen = nullptr,
						   const int &family = AF_UNSPEC,
						   const int &socktype = SOCK_STREAM,
						   const int &protocol = IPPROTO_IP,
						   const int &flags =
						   AI_PASSIVE | AI_NUMERICHOST | AI_NUMERICSERV );

#endif //__TCP_SERVICE_SOCKET_BIND_LISTEN_ADDRINFO_H
