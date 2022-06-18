
#include "udp_service_echo.h"


void udp_service_echo(const int & socket_fd,
					  const struct sockaddr * server_address,
					  const socklen_t & sizeof_servaddr,
					  const int &seconds )
{
	std::cout << "------ udp_service_echo() : 'quit' is exit : " << std::endl;
#if UDP_Echo==0
	/* 传递互动信息 */
	udp_service_echo_msgs(socket_fd, server_address, sizeof_servaddr, seconds);
#elif UDP_Echo==1
	/* 接收标志、目的IP地址、接口/网口索引 单个读/写缓冲区 */
	udp_service_echo_flags(socket_fd, server_address, sizeof_servaddr);
#elif UDP_Echo==2
	/* recvmsg() 仅套接字描述符 分散读 可选标志 可选对面地址 可选控制信息 */
/* 	在信号处理函数中进行非局部转移
 	增加 可靠性： 	1、超时和重传：处理丢失数据；
 				 	2、序列号：验证应答是否匹配。    */
	udp_service_echo_seq_ts(socket_fd, server_address, sizeof_servaddr);
#endif
	std::cout << "------ udp_service_echo() : exit ! " << std::endl;
}


/* 传递信息 */
void udp_service_echo_msgs(const int & socket_fd,
						   const struct sockaddr * server_addr,
						   const socklen_t & sizeof_servaddr,
						   const int &seconds )
{
	socklen_t len_size = sizeof_servaddr;
	int	retval;
#ifdef Receiver_MAXBuf_mode
	char buf[Receiver_MAXBuf/* 修改 接收 缓冲区 大小 */];
#else
	char buf[MAXLINE];
#endif
	while (true) {
//		debug_line(__LINE__,(char *)__FUNCTION__);
		/* 设置超时: select() 等待描述符变为可读 */
		if (fd_read_select_timeo(socket_fd, seconds) == 0) {
			if (seconds != 0) std::cerr << "socket timeout" << std::endl;
		} else {
			memset(buf, 0 , sizeof(buf));
//			retval = recvfrom(socket_fd, buf, sizeof(buf), MSG_WAITALL,
			retval = recvfrom(socket_fd, buf, sizeof(buf), 0,
							  (struct sockaddr *)server_addr, &len_size);
			err_sys(retval, __LINE__, "recvfrom()");
			if (retval == 0) {
//				debug_line(__LINE__,(char *)__FUNCTION__);
				/** 广播点，UDP fd 不需要关闭 **/
				// close(socket_fd);
//				break;
			}
#if 1
			if (buf[retval - 1] == '\n') buf[retval - 1] = '\0';
			std::cout << "recvfrom:\t" << buf << std::endl;
#else
			std::cout << "[ "
					  << get_addr_port_ntop((struct sockaddr *)&server_addr,
											len_size)
					  << " ]: " << buf << std::endl;
#endif

			if (if_run("time", buf)) {
				memset(buf, 0, sizeof(buf));
				time_t ticks;
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
				ticks = time(nullptr);
				snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
			}

//			retval = sendto(socket_fd, buf, strlen(buf), MSG_WAITALL,
			retval = sendto(socket_fd, buf, strlen(buf), 0,
							(struct sockaddr *) server_addr, len_size);
			err_sys(retval, __LINE__, "sendto()");

			if ( if_quit_0(buf, "quit") ) {
				debug_line(__LINE__, (char *)__FUNCTION__, "广播点，UDP fd 不需要关闭");
				/** 广播点，UDP fd 不需要关闭 **/
				// close(socket_fd);
//			break;
			}
		}
	}
}


/* 接收标志、目的IP地址、接口/网口索引 单个读/写缓冲区 */
void udp_service_echo_flags(const int & sock_fd,
							const struct sockaddr * server_addr,
							const socklen_t & sizeof_servaddr)
{
	_debug_log_info("接收标志、目的IP地址、接口/网口索引")
#ifdef	IP_PKTINFO
	const int on = 1;
	err_ret(setsockopt(sock_fd, IPPROTO_IP,
					   IP_PKTINFO/*报文(UDP)相关控制信息*/, &on, sizeof(on)),
			__FILE__, __func__, __LINE__, "setsockopt()");
#endif
#ifdef	IP_RECVDSTADDR
	err_ret(setsockopt(socket_fd, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on)),
			__FILE__, __func__, __LINE__, "setsockopt()");
#endif
#ifdef	IP_RECVIF
	err_ret(setsockopt(socket_fd, IPPROTO_IP, IP_RECVIF, &on, sizeof(on)),
			__FILE__, __func__, __LINE__, "setsockopt()");
