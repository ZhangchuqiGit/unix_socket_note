
#include "zcq_header.h"

#if 0
int main(int argc, char **argv) {
	unix_DGRAM_client();
}
#endif

void unix_DGRAM_client()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int socket_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	socket_fd = socket(AF_LOCAL, SOCK_DGRAM, IPPROTO_IP);
	err_sys(socket_fd, __LINE__, "socket()");
/*************************************************************************/
	/** unix_local 客户端需要绑定自己的信息,否则服务器无法给客户端发送数据 **/
	/* ---- 配置 信息 ---- */
	struct sockaddr_un client_addr{}; // 域内套接字用
	bzero(&client_addr, sizeof(client_addr)); // 清 0
	client_addr.sun_family = AF_LOCAL; // 主机本地（管道和文件域）

	std::string path = "/home/zcq/datagram";
	unlink(path.data()); /** 删除文件/链接名称，防止已经存在 */
	strcpy(client_addr.sun_path, path.data()); // 绑定自己的路径名

	/* ---- bind () ---- */
	retval=bind(socket_fd, (struct sockaddr *)(&client_addr), sizeof(client_addr));
	err_sys(retval, __LINE__, "bind()");
	/** bind() 默认文件权限 0777 **/
//	retval=chmod(path.data(),0777); // 设置文件权限
//	err_sys(retval, __LINE__, "chmod()");
/*************************************************************************/
	/* ---- 配置 信息 ---- */
	struct sockaddr_un servaddr{}; // 域内套接字用
	bzero(&servaddr, sizeof(servaddr)); // 清 0
	servaddr.sun_family	= AF_LOCAL; // 主机本地（管道和文件域）
	strcpy(servaddr.sun_path, UNIX_path_datagram); // 绑定自己的路径名

	/* do it all */
	udp_client_call(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
/*************************************************************************/
	/* ---- close () ---- */
	retval=close(socket_fd);
	err_sys(retval, __LINE__, "close()");

	unlink(path.data()); /** 删除文件/链接名称，防止已经存在 */
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}
