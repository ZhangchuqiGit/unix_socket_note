
#include "udp_client_call.h"

/****************************** 请求-应答 *********************************/
/* 	在信号处理函数中进行非局部转移
 	增加 可靠性： 	1、超时和重传：处理丢失数据；
 				 	2、序列号：验证应答 是否匹配。    */

/* 一般函数的跳转函数：
 跳转缓冲区: static jmp_buf env;
 int setjmp (jmp_buf env); // 设置跳转点，注册 env，返回 longjmp()中的 val。
 void longjmp (jmp_buf env, int val);	// 跳到 setjmp()
---------------------------------------------------------------------------
 #include <setjmp.h> 专为信号设置的跳转函数：在信号处理函数中进行非局部转移
 跳转缓冲区: static jmp_buf env;
 int sigsetjmp (sigjmp_buf env, int savemask); // 设置跳转点，注册 env
 void siglongjmp (sigjmp_buf env, int val); // 跳到 sigsetjmp()
 返回值：若直接调用 sigsetjmp()则返回 0，
 		若从 siglongjmp()调用， sigsetjmp()则返回 val
---------------------------------------------------------------------------
 如果 savemask 非0，则 sigsetjmp()在 env 中保存进程的当前信号屏蔽字。
 调用 siglongjmp 时，则 siglongjmp()从 env 恢复保存的信号屏蔽字。   */

static sigjmp_buf env; // 跳转缓冲区

void recvfrom_alarm_sigjmp_rtt(int signum) // 信号处理函数
{
	std::cout.flush();
	/* just interrupt the recvfrom() */
	std::cerr << "recvfrom_alarm_sigjmp_rtt(): 信号中断" << std::endl;
	std::cerr.flush();
	siglongjmp(env, 1); // 跳到 sigsetjmp()：返回 val
}

/* 	在信号处理函数中进行非局部转移
 	增加 可靠性： 	1、超时和重传：处理丢失数据；
 				 	2、序列号：验证应答 是否匹配。    */
void udp_client_call_rtt(const int & sock_fd,
						 const struct sockaddr * serv_addr,
						 const socklen_t & sizeof_addr,
						 const time_t &seconds) {
	_debug_log_info("udp_client_call_rtt()")
#ifdef    IP_PKTINFO
	const int on = 1;
	err_ret(setsockopt(sock_fd, IPPROTO_IP,
					   IP_PKTINFO/*报文(UDP)相关控制信息*/, &on, sizeof(on)),
			__FILE__, __func__, __LINE__, "setsockopt()");
#endif
/***************************** 注册信号 ******************************/
	/* 注册信号; 返回旧信号处理程序 */
//	sighandler_t sig_old_handler = Signal(SIGALRM, recvfrom_alarm);
	Signal(SIGALRM, recvfrom_alarm_sigjmp_rtt);
/********************************************************************/
	ssize_t retval; // 函数返回值
	char sendline[MAXLINE], recvline[MAXLINE], interface_name[IFNAMSIZ] = {'\0'};
	socklen_t sizeof_addr_recv;
	struct in_addr in_zero{};
	bzero(&in_zero, sizeof(struct in_addr)); /* IPv4 address 0.0.0.0 */
	struct in_pktinfo pktinfo{};    // Structure used for IP_PKTINFO
/**----------------------------------------------------------**/
	struct rtt_info rttinfo{};        // 超时和重传：处理丢失数据
	struct seq_ts sendhdr{0, 0}, recvhdr{}; // 序列号：验证应答
	int rttinit = 0;
	rtt_debug_flag = 1;    // 打印调试信息
	while (true) {
		memset(sendline, 0, sizeof(sendline));
		memset(recvline, 0, sizeof(recvline));
		sizeof_addr_recv = sizeof_addr;
		std::cerr.flush();
		std::cout << "you input:\t";
		std::cout.flush();
		retval = read(STDIN_FILENO, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "read()");
		if (retval == 0) break;
		if (retval > 1 && sendline[retval - 1] == '\n')
			sendline[retval - 1] = '\0';
/**----------------------------------------------------------**/
		if (rttinit == 0) {
			rtt_init(&rttinfo);    // 发送新的分组
			rttinit = 1;
		}

		sendhdr.seq++; // 序列号
send_again:
		std::cout << "序列号: " << sendhdr.seq << std::endl;
		sendhdr.ts = rtt_ts(&rttinfo); // 返回当前时间戳
		Sendmsg_seq_ts(sock_fd, &sendhdr, sizeof(sendhdr),
					   sendline, strlen(sendline),
					   (struct sockaddr *) serv_addr,
					   sizeof_addr_recv);

		retval = rtt_start(&rttinfo);

		alarm(retval);    /* calc timeout value & start timer */

		if (sigsetjmp(env, 1) != 0) { /* 发送 或 接收 超时，重新传输 */
			if (rtt_timeout(&rttinfo) < 0) {
				errno = ETIMEDOUT; // Connection timed out
				err_msg(-1, __LINE__,
						"no response from server, giving up!");
				rttinit = 0; // 超时和重传 达到 ？次，放弃 该分组
			} else {
				/* 收到应答，停止计时器，自动修改重传时间。*/
				rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);
				std::cout << "\ttimes retransmitted, again: "
						  << rttinfo.times << std::endl;
				rtt_debug(&rttinfo); // 打印调试信息
				goto send_again; // 重传
			}
		}
		else {
			do {
				retval = Recvmsg_seq_ts(sock_fd, &recvhdr, sizeof(recvhdr),
										recvline, sizeof(recvline),
										(struct sockaddr *) serv_addr,
										sizeof_addr_recv,
										&pktinfo);
			} while (retval < (ssize_t) sizeof(struct seq_ts) ||
					 recvhdr.seq != sendhdr.seq);
		}
		alarm(0); // 若 发送 或 接收 超时，重新传输

		/* 收到应答，停止计时器，自动修改重传时间。*/
		rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);
		rtt_debug(&rttinfo); // 打印调试信息
/**----------------------------------------------------------**/
		std::cout << retval - sizeof(struct seq_ts) << " byte datagram from "
				  << get_addr_port_ntop_r(serv_addr, sizeof_addr_recv);
		if (memcmp(&pktinfo.ipi_addr, &in_zero, sizeof(in_zero)) != 0) {
			std::cout << "  to "
					  << addr_net_to_ptr_r(AF_INET, &pktinfo.ipi_addr)
					  << "\nRouting destination address "
					  << addr_net_to_ptr_r(AF_INET, &pktinfo.ipi_spec_dst)
					  << std::endl;
		} else std::cout << std::endl;
		if (pktinfo.ipi_ifindex > 0 &&
			if_indextoname(pktinfo.ipi_ifindex, interface_name) != nullptr) {
			std::cout << "recv i/f = " << interface_name << std::endl;
		}
/**----------------------------------------------------------**/
		if (recvhdr.seq == sendhdr.seq)
			std::cout << "recvline: " << recvline << std::endl;
	}
}
