//
// Created by zcq on 2021/3/29.
//

#include "ip_port_addrs_names.h"

#include <netdb.h>
#include <sys/socket.h>


/*************************** 可重入版本 推荐使用 ***************************/

/**	可重入版本 getservbyname_r() 和 getservbyport_r() 推荐使用 **/


#if 0
int main(int argc, char **argv) {
	serv_display_r();
}
#endif

void serv_display_r() /* $ cat /etc/services */
{
#if 0
	Get_serv_byname_r("daytime", "tcp");

	Get_serv_byname_r("domain", "tcp");
	Get_serv_byname_r("domain", nullptr);
	Get_serv_byname_r("domain", "udp");

	Get_serv_byname_r("ftp", "tcp");
	Get_serv_byname_r("ftp", nullptr);
	Get_serv_byname_r("ftp", "udp"); // error, no exit "udp"

	Get_serv_byname_r("echo", "tcp");
	Get_serv_byname_r("echo", "udp");

	Get_serv_byname_r("ssh", "tcp");
	Get_serv_byname_r("ssh", "udp"); // error, no exit "udp"
#else
	Get_serv_byname_r("ssh", "udp"); // error, no exit "udp"

	Get_serv_byport_r(53/*domain*/,"tcp");
	Get_serv_byport_r(53/*domain*/,"udp");
	Get_serv_byport_r(53/*domain*/,nullptr);
#endif
}

/** 服务名 和 端口号 的映射存放在 /etc/services 配置文件中，尽管 端口号 会发生改变，
仅需 sudo gedit /etc/services 修改文件中某个服务对应的 端口号(不需要重新编译程序) **/

/** 使用 服务名+协议名 或 端口号+协议名 得到 服务 IP 信息 结构体 servent **/
/* 	struct servent {
		char*   s_name;     服务名
		char**  s_aliases;  服务 别名 列表
		int     s_port;     端口号（二进制网络字节序，
							需要通过 ntohs(port)转出 数字 portnum）
		char*   s_proto;    协议名
	}	 	*/

/** getservbyname_r()作用：使用 服务名+协议名 获取 服务 IP 信息。 **/
/* 	int getservbyname_r (	const char 		*name,
							const char 		*proto,
							struct servent 	*result_buf,
							char 			*buf,
							size_t 			buflen,
							struct servent 	**result );
参数：
name：	服务名。
proto：	协议名，有 "tcp","udp"等，"NULL"表任何协议名
resultbuf：	成功的情况下存储结果用。
buf：	这是一个临时的缓冲区，用来存储过程中的各种信息，一般8192大小就够了，
	可以申请一个数组 char buf[8192]。
buflen：是buf缓冲区的大小。
result：如果成功，则这个hostent指针指向resultbuf，也就是正确的结果；如果失败，则result为NULL。
返回: 成功返0，失败返非0     */

/** getservbyport_r()作用：使用 端口号+协议名 获取 服务 IP 信息。 **/
/* 	int getservbyport_r (	int 			port,
 							const char 		*proto,
							struct servent 	*result_buf,
							char 			*buf,
							size_t 			buflen,
							struct servent 	**result );
参数：
port: 	端口号，必须是 网络字节序，需要通过 htons( portnum )转入 二进制网络字节序 port；
proto：	协议名，有 "tcp","udp"等，"NULL"表任何协议名
resultbuf：	成功的情况下存储结果用。
buf：	这是一个临时的缓冲区，用来存储过程中的各种信息，一般8192大小就够了，
	可以申请一个数组 char buf[8192]。
buflen：是buf缓冲区的大小。
result：如果成功，则这个hostent指针指向resultbuf，也就是正确的结果；如果失败，则result为NULL。
返回: 成功返0，失败返非0	    */


/**************************** getservbyname_r ****************************/

/* getservbyname_r()作用：使用 服务名+协议名 获取 服务 IP 信息。 */
int Get_serv_byname_r(std::string_view servname, const char *protoname)
{
	struct servent result_buf{};
	struct servent *servent_ptr; // IP 地址信息，成功则指向 result_buf，失败则为 NULL
	size_t buflen = BUFFSIZE;
	char buf[buflen];
	bzero(buf, buflen);
	int retval = getservbyname_r(servname.data(), /* 服务名 */
								 protoname,   /* 协议名，"tcp","udp","NULL" */
								 &result_buf, /* 成功则存储 结果 */
								 buf, 		  /* 临时的缓冲区，存储过程中的各种信息 */
								 buflen, 	  /* 临时的缓冲区 大小 */
								 &servent_ptr /* 成功则指向 result_buf，失败则为 NULL */
	);
	err_msg(retval, __LINE__,"%s():\tportnum:%d;\tprotoname:%s",
			__func__, servname.data(), protoname);

	serv_byname(&result_buf);
//	serv_byname(servent_ptr);
	return retval;
}

/**************************** getservbyport_r ****************************/

/* getservbyport_r()作用：使用 端口号+协议名 获取 服务 IP 信息。 */
int Get_serv_byport_r(const int &portnum, const char* protoname)
{
	struct servent result_buf{};
	struct servent *servent_ptr; // IP 地址信息，成功则指向 result_buf，失败则为 NULL
	size_t buflen = BUFFSIZE;
	char buf[buflen];
	bzero(buf, buflen);
	int retval = getservbyport_r(htons(portnum), /* 端口号，必须是 网络字节序 */
								 protoname,	  /* 协议名，"tcp","udp","NULL" */
								 &result_buf, /* 成功则存储 结果 */
								 buf, 		  /* 临时的缓冲区，存储过程中的各种信息 */
								 buflen, 	  /* 临时的缓冲区 大小 */
								 &servent_ptr /* 成功则指向 result_buf，失败则为 NULL */
	);
	err_msg(retval, __LINE__,"%s():\tportnum:%d;\tprotoname:%s",
			__func__, portnum, protoname);

	serv_byname(&result_buf);
//	serv_byname(servent_ptr);
	return retval;
}

