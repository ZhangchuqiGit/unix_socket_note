//
// Created by zcq on 2021/4/9.
//

#include "zcq_header.h"

/*************************************************************************/
/* 	使用协议 protocol 在域 domain 中创建类型为 type 的 新 套接字描述符。
 	如果 protocol 为零(IPPROTO_IP=0)，则会自动选择一个。
 	成功返回 新 套接字描述符，如果错误则返回 -1。
int socket (int domain, int type, int protocol );
---------------------------------------------------------------------------
	创建一个流管道，返回 两个 相互连接 的 新 套接字描述符。
	它们在域 domain 中类型为 type 并使用协议 protocol 相互连接，
 	并将它们的文件描述符放入 fds[0] 和 fds[1] 中。
 	如果 protocol 为零(IPPROTO_IP=0)，则会自动选择一个。
 	成功返回 0，错误返回 -1。
int socketpair (int domain, int type, int protocol, int fds[2] );
基本用法：
	1. 这对套接字可用于 全双工通信，每一个套接字 既可读 又可写。
 例如，可以往 fds[0] 中写，从 fds[1] 中读；或者从 fds[1] 中写，从 fds[0] 中读。
	2. 如果 读写 操作位于 同一进程，文件描述符 fds[0] 和 fds[1] 是进程共享的，
 往一个套接字(如 fds[0])中写入后，再从该套接字 读时会阻塞，
 只能 在另一个套接字中(如 fds[1])上 读 成功；
	3. 如果 读写 操作位于 不同进程，文件描述符 fds[0] 和 fds[1] 是进程共享的，
 例如，父子进程，一般会功能分离；
 父进程 关闭 fds[1]，读写 fds[0]；子进程 关闭 fds[0]，读写 fds[1]。       */
/*************************************************************************/

/************************** 读写操作 位于 同一进程 *************************/
/* 读写操作 位于 同一进程 */
void unix_socketpair_same()
{
	std::cout << __func__ << "() 进程 parent ID: " << getpid() << std::endl;
	int fp = fileno(stdin); /* 转化文件描述符 */
	err_sys(fp, __FILE__, __func__, __LINE__, "fileno()");
//	char buf[MAXLINE] = {0};
	char buf[64] = {0};
	int socketpair_fd[2];
	int retval = socketpair(AF_UNIX, SOCK_STREAM, 0, socketpair_fd);
	err_sys(retval, __FILE__, __func__, __LINE__, "socketpair()");
	while (true)
	{
		retval = Read(fp, buf, sizeof(buf));
		err_sys(retval, __FILE__, __func__, __LINE__, "Read()");
		if (buf[retval-1]=='\n') buf[retval-1]='\r';
		std::cout << "input:\t" << buf << std::endl;

		retval = Write(socketpair_fd[1], buf, strlen(buf));
		err_sys(retval, __FILE__, __func__, __LINE__, "Write()");
		std::cout << "write:\t" << buf << std::endl;

		retval = Read(socketpair_fd[0], buf, sizeof(buf));
		err_sys(retval, __FILE__, __func__, __LINE__, "Read()");
		std::cout << "read:\t" << buf << std::endl;
	}
}

/********************** 读写操作 位于 不同进程（父子进程）********************/

/* 子进程 */
void unix_poll_child(const int &fd)
{
//	char buf[MAXLINE] = {0};
	char buf[64] = {0};
	int retval; // 函数返回值
	std::string strbuf;
	while (true)
	{
		retval = Read(fd, buf, sizeof(buf));
		err_sys(retval, __LINE__, "Read()");
		if (retval == 0) // TCP connection closed (TCP 断开)
		{
			debug_line(__LINE__, __func__, "close");
			/* ---- close () ---- */
			retval = close(fd);
			err_sys(retval, __LINE__, "close()");
			return;
		} else {
			if (retval > 1 && buf[retval - 1] == '\n') buf[retval - 1] = '\0';
			if (if_quit_0("child quit", buf)) {
				debug_line(__LINE__, __func__, "quit");
				return;
			}
			if (if_quit_0("quit", buf)) {
				debug_line(__LINE__, __func__, "quit");
				return;
			}
			if (if_run("time", buf)) {
				bzero(buf, sizeof(buf)); // 清 0
/* Return the current time and put it in *TIMER if timer is not nullptr */
				time_t ticks = time(nullptr);
				/* 格式化串"%a.bs" :
	对于 a，表示如果字符串长度小于a,那么右对齐左边补空格，若大于a则原样输出不限制
	对于 b，表示如果字符串长度超过b，那么只取前b个    */
				snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
			}
			strbuf = "Child: ";
			strbuf += buf;
			retval = strbuf.size();
			if (strbuf[retval-1]=='\n')	strbuf[retval-1] = '\0';
			retval = Write(fd, strbuf.c_str(), strbuf.size());
			err_sys(retval, __LINE__, "Write()");
			strbuf.clear();
		}
	}
}

