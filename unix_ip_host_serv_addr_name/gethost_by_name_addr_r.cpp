//
// Created by zcq on 2021/3/29.
//

#include "ip_port_addrs_names.h"

#include <netdb.h>
#include <sys/socket.h>


/**	可重入版本
	gethostbyname_r()，只能 获得 IPv4 信息！
	gethostbyname2_r()，
	gethostbyaddr_r()    				**********************************/

#if 0
int main(int argc, char **argv) {
	host_display_r();
}
#endif

void host_display_r()
{
#if 1
	/* 只能 获得 IPv4 信息！*/
	Get_host_byname_r_IPv4("v.qq.com");
	Get_host_byname_r_IPv4("zcq");
	Get_host_byname_r_IPv4("zcq.local");
	Get_host_byname_r_IPv4("www.baidu.com");
#elif 1
	Get_host_byname2_r("v.qq.com");
	Get_host_byname2_r("zcq");
	Get_host_byname2_r("zcq.local");
	Get_host_byname2_r("www.baidu.com");
#else
	Get_host_byaddr_r("192.168.6.7", AF_INET); // zcq.local
	Get_host_byaddr_r("192.168.6.7", AF_INET6); // zcq.local
	Get_host_byaddr_r("192.168.6.119", AF_INET); // zcq.local
	Get_host_byaddr_r("192.168.6.119", AF_INET6); // zcq.local
	Get_host_byaddr_r("14.215.85.123", AF_INET); // v.qq.com
	Get_host_byaddr_r("14.215.85.123", AF_INET6); // v.qq.com
	Get_host_byaddr_r("127.0.1.1", AF_INET); // zcq
	Get_host_byaddr_r("127.0.1.1", AF_INET6); // zcq
	Get_host_byaddr_r("127.0.0.1", AF_INET); // zcq
	Get_host_byaddr_r("127.0.0.1", AF_INET6); // zcq
#endif
}

/**	客户端直接使用 IP 地址会有很大的弊端，一旦 IP 地址变化（IP 地址会经常变动），
	客户端软件就会出现错误。而使用 域名 或者 主机名 会方便很多，
	注册后的域名只要每年续费就永远属于自己的，
	更换 IP 地址时修改 域名解析 即可，不会影响软件的正常使用。 **/

/** 使用 域名 或者 主机名 或 IP 地址 得到 主机 IP 地址信息 结构体 hostent **/
/*	struct hostent {
		char  * h_name; 	 主机 规范名，
			例如 www.baidu.com 的 规范名 是 www.a.shifen.com

		char ** h_aliases; 	 主机 别名 列表，一个主机可以有多个别名，
			可以通过多个域名访问同一主机。同一 IP地址可以绑定多个域名，
			因此除了当前域名还可以指定其他域名。
			例如 www.baidu.com 就是 baidu 他自己的 别名

		int  h_addrtype; 	 主机 IP地址 类型，IPv4:AF_INET，IPv6:AF_INET6

		int  h_length; 		 主机 IP地址 字节长度，
			IPv4 字节长度为 4，IPv6 字节长度为 16

		char ** h_addr_list; 主机 IP地址 列表， 二进制网络字节序格式 IP 地址
			（ 二进制网络字节序格式，需要通过 inet_ntop()转出 字符串 IP地址）
			对于用户较多的服务器，可能会分配多个 IP 地址给同一域名，
			利用多个服务器进行均衡负载
	}	*/

/** gethostbyaddr_r()作用：用 IP 地址 获取 主机 IP 信息。 **/
/* 	int gethostbyaddr_r(const void 		*addr,  // 二进制网络字节序格式 IP 地址
 						socklen_t 		len,
						int 			type,  (family：AF_INET 或 AF_INET6)
						struct hostent 	*result_buf,
						char 			*buf,
						size_t			buflen,
						struct hostent 	**result,
						int 			*h_errnop );
参数：
addr：	指向 IPv4 或 IPv6 地址结构 in_addr 或 in6_addr,
 	二进制网络字节序 IP 地址，需要通过 inet_ntop() 输出 ip 二进制网络字节序格式
len：	结构 addr 的大小，IPv4 字节长度为 4，IPv6 字节长度为 16
type：	主机 IP地址 类型，IPv4:AF_INET，IPv6:AF_INET6
resultbuf：	成功的情况下存储结果用。
buf：	这是一个临时的缓冲区，用来存储过程中的各种信息，一般8192大小就够了，
	可以申请一个数组 char buf[8192]。
buflen：是buf缓冲区的大小。
result：如果成功，则这个hostent指针指向resultbuf，也就是正确的结果；如果失败，则result为NULL。
h_errnop：存储错误码。
返回: 成功返0，失败返非0  */

