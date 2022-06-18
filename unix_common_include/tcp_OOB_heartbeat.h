//
// Created by zcq on 2021/5/8.
//

#ifndef __TCP_OOB_HEARTBEAT_H
#define __TCP_OOB_HEARTBEAT_H

#include "zcq_header.h"

/* 心搏机制：周期信号轮询对端，无响应即不存活 */
void heartbeat_client(const int &sock_fd, const int &nsec_arg, const int &max_arg);

/* 心搏机制：周期信号轮询对端，无响应即不存活 */
void heartbeat_service(const int &sock_fd, const int &nsec_arg, const int &max_arg);

#endif //__TCP_OOB_HEARTBEAT_H
