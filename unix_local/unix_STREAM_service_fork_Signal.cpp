
#include "zcq_header.h"

#if 0
int main(int argc, char **argv) {
	unix_service_fork_01();
}
#endif

void unix_STREAM_service_fork()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int ret_value; // 函数返回值
	int listen_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
	listen_fd = socket(AF_UNIX, SOCK_STREAM, IPPROTO_IP);
	err_sys(listen_fd, __LINE__, "socket()");

	/* ---- 允许地址的立即重用，端口不占用 ---- */
//	int option = 1;
//	ret_value=setsockopt(listen_fd, SOL_SOCKET/* 通用选项 */,
//						 SO_REUSEADDR/*地址立即重用*/, &option, sizeof(option) );
//	err_sys(ret_value, __LINE__,"setsockopt()");
	/* ---- 允许端口的立即重用 ---- */
//	ret_value=setsockopt(socket_listen_fd, SOL_SOCKET/*通用选项*/,
//						 SO_REUSEPORT/*端口立即重用*/, &option, sizeof(option) );
//	err_sys(ret_value, __LINE__,"setsockopt()");
#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	ret_value=setsockopt(listen_fd, SOL_SOCKET/*通用选项*/,
						 SO_RCVBUF/* 接收缓冲区 */,
						 &receiver_buf, sizeof(receiver_buf) );
	err_sys(ret_value, __LINE__,"setsockopt()");
#endif
	unlink(UNIX_path_stream); /** 删除文件/链接名称，防止已经存在 */

	/* ---- 配置 信息 ---- */
	struct sockaddr_un servaddr{};
	bzero(&servaddr, sizeof(servaddr)); // 清 0
	servaddr.sun_family		=AF_LOCAL; // 主机本地（管道和文件域）
	strcpy(servaddr.sun_path, UNIX_path_stream);

	/* ---- bind () ---- */
	ret_value=bind(listen_fd, (struct sockaddr *)(&servaddr), sizeof(servaddr) );
	err_sys(ret_value, __LINE__, "bind()");

	/* ---- listen () ---- */
	ret_value=listen(listen_fd, MAX_listen/*最大连接数*/ );
	err_sys(ret_value, __LINE__, "listen()");

	/*******************************************************************/
	/* ---- Signal () ---- */
	// SIGCHLD : 父进程 捕捉 子进程 的状态信号。
	Signal_fork(SIGCHLD);	/* must call waitpid() */
	/*******************************************************************/

	int accept_fd; // 新(连接成功)套接字描述符
	while (true)
	{
		/* ---- accept () ---- */
		accept_fd = Accept(listen_fd, (struct sockaddr *)(&servaddr));

		/* ---- 创建 服务 子进程 ---- */
		service_fork(listen_fd, accept_fd);
	}

	/* ---- close () ---- */
	ret_value=close(listen_fd);
	err_sys(ret_value, __LINE__, "close()");

	unlink(UNIX_path_stream); /** 删除文件/链接名称，防止已经存在 */
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}

