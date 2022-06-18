
#include "zcq_header.h"

/* 心搏机制：周期信号轮询对端，无响应即不存活 */
void tcp_service_OOB_heartbeat_fork_Signal()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
	int retval; // 函数返回值
	int sock_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	err_sys(sock_fd, __LINE__, "socket()");

	/* ---- 允许地址的立即重用，端口不占用 ---- */
	/* ---- 允许端口的立即重用 ---- */
	int option = 1;
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   { SO_REUSEADDR/*地址立即重用*/, SO_REUSEPORT/*端口立即重用*/ },
			   &option, sizeof(option) );

#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	ret_value=setsockopt(socket_listen_fd, SOL_SOCKET/*通用选项*/,
						 SO_RCVBUF/* 接收缓冲区 */,
						 &receiver_buf, sizeof(receiver_buf) );
	err_sys(ret_value, __LINE__,"setsockopt()");
#endif

	/* ---- 配置 ip 地址 端口 信息 ---- */
	struct sockaddr_in servaddr{};
	bzero(&servaddr, sizeof(servaddr)); // 清 0
	servaddr.sin_family		=AF_INET; // IPv4
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY); // Internet address
	servaddr.sin_port		=htons(PORT_service); // 端口号

	/* ---- bind () ---- */
	retval=bind(sock_fd, (struct sockaddr *)(&servaddr), sizeof(servaddr) );
	err_sys(retval, __LINE__, "bind()");

	/* ---- listen () ---- */
	retval=listen(sock_fd, MAX_listen/*最大连接数*/ );
	err_sys(retval, __LINE__, "listen()");

/*******************************************************************/
	/* ---- Signal () ---- */
	// SIGCHLD : 父进程 捕捉 子进程 的状态信号。
	Signal_fork(SIGCHLD);	/* must call waitpid() */
/*******************************************************************/

	int accept_fd; // 新(连接成功)套接字描述符
	while (true)
	{
		/* ---- accept () ---- */
		accept_fd = Accept(sock_fd, (struct sockaddr *)(&servaddr));

		/* ---- 创建 服务 子进程 ---- */
			if (fork() == 0) {		// child process
			err_sys(close(sock_fd), __LINE__, "close()");

			/** 心搏机制：周期信号轮询对端，无响应即不存活 */
			heartbeat_service(accept_fd, 1, 5);

			tcp_service_echo(accept_fd);    // process the request
			exit(0);
		}
		err_sys(close(accept_fd), __LINE__, "close()");
	}

	/* ---- close () ---- */
	retval=close(sock_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}

