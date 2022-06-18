//
// Created by zcq on 2021/3/29.
//

#include "ip_port_addrs_names.h"

#include <netdb.h>
#include <sys/socket.h>

/**************************** 不可重入版本 ********************************/

/** 不推荐使用 gethostbyname() 和 gethostbyaddr()，只能 获得 IPv4 信息！ **/
/**	不推荐使用 gethostbyname2() **/

/**	推荐使用 可重入版本
	gethostbyname_r()，只能 获得 IPv4 信息！
	gethostbyname2_r()，
	gethostbyaddr_r()    				**********************************/

/**	gethostbyname() 和 gethostbyaddr() 仅仅支持 IPv4，
 * getaddrinfo() 能够处理 名字到地址 以及 服务到端口 这两种转换，返回 sockaddr 结构链表。
 * 这些 sockaddr 结构 随后可由 套接口函数 直接使用。
 * 如此一来，getaddrinfo() 把协议相关性安全隐藏在这个库函数内部。
 * 应用程序只要处理由 getaddrinfo() 填写的套接口地址结构。
 * 头文件：Linux #include<netdb.h>；windows #include<ws2tcpip.h>    **/


#if 0
int main(int argc, char **argv) {
	host_display();
}
#endif

void host_display()
{
#if 0
	// $ hostname # zcq
	// $ hostname --all-ip-addresses # 192.168.6.119
	Get_host_byname_IPv4("zcq"); // 127.0.1.1

	Get_host_byname_IPv4("zcq.local"); // 192.168.6.119

	Get_host_byname_IPv4("www.baidu.com");

	Get_host_byname_IPv4("v.qq.com");

	Get_host_byname_IPv4("amazonaws.com");

	Get_host_byname_IPv4("www.google.com");
#elif 1
	Get_host_byname2("zcq.local"); // 192.168.6.119

	Get_host_byname2("www.google.com");
#elif 1
	Get_host_byaddr_IPv4("192.168.6.7"); // zcq.local
	Get_host_byaddr_IPv4("192.168.6.119"); // zcq.local
	Get_host_byaddr_IPv4("14.215.85.123"); // v.qq.com
	Get_host_byaddr_IPv4("127.0.1.1"); // zcq
#else
	Get_IPv4_host_byname_byaddr("v.qq.com");
#endif
}

/**	客户端直接使用 IP 地址会有很大的弊端，一旦 IP 地址变化（IP 地址会经常变动），
	客户端软件就会出现错误。而使用 域名 或者 主机名 会方便很多，
	注册后的域名只要每年续费就永远属于自己的，
	更换 IP 地址时修改 域名解析 即可，不会影响软件的正常使用。 **/

/** 使用 域名 或者 主机名 或 IP 地址 得到 主机 IP 地址信息 结构体 hostent **/
/*	struct hostent {
		char  * h_name; 	 主机 规范名，
			例如 www.baidu.com 的 规范名 是 www.a.shifen.com

		char ** h_aliases; 	 主机 别名 列表，一个主机可以有多个别名，
			可以通过多个域名访问同一主机。同一 IP地址可以绑定多个域名，
			因此除了当前域名还可以指定其他域名。
			例如 www.baidu.com 就是 baidu 他自己的 别名

		int  h_addrtype; 	 主机 IP地址 类型，IPv4:AF_INET，IPv6:AF_INET6

		int  h_length; 		 主机 IP地址 字节长度，
			IPv4 字节长度为 4，IPv6 字节长度为 16

		char ** h_addr_list; 主机 IP地址 列表， 二进制网络字节序格式 IP 地址
			（ 二进制网络字节序格式，需要通过 inet_ntop()转出 字符串 IP地址）
			对于用户较多的服务器，可能会分配多个 IP 地址给同一域名，
			利用多个服务器进行均衡负载
	}	*/

/** gethostbyname()作用：用 域名 或者 主机名 获取 主机 IP 信息。只能 获得 IPv4 信息！
	域名 仅仅是 IP 地址的一个助记符，目的是方便记忆，
	通过域名并不能找到目标计算机，通信之前必须要将 域名 转换成 IP 地址。 **/
/*	struct hostent * gethostbyname (const char *hostname);
参数：	hostname 为主机名，也就是 域名 字符串，如百度的host名是www.baidu.com
返回: 域名 对应的 IP 地址信息( hostent 结构体)；失败返回 NULL，同时设置 h_errno  */

/** gethostbyname2()作用：用 域名 或者 主机名 获取 主机 IP 信息。
	域名 仅仅是 IP 地址的一个助记符，目的是方便记忆，
	通过域名并不能找到目标计算机，通信之前必须要将 域名 转换成 IP 地址。 **/
/*	struct hostent * gethostbyname2 (const char *hostname, int af);
参数：	hostname 为主机名，也就是 域名 字符串，如百度的host名是www.baidu.com
		af：主机 IP地址 类型，IPv4:AF_INET 同 gethostbyname()，IPv6:AF_INET6
返回: 域名 对应的 IP 地址信息( hostent 结构体)；失败返回 NULL，同时设置 h_errno  */

