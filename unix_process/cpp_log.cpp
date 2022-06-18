//
// Created by zcq on 2021/4/4.
//
/** 自动 启动 服务

sudo gedit /etc/services
添加行：myservice 2000/tcp
添加行：myservice 2000/udp

sudo gedit /etc/inetd.conf
添加行：
# servicename 	type 	proto 	flags 	user 	server_path 	args
myservice	stream	tcp	nowait	root	/media/zcq/fast-office/ZCQ_cpp_code/网络通信/ZCQ-UNIX网络编程-套接字/myservice/myservice
myservice	dgram	udp	wait	root	/media/zcq/fast-office/ZCQ_cpp_code/网络通信/ZCQ-UNIX网络编程-套接字/myservice/myservice

******/


/**	Linux 日记系统 由 系统日志监控程序 syslogd 和 内核日志监控程序 klogd 组成。
 从它们的命名可以看到，这两个监控程序都是 守护程序（daemon），且都注册成了系统服务。
 换句话说，我们可以在目录 /etc/init.d/ 下找到它们对应的执行程序，
 并通过 $ service 命令对它们进行启动，关闭，重启等操作。
 /etc/syslog.conf (ubuntu下为 /etc/rsyslog.d/50-default.conf) 文件是 Linux 日记系统的 配置文件。
---------------------------------------------------------------------------
	Linux C 中提供一套系统日记写入接口，包括三个函数：openlog()，syslog()和 closelog()。
 调用 openlog()是可选择的。如果不调用 openlog()，
 则在第一次调用 syslog()时，自动调用 openlog()。
 调用 closelog()也是可选择的，它只是关闭被用于与 syslog()守护进程通信的描述符。
 修改syslog.conf文件
---------------------------------------------------------------------------
	一般来说，我们希望能够为自己的应用程序指定特定的日记文件。
 这时候，我们就需要修改 /etc/syslog.conf 文件。
 假设我们现在要把调试（debug）日记记录写到文件 /var/log/debug 文件中。
 第一步要做的是，在 syslog.conf 文件添加如下消息规则作为第一条规则：
 	user.debug 		/var/log/debug
 要是添加的新规则生效，
 第二步我们需要重启 syslogd 和 klogd ：
 	service syslog restart ( ubuntu下为 /etc/init.d/rsyslog restart )
---------------------------------------------------------------------------
	本人 /etc/syslog.conf (ubuntu下为 /etc/rsyslog.d/50-default.conf) 配置 文件内容:
---------------------------------------------------------------------------
#  /etc/syslog.conf	Configuration file for inetutils-syslogd.
#
#			For more information see syslog.conf(5) manpage.

#
# First some standard logfiles.  Log by facility.
#

#指出auth.*和authpriv.*消息存放于/var/log/auth.log中，就是关于验证的一些日志信息
auth,authpriv.*		/var/log/auth.log
#除了上面的，其他的日志信息都存放于/var/log/syslog中
*.*;auth,authpriv.none		-/var/log/syslog
cron.*				/var/log/cron.log
daemon.*			-/var/log/daemon.log
# kern 关于内核的日志信息存放于/var/log/syslog中
kern.*				-/var/log/kern.log
# lpr 打印
lpr.*				-/var/log/lpr.log
mail.*				-/var/log/mail.log
user.*				-/var/log/user.log
uucp.*				/var/log/uucp.log

## 比如：mail.*			-/var/log/mail.log 中，
## 存在一个“-”，这表示是使用异步的方式记录, 因为日志一般会比较大。


#
# Logging for the mail system.  Split it up so that
# it is easy to write scripts to parse these files.
#
mail.info			-/var/log/mail.info
mail.warn			-/var/log/mail.warn
mail.err			/var/log/mail.err

# Logging for INN news system
#
news.crit			/var/log/news/news.crit
news.err			/var/log/news/news.err
news.notice			-/var/log/news/news.notice

#
# Some `catch-all' logfiles.
#
*.=debug;\
	auth,authpriv.none;\
	news.none;mail.none;                  local1.none	-/var/log/debug
*.=info;*.=notice;*.=warn;\
	auth,authpriv.none;\
	cron,daemon.none;\
	mail,news.none;                       local1.none	-/var/log/messages

#
# Emergencies are sent to everybody logged in.
#
*.emerg				*

#
# I like to have messages displayed on the console, but only on a virtual
# console I usually leave idle.
#
#daemon,mail.*;\
#	news.=crit;news.=err;news.=notice;\
#	*.=debug;*.=info;\
#	*.=notice;*.=warn	/dev/tty8

# The named pipe /dev/xconsole is for the `xconsole' utility.  To use it,
# you must invoke `xconsole' with the `-file' option:
#
#    $ xconsole -file /dev/xconsole [...]
#
# NOTE: adjust the list below, or you'll go crazy if you have a reasonably
#      busy site..
#
daemon.*;mail.*;\
	news.crit;news.err;news.notice;\
	*.=debug;*.=info;\
	*.=notice;*.=warn	|/dev/xconsole

########################################################################
# local1.none 使得设备local1的日志不记录在messages文件里，
# 将设备 local1 的所有级别的信息都记录在 userlog 文件里，
# local1.*   /var/log/userlog
local1.* 	/media/zcq/fast-office/ZCQ_cpp_code/网络通信/ZCQ-UNIX网络编程-套接字/myservice/local1.log
local1.debug 	/media/zcq/fast-office/ZCQ_cpp_code/网络通信/ZCQ-UNIX网络编程-套接字/myservice/local1.log
local1.info 	/media/zcq/fast-office/ZCQ_cpp_code/网络通信/ZCQ-UNIX网络编程-套接字/myservice/local1.log
local1.warn 	/media/zcq/fast-office/ZCQ_cpp_code/网络通信/ZCQ-UNIX网络编程-套接字/myservice/local1.log
local1.err 	-/media/zcq/fast-office/ZCQ_cpp_code/网络通信/ZCQ-UNIX网络编程-套接字/myservice/local1.log
## 存在一个“-”，这表示是使用异步的方式记录, 因为日志一般会比较大。

########################################################################
#debug      –有调式信息的，日志信息最多
#info       –一般信息的日志，最常用
#notice     –最具有重要性的普通条件的信息
#warning    –警告级别
#err        –错误级别，阻止某个功能或者模块不能正常工作的信息
#crit       –严重级别，阻止整个系统或者整个软件不能正常工作的信息
#alert      –需要立刻修改的信息
#emerg      –内核崩溃等严重信息
#none       –什么都不记录
#从上到下，级别从低到高，记录的信息越来越少
---------------------------------------------------------------------------
	 */


