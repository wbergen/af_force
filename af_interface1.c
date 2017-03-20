// Following source used as template
// http://www.thelinuxdaily.com/2010/05/grab-raw-keyboard-input-from-event-device-node-devinputevent/

/*  Will Bergen - AF_Interface
*   Userland interface for our lkm.  Reads raw keyboard input, looking for trigger sequences
*   Communicates with lkm via ioctl device.  Communicates arm/disarm instructions to lkm
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

// Kern Com:
#include <sys/ioctl.h>

#include "af_common.h"
 
void handler (int sig)
{
  printf ("exiting...(%d)\n", sig);
  exit (0);
}
 
void perror_exit (char *error)
{
  perror (error);
  handler (9);
}

void send_k_message(char *msg)
{
  // printf("Sending %s to k...\n", msg);
  int fd;
  fd = open(DEVICE_PATH, O_RDWR);
  ioctl(fd, IOCTL_CMD, msg);
  // printf("ioctl executed\n");
  close(fd);
}
 
int main (int argc, char *argv[])
{
  struct input_event ev[64];
  int fd, rd, value, size = sizeof (struct input_event);
  // char name[256] = "Unknown";
  char *device = NULL;
 
  // Setup check
  if (argv[1] == NULL){
    printf("Using %s\nPass your device as arg1 to use it instead.\n", KEYBOARD_INPUT);
    device = KEYBOARD_INPUT;
  } else {
    printf("Using %s...\n", argv[1]);
    device = argv[1];
  }


 
  // Check For Root:
  if ((getuid()) != 0){
    printf("NOT ROOT\n");
    exit(0);
  }

  //Open Device
  if ((fd = open (device, O_RDONLY)) == -1)
    printf ("%s is not a vaild device\n", device);
    // Probably indicates no root.. wait wut?
 
  //Print Device Name (keep this, nice)
  // ioctl (fd, EVIOCGNAME (sizeof (name)), name);
  // printf ("Reading From : %s (%s)\n", device, name);
 

  // Bookkeeping vars:
  int q[6] = {0,0,0,0,0,0};
  int q2[6] = {0,0,0,0,0,0};
  int q_correct[6] = {31,18,46,19,18,20};
  int q2_correct[6] = {31,50,24,24,20,35};
  int ct = 0;
  int ct2 = 0;
  while (1){
      if ((rd = read (fd, ev, size * 64)) < size)
          perror_exit ("read()");      
 
      value = ev[0].value;


      // Need to factor this LOL
      // Keyboard down:
      if (ev[1].type == 1 && ev[1].value == 1) {
        // printf("-- value: %i\n", value);
        
        // SECRET:
        if (value == 31 && ct == 0) {
          // base case:
          ct = 1;
          q[0]=value;
        } else if ((int)value == q_correct[ct] && ct < 6){
          // fill array if we have 31 (s) in 1st pos.
          q[ct]=value;
          ct++;
        } else {
          ct = 0;
          memset(&q[0], 0, sizeof(q));
        }

        // Check for success:
        if (ct == 6 && memcmp(&q, &q_correct, sizeof(q)) == 0) {
          // React to success + rest vars:
          printf("%s\n", "Winner! :)");
          send_k_message("arm");
          ct = 0;
          memset(&q[0], 0, sizeof(q));
        }


        // SMOOTH:
        if (value == 31 && ct2 == 0) {
          // base case:
          ct2 = 1;
          q2[0]=value;
        } else if ((int)value == q2_correct[ct2] && ct2 < 6){
          // fill array if we have 31 (s) in 1st pos.
          q2[ct2]=value;
          ct2++;
        } else {
          ct2 = 0;
          memset(&q2[0], 0, sizeof(q2));
        }

        // Check for success:
        if (ct2 == 6 && memcmp(&q2, &q2_correct, sizeof(q2)) == 0) {
          // React to success + rest vars:
          printf("%s\n", "Winner! :)");
          send_k_message("disarm");
          ct2 = 0;
          memset(&q2[0], 0, sizeof(q2));
        }

        // Pretty q printing for debuggin:
      //   int i = 0;
      //   printf("q2: ");
      //   for (i = 0; i < 6; ++i)
      //   {
      //     printf("%i ", q2[i]);
      //   }
      //   printf("\n");

      }

  }
 
  return 0;
}