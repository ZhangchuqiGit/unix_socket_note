//
// Created by zcq on 2021/4/3.
//

#ifndef __UDP_CLIENT_SOCKET_CONNECT_CONFIG_ADDRINFO_H
#define __UDP_CLIENT_SOCKET_CONNECT_CONFIG_ADDRINFO_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //for inet_addr

#include <iostream>
//using namespace std;

/*	AI_PASSIVE		0x0001	套接字被动打开，通常用于 服务器 server 的 socket()
							仅当 hostname=NULL 时起作用；若设置了该标志，
							则查询结果中的IP地址是通配地址；
							若未设置该标志，则IP地址是环回地址。
	AI_NUMERICHOST	0x0004	表明 host 只是 IP地址串，不是主机名(域名)
	AI_NUMERICSERV	0x0400	表明 service 只是 端口号，不是服务名*/
//int udp_socket_config(struct sockaddr **sa_p,
int udp_socket_config(struct sockaddr *sa_p,
					  socklen_t *sa_len = nullptr,
					  const char *host = HOST_Service_IPv4,
					  const char *service  = PORT_service_STR,
					  const int &family = AF_UNSPEC,
		/* SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
					  const int &socktype = SOCK_DGRAM,
					  const int &protocol = IPPROTO_IP,
					  const int &flags =
					  AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV );
//					  AI_NUMERICHOST | AI_NUMERICSERV );

int udp_socket_connect_config( struct sockaddr *sa_p,
							   socklen_t *sa_len = nullptr,
							   const char *host = HOST_Service_IPv4,
							   const char *service  = PORT_service_STR,
							   const int &family = AF_UNSPEC,
		/* SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
							   const int &socktype = SOCK_DGRAM,
							   const int &protocol = IPPROTO_IP,
							   const int &flags =
							   AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV );


#endif //__UDP_CLIENT_SOCKET_CONNECT_CONFIG_ADDRINFO_H
