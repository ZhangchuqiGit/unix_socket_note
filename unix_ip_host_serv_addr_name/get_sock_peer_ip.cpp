//
// Created by zcq on 2021/3/18.
//

#include "ip_port_addrs_names.h"

#include <sys/socket.h>

/** 如果函数调用成功，则返回0，如果调用出错，则返回-1。
---------------------------------------------------------------------------
 	获取 本地 与某个套接字 关联的 IP地址 和 端口号。
int getsockname(int sockfd,struct sockaddr* localaddr,socklen_t *addrlen);
---------------------------------------------------------------------------
	获取 对方 与某个套接字 关联的 IP地址 和 端口号。
int getpeername(int sockfd,struct sockaddr* peeraddr,socklen_t *addrlen);
---------------------------------------------------------------------------
	getpeername()只有在连接建立以后才调用，否则不能正确获得对方地址和端口，
所以它的参数描述字一般是已连接描述字而非监听套接口描述字。
	没有连接的UDP不能调用getpeername()，但是可以调用getsockname()和TCP一样，
它的地址和端口不是在调用socket()就指定了，而是在第一次调用sendto()函数以后。
	已经连接的UDP，在调用connect()以后，这getsockname()、getpeername()都是可以用的。
但是这时意义不大，因为已经连接（connect）的UDP已经知道对方的地址。
---------------------------------------------------------------------------
需要使用的理由：
	在一个没有调用bind()的TCP客户上，connect()成功返回后，
getsockname()用于返回由内核赋予该连接的 本地 IP地址 和 端口号。
	在以端口号为0调用bind()（告知内核去选择本地临时端口号）后，
getsockname()用于返回由内核赋予的 本地 端口号。
	在一个以通配IP地址调用bind()的TCP服务器上，与某个客户的连接一旦建立（accept()成功返回），
getsockname()就可以用于返回由内核赋予该连接的 本地 IP地址。
在这样的调用中，套接字描述符参数必须是 已连接 套接字的描述符，而不是监听套接字的描述符。
	当一个服务器是由调用过accept()的某个进程通过调用exec执行程序时，
它能够获取客户身份的唯一途径便是调用getpeername()。		 **/

/**---------------------------------------------------------------------**/

