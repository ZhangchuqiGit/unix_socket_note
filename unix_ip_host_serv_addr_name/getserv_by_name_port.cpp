//
// Created by zcq on 2021/3/29.
//

#include "ip_port_addrs_names.h"

#include <netdb.h>
#include <sys/socket.h>


/************************* 不可重入版本 不推荐使用 **************************/

/** 不推荐使用 getservbyname() 和 getservbyport() **/

/**	可重入版本 getservbyname_r() 和 getservbyport_r() 推荐使用 **/


#if 0
int main(int argc, char **argv) {
	serv_display();
}
#endif

void serv_display() /* $ cat /etc/services */
{
#if 0
	Get_serv_byname("daytime", "tcp");

	Get_serv_byname("domain", "tcp");
	Get_serv_byname("domain", nullptr);
	Get_serv_byname("domain", "udp");

	Get_serv_byname("ftp", "tcp");
	Get_serv_byname("ftp", nullptr);
	Get_serv_byname("ftp", "udp"); // error, no exit "udp"

	Get_serv_byname("echo", "tcp");
	Get_serv_byname("echo", "udp");

	Get_serv_byname("ssh", "tcp");
	Get_serv_byname("ssh", "udp"); // error, no exit "udp"
#else
	Get_serv_byport(53/*domain*/,"tcp");
	Get_serv_byport(53/*domain*/,"udp");
	Get_serv_byport(53/*domain*/,nullptr);
#endif
}

/** 服务名 和 端口号 的映射存放在 /etc/services 配置文件中，尽管 端口号 会发生改变，
仅需 sudo gedit /etc/services 修改文件中某个服务对应的 端口号(不需要重新编译程序) **/

/** 使用 服务名+协议名 或 端口号+协议名 得到 服务 IP 信息 结构体 servent **/
/* 	struct servent {
		char*   s_name;     服务名
		char**  s_aliases;  服务 别名 列表
		int     s_port;     端口号（网络字节序，需要通过 ntohs(port)转出 数字 portnum）
		char*   s_proto;    协议名
	}	 	*/

/** getservbyname()作用：使用 服务名+协议名 获取 服务 IP 信息。 **/
/* struct servent * getservbyname (const char* servname, const char* protoname);
参数：	servname 服务名；
 		protoname 协议名，有 "tcp","udp"等，"NULL"表任何协议名
返回: 对应的 服务 IP 信息( servent 结构体)；失败返回 NULL，同时设置 h_errno
struct servent* sptr = getservbyname ("domain", "udp");
struct servent* sptr = getservbyname ("ftp", NULL);			 */

/** getservbyport()作用：使用 端口号+协议名 获取 服务 IP 信息。 **/
/*	struct servent * getservbyport (int port, const char* protoname);
参数：	port 端口号，必须是 网络字节序，需要通过 htons( portnum )转入 网络字节序 port；
 		protoname 协议名，有 "tcp","udp"等，"NULL"表任何协议名
返回: 对应的 服务 IP 信息( servent 结构体)；失败返回 NULL，同时设置 h_errno
struct servent* sptr = getservbyport (htons(53), NULL);
struct servent* sptr = getservbyport (htons(21), "tcp");	*/

/*************************************************************************/

void serv_lambda_func(std::string_view str, char **ptr)
{
	std::cout << str;
	for (int i = 0; *ptr != nullptr; ++i, ++ptr) {
		if (i % 3 == 0) std::cout << std::endl;
		else std::cout << "\t  \t";
		std::cout << *ptr;
	}
	std::cout << std::endl;
}

void serv_byname(struct servent *servent_ptr)
{
	std::cout << "========================================" << std::endl;
	std::cout << "服务名 servname:\t" << servent_ptr->s_name << std::endl;
	serv_lambda_func("服务 别名 alias:", servent_ptr->s_aliases);
	std::cout << "端口号 port:\t\t" << ntohs(servent_ptr->s_port) << std::endl;
	std::cout << "协议名 proto:\t" << servent_ptr->s_proto << std::endl;
}

/**************************** getservbyname ****************************/

/* getservbyname()作用：使用 服务名+协议名 获取 服务 IP 信息。 */
struct servent * Get_serv_byname(std::string_view servname, const char * protoname)
{
	struct servent *servent_ptr; // 服务 IP 信息
	/* 使用 服务名+协议名 获取 服务 IP 信息 */
	if ((servent_ptr=getservbyname(servname.data(), protoname)) == nullptr) {
		err_msg(-1, __LINE__,
				"getservbyname()\nservname:%s\t protoname:%s",
				servname.data(), protoname);
		return (struct servent * )nullptr;
	}
	serv_byname(servent_ptr);
	return servent_ptr;
}

/**************************** getservbyport ****************************/

/* getservbyport()作用：使用 端口号+协议名 获取 服务 IP 信息。 */
struct servent * Get_serv_byport(const int &portnum, const char* protoname)
{
	struct servent *servent_ptr; // 服务 IP 信息
	/* 使用 端口号+协议名 获取 服务 IP 信息 */
	if ((servent_ptr=getservbyport(htons(portnum)/*转为网络字节序*/, protoname)) == nullptr) {
		err_msg(-1, __LINE__,
				"getservbyport()\nportnum:%d\t protoname:%s",
				portnum, protoname);
		return nullptr;
	}
	serv_byname(servent_ptr);
	return servent_ptr;
}

