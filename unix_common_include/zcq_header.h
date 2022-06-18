
#pragma once

/*************************************************************************/
#include <fstream> // 文件流 <istream> <ostream>
#include <iostream> // 标准流 <istream> <ostream>
#include <stdexcept>
#include <linux/sockios.h>
#include <arpa/inet.h> //for inet_addr
#include <iomanip> // 格式控制符 dec是十进制 hex是16进制 oct是8进制
#include <sys/socket.h> /* basic socket definitions */
#include <ctime> // std::time_t, std::tm, std::localtime, std::mktime
#include <cerrno>
#include <fcntl.h>  //  File Control Operations
#include <netdb.h>
#include <csignal>
#include <cstdio>
#include <string>
#include <string_view>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h> /* for S_xxx file mode constants */
#include <sys/uio.h>  /* for iovec{} and readv/writev */
#include <sys/wait.h>
#include <sys/un.h>  /* for Unix domain sockets */
#include <memory>
#include <algorithm>
#include <vector>
#include <syslog.h>
#include <sys/param.h>
//#include <event.h>
#include <event2/event.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <kqueue/sys/event.h>
#include <sys/ioctl.h>
#include <climits>		/* for OPEN_MAX */
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <netinet/sctp.h> /* note if sctp does not exist we blow up :> */
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <net/route.h>
#include <net/ethernet.h>
#include <error.h>
#include <unistd.h> //for read
#include <sys/select.h>
#include <initializer_list>
#include <linux/socket.h>
#include <net/if_arp.h>
#include <net/if_ppp.h>
#include <net/if_slip.h>
#include <net/if_shaper.h>
#include <linux/netlink.h>
#include <ifaddrs.h>
#include <sys/cdefs.h>
#include <linux/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h> // POSIX Standard: Primitive System Data Types
#include <unistd.h> // POSIX Standard: Symbolic Constants
#include <linux/if_packet.h>
#include <linux/if_ether.h> /* 链路层协议 */
#include <sys/user.h>
#include <sys/utsname.h>
#include <csetjmp>
#include <chrono> // std::chrono::system_clock

/**===== c++ 线程 可和 POSIX 线程 联合使用 =====**/
#include <future>
#include <threads.h> // ISO C11 标准
/**------- c++ 线程 -------**/
#include <thread> // C++ 多线程
#include <atomic> // C++ 多线程
#include <mutex> // 互斥锁
#include <condition_variable> // C++ 多线程
/**------- POSIX 线程 -------**/
#include <pthread.h>
/**-----------------------**/



/*************************************************************************/
#include "zcq_config.h" // 配置选项

#include "IO_head.h" /* I/O 函数 */

#include "ip_port_addrs_names.h"
#include "unix_process.h"
#include "if_quit_or_run.h"

/** err_sys(-1, __LINE__, "%s()", __func__); **/
/** err_sys(-1, __FILE__, __func__, __LINE__, "%s()", __func__); **/
#include "error_print.h" // 错误打印消息

#include "sock_ntop_pton.h"
#include "sock_addr_config.h"  /* ---- sock address 配置 ---- */
#include "tcp_client_socket_connect_addrinfo.h"
#include "stdin_input.h"
#include "tcp_service_socket_bind_listen_addrinfo.h"
#include "service_accept.h"
#include "udp_client_socket_connect_config_addrinfo.h"
#include "udp_service_socket_bind_addrinfo.h"
#include "ip_port_addrs_names.h"
#include "sock_fd_select_timeo.h"
#include "IO_fcntl.h"
#include "get_time_debug.h"
#include "tcp_client_web.h"
#include "IO_ioctl_socket.h"
#include "IO_File.h"
#include "multicast.h"
#include "wrap_family_convert.h"
#include "rtt.h"
#include "wrap_sockopt.h"
#include "sctp_bind_list.h"
#include "sctp_event.h"
#include "tcp_OOB_heartbeat.h"


