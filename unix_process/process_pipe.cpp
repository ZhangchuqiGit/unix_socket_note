
#include "zcq_header.h"

/************ 管道 pipe 具有血缘关系(公共祖先)的进程间通信 ***********/
/* 	进程间的通信方式 pipe（管道）
	管道的原理: 管道实为内核使用环形队列机制，借助内核缓冲区(如4k)实现。
	管道 是一种最基本的 IPC 机制，作用于有血缘关系(公共祖先)的进程之间，完成数据传递。
调用 pipe() 系统函数即可创建一个管道。
 	管道创建成功以后，创建该管道的进程（父进程）同时掌握着管道的读端和写端。
---------------------------------------------------------------------------
特质：
1. 其本质是一个伪文件(实为内核缓冲区)
2. 由两个文件描述符引用，一个表示读端，一个表示写端。
3. 规定数据从管道的写端流入管道，从读端流出。
---------------------------------------------------------------------------
局限性：
① 数据自己读却不能自己写。
② 数据一旦被读走，便不在管道中存在，不可反复读取。
③ 由于管道采用半双工通信方式。因此，数据只能在一个方向上流动。
④ 只能在有 公共祖先 的进程间使用管道。
常见的通信方式有，单工通信、半双工通信、全双工通信。
---------------------------------------------------------------------------
特点:
1.管道只允许具有血缘关系(公共祖先)的进程间通信，如父子进程间的通信，半双工通信。
2.管道只允许单向通信。 从消息队列中读出消息，消息队列中对应的数据都会被删除。
3.管道内部保证同步机制，从而保证访问数据的一致性。
4.面向字节流。
5.管道随进程，进程在管道在，进程消失管道对应的端口也关闭，两个进程都消失管道也消失。
---------------------------------------------------------------------------
总结：
	如果一个管道的写端一直在写，而读端的引⽤计数是否⼤于0决定管道是否会堵塞，
引用计数大于0，只写不读再次调用 write 会导致管道堵塞；
	如果一个管道的读端一直在读，而写端的引⽤计数是否⼤于0决定管道是否会堵塞，
引用计数大于0，只读不写再次调用 read 会导致管道堵塞；
	而当他们的引用计数等于0时，
只写不读 会导致 写端的进程 收到一个 SIGPIPE 信号，导致进程终止，
只读不写 会导致 read 返回 0,就像读到⽂件末尾⼀样。		*/


#include <csignal>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
//#include <ctime> // <sys/time.h>
#include <ctime>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h> // POSIX Standard: Primitive System Data Types
#include <unistd.h> // POSIX Standard: Symbolic Constants
#include <iostream>
using namespace std;


void process_pipe(); // 建立父子进程间通信的管道：先 父写 子读，后 子写 父读


/*	bash shell 	------------------------
		$ g++ test.cpp -o test
		$ ./test myfile.txt youfile.txt one.txt
	c++ 		------------------------
		int main(int argc, char **argv)
----------------------------------------
  	c++    		bash shell
  	argc	=	${#argv[@]} # 参数个数
  	argv[0]	=	myfile.txt 	# 参数 : ./当前文件名
  	argv[1]	=	myfile.txt 	# 参数 1
  	argv[2]	=	youfile.txt # 参数 2
  	argv[3]	=	one.txt 	# 参数 3	  	*/
#if 0
int main(int argc, char **argv)
{
	process_pipe(); // 建立父子进程间通信的管道：先 父写 子读，后 子写 父读

	return 0;
}
#endif


/**********************	管道 创建 步骤 **********************/
/*	实现父子进程间通信步骤:
1.父进程调用 pipe()函数创建管道，得到两个文件描述符 fd[0]、fd[1] 指向管道的 读端 和 写端。
2.父进程调用 fork()创建子进程，那么子进程也有两个文件描述符指向同一管道。
3.父进程 关闭 管道 读端，子进程 关闭 管道 写端。
  父进程 可以向管道中 写入 数据，子进程 将管道中的数据 读出。
  由于管道是利用环形队列实现的，数据从写端流入管道，从读端流出，这样就实现了进程间通信。 */

/*	int pipe(int filedes[2]); // 建立管道
filedes[0]:读取端;
filedes[1]:写入端。
返回值: 成功返回 0，否则返回 -1，错误原因存于 errno 中。
常见错误代码:
    EMFILE：进程 已用完 文件描述符 fd 最大数量（已无 文件描述符 可用）。
    EFAULT：参数 filedes 数组地址 不合法	 */
void process_pipe() // 建立父子进程间通信的管道：先 父写 子读，后 子写 父读
{
	int fd[2]; // 文件描述符
	int fd2[2]; // 文件描述符
	if (pipe(fd) == -1) // 创建管道
	{
		perror("pipe");
		exit(1);
	}
	if (pipe(fd2) == -1) // 创建管道
	{
		perror("pipe");
		exit(1);
	}
	cout << "fd " << fd[0] << endl
		 << "fd " << fd[1] << endl
		 << "fd2 " << fd2[0] << endl
		 << "fd2 " << fd2[1] << endl;

	pid_t pid = fork(); // 创建 子进程
	if(pid == -1) // 负值 : 创建子进程失败。
	{
		cerr << "创建子进程失败，pid: " << pid << endl;
		exit(1);
	}
	else if (pid==0) // 0 : 当前是 子进程 运行。
	{
		sleep(1);
		cout <<"当前是子进程运行，child ID: "<< getpid() <<endl;

/* 先 父写 子读 */
		close(fd[1]); //关闭 写 描述符
		char buf[128];
		memset(buf, 0, sizeof(buf)); // 清空.
		int len = read(fd[0], buf, sizeof(buf));  //等待管道上的数据
		cout << "child process wait to read: " << buf << "\tlen: " << len << endl;
		close(fd[0]); //关闭 读 描述符
//		write(STDOUT_FILENO, buf, len);

/* 后 子写 父读 */
		close(fd2[0]); // 关闭 读 描述符
		string buf1 = "qcz";
		printf("child process send: %s\n", buf1.c_str());
		write(fd2[1], buf1.c_str(), buf1.size());   //向管道写入字符串数据
		close(fd2[1]); //关闭 写 描述符
	}
	else // 正值 : 当前是 父进程 运行。正值 为 子进程 的 进程 ID。
	{
		pid_t pid_parent = getpid();
		cout << "当前是父进程运行，parant ID: " << pid_parent
			 << ", child ID: " << pid << endl;

/* 先 父写 子读 */
		close(fd[0]); // 关闭 读 描述符
		string buf2 = "zcq";
		printf("parent process send: %s\n", buf2.c_str());
		write(fd[1], buf2.c_str(), buf2.size());   //向管道写入字符串数据
//		write(fd[1], buf, strlen(buf));   //向管道写入字符串数据
		close(fd[1]); //关闭 写 描述符

/* 后 子写 父读 */
		close(fd2[1]); //关闭 写 描述符
		char buf[128];
		memset(buf, 0, sizeof(buf)); // 清空.
		int len = read(fd2[0], buf, sizeof(buf));  //等待管道上的数据
		cout << "parent process wait to read: " << buf << "\tlen: " << len << endl;
		close(fd2[0]); //关闭 读 描述符

		pid = wait(nullptr); // 阻塞自己 等待 有一个 子进程 死(结束)。
		cout << "Child process (pid:" << pid << ") is finish." << endl;
	}
}


