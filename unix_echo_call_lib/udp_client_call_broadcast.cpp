//
// Created by zcq on 2021/4/17.
//

#include "udp_client_call.h"

#define UDP_Call_broadcast   2

void udp_client_call_broadcast(const int & sock_fd,
							   const struct sockaddr * serv_addr,
							   const socklen_t & sizeof_addr,
							   const time_t &seconds)
{
#if UDP_Call_broadcast==0
	/* 使用 pselect 阻塞和解阻塞信号 */
	udp_client_call_broadcast_pselect(sock_fd, serv_addr, sizeof_addr, seconds);
#elif UDP_Call_broadcast==1
	/* 在信号处理函数中进行非局部转移 */
	udp_client_call_broadcast_sigjmp(sock_fd, serv_addr, sizeof_addr, seconds);
#elif UDP_Call_broadcast==2
	/* 使用从信号处理函数到主控函数的管道作为 IPC */
	udp_client_call_broadcast_IPC(sock_fd, serv_addr, sizeof_addr, seconds);
#endif
}

/*************************************************************************/
/*************************************************************************/

#define MODE_pselect   0

void recvfrom_alarm(int signum) // 信号处理函数
{
	std::cout.flush();
	/* just interrupt the recvfrom() */
	std::cerr << "recvfrom_alarm(): 信号中断" << std::endl;
}

/* 避免由 SIGALRM 竞争  使用 pselect 阻塞和解阻塞信号 */
void udp_client_call_broadcast_pselect(const int & sock_fd,
									   const struct sockaddr * serv_addr,
									   const socklen_t & sizeof_addr,
									   const time_t &seconds)
{
	char servaddr[sizeof_addr];
	memcpy(servaddr, serv_addr, sizeof_addr);
	auto *servaddr_ptr = (struct sockaddr *)servaddr;

	int retval; // 函数返回值
	char sendline[MAXLINE] = {'\0'};
	char recvline[MAXLINE] = {'\0'};
	socklen_t sizeof_addr_tmp = sizeof_addr;

/************************** 设置 I/O 非阻塞 **************************/
	_debug_log_info("设置 I/O 非阻塞")
	retval = Fcntl(sock_fd, F_GETFL, 0); 				// 获取 I/O 状态
	Fcntl(sock_fd, F_SETFL, retval | O_NONBLOCK); 		// 设置 I/O 非阻塞

/***************************** 注册信号 ******************************/
#if  MODE_pselect
	sigset_t sigset_empty; // 清空的 信号组
	/* 清除 信号组 中的所有信号 */
	err_sys(sigemptyset(&sigset_empty), __LINE__, "sigemptyset()");
#else
	sigset_t sigset_alrm; // 要阻止，取消阻止或等待的 信号组
	/* 清除 信号组 中的所有信号 */
	err_sys(sigemptyset(&sigset_alrm), __LINE__, "sigemptyset()");
	/* 将 SIGALRM 信号 添加到 sigset_alrm 信号组 */
	retval = sigaddset(&sigset_alrm, SIGALRM);
	err_sys(retval, __LINE__, "sigaddset()");
#endif
	/* 注册信号; 返回旧信号处理程序 */
//	sighandler_t sig_old_handler = Signal(SIGALRM, recvfrom_alarm);
	Signal(SIGALRM, recvfrom_alarm);

/********************************************************************/
	while (true) {
		std::cout << "you input:\t";
		std::cout.flush();
		retval = read(STDIN_FILENO, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "read()");
		if (retval == 0) break;

		retval = sendto(sock_fd, sendline, strlen(sendline), 0,
						(struct sockaddr *) serv_addr, sizeof_addr);
		err_sys(retval, __LINE__, "sendto()");
		if (sendline[retval - 1] == '\n') sendline[retval - 1] = '\0';
		std::cout << "sendto:\t" << sendline << std::endl;
		memset(sendline, 0, sizeof(sendline));

#if  MODE_pselect
		/* 单线程: 检测或改变信号屏蔽字 */
		retval = sigprocmask(SIG_BLOCK/*屏蔽信号*/, &sigset_alrm, nullptr);
		err_sys(retval, __LINE__, "sigprocmask()");
		fd_set read_set; // 鉴别事件的标志位集合
		FD_ZERO(&read_set); // 初始化集合:所有位均关闭
		struct timespec tv{};
		tv.tv_sec = seconds;
		tv.tv_nsec = 0;
#endif
		alarm(seconds);
		_debug_log_info("alarm(3)")
		while (true) {
			_debug_log_info("while()")
#if  MODE_pselect
			FD_SET(sock_fd, &read_set); // 打开 sock_fd 位
			retval = pselect(sock_fd + 1,
							 &read_set, nullptr, nullptr,
							 (seconds >= 0) ? &tv : nullptr,
							 &sigset_empty/*信号掩码*/ );
			if(retval < 0) {
				_debug_log_info("retval < 0")
				if (errno == EINTR) break; // 系统调用中断
				err_sys(retval, __LINE__, "fd_read_pselect_timeo()");
			}
#else
			/* 设置超时: pselect() 等待描述符变为可读
			   信号掩码: 若收到该信号，中断返回 0；否则返回 -1(error)  */
			retval = fd_read_pselect_timeo(sock_fd, sigset_alrm/*信号掩码*/,
										   seconds);
#endif
			if (retval == 0) {
				std::cerr << "fd_read_pselect_timeo() timeout" << std::endl;
				break;
			}
			if (retval != 1) err_sys(-1, __LINE__,
									 "pselect() return %d", retval);
			_debug_log_info("retval == 1")
			retval = recvfrom(sock_fd, recvline, sizeof(recvline), 0,
							  (struct sockaddr *) servaddr_ptr,
							  &sizeof_addr_tmp);
			if(retval < 0) {
				if (errno == EINTR) break; // 系统调用中断
				err_sys(retval, __LINE__, "recvfrom()");
			}
			if (recvline[retval - 1] == '\n') recvline[retval - 1] = '\0';
			std::cout << "[ "
					  << get_addr_port_ntop((struct sockaddr *)&servaddr_ptr,
											sizeof_addr_tmp)
					  << " ]: " << recvline << std::endl;
			memset(recvline, '\0', sizeof(recvline));
			sizeof_addr_tmp = sizeof_addr;
		}
	}
}

