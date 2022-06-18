/** 创建新的线程不影响已打开的描述符的引用计数，这一点不同于 fork()
 最后总结下std::thread对比于pthread的优缺点：
优点：
1. 简单，易用
2. 跨平台，pthread只能用在 POSIX(ANSI C) 系统上（其他系统有其独立的thread实现）
3. 提供了更多高级功能，比如future
4. 更加C++（跟匿名函数，std::bind，RAII等C++特性更好的集成）

缺点：
1. 没有RWlock。有一个类似的shared_mutex，不过它属于C++14,你的编译器很有可能不支持。
2. 操作线程和Mutex等的API较少。毕竟为了跨平台，只能选取各原生实现的子集。如果你需要设置某些属性，
 需要通过API调用返回原生平台上的对应对象，再对返回的对象进行操作。
 **/
#include "tcp_client_web.h"

/**===== c++ 线程 可和 POSIX 线程 联合使用 =====**/
#include <future>
#include <threads.h> // ISO C11 标准
/**------- c++ 线程 -------**/
#include <thread> // C++ 多线程
#include <atomic> // C++ 多线程
#include <mutex> // 互斥锁
#include <condition_variable> // C++ 多线程
/**------- POSIX 线程 -------**/
#include <pthread.h>
/**-----------------------**/


/* 发起非阻塞连接 */
void *pthread_start_connect(void *arg);

/* 发送命令到服务器 */
void pthread_web_write_CMD(struct httpget *http_ptr);

/* 接收来自服务器的数据 */
void pthread_web_read_DATA(struct httpget *http_ptr);

/* 写文件数据 */
void pthread_file_html_write(const uint8_t &mode,
							 struct httpget *http_ptr, std::string_view buf);

struct httpget phttp_s[MAX_pagenum];

void tcp_client_pthread_web()
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
	int pnum_connecting = 0;// 已发起连接的网页数量
	int pnum_unconnect = 0;	// 未建立连接的网页数量
	int pnum_unread = 0;	// 未读取的网页数量
	int pmax_paranum = 0;	// 最大并行连接数
	int pall_pagenum = 0;	// 网页的总数量

	pmax_paranum = strtoimax(vec_user[0].c_str(), nullptr, 10);
	_debug_log_info("最大并行连接数: %d \tallnum: %zu", pmax_paranum, vec_user.size());

	pall_pagenum = std::min( (int)vec_user.size() - 2, MAX_pagenum);
	_debug_log_info("网页的总数量: %d", pall_pagenum);
	for (int i = 0; i < pall_pagenum; ++i) {
		phttp_s[i].hostname = vec_user[1].data();
		phttp_s[i].pagename = vec_user[i + 2].data();
		phttp_s[i].flags = F_notwork;
		phttp_s[i].sockfd = -1;
		phttp_s[i].tid = 0;
	}
	pnum_unread = pnum_unconnect = pall_pagenum;

	while (pnum_unread > 0 && pmax_paranum > 0)
	{
		while (pnum_connecting < pmax_paranum && pnum_unconnect > 0) {
			int i;
			/* find a unconnect page */
			for (i = 0; i < pall_pagenum/*网页的总数量*/; ++i) {
				if (phttp_s[i].sockfd == -1 &&
					phttp_s[i].flags == F_notwork)
					break;
			}
			if (i == pall_pagenum) {
				err_quit(-1, __LINE__,
						 "pnum_unconnect:%d but nothing found", pnum_unconnect);
			}

			pthread_t tid = 0;
			retval = pthread_create(&tid, nullptr,
									pthread_start_connect,
									&phttp_s[i]);
			err_sys(retval, __LINE__, "pthread_create()");
			_debug_log_info("创建服务线程 tid: %lu", tid)

			++pnum_connecting; 	// 已发起连接的网页数量
			--pnum_unconnect; 	// 未建立连接的网页数量
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));//线程休眠 1ms

		for (int i = 0, ret; i < pall_pagenum/*网页的总数量*/; ++i) {
			if (phttp_s[i].sockfd != -1) {
				/*尝试等待可连接线程结束，检查可连接线程是否已终止，回收线程*/
				ret = pthread_tryjoin_np(phttp_s[i].tid, nullptr);
				if (ret <= 0) {
					--pnum_unread; 		// 已发起连接的网页数量
					--pnum_connecting; 	// 已发起连接的网页数量
					_debug_log_info("回收线程 tid: %lu \tfd: %d",
									phttp_s[i].tid, phttp_s[i].sockfd)
					close(phttp_s[i].sockfd);
					phttp_s[i].sockfd = -1;
					break;
				}
			}
		}
	}
}

