

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
（5）离开多播组。            */

void udp_multicast()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int TX_sockfd, RX_sockfd; // 套接字描述符
	socklen_t sa_len = 0;
	struct sockaddr_in client_addr{};
	bzero(&client_addr, sizeof(client_addr)); // 清 0

	_debug_log_info("套接字初始化: 发送")
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	/* ---- 配置 ip 地址 端口 信息 ---- */
	TX_sockfd = udp_socket_config((struct sockaddr *) &client_addr, &sa_len,
								  HOST_Multicast, PORT_Multicast_STR, AF_INET);

	_debug_log_info("套接字初始化: 接收")
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	/* ---- 配置 ip 地址 端口 信息 ---- */
	/* ---- bind () ---- */
	RX_sockfd = udp_socket_bind_config(nullptr, nullptr,
									   HOST_Multicast, PORT_Multicast_STR, AF_INET);

	_debug_log_info("设置多播的参数")
	Mcast_set_LOOP(TX_sockfd, 0); // 0:禁止回馈

	_debug_log_info("加入多播组")
	Mcast_join(RX_sockfd, (struct sockaddr *)&client_addr, sa_len,
			   nullptr, 0);

/*******************************************************************/
	/* ---- Signal () ---- */
	// SIGCHLD : 父进程 捕捉 子进程 的状态信号。
	Signal_fork(SIGCHLD);	/* must call waitpid() */
/*******************************************************************/

	_debug_log_info("发送和接收数据")
	/* ---- 创建 子进程 ---- */
	pid_t child_pid = fork(); // 创建 子进程
	if (child_pid == -1) // 负值 : 创建子进程失败。
	{
		err_ret(child_pid, __LINE__, "创建子进程失败");
	}
	else if (child_pid == 0) // 0 : 当前是 子进程 运行。
	{
		err_sys(close(TX_sockfd), __LINE__, "close()");

		recv_all(RX_sockfd, sa_len);

		_debug_log_info("离开多播组")
		Mcast_leave(RX_sockfd, (struct sockaddr *)&client_addr, sa_len);

		err_sys(close(RX_sockfd), __LINE__, "close()");

		/* 子进程异常，通知父进程退出 */
		kill(getppid()/* 获取父进程ID */, SIGTERM/* 终止请求 */);
		exit(0);
	}
	else // 正值 : 当前是 父进程 运行。正值 为 子进程 的 进程 ID。
	{
		std::cout << "创建子进程 child ID: " << child_pid << std::endl;
		err_sys(close(RX_sockfd), __LINE__, "close()");

		send_all(TX_sockfd, (struct sockaddr *)&client_addr, sa_len);

		err_sys(close(TX_sockfd), __LINE__, "close()");
	}
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}

