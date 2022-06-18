
#include "zcq_header.h"

#include <csignal>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "unistd.h"  //for read
#include <ctime>
#include <iostream>
//using namespace std;


/** 自动启动服务

  sudo gedit /etc/services
添加行：myservice 2000/tcp
添加行：myservice 2000/udp

  sudo gedit /etc/inetd.conf
添加行：
# servicename 	type 	proto 	flags 	user 	server_path 	args
myservice	stream	tcp	nowait	root	/media/zcq/fast-office/ZCQ_cpp_code/网络通信/ZCQ-UNIX网络编程-套接字/myservice/myservice
myservice	dgram	udp	wait	root	/media/zcq/fast-office/ZCQ_cpp_code/网络通信/ZCQ-UNIX网络编程-套接字/myservice/myservice
 ******/

/* 生成日志消息 */
void daemon_inetd(const char *pname)
{
	daemon_proc = 1;		/* for our err_XXX() functions */
	openlog(pname, LOG_PID, LOG_LOCAL1 /* 本地策略 /var/log/local1.log */ );
}

void tcp_service_daemon_inetd()
{
//	std::cout << __func__ << "\t进程 parent ID: " << getpid() << std::endl;
//	std::cout << "Hello, I am a server." << " Welcome to connect me !" << std::endl;
//	std::cout << "---------------------------------------------------" << std::endl;

	/** 生成日志消息 */
	daemon_inetd("ZCQ-UNIX网络编程-套接字");
	syslog(LOG_LOCAL1 | LOG_INFO /* /var/log/messages */,
		   "======================================================\n");
//	closelog();

	/*	获取 对方 与某个套接字 关联的 IP地址。 *//* 输出 ip 字符串 如 192.168.6.7 */
	std::string buf = Getpeername_ip(0, 0);
	err_msg(-1, __LINE__, "%s(): connection from %s",
			__func__, buf.c_str());

	char buff[MAXLINE];
/* Return the current time and put it in *TIMER if timer is not nullptr */
	time_t ticks = time(nullptr);
	/* 格式化串"%a.bs" :
对于 a，表示如果字符串长度小于a,那么右对齐左边补空格，若大于a则原样输出不限制
对于 b，表示如果字符串长度超过b，那么只取前b个    */
	snprintf(buff, sizeof(buff), "%.24s\n", ctime(&ticks) );

	buf += buff;
	err_sys(Write(0, buf.data(), buf.size()),
			__LINE__, "Write(): %s", buf.c_str());

	/* close TCP connection */
	err_sys(close(0), __LINE__, "close()");

	/** 关闭日志消息 */
	syslog(LOG_LOCAL1 | LOG_INFO /* /var/log/messages */,
		   "关闭日志消息");
	closelog();

//	std::cout << "---------------------------------------------------" << std::endl;
//	std::cout << "service : bye !" << std::endl;
}

