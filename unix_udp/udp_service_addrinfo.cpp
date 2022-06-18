
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
	udp_service_addrinfo();
}
#endif


void udp_service_addrinfo()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int ret_value; // 函数返回值
	int socket_fd; // 套接字描述符
//	struct sockaddr_in client_address{}; // 配置
	struct sockaddr_un	sa{};
	socklen_t sa_len;

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	/* ---- 配置 ip 地址 端口 信息 ---- */
	/* ---- bind () ---- */
	socket_fd = udp_socket_bind_config((struct sockaddr *)&sa, &sa_len);

	/* 信息传递 */
	udp_service_echo(socket_fd, (struct sockaddr *)&sa, sa_len );

	/* ---- close () ---- */
	ret_value=close(socket_fd);
	err_sys(ret_value, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}

