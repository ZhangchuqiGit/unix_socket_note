
#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
//#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read

#include <iostream>
//using namespace std;


#if 0
int main(int argc, char **argv) {
	udp_service_addrinfo_daemon();
}
#endif


void udp_service_addrinfo_daemon_syslog()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int ret_value; // 函数返回值
	int socket_fd; // 套接字描述符
//	struct sockaddr_in client_address{}; // 配置
	struct sockaddr	sa{};
//	socklen_t sa_len;

	/** 守护进程 创建步骤：失败返回负值；成功则父进程退出，子进程继续执行 */
	daemon_init();

	/** 生成日志消息 */
	openlog("ZCQ-UNIX网络编程-套接字", LOG_CONS | LOG_PID,
			LOG_LOCAL1 /* 本地策略 /var/log/local1.log */ );
	syslog(LOG_LOCAL1 | LOG_INFO /* /var/log/messages */,
		   "======================================================\n");
//	closelog();

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	/* ---- 配置 ip 地址 端口 信息 ---- */
	/* ---- bind () ---- */
	socket_fd = udp_socket_bind_config(&sa);

	/* 信息传递 */
	udp_service_echo(socket_fd, (struct sockaddr *)&sa, sizeof(sa) );

	/* ---- close () ---- */
	ret_value=close(socket_fd);
	err_sys(ret_value, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;

	syslog(LOG_LOCAL1 | LOG_INFO /* /var/log/messages */,
		   "关闭日志消息");

	/** 关闭日志消息 */
	closelog();
}

