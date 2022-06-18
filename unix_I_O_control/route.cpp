
#include "IO_ioctl_socket.h"

/** I/O control 控制操作   #include <unistd.h> or #include <sys/ioctl.h>
 	网络程序(一般是服务器程序)中 ioctl()常用于在程序启动时获得主机上所有接口的信息：
 接口的地址、接口是否支持广播、是否支持多播 等等。
---------------------------------------------------------------------------
 int ioctl (int sock_fd, unsigned long int request, ...); // I/O 控制操作
 返回：成功返回 0，出错返回 -1
---------------------------------------------------------------------------
 	下表列出了 ioctl() 参数 request 以及 地址 ... 必须指向的数据类型：
 类别		Request				说明						数据类型
- - - -
 套接字		SIOCATMARK			是否位于带外标记				int
			SIOCSPGRP			设置 套接字的进程ID或进程组ID	int
			SIOCGPGRP			获取 套接字的进程ID或进程组ID	int
- - - -
 文件		FIONBIN				设置/清除 非阻塞I/O标志		int
			FIOASYNC			设置/清除 信号驱动异步I/O标志	int
			FIONREAD			获取 接收缓存区中的字节数		int
			FIOSETOWN			设置 文件的进程ID或进程组ID	int
			FIOGETOWN			获取 文件的进程ID或进程组ID	int
- - - -
(网络)接口	SIOCGIFNAME			获取 接口名字(网口名)
			SIOCSIFNAME			设置 接口名称(网口名)
			SIOCSIFLINK			设置 接口频道
			SIOCGIFCONF			获取 接口清单(含网口名)		struct ifconf
			SIOCDIFADDR			删除 接口地址(IP)			struct ifreq
			SIOCSIFADDR			设置 接口地址(IP)			struct ifreq
			SIOCGIFADDR			获取 接口地址(IP)			struct ifreq
			SIOCSIFFLAGS		设置 接口状态标志				struct ifreq
			SIOCGIFFLAGS		获取 接口状态标志				struct ifreq
			SIOCSIFDSTADDR		设置 远程地址(点到点)			struct ifreq
			SIOCGIFDSTADDR		获取 远程地址(点到点)			struct ifreq
			SIOCGIFBRDADDR		获取 广播地址(broadcast)		struct ifreq
			SIOCSIFBRDADDR		设置 广播地址(broadcast)		struct ifreq
			SIOCGIFNETMASK		获取 子网掩码(network mask)	struct ifreq
			SIOCSIFNETMASK		设置 子网掩码(network mask)	struct ifreq
			SIOCGIFMETRIC		获取 接口测度				struct ifreq
			SIOCSIFMETRIC		设置 接口测度				struct ifreq
			SIOCGIFMTU			获取 接口 MTU 最大传输单元	struct ifreq
			SIOCSIFMTU			设置 接口 MTU 最大传输单元	struct ifreq
			SIOCGIFMEM			获取 内存地址（BSD）
			SIOCSIFMEM			设置 内存地址（BSD）
			SIOCSIFHWADDR		设置 以太网/主机硬件地址(MAC机器码)
			SIOCGIFHWADDR		获取 以太网/主机硬件地址(MAC机器码)
			SIOCADDMULTI		增加 组播地址列表
			SIOCDELMULTI		删除 组播地址列表
			SIOCSIFPFLAGS		设置 扩展标志集
			SIOCGIFPFLAGS		获取 扩展标志集
			SIOCSIFHWBROADCAST	设置 硬件广播地址
			SIOCGIFCOUNT		获取 设备数量
			SIOCGIFBR			获取 桥接支持
			SIOCSIFBR			设置 桥接选项
			SIOCGIFTXQLEN		获取 发送队列长度
			SIOCSIFTXQLEN		设置 发送队列长度
			SIOCGIFBR			桥接支持
			SIOCSIFBR			设置 桥接选项
			SIOCxxx				（还有很多取决于系统的实现，见<ioctls.h>）
- - - -
 ARP高速缓存	SIOCSARP			设置 ARP 表项				struct arpreq
			SIOCGARP			获取 ARP 表项				struct arpreq
			SIOCDARP			删除 ARP 表项				struct arpreq
- - - -
 RARP 缓存 	SIOCSRARP			设置 ARP 表项
			SIOCGRARP			获取 ARP 表项
			SIOCDRARP			删除 ARP 表项
- - - -
 路由		SIOCADDRT			增加 路由表条目				struct rtentry
			SIOCDELRT			删除 添加路由表条目			struct rtentry
			SIOCRTMSG			呼叫 路由系统
- - - -
 网卡		SIOCGIFMAP			获取 网卡映射参数
			SIOCSIFMAP			设置 网卡映射参数
 			SIOCGIFINDEX   		获取 网卡序号(接口索引 if_index)
- - - -
DLCI 		SIOCADDDLCI			创建 新 DLCI 设备
			SIOCDELDLCI			删除 DLCI 设备
- - - -
 流			I_xxx				（见书 31.5 节）              **/