/** C++ 实现 log 日志系统 **/
/*
1.log日志的作用
 	在软件开发周期中，不管是前台还是后台，系统一般会采用一个持久化的日志系统来记录运行情况。
在代码中嵌入log代码信息，主要记录下列信息：
	（1）记录系统运行异常信息。
	（2）记录系统运行状态信息。
	（3）记录系统运行性能指标。
	通过对上述信息分析和诊断，我们能采取正确的手段来提高系统质量和系统性能。
由此可见log日志在系统中的重要地位和存在的必要性。
---------------------------------------------------------------------------
2.log日志的类型与级别
2.1日志的类型
	安全类信息：记录系统边界交互行为和信息；
	业务类信息：记录系统内部业务处理行为和信息；
	性能类信息：记录系统硬件对业务处理的支撑能力。
2.2日志的级别：
	ERROR（错误）：此信息输出后，主体系统核心模块不能正常工作，需要修复才能正常工作。
	WARN（警告）：此信息输出后，系统一般模块存在问题，不影响系统运行。
	INFO（通知）：此信息输出后，主要是记录系统运行状态等关联信息。
	DEBUG（调试）：最细粒度的输出，除却上面各种情况后，你希望输出的相关信息，都可以在这里输出。
	TRACE（跟踪）：最细粒度的输出，除却上面各种情况后，你希望输出的相关信息，都可以在这里输出。
---------------------------------------------------------------------------


 */

/********** 生成日志消息
 * syslog是Linux系统默认的日志守护进程。
 * 默认的主配置文件和辅助配置文件分别是
 * /etc/syslog.conf (ubuntu下为 /etc/rsyslog.d/50-default.conf)
 * 和 /etc/sysconfig/syslog 文件。
 * 通常，syslog 接受来自系统的各种功能的信息，每个信息都包括重要级。
 * /etc/rsyslog.conf 文件通知 syslogd 如何根据设备和信息重要级别来报告信息。
 * syslog是UNIX系统中提供的一种日志记录方法(RFC3164)，syslog本身是一个服务器，
 * 程序中凡是使用syslog记录的信息都会发送到该服务器，服务器根据配置决定此信息是否记录，
 * 是记录到磁盘文件还是其他地方，这样使系统内所有应用程序都能以统一的方式记录日志，
 * 为系统日志的统一审计提供了方便。  */
