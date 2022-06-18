//
// Created by zcq on 2021/3/28.
//

#include "sctp_event.h"


/**	evnts.sctp_data_io_event = 1; // I/O 通知事件 才能调用sctp_recvmsg()接收对端DATA

 	evnts.sctp_association_event = 1; 		// SCTP关联事件
	evnts.sctp_address_event = 1; 			// SCTP地址事件
	evnts.sctp_send_failure_event = 1; 		// SCTP发送失败事件
	evnts.sctp_peer_error_event = 1; 		// sctp peer 错误事件
	evnts.sctp_shutdown_event = 1; 			// 连接被关闭
	evnts.sctp_partial_delivery_event = 1; 	// 部分递交
	evnts.sctp_adaptation_layer_event = 1; 	// SCTP适配层事件
	evnts.sctp_authentication_event = 1; 	// SCTP身份验证事件
	evnts.sctp_sender_dry_event = 1; 		//
	evnts.sctp_stream_reset_event = 1; 		// SCTP流重置事件
	evnts.sctp_assoc_reset_event = 1; 		// SCTP关联重置事件
	evnts.sctp_stream_change_event = 1; 	// SCTP流更改事件           **/

/* 打印 通知事件 *//** 设置的通知事件存在 接收区 buf_str 的头部区域 **/
void print_notification_event(std::string_view buf_str)
{
	union sctp_notification *sn_p; 			// (所有)通知类型 + Data
	sn_p = (union sctp_notification *) buf_str.data();

	struct sctp_assoc_change *sac; 			// SCTP关联事件
	struct sctp_paddr_change *spc;			// SCTP peer 地址事件
	struct sctp_send_failed *ssf;			// SCTP发送失败事件
	struct sctp_remote_error *sre;			// sctp peer 错误事件
	struct sctp_shutdown_event *sse;		// 连接被关闭
	struct sctp_pdapi_event *pdapi;			// 部分递交
	struct sctp_adaptation_event *sae;		// SCTP适配层事件
//	struct sctp_authkey_event *sake;		// SCTP身份验证事件
//	struct sctp_sender_dry_event *ssde;		//
//	struct sctp_stream_reset_event *ssre;	// SCTP流重置事件
//	struct sctp_assoc_reset_event *sare;	// SCTP关联重置事件
//	struct sctp_stream_change_event *ssce;	// SCTP流更改事件

	using namespace std;
	string str;
	switch(sn_p->sn_header.sn_type/*通知类型*/)
	{
		case SCTP_ASSOC_CHANGE:				/* SCTP关联事件 */
			sac = &sn_p->sn_assoc_change;
			switch(sac->sac_state) {
				case SCTP_COMM_UP:			// 通信建立
					str = "COMMUNICATION UP/通信建立";
					break;
				case SCTP_COMM_LOST:		// 通信丢失
					str = "COMMUNICATION LOST/通信丢失";
					break;
				case SCTP_RESTART:			// 重启
					str = "RESTART/重启";
					break;
				case SCTP_SHUTDOWN_COMP:	// 关机
					str = "SHUTDOWN COMPLETE/关机";
					break;
				case SCTP_CANT_STR_ASSOC:	// 无法关联
					str = "CAN'T START ASSOC/无法关联";
					break;
				default:
					str = "UNKNOWN";
					break;
			}
			cout << "SCTP_ASSOC_CHANGE/SCTP关联事件: " << str << endl
				 << "sac_assoc_id: " << sac->sac_assoc_id << endl;
			break;

		case SCTP_PEER_ADDR_CHANGE: 		/* SCTP peer 地址事件 */
			spc = &sn_p->sn_paddr_change;
			switch(spc->spc_state) {
				case SCTP_ADDR_AVAILABLE: 	// 地址有效
					str = "ADDRESS AVAILABLE/地址有效";
					break;
				case SCTP_ADDR_UNREACHABLE:	// 地址无法访问
					str = "ADDRESS UNREACHABLE/地址无法访问";
					break;
				case SCTP_ADDR_REMOVED:		// 地址已删除
					str = "ADDRESS REMOVED/地址已删除";
					break;
				case SCTP_ADDR_ADDED:		// 地址已添加
					str = "ADDRESS ADDED/地址已添加";
					break;
				case SCTP_ADDR_MADE_PRIM:	// 主要地址
					str = "ADDRESS MADE PRIMARY/主要地址";
					break;
				case SCTP_ADDR_CONFIRMED:	// 地址确认
					str = "ADDRESS CONFIRMED/地址确认";
					break;
				default:
					str = "UNKNOWN";
					break;
			}
			cout << "SCTP_PEER_ADDR_CHANGE/SCTP peer 地址事件: " << str << endl
				 << "addr: " << Addr_net_to_ptr_r((struct sockaddr *)&spc->spc_aaddr)
				 << "\tspc_assoc_id: " << spc->spc_assoc_id << endl;
			break;

		case SCTP_SEND_FAILED: 				/* SCTP发送失败事件 */
			ssf = &sn_p->sn_send_failed;
			cout << "SCTP_SEND_FAILED/SCTP发送失败事件: "
				 << "ssf_assoc_id: " << ssf->ssf_assoc_id
				 << "\tssf_error: " << ssf->ssf_error << endl;
			break;

		case SCTP_REMOTE_ERROR: 			/* sctp peer 错误事件 */
			sre = &sn_p->sn_remote_error;
			cout << "SCTP_REMOTE_ERROR/SCTP peer 错误事件: "
				 << "sre_assoc_id: " << sre->sre_assoc_id
				 << "\tsre_error: " << sre->sre_error << endl;
			break;

		case SCTP_SHUTDOWN_EVENT: 			/* 连接被关闭 */
			sse = &sn_p->sn_shutdown_event;
			cout << "SCTP_SHUTDOWN_EVENT/连接被关闭: sse_assoc_id: 0x"
				 << hex << sse->sse_assoc_id << endl;
			break;

		case SCTP_PARTIAL_DELIVERY_EVENT: 	/* 部分递交 */
			pdapi = &sn_p->sn_pdapi_event;
			if(pdapi->pdapi_indication == SCTP_PARTIAL_DELIVERY_ABORTED)
				cout << "SCTP_PARTIAL_DELIEVERY_ABORTED/部分递交" << endl;
			else
				cout << "Unknown SCTP_PARTIAL_DELIEVERY_ABORTED/部分递交: 0x"
					 << hex << pdapi->pdapi_indication << endl;
			break;

		case SCTP_ADAPTATION_INDICATION: 	/* SCTP适配层事件 */
			sae = &sn_p->sn_adaptation_event;
			cout << "SCTP_ADAPTATION_INDICATION/SCTP适配层事件: "
				 << "sai_assoc_id: " << sae->sai_assoc_id << endl;
			break;

		default:
			cout << "Unknown notification event sn_type: 0x"
				 << hex << sn_p->sn_header.sn_type << endl;
	}
}