/*	获取 本地 与某个套接字 关联的 IP地址。 */
/* 输出 ip 字符串 如 192.168.6.7 */
std::string Getsockname_ip(const int &socket_fd, const int &family)
{
	int retval; // 函数返回值
	socklen_t length;
//	struct sockaddr sai{}; 		/* generic socket address */
//	struct sockaddr_storage	sas{};	/* large socket address */
//	length = _SS_SIZE;
	switch (family)
	{
		case AF_INET: {
			struct sockaddr_in sai_IPv4{}; /* Internet socket address. */
			length = INET_ADDRSTRLEN;
			retval = getsockname(socket_fd, (struct sockaddr *) &sai_IPv4, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip 字符串 如 192.168.6.7 */
			return addr_net_to_ptr((struct sockaddr *) &sai_IPv4);
		}
		case AF_INET6: {
			struct sockaddr_in6 sai_IPv6{}; /* IPv6 socket address. */
			length = INET6_ADDRSTRLEN;
			retval = getsockname(socket_fd, (struct sockaddr *) &sai_IPv6, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip 字符串 如 192.168.6.7 */
			return addr_net_to_ptr((struct sockaddr *) &sai_IPv6);
		}
//		case AF_LOCAL: {
		case AF_UNIX: {
			struct sockaddr_un sau{}; /* address of an AF_LOCAL(aka AF_UNIX) socket. */
			length = _SS_SIZE;
			retval = getsockname(socket_fd, (struct sockaddr *) &sau, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip 字符串 如 192.168.6.7 */
			return addr_net_to_ptr((struct sockaddr *) &sau);
		}
		default:
			break;
	}
	return "nullptr";
}

/*	获取 本地 与某个套接字 关联的 IP地址 和 端口号。 */
/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
std::string getsockname_ip_port(const int &socket_fd, const int &family)
{
	int retval; // 函数返回值
	socklen_t length;
//	struct sockaddr sai{}; 		/* generic socket address */
//	struct sockaddr_storage	sas{};	/* large socket address */
//	length = _SS_SIZE;
	switch (family)
	{
		case AF_INET: {
			struct sockaddr_in sai_IPv4{}; /* Internet socket address. */
			length = INET_ADDRSTRLEN;
			retval = getsockname(socket_fd, (struct sockaddr *) &sai_IPv4, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
			return get_addr_port_ntop( (struct sockaddr *) &sai_IPv4 );
		}
		case AF_INET6: {
			struct sockaddr_in6 sai_IPv6{}; /* IPv6 socket address. */
			length = INET6_ADDRSTRLEN;
			retval = getsockname(socket_fd, (struct sockaddr *) &sai_IPv6, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
			return get_addr_port_ntop( (struct sockaddr *) &sai_IPv6 );
		}
		case AF_UNIX: {
			return Getsockname_ip(socket_fd, AF_UNIX);
		}
		default:
			break;
	}
	return "nullptr";
}

/*	获取 本地 与某个套接字 关联的 IP地址 和 端口号。 */
std::string Getsockname_ip_port(const int &socket_fd, int & portnum,
								const int &family)
{
	std::string buf_str;
	int pos = -1;
	switch (family)
	{
		case AF_INET: {
			/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
			buf_str = getsockname_ip_port(socket_fd, AF_INET);
			pos = buf_str.rfind(':');
			if (pos > 0) {
				portnum = std::stoi(buf_str.substr(pos+1));
				//buf_str.erase(buf_str.begin()+pos, buf_str.end());
				buf_str.erase(pos);	/* 从位置pos处删除直到结尾 */
				return buf_str;
			}
		}
			break;
		case AF_INET6: {
			/* 输出 ip + port 字符串 如 [1:...:1]:12345 */
			buf_str = getsockname_ip_port(socket_fd, AF_INET6);
			pos = buf_str.rfind(']', buf_str.size()-1);	// [1:...:1]:12345
			if (pos > 0) {
				portnum = std::stoi(buf_str.substr(pos+2));
				//buf_str.erase(buf_str.begin()+pos, buf_str.end());
				buf_str.erase(pos);	/* 从位置pos处删除直到结尾 */
				buf_str.erase(0,1); // '['
				return buf_str;
			}
		}
			break;
		case AF_UNIX:
			buf_str = getsockname_ip_port(socket_fd, AF_UNIX);
			break;
		default:
			buf_str = "error ";
			buf_str += std::to_string(family);
			break;
	}
	portnum = -1;
	return buf_str;
}

/**---------------------------------------------------------------------**/

/*	获取 对方 与某个套接字 关联的 IP地址。 */
/* 输出 ip 字符串 如 192.168.6.7 */
std::string Getpeername_ip(const int &socket_fd, const int &family)
{
	int retval; // 函数返回值
	socklen_t length;
//	struct sockaddr sai{}; 		/* generic socket address */
//	struct sockaddr_storage	sas{};	/* large socket address */
//	length = _SS_SIZE;
	switch (family)
	{
		case AF_INET: {
			struct sockaddr_in sai_IPv4{}; /* Internet socket address. */
			length = INET_ADDRSTRLEN;
			retval = getpeername(socket_fd, (struct sockaddr *) &sai_IPv4, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip 字符串 如 192.168.6.7 */
			return addr_net_to_ptr((struct sockaddr *) &sai_IPv4);
		}
		case AF_INET6: {
			struct sockaddr_in6 sai_IPv6{}; /* IPv6 socket address. */
			length = INET6_ADDRSTRLEN;
			retval = getpeername(socket_fd, (struct sockaddr *) &sai_IPv6, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip 字符串 如 192.168.6.7 */
			return addr_net_to_ptr((struct sockaddr *) &sai_IPv6);
		}
		default: {
			struct sockaddr_storage sas{};
			length = sizeof(struct sockaddr_storage);
			retval = getpeername(socket_fd, (struct sockaddr *) &sas, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip 字符串 如 192.168.6.7 */
			return addr_net_to_ptr((struct sockaddr *) &sas);
		}
	}
}

/*	获取 对方 与某个套接字 关联的 IP地址 和 端口号。 */
/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
std::string getpeername_ip_port(const int &socket_fd, const int &family)
{
	int retval; // 函数返回值
	socklen_t length;
//	struct sockaddr sai{}; 		/* generic socket address */
//	struct sockaddr_storage	sas{};	/* large socket address */
//	length = _SS_SIZE;
	switch (family)
	{
		case AF_INET: {
			struct sockaddr_in sai_IPv4{}; /* Internet socket address. */
			length = INET_ADDRSTRLEN;
			retval = getpeername(socket_fd, (struct sockaddr *) &sai_IPv4, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
			return get_addr_port_ntop( (struct sockaddr *) &sai_IPv4 );
		}
		case AF_INET6: {
			struct sockaddr_in6 sai_IPv6{}; /* IPv6 socket address. */
			length = INET6_ADDRSTRLEN;
			retval = getpeername(socket_fd, (struct sockaddr *) &sai_IPv6, &length);
			err_ret(retval, __LINE__, "%s()", __func__);
			/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
			return get_addr_port_ntop( (struct sockaddr *) &sai_IPv6 );
		}
		default:
			break;
	}
	return "nullptr";
}

/*	获取 对方 与某个套接字 关联的 IP地址 和 端口号。 */
std::string Getpeername_ip_port(const int &socket_fd, int & portnum,
								const int &family)
{
	std::string buf_str;
	int pos = -1;
	switch (family)
	{
		case AF_INET: {
			/* 输出 ip + port 字符串 如 192.168.6.7:1080 */
			buf_str = getpeername_ip_port(socket_fd, AF_INET);
			pos = buf_str.rfind(':');
			if (pos != -1) {
				portnum = std::stoi(buf_str.substr(pos+1));
				//buf_str.erase(buf_str.begin()+pos, buf_str.end());
				buf_str.erase(pos);	/* 从位置pos处删除直到结尾 */
			}
			return buf_str;
		}
			break;
		case AF_INET6: {
			/* 输出 ip + port 字符串 如 [1:...:1]:12345 */
			buf_str = getpeername_ip_port(socket_fd, AF_INET6);
			pos = buf_str.rfind(']', buf_str.size()-1);	// [1:...:1]:12345
			if (pos != -1) {
				portnum = std::stoi(buf_str.substr(pos+2));
				//buf_str.erase(buf_str.begin()+pos, buf_str.end());
				buf_str.erase(pos);	/* 从位置pos处删除直到结尾 */
				buf_str.erase(0,1); // '['
			}
			return buf_str;
		}
			break;
		default:
			buf_str = "error ";
			buf_str += std::to_string(family);
			break;
	}
	portnum = -1;
	return buf_str;
}

/**---------------------------------------------------------------------**/

/* 获取 本地/对方 与某个套接字 关联的 IP地址 和 端口号。 */
void getIP_addr_port(const int &socket_fd, const int &family)

{
	std::cout << "------------------------------------------" << std::endl;
	std::cout << "获取 本地/对方 与套接字 "<< socket_fd
			  <<" 关联的 IP地址 和 端口号" << std::endl;

	int portnum;
	std::cout << "本地 IP地址: " << Getsockname_ip_port(socket_fd, portnum, family)
			  << "  \t\t本地 端口号: " << portnum << std::endl;

	std::cout << "对方 IP地址: " << Getpeername_ip_port(socket_fd, portnum, family)
			  << "  \t\t对方 端口号: " << portnum << std::endl;

	std::cout << "------------------------------------------" << std::endl;
}

