
/**** 多线程 的 同步 与 互斥
互斥锁(同步)、条件变量(同步)、读写锁(同步)、自旋锁(同步)、信号量(同步与互斥) ****/
/* 	同步 与 互斥 的概念：
现代操作系统基本都是多任务操作系统，即同时有大量可调度实体在运行。
在多任务操作系统中，同时运行的多个任务可能：
 	都需要访问/使用同一种资源；
    多个任务之间有依赖关系，某个任务的运行依赖于另一个任务。
---------------------------------------------------------------------------
【同步】：
	是指散步在不同任务之间的若干程序片断，它们的运行必须严格按照规定的某种 先后次序 来运行，
这种 先后次序 依赖于要完成的特定的任务。
	最基本的场景就是：两个或两个以上的进程或线程在运行过程中协同步调，
按预定的 先后次序 运行。比如 A 任务的运行依赖于 B 任务产生的数据。
---------------------------------------------------------------------------
【互斥】：
 	是指散步在不同任务之间的若干程序片断，当某个任务运行其中一个程序片段时，
其它任务就不能运行它们之中的任一程序片段，只能 等 到该任务运行完这个程序片段后才可以运行。
	最基本的场景就是：一个公共资源同一时刻只能被一个进程或线程使用，
多个进程或线程 不能 同时 使用 公共资源。			 */

/***************************
 * 		互斥锁（同步）mutex
 * **************************/
/*	互斥锁 是一个 互斥 的同步对象，意味着 同一时间 有且仅有 一个线程 可以获取它。
互斥锁 可适用于任何一个共享资源 每次 只能被 一个线程 访问 的情况。
互斥锁，想象成一把锁头，只有一把钥匙，谁得到这把钥匙就谁先用。
互斥锁会由并发改为串行，牺牲了运行效率，但保证了数据安全。
互斥锁 只有 两种 状态,即 上锁(lock)和 解锁(unlock)。
---------------------------------------------------------------------------
原子操作：
	所谓的原子操作，取的就是“原子是最小的、不可分割的最小个体”的意义，
	它表示在多个线程访问同一个全局资源的时候，
	能够确保所有其他的线程都不在同一时间内访问相同的资源，
	也就是确保了在同一时刻只有唯一的线程对这个资源进行访问。
	原子操作能够操作最接近机器的指令，因而效率更高。
---------------------------------------------------------------------------
【互斥锁的特点】：
1. 原子性：把一个 互斥量 锁定为一个 原子操作，
这意味着操作系统（或 pthread 函数库）保证了一个线程锁定了一个互斥量，
没有其他线程在同一时间可以成功锁定这个互斥量；
2. 唯一性：如果一个线程锁定了一个互斥量，在它解除锁定之前，没有其他线程可以锁定这个互斥量；
3. 非繁忙等待：如果一个线程已经锁定了一个互斥量，第二个线程又试图去锁定这个互斥量，
则第二个线程将被挂起（不占用任何cpu资源），直到第一个线程解除对这个互斥量的锁定为止，
第二个线程则被唤醒并继续执行，同时锁定这个互斥量。
---------------------------------------------------------------------------
【互斥锁的操作流程如下】：
1. 在访问共享资源后临界区域前，对互斥锁进行加锁；
2. 在访问完成后释放互斥锁导上的锁；
3. 对互斥锁进行加锁后，任何其他试图再次对互斥锁加锁的线程将会被阻塞，直到锁被释放。
4. 销毁互斥锁。互斥锁在使用完毕后，必须要对互斥锁进行销毁，以释放资源。
---------------------------------------------------------------------------
静态方式创建互斥锁：默认属性
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 静态方式创建互斥锁
---------------------------------------------------------------------------
动态方式创建互斥锁：
初始化指定的互斥锁 mutex(互斥体变量)。成功返回0
如果参数 attr 为 NULL（同静态方式），则互斥体变量 mutex 使用 默认属性。
int pthread_mutex_init (pthread_mutex_t *mutex,
						const pthread_mutexattr_t *attr); // 动态方式创建互斥锁
---------------------------------------------------------------------------
对指定的互斥锁加锁，若互斥锁已经上锁，则调用者线程 一直阻塞 直到 互斥锁解锁后再上锁。
int pthread_mutex_lock (pthread_mutex_t *mutex);
---------------------------------------------------------------------------
尝试加锁，不阻塞，失败返回。成功返回0
若指定的互斥锁未加锁，则上锁，返回 0；
若指定的互斥锁已加锁，则函数 直接返回 失败，即 EBUSY。
int pthread_mutex_trylock(pthread_mutex_t *mutex);
---------------------------------------------------------------------------
对指定的互斥锁解锁。成功返回0
int pthread_mutex_unlock(pthread_mutex_t *mutex);
---------------------------------------------------------------------------
销毁指定的互斥锁。互斥锁在使用完毕后，必须要对互斥锁进行销毁，以释放资源。成功返回 0。
int pthread_mutex_destroy(pthread_mutex_t *mutex);
---------------------------------------------------------------------------
当线程试图获取一个已加锁的互斥量时，互斥量原语允许绑定线程阻塞时间。即非阻塞加锁互斥量。
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
 							const struct timespec *abs_timeout);
---------------------------------------------------------------------------
	互斥锁属性: pthread_mutexattr_t *attr
互斥锁的属性在创建锁的时候指定，有四个值可供选择：
1. PTHREAD_MUTEX_TIMED_NP				普通锁
	当一个线程加锁以后，其余请求锁的线程将形成一个等待队列，并在解锁后按优先级获得锁。
 	这种锁策略保证了资源分配的公平性。
2. PTHREAD_MUTEX_RECURSIVE_NP			嵌套锁
 	允许同一个线程对同一个锁 成功获得 多次，并通过 多次 unlock 解锁。
 	如果是不同线程请求，则在加锁线程解锁时重新竞争。
3. PTHREAD_MUTEX_ERRORCHECK_NP			检错锁
 	如果同一个线程请求同一个锁，则返回 EDEADLK，
 	否则与 普通锁(PTHREAD_MUTEX_TIMED_NP) 类型动作相同。
 	这样就保证当不允许多次加锁时不会出现最简单情况下的死锁。
4. PTHREAD_MUTEX_ADAPTIVE_NP			适应锁
 	动作最简单的锁类型，仅等待解锁后重新竞争。
---------------------------------------------------------------------------
在同一进程中的线程，如果加锁后没有解锁，则任何其他线程都无法再获得锁。
普通锁 和 适应锁 类型，解锁者可以是 同进程内 任何线程；
检错锁 则必须由 加锁者解锁 才有效，否则返回 EPERM；
嵌套锁，文档和实现要求必须由加锁者解锁，但实验结果表明并没有这种限制。	 */


