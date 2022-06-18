//
// Created by zcq on 2021/3/16.
//

#ifndef __UDP_SERVICE_ECHO_H
#define __UDP_SERVICE_ECHO_H

#include "zcq_header.h"
//#include "zcq_config.h"	// 配置选项
//#include "zcq_error.h" // 错误打印消息


#include <cerrno>
#include <cstdlib>
#include <unistd.h>  //for read
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>  //for read
#include <ctime>


void udp_service_echo(const int & socket_fd,
					  const struct sockaddr * server_address,
					  const socklen_t & sizeof_servaddr,
					  const int &seconds = 0 );

/* 传递信息 */
void udp_service_echo_msgs(const int & socket_fd,
						   const struct sockaddr * server_addr,
						   const socklen_t & sizeof_servaddr,
						   const int &seconds = 0 );

/* 接收标志、目的IP地址、接口/网口索引 单个读/写缓冲区 */
void udp_service_echo_flags(const int & sock_fd,
							const struct sockaddr * server_addr,
							const socklen_t & sizeof_servaddr);

/* recvmsg() 仅套接字描述符 分散读 可选标志 可选对面地址 可选控制信息 */
/* 	在信号处理函数中进行非局部转移
 	增加 可靠性： 	1、超时和重传：处理丢失数据；
 				 	2、序列号：验证应答是否匹配。    */
void udp_service_echo_seq_ts(const int & sock_fd,
							 const struct sockaddr * server_addr,
							 const socklen_t & sizeof_servaddr);


#endif //__UDP_SERVICE_ECHO_H
