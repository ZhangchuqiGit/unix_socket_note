
/** 执行时间
   最快	非阻塞 I/O 版本 select() or poll() or epoll() or kqueue() and so on.
	|	线程 版本
	|	进程 版本
   最慢	阻塞 I/O 版本 select() or poll() or epoll() or kqueue() and so on.
   超慢	停-等 版本                    **/

#include <iostream>
#include "zcq_header.h"

#include <sys/cdefs.h>

#if 1 /* main() */

/*************************/
void test_example();

/*************************/
#define SELECT_display                 0

/***************************************************/
/*	bash shell 	------------------------
	$ g++ test.cpp -o test
  	$ ./test myfile.txt youfile.txt one.sh
	c++ 		------------------------
		int main(int argc, char **argv)
----------------------------------------
  	c++    		bash shell
  	argc	=	${#argv[@]} # 参数个数
  	argv[0]	=	myfile.txt 	# 参数 : ./当前文件名
  	argv[1]	=	myfile.txt 	# 参数 1
  	argv[2]	=	youfile.txt # 参数 2
  	argv[3]	=	one.sh 	# 参数 3	  	*/
int main(int argc, char **argv)
{
	using namespace std;

#if SELECT_display == 1
	Readv_test();
#elif SELECT_display == 2
	test_example();
#elif SELECT_display == 3
	host_display();
#elif SELECT_display == 4
	serv_display();
#elif SELECT_display == 5
	host_display_r();
#elif SELECT_display == 6
	serv_display_r();
#elif SELECT_display == 7
	tcp_service_daemon_inetd();
#elif SELECT_display == 8
	ioctl_test();
#elif SELECT_display == 9
	struct addrinfo *result = Get_addrinfo( "::1",
											nullptr, AF_INET6,											SOCK_DGRAM, IPPROTO_IP,
											AI_CANONNAME);
	freeaddrinfo(result); // 释放结构体链表指向的任何动态存储空间
#else
	int num = -1;
	while (true)
	{
		cout << "============================================= tcp_service" << endl
			 << "tcp_service_fork.cpp                      0" << endl
			 << "tcp_service_fork_Signal.cpp               1" << endl
			 << "tcp_service_IO_select.cpp                 2" << endl
			 << "tcp_service_IO_poll.cpp                   3" << endl
			 << "tcp_service_IO_poll_addrinfo.cpp          4" << endl
			 << "tcp_service_IO_epoll_addrinfo.cpp         5" << endl
			 << "tcp_service_daemon_inetd.cpp              6" << endl
			 << "tcp_service_IO_nonblock_select.cpp        7" << endl
			 << "tcp_service_OOB_Signal_Setsockopt.cpp     8" << endl
			 << "tcp_service_OOB_nonblock_select.cpp       9" << endl
			 << "tcp_service_OOB_heartbeat_fork_Signal.cpp 100" << endl
			 << "pthread_tcp_service_addrinfo.cpp          101" << endl
			 << "--------------------------------------------- tcp_client" << endl
			 << "tcp_client_one.cpp                        10" << endl
			 << "tcp_client_for.cpp                        11" << endl
			 << "tcp_client_one_byNamePort.cpp             12" << endl
			 << "tcp_client_one_addrinfo.cpp               13" << endl
			 << "tcp_client_nonblock_select_web.cpp        14" << endl
			 << "tcp_client_pthread_web.cpp                114" << endl
			 << "tcp_client_OOB.cpp                        15" << endl
			 << "tcp_client_OOB_heartbeat_pselect.cpp      110" << endl
			 << "pthread_tcp_client_addrinfo.cpp           111" << endl
			 << "============================================ udp_service" << endl
			 << "udp_service.cpp                           20" << endl
			 << "udp_service_addrinfo.cpp                  21" << endl
			 << "udp_service_addrinfo_daemon_syslog.cpp    22" << endl
			 << "udp_service_bind_all_IPv4.cpp             23" << endl
			 << "-------------------------------------------- udp_client" << endl
			 << "udp_client_one.cpp                        30" << endl
			 << "udp_client_one_config.cpp                 31" << endl
			 << "udp_client_one_addrinfo.cpp               32" << endl
			 << "-------------------------------------------- udp_multicast" << endl
			 << "udp_multicast.cpp                         40" << endl
			 << "udp_servicr_multicast_SAPSDP.cpp          41" << endl
			 << "udp_client_multicast_SNTP.cpp             42" << endl
			 << "-------------------------------------------- udp_broadcast" << endl
			 << "udp_client_broadcast.cpp                  50" << endl
			 << "============================================ udp_tcp_service" << endl
			 << "udp_tcp_service_fork_Signal_IO_select.cpp 55" << endl
			 << "============================================ sctp_service" << endl
			 << "sctp_service.cpp                          60" << endl
			 << "sctp_service_AutoCloseClient.cpp          61" << endl
			 << "sctp_service_event.cpp                    62" << endl
			 << "sctp_service_fork.cpp                     63" << endl
			 << "-------------------------------------------- sctp_client" << endl
			 << "sctp_client.cpp                           65" << endl
			 << "sctp_client_event.cpp                     66" << endl
			 << "============================================ unix local" << endl
			 << "-------------------------------------------- unix_STREAM_service" << endl
			 << "unix_STREAM_service_fork.cpp              70" << endl
			 << "unix_STREAM_service_IO_epoll_addrinfo.cpp 71" << endl
			 << "-------------------------------------------- unix_STREAM_client" << endl
			 << "unix_STREAM_client_one.cpp                75" << endl
			 << "-------------------------------------------- unix_DGRAM_service" << endl
			 << "unix_DGRAM_service.cpp                    80" << endl
			 << "-------------------------------------------- unix_DGRAM_client" << endl
			 << "unix_DGRAM_client.cpp                     85" << endl
			 << "-------------------------------------------- socketpair" << endl
			 << "unix_socketpair_same.cpp                  90" << endl
			 << "unix_socketpair_diff.cpp                  91" << endl
			 << "unix_socketpair_system_fork_exec_fds.cpp  92" << endl
			 << "---------------------- please input : ";
		if (!cout.good())
		{
			cout.clear();  // 清除错误标志
		}
		cin >> std::dec >> num;
		if (!cin.good())
		{
			cin.clear();  // 清除错误标志
			cin.ignore(); //当输入缓冲区没有数据时，也会阻塞等待数据的到来
			num = -1;
		}
//		cin.get(); // \n
		switch ( num ) {
			case 0:
				tcp_service_fork();
				exit(0);
			case 1:
				tcp_service_fork_Signal();
				exit(0);
			case 2:
				tcp_service_IO_select();
				exit(0);
			case 3:
				tcp_service_IO_poll();
				exit(0);
			case 4:
				tcp_service_IO_poll_addrinfo();
				exit(0);
			case 5:
				tcp_service_IO_epoll_addrinfo();
				exit(0);
			case 6:
				tcp_service_daemon_inetd();
				exit(0);
			case 7:
				tcp_service_IO_nonblock_select();
				exit(0);
			case 8:
				tcp_service_OOB_Signal_Setsockopt();
				exit(0);
			case 9:
				tcp_service_OOB_nonblock_select();
				exit(0);
			case 100:
				tcp_service_OOB_heartbeat_fork_Signal();
				exit(0);
			case 101:
				pthread_tcp_service_addrinfo();
				exit(0);
			case 10:
				tcp_client_one();
				exit(0);
			case 11:
				tcp_client_for();
				exit(0);
			case 12:
				tcp_client_one_byNamePort();
				exit(0);
			case 13:
				tcp_client_one_addrinfo();
				exit(0);
			case 14:
				tcp_client_nonblock_select_web();
				exit(0);
			case 114:
				tcp_client_pthread_web();
				exit(0);
			case 15:
				tcp_client_OOB();
				exit(0);
			case 110:
				tcp_client_OOB_heartbeat_pselect();
				exit(0);
			case 111:
				pthread_tcp_client_addrinfo();
				exit(0);
			case 20:
				udp_service();
				exit(0);
			case 21:
				udp_service_addrinfo();
				exit(0);
			case 22:
				udp_service_addrinfo_daemon_syslog();
				exit(0);
			case 23:
				udp_service_bind_all_IPv4();
				exit(0);
			case 30:
				udp_client_one();
				exit(0);
			case 31:
				udp_client_one_config();
				exit(0);
			case 32:
				udp_client_one_addrinfo();
				exit(0);
			case 40:
				udp_multicast();
				exit(0);
			case 41:
				udp_servicr_multicast_SAPSDP();
				exit(0);
			case 42:
				udp_client_multicast_SNTP();
				exit(0);
			case 50:
				udp_client_broadcast();
				exit(0);
			case 55:
				udp_tcp_service_fork_Signal_IO_select();
				exit(0);
			case 60:
				sctp_service();
				exit(0);
			case 61:
				sctp_service_AutoCloseClient();
				exit(0);
			case 62:
				sctp_service_event();
				exit(0);
			case 63:
				sctp_service_fork();
				exit(0);
			case 65:
				sctp_client();
				exit(0);
			case 66:
				sctp_client_event();
				exit(0);
			case 70:
				unix_STREAM_service_fork();
				exit(0);
			case 71:
				unix_STREAM_service_IO_epoll_addrinfo();
				exit(0);
			case 75:
				unix_STREAM_client_one();
				exit(0);
			case 80:
				unix_DGRAM_service();
				exit(0);
			case 85:
				unix_DGRAM_client();
				exit(0);
			case 90:
				unix_socketpair_same();/* 读写操作 位于 同一进程 */
				exit(0);
			case 91:
				unix_socketpair_diff();/* 读写操作 位于 不同进程（父子进程）*/
				exit(0);
			case 92:
				unix_socketpair_system_fork_exec_fds();/* 读写操作 位于 不同进程（父子进程）*/
				exit(0);
			default:
				num = -1;
				break;
		}
		cout << endl << endl;
	}
#endif
}

