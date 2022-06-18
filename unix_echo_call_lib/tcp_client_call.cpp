
/** 执行时间
   最快	非阻塞 I/O 版本 select() or poll() or epoll() or kqueue() and so on.
	|	线程 版本
	|	进程 版本
   最慢	阻塞 I/O 版本 select() or poll() or epoll() or kqueue() and so on.
   超慢	停-等 版本                    **/

#include "tcp_client_call.h"

void tcp_client_call(FILE *fp, const int &sock_fd)
{
	std::cout << "------ tcp_client_call() : 'quit' is exit : " << std::endl;
#if (TCP_Echo_Call==0 || TCP_Echo_Call==1)
	/* 传递互动信息 */ 	/* 传递文本串，计算两数之和 */
	tcp_client_call_msg(fp, sock_fd);
#elif TCP_Echo_Call==2
	/* 传递(二进制)结构，计算两数之和 */
	client_call_sum_struct(fp, sock_fd);
#elif TCP_Echo_Call==3
	/* I/O 复用 */
	tcp_client_call_select(fp, sock_fd);
#elif TCP_Echo_Call==4
	/* I/O 复用 */
	tcp_client_call_poll(fp, sock_fd);
#elif TCP_Echo_Call==5
	/* I/O 复用 高级轮询技术 kqueue */
	tcp_client_call_kqueue(fp, sock_fd);
#elif TCP_Echo_Call==6
	/* 非阻塞 I/O 复用 select */
	tcp_client_call_nonblock_select(fp, sock_fd);
#elif TCP_Echo_Call==7
	/* 父进程从标准输入，子进程从标准输出 */
	tcp_client_call_fork(fp, sock_fd);
#endif
	std::cout << "------ tcp_client_call() : exit ! " << std::endl;
}

/* 传递互动信息 */ /* 传递文本串，计算两数之和 */
void tcp_client_call_msg(FILE *fp, const int &sock_fd)
{
	char sendline[MAXLINE];
	char recvline[MAXLINE];
	size_t retval;
//	fgetc(fp); // 从终端捕获 '\n'，丢弃
	while (true)
	{
//		debug_line(__LINE__,(char *)__FUNCTION__);
		if ( if_quit_0(sendline, "quit") ) break;
		std::cout << "you input:\t";

		retval = get_Line(fp, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "get_Line()");
		retval = strlen(sendline);
		if (sendline[retval-1] == '\n') sendline[retval-1] = '\0';
		std::cout << "Write:\t\t" << sendline << std::endl;

		retval = Write(sock_fd, sendline, strlen(sendline));
		err_sys(retval, __LINE__, "Write()");

//		debug_line(__LINE__,(char *)__FUNCTION__);

		retval = Read(sock_fd, recvline, sizeof(recvline));
		if (retval > 0) {
			std::cout << "fputs:\t\t" << std::endl;
			retval = fputs(recvline, stdout);
			err_sys(retval, __LINE__, "fputs()");
		}
		else if (retval == 0) {
			debug_line(__LINE__, (char *)__FUNCTION__);
			std::cerr << "service connection closed !" << std::endl;
			break;//TCP connection closed (TCP 断开)
		}
		else { /* retval < 0 */
			err_quit(retval, __LINE__, "Read()");
		}
	}
}

/* 传递(二进制)结构，计算两数之和 */
void tcp_client_call_sum_struct(FILE *fp, const int &sock_fd)
{
	char sendline[MAXLINE];
	size_t retval;
	struct args args{};
	struct result result{};
	result.sum = -1;
	fgetc(fp); // 从终端捕获 '\n'，丢弃
	while (true)
	{
		std::cout << "you input A & B, return (A+B) : ";

		//		debug_line(__LINE__,(char *)__FUNCTION__);
		if ( if_quit_struct(&result) ) break;
		std::cout << "you input:\t";

		retval = get_Line(fp, sendline, sizeof(sendline));
		err_sys(retval, __LINE__, "get_Line()");
		retval = strlen(sendline);
		if (sendline[retval-1] == '\n') sendline[retval-1] = '\0';
		std::cout << "Write:\t\t" << sendline << std::endl;

		if (sscanf(sendline, "%ld %ld", &args.arg1, &args.arg2) != 2) {
			std::cerr << "invalid input : " << sendline << std::endl;
			continue;
		}

		retval = Write(sock_fd, &args, sizeof(args));
		err_sys(retval, __LINE__, "Write()");

		retval = Read(sock_fd, &result, sizeof(result));
		err_sys(retval, __LINE__, "Read()");
		if (retval == 0) // TCP connection closed (TCP 断开)
		{
			std::cerr << "服务器(过早)终止" << std::endl;
			return;
		} else std::cout << "result : " << result.sum << std::endl;
	}
}


