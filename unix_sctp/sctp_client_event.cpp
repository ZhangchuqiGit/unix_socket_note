
#include "zcq_header.h"

void sctp_client_event()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int sock_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_SEQPACKET 固定最大长度的有序，可靠，基于连接的数据报。*/
	sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
	err_sys(sock_fd, __LINE__, "socket()");

#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	const int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   {SO_RCVBUF/* 接收缓冲区 */},
			   &receiver_buf, sizeof(receiver_buf));
#endif
/**************************** 设置 SCTP 通知事件 ***************************/
	/* 设置 SCTP 通知事件 */
	struct sctp_event_subscribe evnts{}; // 感兴趣事件集
	bzero(&evnts, sizeof(evnts)); // 清 0
	evnts.sctp_data_io_event = 1; //设置 I/O 通知事件 才能调用sctp_recvmsg()接收对端DATA
	evnts.sctp_association_event = 1; // SCTP关联事件

	Setsockopt(sock_fd, IPPROTO_SCTP/* SCTP 选项 */, SCTP_EVENTS/*事件*/,
			   &evnts, sizeof(evnts));
/*************************************************************************/

	/* ---- 配置 ip 地址 端口 信息 ---- */
	struct sockaddr_in servaddr{};
	bzero(&servaddr, sizeof(servaddr)); // 清 0
	servaddr.sin_family	=AF_INET; // IPv4
	servaddr.sin_port	=htons(PORT_service); // 端口号
#if 1
	servaddr.sin_addr.s_addr = inet_addr(HOST_Service_IPv4); // Internet address
#else	// Internet address
	retval=inet_pton(AF_INET, HOST_Service_IPv4, &servaddr.sin_addr);
	err_sys(retval, __LINE__, "inet_pton(): %s", HOST_Service_IPv4);
	if (retval == 0)	/* not set */
		err_quit(retval, __LINE__, "inet_pton not set %s", HOST_Service_IPv4);
#endif

	/* 设置 感兴趣 SCTP 通知事件 *//** 获取 所有 对方地址 本地地址 **/
	sctp_client_call_event(sock_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	/* ---- close () ---- */
	retval=close(sock_fd);
	err_sys(retval, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}

