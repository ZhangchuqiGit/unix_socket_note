//
// Created by zcq on 2021/4/23.
//

#include "rtt.h"


// 原子修改类型，不会在操作过程中到达信号
volatile int rtt_debug_flag = 0; /* debug flag; can be set by caller */

/* RTO = srtt + 4 * rttvar
   RTO：重传时间，从发出请求到下一次重新发出请求的时间间隔。   */
#define	RTT_RTO(ptr)   ( (ptr->srtt) + (4.0 * (ptr->rttvar)) )

/* retransmit timeout, RTO: RTT_RXTMIN ~ RTT_RXTMAX */
float rtt_rto_min_max(float rto)
{
	if (rto < RTT_RTOMIN) rto = RTT_RTOMIN;
	else if (rto > RTT_RTOMAX) rto = RTT_RTOMAX;
	return (rto);
}

/* 发送新的分组 要 重置请求次数 */
void rtt_newpack_ct(struct rtt_info *ptr)
{
	ptr->times = 0;
}

/* 发送新的分组 */
void rtt_init(struct rtt_info *ptr)
{
	ptr->rtt    = 0; 		// 往返时间
	ptr->srtt   = 0; 		// 平滑化 RTT 估算因子
	ptr->rttvar = 0.5; 		// 平滑化 平均偏差 估算因子
	/* first RTO = srtt + 4 * rttvar = 2 seconds */
	ptr->rto = rtt_rto_min_max(RTT_RTO(ptr));

	rtt_newpack_ct(ptr); // 发送新的分组 要 重置请求次数

	struct timeval tv{};
	err_ret(gettimeofday(&tv, nullptr),
			__FILE__, __func__, __LINE__, "gettimeofday()");
	ptr->timebase = tv.tv_sec; // sec since 1/1/1970 at start
}

/* 返回当前时间戳(32位整数)，自调用 rtt_init()以来以毫秒为单位。 */
uint32_t rtt_ts(const struct rtt_info *ptr)
{
	struct timeval tv{};
	err_ret(gettimeofday(&tv, nullptr),
			__FILE__, __func__, __LINE__, "gettimeofday()");
	uint32_t ts = (tv.tv_sec - ptr->timebase) * 1000 +
				  (tv.tv_usec/*微秒*/ / 1000);
	return (ts); // 毫秒
}

int rtt_start(struct rtt_info *ptr)
{
	/* return value can be used as: alarm(rtt_start(&foo)) */
	return (int)(ptr->rto + (float)0.5);
}

/* 	收到应答，停止计时器，自动修改重传时间。
 	计算 往返时间 RTT，然后更新RTT的估算值 及其 平均偏差。
 	应在 关闭计时器 alarm(0)后 或 发生超时后 立即调用此函数。 */
void rtt_stop(struct rtt_info *ptr, uint32_t ms)
{
	ptr->rtt = ms / (float)1000.0; // measured RTT in seconds
	double delta = ptr->rtt - ptr->srtt;
	ptr->srtt += (float) (delta / 8); // 增益 g = 1/8
	if (delta < 0.0) delta = -delta; // |delta|
	ptr->rttvar += (float) ((delta - ptr->rttvar) / 4); // 增益 h = 1/4
	ptr->rto = rtt_rto_min_max(RTT_RTO(ptr));
}

int rtt_timeout(struct rtt_info *ptr)
{
	ptr->rto *= 2; // 重传时间 加倍
	if (++ptr->times > RTT_timesMAX) return(-1);
	return(0);
}

/* 打印调试信息 */
void rtt_debug(struct rtt_info *ptr)
{
	if (rtt_debug_flag == 0) return;
	fflush(stdout);
	fprintf(stderr, "\nRTT = %.3f, srtt = %.3f, rttvar = %.3f, RTO = %.3f\n",
			ptr->rtt, ptr->srtt, ptr->rttvar, ptr->rto);
	fflush(stderr);
}


