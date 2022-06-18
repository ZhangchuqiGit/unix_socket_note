//
// Created by zcq on 2021/3/30.
//

#ifndef __SOCK_ADDR_CONFIG_H
#define __SOCK_ADDR_CONFIG_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //for inet_addr

#include <iostream>
//using namespace std;


/* ---- 配置 ip 地址 端口 信息 ---- */
void sock_address_config( struct sockaddr *sa,
						  const int &family /* Address families */,
						  const int &port_net	/* 网络字节序 端口号 */,
						  const void *host_net	/* 网络字节序 IP 地址 */);

/* ---- 配置 ip 地址 端口 信息 ---- */
void Sock_address_config( struct sockaddr *sa,
						  const int &family = AF_INET/* Address families */,
						  const int &portnum = PORT_service/* 端口号 */,
						  std::string_view host_ip = HOST_Service_IPv4/* 字符串 IP 地址 */ );

#endif //__SOCK_ADDR_CONFIG_H
