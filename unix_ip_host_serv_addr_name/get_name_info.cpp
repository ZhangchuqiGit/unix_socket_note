//
// Created by zcq on 2021/4/1.
//

#include "ip_port_addrs_names.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>


/** 支持 IPv4 和 IPv6 **/


/** getnameinfo() 是 getaddrinfo() 的互补函数，它以一个套接字地址为参数，
 * 返回描述其中的主机的一个字符串和描述其中的服务的另一个字符串。
 * 本函数以协议无关的方式提供这些信息，也就是说，
 * 调用者不必关心存放在套接字地址结构中的协议地址的类型，因为这些细节由函数自行处理。
 * inet_ntop() 和 getnameinfo() 的差别在于，
 * 前者不涉及 DNS，只返回 IP地址 和 端口号 的一个 可显示版本；
 * 后者通常 尝试 获取 主机 和 服务 的名字  **/

/**	gethostbyname() 和 gethostbyaddr这() 仅仅支持 IPv4，
 * getaddrinfo() 能够处理 名字到地址 以及 服务到端口 这两种转换，返回 sockaddr 结构链表。
 * 这些 sockaddr 结构 随后可由 套接口函数 直接使用。
 * 如此一来，getaddrinfo() 把协议相关性安全隐藏在这个库函数内部。
 * 应用程序只要处理由 getaddrinfo() 填写的套接口地址结构。
 * 头文件：Linux #include<netdb.h>；windows #include<ws2tcpip.h>    **/

/* 	int getnameinfo (const struct sockaddr *sockaddr, socklen_t addrlen,
					 char *host, size_t hostlen,
					 char *serv, size_t servlen,      int flags );
参数：
	sockaddr：	指向一个套接字地址结构，其中包含待转换成直观可读的字符串的协议地址。
				该结构通常由 accept、recvfrom、getsockname、getpeername 返回。
	addrlen：	是结构 sockaddr 的长度
	host：		调用者 预先分配 的存储空间，用来存储 返回的 主机 字符串
	hostlen：	对应于参数 host 的长度。如果调用者 不想返回 主机 字符串，就指定为0
	serv：		调用者 预先分配 的存储空间，用来存储 返回的 服务 字符串
	servlen：	对应于参数 serv 的长度。如果调用者 不想返回 服务 字符串，就指定为0
	flags：		位掩码，可以 或运算 a|b
返回值：0 成功，非0 出错。 (见gai_strerror函数)
---------------------------------------------------------------------------
flags 标志：

 NI_DGRAM：声明(设置) 数据报套接字 UDP
	因为在套接字地址结构中给出的仅仅是IP地址和端口号，
 	getnameinfo()无法就此确定所用协议(TCP或UDP)。
	有若干个端口号在TCP上用于一个服务，在UDP上却用于截然不同的另一个服务。
	端口514就是这样的一个例子，它在TCP上提供rsh服务，在UDP上提供syslog服务。

 NI_NAMEREQD：如果无法使用DNS反向解析出 主机名，则返回一个错误。
	需要把客户的IP地址映射成主机名的那些 服务器 可以使用这个特性。
	这些服务器随后以这样返回的主机名调用gethostbyname()，
	以便验证gethostbyname()返回的某个地址就是早先调用getnameinfo()指定的套接字地址结构中的地址。

 NI_NOFQDN：该标志导致返回的 主机名 第一个点号之后的内容被截去。
	举例来说，假设套接字地址结构中的IP地址为192.168.42.2，
	那么不设置本标志的gethostbyaddr()返回的主机名为 aix.unpbook.com，
	而设置本标志的gethostbyaddr()返回的主机名为    aix 。

 NI_NUMERICHOST：该标志告知getnameinfo()不要调用DNS（因为调用DNS可能耗时），
	而是以数值表达格式以字符串的形式返回 IP地址（可能通过调用inet_ntop()实现）。

 NI_NUMERICSERV：该标志指定以十进制数格式作为字符串返回 端口号，以代替查找服务名。
	既然客户的端口号通常没有关联的服务名——它们是临时的端口，
 	服务器 通常应该设置 NI_NUMERICSERV 标志。

 NI_NUMERICSCOPE：该标志指定以数值格式作为字符串返回 范围标识，以代替其名字。 	*/

/*************************************************************************/

/* 尝试 获取 主机 和 服务 的名字 */
void Get_nameinfo(const struct sockaddr *sockaddr, const socklen_t &addrlen,
				  char *host, const size_t &hostlen,
				  char *serv, const size_t &servlen, 	const int &flags )
{
	int	retval = getnameinfo(sockaddr, addrlen,
								host, hostlen, serv, servlen, flags );
	err_addrinfo(retval, __LINE__, "%s(): host:%s; serv:%s", __func__,
				 (host == nullptr) ? "no hostname" : host,
				 (serv == nullptr) ? "no service" : serv);
#if 1
	std::cout << "协议族 family:\t" << sockaddr->sa_family << std::endl;
	if (hostlen != 0) std::cout << "主机名/IP地址:\t"
								<< std::setw(servlen) << host << std::endl;
	if (servlen != 0) std::cout << "服务名/端口号:\t"
								<< std::setw(servlen) << serv << std::endl;
#endif
}

/* 尝试 获取 主机 和 服务 的名字 */
void Get_nameinfo(const struct sockaddr *sockaddr, const socklen_t &addrlen)
{
	size_t hostlen = INET_ADDRSTRLEN;
	char host[hostlen];
	bzero(host, hostlen); // 清 0

	size_t servlen = 8;
	char serv[servlen];
	bzero(serv, servlen); // 清 0

	Get_nameinfo( (struct sockaddr *) (&sockaddr), addrlen,
				  host, hostlen, serv, servlen);
}