/*************************************************************************/
/* 	使用协议 protocol 在域 domain 中创建类型为 type 的 新 套接字描述符。
 	如果 protocol 为零(IPPROTO_IP=0)，则会自动选择一个。
 	成功返回 新 套接字描述符，如果错误则返回 -1。
int socket (int domain, int type, int protocol );
---------------------------------------------------------------------------
	创建 两个 新 套接字描述符，
	它们在域 domain 中类型为 type 并使用协议 protocol 相互连接，
 	并将它们的文件描述符放入 fds[0] 和 fds[1] 中。
 	如果 protocol 为零(IPPROTO_IP=0)，则会自动选择一个。
 	成功返回 0，错误返回 -1。
int socketpair (int domain, int type, int protocol, int fds[2] );
基本用法：
	1. 这对套接字可用于 全双工通信，每一个套接字 既可读 又可写。
 例如，可以往 fds[0] 中写，从 fds[1] 中读；或者从 fds[1] 中写，从 fds[0] 中读。
	2. 如果 读写 操作位于 同一进程，文件描述符 fds[0] 和 fds[1] 是进程共享的，
 往一个套接字(如 fds[0])中写入后，再从该套接字 读时会阻塞，
 只能 在另一个套接字中(如 fds[1])上 读 成功；
	3. 如果 读写 操作位于 不同进程，文件描述符 fds[0] 和 fds[1] 是进程共享的，
 比如父子进程，一般会功能分离，一个用来读，一个用来写。       */

/************************************ tcp ****/
#include "tcp_service_echo.h"
#include "tcp_client_call.h"

void tcp_service_fork();
void tcp_service_fork_Signal();
void tcp_service_IO_select();
void tcp_service_IO_poll();
void tcp_service_IO_poll_addrinfo();
void tcp_service_IO_epoll_addrinfo();
void tcp_service_daemon_inetd();
void tcp_service_IO_nonblock_select();
void tcp_service_OOB_Signal_Setsockopt();
void tcp_service_OOB_nonblock_select();

/* 心搏机制：周期信号轮询对端，无响应即不存活 */
void tcp_service_OOB_heartbeat_fork_Signal();

void pthread_tcp_service_addrinfo();

/**---------**/

void tcp_client_one();
void tcp_client_for();
void tcp_client_one_byNamePort();
void tcp_client_one_addrinfo();
void tcp_client_OOB();

/* 心搏机制：周期信号轮询对端，无响应即不存活 */
void tcp_client_OOB_heartbeat_pselect();

void pthread_tcp_client_addrinfo();

/************************************ udp ****/
#include "udp_service_echo.h"
#include "udp_client_call.h"

void udp_service();
void udp_service_addrinfo();
void udp_service_addrinfo_daemon_syslog();
void udp_service_bind_all_IPv4();

void udp_client_one();
void udp_client_one_config();
void udp_client_one_addrinfo();
void udp_client_broadcast();
void udp_multicast();
void udp_servicr_multicast_SAPSDP();
void udp_client_multicast_SNTP();

/************************************ udp && tcp ****/
void udp_tcp_service_fork_Signal_IO_select();

/************************************ sctp ****/
/* SCTP应用编程中的注意事项
SCTP和TCP、UDP有些不一样，在写应用程序的时候需要注意一下。

1. 可以bind多个IP地址
这个是SCTP的特点，两端都可以用多个IP地址和对端来通信，
 因此不管是client端，还是server端，SCTP都可以bind多个IP地址（端口号必须相同）。
比如：
// bind 第一个地址
bind(fd, (struct sockaddr *)&cliaddr1, sizeof(cliaddr));
// bind 第二个地址
sctp_bindx(fd, (struct sockaddr *)&cliaddr2, 1, SCTP_BINDX_ADD_ADDR);
cliaddr1 和 cliaddr2 里面，IP地址 不同，port 必须相同。

2. 需要注册事件通知
为了能调用 sctp_recvmsg()收到对端发送的DATA，需要事先为该socket注册一个事件通知功能：
struct sctp_event_subscribe events;
events.sctp_data_io_event = 1; 		// I/O 通知事件
setsockopt(fd, SOL_SCTP, SCTP_EVENTS, (const void *)&events, sizeof(events));
不注册这个事件通知，是不能通过调用 sctp_recvmsg() 接收对端的 DATA的。
类似的，注册其他事件类型之后，
 可以通过sctp_recvmsg()收到 SCTP_COMM_UP（连接建立完成）、
 SCTP_SHUTDOWN_EVENT（连接被关闭）等通知。   */
#include "sctp_wrap.h"
#include "sctp_service_echo.h"
#include "sctp_client_call.h"

void sctp_service();
void sctp_service_AutoCloseClient(); /* 自动关闭 指定空闲时间的 客户端 */
void sctp_service_event(); /* 设置 所有 感兴趣 SCTP 通知事件 */

