
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
	udp_client_broadcast();
}
#endif

void udp_client_broadcast()
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
	client_addr.sin_addr.s_addr = inet_addr(ADDR_Broadcast); // Internet address
#else	// Internet address
	retval=inet_pton(AF_INET, ADDR_Broadcast, &client_addr.sin_addr);
	/* errno set */
	err_sys(retval, __LINE__, "inet_pton error for %s", ADDR_Broadcast);
	/* errno not set */
	if (retval == 0)
		err_quit(retval, __LINE__, "inet_pton errno not set %s", ADDR_Broadcast);
#endif

	/* ---- 启用广播模式 ---- 用于发送广播消息 */
	const int option = 1;
	retval=setsockopt(socket_fd, SOL_SOCKET/*通用选项*/,
					  SO_BROADCAST/*广播模式*/, &option, sizeof(option) );
	err_sys(retval, __LINE__,"setsockopt()");

	udp_client_call_broadcast(socket_fd,
							  (struct sockaddr *) &client_addr,
							  sizeof(struct sockaddr_in));

	/* ---- close () ---- */
	retval=close(socket_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}