void sctp_print_addresses(struct sockaddr *sa, const int &sa_num)
{
	int sa_len;
	for (int i = 0; i < sa_num; ++i) {
		std::cout << "NO " << i << " : " << get_addr_port_ntop_r(sa) << std::endl;
		switch(sa->sa_family) {
			case AF_INET: sa_len =  sizeof(struct sockaddr_in); break;
			case AF_INET6: sa_len = sizeof(struct sockaddr_in6); break;
			default:
				err_ret(-1, __FILE__, __func__, __LINE__,
						"sctp_print_addresses(): unknown AF");
				break;
		}
		sa = (struct sockaddr *)((char *)sa + sa_len);
	}
}

/* 检查 通知事件 *//** 获取 所有 对方地址 本地地址 **/
void check_notification(const int &sock_fd, std::string_view bufstr)
{
	union sctp_notification *sn_p; 			// (所有)通知类型 + Data
	sn_p = (union sctp_notification *) bufstr.data();

	struct sctp_assoc_change *sac; 			// SCTP关联事件

	using namespace std;
	switch(sn_p->sn_header.sn_type/*通知类型*/)
	{
		case SCTP_ASSOC_CHANGE:				/* SCTP关联事件 */
			sac = &sn_p->sn_assoc_change;
			if((sac->sac_state == SCTP_COMM_UP) || // 通信建立
			   (sac->sac_state == SCTP_RESTART)) { // 重启
				struct sockaddr *sa_peer, *sa_local;

				/* 在套接字上 获取所有 对方地址 */
				int num_p = sctp_getpaddrs(sock_fd, sac->sac_assoc_id, &sa_peer);
				cout << "There are " << num_p
					 << " remote addresses and they are" << endl;
				sctp_print_addresses(sa_peer, num_p);
				/* 释放由 sctp_getpaddrs() 分配的所有资源 */
				sctp_freepaddrs(sa_peer);

				/* 在套接字上 获取所有 本地地址 */
				int num_l = sctp_getladdrs(sock_fd, sac->sac_assoc_id, &sa_local);
				cout << "There are " << num_l
					 << " local addresses and they are" << endl;
				sctp_print_addresses(sa_local, num_l);
				/* 释放由 sctp_getladdrs() 分配的所有资源 */
				sctp_freeladdrs(sa_local);
			}
			break;
		default: break;
	}
}