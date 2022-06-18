
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

/* 对底层内核的控制 */
int IOctl(const int &fd, const ulong &request, const void *arg,
		  const int &line, const char *func)
{
	int retval = ioctl(fd, request, arg);
	if (retval < 0) err_sys(close(fd), __LINE__, "close()");
	err_sys(retval, __FILE__, __func__, __LINE__,
			"ioctl()\t调用地方: %s(): %d", func, line);
	return retval;
}

/*************************************************************************/

#define test_select                5

void ioctl_test()
{
#if test_select==1
	test01_GetNetCardIP();
#elif test_select==2
	test02_ifconfig();
#elif test_select==3
	test03_ifconfig();
#elif test_select==4
	test04_ARP_mac();
#elif test_select==5
	test05_route();
#endif
}

/******************************** 程序01 *********************************/
/* 程序01：获取 网口名的 IP地址 */
void test01_GetNetCardIP()
{
	_debug_log_info("程序01：获取 网口名的 IP地址")
	std::string hints_str;
	hints_str += "usage: <network card> ...\n";
	hints_str += "usage: enp3s0 docker0 lo\n";
	hints_str += "Now input please...\n";
	hints_str += "usage: ";
/* 	输入列表内容，如：111 222 333  zzz    gsfg
	获取每一个 std::vector<string> 子内容        */
	std::vector<std::string> vec_user = hints_strtovec(hints_str);
	if (vec_user.empty())
		err_quit(-1, __LINE__, "usage: <network card> ...");
/***********************************************************/
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	err_sys(sock_fd, __LINE__, "socket()");

	struct ifreq ifreq_ptr{}; // 定义网口请求结构体
	for (const auto &i : vec_user) {
		strcpy(ifreq_ptr.ifr_name, i.c_str());
		if (IOctl(sock_fd, SIOCGIFADDR/*获取接口地址*/, &ifreq_ptr) < 0)
			perror("ioctl()");
		std::cout << "网口名: " << i.c_str() << "\t\t网口地址: "
				  << Addr_net_to_ptr((struct sockaddr *) &ifreq_ptr.ifr_addr) << std::endl;
	}
}

/******************************** 程序02 *********************************/
#if 0
void interface_status_printf(const short int &flags) // 打印网口状态标志信息
{
	if (flags & IFF_UP) {
		std::cout << "UP ";
	}
	if (flags & IFF_BROADCAST) {
		std::cout << "BROADCAST ";
	}
	if (flags & IFF_LOOPBACK) {
		std::cout << "LOOPBACK ";
	}
	if (flags & IFF_POINTOPOINT) {
		std::cout << "POINTOPOINT ";
	}
	if (flags & IFF_RUNNING) {
		std::cout << "RUNNING ";
	}
	if (flags & IFF_PROMISC) {
		std::cout << "PROMISC ";
	}
	if (flags & IFF_DEBUG) {
		std::cout << "DEBUG ";
	}
	if (flags & IFF_NOTRAILERS) {
		std::cout << "NOTRAILERS ";
	}
	if (flags & IFF_NOARP) {
		std::cout << "NOARP ";
	}
	if (flags & IFF_ALLMULTI) {
		std::cout << "ALLMULTI ";
	}
	if (flags & IFF_MASTER) {
		std::cout << "MASTER ";
	}
	if (flags & IFF_SLAVE) {
		std::cout << "SLAVE ";
	}
	if (flags & IFF_MULTICAST) {
		std::cout << "MULTICAST ";
	}
	if (flags & IFF_DYNAMIC) {
		std::cout << "DYNAMIC ";
	}
	if (flags & IFF_AUTOMEDIA) {
		std::cout << "AUTOMEDIA ";
	}
	if (flags & IFF_PORTSEL) {
		std::cout << "PORTSEL ";
	}
}
#endif

