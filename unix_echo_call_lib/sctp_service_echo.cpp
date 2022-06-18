//
// Created by zcq on 2021/3/28.
//

#include "sctp_service_echo.h"


/* 传递信息 */
void sctp_service_echo(const int & sock_fd,
					   struct sockaddr *sa, const socklen_t &salen)
{
	std::cout << "------ sctp_service_echo() : 'quit' is exit : " << std::endl;
#if SCTP_Echo==0
	sctp_service_echo_msg(sock_fd, sa, salen);
#elif SCTP_Echo==1
	/* 传输“过大”消息时部分递交，其它等待该消息完整提交 */
	sctp_service_echo_part(sock_fd, sa, salen);
#elif SCTP_Echo==2

#endif
	std::cout << "------ sctp_service_echo() : exit ! " << std::endl;
}

/****************** 设置 SCTP 一对多(udp) : 无 accept() ********************/
void sctp_service_echo_msg(const int & sock_fd,
						   struct sockaddr *sa, const socklen_t &salen)
{
#if 0
	char myaddr_buf[salen];
	memcpy(myaddr_buf, sa, salen);
	auto addr_ptr = (struct sockaddr *)myaddr_buf;
#else
	auto addr_ptr = sa;
#endif
	socklen_t addr_len = salen;

	int retval; // 函数返回值
	char buf[BUFFSIZE];
	int msg_flags; // 来自客户端的消息标志
	struct sctp_sndrcvinfo sctp_SndRcv_info{}; //存放消息相关的细节信息(若设置了通知事件)
	bzero(&sctp_SndRcv_info, sizeof(sctp_SndRcv_info)); // 清 0
	while (true)
	{
		bzero(buf, sizeof(buf)); // 清 0
		addr_len = salen;

		retval = sctp_recvmsg(sock_fd, buf, sizeof(buf),
							  (struct sockaddr *)addr_ptr, &addr_len,
							  &sctp_SndRcv_info, &msg_flags);
		err_sys(retval, __LINE__, "sctp_recvmsg()");

		if (retval > 1 && buf[retval-1] == '\n') buf[retval-1] = '\0';
		std::cout << "reciver: " << buf << std::endl;

#if 1
		struct_print(sock_fd, addr_ptr, addr_len,
					 (struct sctp_sndrcvinfo *)&sctp_SndRcv_info, msg_flags);
#endif
//		++ sctp_SndRcv_info.sinfo_stream; // 所有外出消息将被发送到该流中
		if(sctp_SndRcv_info.sinfo_stream >=
		   sctp_get_no_strms(sock_fd, (struct sockaddr *)addr_ptr,
							 addr_len, sctp_SndRcv_info) ) {
			std::cerr << "流: sinfo_stream = 0" << std::endl;
			sctp_SndRcv_info.sinfo_stream = 0; // 所有外出消息将被发送到该流中
		}

//		if (if_run("quit", buf+3) ) {
//			std::cerr << "NOW : goodbye !" << std::endl;
//			std::cerr.flush();
//			sctp_SndRcv_info.sinfo_flags|=SCTP_EOF;
//		}
		if (if_run("time", buf+3)) {
			memset(buf, 0, sizeof(buf));
			time_t ticks;
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
			ticks = time(nullptr);
			snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
		}

		retval = sctp_sendmsg(sock_fd, buf, strlen(buf),
							  (struct sockaddr *)addr_ptr, addr_len,
							  sctp_SndRcv_info.sinfo_ppid,
							  sctp_SndRcv_info.sinfo_flags,
							  sctp_SndRcv_info.sinfo_stream,
							  0, 0);
		err_sys(retval, __LINE__, "sctp_sendmsg()");
	}
}


/* 传输“过大”消息时部分递交，其它等待该消息完整提交 */
void sctp_service_echo_part(const int &sock_fd,
							struct sockaddr *sa, const socklen_t &salen)
{
#if 0
	char myaddr_buf[salen];
	memcpy(myaddr_buf, sa, salen);
	auto addr_ptr = (struct sockaddr *)myaddr_buf;
#else
	auto addr_ptr = sa;
#endif
	socklen_t addr_len = salen;