/* 父进程 */
void unix_poll_parent(const int &fd)
{
	int fp = fileno(stdin); /* 转化文件描述符 */
	err_sys(fp, __FILE__, __func__, __LINE__, "fileno()");
//	char buf[MAXLINE] = {0};
	char buf[64] = {0};
	int retval; // 函数返回值
/**********************************************/
	struct pollfd fds[2];
/* 设置 有普通数据或者优先数据可读，POLLOUT | POLLWRBAND 等价 select()的可读事件 */
	fds[0].fd = fp;
	fds[0].events = POLLIN | POLLPRI;
/* 设置 有普通数据或者优先数据可读，POLLOUT | POLLWRBAND 等价 select()的可读事件 */
	fds[1].fd = fd;
	fds[1].events = POLLIN | POLLPRI;
	while (true)
	{
		/* poll() 设置或者检查存放在数据结构 pollfd 中的标志位来鉴别事件 */
		retval = poll(fds, 2, INFTIM);    // 返回就绪描述符的数量
		err_sys(retval, __FILE__, __func__, __LINE__, "poll()");

		/* input is readable */
		if (fds[0].revents & (POLLIN | POLLPRI | POLLERR)) {
//			debug_line(__LINE__, __func__, "input");
			retval = Read(fp, buf, sizeof(buf));
			err_sys(retval, __FILE__, __func__, __LINE__, "Read()");
			if (retval>1 && buf[retval - 1] == '\n') buf[retval - 1] = '\0';
			std::cout << "parent input:\t" << buf << std::endl;

			retval = Write(fd, buf, strlen(buf));
			err_sys(retval, __LINE__, "Write()");

			if (if_quit_0("quit", buf)) {
				debug_line(__LINE__, __func__, "quit");
				return;
			}
		}

		/* socket is readable */
		if (fds[1].revents & (POLLIN | POLLPRI | POLLERR)) {
//			debug_line(__LINE__, __func__, "Read");
			retval = Read(fd, buf, sizeof(buf));
			err_sys(retval, __LINE__, "Read()");
			if (retval == 0) // TCP connection closed (TCP 断开)
			{
				debug_line(__LINE__, __func__, "close");
				/* ---- close () ---- */
				retval=close(fd);
				err_sys(retval, __LINE__, "close()");
				std::cerr << "接收异常" << std::endl;
				return;
			}
			std::cout << "parent read:\t" << buf << std::endl;
		}
	}
}

/* 读写操作 位于 不同进程（父子进程）*/
void unix_socketpair_diff()
{
	std::cout << __func__ << "() 进程 parent ID: " << getpid() << std::endl;
	int socketpair_fd[2];
	int retval = socketpair(AF_UNIX, SOCK_STREAM, 0, socketpair_fd);
	err_sys(retval, __FILE__, __func__, __LINE__, "socketpair()");
/*********************************************************/
	/* ---- Signal () ---- */
	// SIGCHLD : 父进程 捕捉 子进程 的状态信号。
	Signal_fork(SIGCHLD);	/* must call waitpid() */
/*********************************************************/
	/* ---- 创建 子进程 ---- */
	pid_t child_pid = fork(); // 创建 子进程
	if (child_pid < 0) // 负值 : 创建子进程失败。
		err_sys(child_pid, (char *)__FILE__, __func__,
				(int)__LINE__, "fork()");
	else if (child_pid == 0) // 0 : 当前是 子进程 运行。
	{
		/* ---- close () ---- */
		retval=close(socketpair_fd[0]);
		err_sys(retval, __FILE__, __func__, __LINE__, "close()");
		unix_poll_child(socketpair_fd[1]);
	}
	else // 正值 : 当前是 父进程 运行。正值 为 子进程 的 进程 ID。
	{
		std::cout << "创建子进程 child ID: " << child_pid << std::endl;
		/* ---- close () ---- */
		retval=close(socketpair_fd[1]);
		err_sys(retval, __FILE__, __func__, __LINE__, "close()");
		unix_poll_parent(socketpair_fd[0]);
	}
}

