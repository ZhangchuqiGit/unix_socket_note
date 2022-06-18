//
// Created by zcq on 2021/4/13.
//

#ifndef __IO_IOCTL_SOCKET_H
#define __IO_IOCTL_SOCKET_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <linux/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
//#include <net/if.h>
#include <linux/sockios.h>
#include <initializer_list>
#include <memory>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_ppp.h>
#include <net/if_packet.h>
#include <net/if_slip.h>
#include <net/if_shaper.h>
#include <net/route.h>
#include <net/ethernet.h>
#include <sys/param.h>

/* 对底层内核的控制 */
int IOctl(const int &fd, const ulong &request, const void *arg,
		  const int &line=0, const char *func=" ");

/*************************************************************************/
void ioctl_test();

/******************************** 程序01 *********************************/
/* 程序01：获取 网口名的 IP地址 */
void test01_GetNetCardIP();

/******************************** 程序02 *********************************/
/* 程序02：简单 shell ifconfig 查询功能 */
void test02_ifconfig();

/******************************** 程序03 *********************************/
/* 返回网口总数 */
int interface_number(const int &sock_fd);

std::unique_ptr<struct if_info []> Get_if_info(const int &family);
std::unique_ptr<struct if_info []> get_if_info(const int &sock_fd, const int &family);

/* 程序03：进阶 shell ifconfig 查询功能 */
void test03_ifconfig();

//#define	IFI_HADDR 	8			// allow for 64-bit EUI-64 in future
#define	IFI_alias 	1			// if_addr is an alias

struct if_info                                  /** $ ifconfig **/
{
	/** struct ifreq : union ifr_ifrn **/
	char if_name[IFNAMSIZ];		/* 网口名 Interface name, e.g. "enp3s0","lo" */

	/** struct ifreq : union ifr_ifru **/
	struct sockaddr if_addr;	/* 网卡IP地址 primary address */
	struct sockaddr if_dstaddr;	/* 目标地址(点到点) destination address */
	struct sockaddr if_brdaddr;	/* 广播地址 broadcast address */
	struct sockaddr if_netmask;	/* 子网掩码 interface network mask */
	struct sockaddr if_hwaddr;	/* 硬件地址(MAC机器码) hardware address */
	short if_flags;		/* 网口状态标志信息 IFF_xxx constants from <net/if.h> */
	int   if_metric;	/* 接口测度 metric */
	int   if_index;		/* 接口索引 interface index */
	int   if_bandwidth;	/* link bandwidth */
	int   if_quelen;	/* queue length */
	int   if_mtu;		/* interface MTU */
	struct ifmap if_map;		/* 网卡 device map */
	char ifru_slave[IFNAMSIZ];	/* 占位符 slave device */
	char ifru_newname[IFNAMSIZ];/* New name */
//	__caddr_t ifru_data;		/* 用户数据	*/

	char if_alias[IFNAMSIZ]; 			// 网口别名
	short if_alias_flag;				// our own IFI_xxx flags
//	struct sockaddr_dl if_dl;			// Link-Level sockaddr
	struct if_info *if_next = nullptr;	// next of if_info structures
};

/******************************** 程序04 *********************************/
/* 程序04：使用 ARP高速缓存 获取以太网/主机硬件地址(MAC机器码) */
void test04_ARP_mac();

/******************************** 程序05 *********************************/
/* 程序05：增加/删除 路由表条目 */
void test05_route();

/* IPv4 add/del route item in route table */
int inet_setroute(std::vector<std::string> &vec_str);

/* IPv6 add/del route item in route table */
int inet6_setroute(std::vector<std::string> &vec_str);


#endif //__IO_IOCTL_SOCKET_H
