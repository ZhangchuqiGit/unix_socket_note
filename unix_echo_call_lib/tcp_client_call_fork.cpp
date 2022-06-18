
/** 执行时间
   最快	非阻塞 I/O 版本 select() or poll() or epoll() or kqueue() and so on.
	|	线程 版本
	|	进程 版本
   最慢	阻塞 I/O 版本 select() or poll() or epoll() or kqueue() and so on.
   超慢	停-等 版本                    **/

#include "tcp_client_call.h"

/*             客户端
 标准输入 ————> 父进程 ————————> 服务器
                 |
                 | fork()
                 |
 标准输出 <———— 子进程 <———————— 服务器                    */

/* 父进程从标准输入，子进程从标准输出 */
void tcp_client_call_fork(FILE *fp, int sock_fd)
{
	_debug_log_info("父进程从标准输入，子进程从标准输出")
	int retval; // 函数返回值
	int FILE_fp = fileno(fp); // 转化文件描述符
	err_sys(FILE_fp, __LINE__, "fileno(fp)");
/******************************************************/
	/* ---- Signal () ---- */
// SIGCHLD : 父进程 捕捉 子进程 的状态信号。
	Signal_fork(SIGCHLD);	/* must call waitpid() */
/******************************************************/
	/* ---- 创建 子进程 ---- */
	pid_t child_pid = fork(); // 创建 子进程
	if (child_pid == -1) // 负值 : 创建子进程失败。
	{
		err_ret(child_pid, __LINE__, "创建子进程失败");
	}
	else if (child_pid == 0) // 0 : 当前是 子进程 运行。
	{
		_debug_log_info(" 子进程 ")
		char buf[MAXLINE] = {'\0'};
		while(true) {
			retval = Read(sock_fd, buf, sizeof(buf));
			if (retval < 0) {
				if (errno != EWOULDBLOCK)
					err_sys(retval, __LINE__, "Read():read error on sock_fd");
			} else if (retval == 0) {
				err_ret(-1, __LINE__, "Read():子进程退出");
				break;
			}
//			retval = Write(STDOUT_FILENO, buf, strlen(buf));
//			err_sys(retval, __LINE__, "Write()");
			if (buf[retval-1] == '\n') buf[retval-1] = '\0';
			std::cout << "接送: " << buf << std::endl;
		}
		/* 子进程异常，通知父进程退出 */
		kill(getppid()/* 获取父进程ID */, SIGTERM/* 终止请求 */);
		exit(0);
	}
	else // 正值 : 当前是 父进程 运行。正值 为 子进程 的 进程 ID。
	{
		std::cout << "创建子进程 child ID: " << child_pid << std::endl;
		char buf[MAXLINE] = {'\0'};
		while(true) {
			retval = Read(FILE_fp, buf, sizeof(buf));
			if (retval < 0) {
				if (errno != EWOULDBLOCK)
					err_sys(retval, __LINE__, "Read():read error on sock_fd");
			} else if (retval == 0) {
				err_ret(-1, __LINE__, "Read():父进程退出");
				break;
			}
			if (retval > 1 && buf[retval-1] == '\n') buf[retval-1] = '\0';
			retval = Write(sock_fd, buf, strlen(buf));
			err_sys(retval, __LINE__, "Write()");
			std::cout << "发送: " << buf << std::endl;
		}
		/* 此处父进程不能调用 close()，用 shutdown()发送的 FIN 使子进程退出 */
		retval = shutdown(sock_fd, SHUT_WR);/* send FIN */
		err_ret(retval, __LINE__, "shutdown()");
	}
}