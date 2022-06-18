
#include "ip_port_addrs_names.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstdio>
#include <cstring>
#include <iostream>
//using namespace std;

int ip_to_bin()
{
	struct in_addr ia4{}; // 互联网 IPv4 地址
//	struct in6_addr ia6{}; // 互联网 IPv6 地址

	//inet_addr() 将一个 点分十进制IP格式的字符串 转换为 二进制网络字节序 值
	in_addr_t ia4_tmp = inet_addr("192.168.3.144");
	printf("ia4.s_addr = 0x%x\n", ia4_tmp); // 16进制 显示 网络字节序
	ia4.s_addr = ia4_tmp;

	//inet_aton() 将一个 点分十进制IP格式的字符串 转换为 二进制网络字节序 结构。
    inet_aton("192.168.3.144", &ia4);

    /* ia4.s_addr = 0x9003a8c0 */
    printf("ia4.s_addr = 0x%x\n", ia4.s_addr); // 16进制 显示 网络字节序

	//inet_ntoa() 将一个 二进制网络字节序 结构 转换为 点分十进制IP格式的字符串。
	/* inet_ntoa(ia4) = 192.168.3.144 */
	printf("inet_ntoa(ia4) = %s\n", inet_ntoa(ia4));

    return 0;
}

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

