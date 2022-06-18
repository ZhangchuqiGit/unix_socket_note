
#include "zcq_header.h"

/**************** 消息队列 msg 两个不相关的进程之间通信 ****************/
/*	消息队列提供了一种在两个不相关的进程之间传递数据的简单有效方法，
独立于发送和接收进程而存在。消息队列在进程间以数据块为单位传递数据，
每个数据块都有一个类型标记，接受进程可以独立地接受含有不同类型值的数据块。
Linux 系统有两个宏定义 MSGMAX 与 MSGMNB，
它们以字节为单位分别定义了一条消息和一个队列的最大长度。
---------------------------------------------------------------------------
特点:
	1.消息队列可以实现消息的随机查询。消息不一定要以先进先出的次序读取，
编程时可以按消息的类型读取。
	2.消息队列允许一个或多个进程向它写入或者读取消息。
	3.与无名管道、命名管道一样，从消息队列中读出消息，消息队列中对应的数据都会被删除。
	4.每个消息队列都有消息队列标识符，消息队列的标识符在整个系统中是唯一的。
	5.消息队列是消息的链表，存放在内存中，由内核维护。只有内核重启或人工删除消息队列时，
该消息队列才会被删除。若不人工删除消息队列，消息队列会一直存在于系统中。
---------------------------------------------------------------------------
	同一个地址才能保证是同一个银行，同一个银行双方才能借助它来托管，
同一个保险柜号码才能保证是对方托管给自己的东西。
 	而在消息队列操作中，键（key）值相当于地址，消息队列标示符相当于具体的某个银行，
消息类型相当于保险柜号码。
 	同一个 键值（key） 可以保证是同一个消息队列，
 	同一个 消息队列标示符 才能保证不同的进程可以相互通信，
 	同一个 消息类型 才能保证某个进程取出是对方的信息。		*/



#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <csignal>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime> // <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h> // POSIX Standard: Primitive System Data Types
#include <unistd.h> // POSIX Standard: Symbolic Constants
#include <iostream>
//using namespace std;


key_t process_ftok(const char *pathname=nullptr, int proi_id=127); // 生成键值
void process_msg(); // 消息队列 发送 和 接受
void process_attribute(); // 消息队列属性

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
//	process_ftok("../test.txt"); // 生成键值
//	process_msg(); // 消息队列 发送 和 接受
	process_attribute(); // 消息队列属性

	return 0;
}
#endif


