
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
	tcp_client_one_01();
}
#endif

void tcp_client_one()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int socket_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	err_sys(socket_fd, __LINE__, "socket()");

	/* ---- 配置 ---- */
//	char ip[]="10.23.21.248"; // 本机 ip
	struct sockaddr_in client_address{};
	bzero(&client_address, sizeof(client_address)); // 清 0
	client_address.sin_family		=AF_INET; // IPv4
	client_address.sin_addr.s_addr	=inet_addr(HOST_Service_IPv4); // Internet address
	client_address.sin_port			=htons(PORT_service); // 端口号

	/* ---- connect () ---- */
#if 0
	retval=connect(socket_fd, (struct sockaddr *)(&client_address),
				   sizeof(client_address) );
#else
	/* 使用 非阻塞 I/O 复用 select() 为 connect() 设置超时 */
	retval= connect_nonb_timeo(socket_fd, (struct sockaddr *)(&client_address),
							   sizeof(client_address), 3);
#endif
	err_sys(retval, __LINE__, "connect()");

	/* 获取 与某个套接字 关联的 本地/对方 协议地址 */
	getIP_addr_port(socket_fd);

	/* do it all */
	tcp_client_call(stdin, socket_fd);		/* do it all */

	/* ---- close () ---- */
	retval=close(socket_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}
