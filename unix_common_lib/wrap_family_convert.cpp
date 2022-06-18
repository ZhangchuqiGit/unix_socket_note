//
// Created by zcq on 2021/4/18.
//

#include "wrap_family_convert.h"


int Family_to_level(const int &family)
{
	int retval = family_to_level(family);
	err_sys(retval, __FILE__, __func__, __LINE__, "%s()", __func__);
	return retval;
}

int family_to_level(const int &family)
{
	switch (family) {
		case AF_INET: 	return IPPROTO_IP;
		case AF_INET6:	return IPPROTO_IPV6;
		default: errno = EAFNOSUPPORT; /* Address family not supported by protocol */
			break;
	}
	return -1;
}


int Sockfd_to_family(const int &sock_fd)
{
	int retval = sockfd_to_family(sock_fd);
	err_sys(retval, __FILE__, __func__, __LINE__, "%s()", __func__);
	return retval;
}

int sockfd_to_family(const int &sock_fd)
{
	struct sockaddr_storage ss{};
	socklen_t len = sizeof(ss);
	/* 获取 本地 与某个套接字 关联的 IP地址 和 端口号。*/
	if (getsockname(sock_fd, (struct sockaddr *) &ss, &len) < 0) return(-1);
	return(ss.ss_family);
}

