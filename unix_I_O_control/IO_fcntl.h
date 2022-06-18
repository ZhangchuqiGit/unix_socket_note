//
// Created by zcq on 2021/4/10.
//

#ifndef __IO_FCNTL_H
#define __IO_FCNTL_H

#include "zcq_header.h"

#include <fcntl.h>

int Fcntl(const int & fd, const int & cmd, const int & arg,
		  const int &line=0, const char *func=" ");


#endif //__IO_FCNTL_H
