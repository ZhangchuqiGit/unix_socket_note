
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
	tcp_service_fork_01();
}
#endif


void tcp_service_fork()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int ret_value; // 函数返回值
	int socket_listen_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
	socket_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	err_sys(socket_listen_fd, __LINE__, "socket()");

	const int option = 1;

	/* ---- 允许地址的立即重用，端口不占用 ---- */
	ret_value=setsockopt(socket_listen_fd, SOL_SOCKET/* 通用选项 */,
						 SO_REUSEADDR/*地址立即重用*/, &option, sizeof(option) );
	err_sys(ret_value, __LINE__,"setsockopt()");

	/* ---- 允许端口的立即重用 ---- */
//	ret_value=setsockopt(socket_listen_fd, SOL_SOCKET/*通用选项*/,
//						 SO_REUSEPORT/*端口立即重用*/, &option, sizeof(option) );
//	err_sys(ret_value, __LINE__,"setsockopt()");

#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	ret_value=setsockopt(socket_listen_fd, SOL_SOCKET/*通用选项*/,
						 SO_RCVBUF/* 接收缓冲区 */,
						 &receiver_buf, sizeof(receiver_buf) );
	err_sys(ret_value, __LINE__,"setsockopt()");
#endif

	/* ---- 配置 ip 地址 端口 信息 ---- */
	struct sockaddr_in server_address{};
	bzero(&server_address, sizeof(server_address)); // 清 0
	server_address.sin_family		=AF_INET; // IPv4
	server_address.sin_addr.s_addr	=htonl(INADDR_ANY); // Internet address
	server_address.sin_port			=htons(PORT_service); // 端口号

	/* ---- bind () ---- */
	ret_value=bind(socket_listen_fd, (struct sockaddr *)(&server_address), sizeof(server_address) );
	err_sys(ret_value, __LINE__, "bind()");

	/* ---- listen () ---- */
	ret_value=listen(socket_listen_fd, MAX_listen/*最大连接数*/ );
	err_sys(ret_value, __LINE__, "listen()");

	int socket_accept_fd; // 新(连接成功)套接字描述符
	while (true)
	{
		/* ---- accept () ---- */
		socket_accept_fd = Accept(socket_listen_fd);

		/* ---- 创建 服务 子进程 ---- */
		service_fork(socket_listen_fd, socket_accept_fd);
	}

	/* ---- close () ---- */
	ret_value=close(socket_listen_fd);
	err_sys(ret_value, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}