/*************************************************************************/
#if SELECT_display == 2

#include <memory>

#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <functional>

struct B {
	virtual void bar() { std::cout << "B::bar\n"; }
	virtual ~B() = default;
};
struct D : B
{
	D() { std::cout << "D::D\n";  }
	~D() override { std::cout << "D::~D\n";  }
	void bar() override { std::cout << "D::bar\n";  }
};

// 消费 unique_ptr 的函数能以值或以右值引用接收它
std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
	p->bar();
	return p;
}

void close_file(std::FILE* fp) { std::fclose(fp); }

int* malloc_int(int value)
{
	int* p = (int*)malloc(sizeof(int));
	*p = value;
	return p;
}

void test_example()
{
	using namespace std;

	unique_ptr<int, decltype(free)*> myIntSmartPtr(malloc_int(42), free);
//	int * mmm = myIntSmartPtr.get();

	std::cout << "unique ownership semantics demo\n";
	{
		auto p = std::make_unique<D>(); // p 是占有 D 的 unique_ptr
		auto q = pass_through(std::move(p));
		q = std::move(p);
		assert(!p); // 现在 p 不占有任何内容并保有空指针
		q->bar();   // 而 q 占有 D 对象
	} // ~D 调用于此

	std::cout << "Runtime polymorphism demo\n";
	{
		std::unique_ptr<B> p = std::make_unique<D>(); // p 是占有 D 的 unique_ptr
		// 作为指向基类的指针
		p->bar(); // 虚派发

		std::vector<std::unique_ptr<B>> v;  // unique_ptr 能存储于容器
		v.push_back(std::make_unique<D>());
		v.push_back(std::move(p)); // 将元素添加到容器末尾
		v.emplace_back(new D); // 添加新元素到容器尾
		for(auto& pp: v) pp->bar(); // 虚派发
	} // ~D called 3 times

	std::cout << "Custom deleter demo\n";
	std::ofstream("../demo.txt") << "zcq"; // 准备要读的文件
	{
		std::unique_ptr<std::FILE, void (*)(std::FILE*) >
				fp( std::fopen("demo.txt", "r"), close_file );
		if(fp) // fopen 可以打开失败；该情况下 fp 保有空指针
			std::cout << (char)std::fgetc(fp.get()) << '\n';
	} // fclose() 调用于此，但仅若 FILE* 不是空指针
	// （即 fopen 成功）

	std::cout << "Custom lambda-expression deleter demo\n";
	{
		std::unique_ptr< D, std::function<void(D*)> >
				p(new D,
				  [](D* ptr)
				  {
					  std::cout << "destroying from a custom deleter...\n";
					  delete ptr;
				  }
		);  // p 占有 D

		p->bar();
	} // 调用上述 lambda 并销毁 D

	std::cout << "Array form of unique_ptr demo\n";
	{
		std::unique_ptr<D[]> p{new D[3]};
	} // 调用 ~D 3 次
}
#endif