	int retval;
	int msg_flags; // 来自客户端的消息标志
	struct sctp_sndrcvinfo sctp_SndRcv_info{}; //存放消息相关的细节信息(若设置了通知事件)
	bzero(&sctp_SndRcv_info, sizeof(sctp_SndRcv_info)); // 清 0
	char buf[128];
	std::string buf_str;
	while (true)
	{
		addr_len = salen;

		/* 传输“过大”消息时部分递交，其它等待该消息完整提交 */
		retval = pdapi_recvmsg(sock_fd, buf_str, addr_ptr, &addr_len,
							   &sctp_SndRcv_info, &msg_flags);

		if (retval > 1) {
			retval = buf_str.size();
			if(buf_str[retval-1] == '\n') buf_str[retval-1] = '\0';
		}
		std::cout << "From stream: " << sctp_SndRcv_info.sinfo_stream
				  << " reciver "<< retval << " sizes: " << buf_str << std::endl;

#if 0
		struct_print(sock_fd, addr_ptr, addr_len,
					 (struct sctp_sndrcvinfo *)&sctp_SndRcv_info, msg_flags);
//		++ sctp_SndRcv_info.sinfo_stream; // 所有外出消息将被发送到该流中
		if(sctp_SndRcv_info.sinfo_stream >=
		   sctp_get_no_strms(sock_fd, (struct sockaddr *)addr_ptr,
							 addr_len, sctp_SndRcv_info) ) {
			std::cerr << "流: sinfo_stream = 0" << std::endl;
			sctp_SndRcv_info.sinfo_stream = 0; // 所有外出消息将被发送到该流中
		}
#endif
		if (buf_str.size() > 3) {
			if (if_run("time", buf_str.substr(3))) {
				memset(buf, 0, sizeof(buf));
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
				time_t ticks = time(nullptr);
				snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
				buf_str.clear();
				buf_str = buf;
			}
		}
		if (if_run("time", buf_str ) ) {
			memset(buf, 0, sizeof(buf));
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
			time_t ticks = time(nullptr);
			snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
			buf_str.clear();
			buf_str = buf;
		}

		retval = sctp_sendmsg(sock_fd, buf_str.c_str(), buf_str.size(),
							  (struct sockaddr *) addr_ptr, addr_len,
							  sctp_SndRcv_info.sinfo_ppid,
							  sctp_SndRcv_info.sinfo_flags,
							  sctp_SndRcv_info.sinfo_stream,
							  0, 0);
		err_sys(retval, __LINE__, "sctp_sendmsg()");
	}
}


/* 设置 所有 感兴趣 SCTP 通知事件 */
void sctp_service_echo_event(const int & sock_fd,
							 struct sockaddr *sa, const socklen_t &salen)
{
	std::cout << "------ sctp_service_echo() : 'quit' is exit : " << std::endl;
#if 0
	char myaddr_buf[salen];
	memcpy(myaddr_buf, sa, salen);
	auto addr_ptr = (struct sockaddr *)myaddr_buf;
#else
	auto addr_ptr = sa;
#endif
	socklen_t addr_len = salen;

	int retval;
	int msg_flags; // 来自客户端的消息标志
	struct sctp_sndrcvinfo sctp_SndRcv_info{}; //存放消息相关的细节信息(若设置了通知事件)
	bzero(&sctp_SndRcv_info, sizeof(sctp_SndRcv_info)); // 清 0
	char buf[128];
	std::string buf_str;
	while (true)
	{
		addr_len = salen;
		do {
			/** 设置的通知事件存在 接收区 buf_str 的头部区域 **/
			/* 传输“过大”消息时部分递交，其它等待该消息完整提交 */
			retval = pdapi_recvmsg(sock_fd, buf_str, addr_ptr, &addr_len,
								   &sctp_SndRcv_info, &msg_flags);
			if (msg_flags & MSG_NOTIFICATION)
				print_notification_event(buf_str.data());
		} while (msg_flags & MSG_NOTIFICATION);

		if (retval > 1) {
			retval = buf_str.size();
			if(buf_str[retval-1] == '\n') buf_str[retval-1] = '\0';
		}
		std::cout << "reciver "<< retval << " sizes: " << buf_str << std::endl;

#if 0
		struct_print(sock_fd, addr_ptr, addr_len,
					 (struct sctp_sndrcvinfo *)&sctp_SndRcv_info, msg_flags);
#endif
#if 0
		++ sctp_SndRcv_info.sinfo_stream; // 所有外出消息将被发送到该流中
		if(sctp_SndRcv_info.sinfo_stream >=
		   sctp_get_no_strms(sock_fd, (struct sockaddr *)addr_ptr,
							 addr_len, sctp_SndRcv_info) ) {
			std::cerr << "流: sinfo_stream = 0" << std::endl;
			sctp_SndRcv_info.sinfo_stream = 0; // 所有外出消息将被发送到该流中
		}
#endif
		if (buf_str.size() > 3) {
			if (if_run("time", buf_str.substr(3))) {
				memset(buf, 0, sizeof(buf));
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
				time_t ticks = time(nullptr);
				snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
				buf_str.clear();
				buf_str = buf;
			}
		}
		if (if_run("time", buf_str ) ) {
			memset(buf, 0, sizeof(buf));
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
			time_t ticks = time(nullptr);
			snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
			buf_str.clear();
			buf_str = buf;
		}

		retval = sctp_sendmsg(sock_fd, buf_str.c_str(), buf_str.size(),
							  (struct sockaddr *) addr_ptr, addr_len,
							  sctp_SndRcv_info.sinfo_ppid,
							  sctp_SndRcv_info.sinfo_flags,
							  sctp_SndRcv_info.sinfo_stream,
							  0, 0);
		err_sys(retval, __LINE__, "sctp_sendmsg()");
	}
	std::cout << "------ sctp_service_echo() : exit ! " << std::endl;
}


