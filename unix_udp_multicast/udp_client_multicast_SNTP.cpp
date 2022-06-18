

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
//for inet_addr
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
（5）离开多播组。            */

void sock_set_wild(const struct sockaddr *sa, const socklen_t &salen);
void sntp_proc(const char *buf, ssize_t num, const struct timeval *nowptr);

void udp_client_multicast_SNTP()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int sock_fd; // 套接字描述符
	socklen_t sa_len = 0;
	struct sockaddr_in client_addr{};
	struct sockaddr_in wild{};
	bzero(&client_addr, sizeof(client_addr)); // 清 0
	bzero(&wild, sizeof(wild)); // 清 0

	_debug_log_info("套接字初始化")
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	/* ---- 配置 ip 地址 端口 信息 ---- */
	sock_fd = udp_socket_config((struct sockaddr *) &client_addr, &sa_len,
								"224.0.1.1", "sntp", AF_INET,
								SOCK_DGRAM, IPPROTO_IP,
								AI_CANONNAME | AI_NUMERICHOST);

	sock_set_wild((struct sockaddr *) &wild, sa_len);
	err_sys(bind(sock_fd, (struct sockaddr *) &wild, sa_len),
			__LINE__,"setsockopt()");

	/* obtain interface list and process each one */
	std::unique_ptr<struct if_info []> if_info_ary
			= get_if_info(sock_fd, client_addr.sin_family);
	for (struct if_info * if_i = if_info_ary.get() ;
		 if_i != nullptr;
		 if_i = if_i->if_next )
	{
		if (if_i->if_flags & IFF_MULTICAST/*支持多播*/) {
			_debug_log_info("加入多播组")
			Mcast_join(sock_fd, (struct sockaddr *)&client_addr, sa_len,
					   if_i->if_name, 0);
			std::cout << "joined "
					  << get_addr_port_ntop((struct sockaddr *)&client_addr).c_str()
					  << " on " << if_i->if_name/*网口名*/ << std::endl;
		}
	}

	_debug_log_info("recvfrom()")
	ssize_t num;
	socklen_t len = 0;
	char buf[MAXLINE];
	struct timeval now{};
	struct sockaddr_in from{};
	while (true) {
		len = sa_len;
		num = recvfrom(sock_fd, buf, sizeof(buf), 0,
					   (struct sockaddr *)&from, &len);
		gettimeofday(&now, nullptr);
		sntp_proc(buf, num, &now);
	}

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}

/* wildcard 通配符 */
void sock_set_wild(const struct sockaddr *sa, const socklen_t &salen)
{
	switch (sa->sa_family) {
		case AF_INET: {
			auto *sin = (struct sockaddr_in *)sa;
			sin->sin_addr.s_addr = htonl(INADDR_ANY);
			break;
		}
		case AF_INET6: {
			auto *sin6 = (struct sockaddr_in6 *)sa;
			memcpy(&sin6->sin6_addr, &in6addr_any, sizeof(struct in6_addr));
			break;
		}
		default: break;
	}
}

#define	JAN_1970	2208988800UL	/* 1970 - 1900 in seconds */
#define	VERSION_MASK	0x38
#define	MODE_MASK		0x07
#define	MODE_CLIENT		3
#define	MODE_SERVER		4
#define	MODE_BROADCAST	5

/* Network Time Protocol（NTP）协议是用来使计算机时间同步化的一种协议，
 * 它可以使计算机对其服务器或时钟源（如石英钟，GPS等）做同步化，
 * 它可以提供高精确度的时间校正（LAN上与标准时间差小于1毫秒，WAN上几十毫秒），
 * 且可用加密确认的 方式来防止协议攻击。时间应该是国际标准时间 UTC。
 * NTP(Network Time Protocol）网络时间协议基于UDP，用于网络时间同步的协议，
 * 使网络中的计算机时钟同步到UTC，再配合各个时区的偏移调整就能实现精准同步对时功能。
 * 提供NTP对时的服务器有很多，比如微软的NTP对时服务器，利用NTP服务器提供的对时功能，
 * 可以使我们的设备时钟系统能够正确运行。*/