/* 一到多 转 一对一 的迸发SCTP服务；需用 sctp_peelof() 从sctp服务获取一对一套接字 */
void sctp_service_fork();

void sctp_client();
void sctp_client_event();

/************************************ unix ( AF_LOCAL ) ****/
void unix_STREAM_service_fork();
void unix_STREAM_service_IO_epoll_addrinfo();

void unix_STREAM_client_one();
/**------------------------------**/
void unix_DGRAM_service();

void unix_DGRAM_client();
/**------------------------------**/
/* 读写操作 位于 同一进程 */
void unix_socketpair_same();

/* 读写操作 位于 不同进程（父子进程） */
void unix_socketpair_diff();

/* 读写操作 位于 不同进程（父子进程）
 * 进程间传递文件描述符 recv_fd
 * 单个读/写缓冲区 */
/**	进程间传递文件描述符
 0.socketpair()创建一个流管道，返回 两个 相互连接 的 新 套接字描述符。
 1.system()执行 shell 命令：
 	g++-10 -Wall -ggdb -std=c++17 ../test/openfile.cpp -o ./openfile.o
 2.注册信号 SIGCHLD：父进程 捕捉 子进程 的状态信号
 3.fork()创建的 子进程：
 	3.1 创建文件 ../test/test.txt 并写入数据；
 	3.2 execlp()执行 shell 命令：
 		./openfile.o ./openfile.o socketpair_fd[1] ../test/test.txt O_RDONLY nullptr
 	3.3 子进程 结束。
 4.父进程：
 	4.1 等待描述符 socketpair_fd[0] 变为可读
 	4.2 读取文件直至结束
 	4.3 父进程 结束。                   **/
void unix_socketpair_system_fork_exec_fds();


/*************************************************************************/
/*	ANSI C标准中有几个标准预定义宏（也是常用的）：
 __LINE__：在源代码中插入当前源代码行号；
 __FILE__：在源文件中插入当前源文件名；
 __func__、__FUNCTION__：在源文件中插入当前函数名；
 __DATE__：在源文件中插入当前的编译日期
 __TIME__：在源文件中插入当前编译时间；
 __STDC__：当要求程序严格遵循ANSI C标准时该标识被赋值为1；
 __cplusplus：当编写C++程序时该标识符被定义。
---------------------------------------------------------------------------
	在单一的宏定义中，只能使用 “#” 或 “##” 中的 一种 操作符。除非必要，否则尽量都不使用。
 	#: 	把参数转换成字符串
例子：		#define P(A) 	printf( "%s=%d\n", #A, A);
        	int a=1;	int b=2;	P(a+b);		结果：a+b=123
	##:	粘合剂，将前后两部分粘合起来。
例子：		#define name(n) 	zcq ## n
 			name(666) 	结果：zcq666
---------------------------------------------------------------------------
 __VA_ARGS__: 可变参数的宏
例子：			#define str(...) printf(__VA_ARGS__)
				str("0123456") 	结果：0123456
例子：			#define str(...) printf(__VA_ARGS__)
				str("%d", 123) 	结果：123
---------------------------------------------------------------------------
 ##__VA_ARGS__: 当可变参数的个数为0时，这里##起到把前面多余的","去掉的作用,否则编译出错
例子：			#define myprint(fmt,...) 	printf(fmt,##__VA_ARGS__)
				myprint("1+2") 			结果：1+2
				myprint("1+2","=3") 	结果：1+2=3       */


#ifndef Debug_line
#define Debug_line

// debug_line(__LINE__,__func__,"");
#define debug_line [](const int &line, const char *name , const char * str="") \
{ std::cout << ">—>—>— Line: " << line << " --- " << name << "(): " << str << std::endl;}

#if 0
// _debug_log_info(" ")
#define _debug_log_info(fmt, ...)  { \
printf(">—>—> %d  %s()  INFO: ", __LINE__, __func__); \
printf(fmt, ##__VA_ARGS__); printf("\n");   }
#elif 1
// _debug_log_info(" ")
#define _debug_log_info(fmt, ...)  { \
printf(">—>—> %d\tINFO: ", __LINE__); \
printf(fmt, ##__VA_ARGS__); printf("\n");   }
#else
// _debug_log_info(" ")
#define _debug_log_info(fmt, ...)  {}
#endif

#endif