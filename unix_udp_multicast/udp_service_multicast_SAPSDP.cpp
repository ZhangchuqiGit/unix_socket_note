

/**************** 套接口 选项列表 ****************/
/*
#include <sys/socket.h>
int getsockopt ( int sockfd, int level, int optname,
				           void* optval socklen_t* optlen);
int setsockopt ( int sockfd, int level, int optname,
                     const void* optval, socklen_t optlen);
参数:
    sockfd: 套接字 sock fd
    level: 协议
    optname, 选项名
    optval: 选项值指针(数据类型)
    optlen: optval 的长度
返回: 成功:0  失败:-1
---------------------------------------------------------------------------
level	optname					get set 说明					   标志 数据类型
---------------------------------------------------------------------------
SOL_SOCKET	(通用选项)
 		SO_BROADCAST			*	*	允许发送广播数据报			*	int
		SO_DEBUG				*	*	使能调试跟踪					*	int
		SO_DONTROUTE			*	*	不查找路由					*	int
		SO_ERROR				*		获取待处理错误并消除				int
		SO_KEEPALIVE			*	*	周期性测试连接是否存活			*	int
		SO_LINGER				*	*	若有数据待发送则延迟关闭			linger{}
		SO_OOBINLINE			*	*	让接收到的带外数据继续在线存放	*	int
		SO_RCVBUF				*	*	接收缓冲区大小					int
		SO_SNDBUF				*	*	发送缓冲区大小					int
		SO_RCVLOWAT				*	*	接收缓冲区下限					int
		SO_SNDLOWAT				*	*	发送缓冲区下限					int
		SO_RCVTIMEO				*	*	接收超时							timeval{}
		SO_SNDTIMEO				*	*	发送超时							timeval{}
		SO_REUSEADDR			*	*	允许重用本地地址				*	int
		SO_REUSEPORT			*	*	允许重用本地端口				*	int
		SO_TYPE					*		取得套接口类型					int
		SO_USELOOPBACK			*	*	路由套接口取得所发送数据的副本	*	int
		SO_xxx
---------------------------------------------------------------------------
IPPROTO_ICMPV6
 		ICMP6_FILTER			*	*	指定传递的ICMPv6消息类型			icmp6_filter{}
---------------------------------------------------------------------------
IPPROTO_TCP	(TCP 选项)
 		TCP_KEEPALIVE			*	*	控测对方是否存活前连接闲置秒数		int
		TCP_MAXRT				*	*	TCP最大重传时间					int
		TCP_MAXSEG				*	*	TCP最大分节大小					int
		TCP_NODELAY				*	*	禁止Nagle算法				*	int
		TCP_STDURG				*	*	紧急指针的解释				*	int
		TCP_xxx
---------------------------------------------------------------------------
IPPROTO_IP	(IPv4 选项)
 		IP_HDRINCL				*	*	IP头部包括数据				*	int
		IP_OPTIONS				*	*	IP头部选项						见后面说明
		IP_RECVDSTADDR			*	*	返回目的IP地址				*	int
		IP_RECVIF				*	*	返回接收到的接口索引			*	int
		IP_TOS					*	*	服务类型和优先权					int
		IP_TTL					*	*	存活时间							int

		IP_MULTICAST_IF			*	*	指定外出接口						in_addr{}
		IP_MULTICAST_TTL		*	*	指定外出超时时间 TTL				u_char
		IP_MULTICAST_LOOP		*	*	指定是否回馈						u_char

		IP_ADD_MEMBERSHIP			*	加入多播组						ip_mreq{}
		IP_DROP_MEMBERSHIP			*	离开多播组						ip_mreq{}
		IP_BLOCK_SOURCE				*	阻塞多播源						ip_mreq_source{}
		IP_UNBLOCK_SOURCE			*	开通多播源						ip_mreq_source{}
		IP_ADD_SOURCE_MEMBERSHIP 	*	加入源特定多播组					ip_mreq_source{}
		IP_DROP_SOURCE_MEMBERSHIP 	*	离开源特定多播组					ip_mreq_source{}
		IP_xxx
---------------------------------------------------------------------------
IPPROTO_IPV6	(IPv6 选项)
		IPV6_CHECKSUM			*	*	原始套接口的校验和字段偏移			int
		IPV6_NEXTHOP			*	*	指定下一跳 NEXT HOP 地址		*	sockaddr_in6{}
 		IPV6_ADDRFORM			*	*	改变套接口的地址结构				int
		IPV6_RECVDSTOPTS		*	*	接收目标选项					*	int
		IPV6_RECVHOPLIMIT		*	*	接收单播跳限					*	int
		IPV6_RECVHOPOPTS		*	*	接收步跳选项					*	int
		IPV6_RECVPKTINFO		*	*	接收分组信息					*	int
		IPV6_PKTOPTIONS			*	*	指定分组选项						见后面说明
		IPV6_RECVRTHDR			*	*	接收原路径					*	int
		IPV6_UNICAST_HOPS		*	*	默认单播跳限						int

		IPV6_MULTICAST_IF		*	*	指定外出接口						u_int
		IPV6_MULTICAST_HOPS		*	*	指定外出跳限 HOPS				int
		IPV6_MULTICAST_LOOP		*	*	指定是否回馈					*	u_int

		IPV6_ADD_MEMBERSHIP	 		*	加入多播组						ipv6_mreq{}
		IPV6_DROP_MEMBERSHIP 		*	离开多播组						ipv6_mreq{}
		IPV6_xxx
---------------------------------------------------------------------------
#ifdef __USE_MISC (兼容 IPv4、IPv6 选项)
		MCAST_JOIN_GROUP 			*	加入多播组						group_req{}
		MCAST_LEAVE_GROUP   		*	离开多播组						group_req{}
		MCAST_BLOCK_SOURCE 			*	阻塞多播源						group_source_req{}
		MCAST_UNBLOCK_SOURCE  		*	开通多播源						group_source_req{}
		MCAST_JOIN_SOURCE_GROUP 	*	加入源特定多播组					group_source_req{}
		MCAST_LEAVE_SOURCE_GROUP   	*	离开源特定多播组					group_source_req{}
---------------------------------------------------------------------------
 */

