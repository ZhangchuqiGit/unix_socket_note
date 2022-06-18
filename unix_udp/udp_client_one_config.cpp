
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

void udp_client_one_config()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int socket_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	err_sys(socket_fd, __LINE__, "socket()");

	struct sockaddr_in client_addr{}; // 配置
	/* ---- 配置 ip 地址 端口 信息 ---- */
	Sock_address_config((struct sockaddr *)&client_addr,
						AF_INET 		/* Address families*/ ,
						PORT_service	/* 数字 端口号 */ ,
						HOST_Service_IPv4/* 字符串 IP 地址 */ );

	/* do it all */
	udp_client_call(socket_fd, (struct sockaddr *)&client_addr, sizeof(client_addr));

	/* ---- close () ---- */
	retval=close(socket_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}
