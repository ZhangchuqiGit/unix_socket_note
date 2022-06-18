//
// Created by zcq on 2021/3/16.
//

#ifndef __TCP_SERVICE_ECHO_H
#define __TCP_SERVICE_ECHO_H

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


void tcp_service_echo(const int & socket_fd);

/* 传递互动信息 */
void tcp_service_echo_msg(const int & sock_fd);

/* 传递文本串，计算两数之和 */
void tcp_service_echo_sum(const int & sock_fd);

/* 传递(二进制)结构，计算两数之和 */
void tcp_service_echo_sum_struct(const int & sock_fd);

/* I/O 复用 高级轮询技术 epoll */
void tcp_service_echo_IO_epoll(const int &listen_fd);


#endif //CP_STR_ECHO_H
