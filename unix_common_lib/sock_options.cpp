
#include "zcq_header.h"


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
level	optname				get set 说明						标志 数据类型
---------------------------------------------------------------------------
SOL_SOCKET	(通用选项)
 		SO_BROADCAST		*	*	允许发送广播数据报			*	int
		SO_DEBUG			*	*	使能调试跟踪					*	int
		SO_DONTROUTE		*	*	不查找路由					*	int
		SO_ERROR			*		获取待处理错误并消除				int
		SO_KEEPALIVE		*	*	周期性测试连接是否存活			*	int
		SO_LINGER			*	*	若有数据待发送则延迟关闭			linger{}
		SO_OOBINLINE		*	*	让接收到的带外数据继续在线存放	*	int
		SO_RCVBUF			*	*	接收缓冲区大小					int
		SO_SNDBUF			*	*	发送缓冲区大小					int
		SO_RCVLOWAT			*	*	接收缓冲区下限					int
		SO_SNDLOWAT			*	*	发送缓冲区下限					int
		SO_RCVTIMEO			*	*	接收超时							timeval{}
		SO_SNDTIMEO			*	*	发送超时							timeval{}
		SO_REUSEADDR		*	*	允许重用本地地址				*	int
		SO_REUSEPORT		*	*	允许重用本地端口				*	int
		SO_TYPE				*		取得套接口类型					int
		SO_USELOOPBACK		*	*	路由套接口取得所发送数据的副本	*	int
		SO_xxx
---------------------------------------------------------------------------
IPPROTO_IP	(IPv4 选项)
 		IP_HDRINCL			*	*	IP头部包括数据				*	int
		IP_OPTIONS			*	*	IP头部选项						见后面说明
		IP_RECVDSTADDR		*	*	返回目的IP地址				*	int
		IP_RECVIF			*	*	返回接收到的接口索引			*	int
		IP_TOS				*	*	服务类型和优先权					int
		IP_TTL				*	*	存活时间							int
		IP_MULTICAST_IF		*	*	指定外出接口						in_addr{}
		IP_MULTICAST_TTL	*	*	指定外出TTL						u_char
		IP_MULTICAST_LOOP	*	*	指定是否回馈						u_char
		IP_ADD_MEMBERSHIP		*	加入多播组						ip_mreq{}
		IP_DROP_MEMBERSHIP		*	离开多播组						ip_mreq{}
		IP_BLOCK_SOURCE			*	阻塞多播源						ip_mreq_source{}
		IP_UNBLOCK_SOURCE		*	开通多播源						ip_mreq_source{}
		IP_ADD_SOURCE_MEMBERSHIP *	加入源特定多播组					ip_mreq_source{}
		IP_DROP_SOURCE_MEMBERSHIP *	离开源特定多播组					ip_mreq_source{}
		IP_xxx
---------------------------------------------------------------------------
IPPROTO_ICMPV6
 		ICMP6_FILTER		*	*	指定传递的ICMPv6消息类型		icmp6_filter{}
---------------------------------------------------------------------------
IPPROTO_IPV6	(IPv6 选项)
		IPV6_CHECKSUM		*	*	原始套接口的校验和字段偏移			int
		IPV6_NEXTHOP		*	*	指定下一跳 NEXT HOP 地址		*	sockaddr_in6{}
 		IPV6_ADDRFORM		*	*	改变套接口的地址结构				int
		IPV6_RECVDSTOPTS	*	*	接收目标选项					*	int
		IPV6_RECVHOPLIMIT	*	*	接收单播跳限					*	int
		IPV6_RECVHOPOPTS	*	*	接收步跳选项					*	int
		IPV6_RECVPKTINFO	*	*	接收分组信息					*	int
		IPV6_PKTOPTIONS		*	*	指定分组选项						见后面说明
		IPV6_RECVRTHDR		*	*	接收原路径					*	int
		IPV6_UNICAST_HOPS	*	*	默认单播跳限						int
		IPV6_MULTICAST_IF	*	*	指定外出接口						u_int
		IPV6_MULTICAST_HOPS	*	*	指定外出跳限						int
		IPV6_MULTICAST_LOOP	*	*	指定是否回馈					*	u_int
		IPV6_ADD_MEMBERSHIP	 	*	加入多播组						ipv6_mreq{}
		IPV6_DROP_MEMBERSHIP 	*	离开多播组						ipv6_mreq{}
		IPV6_xxx
