//
// Created by zcq on 2021/4/11.
//

#ifndef __TCP_CLIENT_WEB_H
#define __TCP_CLIENT_WEB_H

#include "zcq_header.h"

#define	MAX_pagenum 		20

/* 80 端口是为 HTTP(HyperText Transport Protocol)即超文本传输协议开放的，
 * 此为上网冲浪使用次数最多的协议，主要用于 WWW(World Wide Web) 即万维网传输信息的协议。 */
#define	SERV_name_port 	"80"	// port number or service name

struct httpget {
	char *hostname;	// hostname or IPv4/IPv6 address
	char *pagename;	// pagename, such as '/'
	int sockfd;	// descriptor
	int flags;	/** F_xxx below **/
	pthread_t tid; // 线程标识符
};
/** F_xxx below **/
#define	F_notwork		0		// not connect.
#define	F_connecting	(1<<0)	// connect() 在后台进行中
#define	F_writable		(1<<1)	// set writable after connect() completed.
#define	F_readable		(1<<2)	// set readable after writed.
#define	F_done			(1<<3)	// all done

/* HTTP 下载协议，请求头，发送给服务端的 */
#define	HTTP_GET_CMD		"GET %s HTTP/1.1\r\n\r\n"
//#define	HTTP_GET_CMD		"GET %s HTTP/1.0\r\n\r\n"

void tcp_client_nonblock_select_web(const time_t &sec = -1/* Seconds */,
									const useconds_t &usec = 0/* Microseconds */ );

void tcp_client_pthread_web();


#endif //__TCP_CLIENT_WEB_H