/********************** 读写操作 位于 不同进程（父子进程）********************/

/* 读写操作 位于 不同进程（父子进程）
 * 进程间传递文件描述符 recv_fd
 * 单个读/写缓冲区 */
/**	进程间传递文件描述符
 0.socketpair()创建一个流管道，返回 两个 相互连接 的 新 套接字描述符。
 1.system()执行 shell 命令：
 	g++-10 -Wall -ggdb -std=c++17 ../test/openfile.cpp -o ./openfile.o
 2.注册信号 SIGCHLD：父进程 捕捉 子进程 的状态信号
 3.fork()创建的 子进程：
 	3.1 创建文件 ../test/test.txt 并写入数据；
 	3.2 execlp()执行 shell 命令：
 		./openfile.o ./openfile.o socketpair_fd[1] ../test/test.txt O_RDONLY nullptr
 	3.3 子进程 结束。
 4.父进程：
 	4.1 等待描述符 socketpair_fd[0] 变为可读
 	4.2 读取文件直至结束
 	4.3 父进程 结束。                          **/
void unix_socketpair_system_fork_exec_fds()
{
	std::cout << __func__ << "() 进程 parent ID: " << getpid() << std::endl;
	int retval; // 函数返回值
	int socketpair_fd[2];
	retval = socketpair(AF_UNIX, SOCK_STREAM, 0, socketpair_fd);
	err_sys(retval, __FILE__, __func__, __LINE__, "socketpair()");
/*********************************************************/
	/* 建议 system() 只 用来执行 shell 命令 */
	/** g++-10 -Wall -ggdb -std=c++17 ../test/openfile.cpp -o ./openfile.o */
	std::string shell_CMD = "g++-10 -Wall -ggdb -std=c++17 ";
	shell_CMD += " ../test/openfile.cpp -o ";
	std::string pathname = "./openfile.o";
	shell_CMD += pathname;
	unix_process_system(shell_CMD);
	debug_line(__LINE__, __func__, "unix_process_system()");
	/**-----------------------------------------**/
	retval=chmod(pathname.data(),0777); // 设置文件权限
	err_sys(retval, __LINE__, "chmod()");
/*********************************************************/
	/* ---- Signal () ---- */
	// SIGCHLD : 父进程 捕捉 子进程 的状态信号。
	Signal_fork(SIGCHLD);	/* must call waitpid() */
/*********************************************************/
	/* ---- 创建 子进程 ---- */
	pid_t child_pid = fork(); // 创建 子进程
	if (child_pid < 0) // 负值 : 创建子进程失败。
		err_sys(child_pid, (char *)__FILE__, __func__,
				(int)__LINE__, "fork()");
	else if (child_pid == 0) // 0 : 当前是 子进程 运行。
	{
		/* ---- close () ---- */
		retval=close(socketpair_fd[0]);
		err_sys(retval, __FILE__, __func__, __LINE__, "close()");
		/**-----------------------------------------**/
		std::string filename = "../test/test.txt";
		int file_fd = open(filename.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0777); // 创建文件 最高权限 <fcntl.h>
		write(file_fd,filename.c_str(),filename.size()); // 写文件数据 <unistd.h>
/* Return the current time and put it in *TIMER if timer is not nullptr */
		time_t ticks = time(nullptr);
		std::string buf = "\n";
		buf += ctime(&ticks);
		buf += "0123456789+-*/\nzcqzcqzcq\n";
		write(file_fd, buf.c_str(), buf.size()); // 写文件数据 <unistd.h>
		buf.clear();
		/* ---- close () ---- */
		retval=close(file_fd); // 文件创建或打开后，不使用时要 关闭文件 <unistd.h>
		err_sys(retval, __FILE__, __func__, __LINE__, "close()");
		debug_line(__LINE__, __func__, "close");
		/**-----------------------------------------**/
		retval = execlp(pathname.data(), pathname.data(),
						std::to_string(socketpair_fd[1]).data(),
						filename.data(),
						std::to_string(O_RDONLY).data(), /* open/fcntl */
						nullptr );
		err_sys(-1, __LINE__, "execl error(%d)", retval);
	}
	else // 正值 : 当前是 父进程 运行。正值 为 子进程 的 进程 ID。
	{
		std::cout << "创建子进程 child ID: " << child_pid << std::endl;
		/* ---- close () ---- */
		retval=close(socketpair_fd[1]);
		err_sys(retval, __FILE__, __func__, __LINE__, "close()");
		debug_line(__LINE__, __func__, "close");
		/**-----------------------------------------**/
		char buf[MAXLINE] = {'\0'};
		int file_fd;
		/* 设置超时: select() 等待描述符变为可读 */
		if (fd_read_select_timeo(socketpair_fd[0], Wait_forever) == 0) {
			std::cerr << "socket timeout" << std::endl;
		} else { /* socket is readable */
			/* 进程间传递文件描述符 file_fd */
			Read_fd(socketpair_fd[0], &buf, sizeof(buf), file_fd);
			std::cout << "进程间传递数据 buf:\t\t\t" << buf << std::endl;
			std::cout << "进程间传递文件描述符 file_fd:\t" << file_fd << std::endl;
			while (true) {
				retval = Read(file_fd, buf, sizeof(buf));
				err_sys(retval, __LINE__, "Read()");
				if (retval == 0) // TCP connection closed (TCP 断开)
				{
					if (errno != 0) {
						if (errno == ECHILD)
							std::cerr << "No child processes"
									  << std::endl;
						else
							std::cerr << "文件读取异常，errno = "
									  << errno << std::endl;
					} else std::cerr << "文件读取结束" << std::endl;
					/* ---- close () ---- */
					retval = close(file_fd);
					err_sys(retval, __LINE__, "close()");
					debug_line(__LINE__, __func__, "close");
					break;
				}
				if (retval > 1 && buf[retval - 1] == '\n')
					buf[retval - 1] = '\0';
				std::cout << "parent read:\n" << buf << std::endl;
			}
		}
		/* ---- close () ---- */
		retval=close(socketpair_fd[0]);
		err_sys(retval, __FILE__, __func__, __LINE__, "close()");
	}
}