---------------------------------------------------------------------------
IPPROTO_TCP	(TCP 选项)
 		TCP_KEEPALIVE		*	*	控测对方是否存活前连接闲置秒数		int
		TCP_MAXRT			*	*	TCP最大重传时间					int
		TCP_MAXSEG			*	*	TCP最大分节大小					int
		TCP_NODELAY			*	*	禁止Nagle算法				*	int
		TCP_STDURG			*	*	紧急指针的解释				*	int
		TCP_xxx
---------------------------------------------------------------------------
 */


/************* 检查选项 **************/
/*
 1）SO_KEEPALIVE说明
	给一个TCP套接字设置保持存活选项后，如果2小时内在该套接字的任何一方向上都没有数据交换，
 TCP就自动给对端发送一个保持存活探测分节。这是一个对端必须相应的TCP分节，它会导致以下3种情况之一。
    对端以期望的ACK响应。应用进程得不到通知（因为一切正常）。
    在又经过仍无动静的2小时后，TCP将发出另一个探测分节。
    对端以RST响应，它告知本端TCP：对端已崩溃且已重新启动。
    该套接字的待处理错误被置为ECONNRESET，套接字本身则被关闭。
    对端对保持存活探测分节没有任何响应。 如果根本没有对TCP的探测分节的响应，
    该套接字的待处理错误就被置为ETIMEOUT，套接字本身则被关闭。
    然而如果该套接字收到一个ICMP错误作为某个探测分节的响应，
    那就返回响应的错误，套接字本身也被关闭。
    本选项的功能是检测对端主机是否崩溃或变的不可达
    （譬如拨号调制解调器连接掉线，电源发生故障等等）。
    如果对端进程崩溃，它的TCP将跨连接发送一个FIN，这可以通过调用select很容易的检测到。
2）SO_LINGER
 	本选项指定close函数对面向连接的协议（例如TCP和SCTP，但不是UDP）如何操作。
 	默认操作是close立即返回，但是如果有数据残留在套接字发送缓冲区中，
 	系统将试着把这些数据发送给对端。SO_LINGER如果选择此选项，
 	close或 shutdown将等到所有套接字里排队的消息成功发送或到达延迟时间后才会返回。
 	否则，调用将立即返回。
 */

#if 0

#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>	/* for TCP_xxx defines */
#include <string>
using namespace std;


union val {
	int				i_val;
	long 			l_val;
	struct 	linger 	linger_val;
	struct 	timeval timeval_val;
} opt_val;

static char *sock_str_flag		(union val *ptr, int len);
static char *sock_str_int		(union val *ptr, int len);
static char *sock_str_linger	(union val *ptr, int len);
static char *sock_str_timeval	(union val *ptr, int len);