/**********************	消息队列 msg 常用操作函数  **********************/
/*	<sys/msg.h>  <sys/types.h>  <sys/ipc.h>
 	同一个 键值（key） 可以保证是 同一个 消息队列(标示符)，
 	同一个 消息队列(标示符) 才能保证不同的进程可以相互通信，
 	同一个 消息类型 才能保证某个进程取出是对方的信息。
---------------------------------------------------------------------------
key_t ftok(const char *pathname, int id); // 生成键值(可能会变，建议指定一个固定key值)
参数：pathname: 路径名；		id: 项目 ID，非 0 整数(只有低 8 位有效,1~127)
返回值：成功返回 key 值；	失败返回 -1
---------------------------------------------------------------------------
int msgget(key_t key, int msgflag); // 创建 新的 或 打开 一个消息队列。
不同的进程调用此函数，同一个 key 值就能得到 同一个 消息队列的标识符。
参数：
key: 键值；	特殊 键值：IPC_PRIVATE((__key_t)0)创建 私有队列
msgflag:	IPC_CREAT | IPC_EXCL | 0777(文件权限)
	IPC_CREAT	如果消息队列不存在，则创建消息队列，否则进行打开操作。
	IPC_EXCL	如果消息队列存在，则失败。必须配合 IPC_CREAT，表示“不存则创，存则失败”
	IPC_NOWAIT	等待时，函数立即返回 -1。
返回值：成功返回 消息队列的标识符；	失败返回 -1
---------------------------------------------------------------------------
int msgsnd(int msqid, const void *msg_ptr, size_t msg_sz, int msgflag);
// 发送消息。
参数：
msqid：消息队列的标识符，由 msgget()获得。
msg_ptr：指向要发送消息结构体的地址。
msg_sz：要发送消息的长度（字节数）。
msgflag：函数行为的控制属性，其取值如下：
	0：调用阻塞直到发送消息成功为止。
	IPC_NOWAIT: 等待时返回错误，若消息没有立即发送则调用该函数立即返回。
返回值：成功返回 0；	失败返回 -1
---------------------------------------------------------------------------
int msgrcv(int msqid, void *msg_ptr, size_t msg_sz, long msgtype, int msgflag);
// 读取信息
从标识符为 msqid 的消息队列中接收一个消息。一旦接收消息成功，则消息在消息队列中被删除。
参数：
msqid：消息队列的标识符，由 msgget()获得，代表要从哪个消息列中获取消息。
msg_ptr：指向要存放消息结构体的地址。
msg_sz：要存放消息的长度（字节数）。
msgtype：消息的类型。可以有以下几种类型：
	msgtyp = 0：返回队列中的第一个消息。
	msgtyp > 0：返回队列中消息类型为 msgtyp 的消息（常用）。
	msgtyp < 0：返回队列中消息类型值小于或等于 msgtyp 绝对值的消息，
			如果这种消息有若干个，则取类型值最小的消息。
	注意：若队列中有多条此类型的消息，则获取最先添加的消息，即先进先出原则。
msgflag：函数行为的控制属性，其取值如下：
	0：调用阻塞直到接收消息成功为止。
	IPC_NOWAIT: 等待时返回错误，若没有收到消息立即返回 -1。
	MSG_NOERROR: 即使消息太大也没有错误。若返回的消息字节数比 nbytes 字节数多，
 			则消息就会截短到 nbytes 字节，且不通知消息发送的进程。
返回值：成功返回 读取消息的长度；		失败返回 -1
---------------------------------------------------------------------------
int msgctl(int msqid, int cmd, struct msqid_ds *buf); // 控制: 修改、删除 消息队列
对消息队列进行各种控制，如修改消息队列的属性，或删除消息消息队列。
参数：
msqid：消息队列的标识符，由 msgget()获得。
cmd：函数功能的控制。其取值如下：
	IPC_RMID：删除由 msqid 指示的消息队列，将它从系统中删除并破坏相关数据结构。
	IPC_STAT：将 msqid 相关的数据结构中的元素的当前值 存入到 由 buf 指向的结构中。
	IPC_SET：将 msqid 相关的数据结构中的元素 设置为 由 buf 指向的结构中的对应值。
buf：数据结构 msqid_ds 地址，用来存放或更改消息队列的属性。
返回值：成功返回 0；	失败返回 -1
---------------------------------------------------------------------------
	linux内核采用的结构 msqid_ds 管理 消息队列
struct msqid_ds	{	// 消息队列的属性
    struct ipc_perm msg_perm;  //消息队列访问权限
    struct msg *msg_first;    //指向第一个消息的指针
    struct msg *msg_last;     //指向最后一个消息的指针
	ulong  msg_cbytes;       //消息队列当前的字节数
	ulong  msg_qnum;        //消息队列当前的消息个数
	ulong  msg_qbytes;     //消息队列可容纳的最大字节数
	pid_t  msg_lsqid;     //最后发送消息的进程号ID
	pid_t  msg_lrqid;     //最后接收消息的进程号ID
	time_t msg_stime;     //最后发送消息的时间
	time_t msg_rtime;     //最后接收消息的时间
	time_t msg_ctime;    //最近修改消息队列的时间
};
---------------------------------------------------------------------------
	linux内核采用的结构 msg_queue 来描述 消息队列
struct msg_queue {		// 消息队列的状态
	struct ipc_perm q_perm;
	time_t q_stime;       // last msgsnd() time
	time_t q_rtime;       // last msgrcv() time
	time_t q_ctime;       // last change time
	unsigned long q_cbytes;    // 当前队列中的字节数
	unsigned long q_qnum;      // 当前队列中的消息数
	unsigned long q_qbytes;    // 队列上的最大字节数
	pid_t q_lspid;       // 最后一刻 msgsnd()
	pid_t q_lrpid;       // last receive pid
	struct list_head q_messages;
	struct list_head q_receivers;
	struct list_head q_senders;
};
---------------------------------------------------------------------------
	syetem V IPC 为每一个 IPC 结构设置了一个 ipc_perm 结构，该结构规定了许可权和所有者
struct ipc_perm {
   	key_t  key;    //调用shmget()时给出的关键字
   	uid_t  uid;    //共享内存所有者的有效用户ID
  	gid_t  gid;    //共享内存所有者所属组的有效组ID
   	uid_t  cuid;   //共享内存创建 者的有效用户ID
   	gid_t  cgid;   //共享内存创建者所属组的有效组ID
  	unsigned short  mode; //Permissions + SHM_DEST和SHM_LOCKED标志
  	unsigned short   seq; //序列号
};		 */