/*************************************************************************/

/* -------------------------------------------------------------------------
 route [-nee]
 route add [-net|-host] [网域或主机] netmask [mask] [gw|dev]
 route del [-net|-host] [网域或主机] netmask [mask] [gw|dev]
参数：
   -v : 显示详细信息
   -n ：不要使用通讯协定或主机名称，直接使用 IP 或 port number；
   -e ：显示更多信息
增加 (add) 与删除 (del) 路由的相关参数：
   -net    ：表示后面接的路由为一个网域；
   -host   ：表示后面接的为连接到单部主机的路由；
   netmask ：与网域有关，可以设定 netmask 决定网域的大小；
   gw      ：gateway 的简写，后续接的是 IP 的数值喔，与 dev 不同；
   dev     ：如果只是要指定由那一块网路卡连线出去，则使用这个设定，后面接 eth0 等
---------------------------------------------------------------------------
$ route -vne
内核 IP 路由表
目标         网关          子网掩码         标志  跃点   引用  使用  接口
0.0.0.0      192.168.6.1  0.0.0.0         UG    100   0     0    enp3s0
172.17.0.0   0.0.0.0      255.255.0.0     U     0     0     0    docker0
192.168.6.0  0.0.0.0      255.255.255.0   U     100   0     0    enp3s0
---------------------------------------------------------------------------
 route -vne6
内核 IPv6 路由表
Destination                    Next Hop/跃点          Flag Met Ref Use If
::/0                           ::                     !n   -1  1     0 lo
::1/128                        ::                     U    256 1     0 lo
fe80::/64                      ::                     U    100 1     0 enp3s0
::/0                           ::                     !n   -1  1     0 lo
::1/128                        ::                     Un   0   8     0 lo
fe80::71e2:5264:8af7:b70a/128  ::                     Un   0   2     0 enp3s0
ff00::/8                       ::                     U    256 7     0 enp3s0
::/0                           ::                     !n   -1  1     0 lo
---------------------------------------------------------------------------
$ routel
         target          gateway        source    proto   scope   dev    tbl
        default      192.168.6.1                   dhcp          enp3s0
    172.17.0.0/ 16                   172.17.0.1   kernel  link  docker0
   192.168.6.0/ 24                192.168.6.119   kernel  link  enp3s0
      127.0.0.0        broadcast      127.0.0.1   kernel  link  lo       local
     127.0.0.0/ 8          local      127.0.0.1   kernel  host  lo       local
      127.0.0.1            local      127.0.0.1   kernel  host  lo       local
127.255.255.255        broadcast      127.0.0.1   kernel  link  lo       local
     172.17.0.0        broadcast     172.17.0.1   kernel  link  docker0  local
     172.17.0.1            local     172.17.0.1   kernel  host  docker0  local
 172.17.255.255        broadcast     172.17.0.1   kernel  link  docker0  local
    192.168.6.0        broadcast  192.168.6.119   kernel  link  enp3s0   local
  192.168.6.119            local  192.168.6.119   kernel  host  enp3s0   local
  192.168.6.255        broadcast  192.168.6.119   kernel  link  enp3s0   local
            ::1                                   kernel        lo
        fe80::/ 64                                kernel        enp3s0
            ::1            local                  kernel        lo       local
fe80::71e2:5264:8af7:b70a  local                  kernel        enp3s0   local
---------------------------------------------------------------------------
 该 rtentry{}通过 SIOCADDRT(增加 路由表条目) 和 SIOCDELRT(删除 添加路由表条目) 调用传递。
 struct rtentry {
	unsigned long int rt_pad1;
	struct sockaddr rt_dst;		目标/远程地址 destination address
	struct sockaddr rt_gateway;	网关地址 (RTF_GATEWAY)
	struct sockaddr rt_genmask;	目标网络掩码（IP)
	u_short rt_flags;			网口状态标志信息
	short  	rt_pad2;
	u_long 	rt_pad3;
	u_char 	rt_tos;
	u_char 	rt_class;
#if __WORDSIZE == 64
	short 	rt_pad4[3];
#else
	short   rt_pad4;
#endif
	short 	rt_metric;	接口测度 +1 代表 二进制兼容性！
	char 	*rt_dev;	强制添加设备。
	u_long 	rt_mtu;		路由器 interface MTU
	u_long 	rt_window;	Window 夹紧
	u_long 	rt_irtt;	初始 RTT
};
#define rt_mss	rt_mtu 	兼容性黑客
---------------------------------------------------------------------------
 struct in6_rtmsg {
	struct in6_addr rtmsg_dst;		目标/远程地址 destination address
	struct in6_addr rtmsg_src;		源地址 source address
	struct in6_addr rtmsg_gateway; 	网关地址 (RTF_GATEWAY)
	uint32_t 	rtmsg_type;
	uint16_t 	rtmsg_dst_len;		目标/远程地址(点到点) 长度
	uint16_t 	rtmsg_src_len;
	uint32_t 	rtmsg_metric;		接口测度 +1 代表 二进制兼容性！
	u_long 		rtmsg_info;
	uint32_t 	rtmsg_flags;		网口状态标志信息
	int 		rtmsg_ifindex;		获取网卡序号 接口索引 interface index
};                                                     */