/**************** POSIX 线程 可和 c++ 线程 联合使用 *********************/

/**************** c++ 线程 ***************/
#include <thread> // C++ 多线程
#include <atomic> // C++ 多线程
#include <mutex> // 互斥锁
#include <condition_variable> // C++ 多线程
#include <future> // C++ 多线程

/**************** POSIX 线程 *********************/
#include <pthread.h>

#include "../../Linux_IO_error.h" // <cerrno> <error.h> errno 错误对照表
#include <chrono> // std::chrono::system_clock
#include <ctime> // std::time_t, std::tm, std::localtime, std::mktime
//#include <cstring>
#include <cstddef>
#include <string>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <cstdio>
#include <sys/types.h> // POSIX Standard: Primitive System Data Types
#include <unistd.h> // POSIX Standard: Symbolic Constants
#include <iostream>
using namespace std;

/* 输出自定义错误信息 */
void perr(const int &retval, const int &code_line=0, string_view ptr="");
//perr(retval, __LINE__, "info");//输出自定义错误信息

void fun_clear(void *arg); // 线程清理函数

/***********************************************/

void mythread_sell(); //


/*	bash shell 	------------------------
	$ g++ test.cpp -o test
  	$ ./test myfile.txt youfile.txt one.sh
	c++ 		------------------------
		int main(int argc, char **argv)
----------------------------------------
  	c++    		bash shell
  	argc	=	${#argv[@]} # 参数个数
  	argv[0]	=	myfile.txt 	# 参数 : ./当前文件名
  	argv[1]	=	myfile.txt 	# 参数 1
  	argv[2]	=	youfile.txt # 参数 2
  	argv[3]	=	one.sh 	# 参数 3	  	*/
#if 1
int main(int argc, char **argv)
{
	thread::id id_thread = this_thread::get_id(); // 获取当前线程 ID 值
	cout << "线程函数 " << __func__ << " ()(标识符: " << id_thread << ")" << endl;

	mythread_sell(); //

#if 1
	this_thread::sleep_for(std::chrono::milliseconds(10));//线程休眠 10ms
#else
	sleep(1); // 进程休眠 1s
	usleep(10); // 进程休眠 10 ms
#endif

	this_thread::yield();  //让出自己的CPU时间片

	cout << "当前 mian 线程退出" << endl;
	pthread_exit(nullptr); // 当前线程退出，不影响进程

	return 0;
}
#endif


