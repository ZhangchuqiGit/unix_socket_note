//
// Created by zcq on 2021/4/11.
//

#include "tcp_client_web.h"


/* 发起非阻塞连接 */
void start_connect(struct httpget *http_ptr);

/* 发送命令到服务器 */
void web_write_CMD(struct httpget *http_ptr);

/* 接收来自服务器的数据 */
void web_read_DATA(struct httpget *http_ptr);

/* 写文件数据 */
void file_html_write(struct httpget *http_ptr, std::string_view buf);

struct httpget http_s[MAX_pagenum];
int Maxfd = -1;
fd_set Readset, Writeset;// 鉴别事件的标志位集合
int num_connecting = 0;	// 已发起连接的网页数量
int num_unconnect = 0;	// 未建立连接的网页数量
int num_unread = 0;		// 未读取的网页数量
int max_paranum = 0;	// 最大并行连接数
int all_pagenum = 0;	// 网页的总数量

void tcp_client_nonblock_select_web(const time_t &sec/* Seconds */,
									const useconds_t &usec/* Microseconds */ )
{
	_debug_log_info("tcp_client_nonblock_select_web()")
	std::string hints_str;
	hints_str += "usage: web <#paramax> <hostname> <pagename1> <pagename2> ...\n";
	hints_str += "usage: web      3    www.baidu.com    /       image1.gif ...\n";
	hints_str += "Now input please...\n";
	hints_str += "usage: web ";
/* 	输入列表内容，如：111 222 333  zzz    gsfg
	获取每一个 std::vector<string> 子内容        */
	std::vector<std::string> vec_user = hints_strtovec(hints_str);
	if (vec_user.size() < 3)
		err_quit(-1, __LINE__,
				 "usage: web <#paramax> <hostname> <pagename1> <pagename2> ...");
/*******************************************************************/
	int retval; 			// 函数返回值

	max_paranum = strtoimax(vec_user[0].c_str(), nullptr, 10);
	_debug_log_info("最大并行连接数: %d", max_paranum);

	all_pagenum = std::min((int)vec_user.size() - 2, MAX_pagenum);
	_debug_log_info("网页的总数量: %d", all_pagenum);
	for (int i = 0; i < all_pagenum; ++i) {
		http_s[i].hostname = vec_user[1].data();
		http_s[i].pagename = vec_user[i + 2].data();
		http_s[i].flags = F_notwork;
		http_s[i].sockfd = -1;
	}
	num_unread = num_unconnect = all_pagenum;
	FD_ZERO(&Readset); 	// 初始化集合:所有位均关闭
	FD_ZERO(&Writeset); 	// 初始化集合:所有位均关闭
	fd_set rs, ws; 				// 鉴别事件的标志位集合
	struct timeval tv{};
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	while (num_unread > 0)
	{
		while (num_connecting < max_paranum && num_unconnect > 0) {
			int i;
			/* find a unconnect page */
			for (i = 0; i < all_pagenum/*网页的总数量*/; ++i)
				if (http_s[i].sockfd == -1 && http_s[i].flags == F_notwork)
					break;
			if (i == all_pagenum)
				err_quit(-1, __LINE__,
						"num_unconnect:%d but nothing found",
						num_unconnect);
			start_connect(&http_s[i]); // 发起非阻塞连接
			++num_connecting;  	// 已发起连接的网页数量
			--num_unconnect;  	// 未建立连接的网页数量
		}
/*******************************************************************/
		rs = Readset;
		ws = Writeset;
		/* select() 设置或者检查存放在数据结构 fd_set 中的标志位来鉴别事件 */
		retval = select(Maxfd + 1, &rs, &ws, nullptr,
						(sec>=0)? &tv:nullptr );
		err_sys(retval, __LINE__, "select()");
		if (retval > 0) {
			for (int sock_fd, i = 0; i < all_pagenum; ++i)
			{
				if (http_s[i].sockfd == -1 ||
					http_s[i].flags == F_notwork ||
					http_s[i].flags & F_done ) continue;
				sock_fd = http_s[i].sockfd;
/************************ check connect() ***********************/
				if ((http_s[i].flags & F_connecting) &&
					(FD_ISSET(sock_fd, &rs) || FD_ISSET(sock_fd, &ws)))
				{
					int error = 0;
					socklen_t error_len = sizeof(error);
					if (getsockopt(sock_fd, SOL_SOCKET/*通用选项*/,
								   SO_ERROR/*获得待处理错误*/,
								   &error, &error_len) < 0 || error != 0) {
						err_ret(-1, __LINE__,
								"nonblock connect failed for %s",
								http_s[i].pagename);
						--num_unread;        // 未读取的网页数量
						http_s[i].flags = F_notwork;
						FD_CLR(sock_fd, &Writeset);
						err_sys(close(sock_fd), __LINE__, "close()");
						continue;
					}
					/** nonblock connect() is already done **/
					_debug_log_info("connection established for %s",
									http_s[i].pagename);
					http_s[i].flags = F_writable;
					FD_SET(sock_fd, &Writeset);
				}
/*************************** writable *************************/
				if (http_s[i].flags & F_writable && FD_ISSET(sock_fd, &ws)) {
					/* 发送命令到服务器 */
					web_write_CMD(&http_s[i]);
					continue;
				}
/*************************** readable *************************/
				if (http_s[i].flags & F_readable && FD_ISSET(sock_fd, &rs)) {
					/* 接收来自服务器的数据 */
					web_read_DATA(&http_s[i]);
				}
			}
		}
	}
}