/******************************** 程序05 *********************************/
/*目标/target 网关         子网掩码  		标志  NextHop    引用 使用  接口/dev  <MTU>
Destination  gateway      netmask	    flag  跃点  Met  Ref  Use  interface <MTU>
0.0.0.0      192.168.6.1  0.0.0.0        UG   100        0    0    enp3s0
172.17.0.0   0.0.0.0      255.255.0.0    U    0          0    0    docker0
192.168.6.0  0.0.0.0      255.255.255.0  U    100        0    0    enp3s0
192.168.6.0  0.0.0.0      255.255.255.0  U    100        0    0    enp3s0
::/0                                     !n   ::   -1    1    0    lo
::1/128                                  U    ::   256   1    0    lo
fe80::/64                                U    ::   100   1    0    enp3s0
::/0                                     !n   ::   -1    1    0    lo
::1/128                                  uN   ::   0     8    0    lo      */

/* 程序05：增加/删除 路由表条目 */
void test05_route()
{
	_debug_log_info("程序05：增加/删除 路由表条目")
	std::string strbuf;
	strbuf += "usage: IPv4 <-net> <gateway> <netmask> <dev> <MTU> <+/->\n";
	strbuf += "eg: IPv4 172.17.0.0 0.0.0.0  255.255.0.0 docker0 1500 +\n";
	strbuf += "eg: IPv4 172.17.0.0 0.0.0.0  255.255.0.0 docker0 1500 -\n";
	strbuf += "usage: IPv6 <-net> <metric> <dev> <+/->\n";
	strbuf += "eg: IPv6 ::/0 -1 lo +\n";
	strbuf += "eg: IPv6 ::/0 -1 lo -\n";
	strbuf += "Now input please...\n";
/* 	输入列表内容，如：111 222 333  zzz    gsfg
	获取每一个 std::vector<string> 子内容        */
	std::vector<std::string> vec_user = hints_strtovec(strbuf);
/***********************************************************/
	if (vec_user.size() < 5) err_quit(-1, __LINE__, "< 5");

	strbuf = vec_user[0];
	vec_user.erase(vec_user.cbegin(), vec_user.cbegin()+1);

	if(strbuf == "IPv4") {
		if(vec_user.size() >= 6) inet_setroute(vec_user);
		else err_quit(-1, __LINE__, "IPv4: < 6");
	}
	else if(strbuf == "IPv6") {
		if(vec_user.size() >= 4) inet6_setroute(vec_user);
		else err_quit(-1, __LINE__, "IPv6: < 4");
	}
	else err_quit(-1, __LINE__, "IPv4/IPv6");
}