/*************************************************************************/
/*************************************************************************/

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

void recvfrom_alarm_sigjmp(int signum) // 信号处理函数
{
	std::cout.flush();
	/* just interrupt the recvfrom() */
	std::cerr << std::endl << "recvfrom_alarm_sigjmp(): 信号中断" << std::endl;
	siglongjmp(env, 1); // 跳到 sigsetjmp()：返回 val
}

/* 在信号处理函数中进行非局部转移 */
void udp_client_call_broadcast_sigjmp(const int & sock_fd,
									  const struct sockaddr * serv_addr,
									  const socklen_t & sizeof_addr,
									  const time_t &seconds)
{
	char servaddr[sizeof_addr];
	memcpy(servaddr, serv_addr, sizeof_addr);
	auto *servaddr_ptr = (struct sockaddr *)servaddr;

	int retval; // 函数返回值
	char sendline[MAXLINE] = {'\0'};
	char recvline[MAXLINE] = {'\0'};
	socklen_t sizeof_addr_tmp = sizeof_addr;

/***************************** 注册信号 ******************************/
	/* 注册信号; 返回旧信号处理程序 */
//	sighandler_t sig_old_handler = Signal(SIGALRM, recvfrom_alarm);
	Signal(SIGALRM, recvfrom_alarm_sigjmp);

/********************************************************************/
	while (true) {
		std::cout.flush();
		std::cout << "you input:\t";
		std::cout.flush();
		retval = read(STDIN_FILENO, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "read()");
		if (retval == 0) break;

		retval = sendto(sock_fd, sendline, strlen(sendline), 0,
						(struct sockaddr *) serv_addr, sizeof_addr);
		err_sys(retval, __LINE__, "sendto()");
		if (sendline[retval - 1] == '\n') sendline[retval - 1] = '\0';
		std::cout << "sendto:\t" << sendline << std::endl;
		memset(sendline, 0, sizeof(sendline));

		alarm(seconds);
		_debug_log_info("alarm(3)")
		while (true) {
			_debug_log_info("while()")

			if (sigsetjmp(env, 1) != 0) break;

			retval = recvfrom(sock_fd, recvline, sizeof(recvline), 0,
							  (struct sockaddr *) servaddr_ptr,
							  &sizeof_addr_tmp);
			if(retval < 0) {
				if (errno == EINTR) break; // 系统调用中断
				err_sys(retval, __LINE__, "recvfrom()");
			}
			if (recvline[retval - 1] == '\n') recvline[retval - 1] = '\0';
			std::cout << "[ "
					  << get_addr_port_ntop_r((struct sockaddr *)&servaddr_ptr,
											  sizeof_addr_tmp)
					  << " ]: " << recvline << std::endl;
			memset(recvline, 0, sizeof(recvline));
			sizeof_addr_tmp = sizeof_addr;
		}
	}
}


