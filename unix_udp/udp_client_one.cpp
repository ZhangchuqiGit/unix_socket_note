
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
	udp_client_one();
}
#endif

void udp_client_one()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int socket_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	err_sys(socket_fd, __LINE__, "socket()");

	/* ---- 配置 ip 地址 端口 信息 ---- */
//	char ip[]="10.23.21.248"; // 本机 ip
	struct sockaddr_in client_addr{};
	bzero(&client_addr, sizeof(client_addr)); // 清 0
	client_addr.sin_family	= AF_INET; // IPv4
	client_addr.sin_port	= htons(PORT_service); // 端口号
#if 0
	client_address.sin_addr.s_addr = inet_addr(Service_IPv4_host); // Internet address
#else	// Internet address
	retval=inet_pton(AF_INET, HOST_Service_IPv4, &client_addr.sin_addr);
	/* errno set */
	err_sys(retval, __LINE__, "inet_pton error for %s", HOST_Service_IPv4);
	/* errno not set */
	if (retval == 0)
		err_quit(retval, __LINE__, "inet_pton errno not set %s", HOST_Service_IPv4);
#endif

	/* do it all */
	udp_client_call(socket_fd, (struct sockaddr *)&client_addr, sizeof(client_addr));

	/* ---- close () ---- */
	retval=close(socket_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}
