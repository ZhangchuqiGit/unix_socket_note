//
// Created by zcq on 2021/3/24.
//

#ifndef __SCTP_WRAP_H
#define __SCTP_WRAP_H

#include "zcq_header.h"

#include <iostream>
//using namespace std;

/******************************************************************************/

/* 心博控制 */
void heartbeat_action(const int &sock_fd,
					  struct sockaddr *to_addr,
					  const socklen_t &to_addr_len,
					  const u_int value);

/* sctp IP地址 转换 关联ID信息 */
sctp_assoc_t sctp_IPaddr_to_assocID(const int &sock_fd,
									struct sockaddr *to_addr,
									const socklen_t &to_addr_len,
									const sctp_assoc_t &sinfo_assoc_id);

/******************************************************************************/

int sctp_get_no_strms(const int &sock_fd, struct sockaddr *to_addr,
					  const socklen_t &to_addr_len,
					  struct sctp_sndrcvinfo sctp_SndRcv_info);

void struct_print (const int &sock_fd, const struct sockaddr *from,
				   const socklen_t &fromlen, const struct sctp_sndrcvinfo *sinfo,
				   const int &msg_flags);

#endif //__SCTP_WRAP_H



/* ---------- https://blog.csdn.net/x13262608581/article/details/109144584
IPPROTO_SCTP	(SCTP 选项)
SCTP_ADAPTION_LAYER			*	*	适配层指示							sctp_setadaption{}
SCTP_ASSOCINFO				*	*	检查并设置关联信息					sctp_assocparams{}
SCTP_AUTOCLOSE				*	*	自动关闭								int
SCTP_DEFAULT_SEND_PARAM		*	*	(发送大量消息)相同默认参数				sctp_sndrcvinfo{}
SCTP_DISABLE_FRAGMENTS		*	*	发送端禁止分片					*	int
SCTP_EVENTS					*	*	感兴趣事件通知						sctp_event_subscribe{}
SCTP_GET_PEER_ADDR_INFO		*		获取某个给定对端地址的相关信息			sctp_paddrinfo{}
SCTP_I_WANT_MAPPED_V4_ADDR	*	*	用于AF_INET6套接字开启或禁止		*	int
IPV4映射地址(默认为开启)
SCTP_INITMSG				*	*	默认INIT参数							sctp_initmsg{}
SCTP_MAXBURST				*	*	用于分组发送的最大猝发大小				int
SCTP_MAXSEG					*	*	允许应用获取或设置用于分片最大片段		int
SCTP_NODELAY				*	*	禁止Nagle算法					*	int
SCTP_PEER_ADDR_PARAMS		*	*	允许应用获取/设置某关联对端地址参数		sctp_paddrparams{}
SCTP_PRIMARY_ADDR			*	*	主目的地址							sctp_setprim{}
SCTP_RTOINFO				*	*	RTO消息								sctp_rtoinfo{}
SCTP_SET_PEER_PRIMARY_ADDR		*	请求对端把指定本地地址作为它主目的地址 	sctp_setpeerprim{}
SCTP_STATUS					*		获取关联状态							sctp_status{}

 */