/* key_t ftok(const char *pathname, int id);//生成键值
注意当 pathname 文件不存在，键值会变，建议 自定义 key 键值
参数：pathname: 路径名；		id: 项目 ID，非 0 整数(只有低 8 位有效,1~127)
返回值：成功返回 key 值；	失败返回 -1		*/
key_t process_ftok(const char *pathname, int proi_id) // 生成键值
{
	using namespace std;
	key_t key; // 键值
	if (pathname == nullptr)
	{
		key = random();
	}
	else
	{
		key = ftok(pathname, proi_id);
		if (key == -1){
			perror("ftok failed");
			cerr << strerror(errno) << "；错误值:" << errno << endl;
			exit(EXIT_FAILURE);
		}
	}
	cout << "key = " << key << endl;
	return key;
}


struct msg_data // 自定义数据包
{
	int type;
	char str[32];
	int size = sizeof(this->str) - sizeof(char);
};

void recievef(int msqid, msg_data mymsg_data)
{
	printf("------------ recievef begin !\n");
	while (true)
	{
		/* 读取信息 */
		if (msgrcv(msqid,&mymsg_data,sizeof(mymsg_data.str),0,0)==-1)
		{
			fprintf(stderr,"msgrcv failure %d : %s\n",
					errno,strerror(errno));
			exit(EXIT_FAILURE);
		}
		printf("recieve data :\n");
		printf("type : %d\n",mymsg_data.type);
		printf("str : %s",mymsg_data.str);

		/* 读取结束 */
		if (strncmp(mymsg_data.str,"end",3)==0)
		{
			printf("读取结束\n");
			break;
		}

		/* 发送消息 */
		strcpy(mymsg_data.str,"Recieve OK\n");
		if(msgsnd(msqid,&mymsg_data,sizeof(mymsg_data.str),0)== -1)
		{
			fprintf(stderr,"msgsnd failure %d : %s\n",
					errno,strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/* IPC_RMID 删除由 msqid 指示的消息队列 */
	if (msgctl(msqid,IPC_RMID,nullptr) == -1) // 控制: 修改、删除 消息队列
	{
		fprintf(stderr,"msgctl failure %d",errno);
		exit(EXIT_FAILURE);
	}
	printf("------------ recievef end !\n");
}

void sendf(int msqid, msg_data mymsg_data)
{
	printf("------------ sendf begin !\n");
	int num=0;

	while (true)
	{
		++num;
		printf(" NO:%d send: ",num);
		mymsg_data.type=num;

		/* 从终端读取一条信息 */
		if (fgets(mymsg_data.str, sizeof(mymsg_data.str), stdin) == nullptr) // -1
		{
			fprintf(stderr,"fgets error !\n");
			exit(9);
		}
		printf("You wrote: %s", mymsg_data.str);

		/* 发送消息 */
		if(msgsnd(msqid,&mymsg_data,sizeof(mymsg_data.str),0)== -1)
		{
			fprintf(stderr,"msgsnd failure %d : %s\n",
					errno,strerror(errno));
			exit(EXIT_FAILURE);
		}

		/* 发送结束 */
		if (strncmp(mymsg_data.str,"end",3)==0)
		{
			printf("发送结束\n");
			break;
		}

		/* 读取信息 */
		if (msgrcv(msqid,&mymsg_data,sizeof(mymsg_data.str),0,0)==-1)
		{
			fprintf(stderr,"msgrcv failure %d : %s\n",
					errno,strerror(errno));
			exit(EXIT_FAILURE);
		}
		else printf("The recipient reported: %s\n",mymsg_data.str);
	}

	printf("------------ sendf end !\n");
}

void process_msg() // 消息队列 发送 和 接受
{
	key_t key = process_ftok(); // 生成键值
	int msqid; // 消息队列的标识符

	/* 创建 新的 或 打开 一个消息队列。 */
	msqid = msgget(key,IPC_CREAT | 0777/*文件权限*/);
//	msqid=msgget((key_t)123/*自定义*/,IPC_CREAT|0777);//创建 新的 或 打开 一个消息队列。
	if (msqid == -1)
	{
		fprintf(stderr,"msgget failure %d : %s\n",errno,strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct msg_data mymsg_data{123456789,"zcq"};
	while (true)
	{
		printf("recieve:0\tsendf:1\nput in : ");
		int ch = fgetc(stdin); // 读取一个字符
		fgetc(stdin); // 读取一个字符 '\n'
		if (ch == EOF) // -1
		{
			fprintf(stderr,"put in error !\n");
		}
		else
		{
			if (ch == '0')
			{
				recievef(msqid, mymsg_data);
				break;
			}
			else if (ch == '1')
			{
				sendf(msqid, mymsg_data);
				break;
			}
		}
	}
}


/*	linux内核采用的结构 msqid_ds 管理 消息队列
struct msqid_ds	{									// 消息队列的属性
	struct ipc_perm msg_perm;  //消息队列访问权限
	struct msg *msg_first;    //指向第一个消息的指针
	struct msg *msg_last;     //指向最后一个消息的指针
	ulong  msg_cbytes;       //
	ulong  msg_qnum;        //消息队列当前的消息个数
	ulong  msg_qbytes;     //消息队列可容纳的最大字节数
	pid_t  msg_lsqid;     //最后发送消息的进程号ID
	pid_t  msg_lrqid;     //最后接收消息的进程号ID
	time_t msg_stime;     //最后发送消息的时间
	time_t msg_rtime;     //最后接收消息的时间
	time_t msg_ctime;    //最近修改消息队列的时间
};
---------------------------------------------------------------------------
int msgctl(int msqid, int cmd, struct msqid_ds *buf); // 控制: 修改、删除 消息队列
对消息队列进行各种控制，如修改消息队列的属性，或删除消息消息队列。
参数：
msqid：消息队列的标识符，由 msgget()获得。
cmd：函数功能的控制。其取值如下：
IPC_RMID：删除由 msqid 指示的消息队列，将它从系统中删除并破坏相关数据结构。
IPC_STAT：将 msqid 相关的数据结构中的元素的当前值 存入到 由 buf 指向的结构中。
IPC_SET：将 msqid 相关的数据结构中的元素 设置为 由 buf 指向的结构中的对应值。
buf：数据结构 msqid_ds 地址，用来存放或更改消息队列的属性。
返回值：成功返回 0；	失败返回 -1 			*/
void msg_stat(int msqid, struct msqid_ds &msg_info) // 输出消息队列属性
{
	using namespace std;
	if (msgctl(msqid, IPC_STAT, &msg_info) == -1) // 控制: 修改、删除 消息队列
	{
		fprintf(stderr,"get msg info error : %d",errno);
		exit(EXIT_FAILURE);
	}
	printf( "消息队列当前的字节数: %ld\n",msg_info.msg_cbytes);
	printf( "number of messages in queue is %lu\n",msg_info.msg_qnum);
	printf( "max number of bytes on queue is %lu\n",msg_info.msg_qbytes);
	//每个消息队列的容量（字节数）都有限制MSGMNB，值的大小因系统而异。在创建新的消息队列时，//msg_qbytes的缺省值就是MSGMNB
	printf( "pid of last msgsnd is %d\n",msg_info.msg_lspid);
	printf( "pid of last msgrcv is %d\n",msg_info.msg_lrpid);
	printf( "last msgsnd time is %s", ctime(&(msg_info.msg_stime)));
	printf( "last msgrcv time is %s", ctime(&(msg_info.msg_rtime)));
	printf( "last change time is %s", ctime(&(msg_info.msg_ctime)));
	printf( "msg uid is %d\n",msg_info.msg_perm.uid);
	printf( "msg gid is %d\n",msg_info.msg_perm.gid);
}

void process_attribute() // 消息队列属性
{
	key_t key = process_ftok("../test"); // 生成键值
	int msqid; // 消息队列的标识符

	/* 创建 新的 或 打开 一个消息队列。 */
	msqid = msgget(key, IPC_CREAT|IPC_EXCL|0777/*文件权限*/);
//	msqid=msgget((key_t)123/*自定义*/,IPC_CREAT|0777);//创建 新的 或 打开 一个消息队列。
	if (msqid == -1)
	{
		fprintf(stderr,"msgget failure %d : %s\n",errno,strerror(errno));
		exit(EXIT_FAILURE);	/* EXIT_FAILURE  1   Failing */
	}

	struct msqid_ds mymsg_data{}; // 消息队列的属性

	msg_stat(msqid, mymsg_data); // 输出消息队列属性

	struct msg_data msg_sbuf{}; // 自定义数据包
	msg_sbuf.type=10;
	strcpy(msg_sbuf.str,"zcq");

	/* 发送消息 */
	if(msgsnd(msqid, &msg_sbuf, msg_sbuf.size,IPC_NOWAIT)== -1)
	{
		fprintf(stderr,"msgsnd failure %d : %s\n",
				errno,strerror(errno));
		exit(EXIT_FAILURE);
	}

	msg_stat(msqid, mymsg_data); // 输出消息队列属性

	/* IPC_RMID 删除由 msqid 指示的消息队列 */
	if (msgctl(msqid,IPC_RMID,nullptr) == -1) // 控制: 修改、删除 消息队列
	{
		fprintf(stderr,"msgctl failure %d",errno);
		exit(EXIT_FAILURE);
	}
}



