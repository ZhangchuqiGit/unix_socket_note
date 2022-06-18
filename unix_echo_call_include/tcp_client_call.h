//
// Created by zcq on 2021/3/17.
//

#ifndef __TCP_CLIENT_CALL_H
#define __TCP_CLIENT_CALL_H

#include "zcq_header.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>  //for read

/** 所谓的IO复用，就是同时等待多个文件描述符就绪，以系统调用的形式提供。
如果所有文件描述符都没有就绪的话，该系统调用阻塞，否则调用返回，允许用户进行后续的操作。
常见的 IO复用 技术有 select, poll, epoll 以及 kqueue 等等。
其中 epoll 为 Linux 独占，而 kqueue 则在许多 UNIX系统上存在，包括 macOS。  **/


void tcp_client_call(FILE *fp, const int &sock_fd);

/* 传递互动信息 */
void tcp_client_call_msg(FILE *fp, const int &sock_fd);

/* 传递(二进制)结构，计算两数之和 */
void tcp_client_call_sum_struct(FILE *fp, const int &sock_fd);

/* I/O 复用 select */
void tcp_client_call_select(FILE *fp, const int &sock_fd);

/* 非阻塞 I/O 复用 select */
void tcp_client_call_nonblock_select(FILE *fp, const int &sock_fd);

/* I/O 复用 poll */
void tcp_client_call_poll(FILE *fp, const int &sock_fd);

/* I/O 复用 高级轮询技术 epoll */
//void tcp_client_call_epoll(FILE *fp, const int &sock_fd);

/* I/O 复用 高级轮询技术 kqueue */
void tcp_client_call_kqueue(FILE *fp, const int &sock_fd);

/* 父进程从标准输入，子进程从标准输出 */
void tcp_client_call_fork(FILE *fp, int sock_fd);


#endif //__TCP_CLIENT_CALL_H
