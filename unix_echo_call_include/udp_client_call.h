//
// Created by zcq on 2021/3/17.
//

#ifndef __UDP_CLIENT_CALL_H
#define __UDP_CLIENT_CALL_H

#include "zcq_header.h"
//#include "zcq_config.h"	// 配置选项
//#include "zcq_error.h" // 错误打印消息

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>  //for read
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>


void udp_client_call(const int & socket_fd,
					 const struct sockaddr *cli_addr,/** 只要参数 address family */
					 const socklen_t & sizeof_addr,
					 const time_t &seconds = 3);

void udp_client_call_r(const int & socket_fd,
					   const struct sockaddr *cli_addr,/** 只要参数 address family */
					   const socklen_t & sizeof_addr,
					   const time_t &seconds = 3);

/*************************************************************************/

/* 传递互动信息 */
void udp_client_call_msg(const int & socket_fd,
						 const struct sockaddr * serv_addr,
						 const socklen_t & sizeof_addr);

/*************************************************************************/

/* 使用 SIGALRM 为 recvfrom 设置超时 */
void udp_client_call_timeo_SIGALRM(const int & socket_fd,
								   const struct sockaddr * serv_addr,
								   const socklen_t & sizeof_addr,
								   const time_t &seconds = 3);

/* 使用 select 为 recvfrom 设置超时: 等待描述符变为可读 */
void udp_client_call_timeo_select(const int & socket_fd,
								  const struct sockaddr * serv_addr,
								  const socklen_t & sizeof_addr,
								  const time_t &seconds = 3);

/* 使用 通用套接字选项 SO_RCVTIMEO（接收超时） 为 recvfrom 设置超时 */
void udp_client_call_timeo_SockOptions(const int & socket_fd,
									   const struct sockaddr * serv_addr,
									   const socklen_t & sizeof_addr,
									   const time_t &seconds = 3);

/************************* 避免由 SIGALRM 竞争 ****************************/

void udp_client_call_broadcast(const int & sock_fd,
							   const struct sockaddr * serv_addr,
							   const socklen_t & sizeof_addr,
							   const time_t &seconds = 3);

/* 避免由 SIGALRM 竞争  使用 pselect 阻塞和解阻塞信号 */
void udp_client_call_broadcast_pselect(const int & sock_fd,
									   const struct sockaddr * serv_addr,
									   const socklen_t & sizeof_addr,
									   const time_t &seconds = 3);

#include <setjmp.h>
/* 在信号处理函数中进行非局部转移 */
void udp_client_call_broadcast_sigjmp(const int & sock_fd,
									  const struct sockaddr * serv_addr,
									  const socklen_t & sizeof_addr,
									  const time_t &seconds = 3);

/* 避免由 SIGALRM 竞争  使用从信号处理函数到主控函数的管道作为 IPC */
void udp_client_call_broadcast_IPC(const int & sock_fd,
								   const struct sockaddr * serv_addr,
								   const socklen_t & sizeof_addr,
								   const time_t &seconds = 3);

/****************************** 请求-应答 *********************************/

/* 	在信号处理函数中进行非局部转移
 	增加 可靠性： 	1、超时和重传：处理丢失数据；
 				 	2、序列号：验证应答是否匹配。    */
void udp_client_call_rtt(const int & sock_fd,
						 const struct sockaddr * serv_addr,
						 const socklen_t & sizeof_addr,
						 const time_t &seconds = 3);



#endif //__UDP_CLIENT_CALL_H
