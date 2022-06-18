
#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read

#include <iostream>
//using namespace std;


#if 0
int main(int argc, char **argv) {
	udp_client_config();
}
#endif

void udp_client_one_addrinfo()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int socket_fd; // 套接字描述符
	struct sockaddr_in client_addr{}; // 配置
//	struct sockaddr_un	sa{};
	socklen_t sa_len;
#if 1
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	/* ---- 配置 ip 地址 端口 信息 ---- */
	socket_fd = udp_socket_config((struct sockaddr *)&client_addr, &sa_len);
#else
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	/* ---- connect () ---- */
	/* ---- 配置 ip 地址 端口 信息 ---- */
	socket_fd = udp_socket_connect_config( (struct sockaddr *)&client_address, &sa_len);
#endif

	/* do it all */
	udp_client_call(socket_fd, (struct sockaddr *)&client_addr, sa_len);

	/* ---- close () ---- */
	retval=close(socket_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}
