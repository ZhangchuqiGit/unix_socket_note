//
// Created by zcq on 2021/3/29.
//

#ifndef __IP_PORT_ADDRS_NAMES_H
#define __IP_PORT_ADDRS_NAMES_H

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


/************ 获取 本地/对方 与某个套接字 关联的 IP地址 和 端口号 *************/

/* 获取 本地 与某个套接字 关联的 IP地址。 */
/* 输出 ip 字符串 如 192.168.6.7 */
std::string Getsockname_ip(const int &socket_fd, const int &family = AF_INET);

/*	获取 本地 与某个套接字 关联的 IP地址 和 端口号。 */
/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
std::string  getsockname_ip_port(const int &socket_fd, const int &family = AF_INET);

/*	获取 本地 与某个套接字 关联的 IP地址 和 端口号。 */
std::string Getsockname_ip_port(const int &socket_fd, int & portnum,
								const int &family = AF_INET);

/**---------------------------------------------------------------------**/

/*	获取 对方 与某个套接字 关联的 IP地址。 */
/* 输出 ip 字符串 如 192.168.6.7 */
std::string Getpeername_ip(const int &socket_fd, const int &family = AF_INET);

/*	获取 对方 与某个套接字 关联的 IP地址 和 端口号。 */
/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
std::string getpeername_ip_port(const int &socket_fd, const int &family = AF_INET);

/*	获取 对方 与某个套接字 关联的 IP地址 和 端口号。 */
std::string Getpeername_ip_port(const int &socket_fd, int & portnum,
								const int &family = AF_INET);

/**---------------------------------------------------------------------**/

/* 获取 本地/对方 与某个套接字 关联的 IP地址 和 端口号。 */
void getIP_addr_port(const int &socket_fd, const int &family = AF_INET);

/************************* 不可重入版本 不推荐使用 **************************/

/** 不推荐使用 gethostbyname() 和 gethostbyaddr()，只能 获得 IPv4 信息！**/
/**	不推荐使用 gethostbyname2() **/

void host_display(); /* $ cat /etc/hosts */

/* gethostbyname()作用：用 域名 或者 主机名 获取 主机 IP 信息。只能 获得 IPv4 信息！ */
struct hostent * Get_host_byname_IPv4(std::string_view hostname);

/* gethostbyname2()作用：用 域名 或者 主机名 获取 主机 IP 信息。*/
struct hostent * Get_host_byname2(std::string_view hostname,
								  int af = AF_INET6);

/* gethostbyaddr()作用：用 IP 地址 获取 主机 IP 信息。只能 获得 IPv4 信息！  */
struct hostent * Get_host_byaddr_IPv4(std::string_view hostaddr);

/* 	gethostbyname() 用 域名 或者 主机名 获取 主机 IP 信息
	gethostbyaddr() 用 IP 地址 获取 主机 IP 信息 */
void Get_host_byname_byaddr_IPv4(std::string_view hostname);

/************************ 可重入版本 推荐使用 线程安全 **********************/

/**	可重入版本
	gethostbyname_r()，只能 获得 IPv4 信息！
	gethostbyname2_r()，
	gethostbyaddr_r()    		           推荐使用 **/

void host_display_r(); /* $ cat /etc/hosts */
void host_lambda_func(std::string_view str, char **ptr, const int &mode);
void host_byname_echo(struct hostent *hostent_ptr);

/* gethostbyname_r()：用 域名 或者 主机名 获取 主机 IP 信息。只能 获得 IPv4 信息！*/
int Get_host_byname_r_IPv4(std::string_view hostname);

/* gethostbyname2_r()：用 域名 或者 主机名 获取 主机 IP 信息。 */
int Get_host_byname2_r(std::string_view hostname,
					   int af = AF_INET6);

/* gethostbyaddr_r()作用：用 IP 地址 获取 主机 IP 信息。*/
int Get_host_byaddr_r(std::string_view hostaddr,
					  const int &addrtype = AF_INET6);

/************************* 不可重入版本 不推荐使用 **************************/

/** 不推荐使用 getservbyname() 和 getservbyport() **/

void serv_display(); /* $ cat /etc/services */
void serv_byname(struct servent *servent_ptr);

/* getservbyname()作用：使用 服务名+协议名 获取 服务 IP 信息。 */
struct servent * Get_serv_byname(std::string_view servname,
								 const char * protoname = nullptr);

/* getservbyport()作用：使用 端口号+协议名 获取 服务 IP 信息。 */
struct servent * Get_serv_byport(const int &portnum,
								 const char* protoname = nullptr);

/************************ 可重入版本 推荐使用 线程安全 **********************/

/**	可重入版本 getservbyname_r() 和 getservbyport_r() 推荐使用 **/

void serv_display_r(); /* $ cat /etc/services */

/* getservbyname_r()作用：使用 服务名+协议名 获取 服务 IP 信息。 */
int Get_serv_byname_r(std::string_view servname,
					  const char* protoname = nullptr);

/* getservbyport_r()作用：使用 端口号+协议名 获取 服务 IP 信息。 */
int Get_serv_byport_r(const int &portnum,
					  const char* protoname = nullptr);

/************************** 推荐使用 线程安全 ******************************/
/***************** 要求 释放动态存储空间 freeaddrinfo(result) **************/

struct addrinfo *Get_addrinfo(std::string_view host,
							  std::string_view service = PORT_service_STR,
							  const int &family = AF_UNSPEC,
							  const int &socktype = SOCK_STREAM,
							  const int &protocol = IPPROTO_IP,
							  const int &flags =
							  AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV);
/*	AI_CANONNAME	0x0002	ai_canonname 返回 主机的 规范名称
	AI_NUMERICHOST	0x0004	表明 host 只是 IP地址串，不是主机名(域名)
	AI_NUMERICSERV	0x0400	表明 service 只是 端口号，不是服务名*/
struct addrinfo *Get_addrinfo(const char *host = HOST_Service_IPv4,
							  const char *service = PORT_service_STR,
							  const int &family = AF_UNSPEC,
							  const int &socktype = SOCK_STREAM,
							  const int &protocol = IPPROTO_IP,
							  const int &flags =
							  AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV);

/************************** 推荐使用 线程安全 ******************************/
/***************** 要求 释放动态存储空间 freeaddrinfo(result) **************/

/* 尝试 获取 主机 和 服务 的名字 */
void Get_nameinfo(const struct sockaddr *sockaddr, const socklen_t &addrlen,
				  char *host = nullptr, const size_t &hostlen = 0,
				  char *serv = nullptr, const size_t &servlen = 0,
				  const int &flags = NI_NUMERICHOST | NI_NUMERICSERV );


#endif //__IP_PORT_ADDRS_NAMES_H
