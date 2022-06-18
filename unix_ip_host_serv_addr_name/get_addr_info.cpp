//
// Created by zcq on 2021/4/1.
//

#include "ip_port_addrs_names.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>


/** 支持 IPv4 和 IPv6 **/


/**	gethostbyname() 和 gethostbyaddr这() 仅仅支持 IPv4，
 * getaddrinfo() 能够处理 名字到地址 以及 服务到端口 这两种转换，返回 sockaddr 结构链表。
 * 这些 sockaddr 结构 随后可由 套接口函数 直接使用。
 * 如此一来，getaddrinfo() 把协议相关性安全隐藏在这个库函数内部。
 * 应用程序只要处理由 getaddrinfo() 填写的套接口地址结构。
 * 头文件：Linux #include<netdb.h>；windows #include<ws2tcpip.h>    **/

/** getnameinfo() 是 getaddrinfo() 的互补函数，它以一个套接字地址为参数，
 * 返回描述其中的主机的一个字符串和描述其中的服务的另一个字符串。
 * 本函数以协议无关的方式提供这些信息，也就是说，
 * 调用者不必关心存放在套接字地址结构中的协议地址的类型，因为这些细节由函数自行处理。
 * sock_ntop() 和 getnameinfo() 的差别在于，
 * 前者不涉及 DNS，只返回 IP地址 和 端口号 的一个 可显示版本；
 * 后者通常 尝试 获取 主机 和 服务 的名字  **/

/* 	int getaddrinfo (const char *host, const char *service,
					 const struct addrinfo *hints, struct addrinfo **result );
参数：
	host:		主机名(域名) 或 IP地址串(IPv4点分十进制串 或 IPv6的16进制串)
	service：	服务名，十进制的端口号 如12345 或 已定义的服务名，如 ftp、http 等
	hints：	可以是 NULL，可以是指向 addrinfo 结构体的指针，
			调用者在这个结构中填入关于期望返回的信息类型的暗示 hints。
			若是 NULL，则相当于 ai_family=AF_UNSPEC, ai_socktype=0,
			ai_protocol=0, ai_flags=AI_ADDRCONFIG|AI_V4MAPPED.
	result：本函数通过 result 指针参数返回一个指向 addrinfo 结构体链表 的指针。
返回值：0 成功，非0 出错。 (见gai_strerror函数)
---------------------------------------------------------------------------
struct addrinfo {
    int               ai_flags;			位掩码，可以 或运算 a|b
    int               ai_family;		AF_xxx；协议族，同socket()参数1一致
    int               ai_socktype;		SOCK_xxx；socket类型，同socket()参数2一致
    int               ai_protocol;		0 or IPPROTO_xxx for IPv4 and IPv6
    									协议类型，同socket()参数3一致
    socklen_t         ai_addrlen;		length of struct sockaddr ai_addr
    char             *ai_canonname;		ptr to canonical name for host
    									主机的 标准名称
	struct sockaddr  *ai_addr;			ptr to socket address structure
 										该结构体对应的一个网络地址
	struct addrinfo  *ai_next;			ptr to next structure in linked list
 										指向下一个 addrinfo 结构体的指针
};
---------------------------------------------------------------------------
在调用 getaddrinfo()之前 通常需要对以下参数进行以下设置：
	host，service，hints 的 ai_flags、ai_family、ai_socktype、ai_protocol
而 ai_family 只是地址有 IPv4 或 IPv6 的区别。
而 ai_protocol 一般是为 0。
---------------------------------------------------------------------------
参数			取值			值		说明

ai_flags		AI_PASSIVE		0x0001	套接字被动打开，
 							通常用于 服务器 server 的 socket()
							仅当 hostname=NULL 时起作用；若设置了该标志，
							则查询结果中的IP地址是通配地址；
							若未设置该标志，则IP地址是环回地址。
				AI_CANONNAME	0x0002	ai_canonname 返回 主机的 规范名称
				AI_NUMERICHOST	0x0004	表明 hostname 只是 IP地址串，不是主机名(域名)
				AI_NUMERICSERV	0x0400	表明 service 只是 端口号，不是服务名
				AI_V4MAPPED		0x0008	如果同时指定 ai_family 成员的值为 AF_INET6，
							那么如果没有可用的 AAAA 记录，
							就返回与 A 记录对应的 IPv4 映射的 IPv6地址
				AI_ALL			0x0010	如果同时指定 AI_V4MAPPED 标志，
							那么除了返回与 AAAA 记录对应的 IPv6 地址外，
							还返回与 A 记录对应的 IPv4 映射的 IPv6 地址
				AI_ADDRCONFIG	0x0020	根据本地主机配置，仅 返回 IPV4 或 IPV6地址
							若本地主机仅仅配置 IPV4地址，查询结果仅包含 IPV4地址，
							若本地主机仅仅配置 IPV6地址，查询结果仅包含 IPV6地址

ai_family		AF_UNSPEC		0		协议无关
 				AF_INET			2		IPv4
				AF_INET6		23		IPv6

ai_socktype		0						表示不限定
 				SOCK_STREAM		1		流 tcp
 				SOCK_DGRAM		2		数据报 udp

ai_protocol		IPPROTO_IP		0		IP协议
				IPPROTO_IPV4	4		IPv4
				IPPROTO_IPV6	41		IPv6
				IPPROTO_UDP		17		UDP
				IPPROTO_TCP		6		TCP				 */

