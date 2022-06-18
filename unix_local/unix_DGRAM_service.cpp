
#include "zcq_header.h"

#if 0
int main(int argc, char **argv) {
	unix_DGRAM_service();
}
#endif

void unix_DGRAM_service()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int socket_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	socket_fd = socket(AF_LOCAL, SOCK_DGRAM, IPPROTO_IP);
	err_sys(socket_fd, __LINE__, "socket()");

	/* ---- 允许地址的立即重用，端口不占用 ---- */
//	int option = 1;
//	ret_value=setsockopt(socket_fd, SOL_SOCKET/* 通用选项 */,
//						 SO_REUSEADDR/*地址立即重用*/, &option, sizeof(option) );
//	err_sys(ret_value, __LINE__,"setsockopt()");
	/* ---- 允许端口的立即重用 ---- */
//	ret_value=setsockopt(socket_fd, SOL_SOCKET/*通用选项*/,
//						 SO_REUSEPORT/*端口立即重用*/, &option, sizeof(option) );
//	err_sys(ret_value, __LINE__,"setsockopt()");
#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	ret_value=setsockopt(socket_fd, SOL_SOCKET/*通用选项*/,
						 SO_RCVBUF/* 接收缓冲区 */,
						 &receiver_buf, sizeof(receiver_buf) );
	err_sys(ret_value, __LINE__,"setsockopt()");
#endif
	unlink(UNIX_path_datagram); /** 删除文件/链接名称，防止已经存在 */

	/* ---- 配置 信息 ---- */
	struct sockaddr_un servaddr{}; // 域内套接字用
	bzero(&servaddr, sizeof(servaddr)); // 清 0
	servaddr.sun_family	= AF_LOCAL; // 主机本地（管道和文件域）
	strcpy(servaddr.sun_path, UNIX_path_datagram); // 绑定自己的路径名

	/* ---- bind () ---- */
	retval=bind(socket_fd, (struct sockaddr *)(&servaddr), sizeof(servaddr));
	err_sys(retval, __LINE__, "bind()");
	/** bind() 默认文件权限 0777 **/
//	retval=chmod(UNIX_path_datagram,0777); // 设置文件权限
//	err_sys(retval, __LINE__, "chmod()");

	/* 信息传递 */
	bzero(&servaddr, sizeof(servaddr)); // 清 0
	udp_service_echo(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	/* ---- close () ---- */
	retval=close(socket_fd);
	err_sys(retval, __LINE__, "close()");

	unlink(UNIX_path_datagram); /** 删除文件/链接名称，防止已经存在 */
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}

