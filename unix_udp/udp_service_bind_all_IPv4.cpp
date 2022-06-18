
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
	udp_service_bind_all_ip();
}
#endif

void servmode_client(struct if_info *if_i, const int &family); 	// 服务端：单播模式
void servmode_broadcase(struct if_info *if_i, const int &family);// 服务端：只接收广播模式
void servmode_service(const int &family); 						// 服务端：通配模式

/* 信息传递 */
void mydg_echo(const int &sock_fd, std::string_view modestr,
			   const struct sockaddr *myaddr, const socklen_t &addrlen );

void udp_service_bind_all_IPv4()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	const int family = AF_INET;
	std::unique_ptr<struct if_info []> if_info_ary = Get_if_info(family);
	for (struct if_info * if_i = if_info_ary.get();
		 if_i != nullptr;
		 if_i = if_i->if_next ) {
		if (if_i->if_flags & IFF_BROADCAST) // 绑定 支持广播的(UDP) 所有 IP 地址
		{
			pid_t child_pid = 0;

			_debug_log_info("创建 单播模式")
			if ((child_pid = fork()) == 0) { 	// 创建 子进
				servmode_client(if_i, family);			// 服务端：单播模式程
				if_info_ary.reset(); 			// Auto free resource
				exit(0);
			}
			if (child_pid < 0)
				std::cerr << "创建 单播模式 失败！\t"
						  << "IP 地址: "
						  << Addr_net_to_ptr_r(family, &if_i->if_addr)
						  << std::endl;
			else std::cout << "创建 单播模式 子进程 ID: " << child_pid << std::endl;

			_debug_log_info("创建 只接收广播模式")
			if ((child_pid = fork()) == 0) { 	// 创建 子进程
				servmode_broadcase(if_i, family);		// 服务端：只接收广播模式
				if_info_ary.reset(); 			// Auto free resource
				exit(0);
			}
			if (child_pid < 0)
				std::cerr << "创建 只接收广播模式 失败！\t"
						  << "IP 地址: "
						  << Addr_net_to_ptr_r(family, &if_i->if_brdaddr)
						  << std::endl;
			else std::cout << "创建 只接收广播模式 子进程 ID: " << child_pid << std::endl;
		}
	}
	if_info_ary.reset(); // Auto free resource

	_debug_log_info("创建 通配模式")
	servmode_service(family); // 服务端：通配模式

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}

/* 服务端：单播模式 */
void servmode_client(struct if_info *if_i, const int &family) // 服务端：单播模式
{
	int sock_fd; // 套接字描述符
	int retval; // 函数返回值
	const int option = 1;

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	sock_fd = socket(family, SOCK_DGRAM, 0);
	err_sys(sock_fd, __LINE__, "socket()");

	/* ---- 允许地址的立即重用，端口不占用 ---- */
	/* ---- 允许端口的立即重用 ---- */
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   {SO_REUSEADDR/*地址立即重用*/, SO_REUSEPORT/*端口立即重用*/},
			   &option, sizeof(option));

#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	const int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   {SO_RCVBUF/* 接收缓冲区 */},
			   &receiver_buf, sizeof(receiver_buf));
#endif

	/* ---- 配置 ip 地址 端口 信息 ---- */
	auto servaddr_ptr = (struct sockaddr_in *) &if_i->if_addr;
	servaddr_ptr->sin_family = family; 			// IPv4
	servaddr_ptr->sin_port = htons(PORT_service); 	// 端口号

	/* ---- bind () ---- */
	retval=bind(sock_fd, (struct sockaddr *)(servaddr_ptr), sizeof(*servaddr_ptr) );
	if (retval < 0) {
		if (errno == EADDRINUSE/* Address already in use */) {
			std::cout.flush();
			std::cerr << "单播模式[Address already in use] "
					  << get_addr_port_ntop_r((struct sockaddr *)servaddr_ptr)
					  << std::endl;
			err_sys(close(sock_fd), __LINE__, "close()");
			return;
		}
		err_sys(retval, __LINE__, "bind(): %s",
				get_addr_port_ntop_r((struct sockaddr *)servaddr_ptr).c_str() );
	}
	std::cout << "单播模式 bind "
			  << get_addr_port_ntop_r((struct sockaddr *)servaddr_ptr) << std::endl;

	/* 信息传递 */
	mydg_echo(sock_fd, "单播模式",
			  (struct sockaddr *)servaddr_ptr, sizeof(*servaddr_ptr) );

	/* ---- close () ---- */
	err_sys(close(sock_fd), __LINE__, "close()");
}

/* 服务端：只接收广播模式 */
void servmode_broadcase(struct if_info *if_i, const int &family) // 服务端：只接收广播模式
{
	int sock_fd; // 套接字描述符
	int retval; // 函数返回值
	const int option = 1;

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	sock_fd = socket(family, SOCK_DGRAM, 0);
	err_sys(sock_fd, __LINE__, "socket()");

	/* ---- 允许地址的立即重用，端口不占用 ---- */
	/* ---- 允许端口的立即重用 ---- */
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   {SO_REUSEADDR/*地址立即重用*/, SO_REUSEPORT/*端口立即重用*/},
			   &option, sizeof(option));

#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	const int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   {SO_RCVBUF/* 接收缓冲区 */},
			   &receiver_buf, sizeof(receiver_buf));
