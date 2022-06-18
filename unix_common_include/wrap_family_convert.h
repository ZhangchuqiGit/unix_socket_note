//
// Created by zcq on 2021/4/18.
//

#ifndef __WRAP_FAMILY_CONVERT_H
#define __WRAP_FAMILY_CONVERT_H

#include "zcq_header.h"

#include <sys/socket.h>
#include <linux/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/sockios.h>
#include <initializer_list>
#include <memory>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_ppp.h>
#include <net/if_packet.h>
#include <net/if_slip.h>
#include <net/if_shaper.h>
#include <net/route.h>
#include <net/ethernet.h>
#include <sys/param.h>
#include <netinet/in.h>


int Family_to_level(const int &family);
int family_to_level(const int &family);

int Sockfd_to_family(const int &sock_fd);
int sockfd_to_family(const int &sock_fd);


#endif //__WRAP_FAMILY_CONVERT_H