/* 发起连接 */
void *pthread_start_connect(void *arg)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));//线程休眠 1ms
	auto http_ptr = (struct httpget *)(arg); // copy arguments
	http_ptr->tid = pthread_self(); // 获取当前线程 ID 值
	http_ptr->flags = F_connecting;
	http_ptr->sockfd = tcp_socket_connect (http_ptr->hostname, SERV_name_port,
										   AF_UNSPEC, SOCK_STREAM,
										   IPPROTO_IP, AI_CANONNAME );
	http_ptr->flags = F_writable;
	std::cout << "start connect() for pagenam" << http_ptr->pagename
			  << " fd: " << http_ptr->sockfd
			  << " tid: " << http_ptr->tid << std::endl;

	/* 发送命令到服务器 */
	pthread_web_write_CMD(http_ptr);

	/* 接收来自服务器的数据 */
	pthread_web_read_DATA(http_ptr);

	http_ptr->flags = F_done;

	return (void *)nullptr;
//	return (void *)(&tid);// 自定义 可连接线程 退出码 用 static , 静态变量
}

/* 发送命令到服务器 */
void pthread_web_write_CMD(struct httpget *http_ptr)
{
	int retval; // 函数返回值
	char buf[MAXLINE] = {0};

	retval = snprintf(buf, sizeof(buf), HTTP_GET_CMD, http_ptr->pagename);
	err_sys(retval, __LINE__,"snprintf()");

	retval = Write(http_ptr->sockfd, buf, strlen(buf)); // 发送命令到服务器
	err_sys(retval, __LINE__,"Write()");
	std::cout << "wrote " << retval
			  << " bytes of home page " << http_ptr->pagename
			  << " fd: " << http_ptr->sockfd
			  << " tid: " << http_ptr->tid << std::endl;

	http_ptr->flags = F_readable;

//	/* 获取 本地/对方 与某个套接字 关联的 IP地址 和 端口号。 */
//	getIP_addr_port(http_ptr->sockfd, AF_INET);
}

/* 接收来自服务器的数据 */
void pthread_web_read_DATA(struct httpget *http_ptr)
{
	int retval; // 函数返回值
	char buf[MAXLINE] = {0};
	pthread_file_html_write(1, http_ptr, buf);
	while (true) {
		if ((retval = Read(http_ptr->sockfd, buf, sizeof(buf))) <= 0) {
			err_ret(retval, __LINE__, "Read %s (fd: %d) error",
					http_ptr->pagename, http_ptr->sockfd);
			std::cout << "end-of-file on  " << http_ptr->pagename
					  << " fd: " << http_ptr->sockfd
					  << " tid: " << http_ptr->tid << std::endl;
			break;
		} else {
			std::cout << "read " << retval
					  << " bytes of home page " << http_ptr->pagename
					  << " fd: " << http_ptr->sockfd
					  << " tid: " << http_ptr->tid << std::endl;
			/* 写文件数据 */
			pthread_file_html_write(0, http_ptr, buf);
		}
	}
}

/* 写文件数据 */
void pthread_file_html_write(const uint8_t &mode,
							 struct httpget *http_ptr, std::string_view buf) {
	std::string filename;
	filename = "../unix_tcp_web/test.html";
	filename.insert(filename.size() - 5, std::to_string(http_ptr->tid));
	if (mode) unlink(filename.c_str());
	else {
		/* 创建文件 最高权限 0777 */
		int file_fd = open(filename.c_str(), O_CREAT | O_APPEND | O_WRONLY,
						   0777);
		filename = "\n\n===========================================================\n";
		filename += "\t\thostname : ";
		filename += http_ptr->hostname;
		filename += "\t\tpagename : ";
		filename += http_ptr->pagename;
		filename += "\t\tfd : ";
		filename += std::to_string(http_ptr->sockfd);
		filename += "\t\ttid : ";
		filename += std::to_string(http_ptr->tid);
		filename += "\n============                                   ============\n";
		write(file_fd, filename.c_str(), filename.size()); /** 写文件数据 */
		write(file_fd, buf.data(), buf.size()); /** 写文件数据 */
		err_sys(close(file_fd), __LINE__, "close()");
		std::cout << "wrote data of pagename " << http_ptr->pagename
				  << " fd: " << http_ptr->sockfd
				  << " tid: " << http_ptr->tid << std::endl;
	}
}