/*************************************************************************/
/*************************************************************************/

// 原子修改类型，不会在操作过程中到达信号
volatile static int pipe_fd[2] = {-1}; // 管道描述符

void recvfrom_alarm_IPC(int signum) // 信号处理函数
{
	std::cout.flush();
	/* just interrupt the recvfrom() */
	std::cerr << std::endl << "recvfrom_alarm(): 信号中断" << std::endl;
	Write((int)pipe_fd[1], "a", 1);
	/* write one null byte to pipe */
}

/* 避免由 SIGALRM 竞争  使用从信号处理函数到主控函数的管道作为 IPC */
void udp_client_call_broadcast_IPC(const int & sock_fd,
								   const struct sockaddr * serv_addr,
								   const socklen_t & sizeof_addr,
								   const time_t &seconds)
{
	char servaddr[sizeof_addr];
	memcpy(servaddr, serv_addr, sizeof_addr);
	auto servaddr_ptr = (struct sockaddr *)servaddr;

	int retval; // 函数返回值
	char sendline[MAXLINE] = {'\0'};
	char recvline[MAXLINE] = {'\0'};
	socklen_t sizeof_addr_tmp = sizeof_addr;

/************** 管道 pipe 具有血缘关系(公共祖先)的进程间通信 ************/
	err_sys(pipe((int *)pipe_fd), __LINE__, "pipe()"); // 创建管道

/************************** 设置 I/O 非阻塞 **************************/
	_debug_log_info("设置 I/O 非阻塞")
	retval = Fcntl(sock_fd, F_GETFL, 0); 				// 获取 I/O 状态
	Fcntl(sock_fd, F_SETFL, retval | O_NONBLOCK); 		// 设置 I/O 非阻塞

/***************************** 注册信号 ******************************/
	/* 注册信号; 返回旧信号处理程序 */
//	sighandler_t sig_old_handler = Signal(SIGALRM, recvfrom_alarm);
	Signal(SIGALRM, recvfrom_alarm_IPC);

/********************************************************************/
	fd_set readset; // 鉴别事件的标志位集合
	int max_fdfp = std::max((int)pipe_fd[0], sock_fd);
	FD_ZERO(&readset); // 初始化集合:所有位均关闭
	while (true)
	{
		std::cout.flush();
		std::cout << "you input:\t";
		std::cout.flush();
		retval = read(STDIN_FILENO, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "read()");
		if (retval == 0) break;

		retval = sendto(sock_fd, sendline, strlen(sendline), 0,
						(struct sockaddr *) serv_addr, sizeof_addr);
		err_sys(retval, __LINE__, "sendto()");
		if (sendline[retval - 1] == '\n') sendline[retval - 1] = '\0';
		std::cout << "sendto:\t" << sendline << std::endl;
		memset(sendline, 0, sizeof(sendline));

		alarm(seconds);
		_debug_log_info("alarm(3)")
		while (true) {
			_debug_log_info("while()")

			FD_SET(pipe_fd[0], &readset); // 打开 file_fp 位
			FD_SET(sock_fd, &readset); // 打开 sock_fd 位

			/* select() 设置或者检查存放在数据结构 fd_set 中的标志位来鉴别事件 */
			retval=select(max_fdfp+1, &readset,
						  nullptr, nullptr, nullptr);
			if (retval == 0)
				std::cerr << "fd_read_pselect_timeo() timeout" << std::endl;

			if(retval < 0) {
				_debug_log_info("retval < 0")
				if (errno == EINTR) continue; // 系统调用中断
				err_sys(retval, __LINE__, "select()");
			}

			if (FD_ISSET(sock_fd, &readset)) {
				retval = recvfrom(sock_fd, recvline, sizeof(recvline), 0,
								  (struct sockaddr *) servaddr_ptr,
								  &sizeof_addr_tmp);
				if(retval < 0) {
					if (errno == EINTR) break; // 系统调用中断
					err_sys(retval, __LINE__, "recvfrom()");
				}
				if (recvline[retval - 1] == '\n') recvline[retval - 1] = '\0';
				std::cout << "[ "
						  << get_addr_port_ntop_r((struct sockaddr *)&servaddr_ptr,
												  sizeof_addr_tmp)
						  << " ]: " << recvline << std::endl;
				memset(recvline, 0, sizeof(recvline));
			}

			if (FD_ISSET(pipe_fd[0], &readset)) {
				Read((int)pipe_fd[0], &retval, 1); // timer expired
				std::cout << "计时器到期: " << retval << std::endl;
				break;
			}
			sizeof_addr_tmp = sizeof_addr;
		}
	}
}