/*	std::string buf= "zcq";
   openlog("test_syslog", LOG_CONS | LOG_PID, LOG_USER);
   syslog(LOG_USER | LOG_INFO,
	   "syslog test message generated in program %s\n", buf.data());
   closelog();    */
/**	void openlog (char *ident, int option, int facility)
   	void syslog (int priority, const char *format, ...)使用字符串和选项参数生成日志消息
   	void closelog (void)
   	调用 openlog()是可选择的。如果不调用 openlog()，则在第一次调用 syslog()时，
   	自动调用 openlog()。调用 closelog()也是可选择的，
   	它只是关闭被用于与 syslog()守护进程通信的描述符。   **/
//	priority = level /* 日志级别 */  |  facility;
//	facility 参数用来指定何种程式在记录讯息，这可让设定档来设定何种讯息如何处理
/* 	#include <syslog.h>
	格式基本是：timestamp hostname ident[pid]：log message
列如:	每运行一次，程序将往 /var/log/messages 添加一条如下的记录：
	Apr  5 09:56:54 zcq test_syslog[12345]: syslog test message generated in program zcq
---------------------------------------------------------------------------
参数 ident : 一般设成程序的名字
---------------------------------------------------------------------------
参数 option :
	LOG_CONS : 如果送到system logger时发生问题，直接写入系统 console 控制台。
	LOG_ODELAY : 延迟打开直到第一个 syslog()（默认）
	LOG_NDELAY : 不延迟，立即开启连接(通常，连接是在第一次写入讯息时 syslog()才打开的)。
	LOG_PERROR : 将讯息也同时送到 stderr
	LOG_PID : 将 PID含入所有讯息中，记录每个消息的 [pid]
---------------------------------------------------------------------------
参数 priority = level | facility;
---------------------------------------------------------------------------
参数 level 日志(消息)级别 :
	LOG_EMERG : 系统不可用
	LOG_ALERT : 报警，需要立即采取动作
	LOG_CRIT : 非常严重的情况
	LOG_ERR : 错误
	LOG_WARNING : 警告
	LOG_NOTICE : 通知
	LOG_INFO : 信息
	LOG_DEBUG : 调试
---------------------------------------------------------------------------
参数 facility 日志(消息)类型 : 指定何种程式在记录讯息，这可让设定档来设定何种讯息如何处理
	LOG_AUTH : 安全/授权讯息(别用这个，请改用 LOG_AUTHPRIV)	/var/log/auth.log
	LOG_AUTHPRIV : 安全/授权讯息(私用)			/var/log/
	LOG_CRON : cron 时间守护进程专用(cron 及 at) 	/var/log/cron.log
	LOG_DAEMON : daemon 系统守护进程				/var/log/daemon.log
	LOG_FTP : FTP 系统守护进程					/var/log/
	LOG_KERN : 内核消息							/var/log/kern.log
	LOG_LOCAL0 到 LOG_LOCAL7 :   用来定义本地策略（本地使用）	/var/log/
	LOG_LPR : line printer 行式系统				/var/log/
	LOG_MAIL : mail 邮件系统						/var/log/
	LOG_NEWS : 网络新闻系统						/var/log/
	LOG_SYSLOG : syslog 内部产生的消息			/var/log/syslog
	LOG_USER : 任意的用户级消息（默认）			/var/log/user.log
	LOG_UUCP : UUCP 系统						/var/log/
---------------------------------------------------------------------------
参数 facility 的 ID（上面对应的数值）与 名字 的对应关系如下：
    { "auth", 		LOG_AUTH },
    { "authpriv", 	LOG_AUTHPRIV },
    { "cron", 		LOG_CRON },
    { "daemon", 	LOG_DAEMON },
    { "ftp", 		LOG_FTP },
    { "kern", 		LOG_KERN },
    { "lpr", 		LOG_LPR },
    { "mail", 		LOG_MAIL },
    { "mark", 		INTERNAL_MARK },		内部/
	{ "news", 		LOG_NEWS },
	{ "security", 	LOG_AUTH },		已淘汰
	{ "syslog", 	LOG_SYSLOG },
	{ "user", 		LOG_USER },
	{ "uucp", 		LOG_UUCP },
	{ "local0", LOG_LOCAL0 },
	{ "local1", LOG_LOCAL1 },
	{ "local2", LOG_LOCAL2 },
	{ "local3", LOG_LOCAL3 },
	{ "local4", LOG_LOCAL4 },
	{ "local5", LOG_LOCAL5 },
	{ "local6", LOG_LOCAL6 },
	{ "local7", LOG_LOCAL7 },
	{ NULL, -1 }
 这个对应关系作用是是将 syslog() 系统调用中 facility ID
 和 syslog.conf 文件中的配置选项对应起来                        */