void interface_status_printf(const int &flags) // 打印网口状态标志信息
{
	std::array<std::string, 16> arr_str = {
			"UP",			"BROADCAST",
			"LOOPBACK",		"POINTOPOINT",
			"RUNNING",		"PROMISC",
			"DEBUG",		"NOTRAILERS",
			"NOARP",		"ALLMULTI",
			"MASTER",		"SLAVE",
			"MULTICAST",	"DYNAMIC",
			"AUTOMEDIA",	"PORTSEL"
	};
	std::array<int, 16> arr_val = {
			IFF_UP, 		IFF_BROADCAST,
			IFF_LOOPBACK, 	IFF_POINTOPOINT,
			IFF_RUNNING, 	IFF_PROMISC,
			IFF_DEBUG, 		IFF_NOTRAILERS,
			IFF_NOARP, 		IFF_ALLMULTI,
			IFF_MASTER, 	IFF_SLAVE,
			IFF_MULTICAST, IFF_DYNAMIC,
			IFF_AUTOMEDIA, IFF_PORTSEL
	};
	int arr_size = arr_str.size();
	for (int i=0, j=0; i < arr_size; ++i) {
		if (flags & arr_val[i]) {
			if (j > 0) std::cout << " ";
			++j;
			std::cout << arr_str[i];
		}
	}
}

/* 程序02：简单 shell ifconfig 查询功能 */
void test02_ifconfig()
{
	_debug_log_info("程序02：简单 shell ifconfig 查询功能")
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	err_sys(sock_fd, __LINE__, "socket()");

#if 0
	struct ifreq ifreq_buf[MAX_INTERFACE]; // 定义网口信息结构体
	struct ifconf ifconfig{}; // 接口清单
	ifconfig.ifc_len = sizeof(ifreq_buf); // size of buffer
//	ifconfig.ifc_len = MAX_INTERFACE * sizeof(struct ifreq); // size of buffer
	ifconfig.ifc_buf = (caddr_t) ifreq_buf; // buffer address

	IOctl(sock_fd, SIOCGIFCONF/*获取接口清单(含网口名)*/, &ifconfig);

	/* 网口总数 = ifconfig.ifc_len / sizeof(struct ifreq) */
	int interface_num = ifconfig.ifc_len / (int)( sizeof(struct ifreq) );
	std::cout << "网口总数: " << interface_num << std::endl;
#else
	int interface_num = interface_number(sock_fd); // 网口总数
	std::cout << "网口总数: " << interface_num << std::endl;

	struct ifreq ifreq_buf[interface_num]; // 定义网口信息结构体
	struct ifconf ifconfig{}; // 接口清单
	ifconfig.ifc_len = interface_num * (int)sizeof(struct ifreq); // size of buffer
	ifconfig.ifc_buf = (caddr_t) ifreq_buf; // buffer address

	IOctl(sock_fd, SIOCGIFCONF/*获取接口清单(含网口名)*/, &ifconfig);
#endif

	for (int i=0; i < interface_num; ++i)
	{
		std::cout << "--------------------" << std::endl;
		std::cout << "网口名: " << ifreq_buf[i].ifr_name << std::endl;

		IOctl(sock_fd, SIOCGIFFLAGS/*获取接口状态标志*/, &ifreq_buf[i]);
		std::cout << "网口状态标志信息: ";
		interface_status_printf(ifreq_buf[i].ifr_flags); // 打印网口状态标志信息
		std::cout << std::endl;

		IOctl(sock_fd, SIOCGIFMTU/*获取接口MTU*/, &ifreq_buf[i]);
		std::cout << "MTU: " << std::dec << ifreq_buf[i].ifr_mtu << std::endl;

		IOctl(sock_fd, SIOCGIFADDR/*获取网卡IP地址*/, &ifreq_buf[i]);
		std::cout << "网卡IP地址: "
				  << Addr_net_to_ptr(
						  (struct sockaddr *) &ifreq_buf[i].ifr_addr)
				  << std::endl;

		IOctl(sock_fd, SIOCGIFNETMASK/*获取子网掩码(network mask)*/,
			  &ifreq_buf[i]);
		std::cout << "子网掩码(network mask): "
				  << Addr_net_to_ptr(
						  (struct sockaddr *) &ifreq_buf[i].ifr_netmask)
				  << std::endl;

		IOctl(sock_fd, SIOCGIFBRDADDR/*获取广播地址*/, &ifreq_buf[i]);
		std::cout << "广播地址: "
				  << Addr_net_to_ptr(
						  (struct sockaddr *) &ifreq_buf[i].ifr_broadaddr)
				  << std::endl;

		IOctl(sock_fd, SIOCGIFHWADDR/*获取硬件地址(MAC机器码)*/, &ifreq_buf[i]);
#if 1
		std::cout << "以太网/硬件地址(MAC机器码): " << std::hex;
		for (int j = 0; j < 6; ++j) {
			std::cout << std::setw(2) << std::setfill('0')
					  << (uint)ifreq_buf[i].ifr_hwaddr.sa_data[j];
			if (j < 5) std::cout << ":";
			else std::cout << std::endl;
		}
#else
		printf("硬件地址(MAC机器码): "
			   "%02x:%02x:%02x:%02x:%02x:%02x\n\n",
			   (unsigned char)ifreq_buf[i].ifr_hwaddr.sa_data[0],
			   (unsigned char)ifreq_buf[i].ifr_hwaddr.sa_data[1],
			   (unsigned char)ifreq_buf[i].ifr_hwaddr.sa_data[2],
			   (unsigned char)ifreq_buf[i].ifr_hwaddr.sa_data[3],
			   (unsigned char)ifreq_buf[i].ifr_hwaddr.sa_data[4],
			   (unsigned char)ifreq_buf[i].ifr_hwaddr.sa_data[5]);
#endif
		IOctl(sock_fd, SIOCGIFMETRIC/*接口测度*/, &ifreq_buf[i]);
		std::cout << "接口测度 metric: " << ifreq_buf[i].ifr_metric << std::endl;

		IOctl(sock_fd, SIOCGIFDSTADDR/*远程地址(点到点)*/, &ifreq_buf[i]);
		std::cout << "远程地址(点到点): "
				  << Addr_net_to_ptr(
						  (struct sockaddr *) &ifreq_buf[i].ifr_dstaddr)
				  << std::endl;
	}
}

