
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
	tcp_client_host_serv();
}
#endif


void tcp_client_one_byNamePort()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
/*************************************************************************/
	struct in_addr *inetaddr[2];
	struct in_addr **pptr = nullptr;
/*************************************************************************/
	std::string name = name_host_serv("host");
	/* gethostbyname()作用：用 域名 或者 主机名 获取 主机 IP 信息。只能 获得 IPv4 信息！ */
	struct hostent *host_ptr = Get_host_byname_IPv4(name);
	/* gethostbyaddr()作用：用 IP 地址 获取 主机 IP 信息。只能 获得 IPv4 信息！ */
//	struct hostent *host_ptr = Get_host_byaddr_IPv4(name);

	if ( host_ptr == nullptr) {
		debug_line(__LINE__, (char *)__FUNCTION__, "");
		if (inet_aton(name.c_str(), inetaddr[0]) == 0) {
			err_quit(-1, __LINE__,
					 "host name error for \"%s\"", name.c_str());
		} else {
			inetaddr[1] = nullptr;
			pptr = inetaddr;
		}
	}else {
		debug_line(__LINE__, (char *)__FUNCTION__, "");
		pptr = (struct in_addr **) host_ptr->h_addr_list;
	}
/*************************************************************************/
	name = name_host_serv("service");
	/* gethostbyaddr()作用：用 IP 地址 获取 主机 IP 信息。 */
	struct servent *serv_ptr = Get_serv_byname( name, "tcp");
//	int portnum = port_host_serv("portnum");
/* getservbyport()作用：使用 端口号+协议名 获取 服务 IP 信息。 */
//	struct servent *serv_ptr = Get_serv_byport(portnum, "tcp");

	if ( serv_ptr == nullptr) {
		err_quit(-1, __LINE__,
				 "service name error for \"%s\"", name.c_str());
	}
/*************************************************************************/
	int retval; // 函数返回值
	int socket_fd; // 套接字描述符
	struct sockaddr_in service_address{}; // 配置
	struct sockaddr_in6 service_address6{}; // 配置
	struct sockaddr	*sa = nullptr;
	if ( host_ptr->h_addrtype == AF_INET )
		sa = (struct sockaddr *) &service_address;
	else if ( host_ptr->h_addrtype == AF_INET6 )
		sa = (struct sockaddr *) &service_address6;
	else err_quit(-1, __LINE__, "%s()", __func__ );

	for ( ; *pptr != nullptr; ++pptr)
	{
		debug_line(__LINE__, (char *)__FUNCTION__, "");

		/* ---- socket () ---- SOCK_STREAM 顺序的，可靠的，基于连接的字节流 */
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		err_sys(socket_fd, __LINE__, "socket()");

		/* ---- 配置 ip 地址 端口 信息 ---- */
		sock_address_config( sa,
							 (int)(host_ptr->h_addrtype) 	/* Address families */,
							 (int)(serv_ptr->s_port)		/* 网络字节序 端口号 */,
							 *pptr					/* 网络字节序 IP 地址 */);

		debug_line(__LINE__, (char *)__FUNCTION__, "");

		std::string ip_addr_port = get_addr_port_ntop(sa);
		std::cout << "-----------------------" << std::endl
				  << "trying " << ip_addr_port << std::endl;

		/* ---- connect () ---- */
		retval = connect_timeo(socket_fd, sa, 3);
		err_ret(retval, __LINE__, "connect %s error", ip_addr_port.c_str());
		if (retval == 0) {
			std::cout << "connect " << ip_addr_port
					  << " success !" << std::endl;
			break;
		} else {
			/* ---- close () ---- */
			retval = close(socket_fd);
			err_sys(retval, __LINE__, "close()");
		}
	}
	debug_line(__LINE__, (char *)__FUNCTION__, "");

	if (*pptr == nullptr)err_sys(-1, __LINE__, "unable to connect");

	/* 获取 与某个套接字 关联的 本地/对方 协议地址 */
	getIP_addr_port(socket_fd, host_ptr->h_addrtype);

	/* do it all */
	tcp_client_call(stdin, socket_fd);		/* do it all */

	/* ---- close () ---- */
	retval=close(socket_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}