/** gethostbyname_r()作用：用 域名 或者 主机名 获取 主机 IP 信息。
	域名 仅仅是 IP 地址的一个助记符，目的是方便记忆，
	通过域名并不能找到目标计算机，通信之前必须要将 域名 转换成 IP 地址。 **/
/* 	int gethostbyname_r ( 	const char 		*name,
 							struct hostent 	*resultbuf,
							char 			*buf,
							size_t 			buflen,
							struct hostent 	**result,
							int 			*h_errnop );
参数：
name：	主机名，也就是 域名 字符串，如百度的 hostname 名是www.baidu.com。
resultbuf：	成功的情况下存储结果用。
buf：	这是一个临时的缓冲区，用来存储过程中的各种信息，一般8192大小就够了，
	可以申请一个数组 char buf[8192]。
buflen：是buf缓冲区的大小。
result：如果成功，则这个hostent指针指向resultbuf，也就是正确的结果；如果失败，则result为NULL。
h_errnop：存储错误码。
返回: 成功返0，失败返非0  */

/** gethostbyname2_r()作用：用 域名 或者 主机名 获取 主机 IP 信息。
	域名 仅仅是 IP 地址的一个助记符，目的是方便记忆，
	通过域名并不能找到目标计算机，通信之前必须要将 域名 转换成 IP 地址。 **/
/* 	int gethostbyname2_r (	const char 		*name,
 							int 			af,
			     			struct hostent 	*result_buf,
			    			char 			*buf,
			    			size_t 			buflen,
			     			struct hostent 	**result,
			     			int 			*h_errnop );
参数：
name：	主机名，也就是 域名 字符串，如百度的 hostname 名是www.baidu.com。
af：	主机 IP地址 类型，IPv4:AF_INET 同 gethostbyname_r()，IPv6:AF_INET6
resultbuf：	成功的情况下存储结果用。
buf：	这是一个临时的缓冲区，用来存储过程中的各种信息，一般8192大小就够了，
	可以申请一个数组 char buf[8192]。
buflen：是buf缓冲区的大小。
result：如果成功，则这个hostent指针指向resultbuf，也就是正确的结果；如果失败，则result为NULL。
h_errnop：存储错误码。
返回: 成功返0，失败返非0  */

/*************************************************************************/
void host_lambda_func(std::string_view str, char **ptr, const int &mode)
{
	char buf[INET_ADDRSTRLEN];
	char buf6[INET6_ADDRSTRLEN];
	std::cout << str;
	for (int i = 0; *ptr != nullptr; ++i, ++ptr) {
		if (i % 3 == 0) std::cout << std::endl;
		else std::cout << "\t\t   ";
		switch (mode) {
			case 0:
				std::cout << *ptr;
				break;
			case AF_INET:
				std::cout << inet_ntop(AF_INET, *ptr, buf, INET_ADDRSTRLEN);
				break;
			case AF_INET6:
				std::cout << inet_ntop(AF_INET6, *ptr, buf6, INET6_ADDRSTRLEN);
				break;
			default:
				break;
		}
	}
	std::cout << std::endl;
}

void host_byname_echo(struct hostent *hostent_ptr)
{
	std::cout << "--------    ---------" << std::endl;
	std::cout << "主机 规范名 hostname: " << hostent_ptr->h_name << std::endl;
	host_lambda_func("主机 别名 alias:", hostent_ptr->h_aliases, 0);
	switch (hostent_ptr->h_addrtype) {
		case AF_INET:
			std::cout << "主机 IP地址 类型:\tAF_INET = " << AF_INET << std::endl;
			std::cout << "主机 IP地址 字节长度:\t" << hostent_ptr->h_length << std::endl;
			host_lambda_func("主机 IPv4 地址 列表:",
							 hostent_ptr->h_addr_list, AF_INET);
			break;
		case AF_INET6:
			std::cout << "主机 IP地址 类型:\tAF_INET6 = " << AF_INET6 << std::endl;
			std::cout << "主机 IP地址 字节长度:\t" << hostent_ptr->h_length << std::endl;
			host_lambda_func("主机 IPv6 地址 列表:",
							 hostent_ptr->h_addr_list, AF_INET6);
			break;
		default:
			err_sys(-1, __LINE__,"unknown address type");
			break;
	}
}