/********************* I/O口复用 *************************/
/* I/O口复用
---------------------------------------------------------------------------
Select()
	通过设置或者检查存放在数据结构 fd_set 中的标志位的来鉴别事件。Fd_set是一个输入输出参数，用户通过设置(FD_SET)相应的标志位标识关注的描述符，经内核拷贝到内核空间；内核根据输入fd_set 扫描对应的描述符，新建另一fd_set标识活跃的描述符，扫描完毕后将含有活跃描述符的fd_set 拷贝到用户空间。用户检查(FD_ISSET)内核输出的fd_set 确定活跃的描述符。（进程的fd的分配都是从3开始的，0、1、2已经用作标准输入，输出和错误？而fd_set的每一位对应一个fd。）
---------------------------------------------------------------------------
进程管理最大文件描述符
Select 由宏定义FD_SETSIZE决定，用一个unsigned long数组表示。
 	一般FD_SETSIZE定义位1024，可以修改FD_SETSIZE来改变select管理描述符的数量。
Poll 基于链表存储，无上限，但受内存上限限制
Epoll 同 poll。
---------------------------------------------------------------------------
效率
Select 内核和用户空间使用内核拷贝的方式交互数据，无论内核和用户空间，
 	都需要轮询整个fd_set,当随管理的fd增加时，效率会呈线性下降。
Poll 同 select
Epoll 没有内核拷贝，而且只返回就绪的fd。在侦听大量不活跃的fd时，效率比较高。
 	但在侦听少量活跃的fd时，性能不如前两者。因为epoll使用了复杂算法。
---------------------------------------------------------------------------
IO多路复用是指内核一旦发现过程指定的一个或者多个IO条件筹备读取，它就告诉该过程。
IO多路复用实用如下场合：
    当客户解决多个描述符时（个别是交互式输出和网络套接口），必须应用I/O复用。
    当一个客户同时解决多个套接口时，而这种状况是可能的，但很少呈现。
    如果一个TCP服务器既要解决监听套接口，又要解决已连贯套接口，个别也要用到I/O复用。
    如果一个服务器即要解决TCP，又要解决UDP，个别要应用I/O复用。
    如果一个服务器要解决多个服务或多个协定，个别要应用I/O复用。
---------------------------------------------------------------------------*/

