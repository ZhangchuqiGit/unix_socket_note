//
// Created by zcq on 2021/4/23.
//

#ifndef __RTT_H
#define __RTT_H

#include "zcq_header.h"


#define	RTT_RTOMIN      2	/* min retransmit timeout value, in seconds */
#define	RTT_RTOMAX     60	/* max retransmit timeout value, in seconds */
#define	RTT_timesMAX 	3	/* max times to retransmit */

// 原子修改类型，不会在操作过程中到达信号
/* 打印调试信息 */
extern volatile int rtt_debug_flag; /* debug flag; can be set by caller */

/** 发送同一个分组
       客户                   服务器
     	|   					|
  ------|----> 请求(丢失)		|
 RTO   	|  						|
  ------|----> 请求				|
     	| 		  应答(丢失)<----|
     	|----> 请求				|
     	|  	            应答<----|

 RTT：往返时间。    偏差 delta = RTT - srtt ;           g, h: 增益。
 平滑化 RTT 估算值       srtt = srtt + g * delta ;
 平滑化 平均偏差         rttvar = rttvar + h * (|delta| - rttvar)
                        RTO = srtt + 4 * rttvar
 RTO：重传时间，从发出请求到下一次重新发出请求的时间间隔。        **/

struct rtt_info {	/** 超时和重传：处理丢失数据 **/
	float 	rtt;	/* most recent measured RTT, in seconds */
	float 	srtt;	/* smoothed RTT estimator, in seconds */
	float 	rttvar;	/* smoothed mean deviation, in seconds */
	float 	rto;	/* current RTO to use, in seconds */
	int 	times;	/* times retransmitted: 0, 1, 2, ... */
	uint32_t timebase;/* sec since 1/1/1970 at start */
};

/* 发送新的分组 */
void rtt_init(struct rtt_info *ptr);

/* 返回当前时间戳(32位整数)，自调用 rtt_init()以来以毫秒为单位。 */
uint32_t rtt_ts(const struct rtt_info *ptr);

/* 发送新的分组 要 重置请求次数 */
void rtt_newpack_ct(struct rtt_info *ptr);

int rtt_start(struct rtt_info *ptr);

/* 	收到应答，停止计时器，自动修改重传时间。
 	计算 往返时间 RTT，然后更新RTT的估算值 及其 平均偏差。
 	应在 关闭计时器 alarm(0)后 或 发生超时后 立即调用此函数。 */
void rtt_stop(struct rtt_info *ptr, uint32_t ms);

int rtt_timeout(struct rtt_info *ptr);

/* 打印调试信息 */
void rtt_debug(struct rtt_info *ptr);

#endif //__RTT_H