/************ c++ 线程 std::thread 类 成员函数 ***********/
/*	#include <thread> // c++ 线程 std::thread 类 成员函数
---------------------------------------------------------------------------
thread::id thread::get_id(); // 获取 线程标识符，返回对象为 std::thread::id
---------------------------------------------------------------------------
thread::join(); // 阻塞当前线程，直至 子线程 结束（会立即释放资源）
---------------------------------------------------------------------------
thread::detach(); // 线程分离，线程可单独执行（会自动释放资源）
---------------------------------------------------------------------------
bool thread::joinable(); // 判断线程是否可被 join 连接
缺省构造的thread对象、已经 join 或 已经 detach 的thread对象都不是 joinable。
---------------------------------------------------------------------------
pthread_t thread::native_handle(); // 获得与操作系统相关的原生线程句柄
该函数 返回 与 std::thread 具体实现相关的线程句柄。
native_handle() 是连接 thread类 和 操作系统 SDK API 之间的桥梁，
当 thread类 的功能不能满足我们的要求的时候(比如改变某个 线程优先级)，
可以通过 thread类实例的 native_handle()返回值作为参数来调用相关的 pthread函数达到目录。
---------------------------------------------------------------------------
void thread::swap(thread &t); // 线程交换
---------------------------------------------------------------------------
static unsigned int thread::hardware_concurrency(); // 硬件最大的并发线程数目
返回计算机硬件最大的并发线程数目。基本上可以视为处理器的核心数目。		 */

/************ c++ 线程 std::this_thread 类 成员函数 ***********/
/*	#include <thread> // c++ 线程 std::this_thread 类 成员函数
std::this_thread 类 成员函数 用来辅助 std::thread 类，std::async 类。
---------------------------------------------------------------------------
thread::id this_thread::get_id(); // 获取 线程标识符，返回对象为 std::thread::id
---------------------------------------------------------------------------
void this_thread::sleep_for(const chrono::duration<Rep,Period>& rel_time);//线程休眠
阻塞 block 调用该函数的线程 一段时间
---------------------------------------------------------------------------
this_thread::sleep_until(const chrono::duration<Rep,Period>& rel_time);//线程休眠
阻塞 block 调用该函数的线程 直到 指定的时间点 timepoint
---------------------------------------------------------------------------
this_thread::yield(); // 线程让出 CPU
将 处理器 CPU 移交给另一个线程或进程，以允许其他具有相同优先级的线程获得执行的机会。
但是很有可能该线程刚刚回到可执行状态又被再次执行。		 */


/**************** POSIX 线程 可和 c++ 线程 联合使用 *********************/

/* 全局 变量 = 共享资源 */
pthread_mutex_t lock_VAL; // 创建 互斥锁
unsigned int goodsmax=10000;  // 货物个数，售价10元
unsigned int cash=200; // 200元现金
unsigned int goods=goodsmax; // 现存货物个数

/**************** POSIX 线程 可和 c++ 线程 联合使用 *********************/

/* 创建伙计卖货线程 */
void mythread_Buddy_selling() /* 线程函数 可分离 */
{
	thread::id id_thread = this_thread::get_id(); // 获取当前线程 ID 值
	cout << "伙计卖货线程 " << __func__ << " ()(标识符: " << id_thread << ")" << endl;

	int retval; // 返回值
	while (goods)
	{
		retval=pthread_mutex_lock(&lock_VAL); // 互斥锁 上锁
		perr(retval, __LINE__, "pthread_mutex_lock");//输出自定义错误信息

		--goods;        // 卖出1个货物
		cash += 10;     // 卖出1个价值10元的货物

		retval=pthread_mutex_unlock(&lock_VAL); // 互斥锁 解锁
		perr(retval, __LINE__, "pthread_mutex_unlock");//输出自定义错误信息

		this_thread::sleep_for(std::chrono::microseconds (1)); // 线程休眠 1 us
	}
}

