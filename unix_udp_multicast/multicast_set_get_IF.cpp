//
// Created by zcq on 2021/4/18.
//

#include "multicast.h"


/*************************************************************************/

/* 指定外出接口 */
void Mcast_set_IF(const int &sock_fd, const char *ifname, const u_int &ifindex)
{
	err_sys(mcast_set_IF(sock_fd, ifname, ifindex),
			__FILE__, __func__, __LINE__, "%s()", __func__);
}

/* 指定外出接口 */
int mcast_set_IF(const int &sock_fd, const char *ifname, const u_int &ifindex)
{
	switch (Sockfd_to_family(sock_fd))
	{
		case AF_INET: {
			struct in_addr inaddr{};
			struct ifreq ifreq{}; // 网口请求

			if (ifindex > 0) {
				/* if_indextoname(): Convert an index to an interface name */
				if (if_indextoname(ifindex, ifreq.ifr_name) == nullptr) {
					errno = ENXIO; /* No such interface name */
					return(-1);
				}
				goto doioctl;
			}
			else if (ifname != nullptr) {
				strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
doioctl:
				if (ioctl(sock_fd, SIOCGIFADDR/*获取接口地址*/, &ifreq) < 0) return(-1);
				memcpy(&inaddr,
					   &((struct sockaddr_in *) &ifreq.ifr_addr)->sin_addr,
					   sizeof(struct in_addr));
			}
			else inaddr.s_addr = htonl(INADDR_ANY);	/* remove prev. set default */

			return(setsockopt(sock_fd, IPPROTO_IP, IP_MULTICAST_IF/*外出接口*/,
							  &inaddr, sizeof(struct in_addr)));
		}

		case AF_INET6: {
			u_int idx = ifindex;
			if (ifindex == 0) /* must supply either index or name */
			{
				if (ifname == nullptr) {
					errno = EINVAL;
					return(-1);
				}
				if ( (idx = if_nametoindex(ifname)) == 0) {
					errno = ENXIO; /* No such interface name */
					return(-1);
				}
			}
			return(setsockopt(sock_fd, IPPROTO_IPV6,
							  IPV6_MULTICAST_IF/*外出接口*/, &idx, sizeof(idx)));
		}

		default: errno = EAFNOSUPPORT; /* Address family not supported by protocol */
			return(-1);
	}
}

/*************************************************************************/

/* 指定外出接口 */
int Mcast_get_IF(const int &sock_fd)
{
	int retval = mcast_get_IF(sock_fd);
	err_sys(retval, __FILE__, __func__, __LINE__, "%s()", __func__);
	return retval;
}

/* 指定外出接口 */
int mcast_get_IF(const int &sock_fd)
{
	switch (Sockfd_to_family(sock_fd))
	{
		case AF_INET: {
			struct in_addr inaddr{};
			socklen_t len = sizeof(inaddr);
			if (getsockopt(sock_fd, IPPROTO_IP, IP_MULTICAST_IF/*外出接口*/,
						   &inaddr, &len)) return (-1);
			return (in_addr_t)inaddr.s_addr;
		}

		case AF_INET6: {
			u_int idx;
			socklen_t len = sizeof(idx);
			if (getsockopt(sock_fd, IPPROTO_IPV6,
						   IPV6_MULTICAST_IF/*外出接口*/, &idx, &len) < 0) return(-1);
			return (u_int)idx;
		}

		default: errno = EAFNOSUPPORT; /* Address family not supported by protocol */
			return(-1);
	}
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