/**	Service ---------- T2 -- T3 ------------------->
                 //                 \\
 	Client  --- T1 ----------------- T4 ----------->
 网络的往返延迟 delay = (T4-T1) - (T3-T2)
 客户端与服务端的时间校正 correction = T3 + delay/2           */

/** NTP 数据 报文 **/
/* NTP 数据 报文格式 如图所示:
 0   2    5      8          16        24     31
---------------------------------------------------------------
 01  234  567    89012345   67890123  45678901
 LI  VN   Mode   Stratum    Poll      Precision
---------------------------------------------------------------
 32 bits	Root Delay 根时延，表示在到主参考时钟的总往返的时间。
---------------------------------------------------------------
 32 bits	Root Dispersion 根离散，本地时钟相对于主参考时钟的最大误差。
---------------------------------------------------------------
 32 bits	Reference Identifier 标识特定参考时钟，用来标识特殊的参考源
---------------------------------------------------------------
 64 bits	(T4)Reference timestamp 参考时间戳，客户端 被修改的 最新时间。
 			如果值为0表示本地时钟从未被同步过
---------------------------------------------------------------
 64 bits	(T1)originate timestamp 原始时间戳，客户端 发送的时间，
 			NTP报文离开源端时的时间
---------------------------------------------------------------
 64 bits	(T2)receive timestamp 接受时间戳，服务端 接受到的时间，
 			NTP报文到达目的端的时间
---------------------------------------------------------------
 64 bits	(T3)transmit timestamp 传送时间戳，服务端 送出应答的时间，
 			目的端应答报文离开服务器端的时间
---------------------------------------------------------------
 96 bits 	Authenticator（可选项）验证信息
---------------------------------------------------------------------------
字段含义:
 LI 	占用2个bit，闰秒标识器 leap_year_indicator。
 值为“11”时表示告警状态，时钟不能被同步。
--------------------------------------------
 VN 	占用3个bits，NTP 版本号 Version Number
--------------------------------------------
 Mode 	占用3个bits，NTP 工作模式:
不同值表示的含义如下：
 0：reserved，保留。
 1：symmetric active，主动对等体模式。
 2：symmetric passive，被动对等体模式。
 3：client，客户模式。
 4：server，服务器模式。
 5：broadcast，广播模式。
 6：reserved for NTP control messages，NTP控制报文。
 7：reserved for private use，内部使用预留。
--------------------------------------------
 stratum 占用8个bits，时钟的层数，定义了时钟的准确度。
 		层数为1的时钟准确度最高，从1到15依次递减。
--------------------------------------------
 Poll 	占用8个bits，轮询时间，即发送报文的最小间隔时间。
 Precision 占用8个bits，本地时钟精度。
-------------------------------------------- */

/** NTP 控制 报文 **/
/* NTP 控制 报文格式  如图所示:
 0   2    5     8     11     16              31
---------------------------------------------------------------
 01  234  567   890   12345   6789012345678901
 00  VN   110   REM   Op      Sequence
---------------------------------------------------------------
   Status 当前系统的状态      |    Association ID 连接标示
---------------------------------------------------------------
   Offset 偏移量             |    Count 数据域的长度
---------------------------------------------------------------
   Data 包括发送报文或接受报文中的数据信息  最大 468 bits
                             |    Padding 填充字段 16 bits
---------------------------------------------------------------
 96 bits 	Authenticator（可选项）验证信息
---------------------------------------------------------------
字段含义:
 00 	保留位。NTP本身不做处理。
 VN 	占用3个bits，NTP 版本号 Version Number
 6 		占用3个bits，表明是控制报文。
--------------------------------------------
 REM 	占用3个bits，
 R：0表示命令，1表示响应。
 E：0表示发送正常响应，1表示发送错误响应。
 M：0表示最后一个分片，1表示其他。
--------------------------------------------
 Op 	占用5个bits，操作码，表明命令的类型。
 Sequence 	占用16个bits，发送或接受到报文的顺序号。
-------------------------------------------- */