/********************* I/O口复用 *************************/
/* 	poll()函数
原型如下：
1 int poll(struct pollfd *fdarray, unsigned long nfds, int timeout);

各参数含义如下：
struct pollfd *fdarray:		一个结构体，用来保存各个描述符的相关状态。
unsigned long nfds:			fdarray 数组的大小，即里面包含有效成员的数量。
int timeout:				设定的超时时间。（以毫秒为单位）
---------------------------------------------------------------------------
参数 timeout 的设置如下：
	INFTIM (-1):     wait forever
	0   :            return immediately, do not block
	>0     :         wait specified number of milliseconds
---------------------------------------------------------------------------
poll()函数 返回值 ：
-1：有错误产生
0：超时时间到，而且没有描述符有状态变化
>0：有状态变化的描述符个数
---------------------------------------------------------------------------
着重讲 fdarray 数组，因为这是它和select()函数主要的不同的地方：
pollfd 的结构如下：
	struct pollfd {
	    int fd;            要检查的描述符
	    short events;      fd 感兴趣 的事件 / 监听的事件
	    short revents;     fd 发生 的事件 / poll()返回时描述符的返回状态
	 };
---------------------------------------------------------------------------
其实poll()和select()函数要处理的问题是相同的，只不过是不同组织在几乎相同时刻同时推出的，因此才同时保留了下来。select()函数把可读描述符、可写描述符、错误描述符分在了三个集合里，这三个集合都是用bit位来标记一个描述符，一旦有若干个描述符状态发生变化，那么它将被置位，而其他没有发生变化的描述符的bit位将被clear，也就是说select()的readset、writeset、errorset是一个value-result类型，通过它们传值，而也通过它们返回结果。这样的一个坏处是每次重新select 的时候对集合必须重新赋值。而poll()函数则与select()采用的方式不同，它通过一个结构数组保存各个描述符的状态，每个结构体第一项fd代表描述符，第二项代表要监听的事件，也就是感兴趣的事件，而第三项代表poll()返回时描述符的返回状态。合法状态如下：
---------------------------------------------------------------------------
监听事件	返回状态			合法状态
events	revents			POLLIN:        	有普通数据或者优先数据可读
events	revents			POLLRDNORM:    	有普通数据可读
events	revents			POLLRDBAND:    	有优先数据可读
events	revents			POLLPRI:       	有紧急数据可读

events	revents			POLLOUT:       	有普通数据或者优先数据可写，不会导致阻塞
events	revents			POLLWRNORM:    	有普通数据可写，不会导致阻塞
events	revents			POLLWRBAND:    	有优先数据可写，不会导致阻塞

 						POLLMSG:		SIGPOLL / SIGIO 消息可用。默认会导致进程退出。
									文件描述符准备就绪, 可以开始进行 I/O 操作。

		revents			POLLERR:       	有 错误 事件发生
		revents			POLLHUP:       	有 挂起 事件发生
		revents			POLLNVAL:      	指定的(文件)描述符 非法
---------------------------------------------------------------------------
POLLIN | POLLPRI 等价 select() 的可读事件；
POLLOUT | POLLWRBAND 等价 select() 的可写事件；
POLLIN 等价 POLLRDNORM | POLLRDBAND。
如果你对一个描述符的可读事件和可写事件以及错误等事件均感兴趣那么你应该都进行相应的设置。
 */