#endif

/*
struct sockaddr_in {}; // IPv4 的 socket 套接字 地址
struct sockaddr_in6 {}; // IPv6 的 socket 套接字 地址
struct in_addr ia4{}; // 互联网 IPv4 地址
struct in6_addr ia6{}; // 互联网 IPv6 地址
---------------------------------------------------------------------------
其中每8位代表一个IP地址位中的一个数值.
例如 192.168.3.144 记为 0x9003a8c0,
其中 c0为192, a8为168, 03为3, 90为144
---------------------------------------------------------------------------
//inet_addr() 将一个 点分十进制IP格式的字符串 转换为 二进制网络字节序 值
in_addr_t inet_addr(const char *cp);
参数：字符串，一个点分十进制的 IP 地址，如 "127.0.0.1"
返回值：
	如果正确执行将返回一个无符号长整数型。
	如果传入的字符串不是一个合法的 IP 地址，将返回 INADDR_NONE。
---------------------------------------------------------------------------
//inet_aton() 将一个 点分十进制IP格式的字符串 转换为 二进制网络字节序 结构。
int inet_aton(const char *string, struct in_addr *addr);
参数：
1 输入参数 string 包含 ASCII 表示的 IP地址(点分十进制IP格式的字符串)。
2 输出参数 addr 是将要用新的IP地址更新的结构。
返回值：成功返回 非0，如果输入地址 不正确 则会返回 0。
 	使用这个函数并没有错误码存放在 errno 中，所以它的值会被忽略。
---------------------------------------------------------------------------
//inet_ntoa() 将一个 二进制网络字节序 结构 转换为 点分十进制IP格式的字符串。
char *inet_ntoa(struct in_addr in);
参数：一个网络上的IP地址
返回值：
 	如果正确，返回一个字符指针，指向一块存储着点分格式IP地址的静态缓冲区；
 	错误，返回 NULL。		  	*/



