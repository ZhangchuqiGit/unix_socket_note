//
// Created by zcq on 2021/4/5.
//

#ifndef __SOCK_FD_SELECT_TIMEO_H
#define __SOCK_FD_SELECT_TIMEO_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //for inet_addr

#include <iostream>
//using namespace std;

/* 设置超时: select() 等待描述符变为可读 */
int fd_read_select_timeo(const int & socket_fd, const int &seconds = 3);

/* 设置超时: pselect() 等待描述符变为可读
	信号掩码: 若收到该信号，中断返回 0；否则返回 -1(error)  */
int fd_read_pselect_timeo(const int & socket_fd,
						  sigset_t sig/*信号掩码:阻塞*/,
						  const time_t &seconds = 3,
						  const long &nanoseconds = 0 );


#endif //__SOCK_FD_SELECT_TIMEO_H
