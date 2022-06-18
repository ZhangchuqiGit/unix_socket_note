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

#include "zcq_header.h"

/**===== c++ 线程 可和 POSIX 线程 联合使用 =====**/
#include <future>
/**------- c++ 线程 -------**/
#include <thread> // C++ 多线程
#include <atomic> // C++ 多线程
#include <mutex> // 互斥锁
#include <condition_variable> // C++ 多线程
/**------- POSIX 线程 -------**/
#include <pthread.h>
/**-----------------------**/

/* ---- 创建 服务 线程 ---- */
void pthread_tcp_client_addrinfo_create(const int &sock_fd);

void pthread_tcp_client_addrinfo()
{
	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;

	int sock_fd = tcp_socket_connect(HOST_Service_IPv4, PORT_service_STR );

	/* ---- 创建 服务 线程 ---- */
	pthread_tcp_client_addrinfo_create(sock_fd);

	pthread_exit(nullptr); // 当前线程退出，不影响进程

	/* ---- close () ---- */
	err_sys(close(sock_fd), __LINE__, "close()");

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "service : bye !" << std::endl;
}

void *pthread_tcp_client_addrinfo_main(void *arg)
{
	static pthread_t tid = pthread_self(); // 获取当前线程 ID 值
	std::cout << "(子)线程 " << __func__ << "()(tid: " << tid << ")" << std::endl;

	pthread_detach(tid); // 设置可分离线程
	_debug_log_info("线程 tid: %lu 设置可分离", tid)

	int sock_fd = *(int *) (arg); // copy arguments
	int retval; // 函数返回值
	char recvline[MAXLINE];

	while (true) {
		retval = Read(sock_fd, recvline, sizeof(recvline));
		err_sys(retval, __LINE__, "Read()");
		if (retval == 0) {
			std::cerr << "server terminated prematurely !" << std::endl;
			break;//TCP connection closed (TCP 断开)
		}
		std::cout << "接收: " << recvline << std::endl;
		if (if_run("quit", recvline)) break;
	}

	_debug_log_info("close tid: %ld", tid)
	/* ---- close () ---- */
	err_sys(close(sock_fd), __LINE__, "close()");

	return (void *)nullptr;
//	return (void *)(&tid);// 自定义 可连接线程 退出码 用 static , 静态变量
}

void *pthread_tcp_client_addrinfo_input(void *arg)
{
	static pthread_t tid = pthread_self(); // 获取当前线程 ID 值
	std::cout << "(子)线程 " << __func__ << "()(tid: " << tid << ")" << std::endl;

	int sock_fd = *(int *) (arg); // copy arguments
	int retval; // 函数返回值

	char sendline[MAXLINE];
//	std::string sendline;
	while (true) {
#if 1
		retval = Read(STDIN_FILENO, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "Read()");
		if (retval == 0) {
			std::cerr << "EOF on stdin" << std::endl;
			break;
		}
		retval = Write(sock_fd, sendline, strlen(sendline));
		err_sys(retval, __LINE__, "Write()");
#else
		sendline.clear();
		if ( !std::getline(std::cin, sendline).good() ) // 读取一行，不含'\n'
		{
			std::cerr << "EOF on stdin" << std::endl;
			break;
		}
		retval = Write(sock_fd, sendline.c_str(), sendline.size());
		err_sys(retval, __LINE__, "Write()");
#endif
		if (retval > 1 && sendline[retval-1] == '\n') sendline[retval-1] = 0;
		std::cout << "发送: " << sendline << std::endl;
		if (if_run("quit", sendline)) break;
	}

	_debug_log_info("shutdown tid: %ld", tid)
	retval = shutdown(sock_fd, SHUT_WR);	/* EOF on stdin, send FIN */
	err_ret(retval, __LINE__, "shutdown()");

	return (void *)nullptr;
//	return (void *)(&tid);// 自定义 可连接线程 退出码 用 static , 静态变量
}

/* ---- 创建 服务 线程 ---- */
void pthread_tcp_client_addrinfo_create(const int &sock_fd)
{
	int retval; 				// 返回值
	static int sockfd;			// copy arguments
	pthread_t tid; 				// 线程标识符

	sockfd = sock_fd;			// 要循环利用就不得 int sock_fd = xxx;

	retval = pthread_create(&tid, nullptr,
							pthread_tcp_client_addrinfo_main, &sockfd);
	err_sys(retval, __LINE__, "pthread_create()");
	_debug_log_info("创建服务线程 tid: %lu", tid)

	/**--------------------------------------------------**/
	pthread_attr_t attr_pth; 	// 线程 属性值

	/* 初始化 线程 默认属性值 */
	retval = pthread_attr_init(&attr_pth); // 默认属性初始化
	err_sys(retval, __LINE__, "pthread_attr_init");

	/* 设置线程属性为可分离状态 */
	retval=pthread_attr_setdetachstate(&attr_pth,PTHREAD_CREATE_DETACHED);
	err_sys(retval, __LINE__, "pthread_attr_setdetachstate");

	retval = pthread_create(&tid, &attr_pth /* 指定 属性 */,
							pthread_tcp_client_addrinfo_input, &sockfd);
	err_sys(retval, __LINE__, "pthread_create()");
	_debug_log_info("创建服务线程 tid: %lu 设置可分离", tid)

	/* 销毁 线程 属性值 */
	retval=pthread_attr_destroy(&attr_pth); // 销毁线程属性值
	err_sys(retval, __LINE__, "pthread_attr_destroy");

	/* 让 创建的服务子线程 先运行，确保变量 static int sock_fd 已成 子线程 变量副本 */
	std::this_thread::sleep_for(std::chrono::milliseconds(1));//线程休眠 1ms
}