/* IPv4 add/del route item in route table */
int inet_setroute(std::vector<std::string> &vec_str)
{
	_debug_log_info("inet_setroute()")

	struct rtentry route{}; // ipv4 route struct
	memset(&route, 0, sizeof(route));

	/* default target is net (host) */
	route.rt_flags = RTF_UP;

	/* 目标网域 */
//	route.rt_flags |= RTF_HOST;
	auto *addr = (struct sockaddr_in *) &route.rt_dst;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(vec_str[0].c_str());

	/* 网关 gateway */
	route.rt_flags |= RTF_GATEWAY;
	addr = (struct sockaddr_in *) &route.rt_gateway;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(vec_str[1].c_str());

	/* 子网掩码 netmask */
	addr = (struct sockaddr_in *) &route.rt_genmask;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(vec_str[2].c_str());

	/* device interface */
	route.rt_dev = vec_str[3].data();

	/* MTU */
	route.rt_flags |= RTF_MTU;
	route.rt_mtu = std::strtoul(vec_str[4].c_str(), nullptr, 10);
	_debug_log_info("route.rt_mtu: %lu", route.rt_mtu)

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	err_sys(sock_fd, __LINE__, "socket()");

	_debug_log_info("sock_fd: %d", sock_fd)

	if (vec_str[5] == "+") IOctl(sock_fd, SIOCADDRT, &route, __LINE__);
	else if (vec_str[5] == "-") IOctl(sock_fd, SIOCDELRT, &route);
	err_sys(close(sock_fd), __LINE__, "close()");
	return 0;
}

/* IPv6 add/del route item in route table */
int inet6_setroute(std::vector<std::string> &vec_str)
{
	_debug_log_info("inet6_setroute()")

	struct in6_rtmsg route{}; // ipv6 route struct
	memset(&route, 0, sizeof(route));

	/* default target is net (host) */
	route.rtmsg_flags = RTF_UP;
	route.rtmsg_dst_len = 128;  // prefixlen 64 128

	/* 目标网域 */
	route.rtmsg_flags |= RTF_HOST;
	inet_pton(AF_INET6, vec_str[0].c_str(), &route.rtmsg_dst);// 转为网络字节序

	/* metric */
	route.rtmsg_metric = strtoumax(vec_str[1].c_str(),
								   nullptr, 10);

	/* device interface */
	std::string devname = vec_str[2];
//	if (vec_str[i] == "device" || vec_str[i] == "dev")
//		devname = vec_str[i];

//	/* 网关 gateway */
//	route.rtmsg_flags |= RTF_GATEWAY;
//	inet_pton(AF_INET6, vec_str[i].c_str(), &route.rtmsg_gateway);// 转为网络字节序

//	if (vec_str[i] == "mod") route.rtmsg_flags |= RTF_MODIFIED;
//	if (vec_str[i] == "dyn") route.rtmsg_flags |= RTF_DYNAMIC;

	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	int sock_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP);
	err_sys(sock_fd, __LINE__, "socket()");

	struct ifreq ifreq_s{}; // 定义网口请求结构体
	memset(&ifreq_s, 0, sizeof(ifreq_s));

	if (!devname.empty()) {
		strncpy(ifreq_s.ifr_name, devname.c_str(), IFNAMSIZ); // 网口名
//		memcpy(ifreq_s.ifr_name, devname.c_str(), IFNAMSIZ); // 网口名
		IOctl(sock_fd, SIOCGIFINDEX/*获取网卡序号*/, &ifreq_s, __LINE__);
		route.rtmsg_ifindex = ifreq_s.ifr_ifindex;
	}

	/* MTU */
//	uint16_t mtu = strtoumax(vec_str[i].c_str(), nullptr, 10);
//	if (mtu) {
//		ifreq_s.ifr_mtu = mtu;
//		IOctl(sock_fd, SIOCSIFMTU/*设置接口MTU*/, &ifreq_s, __LINE__);
//	}

	_debug_log_info("vec_str[3]: %s", vec_str[3].c_str())

	if (vec_str[3] == "+") IOctl(sock_fd, SIOCADDRT, &route, __LINE__);
	else if (vec_str[3] == "-") IOctl(sock_fd, SIOCDELRT, &route, __LINE__);
	err_sys(close(sock_fd), __LINE__, "close()");

	_debug_log_info("return")
	return 0;
}
