
#include "zcq_header.h"

#include <sys/select.h>

#include <cstdlib>
#include <csignal>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
//#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read

#include <iostream>
//using namespace std;


/* ---- 创建 服务 子进程 ---- */
void service_fork_udp_tcp(const int &socket_listen_fd, const int &socket_accept_fd);

#if 0
int main(int argc, char **argv) {
	udp_tcp_service_fork_Signal_IO_select();
}
#endif

/** 父进程 直接使用 udp，创建 服务子进程 使用 tcp **/
void udp_tcp_service_fork_Signal_IO_select()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int ret_value; // 函数返回值
	int tcp_socket_listen_fd; // 套接字描述符
	int udp_socket_fd; // 套接字描述符

/******************************** tcp ************************************/

	/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
	tcp_socket_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	err_sys(tcp_socket_listen_fd, __LINE__, "socket()");

	const int option = 1;

	/* ---- 允许地址的立即重用，端口不占用 ---- */
	ret_value=setsockopt(tcp_socket_listen_fd, SOL_SOCKET, SO_REUSEADDR,
						 &option, sizeof(option) );
	err_sys(ret_value, __LINE__,"setsockopt()");

	/** ---- 配置 ip 地址 端口 信息 ---- **/
	struct sockaddr_in server_address{};
	bzero(&server_address, sizeof(server_address)); // 清 0
	server_address.sin_family		=AF_INET; // IPv4
	server_address.sin_addr.s_addr	=htonl(INADDR_ANY); // Internet address
	server_address.sin_port			=htons(PORT_service); // 端口号

	/* ---- bind () ---- */
	ret_value=bind(tcp_socket_listen_fd, (struct sockaddr *)(&server_address),
				   sizeof(server_address) );
	err_sys(ret_value, __LINE__, "bind()");

	/* ---- listen () ---- */
	ret_value=listen(tcp_socket_listen_fd, MAX_listen/*最大连接数*/ );
	err_sys(ret_value, __LINE__, "listen()");

/******************************** udp ************************************/

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	err_sys(udp_socket_fd, __LINE__, "socket()");

	/** ---- 配置 ip 地址 端口 信息 ---- **/

	/* ---- bind () ---- */
	ret_value=bind(udp_socket_fd, (struct sockaddr *)(&server_address),
				   sizeof(server_address) );
	err_sys(ret_value, __LINE__, "bind()");

/******************************************************/
	/* ---- Signal () ---- */
	// SIGCHLD : 父进程 捕捉 子进程 的状态信号。
	Signal_fork(SIGCHLD);	/* must call waitpid() */
/******************************************************/
	int socket_accept_fd; // 新(连接成功)套接字描述符
	int num_ready = 0; // 就绪描述符的数量
	fd_set read_set;
	FD_ZERO(&read_set);
	int max_fd = std::max(tcp_socket_listen_fd, udp_socket_fd) + 1;
	while (true)
	{
		FD_SET(tcp_socket_listen_fd, &read_set);
		FD_SET(udp_socket_fd, &read_set);
		if ( (num_ready = select(max_fd, &read_set, nullptr,
								 nullptr, nullptr)) < 0 ) {
			debug_line(__LINE__, (char *)__FUNCTION__);
			if (errno == EINTR)	continue; // 若被系统中断，自己重启
			else err_sys(num_ready, __LINE__, "select()");
		}
/*******************************************************************/
		/* new client connection */
		if (FD_ISSET(tcp_socket_listen_fd, &read_set)) {
			/* ---- accept () ---- */
			socket_accept_fd = Accept(tcp_socket_listen_fd);
//			struct sockaddr_in client_address{};
//			bzero(&client_address, sizeof(client_address)); // 清 0
//			socket_accept_fd = Accept_udp_tcp(tcp_socket_listen_fd,
//											  (struct sockaddr *)&client_address,
//											  sizeof(client_address) );
			/* ---- 创建 服务 子进程 ---- */
			service_fork_udp_tcp(tcp_socket_listen_fd, socket_accept_fd);
			debug_line(__LINE__, (char *)__FUNCTION__);
		}
/*******************************************************************/
		/* 父进程 直接使用 udp，创建 服务子进程 使用 tcp */
		if (FD_ISSET(udp_socket_fd, &read_set)) {
			/* 信息传递 */
			socklen_t len_size = sizeof(struct sockaddr_in);
			char buf[MAXLINE] = {'\0'};

			ret_value = recvfrom(udp_socket_fd, buf, sizeof(buf), 0,
								 (struct sockaddr *)&server_address, &len_size);
			err_sys(ret_value, __LINE__, "recvfrom()");

			if (if_run("time", buf)) {
				memset(buf, 0, sizeof(buf));
				time_t ticks;
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
				ticks = time(nullptr);
				snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
			} else {
				if (ret_value > 1 && buf[ret_value - 1] == '\n')
					buf[ret_value - 1] = '\0';
				std::cout << "recvfrom: " << buf << std::endl;
			}

			ret_value = sendto(udp_socket_fd, buf, strlen(buf), 0,
							   (struct sockaddr *)&server_address, len_size);
			err_sys(ret_value, __LINE__, "sendto()");
		}
	}

	/* ---- close () ---- */
	ret_value=close(udp_socket_fd);
	err_sys(ret_value, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}


/* ---- 创建 服务 子进程 ---- */
void service_fork_udp_tcp(const int &socket_listen_fd, const int &socket_accept_fd)
{
	int ret_value; // 函数返回值

	/* ---- 创建 子进程 ---- */
	pid_t child_pid = fork(); // 创建 子进程
	if (child_pid == -1) // 负值 : 创建子进程失败。
	{
		err_ret(child_pid, __LINE__, "创建子进程失败");
	}
	else if (child_pid == 0) // 0 : 当前是 子进程 运行。
	{
//		cout<<"当前是子进程运行，child ID: "<< getpid() <<endl;
		/* ---- close () ---- */
		ret_value = close(socket_listen_fd);
		err_sys(ret_value, __LINE__, "close()");

		/** 信息传递 **/
		tcp_service_echo(socket_accept_fd);    /* process the request */

		/* ---- close () ---- */
		ret_value = close(socket_accept_fd);
		err_sys(ret_value, __LINE__, "close()");

		exit(0);
	}
	else // 正值 : 当前是 父进程 运行。正值 为 子进程 的 进程 ID。
	{
		std::cout << "创建子进程 child ID: " << child_pid << std::endl;

		/* ---- close () ---- */
		ret_value = close(socket_accept_fd);
		err_sys(ret_value, __LINE__, "close()");
	}
}
