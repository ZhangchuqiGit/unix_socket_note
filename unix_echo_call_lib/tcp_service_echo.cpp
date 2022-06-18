
#include "tcp_service_echo.h"


void tcp_service_echo(const int & socket_fd)
{
	std::cout << "------ tcp_service_echo() : 'quit' is exit : " << std::endl;
#if TCP_Echo_Call==1
	/* 传递文本串，计算两数之和 */
	tcp_service_echo_sum(socket_fd);
#elif TCP_Echo_Call==2
	/* 传递(二进制)结构，计算两数之和 */
	tcp_service_echo_sum_struct(socket_fd);
#else
	/* 传递互动信息 */
	tcp_service_echo_msg(socket_fd);
#endif
	std::cout << "------ tcp_service_echo() : exit ! " << std::endl;
}

/* 传递互动信息 */
void tcp_service_echo_msg(const int & sock_fd)
{
	ssize_t	ret_value;
#ifdef Receiver_MAXBuf_mode
	char buf[Receiver_MAXBuf/* 修改 接收 缓冲区 大小 */];
#else
	char buf[MAXLINE];
#endif
	while (true) {
		ret_value = Read(sock_fd, buf, sizeof(buf)-1);
//		buf[ret_value] = 0;
		err_sys(ret_value, __LINE__, "Read()");
		if (ret_value == 0) {
			debug_line(__LINE__, (char *) __FUNCTION__);
			if (errno == EINTR) continue; // Interrupted system call
			err_ret(-1, __LINE__, "service connection closed !");
			break;//TCP connection closed (TCP 断开)
		}
		else {
			debug_line(__LINE__, (char *)__FUNCTION__);
			if (if_run("time", buf)) {
				std::cout << "接收客户端: time" << std::endl;
				bzero(buf, sizeof(buf)); // 清 0
/* Return the current time and put it in *TIMER if timer is not nullptr */
				time_t ticks = time(nullptr);
				/* 格式化串"%a.bs" :
对于 a，表示如果字符串长度小于a,那么右对齐左边补空格，若大于a则原样输出不限制
对于 b，表示如果字符串长度超过b，那么只取前b个    */
				snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
			} else {
				ret_value = strlen(buf);
				if (buf[ret_value - 1] == '\n') buf[ret_value - 1] = '\0';
				std::cout << "接收客户端: " << buf << std::endl;
				if (if_quit_0(buf, "quit")) break;
			}
			if (Write(sock_fd, buf, strlen(buf)) != ret_value)
				err_sys(ret_value, __LINE__, "Write()");
		}
	}
}

/* 传递文本串，计算两数之和 */
void tcp_service_echo_sum(const int & sock_fd)
{
	long arg1, arg2;
	ssize_t	ret_value;
	char buf[MAXLINE];
	while (true) {
		ret_value = Read(sock_fd, buf, sizeof(buf));
		if (ret_value == 0) return; // TCP connection closed (TCP 断开)
		err_sys(ret_value, __LINE__, "Read()");

		if (buf[ret_value-1] == '\n') buf[ret_value-1] = '\0';
		if ( if_quit_0(buf, "quit") ) break;

		if (sscanf(buf, "%ld %ld", &arg1, &arg2) == 2)
			snprintf(buf, sizeof(buf), "%ld\n", arg1 + arg2);
		else
			snprintf(buf, sizeof(buf), "input error\n");

		ret_value = Write(sock_fd, buf, strlen(buf));
		err_sys(ret_value, __LINE__, "Write()");
	}
}

/* 传递(二进制)结构，计算两数之和 */
void tcp_service_echo_sum_struct(const int & sock_fd)
{
	ssize_t	ret_value;
	struct args args{};
	struct result result{};
	while (true) {
		ret_value = Read(sock_fd, &args, sizeof(args));
		if (ret_value == 0) return; // TCP connection closed (TCP 断开)
		err_sys(ret_value, __LINE__, "Read()");

		result.sum = args.arg1 + args.arg2;

		ret_value = Write(sock_fd, &result, sizeof(result));
		err_sys(ret_value, __LINE__, "Write()");

		std::cout << "reciver : " << args.arg1 << " + "
				  << args.arg2 << " = " << result.sum << std::endl;
		if ( if_quit_struct(&result) ) break;
	}
}




