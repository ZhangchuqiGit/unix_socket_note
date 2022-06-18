
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
	tcp_client_get_host_serv_addrinfo();
}
#endif


void tcp_client_one_addrinfo()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int socket_fd; // 套接字描述符
//	struct sockaddr	sa{};
//	struct sockaddr_storage	sas{}; // 结构足够大，可以容纳任何套接字地址

	debug_line(__LINE__, (char *)__FUNCTION__, "");

	/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
	/* ---- 配置 ---- */
	/* ---- connect () ---- */
	socket_fd = tcp_socket_connect(HOST_Service_IPv4,
								   PORT_service_STR,
								   (int)AF_INET, SOCK_STREAM );
//	socket_fd = tcp_socket_connect(name_host_serv("host"),
//								   PORT_service_STR,
//								   (int) AF_INET6, SOCK_STREAM );

	/* do it all */
	tcp_client_call(stdin, socket_fd);		/* do it all */

	/* ---- close () ---- */
	retval=close(socket_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}
