//
// Created by zcq on 2021/4/27.
//

#include "sctp_bind_list.h"

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

#define  sctp_bind_mode     1

/* SCTP 可以 bind 多个 IP地址（端口号必须相同） */
void sctp_bind_arg_list(const int &sock_fd, std::string_view ip_buf,
						const int &port, const int &flags)
{
	sctp_bind_arg_list(sock_fd, ip_buf, std::to_string(port), flags);
}

/* SCTP 可以 bind 多个 IP地址（端口号必须相同） */
void sctp_bind_arg_list(const int &sock_fd, std::string_view ip_buf,
						std::string_view port_str, const int &flags)
{
#if sctp_bind_mode==0          /* 默认 最简单高效 */
	sctp_bind_arg_list_efficient(sock_fd, ip_buf, port_str, flags);
#elif sctp_bind_mode==1
	sctp_bind_arg_list_vector(sock_fd, ip_buf, port_str, flags);
#elif sctp_bind_mode==2
	sctp_bind_arg_list_unique(sock_fd, ip_buf, port_str, flags);
#endif
}


/* SCTP 可以 bind 多个 IP地址（端口号必须相同） *//**简单但不省内存**/
void sctp_bind_arg_list_efficient(const int &sock_fd, std::string_view ip_buf,
								  std::string_view port_str, const int &flags)
{
	_debug_log_info("SCTP 可以 bind 多个 IP地址（端口号必须相同）")
	const int family = Sockfd_to_family(sock_fd);

	/* 	输入列表内容，如：111 222  333 zzz    gsfg ; 获取每一个子内容 */
	std::vector<std::string> ip_vec = strtovec(ip_buf);
	if (ip_vec.empty()) return;
	_debug_log_info("ip_vec.size(): %zu", ip_vec.size())

	std::vector<struct sockaddr_storage> ipbuf_vec;
	for (const auto &ip : ip_vec)
	{
		struct sockaddr_storage ss{};
		struct addrinfo *result, *saveTOfree;

		result = Get_addrinfo(ip, port_str,
							  AF_UNSPEC, SOCK_SEQPACKET);
		saveTOfree = result;
		do {
			if (family != result->ai_family) {
				_debug_log_info(" diacard ip: %s",
								get_addr_port_ntop_r(result->ai_addr).c_str() )
				continue;
			}
			memcpy(&ss, result->ai_addr, result->ai_addrlen);
			ipbuf_vec.push_back(ss);
		} while ( (result = result->ai_next) != nullptr );
		freeaddrinfo(saveTOfree); // 释放结构体链表指向的任何动态存储空间
	}
	for (auto &ip : ipbuf_vec) {
		std::cout << "------------------- bind ip: "
				  << get_addr_port_ntop_r((struct sockaddr *) &ip) << std::endl;
		err_sys( sctp_bindx(sock_fd, (struct sockaddr *) &ip,
							1, flags),
				 __FILE__, __func__, __LINE__, "sctp_bindx():%s",
				 get_addr_port_ntop_r((struct sockaddr *) &ip).c_str());
	}
}


/* SCTP 可以 bind 多个 IP地址（端口号必须相同） *//**省内存**/
void sctp_bind_arg_list_vector(const int &sock_fd, std::string_view ip_buf,
							   std::string_view port_str, const int &flags)
{
	_debug_log_info("SCTP 可以 bind 多个 IP地址（端口号必须相同）")
	const int family = Sockfd_to_family(sock_fd);

	/* 	输入列表内容，如：111 222  333 zzz    gsfg ; 获取每一个子内容 */
	std::vector<std::string> ip_vec = strtovec(ip_buf);
	if (ip_vec.empty()) return;
	_debug_log_info("ip_vec.size(): %zu", ip_vec.size())

	std::vector<struct sockaddr_in> ip4_vec;
	std::vector<struct sockaddr_in6> ip6_vec;
	for (const auto &ip : ip_vec)
	{
		struct addrinfo *result, *saveTOfree;

		result = Get_addrinfo(ip, port_str,
							  AF_UNSPEC, SOCK_SEQPACKET);
		saveTOfree = result;
		do {
			if (family != result->ai_family) {
				_debug_log_info(" diacard ip: %s",
								get_addr_port_ntop_r(result->ai_addr).c_str() )
				continue;
			}
			switch (result->ai_family) {
				case AF_INET: {
					struct sockaddr_in sa{};
					memcpy(&sa, result->ai_addr, result->ai_addrlen);
					ip4_vec.push_back(sa);
				}
					break;
				case AF_INET6: {
					struct sockaddr_in6 sa6{};
					memcpy(&sa6, result->ai_addr, result->ai_addrlen);
					ip6_vec.push_back(sa6);
				}
					break;
				default: break;
			}
		} while ( (result = result->ai_next) != nullptr );
		freeaddrinfo(saveTOfree); // 释放结构体链表指向的任何动态存储空间
	}
	for (auto &ip : ip4_vec) {
		std::cout << "------------------- bind ip: "
				  << get_addr_port_ntop_r((struct sockaddr *) &ip) << std::endl;
		err_sys( sctp_bindx(sock_fd, (struct sockaddr *) &ip,
							1, flags),
				 __FILE__, __func__, __LINE__, "sctp_bindx():%s",
				 get_addr_port_ntop_r((struct sockaddr *) &ip).c_str());
	}
	for (auto &ip : ip6_vec) {
		std::cout << "------------------- bind ip: "
				  << get_addr_port_ntop_r((struct sockaddr *) &ip) << std::endl;
		err_sys( sctp_bindx(sock_fd, (struct sockaddr *) &ip,
							1, flags),
				 __FILE__, __func__, __LINE__, "sctp_bindx():%s",
				 get_addr_port_ntop_r((struct sockaddr *) &ip).c_str());
	}
}