/*
socket	创建进程的 套接字描述符(socketfd 返回值)
int socket(int family, int type, int protocol);
family：协议族，常用的 AF_INET(对应 ipv4 地址)、AF_INET6(对应 ipv6 地址)。
type：Socket 类型，
 	面向连接的流式 SOCK_STREAM 即 TCP 连接，
 	面向数据报 SOCK_DGRAM 即 UDP 连接。
protocol：指定协议，
 	常用的协议 IPPROTO_TCP (TCP 传输)、IPPROTO_UDP (UDP 传输)。
	该 protocol 与 type 需要对应，即不能 socket 类型为 UDP 而协议为 TCP。
 	一般情况下参数 protocol 填为 0 表示 自动使用 socket 类型的协议。
---------------------------------------------------------------------------
bind	绑定 (ipv4/ipv6) 地址 并赋值给 socket。
int bind(int socketfd, const struct sockaddr *addr, socklen_t addrlen);
socketfd：进程套接字描述符，由 socket() 创建。
addr：sockaddr 结构体指针，包含 IP 地址以及端口。
addrlen：sockaddr 结构体长度(字节数)。
---------------------------------------------------------------------------
listen    将一个 socket 变为侦听状态，监听新的连接请求。
int listen(int socketfd, int backlog)
socketfd：监听的套接字描述符。
backlog：最大连接数。
---------------------------------------------------------------------------
accept    接收客户端连接。
客户端 socket()、connect()后发送请求，此时服务端通过 accept()接受请求并建立链接。
正常建立连接后返回 新的 通信套接字描述符
(这里称为 connect_fd 需要与前面的 socketfd 区分)。
int accept(int socketfd, struct sockaddr *addr, socklen_t *addrlen)
socketfd：监听的套接字描述符。
addr：sockaddr 结构体指针，
 	与 socket()时候的 区别是，这里保存的是 客户端的 IP 地址以及端口。
addrlen：sockaddr 结构体长度(字节数)。
---------------------------------------------------------------------------
write/read、send/recv	服务端 与 客户端 进行 I/O 通信。
ssize_t write(int connect_fd, const void *buf, size_t len)
ssize_t read (int connect_fd,       void *buf, size_t len)
ssize_t send (int connect_fd, const void *buf, size_t len, int flags)
ssize_t recv (int connect_fd,       void *buf, size_t len, int flags)
connect_fd：已连接的 套接字描述符。
buf：数据缓冲区。
len：数据缓冲区 buf 长度。
flags：该参数如果为 0 则与 write、read 相同意义；或者是以下的组合：
	MSG_OOB：表示可以接收或者发送带外数据。
	MSG_DONTROUTE：不查找表；
		是 send()使用的标志，这个标志告诉 IP，目的主机在本地网络上，没有必要查找表，
		这个标志一般用在网络诊断和路由程序里面。
	MSG_PEEK：查看数据，并不从系统缓冲区移走数据；
		是 recv()使用的标志，只是从系统缓冲区中读取内容，而不清楚系统缓冲区的内容。
		在下次读取的时候依然是一样的内容，一般在有过个进程读写数据的时候使用这个标志。
	MSG_WAITALL：等待所有数据
		是 recv()的使用标志，表示等到所有的信息到达时才返回，使用这个标志的时候，
		recv() 返回一直阻塞，直到指定的条件满足时，或者是发生了错误。
返回值：成功返回 读取/写入 字节数，失败的时候返回 - 1，通过 errno 捕捉异常
---------------------------------------------------------------------------
connect	试图建立一个 TCP 连接
send	通过一个socket发送数据
recv	通过一个socket收取数据
select	判断一组socket上的读事件
gethostbyname	通过域名获取机器地址
close	关闭一个套接字，回收该 socket 对应的资源，Windows 系统中对应的是closesocket
shutdown	关闭 socket 收或发通道
setsockopt	设置一个套接字选项
getsockopt	获取一个套接字选项
---------------------------------------------------------------------------
getsockname		获取 与某个套接字 关联的 本地 IP地址 和 本地端口号。
int getsockname (int socket_fd, struct sockaddr addr, socklen_t *len);
---------------------------------------------------------------------------
getsockname		获取 与某个套接字 关联的 外地 IP地址 和 外地端口号。
int getpeername (int socket_fd, struct sockaddr addr, socklen_t *len);

 */


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