/* 一到多 转 一对一 的迸发SCTP服务；需用 sctp_peelof() 从sctp服务分离并获取一对一套接字 */
void sctp_service_echo_fork(const int & sock_fd,
							struct sockaddr *sa, const socklen_t &salen)
{
	std::cout << "------ sctp_service_echo_fork() : 'quit' is exit : " << std::endl;
#if 0
	char myaddr_buf[salen];
	memcpy(myaddr_buf, sa, salen);
	auto addr_ptr = (struct sockaddr *)myaddr_buf;
#else
	auto addr_ptr = sa;
#endif
	socklen_t addr_len = salen;

	int retval;
	sctp_assoc_t assoc_id;
	int msg_flags; // 来自客户端的消息标志
	struct sctp_sndrcvinfo sctp_SndRcv_info{}; //存放消息相关的细节信息(若设置了通知事件)
	bzero(&sctp_SndRcv_info, sizeof(sctp_SndRcv_info)); // 清 0
	char buf[128];
	std::string buf_str;
	while (true)
	{
		addr_len = salen;

		/** 设置的通知事件存在 接收区 buf_str 的头部区域 **/
		/* 传输“过大”消息时部分递交，其它等待该消息完整提交 */
		retval = pdapi_recvmsg(sock_fd, buf_str, addr_ptr, &addr_len,
							   &sctp_SndRcv_info, &msg_flags);

		if (retval > 1) {
			retval = buf_str.size();
			if(buf_str[retval-1] == '\n') buf_str[retval-1] = '\0';
		}
		std::cout << "reciver "<< retval << " sizes: " << buf_str << std::endl;

#if 0
		struct_print(sock_fd, addr_ptr, addr_len,
					 (struct sctp_sndrcvinfo *)&sctp_SndRcv_info, msg_flags);
#endif
#if 0
		++ sctp_SndRcv_info.sinfo_stream; // 所有外出消息将被发送到该流中
		if(sctp_SndRcv_info.sinfo_stream >=
		   sctp_get_no_strms(sock_fd, (struct sockaddr *)addr_ptr,
							 addr_len, sctp_SndRcv_info) ) {
			std::cerr << "流: sinfo_stream = 0" << std::endl;
			sctp_SndRcv_info.sinfo_stream = 0; // 所有外出消息将被发送到该流中
		}
#endif
		if (buf_str.size() > 3) {
			if (if_run("time", buf_str.substr(3))) {
				memset(buf, 0, sizeof(buf));
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
				time_t ticks = time(nullptr);
				snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
				buf_str.clear();
				buf_str = buf;
			}
		}
		if (if_run("time", buf_str ) ) {
			memset(buf, 0, sizeof(buf));
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
			time_t ticks = time(nullptr);
			snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
			buf_str.clear();
			buf_str = buf;
		}

		retval = sctp_sendmsg(sock_fd, buf_str.c_str(), buf_str.size(),
							  (struct sockaddr *) addr_ptr, addr_len,
							  sctp_SndRcv_info.sinfo_ppid,
							  sctp_SndRcv_info.sinfo_flags,
							  sctp_SndRcv_info.sinfo_stream,
							  0, 0);
		err_sys(retval, __LINE__, "sctp_sendmsg()");

		/* sctp IP地址 转换 关联ID信息 */
		assoc_id = sctp_IPaddr_to_assocID(sock_fd,
										  addr_ptr, addr_len,
										  sctp_SndRcv_info.sinfo_assoc_id);
		if(assoc_id == 0) err_ret(-1, __LINE__,
								  "Can't get association id");
		else {
			int sctp_fd = sctp_peeloff(sock_fd, assoc_id);//从sctp服务分离并获取一对一套接字
			err_ret(sctp_fd, __LINE__, "sctp_peeloff()");
			if((retval = fork()) == 0) { // 创建 子进程
				err_sys(close(sock_fd), __LINE__, "close()");
				/* 传输“过大”消息时部分递交，其它等待该消息完整提交 */
				sctp_service_echo_part(sctp_fd, addr_ptr, addr_len);//一对一
				exit(0);
			} else {
				err_sys(close(sctp_fd), __LINE__, "close()");
				std::cout << "创建 子进程 ID: " << retval << std::endl;
			}
		}
	}
	std::cout << "------ sctp_service_echo_fork() : exit ! " << std::endl;
}


