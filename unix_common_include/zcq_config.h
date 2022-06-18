
/* configuration options for current OS */
/********** 配置选项 ************/

#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/*************************************************************************/

#define TCP_Echo_Call   	6
#define UDP_Call    		2
#define UDP_Echo    		1
#define SCTP_Echo    		1
#define SCTP_Call    		0

/*************************************************************************/

#if              		1
#define HOST_Service_IPv4 	"192.168.6.231"				// 笔记本 ip
#define HOST_Service_IPv6 	"fe80::aaf2:1070:bef:ec29"	// 笔记本 ip
#else
#define HOST_Service_IPv4 	"192.168.6.193"				// 台式 ip
#define HOST_Service_IPv6 	"fe80::5a92:6fb5:1e20:8515"	// 台式 ip
#endif

#define ADDR_Broadcast 		"192.168.6.255" // 当前局域网的广播地址

/*************************************************************************/

// 最好用绝对路径名！
/* Unix domain stream cli-serv */
#define	UNIX_path_stream 	"/tmp/zcq_unix_local.stream"
/* Unix domain datagram cli-serv */
#define	UNIX_path_datagram 	"/tmp/zcq_unix_local.datagram"

/*************************************************************************/

/* default group name and port */
#define	SAP_NAME	"sap.mcast.net"
#define	SAP_PORT	"9875"

#define	HOST_Multicast	 		"239.255.6.6"
#define	PORT_Multicast 			20001
#define	PORT_Multicast_STR 		"20001"

/* Define some port number that can be used for client-servers */
/** sudo gedit /etc/services
	myservice 	20000/tcp
	myservice 	20000/udp       **/
/* 端口号 */
#define	PORT_service 		20000			/* TCP and UDP */
#define	PORT_service_STR	"20000"
// name_host_serv("host")

/*************************************************************************/

/* $ cat /proc/sys/net/ipv4/tcp_wmem
	4096    16384   4194304
第一个值是一个限制值，socket 发送缓存区的最少字节数；
第二个值是默认值----16384(16K)；
第三个值是一个限制值，socket 发送缓存区的最大字节数；  */

#define	MAXLINE		( 4 * 1024)		/*  4 kB：socket 发送缓存区的最少字节数 */
#define	BUFFSIZE	(16 * 1024)		/* 16 kB：socket 发送缓存区的默认字节数 */
#define	MAXSIZE		(4 * 1024 * 1024)/* 4 MB：socket 发送缓存区的最大字节数 */

#define	MAXFD	64   /* 不需要的文件描述符 */

/*可以从<syssocket.h>中的 SOMAXCONN 派生以下内容，
但是许多内核仍将其定义为5，同时实际上还支持更多内容*/
#define	MAX_listen 	64	/* 最大客户端连接数 */

/* ---- 修改 接收 缓冲区 大小 ---- */
//#define Receiver_MAXBuf_mode
#define	Receiver_MAXBuf    		MAXSIZE // 修改 缓冲区大小

/********************************* SCTP ***************************************/

#define SERV_SCTP_stream_max 	10	/* normal maximum streams SCTP 流 数目 */

/**上限，可以通过 std::string::max_size() 函数获得
在某些十六位嵌入式开发环境中，这个值是65535，也就是十六进制的0xFFFF
大部分32位环境或者系统中，这个值是 4,294,967,295，
 也就是十六进制0xFFFFFFFF。这大约有4G字节的样子。
大部分64位环境或者系统中，这个值是 4611686018427387903。
 也就是十六进制 0x3FFFFFFF FFFFFFFF。这大约有4G字节的样子。**/
/* 传输“过大”消息时部分递交，其它等待该消息完整提交 */
#ifndef Receiver_MAXBuf_mode
#define SCTP_part_recv 			65535 	/* 触发 SCTP 部分递交消息 */
#else
#define SCTP_part_recv 		Receiver_MAXBuf /* 触发 SCTP 部分递交消息 */
#endif

/*************************************************************************/

/* Posix.1g requires that an #include of <poll.h> DefinE INFTIM, but many
systems still DefinE it in <sys/stropts.h>.  We don't want to include
all the streams stuff if it's not needed, so we just DefinE INFTIM here.
This is the standard value, but there's no guarantee it is -1. */
#include <sys/poll.h>
#ifndef INFTIM
#define INFTIM          (-1)    /* wait forever */
#endif

#define Wait_forever    (-1)    /* select() wait forever */

#ifndef FD_SETSIZE
#define	FD_SETSIZE  __FD_SETSIZE
#endif

/*************************************************************************/

/*************************************************************************/


/*************************************************************************/
