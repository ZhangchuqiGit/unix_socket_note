//
// Created by zcq on 2021/4/2.
//

#ifndef __SERVICE_ACCEPT_H
#define __SERVICE_ACCEPT_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //for inet_addr

#include <iostream>
//using namespace std;


/* ---- accept () 若 被系统中断、返回前连接终止，自己重启 ---- */
int Accept(const int &listen_fd, const int & family);

/* ---- accept () 若 被系统中断、返回前连接终止，自己重启 ---- */
int Accept(const int &listen_fd, struct sockaddr *servaddr = nullptr);

/* ---- accept () 非阻塞 select() ---- */
int Accept_nunblock_select(const int &listen_fd, const int & family = AF_INET);

#endif //__SERVICE_ACCEPT_H
