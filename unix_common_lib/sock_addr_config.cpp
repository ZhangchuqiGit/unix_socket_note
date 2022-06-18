//
// Created by zcq on 2021/3/30.
//

#include "sock_addr_config.h"  /* ---- sock address 配置 ---- */


/* ---- 配置 ip 地址 端口 信息 ---- */
void sock_address_config( struct sockaddr *sa,
						  const int &family	/* Address families */,
						  const int &port_net	/* 网络字节序 端口号 */,
						  const void *host_net	/* 网络字节序 IP 地址 */)
{
	switch (family)
	{
		case AF_INET: {
			auto *sock_addr = (struct sockaddr_in *) sa;
			bzero(sock_addr, sizeof(struct sockaddr_in));  // 清 0
			sock_addr->sin_family = AF_INET; // IPv4
			sock_addr->sin_port = port_net; /* 网络字节序 端口号 */
			/* Internet address */
			/** 在重叠内存块这方面，memmove() 是比 memcpy() 更安全的方法。
				如果 目标区域 和 源区域 有 重叠 的话，
				memmove() 能够保证 源串 在被覆盖之前将 重叠区域 的字节 拷贝到 目标区域中，
				复制后 源区域 的内容会被更改。
				如果 目标区域 与 源区域 没有重叠，则和 memcpy() 函数功能相同 */
			memmove(&sock_addr->sin_addr, host_net, sizeof(struct in_addr));
		}
			break;
		case AF_INET6: {
			auto *sock_addr6 = (struct sockaddr_in6 *) sa;
			bzero(sock_addr6, sizeof(struct sockaddr_in6));  // 清 0
			sock_addr6->sin6_family = AF_INET6; // IPv6
			sock_addr6->sin6_port = port_net; /* 网络字节序 端口号 */
			/* IPv6 address */
			memmove(&sock_addr6->sin6_addr, host_net, sizeof(struct in6_addr));
		}
			break;
		default:
			err_sys(-1, __LINE__, "%s()", __func__);
			break;
	}
}

/* ---- 配置 ip 地址 端口 信息 ---- */
void Sock_address_config( struct sockaddr *sa,
						  const int &family 		/* Address families */,
						  const int &portnum		/* 数字 端口号 */,
						  std::string_view host_ip	/* 字符串 IP 地址 */ )
{
	switch (family)
	{
		case AF_INET: {
#if 1
			struct in_addr sin_addr{}; /* Internet address. */
			inet_pton(AF_INET, host_ip.data(), &sin_addr); // 转为 网络字节序
#else
			/* 输出 ip 二进制网络格式 */
			struct in_addr *sin_addr = (struct in_addr *)
					addr_str_to_net(host_ip.data(), AF_INET);
#endif
			sock_address_config( sa,
								 AF_INET,
								 htons(portnum) /* 端口号：数字 转为 网络字节序 */,
								 &sin_addr /* 网络字节序 */);
		}
			break;
		case AF_INET6: {
#if 1
			struct in6_addr sin6_addr{}; /* IPv6 address. */
			inet_pton(AF_INET6, host_ip.data(), &sin6_addr); // 转为 网络字节序
#else
			/* 输出 ip 二进制网络格式 */
			struct in6_addr *sin6_addr = (struct in6_addr *)
					addr_str_to_net(host_ip.data(), AF_INET6);
		}
#endif
			sock_address_config( sa,
								 AF_INET6,
								 htons(portnum) /* 端口号：数字 转为 网络字节序 */,
								 &sin6_addr /* 网络字节序 */);
		}
			break;
		default:
			err_sys(-1, __LINE__, "%s()", __func__);
			break;
	}
}

