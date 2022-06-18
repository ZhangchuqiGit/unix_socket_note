//
// Created by zcq on 2021/4/25.
//

#ifndef __WRAP_SOCKOPT_H
#define __WRAP_SOCKOPT_H

#include "zcq_header.h"

void Setsockopt(const int &sock_fd, const int &level, const int &optname,
				const void *optval, const socklen_t &optlen);

void Setsockopt(const int &sock_fd, const int &level,
				const std::initializer_list<int > &optname,
				const void *optval, const socklen_t &optlen);

void Getsockopt(const int &sock_fd, const int &level, const int &optname,
				void *optval, socklen_t *optlen);

void Getsockopt(const int &sock_fd, const int &level,
				const std::initializer_list<int > &optname,
				void *optval, socklen_t *optlen);

int Sockatmark(const int &sock_fd); // 是否处于带外标记


#endif //__WRAP_SOCKOPT_H
