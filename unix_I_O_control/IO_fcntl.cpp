
#include "IO_fcntl.h"

/**
 file control
**/


int Fcntl(const int & fd, const int & cmd, const int & arg,
		  const int &line, const char *func)
{
	int	retval = fcntl(fd, cmd, arg);
	err_sys(retval, __FILE__, __func__, __LINE__,
			"Fcntl()\t调用地方: %s(): %d", func, line);
	return retval;
}
