//
// Created by zcq on 2021/4/9.
//

#include <fstream> // 文件流 <istream> <ostream>
#include <iostream> // 标准流 <istream> <ostream>
#include <stdexcept>
#include <linux/sockios.h>
#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read
#include <iomanip> // 格式控制符 dec是十进制 hex是16进制 oct是8进制
#include <sys/types.h> /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <ctime> /* timeval{} for select() */
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
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <netinet/sctp.h> /* note if sctp does not exist we blow up :> */
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <error.h>

#define MAXsize_error (4*1024)

/* 进程间传递文件描述符 */
ssize_t write_fd(int sock_fd, void *ptr, size_t nbytes, int send_fd);

//与系统调用 无关 的 致命 错误打印消息并终止
void err_quit(const int &ret_value, const int &code_line,
			  const char *fmt, ... /* 可直接调用 */ );

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
  	argv[3]	=	one.sh 		# 参数 3	  	*/
int main(int argc, char **argv)
{

	using namespace std;
	if (argc != 4)
		err_quit(-1, __LINE__, "openfile <sock_fd> <filename> <mode>");
	/* mode:open/fcntl */
	int retval = strtoimax(argv[3], nullptr, 10);
	int	fd = open(argv[2], retval);
		if (fd < 0) exit( (errno > 0) ? errno : 255 );

	/* 进程间传递文件描述符 */
	retval = strtoimax(argv[1], nullptr, 10);
	string buf = "ZCQ:进程间传递文件描述符";
	if (write_fd(retval, (char *)buf.c_str(), buf.size(), fd) < 0) exit((errno > 0) ? errno : 255 );
	exit(0);
}

/* sendmsg() 仅套接字描述符 集中写 可选标志 可选对面地址 可选控制信息 */
/* 进程间传递文件描述符 */
ssize_t write_fd(int sock_fd, void *ptr, size_t nbytes, int send_fd)
{
	struct msghdr msghdr_s{};			/* 通讯数据结构 */
	msghdr_s.msg_name = nullptr; 		// 协议地址
	msghdr_s.msg_namelen = 0;			// 协议地址的大小

	/**	在任意时刻，union 中只能有一个数据成员可以有值。
	 	union 成员共享同一块大小的内存，对某一个成员赋值，
	 	会覆盖其他成员的值，一次只能使用其中的一个成员。
	 	当给联合中某个成员赋值之后，该联合中的其它成员就变成未定义状态了。
	 	联合的存储空间至少能够容纳其最大的数据成员，其大小由最大的成员的大小决定*/
	union { 							/** 对齐数据 **/
		struct cmsghdr cmsghdr_s; 		/* 辅助数据 */
		char control[CMSG_SPACE(sizeof(int))];
		// CMSG_SPACE() 辅助数据对象所占用的字节数。包含两个辅助数据之间的填充字节部分。
	} cmsg_control{};
	msghdr_s.msg_control = cmsg_control.control; 				// 辅助数据
	msghdr_s.msg_controllen = sizeof(cmsg_control.control);	// 辅助数据的大小
#if 0
	msghdr_s.msg_control = (caddr_t) &cmsg_control;
	msghdr_s.msg_controllen = sizeof(cmsg_control);
#endif
	struct iovec iovec_s[1]; 			/* 数据缓冲区数组 */
	iovec_s[0].iov_base = ptr;			// 数据缓冲区:数据
	iovec_s[0].iov_len = nbytes;		// 数据缓冲区:数据长度
	msghdr_s.msg_iov = iovec_s; 		// 指定 数据缓冲区数组
	msghdr_s.msg_iovlen = 1;			// msg_iov 中的元素长度

	struct cmsghdr *cmsghdr_ptr;		/* 辅助数据 */
	/* CMSG_FIRSTHDR() 指向 msghdr{}第一个辅助数据 cmsghdr{}结构的指针 */
	cmsghdr_ptr = CMSG_FIRSTHDR(&msghdr_s);
	cmsghdr_ptr->cmsg_len = CMSG_LEN(sizeof(int));
/*	协议	cmsg_level		cmsg_type		说明
	Unix域	SOL_SOCKET		SCM_RIGHTS		发送/接收描述符
							SCM_CREDS		发送/接收用户凭证  */
	cmsghdr_ptr->cmsg_level = SOL_SOCKET;
	cmsghdr_ptr->cmsg_type = SCM_RIGHTS;
/* CMSG_DATA()指向与辅助数据 cmsghdr{}关联的数据 cmsghdr.cmsg_data[]的第一个字节指针 */
	*((int *)CMSG_DATA(cmsghdr_ptr)) = send_fd;

	return sendmsg(sock_fd, &msghdr_s, 0);
}