/* 发起非阻塞连接 */
void start_connect(struct httpget *http_ptr)
{
	_debug_log_info("发起 非阻塞连接 pagename: %s", http_ptr->pagename);
	int retval; // 函数返回值
/*************************** socket() **************************/
	struct addrinfo	*ai = Get_addrinfo(http_ptr->hostname, SERV_name_port,
										  AF_UNSPEC, SOCK_STREAM,
										  IPPROTO_IP, AI_CANONNAME );
	int sock_fd; // 套接字描述符
	sock_fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	err_sys(sock_fd, __LINE__, "socket()");
	http_ptr->sockfd = sock_fd;

/************************ 设置 I/O 非阻塞 ***********************/
//	_debug_log_info("设置 I/O 非阻塞")
	retval = Fcntl(sock_fd, F_GETFL, 0); 				// 获取 I/O 状态
	Fcntl(sock_fd, F_SETFL, retval | O_NONBLOCK); 		// 设置 I/O 非阻塞

/************************* connect() ***************************/
	_debug_log_info("start connect() for pagename: %s \tsock_fd: %d",
					http_ptr->pagename, sock_fd);
	if ( (retval = connect(sock_fd, ai->ai_addr, ai->ai_addrlen)) < 0) {
		if (errno == EINPROGRESS/* 现在正在进行中 */) {
			http_ptr->flags = F_connecting; // connect() 在后台进行中
			FD_SET(sock_fd, &Readset);
			FD_SET(sock_fd, &Writeset);
		} else err_sys(retval, __LINE__, "nonblock connect() error");
	}
	else {
		/** nonblock connect() is already done **/
		_debug_log_info("connection established for %s", http_ptr->pagename);
		http_ptr->flags = F_writable;
		FD_SET(sock_fd, &Writeset);
	}
	if (sock_fd > Maxfd) Maxfd = sock_fd;

	freeaddrinfo(ai); // 释放结构体链表指向的任何动态存储空间
}

/* 发送命令到服务器 */
void web_write_CMD(struct httpget *http_ptr)
{
	int retval; // 函数返回值
	char buf[MAXLINE] = {'\0'};

	retval = snprintf(buf, sizeof(buf), HTTP_GET_CMD, http_ptr->pagename);
	err_sys(retval, __LINE__,"snprintf()");

	retval = Write(http_ptr->sockfd, buf, strlen(buf)); // 发送命令到服务器
	err_sys(retval, __LINE__,"Write()");
	_debug_log_info("wrote %d bytes of home page %s", retval, http_ptr->pagename);

	http_ptr->flags = F_readable;
	FD_CLR(http_ptr->sockfd, &Writeset);// no more writeability test
	FD_SET(http_ptr->sockfd, &Readset);	// will read server's reply

	/* 获取 本地/对方 与某个套接字 关联的 IP地址 和 端口号。 */
	getIP_addr_port(http_ptr->sockfd, AF_INET);
}

/* 接收来自服务器的数据 */
void web_read_DATA(struct httpget *http_ptr)
{
	int retval; // 函数返回值
	char buf[MAXLINE] = {'\0'};

	if ((retval = Read(http_ptr->sockfd, buf, sizeof(buf))) <= 0) {
		if (retval < 0) err_ret(-1, __LINE__,
								"Read %s error",http_ptr->pagename);
		_debug_log_info("end-of-file on %s", http_ptr->pagename);
		err_sys(close(http_ptr->sockfd), __LINE__, "close()");
		http_ptr->flags = F_done;
		FD_CLR(http_ptr->sockfd, &Readset);
		--num_unread;        // 未读取的网页数量
		--num_connecting;    // 已发起连接的网页数量
	} else {
		_debug_log_info("read %d bytes of home page %s", retval, http_ptr->pagename);
		/* 写文件数据 */
		file_html_write(http_ptr, buf);
	}
}

/* 写文件数据 */
void file_html_write(struct httpget *http_ptr, std::string_view buf)
{
	std::string filename = "../unix_tcp_web/test.html";
	static u_char mode = 0;
	if (mode == 0) {
		mode = 1;
		unlink(filename.c_str());
	}
	/* 创建文件 最高权限 0777 */
	int file_fd = open(filename.c_str(), O_CREAT|O_APPEND|O_WRONLY, 0777);
	filename = "\n\n===========================================================\n";
	filename += "\t\thostname : ";
	filename += http_ptr->hostname;
	filename += "\t\tpagename : ";
	filename += http_ptr->pagename;
	filename += "\n============                                   ============\n";
	write(file_fd, filename.c_str(), filename.size()); /** 写文件数据 */
	write(file_fd, buf.data(), buf.size()); /** 写文件数据 */
	err_sys(close(file_fd), __LINE__, "close()");
	_debug_log_info("wrote data of home %s to file", http_ptr->pagename);
}