/******************************** 程序03 *********************************/
/* 程序03：进阶 shell ifconfig 查询功能 */
void test03_ifconfig()
{
#if 0
	_debug_log_info("程序03：进阶 shell ifconfig 查询功能")
	std::string hints_str;
	hints_str += "usage: <ipv4|ipv6>\n";
	hints_str += "usage:  ipv4 \n";
	hints_str += "Now input please...\n";
	hints_str += "usage: ";
/* 	输入列表内容，如：111 222 333  zzz    gsfg
	获取每一个 std::vector<string> 子内容        */
	std::vector<std::string> vec_user = hints_strtovec(hints_str);
	if (vec_user.size() != 1)
		err_quit(-1, __LINE__, "usage: <ipv4|ipv6>");
/***********************************************************/
	int family = 0;
	if (vec_user[0] == "ipv4") 		family = AF_INET;
	else if (vec_user[0] == "ipv6")	family = AF_INET6;
	else err_quit(-1, __LINE__,
				  "invalid <address-family> : %s", vec_user[0].c_str());
#endif
/***********************************************************/
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	err_sys(sock_fd, __LINE__, "socket()");
/***********************************************************/
	struct sockaddr	*sa;
	std::unique_ptr<struct if_info []> if_info_ary = get_if_info(sock_fd, AF_INET);
/* enp3s0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.6.119  netmask 255.255.255.0  broadcast 192.168.6.255
        inet6 fe80::71e2:5264:8af7:b70a  prefixlen 64  scopeid 0x20<link>
        ether 1c:69:7a:39:44:24  txqueuelen 1000  (以太网)
        RX packets 165250  bytes 89541110 (89.5 MB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 66882  bytes 12786588 (12.7 MB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0  */
	for (struct if_info * if_i = if_info_ary.get() ;
		 if_i != nullptr;
		 if_i = if_i->if_next ) {
		using namespace std;
		cout << "---------------------------------------" << endl;

		if (if_i->if_index != 0) // 获取网卡序号
			cout << "index(网卡序号): " << if_i->if_index << " \n";

		cout << if_i->if_name << ": \t"; // 网口名

		cout << "<";
		interface_status_printf(if_i->if_flags); // 打印网口状态标志信息
		cout << "> \t";

		cout << "MTU ";
		if (if_i->if_mtu != 0) cout << std::dec << if_i->if_mtu << "\n";

		if ((sa = &if_i->if_addr) != nullptr)    // 网卡IP地址
			cout << "inet(网卡IP地址): \t" << Addr_net_to_ptr(sa) << "\n";

		if ((sa = &if_i->if_netmask) != nullptr) // 子网掩码(network mask)
			cout << "netmask(子网掩码):\t" << Addr_net_to_ptr(sa) << "\n";

		if ((sa = &if_i->if_brdaddr) != nullptr && sa->sa_data[2]) // 广播地址
//			for (const char &i : sa->sa_data)
//				if (i) {
			cout << "broadcast(广播地址):\t" << Addr_net_to_ptr(sa) << "\n";
//					break;
//				}

		if ((sa = &if_i->if_dstaddr) != nullptr) // 远程地址(点到点)
			for (const char &i : sa->sa_data)
				if (i) {
					cout << "destination \t" << Addr_net_to_ptr(sa)
						 << "\n";
					break;
				}

//		if ((sa = &if_i->if_hwaddr) != nullptr && sa->sa_data[0] != '\0')
		if ((sa = &if_i->if_hwaddr) != nullptr &&
			sa->sa_data[0])//以太网/硬件地址(MAC机器码)
		{
			std::cout << "ether(以太网/硬件地址/MAC机器码): " << std::hex;
			for (int j = 0; j < 6; ++j) {
				std::cout << std::setw(2) << std::setfill('0')
						  << ( (uint8_t)sa->sa_data[j] & 0xff );
				if (j < 5) std::cout << ":";
				else std::cout << endl;
			}
		}

		std::cout << std::dec;

		if (if_i->if_metric != 0) // 接口测度
			std::cout << "metric(接口测度): " << if_i->if_metric << " \t";

		if (if_i->if_bandwidth != 0)
			std::cout << "bandwidth: " << if_i->if_bandwidth << " \t";

		if (if_i->if_quelen != 0) // 发送队列长度
			std::cout << "quelen(发送队列长度): " << if_i->if_quelen << " \t";

		if (if_i->ifru_slave[0]) // 占位符
			std::cout << "slave: " << if_i->ifru_slave << " \n";

		if (if_i->if_alias_flag == IFI_alias)
			std::cout << "alias: " << if_i->if_alias << " \n";

		if (if_i->ifru_newname[0])
			std::cout << "newname: " << if_i->ifru_newname << " \n";

		if (if_i->if_map.port || if_i->if_map.mem_start || if_i->if_map.mem_end ||
			if_i->if_map.base_addr || if_i->if_map.irq || if_i->if_map.dma) {
			/* 网卡设备映射属性 */
			std::cout << "mem_start(开始地址): "
					  << if_i->if_map.mem_start << " \t";
			std::cout << "mem_end(结束地址): "
					  << if_i->if_map.mem_end << " \n";
			std::cout << "base_addr(基地址): "
					  << if_i->if_map.base_addr << "\n";
			std::cout << "irq(中断号): "
					  << if_i->if_map.irq << " \t";
			std::cout << "DMA: "
					  << if_i->if_map.dma << " \t";
			std::cout << "port: "
					  << if_i->if_map.port;
		}

		cout << endl;
	}
	if_info_ary.reset(); // Auto free resource
}

