//
// Created by zcq on 2021/4/25.
//

#include "wrap_sockopt.h"


void Setsockopt(const int &sock_fd, const int &level, const int &optname,
				const void *optval, const socklen_t &optlen)
{
	err_sys(setsockopt(sock_fd, level, optname, optval, optlen),
			__FILE__, __func__, __LINE__, "%s()", __func__);
}

void Setsockopt(const int &sock_fd, const int &level,
				const std::initializer_list<int > &optname,
				const void *optval, const socklen_t &optlen)
{
	for (const auto &optname_i: optname)
		Setsockopt(sock_fd, level, optname_i, optval, optlen);

//		/* ---- 允许地址的立即重用，端口不占用 ---- */
//		retval = setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
//							SO_REUSEADDR/*地址立即重用*/, &option, sizeof(option));
//		err_sys(retval, __LINE__, "setsockopt()");
//
//		/* ---- 允许端口的立即重用 ---- */
//		retval = setsockopt(sock_fd, SOL_SOCKET/*通用选项*/,
//							SO_REUSEPORT/*端口立即重用*/, &option, sizeof(option));
//		err_sys(retval, __LINE__, "setsockopt()");
//
//#ifdef Receiver_MAXBuf_mode
//		/* ---- 修改 接收 缓冲区 大小 ---- */
//				const int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
//				retval=setsockopt(sock_fd, SOL_SOCKET/*通用选项*/,
//								  SO_RCVBUF/* 接收缓冲区 */,
//								  &receiver_buf, sizeof(receiver_buf) );
//				err_sys(retval, __LINE__,"setsockopt()");
//#endif
}

void Getsockopt(const int &sock_fd, const int &level, const int &optname,
				void *optval, socklen_t *optlen)
{
	err_sys(getsockopt(sock_fd, level, optname, optval, optlen),
			__FILE__, __func__, __LINE__, "%s()", __func__);
}

void Getsockopt(const int &sock_fd, const int &level,
				const std::initializer_list<int > &optname,
				void *optval, socklen_t *optlen)
{
	for (const auto &optname_i: optname)
		Getsockopt(sock_fd, level, optname_i, optval, optlen);
}


/* 	每当收到一个带外数据时，就有一个与之关联的带外标记。
 	与带外数据关联的带外标记：带外字节在发送端普通数据流中的位置。
 	不管接收进程在线（SO_OOBINLINE 套接字选项）还是带外（MGS_OOB 标志）接收带外数据，
 带外标记 sockatmark() / SIOCATMARK 都适合。
--------------------------------------------------------------------------------
 在从套接字读入期间，接收进程可调用 sockatmark()函数确定是否处于带外标记。
 int sockatmark(int sockfd); // 返回值：如果在带外标记上为1, 不在标记上为0, 出错为-1
--------------------------------------------------------------------------------
 if (ioctl(fd, SIOCATMARK, &flag) < 0) return(-1);
 SIOCATMARK 带外标记套接字选项； 	flag 带外标：如果在带外标记上为1, 不在标记上为0
--------------------------------------------------------------------------------
（1）带外标记总是指向普通数据最后一个字节紧后的位置。
	 这意味着，如果带外数据 在线接收（SO_OOBINLINE套接字选项开启），
 如果待读入的带外字节是使用 MSG_OOB 标志发送的，sockatmask() 就返回 真。
 	如果 SO_OOBINLINE 套接字选项 没有开启，若下一个待读入的字节是跟在带外数据后发送的第一个字节，
 sockatmark() 就返回 真。
（2）读操作总是停在带外标记上。
 	如果在套接字接收缓冲区有100个字节，不过在带外标记之前只有5个字节，
 而进程执行一个请求100个字节的read调用，那么返回的是带外标记之前的5个字节。
 这种在带外标记上强制停止读操作的做法使得进程能够调用 sockatmark()确实缓冲区指针是否处于带外标记。
 如： 1,2,3,4,5,M,6,7,...,100
 	read : 1,2,3,4,5
 	read : M,6,7,...,100           */
int Sockatmark(const int &sock_fd) // 是否处于带外标记
{
/* Determine whether socket is at a out-of-band mark.  */
	int flag = 0;
#if 1
	flag = sockatmark(sock_fd);
	err_sys(flag, __FILE__, __func__, __LINE__, "sockatmark()");
#else
	if (ioctl(sock_fd, SIOCATMARK/*带外标记*/, &flag) < 0) return -1;
#endif
	return flag;
}

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


