//
// Created by zcq on 2021/3/28.
//

#include "sctp_client_call.h"

void sctp_client_call(const int &sock_fd,
					  struct sockaddr *to_addr, const socklen_t &to_len)
{
	std::cout << "------ sctp_client_call() : 'quit' is exit : " << std::endl;
#if SCTP_Call==0
	char num = '0';
	while (true) {
		std::cout << "NO 1 : Echoing messages to a stream" << std::endl;
		std::cout << "NO 2 : Echoing messages to all streams" << std::endl;
		std::cin.ignore(1);
		std::cin.get(num);
		if ( num == '1' || num == '2' ) break;
		else {
			if ( num == '\r' || num == '\n' ) {
				std::cin.clear();
			}
			else {
				std::cerr << "error input : " << num << std::endl;
				num = '0';
			}
		}
	}
	/* Echoing messages to a stream */
	if ( num == '1' ) {
		std::cout << "------ sctp_client_call_msg() : 'quit' is exit : " << std::endl;
		sctp_client_call_msg(stdin, sock_fd, to_addr, to_len);
		std::cout << "------ sctp_client_call_msg() : exit ! " << std::endl;
	}
		/* Echoing messages to all streams */
	else if ( num == '2' ) {
		std::cout << "------ sctp_client_call_msgs() : 'quit' is exit : " << std::endl;
		sctp_client_call_msgs(stdin, sock_fd, to_addr, to_len);
		std::cout << "------ sctp_client_call_msgs() : exit ! " << std::endl;
	}
#elif SCTP_Call==1

#endif
	std::cout << "------ sctp_client_call() : exit ! " << std::endl;
}


/* Echoing messages to a stream */
void sctp_client_call_msg(FILE *fp, const int &sock_fd,
						  struct sockaddr *to_addr, const socklen_t &to_len)
{
	int ret_value; // 函数返回值
	int msg_flags; // 来自客户端的消息标志
	struct sockaddr_in peer_addr{}; // 对端 ip 地址 端口 信息
	socklen_t peer_addr_sizeof = to_len;
	struct sctp_sndrcvinfo sctp_SndRcv_info{}; //存放消息相关的细节信息(若设置了通知事件)
	bzero(&sctp_SndRcv_info,sizeof(sctp_SndRcv_info)); // 清 0
	char sendline[BUFFSIZE], recvline[BUFFSIZE];
	while (true)
	{
		bzero(sendline, sizeof(sendline));
		bzero(recvline, sizeof(recvline));
		std::cout << "Send form: [StreamNum]SendText; your input: ";

		ret_value = get_Line(fp, sendline, sizeof(sendline));
		err_sys(ret_value, __LINE__, "get_Line()");
		if (ret_value > 1 && sendline[ret_value-1] == '\n' )
			sendline[ret_value-1] = '\0';

		if ( sctp_quit( "quit", sendline, sock_fd, to_addr) ) break;
		else if(sendline[0] != '[') {
			std::cerr << "Error send form !" << std::endl;
			continue;
		}

		sctp_SndRcv_info.sinfo_stream = strtol(&sendline[1],
											   nullptr, 10);
		ret_value = sctp_sendmsg(sock_fd, sendline, strlen(sendline),
								 to_addr, to_len, 0, 0,
								 sctp_SndRcv_info.sinfo_stream,
								 0, 0);
		err_sys(ret_value, __LINE__, "sctp_sendmsg()");

		if (ret_value > 1 && sendline[ret_value-1] == '\n' )
			sendline[ret_value-1] = '\0';
		std::cout << "sendline: " << sendline << std::endl;

		ret_value = sctp_recvmsg(sock_fd, recvline, sizeof(recvline),
								 (struct sockaddr *)&peer_addr,
								 &peer_addr_sizeof,
								 &sctp_SndRcv_info, &msg_flags);
		err_sys(ret_value, __LINE__, "sctp_recvmsg()");

		if (ret_value > 1 && recvline[ret_value-1] == '\n' )
			recvline[ret_value-1] = '\0';
		std::cout << "recvline: " << recvline << std::endl;
#if 0
		struct_print(sock_fd, (struct sockaddr *)&peer_addr,
					 peer_addr_sizeof,
					 (struct sctp_sndrcvinfo *)&sctp_SndRcv_info, msg_flags);
#endif
	}
}