/* 返回网口总数 */
int interface_number(const int &sock_fd)
{
	const uint16_t interface_num = 3; 						// 网口总数
	_debug_log_info("interface_number()")

	auto ifreq_unique = std::make_unique<struct ifreq[]>(interface_num);
	struct ifconf ifconfig{}; // 接口清单
	for (int bufsize=0, len=interface_num; ; ) {
		ifconfig.ifc_len = len * (int)sizeof(struct ifreq); // 缓冲区长度
		ifconfig.ifc_buf = (caddr_t)ifreq_unique.get(); 	// 缓冲区地址
		if (ioctl(sock_fd, SIOCGIFCONF/*获取接口清单(含网口名)*/, &ifconfig) < 0) {
			if (errno != EINVAL/*Invalid argument*/ || bufsize != 0)
				err_sys(-1, __LINE__, "ioctl()");
		} else {
			/* 当 bufsize 没有改变时，则获取成功；若改变，则缓冲区长度不够 */
			if (ifconfig.ifc_len == bufsize) break;
			bufsize = ifconfig.ifc_len;
		}
		len += interface_num; 	// 缓冲区长度，若大小不够，自动增加
		ifreq_unique.reset();	// Auto free resource
		/*  Auto free resource and set to a new */
//		ifreq_unique.reset(new (struct ifreq[len]));
		ifreq_unique = std::make_unique<struct ifreq[]>(len);
	}
	ifreq_unique.reset();	// Auto free resource

	/* 网口总数 = ifconfig.ifc_len / sizeof(struct ifreq) */
	return ( ifconfig.ifc_len / (int)(sizeof(struct ifreq)) );
//	std::cout << "网口总数: " << interface_num << std::endl;
}