struct sock_opts {
	const char *opt_name_str;
	int			opt_level;
	int			opt_name;
	char 		*(*opt_val_str)(union val *, int);
}sock_opts[]={
		/*        opt_str           opt_level   opt_name        value_type */
		{ "SO_BROADCAST",	SOL_SOCKET,	SO_BROADCAST,	sock_str_flag },
		{ "SO_DEBUG",		SOL_SOCKET,	SO_DEBUG,		sock_str_flag },
		{ "SO_DONTROUTE",	SOL_SOCKET,	SO_DONTROUTE,	sock_str_flag },
		{ "SO_ERROR",		SOL_SOCKET,	SO_ERROR,		sock_str_int },
		{ "SO_KEEPALIVE",	SOL_SOCKET,	SO_KEEPALIVE,	sock_str_flag },
		{ "SO_LINGER",		SOL_SOCKET,	SO_LINGER,		sock_str_linger },
		{ "SO_OOBINLINE",	SOL_SOCKET,	SO_OOBINLINE,	sock_str_flag },
		{ "SO_RCVBUF",		SOL_SOCKET,	SO_RCVBUF,		sock_str_int },
		{ "SO_SNDBUF",		SOL_SOCKET,	SO_SNDBUF,		sock_str_int },
		{ "SO_RCVLOWAT",	SOL_SOCKET,	SO_RCVLOWAT,	sock_str_int },
		{ "SO_SNDLOWAT",	SOL_SOCKET,	SO_SNDLOWAT,	sock_str_int },
		{ "SO_RCVTIMEO",	SOL_SOCKET,	SO_RCVTIMEO,	sock_str_timeval },
		{ "SO_SNDTIMEO",	SOL_SOCKET,	SO_SNDTIMEO,	sock_str_timeval },
		{ "SO_REUSEADDR",	SOL_SOCKET,	SO_REUSEADDR,	sock_str_flag },
#ifdef	SO_REUSEPORT
		{ "SO_REUSEPORT",	SOL_SOCKET,	SO_REUSEPORT,	sock_str_flag },
#else
		{ "SO_REUSEPORT",	0,		0,		NULL },
#endif
		{ "SO_TYPE",		SOL_SOCKET,	SO_TYPE,	sock_str_int },
		{ "SO_USELOOPBACK",	SOL_SOCKET,	0,	sock_str_flag },
		{ "IP_TOS", 		IPPROTO_IP,	IP_TOS,		sock_str_int },
		{ "IP_TTL", 		IPPROTO_IP,	IP_TTL,		sock_str_int },
#ifdef	IPV6_DONTFRAG
		{ "IPV6_DONTFRAG",IPPROTO_IPV6,
		  IPV6_DONTFRAG,	sock_str_flag },
#else
		{ "IPV6_DONTFRAG",	0,		0,		NULL },
#endif
#ifdef	IPV6_UNICAST_HOPS
		{ "IPV6_UNICAST_HOPS",	IPPROTO_IPV6,
		  IPV6_UNICAST_HOPS,sock_str_int },
#else
		{ "IPV6_UNICAST_HOPS",	0,		0,		NULL },
#endif
#ifdef	IPV6_V6ONLY
		{ "IPV6_V6ONLY",	IPPROTO_IPV6,
		  IPV6_V6ONLY,	sock_str_flag },
#else
		{ "IPV6_V6ONLY",	0,		0,		NULL },
#endif
		{ "TCP_MAXSEG",	IPPROTO_TCP,TCP_MAXSEG,sock_str_int },
		{ "TCP_NODELAY",IPPROTO_TCP,TCP_NODELAY,sock_str_flag },
#ifdef	SCTP_AUTOCLOSE
		{ "SCTP_AUTOCLOSE",	IPPROTO_SCTP,SCTP_AUTOCLOSE,sock_str_int },
#else
		{ "SCTP_AUTOCLOSE",0,0,nullptr },
#endif
#ifdef	SCTP_MAXBURST
		{ "SCTP_MAXBURST",	IPPROTO_SCTP,SCTP_MAXBURST,	sock_str_int },
#else
		{ "SCTP_MAXBURST",0,0,nullptr },
#endif
#ifdef	SCTP_MAXSEG
		{ "SCTP_MAXSEG",	IPPROTO_SCTP,SCTP_MAXSEG,	sock_str_int },
#else
		{ "SCTP_MAXSEG",0,0,nullptr },
#endif
#ifdef	SCTP_NODELAY
		{ "SCTP_NODELAY",	IPPROTO_SCTP,SCTP_NODELAY,	sock_str_flag },
#else
		{ "SCTP_NODELAY",0,0,nullptr },
#endif
		{ nullptr,	0,0,nullptr }
};

/************* 检查选项 **************/