/*************** gethostbyname_r 只能 获得 IPv4 信息！ *********************/

/* gethostbyname_r()：用 域名 或者 主机名 获取 主机 IP 信息。只能 获得 IPv4 信息！ */
int Get_host_byname_r_IPv4(std::string_view hostname)
{
	struct hostent result_buf{};
	struct hostent *hostent_ptr; // IP 地址信息，成功则指向 result_buf，失败则为 NULL
	size_t buflen = BUFFSIZE;
	char buf[buflen];
	bzero(buf, buflen);
	int h_errnop;
	int retval = gethostbyname_r(hostname.data(),/* 主机名/域名 */
								 &result_buf, /* 成功则存储 结果 */
								 buf, 		  /* 临时的缓冲区，存储过程中的各种信息 */
								 buflen, 	  /* 临时的缓冲区 大小 */
								 &hostent_ptr,/* 成功则指向 result_buf，失败则为 NULL */
								 &h_errnop 	  /* 存储错误码 */
	);
	err_h_errno(retval, h_errnop, __LINE__);

	std::cout << "============================= " << __func__ << std::endl;
	std::cout << "hostname:\t" << hostname << std::endl;
	host_byname_echo(&result_buf);
//	host_byname_echo(hostent_ptr);
	return retval;
}

/*************************** gethostbyname2_r *****************************/

/* gethostbyname2_r()：用 域名 或者 主机名 获取 主机 IP 信息。 */
int Get_host_byname2_r(std::string_view hostname, int af)
{
	struct hostent result_buf{};
	struct hostent *hostent_ptr; // IP 地址信息，成功则指向 result_buf，失败则为 NULL
	size_t buflen = BUFFSIZE;
	char buf[buflen];
	bzero(buf, buflen);
	int h_errnop;
	int retval = gethostbyname2_r(hostname.data(),/* 主机名/域名 */
								  af, 		   /* IPv4:AF_INET，IPv6:AF_INET6 */
								  &result_buf, /* 成功则存储 结果 */
								  buf, 		   /* 临时的缓冲区，存储过程中的各种信息 */
								  buflen, 	   /* 临时的缓冲区 大小 */
								  &hostent_ptr,/* 成功则指向result_buf，失败则为NULL */
								  &h_errnop    /* 存储错误码 */
	);
	err_h_errno(retval, h_errnop, __LINE__);

	std::cout << "============================= " << __func__ << std::endl;
	std::cout << "hostname:\t" << hostname << std::endl;
	host_byname_echo(&result_buf);
//	host_byname_echo(hostent_ptr);
	std::cout << "============================= " << std::endl;
	return retval;
}

/*************************** gethostbyaddr_r *******************************/

/* gethostbyaddr_r()作用：用 IP 地址 获取 主机 IP 信息。 */
int Get_host_byaddr_r(std::string_view hostaddr, const int &addrtype)
{
	/* 输出 ip 二进制网络字节序格式 */
	std::string in_addr_str= addr_str_to_net_r(hostaddr, addrtype);

	socklen_t 	h_length = 4; // 结构 addr 的大小，IPv4 字节长度为 4，IPv6 字节长度为 16
	switch (addrtype) {
		case AF_INET:			h_length = 4;			break;
		case AF_INET6:			h_length = 16;			break;
		default: break;
	}
	struct hostent 	result_buf{};
	struct hostent 	*hostent_ptr; // IP 地址信息，成功则指向 result_buf，失败则为 NULL
	size_t 			buflen = BUFFSIZE;
	char 			buf[buflen];
	bzero(buf, buflen);
	int 			h_errnop;
	int retval = gethostbyaddr_r(in_addr_str.c_str(), /* 二进制网络字节序格式 IP 地址*/
								 h_length, 	  /* IPv4 字节长度为 4，IPv6 字节长度为 16 */
								 addrtype,		  /* AF_INET 或 AF_INET6 */
								 &result_buf, /* 成功则存储 结果 */
								 buf, 		  /* 临时的缓冲区，存储过程中的各种信息 */
								 buflen, 	  /* 临时的缓冲区 大小 */
								 &hostent_ptr,/* 成功则指向 result_buf，失败则为 NULL */
								 &h_errnop 	  /* 存储错误码 */
	);
	err_h_errno(retval, h_errnop, __LINE__);

	std::cout << "============================= " << __func__ << std::endl;
	std::cout << "hostaddr:\t" << hostaddr << std::endl;
	host_byname_echo(&result_buf);
//	host_byname_echo(hostent_ptr);
	return retval;
}
