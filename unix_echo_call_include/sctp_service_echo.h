//
// Created by zcq on 2021/3/28.
//

#ifndef __SCTP_SERVICE_ECHO_H
#define __SCTP_SERVICE_ECHO_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <sys/types.h>
#include <cstring>
#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read

#include <iostream>
//using namespace std;

/* 传递信息 */
void sctp_service_echo(const int &sock_fd,
					   struct sockaddr *sa, const socklen_t &salen);

/******************** 设置 SCTP 一对多(udp) : 无 accept() **********************/

void sctp_service_echo_msg(const int &sock_fd,
						   struct sockaddr *sa, const socklen_t &salen);

/* 传输“过大”消息时部分递交，其它等待该消息完整提交 */
void sctp_service_echo_part(const int &sock_fd,
							struct sockaddr *sa, const socklen_t &salen);

/* 一到多 转 一对一 的迸发SCTP服务；需用 sctp_peelof() 从sctp服务分离并获取一对一套接字 */
void sctp_service_echo_fork(const int & sock_fd,
							struct sockaddr *sa, const socklen_t &salen);

/******************************************************************************/

/** evnts.sctp_data_io_event = 1; // I/O 通知事件 才能调用sctp_recvmsg()接收对端DATA
	evnts.sctp_association_event = 1; // SCTP关联事件
	evnts.sctp_address_event = 1; // SCTP地址事件
	evnts.sctp_send_failure_event = 1; // SCTP发送失败事件
	evnts.sctp_peer_error_event = 1; // sctp peer 错误事件
	evnts.sctp_shutdown_event = 1; // 连接被关闭
	evnts.sctp_partial_delivery_event = 1; // 部分递交
	evnts.sctp_adaptation_layer_event = 1; // SCTP适配层事件
	evnts.sctp_authentication_event = 1; // SCTP身份验证事件
	evnts.sctp_sender_dry_event = 1; //
	evnts.sctp_stream_reset_event = 1; // SCTP流重置事件
	evnts.sctp_assoc_reset_event = 1; // 重置事件
	evnts.sctp_stream_change_event = 1; // SCTP流更改事件           */
/* 设置 所有 感兴趣 SCTP 通知事件 */
void sctp_service_echo_event(const int & sock_fd,
							 struct sockaddr *sa, const socklen_t &salen);


#endif //__SCTP_SERVICE_ECHO_H
