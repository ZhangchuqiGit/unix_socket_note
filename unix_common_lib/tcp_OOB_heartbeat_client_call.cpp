//
// Created by zcq on 2021/5/8.
//

#include "tcp_OOB_heartbeat.h"

static int peer_fd;
static int nsec;		/* #seconds betweeen each alarm */
static int maxnprobes;	/* #probes w/no response before quit */
static int nprobes;		/* #probes since last server response */

static void sig_urg(int signo) // 对端响应
{
	char c;
	if (recv(peer_fd, &c, 1, MSG_OOB/*处理带外数据*/) < 0) {
		if (errno != EWOULDBLOCK)
			err_sys(-1, __FILE__, __func__, __LINE__, "recv()");
	}
//	std::cerr << "处理带外数据" << std::endl;
	nprobes = 0;		/* reset counter */
}

static void sig_alrm(int signo) // 周期信号轮询对端
{
	if (++nprobes > maxnprobes) {
		std::cerr << "server is unreachable" << std::endl;
		exit(0);
	}
	Send(peer_fd, "1", 1, MSG_OOB/*处理带外数据*/);
	alarm(nsec);
}

/* 心搏机制：周期信号轮询对端，无响应即不存活 */
void heartbeat_client(const int &sock_fd, const int &nsec_arg, const int &max_arg)
{
	peer_fd = sock_fd;		/* set globals for signal handlers */
	if ((nsec = nsec_arg) < 1) nsec = 1;
	if ((maxnprobes = max_arg) < nsec) maxnprobes = nsec;
	nprobes = 0;

	/** 当有新的紧急指针到达时，接受进程被通知到。首先，内核会给接受套接字的属主进程发送 SIGURG 信号，
    前提是接受进程调用了 fcntl()或者 ioctl()为这个套接字建立了属主，*/
	Signal(SIGURG, sig_urg); // 注册信号 对端响应
	Fcntl(peer_fd, F_SETOWN, getpid()); //设置所有者（接收SIGIO的进程）

	Signal(SIGALRM, sig_alrm); // 注册信号 周期信号轮询对端
	alarm(nsec);
}