/** gethostbyaddr()作用：用 IP 地址 获取 主机 IP 信息。只能 获得 IPv4 信息！ **/
/*	struct hostent * gethostbyaddr (const void *addr, socklen_t len, int family);
	addr：	指向 IPv4 或 IPv6 地址结构 in_addr 或 in6_addr,
 		二进制网络字节序 IP 地址，需要通过 inet_ntop() 输出 ip 二进制网络字节序格式
	len：	结构 addr 的大小，IPv4 字节长度为 4，IPv6 字节长度为 16
    family：主机 IP地址 类型，IPv4:AF_INET，IPv6:AF_INET6
返回: 域名 对应的 IP 地址信息( hostent 结构体)；失败返回 NULL，同时设置 h_errno  */

/**	错误代码 h_errno
	HOST_NOT_FOUND 	没有找到授权应答主机。
	TRY_AGAIN 		没有找到非授权主机。
	NO_RECOVERY 	无法恢复的错误，FORMERR，REFUSED，NOTIMP。
	NO_DATA 		有效的名字，但没有 关于请求类型的数据记录。    **/


/****************** gethostbyname 只能 获得 IPv4 信息！ ********************/

/* gethostbyname()作用：用 域名 或者 主机名 获取 主机 IP 信息。只能 获得 IPv4 信息！ */
struct hostent * Get_host_byname_IPv4(std::string_view hostname)
{
	struct hostent *hostent_ptr; // IP 地址信息
	if ( (hostent_ptr = gethostbyname(hostname.data())) == nullptr ) {
		err_msg(-1, __LINE__,
				"%s()\thostname:\t%s", __func__ , hostname.data());
		return nullptr;
	}

	std::cout << "============================= " << __func__ << std::endl;
	std::cout << "hostname:\t" << hostname << std::endl;
	host_byname_echo(hostent_ptr);
	return hostent_ptr;
}

/*************************** gethostbyname2 ******************************/

/* gethostbyname2()作用：用 域名 或者 主机名 获取 主机 IP 信息。只能 获得 IPv4 信息！ */
struct hostent * Get_host_byname2(std::string_view hostname, int af)
{
	struct hostent *hostent_ptr; // IP 地址信息
	if ( (hostent_ptr = gethostbyname2(hostname.data(), af)) == nullptr ) {
		err_msg(-1, __LINE__,
				"%s()\thostname:\t%s", __func__ , hostname.data());
		return nullptr;
	}

	std::cout << "============================= " << __func__ << std::endl;
	std::cout << "hostname:\t" << hostname << std::endl;
	host_byname_echo(hostent_ptr);
	return hostent_ptr;
}

/****************** gethostbyaddr 只能 获得 IPv4 信息！ ********************/

/* gethostbyaddr()作用：用 IP 地址 获取 主机 IP 信息。只能 获得 IPv4 信息！ */
struct hostent * Get_host_byaddr_IPv4(std::string_view hostaddr)
{
	struct in6_addr in4_addr_s{}; /* IPv6 address. */
	inet_pton(AF_INET6, hostaddr.data(), &in4_addr_s); // 转为 网络字节序
	/* 输出 ip 二进制网络字节序格式 */
//	auto in6_addr_s = (struct in6_addr *)addr_str_to_net(hostaddr, AF_INET);

	socklen_t 	h_length = 4; // 结构 addr 的大小，IPv4 字节长度为 4，IPv6 字节长度为 16
	int 		type = AF_INET; // 主机 IP地址 类型，IPv4:AF_INET，IPv6:AF_INET6
	struct hostent *hostent_ptr = nullptr; // IP 地址信息
	hostent_ptr = gethostbyaddr(&in4_addr_s, /* 二进制网络字节序格式 IP 地址 */
								h_length, type);
	if (hostent_ptr == nullptr) {
		err_msg(-1, __LINE__, "%s(); hostaddr: %s",
				__func__ ,hostaddr.data());
		return nullptr;
	}

	std::cout << "============================= " << __func__ << std::endl;
	std::cout << "hostaddr:\t" << hostaddr << std::endl;
	host_byname_echo(hostent_ptr);
	return hostent_ptr;
}

/*************************  只能 获得 IPv4 信息！  *************************/

/* 	gethostbyname() 用 域名 或者 主机名 获取 主机 IP 信息
	gethostbyaddr() 用 IP 地址 获取 主机 IP 信息 */
void Get_host_byname_byaddr_IPv4(std::string_view hostname)
{
	struct hostent *hostent_name = Get_host_byname_IPv4(hostname); // IP 地址信息
	if ( hostent_name == nullptr ) return;

	char buf_str[INET_ADDRSTRLEN];
	char **ptr =  hostent_name->h_addr_list;
	struct in_addr *inaddr = nullptr;
	for ( ; *ptr != nullptr; ++ptr) {
		debug_line(__LINE__, (char *)__FUNCTION__, "");
		inaddr = ((struct in_addr *) (*ptr));
		inet_ntop(AF_INET, inaddr, buf_str, INET_ADDRSTRLEN);
		Get_host_byaddr_IPv4(buf_str);
	}
}