#endif
	auto *servaddr_ptr = (struct sockaddr *)server_addr;
	int flags;
	socklen_t len_addr;
	size_t	recvsendsize;
	char buf[MAXLINE], interface_name[IFNAMSIZ];
	struct in_pktinfo pktinfo{}; // Structure used for IP_PKTINFO
	struct in_addr in_zero{};
	bzero(&in_zero, sizeof(struct in_addr)); /* IPv4 address 0.0.0.0 */
	while (true)
	{
		len_addr = sizeof_servaddr;
		flags = 0;
		recvsendsize = Recvfrom_flags(sock_fd, (void *)buf, sizeof(buf), flags,
									  servaddr_ptr, len_addr, &pktinfo);
		std::cout << recvsendsize << " byte datagram from "
				  << get_addr_port_ntop_r(servaddr_ptr, len_addr);

		if (memcmp(&pktinfo.ipi_addr, &in_zero, sizeof(in_zero)) != 0) {
			std::cout << "  to "
					  << addr_net_to_ptr_r(AF_INET, &pktinfo.ipi_addr)
					  << "\nRouting destination address "
					  << addr_net_to_ptr_r(AF_INET, &pktinfo.ipi_spec_dst)
					  << std::endl;
		} else std::cout << std::endl;
		if (pktinfo.ipi_ifindex > 0 &&
			if_indextoname(pktinfo.ipi_ifindex, interface_name) != nullptr) {
			std::cout << "recv i/f = " << interface_name
					  << "\tflags = " << flags << std::endl;
		}
#ifdef	MSG_TRUNC 	/* 数据太大而截断 */
		if (flags & MSG_TRUNC) std::cout << "(datagram truncated)" << std::endl;
#endif
#ifdef	MSG_CTRUNC 	/* 控制信息被截断 */
		if (flags & MSG_CTRUNC) std::cout << "(control info truncated)" << std::endl;
#endif
#ifdef	MSG_BCAST
		if (flags & MSG_BCAST)	printf(" (broadcast)");
#endif
#ifdef	MSG_MCAST
		if (flags & MSG_MCAST)	printf(" (multicast)");
#endif
		if (if_run("time", buf)) {
			memset(buf, 0, sizeof(buf));
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
			time_t ticks = time(nullptr);
			snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
		} else std::cout << "Recvfrom_flag: " << buf << std::endl;

		err_sys(sendto(sock_fd, buf, strlen(buf), 0, servaddr_ptr, len_addr), __LINE__, "sendto()");
		memset(buf, 0, sizeof(buf));
	}
}


/* recvmsg() 仅套接字描述符 分散读 可选标志 可选对面地址 可选控制信息 */
/* 	在信号处理函数中进行非局部转移
 	增加 可靠性： 	1、超时和重传：处理丢失数据；
 				 	2、序列号：验证应答是否匹配。    */
void udp_service_echo_seq_ts(const int & sock_fd,
							 const struct sockaddr * server_addr,
							 const socklen_t & sizeof_servaddr)
{
	_debug_log_info("增加 可靠性")
#ifdef	IP_PKTINFO
	const int on = 1;
	err_ret(setsockopt(sock_fd, IPPROTO_IP,
					   IP_PKTINFO/*报文(UDP)相关控制信息*/, &on, sizeof(on)),
			__FILE__, __func__, __LINE__, "setsockopt()");
#endif
#ifdef	IP_RECVDSTADDR
	err_ret(setsockopt(socket_fd, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on)),
			__FILE__, __func__, __LINE__, "setsockopt()");
#endif
#ifdef	IP_RECVIF
	err_ret(setsockopt(socket_fd, IPPROTO_IP, IP_RECVIF, &on, sizeof(on)),
			__FILE__, __func__, __LINE__, "setsockopt()");
#endif
	auto *servaddr_ptr = (struct sockaddr *)server_addr;
	socklen_t len_addr;
	ssize_t	sizeval;
	char buf[MAXLINE], interface_name[IFNAMSIZ];
	struct in_pktinfo pktinfo{}; // Structure used for IP_PKTINFO
	struct in_addr in_zero{};
	bzero(&in_zero, sizeof(struct in_addr)); /* IPv4 address 0.0.0.0 */
	struct seq_ts hdr{}; // 序列号：验证应答
	while (true)
	{
		len_addr = sizeof_servaddr;
		memset(buf, 0, sizeof(buf));
		sizeval = Recvmsg_seq_ts(sock_fd, &hdr, sizeof(hdr), buf, sizeof(buf),
								 (struct sockaddr *) servaddr_ptr, len_addr,
								 &pktinfo);

		std::cout << "序列号: " << hdr.seq << "\n"
				  << sizeval - sizeof(struct seq_ts) << " byte datagram from "
				  << get_addr_port_ntop_r(servaddr_ptr, len_addr);
		if (memcmp(&pktinfo.ipi_addr, &in_zero, sizeof(in_zero)) != 0) {
			std::cout << "  to "
					  << addr_net_to_ptr_r(AF_INET, &pktinfo.ipi_addr)
					  << "\nRouting destination address "
					  << addr_net_to_ptr_r(AF_INET, &pktinfo.ipi_spec_dst)
					  << std::endl;
		} else std::cout << std::endl;
		if (pktinfo.ipi_ifindex > 0 &&
			if_indextoname(pktinfo.ipi_ifindex, interface_name) != nullptr) {
			std::cout << "recv i/f = " << interface_name << std::endl;
		}

		if (if_run("time", buf)) {
			memset(buf, 0, sizeof(buf));
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
			time_t ticks = time(nullptr);
			snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
		}

		Sendmsg_seq_ts(sock_fd, &hdr, sizeof(hdr), buf, strlen(buf),
					   (struct sockaddr *) servaddr_ptr, len_addr);

		if (buf[sizeval - 1] == '\n') buf[sizeval - 1] = '\0';
		std::cout << "recvfrom: " << buf << std::endl;
	}
}


