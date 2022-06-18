//
// Created by zcq on 2021/4/27.
//

#ifndef __SCTP_BIND_LIST_H
#define __SCTP_BIND_LIST_H

#include "zcq_header.h"

/* SCTP 可以 bind 多个 IP地址（端口号必须相同） */
void sctp_bind_arg_list(const int &sock_fd, std::string_view ip_buf,
						const int &port,
						const int &flags = SCTP_BINDX_ADD_ADDR);

/* SCTP 可以 bind 多个 IP地址（端口号必须相同） */
void sctp_bind_arg_list(const int &sock_fd, std::string_view ip_buf,
						std::string_view port_str,
						const int &flags = SCTP_BINDX_ADD_ADDR);

/* SCTP 可以 bind 多个 IP地址（端口号必须相同） *//**简单但不省内存**/
void sctp_bind_arg_list_efficient(const int &sock_fd, std::string_view ip_buf,
								  std::string_view port_str, const int &flags);

/* SCTP 可以 bind 多个 IP地址（端口号必须相同） *//**省内存**/
void sctp_bind_arg_list_vector(const int &sock_fd, std::string_view ip_buf,
							   std::string_view port_str, const int &flags);

/* SCTP 可以 bind 多个 IP地址（端口号必须相同） */
void sctp_bind_arg_list_unique(const int &sock_fd, std::string_view ip_buf,
							   std::string_view port_str, const int &flags);
/* 扩大 src 空间 */
template<typename Tp>
void ext_ptr(std::unique_ptr<Tp> &src, const size_t &numcpy,
			 const size_t &multiple);



#endif //__SCTP_BIND_LIST_H