/***** 不建议直接调用，通过 function(const char *str, ...) ******/
static void err_doit(const int &ret_value,
					 const char *code_file,		/*源文件名*/
					 const char *code_func, 	/*函数名*/
					 const int &code_line, 		/*源代码行号*/
					 const int &system_log_level,
					 const char *fmt, va_list ap ) /* 不建议直接调用 */
{
	if (ret_value < 0) {
		char buf[MAXsize_error] = {'\0'};

		/* 将 *fmt 和 ... 内容 复制到 buf */
		vsnprintf(buf, MAXsize_error/* 要复制的大小 */, fmt, ap);    /* safe */

		size_t buf_len; // strlen(buf) 实际长度
//	size_t retval = std::size(buf)-1; // 最大大小 = MAXLINE + 1
		strcat(buf, "\n"); // 结尾附加 '\n'

		if (errno != 0) {
			buf_len = strlen(buf); // 实际长度
			// strerror(errno) 返回描述 errno 代码含义的字符串
			snprintf(buf + buf_len, MAXsize_error - buf_len,
					 "{ strerror( errno: %d ): %s }", errno,
					 strerror(errno));
			strcat(buf, "\n"); // 结尾附加 '\n'
		}
		if (h_errno != 0) {
			/* 非可重入查找功能的错误状态。特定于线程的 h_errno 变量 */
			buf_len = strlen(buf); // 实际长度
			snprintf(buf + buf_len, MAXsize_error - buf_len,
					 "{ <netdb.h>; hstrerror( h_errno: %d ): %s }",
					 h_errno, hstrerror(h_errno)/* <netdb.h> */ );
			strcat(buf, "\n"); // 结尾附加 '\n'
		}
		if (ret_value < -1) {
			/* Error values for `getaddrinfo' function.  */
			buf_len = strlen(buf); // 实际长度
			snprintf(buf + buf_len, MAXsize_error - buf_len,
					 "{ <netdb.h>; gai_strerror( ret_value: %d ): %s }",
					 ret_value, gai_strerror(ret_value)/* <netdb.h> */ );
			strcat(buf, "\n"); // 结尾附加 '\n'
		}

		std::string strbuf;
		if (code_file != nullptr) {
			strbuf += "File: ";
			strbuf += code_file;
			strbuf += ";\t";
		}
		if (code_func != nullptr) {
			strbuf += "Func: ";
			strbuf += code_func;
			strbuf += ";\t";
		}
		strbuf += "";

#if 1 /* c++ */
			buf_len = strlen(buf); // 实际长度
			if (buf[buf_len-1] == '\n') buf[buf_len-1] = '\0';
			std::cerr << strbuf.c_str() << "Line: " << code_line
					  << ";\treturn value: " << ret_value  << std::endl
					  << buf << std::endl;
#else /* c */
			fflush(stdout);		/* in case stdout and stderr are the same */
			fputs(buf, stderr);
			fflush(stderr);
#endif
		strbuf.clear();
	}
}

void err_doit(const int &ret_value, const int &code_line,
			  const int &system_log_level ,
			  const char *fmt, va_list ap ) /* 不建议直接调用 */
{
	err_doit(ret_value, nullptr, nullptr, code_line,
			 system_log_level, fmt, ap );
}

//与系统调用 无关 的 致命 错误打印消息并终止
void err_quit(const int &ret_value, const int &code_line,
			  const char *fmt, ... /* 可直接调用 */ ) {
	va_list ap;
	va_start(ap, fmt); // <stdarg.h>
	err_doit(ret_value, code_line, LOG_ERR, fmt, ap);
	va_end(ap); // <stdarg.h>
	if (ret_value < 0) {
		std::cerr << __func__ << "():exit" << std::endl;
		exit(ret_value);
	}
}