/** syslog 配置文件 **/
/*
	/var/log/secure               ##系统登陆日志
	/var/log/cron                  ##定时任务日志
	/var/log/maillog             ##邮件日志
	/var/log/boot.log           ##系统启动日志
 	/var/log/lastlog : 记录每个使用者最近签入系统的时间
	/var/run/utmp : 记录每个使用者签入系统的时间, who, users, finger 等指令会查这个档案.
	/var/log/wtmp : 记录每个使用者签入及签出的时间, last 这个指令会查这个档案.
 		这个档案也记录 shutdown 及 reboot 的动作.
	/var/log/secure : 登录系统的信息
	/var/log/maillog : 记录 sendmail 及 pop 等相关讯息.
	/var/log/cron : 记录 crontab 的相关讯息 ，定时器的信息
	/var/log/dmesg : /bin/dmesg 会将这个档案显示出来, 它是开机时的画面讯息.
	/var/log/xferlog : 记录那些位址来 ftp 拿取那些档案.
	/var/log/messages : 系统大部份的讯息皆记录在此, 包括 login, check password ,
		failed login, ftp, su 等.
---------------------------------------------------------------------------
配置 /etc/syslog.conf 文件格式基本如下：
日志设备.(连接符号)日志级别   日志处理方式(action)
local1.err 	-/media/zcq/fast-office/ZCQ_cpp_code/网络通信
 			/ZCQ-UNIX网络编程-套接字/myservice/local1.log
## 存在一个“-”，这表示是使用异步的方式记录, 因为日志一般会比较大。
---------------------------------------------------------------------------
日志设备(可以理解为日志类型):
auth       –pam产生的日志
authpriv   –ssh,ftp等登录信息的验证信息
cron       –时间任务相关
kern       –内核
lpr        –打印
mail       –邮件
mark(syslog)–rsyslog服务内部的信息,时间标识
news       –新闻组
user        –用户程序产生的相关信息
uucp       –unix to unix copy, unix主机之间相关的通讯
local 1~7  –自定义的日志设备
---------------------------------------------------------------------------
日志级别:
debug      –有调式信息的，日志信息最多
info       –一般信息的日志，最常用
notice     –最具有重要性的普通条件的信息
warning    –警告级别
err        –错误级别，阻止某个功能或者模块不能正常工作的信息
crit       –严重级别，阻止整个系统或者整个软件不能正常工作的信息
alert      –需要立刻修改的信息
emerg      –内核崩溃等严重信息
none       –什么都不记录
从上到下，级别从低到高，记录的信息越来越少   */


#include "zcq_header.h"

/** 格式基本是：timestamp hostname ident[pid]：log message **/
/* 	每运行一次，程序将往 /var/log/messages 添加一条如下的记录：
	Apr  5 09:56:54 zcq test_syslog[12345]: syslog test message generated in program zcq
 其中 ident 就是我们调用 openlog()是指定的 "test_syslog"，
 而之所以会打印出[12345]是 openlog()的 option 参数中指定了 LOG_PID
 */
void fff()
{
	std::string buf= "zcq";
	openlog("test_syslog", LOG_CONS | LOG_PID,
			LOG_USER /* 任意的用户级消息（默认）/var/log/user.log */ );
	syslog(LOG_USER | LOG_INFO,
		   "syslog test message generated in program %s\n", buf.data());
	closelog();
}

/* 返回一个包含当前目录名称的字符串。如果设置了环境变量"PWD"，并且其值正确，那么将使用该值
	/media/zcq/fast-office/ZCQ_cpp_code/网络通信
	/ZCQ-UNIX网络编程-套接字/cmake-build-debug        */
//	std::string buf = get_current_dir_name();
//	mymsg(buf);

/* 生成自定义消息 */
void mymsg(std::string_view sbuf)
{
	char filename[] = "../daemon.txt"; // 文件名，相对路径(../)
	int fd = open(filename,O_CREAT|O_WRONLY|O_APPEND,0777); // 创建文件 <fcntl.h>
	if(fd < 0) exit(-7);

	char buf[128];
	bzero(buf, sizeof(buf));
/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
	time_t ticks = time(nullptr);
	snprintf(buf, sizeof(buf), "[%.24s]: ", ctime(&ticks));

	std::string ssbuf = buf;
	ssbuf += sbuf;
	write(fd, ssbuf.data(), ssbuf.size());
	close(fd);
}