/* SCTP 可以 bind 多个 IP地址（端口号必须相同） */
void sctp_bind_arg_list_unique(const int &sock_fd, std::string_view ip_buf,
							   std::string_view port_str, const int &flags)
{
	_debug_log_info("SCTP 可以 bind 多个 IP地址（端口号必须相同）")
	const int family = Sockfd_to_family(sock_fd);

	/* 	输入列表内容，如：111 222  333 zzz    gsfg ; 获取每一个子内容 */
	std::vector<std::string> ip_vec = strtovec(ip_buf);
	if (ip_vec.empty()) return;
	size_t max_ipv4 = ip_vec.size();
	size_t max_ipv6 = ip_vec.size();
	size_t cnt_ipv4 = 0, cnt_ipv6 = 0;
	_debug_log_info("ip_vec.size(): %zu", ip_vec.size())

	auto bind_ipv4 = std::make_unique<struct sockaddr_in[]>(max_ipv4);
	auto bind_ipv6 = std::make_unique<struct sockaddr_in6[]>(max_ipv6);
	char *ipv4p = (char *)bind_ipv4.get();
	char *ipv6p = (char *)bind_ipv6.get();
	for (const auto &ip : ip_vec)
	{
		struct addrinfo *result, *saveTOfree;

		result = Get_addrinfo(ip, port_str,
							  AF_UNSPEC, SOCK_SEQPACKET);
		saveTOfree = result;
		do {
			if (family != result->ai_family) {
				_debug_log_info(" diacard ip: %s",
								get_addr_port_ntop_r(result->ai_addr).c_str() )
				continue;
			}
			switch (result->ai_family) {
				case AF_INET:
					++cnt_ipv4;
					if (cnt_ipv4 > max_ipv4) {
						size_t numcpy = max_ipv4 * sizeof(struct sockaddr_in);
						max_ipv4 *= 2;
						ext_ptr(bind_ipv4, numcpy, max_ipv4);
						ipv4p = (char *)(bind_ipv4.get() + numcpy);
					}
					memcpy(ipv4p, result->ai_addr, result->ai_addrlen);
					ipv4p += result->ai_addrlen;
//					ipv4p += sizeof(struct sockaddr_in);
					break;
				case AF_INET6:
					++cnt_ipv6;
					if (cnt_ipv6 > max_ipv6) {
						size_t numcpy = max_ipv6 * sizeof(struct sockaddr_in6);
						max_ipv6 *= 2;
						ext_ptr(bind_ipv6, numcpy, max_ipv6);
						ipv6p = (char *)(bind_ipv6.get() + numcpy);
					}
					memcpy(ipv6p, result->ai_addr, result->ai_addrlen);
//					ipv6p += result->ai_addrlen;
					ipv6p += sizeof(struct sockaddr_in6);
					break;
				default: break;
			}
		} while ( (result = result->ai_next) != nullptr );
		freeaddrinfo(saveTOfree); // 释放结构体链表指向的任何动态存储空间
	}
	if (cnt_ipv4 != 0) {
		ipv4p = (char *)bind_ipv4.get();
		err_sys( sctp_bindx(sock_fd, (struct sockaddr *) ipv4p,
							cnt_ipv4, flags),
				 __FILE__, __func__, __LINE__, "sctp_bindx()");
	}
	if (cnt_ipv6 != 0) {
		ipv6p = (char *)bind_ipv6.get();
		err_sys( sctp_bindx(sock_fd, (struct sockaddr *) ipv6p,
							cnt_ipv6, flags),
				 __FILE__, __func__, __LINE__, "sctp_bindx()");
	}
}

/* 扩大 src 空间 */
template<typename Tp>
void ext_ptr(std::unique_ptr<Tp> &src, const size_t &numcpy,
			 const size_t &multiple)
{
	std::unique_ptr<Tp> tmp_ptr = std::make_unique<Tp>(multiple);
	mempcpy(tmp_ptr.get(), src.get(), numcpy);
	src.reset(); // Auto free resource
	src = std::move(tmp_ptr);
	tmp_ptr.reset(); // Auto free resource
}
