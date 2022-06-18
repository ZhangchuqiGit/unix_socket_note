//
// Created by zcq on 2021/3/30.
//

#ifndef __SOCK_NTOP_PTON_H
#define __SOCK_NTOP_PTON_H

#include "zcq_header.h"

#include <iostream>
#include <string>
#include <string_view>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read

/* 输出 ip 二进制网络格式 */
char * addr_str_to_net( std::string_view hostaddr/* 字符串 IP 地址 */,
						const int &family );

/* 输出 ip 二进制网络字节序格式 可重入版本 */
std::string addr_str_to_net_r( std::string_view hostaddr/* 字符串 IP 地址 */,
							   const int &family );

/* 输出 ip 字符串 如 192.168.6.7 */
std::string Addr_net_to_ptr_r(const struct sockaddr *sa_ptr);

/* 输出 ip 字符串 如 192.168.6.7 */
std::string Addr_net_to_ptr_r(const int &family, const struct sockaddr *sa_ptr);

/* 输出 ip 字符串 如 192.168.6.7 */
std::string addr_net_to_ptr_r(const int &family,
							  const void *in4_6addr, const size_t &len = 0);

/* 输出 ip 字符串 如 192.168.6.7 */
std::string addr_net_to_ptr(const struct sockaddr *sa_ptr);

/* 输出 ip 字符串 如 192.168.6.7 */
std::string Addr_net_to_ptr(const struct sockaddr *sa_ptr);

/* 输出 ip + port 字符串 如 192.168.6.7:1080,[1:...:1]:12345 */
std::string get_addr_port_ntop_r(const struct sockaddr *sa_ptr,
								 const socklen_t &len = 0);

/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
std::string get_addr_port_ntop(const struct sockaddr *sa_ptr,
							   const socklen_t &len = 0);

#endif //__SOCK_NTOP_PTON_H
