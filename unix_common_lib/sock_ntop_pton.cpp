//
// Created by zcq on 2021/3/30.
//

#include "sock_ntop_pton.h"


/* 输出 ip 二进制网络字节序格式 */
char * addr_str_to_net( std::string_view hostaddr/* 字符串 IP 地址 */,
						const int &family )
{
	switch (family)
	{
		case AF_INET: {
			static struct in_addr IPv4_addr{}; /* Internet address. */
			inet_pton(AF_INET, hostaddr.data(), &IPv4_addr); // 转为 网络字节序
			return (char *) &IPv4_addr;
		}
		case AF_INET6: {
			static struct in6_addr IPv6_addr{}; /* IPv6 address. */
			inet_pton(AF_INET6, hostaddr.data(), &IPv6_addr); // 转为 网络字节序
			return (char *) &IPv6_addr;
		}
		default:
			break;
	}
	return nullptr;
}

/* 输出 ip 二进制网络字节序格式 */
std::string addr_str_to_net_r( std::string_view hostaddr/* 字符串 IP 地址 */,
							   const int &family )
{
	switch (family)
	{
		case AF_INET: {
			char buf[sizeof(in_addr)+1] = {'\0'};
			inet_pton(AF_INET, hostaddr.data(), buf); // 转为 网络字节序
			return buf;
		}
		case AF_INET6: {
			char buf[sizeof(in6_addr)+1] = {'\0'};
			inet_pton(AF_INET6, hostaddr.data(), buf); // 转为 网络字节序
			return buf;
		}
		default:
			break;
	}
	return "nullptr";
}

/* 输出 ip 字符串 如 192.168.6.7 */
std::string Addr_net_to_ptr_r(const struct sockaddr *sa_ptr)
{
	return Addr_net_to_ptr_r(sa_ptr->sa_family, sa_ptr);
}

/* 输出 ip 字符串 如 192.168.6.7 */
std::string Addr_net_to_ptr_r(const int &family, const struct sockaddr *sa_ptr)
{
	switch (family) {
		case AF_INET:
			return addr_net_to_ptr_r(AF_INET,
							&((struct sockaddr_in *)sa_ptr)->sin_addr );
		case AF_INET6:
			return addr_net_to_ptr_r(AF_INET6,
							&((struct sockaddr_in6 *)sa_ptr)->sin6_addr );
		case AF_UNIX: {
			auto *unp = (struct sockaddr_un *)sa_ptr;
			/*	确定没有路径名绑定到套接字：
				在每个connect（）上发生，除非客户端首先调用bind（）。*/
			if (unp->sun_path[0] == 0)
				return std::string("(no pathname bound)");
			return std::string(unp->sun_path);
		}
		default:
			break;
	}
	return std::string("nullptr");
}

/* 输出 ip 字符串 如 192.168.6.7 */
std::string addr_net_to_ptr_r(const int &family,
							  const void *in4_6addr, const size_t &len)
{
	size_t xlen = len;
	if (xlen == 0)
		switch (family) {
//			case AF_INET: xlen = INET_ADDRSTRLEN; break;
			case AF_INET: xlen =  sizeof(struct sockaddr_in); break;
//			case AF_INET6: xlen = INET6_ADDRSTRLEN; break;
			case AF_INET6: xlen = sizeof(struct sockaddr_in6); break;
			default: return std::string("nullptr");
		}
	char buf[xlen];	/* Unix domain is largest */
	bzero(buf, sizeof(buf));
	if (inet_ntop(family, in4_6addr, buf, xlen) == nullptr)
		return std::string("nullptr");
	return std::string(buf);
}

/* 输出 ip 字符串 如 192.168.6.7 */
std::string addr_net_to_ptr(const struct sockaddr *sa_ptr)
{
	std::string buf_str;
	switch (sa_ptr->sa_family) {
		case AF_INET: {
			char buf[INET_ADDRSTRLEN] = {'\0'};
			/* Internet socket address. */
			auto *sa_in_ptr = (struct sockaddr_in *)sa_ptr;
			if (inet_ntop(AF_INET, &sa_in_ptr->sin_addr, buf, INET_ADDRSTRLEN)
				== nullptr) return ("nullptr");
			buf_str = buf;
			return buf_str;
		}
		case AF_INET6: {
			char buf[INET6_ADDRSTRLEN] = {'\0'};
			/* IPv6 socket address. */
			auto *sa_in6_ptr = (struct sockaddr_in6 *)sa_ptr;
			if (inet_ntop(AF_INET6, &sa_in6_ptr->sin6_addr, buf, INET6_ADDRSTRLEN)
				== nullptr) return("nullptr");
			buf_str = buf;
			return buf_str;
		}
		case AF_UNIX: {
			char buf[_SS_SIZE] = {'\0'};	/* Unix domain is largest */
			auto *unp = (struct sockaddr_un *)sa_ptr;
			/*	确定没有路径名绑定到套接字：
				在每个connect（）上发生，除非客户端首先调用bind（）。*/
			if (unp->sun_path[0] == 0) strcpy(buf, "(no pathname bound)");
			else snprintf(buf, sizeof(buf), "%s", unp->sun_path);
			buf_str = buf;
			return buf_str;
		}
		default:
			break;
	}
	return "nullptr";
}