std::unique_ptr<struct if_info []> Get_if_info(const int &family)
{
	_debug_log_info("Get_if_info()")
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	err_sys(sock_fd, __LINE__, "socket()");
	std::unique_ptr<struct if_info []> if_info_ary = get_if_info(sock_fd, family);
	close(sock_fd);
	return if_info_ary;
}

std::unique_ptr<struct if_info []> get_if_info(const int &sock_fd,
											   const int &family)
{
	_debug_log_info("get_if_info()")
	const int interface_num = interface_number(sock_fd); // 网口总数
//	std::cout << "网口总数: " << interface_num << std::endl;

	struct ifreq ifreq_buf[interface_num]; // 定义网口信息结构体
	struct ifconf ifconfig{}; // 接口清单
	ifconfig.ifc_len = interface_num * (int)sizeof(struct ifreq); // size of buffer
	ifconfig.ifc_buf = (caddr_t) ifreq_buf; // buffer address
	IOctl(sock_fd, SIOCGIFCONF/*获取接口清单(含网口名)*/, &ifconfig, __LINE__);

/**---------------------------------------------**/
	auto if_info_ary = std::make_unique<struct if_info []>(interface_num);
	struct if_info *if_info_ptr;
	struct ifreq *ifreq_ptr;
	for (int i=0, flags; i < interface_num; ++i)
	{
		if (i > 0) if_info_ary[i-1].if_next = &if_info_ary[i];
		if_info_ptr = (struct if_info *)&if_info_ary[i];
		ifreq_ptr = (struct ifreq *)&ifreq_buf[i];

/**---------------------------------------------**/
		memcpy(if_info_ptr->if_name, ifreq_ptr->ifr_name, IFNAMSIZ); // 网口名

		IOctl(sock_fd, SIOCGIFFLAGS/*获取接口状态标志*/, ifreq_ptr, __LINE__);
		flags = ifreq_ptr->ifr_flags;
		if ((flags & IFF_UP) == 0) continue; 	// ignore if interface not UP
		if_info_ptr->if_flags = (short)flags;

		IOctl(sock_fd, SIOCGIFMTU/*获取接口MTU*/, ifreq_ptr, __LINE__);
		if_info_ptr->if_mtu = ifreq_ptr->ifr_mtu;

		if (family == AF_INET) {
			IOctl(sock_fd, SIOCGIFADDR/*获取网卡IP地址*/, ifreq_ptr, __LINE__);
			/* ignore if not desired address-family */
			if (ifreq_ptr->ifr_addr.sa_family != family) break;
			memcpy(&if_info_ptr->if_addr, &ifreq_ptr->ifr_addr,
				   sizeof(struct sockaddr));

			IOctl(sock_fd, SIOCGIFNETMASK/*获取子网掩码(network mask)*/,
				  ifreq_ptr, __LINE__);
			memcpy(&if_info_ptr->if_netmask, &ifreq_ptr->ifr_netmask,
				   sizeof(struct sockaddr));

			if (flags & IFF_BROADCAST) {
				IOctl(sock_fd, SIOCGIFBRDADDR/*获取广播地址*/,
					  ifreq_ptr, __LINE__);
				memcpy(&if_info_ptr->if_brdaddr, &ifreq_ptr->ifr_broadaddr,
					   sizeof(struct sockaddr));
			}
		}

		if (flags & IFF_POINTOPOINT) {
			IOctl(sock_fd, SIOCGIFDSTADDR/*远程地址(点到点)*/,
				  ifreq_ptr, __LINE__);
			memcpy(&if_info_ptr->if_dstaddr, &ifreq_ptr->ifr_dstaddr,
				   sizeof(struct sockaddr));
		}

		IOctl(sock_fd, SIOCGIFHWADDR/*获取硬件地址(MAC机器码)*/, ifreq_ptr, __LINE__);
//		if_info_ptr->if_hwaddr = ifreq_ptr->ifr_hwaddr;
		memcpy(&if_info_ptr->if_hwaddr, &ifreq_ptr->ifr_hwaddr, sizeof(struct sockaddr));

		IOctl(sock_fd, SIOCGIFMETRIC/*接口测度*/, ifreq_ptr, __LINE__);
		if_info_ptr->if_metric = ifreq_ptr->ifr_metric;

		IOctl(sock_fd, SIOCGIFMAP/*获取网卡映射参数*/, ifreq_ptr, __LINE__);
//		if_info_ptr->if_map = ifreq_ptr->ifr_map;
		memcpy(&if_info_ptr->if_map, &ifreq_ptr->ifr_map, sizeof(struct ifmap));

		IOctl(sock_fd, SIOCGIFINDEX/*获取网卡序号*/, ifreq_ptr, __LINE__);
		if_info_ptr->if_index = ifreq_ptr->ifr_ifindex;

		IOctl(sock_fd, SIOCGIFTXQLEN/*获取发送队列长度*/, ifreq_ptr, __LINE__);
		if_info_ptr->if_quelen = ifreq_ptr->ifr_qlen;
	}

	return if_info_ary;
}

