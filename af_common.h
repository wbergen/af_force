#include <linux/ioctl.h>
#include <linux/kdev_t.h> /* for MKDEV */

#define DEVICE_NAME "af_dev"
#define DEVICE_PATH "/dev/af_dev"
#define WRITE 0
static int major_no;
#define MAGIC_NO '4'

/*
AF: AF technique to use:
0: Do nothing
1: Call Scrambler
2: System Request Shutdown
3: Zero cr3
*/
#define AF 0

// Hook Read:
#define INTERCEPT_ON 1

/* 
 * Set the message of the device driver 
 */
#define IOCTL_CMD _IOR(MAGIC_NO, 0, char *)

#define KEYBOARD_INPUT "/dev/input/event2"