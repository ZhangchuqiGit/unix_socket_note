//
// Created by zcq on 2021/3/28.
//

#ifndef __SCTP_CLIENT_CALL_H
#define __SCTP_CLIENT_CALL_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <sys/types.h>
#include <cstring>
#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read

#include <iomanip> // 格式控制符 dec是十进制 hex是16进制 oct是8进制

#include <iostream>
//using namespace std;

/* Echoing messages to a stream */
void sctp_client_call(const int &sock_fd,
					  struct sockaddr *to_addr, const socklen_t &to_len);

/* Echoing messages to a stream */
void sctp_client_call_msg(FILE *fp, const int &sock_fd,
						  struct sockaddr *to_addr, const socklen_t &to_len);

/* Echoing messages to all streams */
void sctp_client_call_msgs(FILE *fp, const int &sock_fd,
						   struct sockaddr *to_addr, const socklen_t &to_len);

/* 设置 感兴趣 SCTP 通知事件 */
void sctp_client_call_event(const int &sock_fd,
							struct sockaddr *to_addr, const socklen_t &to_len);




#endif //__SCTP_CLIENT_CALL_H