#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h> //for inet_addr
#include <unistd.h>  //for read

#include <iostream>
//using namespace std;

#if 0
int main(int argc, char **argv) {
	udp_client_multicast();
}
#endif

/** 多播程序框架
（1）套接字初始化：建立一个socket。
（2）然后设置多播的参数，例如：超时时间TTL、本地回环许可LOOP 等。
（3）加入多播组。
（4）发送和接收数据。
（5）从多播组离开。            */

/* receive and print */
void loop(const int &sock_fd, const socklen_t &sa_len);

void udp_servicr_multicast_SAPSDP()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
/*************************************************************************/
//	_debug_log_info("udp_client_multicast_onlyrecv()")
	std::string str_buf;
	str_buf += "usage: <mcast-addr> <port#> <interface-name>\n";
	str_buf += "usage: 224.2.127.254 9875 enp3s0:\n";
	str_buf += "Now input please...\n";
	str_buf += "usage: ";
/* 	输入列表内容，如：111 222 333  zzz    gsfg
	获取每一个 std::vector<string> 子内容        */
	std::vector<std::string> vec_user = hints_strtovec(str_buf);
//	if (vec_user.empty()) err_quit(-1, __LINE__, "empty args");
/*************************************************************************/
	int retval; // 函数返回值
	int sock_fd = 0; // 套接字描述符
	struct sockaddr_in client_addr{};
	socklen_t sa_len = 0;

	retval = vec_user.size();
	_debug_log_info("retval: %d", retval)
	if (retval == 0) {
		/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
		/* ---- 配置 ip 地址 端口 信息 ---- */
		sock_fd = udp_socket_config((struct sockaddr *) &client_addr, &sa_len,
									SAP_NAME, SAP_PORT,
									AF_UNSPEC, SOCK_DGRAM,
									IPPROTO_IP,
									AI_CANONNAME | AI_NUMERICSERV);
	} else if (retval == 3) {
		/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
		/* ---- 配置 ip 地址 端口 信息 ---- */
		sock_fd = udp_socket_config((struct sockaddr *) &client_addr, &sa_len,
									vec_user[0].c_str(),
									vec_user[1].c_str());
	} else err_quit(-1, __LINE__, "args error");

	_debug_log_info("允许地址的立即重用")
	/* ---- 允许地址的立即重用，端口不占用 ---- */
	const int option = 1;
	retval=setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
					  SO_REUSEADDR/*地址立即重用*/, &option, sizeof(option) );
	err_sys(retval, __LINE__,"setsockopt()");

	_debug_log_info("bind()")
	/* ---- bind () ---- */
	retval=bind(sock_fd, (struct sockaddr *)(&client_addr), sa_len);
	err_sys(retval, __LINE__, "bind()");

	retval = vec_user.size();
	Mcast_join(sock_fd, (struct sockaddr *) &client_addr, sa_len,
			   (retval == 3) ? vec_user[2].c_str() : nullptr, 0);

	/* 获取 与某个套接字 关联的 本地/对方 协议地址 */
	getIP_addr_port(sock_fd);

	loop(sock_fd, sa_len);	// receive and print

	/* ---- close () ---- */
	retval=close(sock_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}

