
#include "zcq_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <sys/types.h>
#include <cstring>
#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read

#include <iostream>
//using namespace std;


#if 0
int main(int argc, char **argv) {
	sctp_client();
}
#endif


void sctp_client()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a client." << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int ret_value; // 函数返回值
	int socket_fd; // 套接字描述符

	/* ---- socket () ---- SOCK_SEQPACKET 固定最大长度的有序，可靠，基于连接的数据报。*/
	socket_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
	err_sys(socket_fd, __LINE__, "socket()");

#ifdef Receiver_MAXBuf_mode
	/* ---- 修改 接收 缓冲区 大小 ---- */
	int receiver_buf = Receiver_MAXBuf; // 修改缓冲区大小
	ret_value=setsockopt(socket_listen_fd, SOL_SOCKET/*通用选项*/,
						 SO_RCVBUF/* 接收缓冲区 */,
						 &receiver_buf, sizeof(receiver_buf) );
	err_sys(ret_value, __LINE__,"setsockopt()");
#endif
/**************************** 设置 SCTP 通知事件 ***************************/
	/* 设置 SCTP 通知事件 */
	struct sctp_event_subscribe evnts{}; // 感兴趣事件集
	bzero(&evnts, sizeof(evnts)); // 清 0
	evnts.sctp_data_io_event = 1; //设置 I/O 通知事件 才能调用sctp_recvmsg()接收对端DATA

	ret_value=setsockopt(socket_fd, IPPROTO_SCTP/* SCTP 选项*/,
						 SCTP_EVENTS/*事件*/, &evnts, sizeof(evnts));
	err_sys(ret_value, __LINE__,"setsockopt()");
/*************************************************************************/

	/* ---- 配置 ip 地址 端口 信息 ---- */
	struct sockaddr_in client_addr{};
	bzero(&client_addr, sizeof(client_addr)); // 清 0
	client_addr.sin_family		=AF_INET; // IPv4
	client_addr.sin_port		=htons(PORT_service); // 端口号
#if 0
	client_addr.sin_addr.s_addr = inet_addr(Client_IP_host); // Internet address
#else	// Internet address
	ret_value=inet_pton(AF_INET, HOST_Service_IPv4, &client_addr.sin_addr);
	/* errno set */
	err_sys(ret_value, __LINE__, "inet_pton error for %s", HOST_Service_IPv4);
	/* errno not set */
	if (ret_value == 0)
		err_quit(ret_value, __LINE__, "inet_pton errno not set %s", HOST_Service_IPv4);
#endif

	sctp_client_call(socket_fd,
					 (struct sockaddr *) &client_addr, sizeof(client_addr));

	/* ---- close () ---- */
	ret_value=close(socket_fd);
	err_sys(ret_value, __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "client : bye !" << std::endl;
}