/* Echoing messages to all streams */
void sctp_client_call_msgs(FILE *fp, const int &sock_fd,
						   struct sockaddr *to_addr, const socklen_t &to_len)
{
	int ret_value; // 函数返回值
	int msg_flags; // 来自客户端的消息标志
	struct sockaddr_in peer_addr{}; // 对端 ip 地址 端口 信息
	socklen_t peer_addr_sizeof = sizeof(peer_addr);
	struct sctp_sndrcvinfo sctp_SndRcv_info{}; //存放消息相关的细节信息(若设置了通知事件)
//	bzero(&sctp_SndRcv_info,sizeof(sctp_SndRcv_info)); // 清 0
	char sendline[BUFFSIZE] = "Msg 00:", recvline[BUFFSIZE];
	int8_t send_begin_len = std::string("Msg 00:").size();
	while (true)
	{
		std::cout << "your input: ";

		ret_value = get_Line(fp, sendline + send_begin_len,
							 sizeof(sendline) - send_begin_len );
		err_sys(ret_value, __LINE__, "get_Line()");

		if (ret_value > 1 && sendline[ret_value-1] == '\n' )
			sendline[ret_value-1] = '\0';
		if ( sctp_quit( "quit", sendline + send_begin_len,
						sock_fd, to_addr) ) break;
		else {
#define SCTP_UNORDERED_test /*发送/接收消息无序*/
#ifdef SCTP_UNORDERED_test
			sctp_SndRcv_info.sinfo_stream = 0;
			for (int16_t i = 0; i < 64; ++i) {
#else
				for (int16_t i = 0; i < SERV_SCTP_stream_max; ++i) {
				sctp_SndRcv_info.sinfo_stream = i;
#endif
				sendline[4] = char(i / 10 + '0');
				sendline[5] = char(i % 10 + '0');
				ret_value = sctp_sendmsg(sock_fd, sendline,
										 strlen(sendline),
										 to_addr, to_len, 0,
#ifdef SCTP_UNORDERED_test
										 SCTP_UNORDERED/*发送/接收消息无序*/,
#else
						0,
#endif
										 sctp_SndRcv_info.sinfo_stream,
										 0, 0);
				err_sys(ret_value, __LINE__, "sctp_sendmsg()");
			}
		}

#ifdef SCTP_UNORDERED_test
		for (int16_t i = 0; i < 64; ++i) {
#else
			for(int16_t i=0; i < SERV_SCTP_stream_max; ++i) {
#endif
			bzero(recvline, sizeof(recvline)); // 清 0
			ret_value = sctp_recvmsg(sock_fd, recvline, sizeof(recvline),
									 (struct sockaddr *) &peer_addr,
									 &peer_addr_sizeof,
									 &sctp_SndRcv_info, &msg_flags);
			err_sys(ret_value, __LINE__, "sctp_recvmsg()");
			std::cout << "From stream: " << sctp_SndRcv_info.sinfo_stream
					  << " ssn: " << sctp_SndRcv_info.sinfo_ssn
					  << " ssoc_id: "
					  << sctp_SndRcv_info.sinfo_assoc_id
					  << " Data: " << recvline << std::endl;
		}
	}
}


/* 设置 感兴趣 SCTP 通知事件 *//** 获取 所有 对方地址 本地地址 **/
void sctp_client_call_event(const int &sock_fd,
							struct sockaddr *to_addr, const socklen_t &to_len) {
	int retval; // 函数返回值
	int flags; // 来自客户端的消息标志
	struct sockaddr_in peer_addr{}; // 对端 ip 地址 端口 信息
	socklen_t peer_len = to_len;
	struct sctp_sndrcvinfo sctp_Rcv_info{}; //存放消息相关的细节信息(若设置了通知事件)
	bzero(&sctp_Rcv_info, sizeof(sctp_Rcv_info));
	char sendline[BUFFSIZE];
	std::string buf_str;
	int sinfo_stream = 0;
	while (true) {
		std::cout << "input: ";

		retval = Read(STDIN_FILENO, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "Read()");
		if (retval > 1 && sendline[retval - 1] == '\n')
			sendline[retval - 1] = '\0';
		if (sctp_quit("quit", sendline, sock_fd, to_addr)) break;

		retval = sctp_sendmsg(sock_fd, sendline, strlen(sendline),
							  to_addr, to_len, 0, 0,
							  sinfo_stream,
							  0, 0);
		err_sys(retval, __LINE__, "sctp_sendmsg()");

		std::cout << "stream[" << sinfo_stream
				  << "]  size[" << retval
				  << "]  Data: " << sendline << std::endl;
		if (++sinfo_stream >= SERV_SCTP_stream_max) sinfo_stream = 0;

		do {
			peer_len = to_len;
			/** 设置的通知事件存在 接收区 buf_str 的头部区域 **/
			/* 传输“过大”消息时部分递交，其它等待该消息完整提交 */
			retval = pdapi_recvmsg(sock_fd, buf_str,
								   (struct sockaddr *) &peer_addr, &peer_len,
								   &sctp_Rcv_info, &flags);
			if (flags & MSG_NOTIFICATION)
				/** 获取 所有 对方地址 本地地址 **/
				check_notification(sock_fd, buf_str); // 检查 通知事件
		} while (flags & MSG_NOTIFICATION);

		if (retval > 1 && buf_str[retval-1] == '\n') buf_str[retval-1] = '\0';
		std::cout << "stream[" << sctp_Rcv_info.sinfo_stream
				  << "]  size[" << retval
				  << "]  seq[" << sctp_Rcv_info.sinfo_ssn
				  << "]  assoc[" << sctp_Rcv_info.sinfo_assoc_id
				  << "]  Data: " << buf_str << std::endl;

#if 0
		struct_print(sock_fd, (struct sockaddr *)&peer_addr,
					 peer_len,
					 (struct sctp_sndrcvinfo *)&sctp_SndRcv_info, flags);
#endif
	}
}

