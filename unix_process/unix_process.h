#ifndef __UNIX_PROCESS_H
#define __UNIX_PROCESS_H

#include "zcq_header.h"

#include <iostream>
#include <csignal>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <ctime> // <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h> // POSIX Standard: Primitive System Data Types
#include <unistd.h> // POSIX Standard: Symbolic Constants
#include <syslog.h>


/**************************** 信号处理 signal *****************************/

/* Type of a signal handler.  */
//typedef void (*__sighandler_t) (int);
//typedef void  Sigfunc(int) ; // 声明 函数 是 仅有一个 int 参数 且 不返回值 的类型
/**-----------------------------------------------**/
/* sigaction() 支持信号传递信息，可用于所有信号安装 */
//Sigfunc *sigaction_func(int signum, Sigfunc *Func);
__sighandler_t sigaction_func(const int &signum, __sighandler_t Func);
void Signal_fork(const int &signum);
__sighandler_t Signal(const int &signum, __sighandler_t Func);

/**************************** 守护进程 daemon *****************************/

/* 守护进程 创建步骤：失败返回负值；成功则父进程退出，子进程继续执行 */
int daemon_init();

/**************************** cpp_log ************************************/

/* 生成自定义消息 */
void mymsg(std::string_view sbuf);

/* ---- 创建 服务 子进程 ---- */
void service_fork(const int &socket_listen_fd, const int &socket_accept_fd);

/****************************** process **********************************/

/* 建议 system() 只 用来执行 shell 命令 */
void unix_process_system(std::string_view shell_CMD);



#endif //__PROCESS_SIGNAL_H
