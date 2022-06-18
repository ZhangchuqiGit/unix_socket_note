
#include "zcq_header.h"

/* 一到多 转 一对一 的迸发SCTP服务；需用 sctp_peelof() 从sctp服务分离并获取一对一套接字 */
void sctp_service_fork()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int retval; // 函数返回值
	int sock_fd; // 套接字描述符
	const int option = 1;

	/* ---- socket () ---- SOCK_SEQPACKET 固定最大长度的有序，可靠，基于连接的数据报。*/
	sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
	err_sys(sock_fd, __LINE__, "socket()");

	/* ---- 允许地址的立即重用，端口不占用 ---- */
	/* ---- 允许端口的立即重用 ---- */
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   {SO_REUSEADDR/*地址立即重用*/, SO_REUSEPORT/*端口立即重用*/},
			   &option, sizeof(option));

#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	const int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	Setsockopt(sock_fd, SOL_SOCKET/* 通用选项 */,
			   {SO_RCVBUF/* 接收缓冲区 */},
			   &receiver_buf, sizeof(receiver_buf));
#endif
/********************** 设置 自动关闭 指定空闲时间的 客户端 ***********************/
	const int close_time = 60; // 指定客户端的空闲时间
	Setsockopt(sock_fd, IPPROTO_SCTP/* SCTP 选项 */,
			   SCTP_AUTOCLOSE/* 自动关闭 */, &close_time, sizeof(close_time));

/************************ 设置每个客户端最大 SCTP 流 数目 ************************/
#if 1
	struct sctp_initmsg initm{}; // SCTP 初始化
	bzero(&initm, sizeof(initm)); // 清 0
	initm.sinit_num_ostreams = SERV_SCTP_stream_max; // 设置 SCTP 流 (客户端)数目
	Setsockopt(sock_fd, IPPROTO_SCTP/* SCTP 选项 */,
			   SCTP_INITMSG/* SCTP_INIT */, &initm, sizeof(initm));
#endif
/*********************** 设置 感兴趣 SCTP 通知事件 **************************/
	/* 设置感兴趣 SCTP 通知事件 */
	struct sctp_event_subscribe evnts{}; // 感兴趣事件集
	bzero(&evnts, sizeof(evnts)); // 清 0
	evnts.sctp_data_io_event = 1; //设置 I/O 通知事件 才能调用sctp_recvmsg()接收对端DATA
	Setsockopt(sock_fd, IPPROTO_SCTP/* SCTP 选项 */, SCTP_EVENTS/*事件*/,
			   &evnts, sizeof(evnts));
/******************************************************************************/
	/* ---- 配置 ip 地址 端口 信息 ---- */
	struct sockaddr_in servaddr{};
	bzero(&servaddr, sizeof(servaddr)); // 清 0
#if 1
	servaddr.sin_family		=AF_INET; // IPv4
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY); // Internet address
	servaddr.sin_port		=htons(PORT_service); // 端口号
	retval=bind(sock_fd, (struct sockaddr *)(&servaddr), sizeof(servaddr) );
	err_sys(retval, __LINE__, "bind()");
#else
	/**** 不管是client端，还是server端，SCTP都可以 bind多个IP地址（端口号必须相同）。****/
	sctp_bind_arg_list(sock_fd,
					   (std::string) HOST_Service_IPv6 + " " +
					   HOST_Service_IPv4 + " 127.0.0.1",
					   PORT_service);
#endif
/******************************************************************************/
	/* ---- listen () ---- */
	retval=listen(sock_fd, MAX_listen/*最大客户端连接数*/ );
	err_sys(retval, __LINE__, "listen()");

/****************** 设置 SCTP 一对多(udp) : 无 accept() ********************/
/* 一到多 转 一对一 的迸发SCTP服务；需用 sctp_peelof() 从sctp服务分离并获取一对一套接字 */
	sctp_service_echo_fork(sock_fd,
						   (struct sockaddr *) &servaddr,
						   sizeof(servaddr));

	/* ---- close () ---- */
	err_sys(close(sock_fd), __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}