struct s_fixedpt {		/* 32-bit fixed-point */
	uint16_t int_part; // 整数部分
	uint16_t fraction; // 小数部分
};
struct ntptime_t {		/* 64-bit fixed-point */
	uint32_t int_part; // 整数部分
	uint32_t fraction; // 小数部分
};

struct NTPdata {		/** 精简版 **/
	/** header */
	uint8_t 	LI_VN_MODE{};
	uint8_t 	stratum{};
	uint8_t 	poll{};
	uint8_t 	precision{};

	struct s_fixedpt root_delay{}; 		// 总往返的时间
	struct s_fixedpt root_dispersion{}; 	// 本地时钟相对于主参考时钟的最大误差
	struct s_fixedpt reference_identifier{}; 	// 标识特定参考时钟

	/** 时间戳 */
	struct ntptime_t reference_timestamp{}; //T4 客户端 参考时间戳
	struct ntptime_t originate_timestamp{}; //T1 客户端 原始时间戳
	struct ntptime_t receive_timestamp{};   //T2 服务端 接受时间戳
	struct ntptime_t transmit_timestamp{};  //T3 服务端 传送时间戳
};

struct ntp_packet_t { 		/** 协议字段版 **/
	/** header */
	uint8_t leap_year_indicator:2; 	// 闰秒标识器
	uint8_t version_number:3; 		// NTP 版本号
	uint8_t mode:3; 			// NTP 工作模式
	uint8_t stratum :8; 		// 时钟的层数
	uint8_t poll :8; 			// 轮询时间
	uint8_t precision :8; 		// 本地时钟精度

	struct s_fixedpt root_delay; 		// 总往返的时间
	struct s_fixedpt root_dispersion; 	// 本地时钟相对于主参考时钟的最大误差
	struct s_fixedpt reference_identifier; 	// 标识特定参考时钟

	/** 时间戳 */
	struct ntptime_t reference_timestamp;  //T4 客户端 参考时间戳
	struct ntptime_t originate_timestamp;  //T1 客户端 原始时间戳
	struct ntptime_t receive_timestamp;    //T2 服务端 接受时间戳
	struct ntptime_t transmit_timestamp;   //T3 服务端 传送时间戳
};

void sntp_proc(const char *buf, ssize_t num, const struct timeval *nowptr)
{
	int				version, mode;
	uint32_t		nsec, useci;
	double			usecf;
	struct timeval	diff{};

	if (num < (ssize_t)sizeof(struct NTPdata)) {
		std::cout.flush();
		std::cerr << "packet too small: " << num << " bytes" << std::endl;
		std::cerr.flush();
		return;
	}

	auto ntp = (struct NTPdata *) buf;

	version = (ntp->LI_VN_MODE & VERSION_MASK) >> 3;
	mode = ntp->LI_VN_MODE & MODE_MASK;
	std::cout << "version: " << version << ", "
			  << "mode: " << mode << ", "
			  << "strat: " << ntp->stratum << std::endl;

	if (mode == MODE_CLIENT) {
		printf("client\n");
		return;
	}

	nsec = ntohl(ntp->transmit_timestamp.int_part) - JAN_1970;
	useci = ntohl(ntp->transmit_timestamp.fraction);	/* 32-bit integer fraction */
	usecf = useci;				/* integer fraction -> double */
	usecf /= 4294967296.0;		/* divide by 2**32 -> [0, 1.0) */
	useci = usecf * 1000000.0;	/* fraction -> parts per million */

	diff.tv_sec = nowptr->tv_sec - nsec;
	if ( (diff.tv_usec = nowptr->tv_usec - useci) < 0) {
		diff.tv_usec += 1000000;
		diff.tv_sec--;
	}
	useci = (diff.tv_sec * 1000000) + diff.tv_usec;	/* diff in microsec */
	printf("clock difference = %d usec\n", useci);
}