#endif
#if     0
	/* ---- 启用广播模式 ---- 用于发送广播消息 */
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */, SO_BROADCAST/*只接收广播模式*/,
			   &option, sizeof(option));
#endif

	/* ---- 配置 ip 地址 端口 信息 ---- */
	auto servaddr_ptr = (struct sockaddr_in *) &if_i->if_brdaddr;
	servaddr_ptr->sin_family = family; 			// IPv4
	servaddr_ptr->sin_port = htons(PORT_service); 	// 端口号

	/* ---- bind () ---- */
	retval=bind(sock_fd, (struct sockaddr *)(servaddr_ptr),	sizeof(*servaddr_ptr) );
	if (retval < 0) {
		if (errno == EADDRINUSE/* Address already in use */) {
			std::cout.flush();
			std::cerr << "只接收广播模式[Address already in use] "
					  << get_addr_port_ntop_r((struct sockaddr *)servaddr_ptr)
					  << std::endl;
			err_sys(close(sock_fd), __LINE__, "close()");
			return;
		}
		err_sys(retval, __LINE__, "bind(): %s",
				get_addr_port_ntop_r((struct sockaddr *)servaddr_ptr).c_str() );
	}

	std::cout << "只接收广播模式 bind "
			  << get_addr_port_ntop_r((struct sockaddr *)servaddr_ptr) << std::endl;

	/* 信息传递 */
	mydg_echo(sock_fd, "只接收广播模式",
			  (struct sockaddr *)servaddr_ptr, sizeof(*servaddr_ptr) );

	/* ---- close () ---- */
	err_sys(close(sock_fd), __LINE__, "close()");
}

/* 服务端：通配模式 */
void servmode_service(const int &family) // 服务端：通配模式
{
	int sock_fd; // 套接字描述符
	int retval; // 函数返回值
	const int option = 1;

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	sock_fd = socket(family, SOCK_DGRAM, 0);
	err_sys(sock_fd, __LINE__, "socket()");

	/* ---- 允许地址的立即重用，端口不占用 ---- */
	/* ---- 允许端口的立即重用 ---- */
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   {SO_REUSEADDR/*地址立即重用*/, SO_REUSEPORT/*端口立即重用*/},
			   &option, sizeof(option));

#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	const int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   {SO_RCVBUF/* 接收缓冲区 */},
			   &receiver_buf, sizeof(receiver_buf));
#endif

	/* ---- 配置 ip 地址 端口 信息 ---- */
	struct sockaddr_in servaddr{};
	bzero(&servaddr, sizeof(servaddr)); // 清 0
	servaddr.sin_family			=family; // IPv4
	servaddr.sin_addr.s_addr	=htonl(INADDR_ANY); // Internet address
	servaddr.sin_port			=htons(PORT_service); // 端口号

	/* ---- bind () ---- */
	retval=bind(sock_fd, (struct sockaddr *)&servaddr,	sizeof(servaddr) );
	if (retval < 0) {
		if (errno == EADDRINUSE/* Address already in use */) {
			std::cout.flush();
			std::cerr << "通配模式[Address already in use] "
					  << get_addr_port_ntop_r((struct sockaddr *)&servaddr)
					  << std::endl;
			err_sys(close(sock_fd), __LINE__, "close()");
			return;
		}
		err_sys(retval, __LINE__, "bind(): %s",
				get_addr_port_ntop_r((struct sockaddr *)&servaddr).c_str() );
	}

	std::cout << "通配模式 bind "
			  << get_addr_port_ntop_r((struct sockaddr *)&servaddr) << std::endl;

	/* 信息传递 */
	mydg_echo(sock_fd, "通配模式",
			  (struct sockaddr *)&servaddr, sizeof(servaddr) );

	/* ---- close () ---- */
	err_sys(close(sock_fd), __LINE__, "close()");
}

/* 信息传递 */
void mydg_echo(const int &sock_fd, std::string_view modestr,
			   const struct sockaddr *myaddr, const socklen_t &addrlen )
{
	char myaddr_buf[addrlen];
	memcpy(myaddr_buf, myaddr, addrlen);
	auto *addr_ptr = (struct sockaddr *)myaddr_buf;

	char buf[MAXLINE];
	socklen_t addr_len;
	ssize_t numval;
	while (true) {
		addr_len = addrlen;
		bzero(buf, sizeof(buf));
		numval = recvfrom(sock_fd, buf, sizeof(buf), 0, addr_ptr, &addr_len);
		err_sys(numval, __LINE__, "recvfrom()");

		if (numval > 1 && buf[numval - 1] == '\n') buf[numval - 1] = '\0';
		std::cout << "------------------------------------\n"
				  << modestr << "\tchild " << getpid() << "\t"
				  << get_addr_port_ntop_r((struct sockaddr *)myaddr) << "\n"
				  << numval << " sizes datagram from "
				  << get_addr_port_ntop_r((struct sockaddr *)addr_ptr) << "\n"
				  << "reciver: " << buf << std::endl;

		if (if_run("time", buf)) {
			memset(buf, 0, sizeof(buf));
			time_t ticks;
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
			ticks = time(nullptr);
			snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
		}

		sendto(sock_fd, buf, strlen(buf), 0, addr_ptr, addr_len);
	}
}
