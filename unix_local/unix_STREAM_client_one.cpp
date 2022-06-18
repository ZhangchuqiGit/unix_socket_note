
#include "zcq_header.h"

#if 0
int main(int argc, char **argv) {
	unix_STREAM_client_one();
}
#endif

void unix_STREAM_client_one()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int socket_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
	socket_fd = socket(AF_LOCAL, SOCK_STREAM, IPPROTO_IP);
	err_sys(socket_fd, __LINE__, "socket()");

	/* ---- 配置 信息 ---- */
	struct sockaddr_un client_addr{};
	bzero(&client_addr, sizeof(client_addr)); // 清 0
	client_addr.sun_family		=AF_LOCAL; // 主机本地（管道和文件域）
	strcpy(client_addr.sun_path, UNIX_path_stream);

	/* ---- connect () ---- */
	retval=connect(socket_fd, (struct sockaddr *)&client_addr, sizeof(client_addr));
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