#define	SAP_VERSION			1
#define SAP_VERSION_MASK	0xe0000000
#define	SAP_VERSION_SHIFT	29
#define	SAP_IPV6			0x10000000
#define	SAP_DELETE			0x04000000
#define	SAP_ENCRYPTED		0x02000000
#define	SAP_COMPRESSED		0x01000000
#define	SAP_AUTHLEN_MASK	0x00ff0000
#define	SAP_AUTHLEN_SHIFT	16
#define	SAP_HASH_MASK		0x0000ffff

/* receive and print */
void loop(const int &sock_fd, const socklen_t &sa_len)
{
	_debug_log_info("loop()")
	using namespace std;

//	struct sockaddr_storage sas{};
	auto uniqueptr = make_unique<char[]>(sa_len+1);
	auto *sa = (struct sockaddr *)uniqueptr.get();

	/* SDP 会话描述 */
	struct SAP_packet {
		uint32_t	sap_header;
		uint32_t	sap_src;
		char		sap_data[BUFFSIZE];
	} sap_packet{};
	size_t saplen = sizeof(sap_packet)-1;

	socklen_t len;
	char *p;
	ssize_t num;
	while (true)
	{
		len = sa_len;
		num = recvfrom(sock_fd, &sap_packet, saplen, 0, sa, &len);
		err_sys(num, __LINE__, "recvfrom()");
		((char *)&sap_packet)[num] = 0;			/* null terminate */

		sap_packet.sap_header = ntohl(sap_packet.sap_header);

		printf("From %s hash 0x%04x\n", addr_net_to_ptr(sa).c_str(),
			   sap_packet.sap_header & SAP_HASH_MASK);

		if (((sap_packet.sap_header & SAP_VERSION_MASK) >> SAP_VERSION_SHIFT) > 1) {
			err_msg(-1, __LINE__,
					"... version field not 1 (0x%08x)", sap_packet.sap_header);
			continue;
		}
		if (sap_packet.sap_header & SAP_IPV6) {
			err_msg(-1, __LINE__, "... IPv6");
			continue;
		}
		if (sap_packet.sap_header & (SAP_DELETE | SAP_ENCRYPTED | SAP_COMPRESSED)) {
			err_msg(-1, __LINE__,
					"... can't parse this packet type (0x%08x)",
					sap_packet.sap_header);
			continue;
		}
		p = sap_packet.sap_data +
			((sap_packet.sap_header & SAP_AUTHLEN_MASK)	>> SAP_AUTHLEN_SHIFT);
		if (strcmp(p, "application/sdp") == 0) p += 16;
		printf("%s\n", p);
	}
}
