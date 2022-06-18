
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
	tcp_client_for_01();
}
#endif

void tcp_client_for()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
	int ret_value; // 函数返回值
	int socket_fd[5]; // 套接字描述符

//	for (int i = 0; i < 5; ++i)
	for (int & socketfd_i : socket_fd) // 循环创建 5 个 客户端
	{
		/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
		socketfd_i = socket(AF_INET, SOCK_STREAM, 0);
		err_sys(socketfd_i, __LINE__, "%s()", __func__);

		/* ---- 配置 ---- */
//	char ip[]="10.23.21.248"; // 本机 ip
		struct sockaddr_in client_address{};
		bzero(&client_address, sizeof(HOST_Service_IPv4)); // 清 0
		client_address.sin_family = AF_INET; // IPv4
		client_address.sin_port = htons(PORT_service); // 端口号
#if 0
		client_address.sin_addr.s_addr = inet_addr(Client_IP_host); // Internet address
#else
		ret_value = inet_pton(AF_INET, HOST_Service_IPv4, &client_address.sin_addr); // Internet address
		/* errno set */
		if (ret_value < 0)err_sys(ret_value, __LINE__,
								  "inet_pton error for %s",
								  PORT_service);
			/* errno not set */
		else if (ret_value == 0)err_quit(ret_value, __LINE__,
										 "inet_pton errno not set %s",
										 PORT_service);
#endif
		/* ---- connect () ---- */
		ret_value = connect(socketfd_i,
							(struct sockaddr *) (&client_address),
							sizeof(client_address));
		err_sys(ret_value, __LINE__, "%s()", __func__);

		std::cout << "Hello, I am a client, socketfd_i is "
				  << socketfd_i << std::endl;
		/* 获取 与某个套接字 关联的 本地/对方 协议地址 */
		getIP_addr_port(socketfd_i);
	}

	for (const int & socketfd_i : socket_fd)
	{
		/* do it all */
#if (TCP_Echo_Call >= 0 && TCP_Echo_Call < 3)
		tcp_client_call(stdin, socketfd_i);		/* do it all */
#elif TCP_Echo_Call==3
		/********************* I/O口复用 *************************/
		tcp_client_call_select(stdin, socketfd_i);
#elif Select_Echo_Call==4
		/********************* I/O口复用 *************************/
		tcp_client_call_poll(stdin, socketfd_i);
#endif
		/* ---- close () ---- */
		ret_value = close(socketfd_i);
		err_sys(ret_value, __LINE__, "%s()", __func__);
		std::cout << "NO " << socketfd_i << " client : bye !" << std::endl;
	}
	std::cout << "---------------------------------------------------" << std::endl;
}

//		/* 获取 与某个套接字 关联的 本地/对方 协议地址 */
//		getIP_addr_port(socket_fd, 1);
//		getIP_addr_port(socket_fd, 2);