/******************************** 程序04 *********************************/
/* 程序04：使用 ARP高速缓存 获取以太网/主机硬件地址(MAC机器码) */
void test04_ARP_mac()
{
	_debug_log_info("test04_ARP_mac()")
	/* ---- socket () ---- SOCK_DGRAM 固定最大长度的无连接，不可靠的数据报 */
	int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	err_sys(sock_fd, __LINE__, "socket()");
/***********************************************************/
	std::unique_ptr<struct if_info []> if_info_ary = get_if_info(sock_fd, AF_INET);
	struct sockaddr	*sa;
	struct arpreq arpreq_s{};
	for (struct if_info * if_i = if_info_ary.get() ;
		 if_i != nullptr;
		 if_i = if_i->if_next )
	{
		using namespace std;

		if ((sa = &if_i->if_addr) != nullptr) 	// 网卡IP地址
			cout << "inet \t" << Addr_net_to_ptr(sa) << " \t";
		else continue;

		/* struct arpreq
  {
    	struct sockaddr arp_pa;		 Protocol address.
		struct sockaddr arp_ha;		 Hardware address.
		int arp_flags;			 Flags.
		struct sockaddr arp_netmask;	 Netmask (only for proxy arps).
		char arp_dev[16];
	};  */
		memcpy(&arpreq_s.arp_pa, sa, sizeof(struct sockaddr));

		if (ioctl(sock_fd, SIOCGARP/*获取ARP表项*/, &arpreq_s) < 0) {
			cout << endl;
			err_ret(-1, __LINE__, "ioctl():SIOCGARP");
			continue;
		}

		//		if ((sa = &if_i->if_hwaddr) != nullptr && sa->sa_data[0] != '\0')
		if ((sa = &arpreq_s.arp_ha) != nullptr && sa->sa_data[0])//以太网/硬件地址(MAC机器码)
		{
			std::cout << "ether: \t\t" << std::hex;
			for (int j = 0; j < 6; ++j) {
				std::cout << std::setw(2) << std::setfill('0')
						  << (uint)sa->sa_data[j];
				if (j < 5) std::cout << ":";
				else std::cout << "\n";
			}
		}
	}
}