/* 	getaddrinfo() 返回的所有存储空间都是动态获取的，
	这些存储空间必须通过调用 freeaddrinfo() 返回给系统。
	参数 ai 应指向由 getaddrinfo() 返回的 第一个 addrinfo 结构。
	这个 结构体链表 指向的 任何动态存储空间 都被 释放掉
void freeaddrinfo ( struct addrinfo *ai ); // 释放结构体链表指向的任何动态存储空间
---------------------------------------------------------------------------
 使用此函数存在的一个潜在问题：
	假设我们调用getaddrinfo()，遍历返回的addrinfo结构链表后找到所需的结构。
如果我们为保存其信息而仅仅复制这个addrinfo结构，然后调用freeaddrinfo()，
那就引入了一个潜藏的错误。原因在于这个addrinfo结构本身指向动态分配的内存空间
（用于存放套接字地址结构和可能有的规范主机名），
因此由我们保存的结构指向的内存空间已在调用freeaddrinfo时返还给系统，
稍后可能用于其他目的。因此，当调用这个函数之后，
getaddinfo()函数返回的addinfo结构的内容就不可以再使用了，因为已经被释放了   */

/*************************************************************************/

struct addrinfo *Get_addrinfo(std::string_view host, std::string_view service,
							  const int &family, const int &socktype,
							  const int &protocol, const int & flags )
{
	return Get_addrinfo( host.data(), service.data(),
						 family, socktype, protocol, flags);
}

struct addrinfo *Get_addrinfo(const char *host, const char *service,
							  const int &family, const int &socktype,
							  const int &protocol, const int & flags )
{
//	debug_line(__LINE__,(char *)__FUNCTION__,"");
	struct addrinfo hints{};
	bzero(&hints, sizeof(struct addrinfo)); // 清 0
	hints.ai_flags = flags;			/* Request for canonical name. */
	hints.ai_family = family;		/* AF_UNSPEC, AF_INET, AF_INET6, etc. */
	hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */
	hints.ai_protocol = protocol; 	/* 0 or IPPROTO_xxx for IPv4 and IPv6. */

	struct addrinfo	*result;
	int	retval = getaddrinfo(host, service, &hints, &result);
	err_addrinfo(retval, __LINE__, "%s(): host:%s; serv:%s", __func__,
				 (host == nullptr) ? "no hostname" : host,
				 (service == nullptr) ? "no service" : service);
#if 0
	std::cout << "位掩码 \t \tai_flags: \t" 		<< result->ai_flags 	<< std::endl
			  << "协议族 \t \tai_family: \t" 	<< result->ai_family 	<< std::endl
			  << "socket类型 \tai_socktype: \t"	<< result->ai_socktype 	<< std::endl
			  << "协议类型 \tai_protocol: \t" 	<< result->ai_protocol 	<< std::endl;
	if (result->ai_canonname != nullptr)
		std::cout << "主机名称 \tai_canonname: \t"
				  << (char *)result->ai_canonname << std::endl;
//	std::cout << "ip: " << get_addr_port_ntop_r(result->ai_addr) << std::endl;
#endif
	return (struct addrinfo *)(result);	/* return pointer to first on linked list */
}