/* 输出 ip 字符串 如 192.168.6.7 */
std::string Addr_net_to_ptr(const struct sockaddr *sa_ptr)
{
	std::string str = addr_net_to_ptr(sa_ptr);
	if (str.empty()) return "nullptr";
	return str;
}


/* 输出 ip + port 字符串 如 192.168.6.7:1080,[1:...:1]:12345 */
std::string get_addr_port_ntop_r(const struct sockaddr *sa_ptr,
								 const socklen_t &len)
{
	std::string buf_str;
	switch (sa_ptr->sa_family)
	{
		case AF_INET: {
			auto *sa_in_ptr = (struct sockaddr_in *)sa_ptr;
			buf_str = addr_net_to_ptr_r(AF_INET, &sa_in_ptr->sin_addr, len);
			if (buf_str == "nullptr") return "nullptr";
			/* ntohs() 函数将一个16位数由 网络字节顺序 转换为 主机字节顺序 数值。 */
			if ( ntohs(sa_in_ptr->sin_port) != 0) {
				buf_str += ":";
				buf_str += std::to_string(ntohs(sa_in_ptr->sin_port));
			}
			return buf_str;
		}
		case AF_INET6: {
			auto *sa_in6_ptr = (struct sockaddr_in6 *)sa_ptr;
			buf_str = addr_net_to_ptr_r(AF_INET6, &sa_in6_ptr->sin6_addr, len);
			if (buf_str == "nullptr") return "nullptr";
			/* ntohs() 函数将一个16位数由 网络字节顺序 转换为 主机字节顺序 数值。 */
			if ( ntohs(sa_in6_ptr->sin6_port) != 0) {
				buf_str.insert(0, "[");
				buf_str += "]:";
				buf_str += std::to_string(ntohs(sa_in6_ptr->sin6_port));
				return buf_str;
			}
			return buf_str;
		}
		case AF_UNIX: {
			return Addr_net_to_ptr_r(AF_UNIX, sa_ptr);
		}
		default:
			buf_str = "sock_ntop: unknown AF_xxx: " +
					  std::to_string(sa_ptr->sa_family) +
					  ",  len " + std::to_string(len);
			return buf_str;
	}
}


/* 输出 ip + port 字符串 如 192.168.6.7:1080,[1:...:1]:12345 */
std::string get_addr_port_ntop(const struct sockaddr *sa_ptr, const socklen_t &len)
{
	char portnum[8] = {'\0'};
//	bzero(portnum, sizeof(portnum)); // 清 0
	char buf[_SS_SIZE] = {'\0'};       /* Unix domain is largest */
//	bzero(buf, sizeof(buf)); // 清 0

	switch (sa_ptr->sa_family)
	{
		case AF_INET: {
			std::string buf_str = Addr_net_to_ptr(sa_ptr);
			if (buf_str.empty()) return "nullptr";
			/** 在重叠内存块这方面，memmove() 是比 memcpy() 更安全的方法。
				如果 目标区域 和 源区域 有 重叠 的话，
				memmove() 能够保证 源串 在被覆盖之前将 重叠区域 的字节 拷贝到 目标区域中，
				复制后 源区域 的内容会被更改。
				如果 目标区域 与 源区域 没有重叠，则和 memcpy() 函数功能相同 */
			if (memmove(buf, buf_str.c_str(), buf_str.size()) == nullptr )
				return "nullptr";
			auto *sa_in_ptr = (struct sockaddr_in *)sa_ptr;
			/* ntohs() 函数将一个16位数由 网络字节顺序 转换为 主机字节顺序 数值。 */
			if ( ntohs(sa_in_ptr->sin_port ) != 0) {
				snprintf(portnum, sizeof(portnum), ":%d",
						 ntohs(sa_in_ptr->sin_port));
				strcat(buf, portnum);
			}
			buf_str = buf;
			return buf_str;
		}
		case AF_INET6: {
			std::string buf_str = Addr_net_to_ptr(sa_ptr);
			if (buf_str.empty()) return "nullptr";
			buf[0] = '[';
			if (memmove(buf + 1, buf_str.c_str(), buf_str.size()) == nullptr )
				return "nullptr";
			auto *sa_in6_ptr = (struct sockaddr_in6 *)sa_ptr;
			/* ntohs() 函数将一个16位数由 网络字节顺序 转换为 主机字节顺序 数值。 */
			if ( ntohs(sa_in6_ptr->sin6_port ) != 0) {
				snprintf(portnum, sizeof(portnum), "]:%d",
						 ntohs(sa_in6_ptr->sin6_port));
				strcat(buf, portnum);
				buf_str = buf;
				return buf_str;
			}
			buf_str = buf+1;
			return buf_str;
		}
		case AF_UNIX: {
			std::string buf_str = addr_net_to_ptr(sa_ptr);
			return buf_str;
		}
		default:
			snprintf(buf, sizeof(buf),
					 "sock_ntop: unknown AF_xxx: %d, len %u",
					 sa_ptr->sa_family, len);
			std::string buf_str = buf;
			return buf_str;
	}
}