void mythread_Boss_reconciliation() /* 线程函数 可分离 */
{
	thread::id id_thread = this_thread::get_id(); // 获取当前线程 ID 值
	cout << "老板对账线程 " << __func__ << " ()(标识符: " << id_thread << ")" << endl;

	int retval; // 返回值
	unsigned int goods_now=0;
	unsigned int cash_now=0;
	unsigned int goods_send=0;
	unsigned int num=0;
	while (true)
	{
		this_thread::sleep_for(std::chrono::milliseconds(100)); // 线程休眠 100ms
		++num;

		retval=pthread_mutex_lock(&lock_VAL);      // 互斥锁 上锁
		perr(retval, __LINE__, "pthread_mutex_lock");//输出自定义错误信息

		goods_now=goods_send=goods;
		cash_now=cash;

		retval=pthread_mutex_unlock(&lock_VAL);     // 互斥锁 解锁
		perr(retval, __LINE__, "pthread_mutex_unlock");//输出自定义错误信息

		cout<< "老板第 "<<num<<" 次对账：" << goodsmax
			<< "个货物，每个售价10元; 起初现金200元。" <<endl;
		cout<<"剩余货物 "<<goods_send<<" 个；\t";
		goods_send=goodsmax-goods_send;
		cout<<"卖出货物 "<<goods_send<<" 个；"<<endl;
		cout<<"应赚现金 "<<(goods_send*10)<<" 元；\t";
		cout<<"应得现金 "<<(goods_send*10+200)<<" 元；"<<endl;
		cout<<"现在现金 "<<cash_now<<" 元；"<<endl;
		if (goods_now==0)break;
	}
	retval=pthread_mutex_destroy(&lock_VAL); // 销毁 互斥锁
	perr(retval, __LINE__, "pthread_mutex_destroy");//输出自定义错误信息
}

/**************** POSIX 线程 可和 c++ 线程 联合使用 *********************/

void mythread_sell()
{
	pthread_mutex_init(&lock_VAL, nullptr); //初始化 互斥锁 动态方式

	/* 创建伙计卖货线程 */
	thread val_thread01(mythread_Buddy_selling); // 定义并初始化
	val_thread01.detach(); // 转换为 可分离线程

	/* 创建老板对账线程 */
	thread val_thread02(mythread_Boss_reconciliation); // 定义并初始化
	val_thread02.detach(); // 转换为 可分离线程
}

/**************** POSIX 线程 可和 c++ 线程 联合使用 *********************/

//perr(retval, __LINE__, "pthread_getattr_np");// 输出自定义错误信息
void perr (const int &retval, const int &code_line, string_view ptr)//输出自定义错误信息
{
//	code_line--;
	if (retval != 0) {
		cerr << "Error:";
		if (!ptr.empty()) {
			cerr << "theme[" << ptr.data() <<"];";
		}
		if (code_line != 0) {
			cerr << "line[" << code_line << "];";
		}
		cerr << "value[" << retval << "]";
		if (retval > 0) {
			if (retval != errno){
				cerr << "{";
				switch (retval) {
					case ESRCH:
						cerr << "No such process";
						break;
					case EINVAL:
						cerr << "Invalid argument"; // 信号无效
						break;
					default:
						break;
				}
				cerr << "};";
			}
		}
		else cerr << ";" << endl;
		cerr << "errno[" << errno << "]";
		if (errno != 0){
			cerr << ":{" << strerror(errno) << "};";//返回描述 errno 代码含义的字符串
		}
		else cerr << ";" << endl;
	}
//	else if(retval > 0)
//	{
//
//	}
//	else
//	{
//
//	}
}

#define data_mode 0
void fun_clear(void *arg) // 线程清理函数
{
	cout << "线程清理函数 "<< __func__ << "()" << endl;
#if data_mode==0 	// 无 参数

#elif data_mode==1 	// 整型 参数
	//	int *p=(long *)(arg);
//	cout << "整型 参数: " << *p << endl;
	cout << "线程清理函数即将退出: " << *((long *)arg) << endl; // 取消点
#elif data_mode==2 	// 字符串 参数
	char *p=(char *)(arg);
	cout << "字符串 参数: " << *p << endl;
#elif data_mode==3 	// 结构体 参数
	auto *mystruct=(struct struct_data *)arg;
	cout << "结构体 参数\tnum: " << mystruct->num << "\tstr:" << mystruct->str << endl;
#elif data_mode==4 	// 共享进程数据
	++count; // 全局变量 -> 共享 进程数据，将会在 主线程 和 子线程 中用到
#endif
//	printf("fun_clear "); // 取消点
}

