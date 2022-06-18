//
// Created by zcq on 2021/4/28.
//

#ifndef __SCTP_EVENT_H
#define __SCTP_EVENT_H

#include "zcq_header.h"


/* 打印 通知事件 *//** 设置的通知事件存在 接收区 buf_str 的头部区域 **/
void print_notification_event(std::string_view buf_str);

/* 检查 通知事件 *//** 获取 所有 对方地址 本地地址 **/
void check_notification(const int &sock_fd, std::string_view bufstr);


#endif //__SCTP_EVENT_H
