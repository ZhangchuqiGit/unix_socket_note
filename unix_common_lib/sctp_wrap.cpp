
#include "sctp_wrap.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <cstdio>
#include <cstdlib>

#include <sys/types.h>
#include <cstring>
#include <arpa/inet.h> //for inet_addr
#include "unistd.h"  //for read

/* 心博控制 */
void heartbeat_action(const int &sock_fd,
					 struct sockaddr *to_addr,
					 const socklen_t &to_addr_len,
					 const u_int value)
{
	struct sctp_paddrparams peer_addr{}; // 对端地址信息
	bzero(&peer_addr,sizeof(peer_addr));
	peer_addr.spp_hbinterval = value; // 心博
	memcpy(&peer_addr.spp_address, to_addr, to_addr_len);
	Setsockopt(sock_fd,IPPROTO_SCTP/* SCTP 选项 */,
			   SCTP_PEER_ADDR_PARAMS/*对端地址信息*/, &peer_addr, sizeof(peer_addr));
}

/* sctp IP地址 转换 关联ID信息 */
sctp_assoc_t sctp_IPaddr_to_assocID(const int &sock_fd,
									struct sockaddr *to_addr,
									const socklen_t &to_addr_len,
									const sctp_assoc_t &sinfo_assoc_id)
{
	struct sctp_paddrparams peer_addr{}; // 对端地址信息
	socklen_t peer_addr_sizeof = sizeof(peer_addr);
	bzero(&peer_addr, peer_addr_sizeof);
	memcpy(&peer_addr.spp_address, to_addr, to_addr_len);
	int retval = sctp_opt_info(sock_fd, sinfo_assoc_id,
							   SCTP_PEER_ADDR_PARAMS/*对端地址信息*/,
							   &peer_addr, &peer_addr_sizeof);
	err_sys(retval, __LINE__, "sctp_opt_info()");
	std::cout << "关联地址信息 sinfo_assoc_id: " << sinfo_assoc_id << std::endl;
	std::cout << "对端地址信息 spp_assoc_id: " << peer_addr.spp_assoc_id << std::endl;
	return(peer_addr.spp_assoc_id);
}

/******************************************************************************/

int sctp_get_no_strms(const int &sock_fd, struct sockaddr *to_addr,
					  const socklen_t &to_addr_len,
					  struct sctp_sndrcvinfo sctp_SndRcv_info)
{
	struct sctp_status sctp_status{};
	socklen_t sctp_status_sizeof = sizeof(sctp_status);
	bzero(&sctp_status, sizeof(sctp_status));

#ifdef SCTP_PEER_ADDR_PARAMS/*对端地址信息*/
	sctp_status.sstat_assoc_id = sctp_IPaddr_to_assocID(sock_fd,
														to_addr, to_addr_len,
														sctp_SndRcv_info.sinfo_assoc_id);
#else
	sctp_status.sstat_assoc_id = sctp_SndRcv_info.sinfo_assoc_id;
#endif
	Getsockopt(sock_fd, IPPROTO_SCTP/* SCTP 选项*/, SCTP_STATUS/*获取关联状态*/,
			   &sctp_status, &sctp_status_sizeof );
	return(sctp_status.sstat_outstrms);
}

void struct_print (const int &sock_fd, const struct sockaddr *from,
				   const socklen_t &fromlen, const struct sctp_sndrcvinfo *sinfo,
				   const int &msg_flags)
{
	using namespace std;
	cout << "===================================" << endl
		 << "sock_fd:\t" << sock_fd << endl
		 << "---------------------" << endl
		 << "datagram from "
		 << get_addr_port_ntop_r((struct sockaddr *)from) << "\n"
		 << "---------------------" << endl
		 << "SCTP 标头信息结构: struct sctp_sndrcvinfo" << endl
		 << "sinfo_stream 所有外出消息将被发送到该流中\t"
		 << sinfo->sinfo_stream << endl
		 << "sinfo_ssn \t" 			<< sinfo->sinfo_ssn << endl
		 << "sinfo_flags \t"		<< sinfo->sinfo_flags << endl
		 << "sinfo_ppid 置于所有外出消息的SCTP净荷协议标识字段的默认值\t"
		 << sinfo->sinfo_ppid << endl
		 << "sinfo_context 指定新的默认上下文，用于检索无法发送到对端的消息\t"
		 << sinfo->sinfo_context << endl
		 << "sinfo_timetolive 指定新的默认生命周期\t"
		 << sinfo->sinfo_timetolive << endl
		 << "sinfo_tsn \t" 			<< sinfo->sinfo_tsn << endl
		 << "sinfo_cumtsn \t" 		<< sinfo->sinfo_cumtsn << endl
		 << "sinfo_assoc_id \t"		<< sinfo->sinfo_assoc_id << endl
		 << "---------------------" << endl
		 << "msg_flags \t" << msg_flags << endl
		 << "===================================" << endl;
}

