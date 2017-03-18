// Form from:
// http://askubuntu.com/questions/54239/problem-with-ioctl-in-a-simple-kernel-module

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "af_common.h"

int main()
{
	int fd;
	char * msg = "yahoooo";
	fd = open(DEVICE_PATH, O_RDWR);
	ioctl(fd, IOCTL_CMD, msg);
	printf("ioctl executed\n");
	close(fd);
	return 0;
}