/*	bash shell 	------------------------
	$ g++ test.cpp -o test
	$ ./test myfile.txt youfile.txt one.txt
	c++ 	------------------------
	int main(int argc, char **argv)
----------------------------------------
  	c++    	bash shell
  	argc	=	${#argv[@]} # 参数个数
  	argv[0]	=	myfile.txt 	# 参数 : ./当前文件名
  	argv[1]	=	myfile.txt 	# 参数 1
  	argv[2]	=	youfile.txt # 参数 2
  	argv[3]	=	one.txt 	# 参数 3	  	*/
#if 0
int main(int argc, char **argv) {
	int sock_fd;
	socklen_t opt_len;
	struct sock_opts *ptr;
	cout << "" << endl;
	for (ptr = sock_opts; ptr->opt_name_str != nullptr; ++ptr) {
		switch (ptr->opt_level) {
			case SOL_SOCKET:
			case IPPROTO_IP:
			case IPPROTO_TCP:
				sock_fd = socket(AF_INET, SOCK_STREAM, 0);
				err_quit(sock_fd, __LINE__, "socket()");
				break;
#ifdef    IPV6
				case IPPROTO_IPV6:
					fd = socket(AF_INET6, SOCK_STREAM, 0);
					break;
#endif
#ifdef    IPPROTO_SCTP
			case IPPROTO_SCTP:
				sock_fd = socket(AF_INET, SOCK_SEQPACKET,
								 IPPROTO_SCTP);
				err_quit(sock_fd, __LINE__, "socket()");
				break;
#endif
			default:
				err_quit(-1, __LINE__,
						 "Can't create fd for level %d", ptr->opt_level);
				break;
		}
		if (ptr->opt_val_str != nullptr) {
			opt_len = sizeof(opt_val);
			if (getsockopt(sock_fd, ptr->opt_level, ptr->opt_name,
						   &opt_val, &opt_len) == -1) {
				err_quit(-1, __LINE__, "getsockopt():%s:%d",
						 ptr->opt_name_str, ptr->opt_name);
			} else {
				switch (ptr->opt_level) {
					case SOL_SOCKET:
						cout << "SOL_SOCKET\t";
						break;
					case IPPROTO_IP:
						cout << "IPPROTO_IP\t";
						break;
					case IPPROTO_TCP:
						cout << "IPPROTO_TCP\t";
						break;
					case IPPROTO_SCTP:
						cout << "IPPROTO_SCTP\t";
						break;
					default:
						cout << "\t\t";
						break;
				}
				cout << ptr->opt_level << "\t"
					 << ptr->opt_name_str << "\t\t\t"
					 << ptr->opt_name << "\t\t\t\t";
				if (ptr->opt_val_str == nullptr)
					cout << "undefined" << endl;
				else cout << *ptr->opt_val_str(&opt_val, opt_len) << endl;
			}
		}
		err_sys(close(sock_fd), __LINE__, "close()");
	}
	return 0;
}
#endif


/* include checkopts3 */
static char strres[128];

static char *sock_str_flag(union val *ptr, int len)
{
/* *INDENT-OFF* */
	if (len != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
	else
		snprintf(strres, sizeof(strres),
				 "%s", (ptr == nullptr) ? "off" : "on");
	return(strres);
/* *INDENT-ON* */
}
/* end checkopts3 */

static char *sock_str_int(union val *ptr, int len)
{
	if (len != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
	else
		snprintf(strres, sizeof(strres), "%d", ptr->i_val);
	return(strres);
}

static char *sock_str_linger(union val *ptr, int len)
{
	struct linger	*lptr = &ptr->linger_val;

	if (len != sizeof(struct linger))
		snprintf(strres, sizeof(strres),
				 "size (%d) not sizeof(struct linger)", len);
	else
		snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d",
				 lptr->l_onoff, lptr->l_linger);
	return(strres);
}

static char *sock_str_timeval(union val *ptr, int len)
{
	struct timeval	*tvptr = &ptr->timeval_val;

	if (len != sizeof(struct timeval))
		snprintf(strres, sizeof(strres),
				 "size (%d) not sizeof(struct timeval)", len);
	else
		snprintf(strres, sizeof(strres), "%ld sec, %ld usec",
				 tvptr->tv_sec, tvptr->tv_usec);
	return(strres);
}

#endif

