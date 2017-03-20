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
  int q_arm[5] = {0,0,0,0,0};
  int q_disarm[8] = {0,0,0,0,0,0,0,0};
  int q_hide[6] = {0,0,0,0,0,0};
  int q_unhide[8] = {0,0,0,0,0,0,0,0};

  int q_arm_correct[5] = {30,33,30,19,50}; // afarm
  int q_disarm_correct[8] = {30,33,32,23,31,30,19,50}; // afdisarm
  int q_hide_correct[6] = {30,33,35,23,32,18}; // afhide
  int q_unhide_correct[8] = {30,33,22,49,35,23,32,18}; // afunhide

  int ct_arm = 0;
  int ct_disarm = 0;
  int ct_hide = 0;
  int ct_unhide = 0;

  // int *input_queue = malloc(sizeof(int)*10);
  // int q_ct = 0;


  while (1){
      if ((rd = read (fd, ev, size * 64)) < size)
          perror_exit ("read()");      
 
      value = ev[0].value;


      // Need to factor this LOL
      // This is embarassing...
      // What we need is a queue, which can be compared per step to the "xxx_correct" arrays, but...
      // Simulate a q of 10:

      // printf("len of q_disarm: %i\n", sizeof(q_disarm_correct)/sizeof(int*));



      // Keyboard down:
      if (ev[1].type == 1 && ev[1].value == 1) {
        // printf("-- value: %i\n", value);

        // add each new char to the input_queue
        // IGNORE THIS
        // if (q_ct < 10) {
        //   int j= 0;
        //   for (int i = 0; i < q_ct; ++i)
        //   {
        //     /* code */
        //   }
        //   memset(((int*)input_queue)+q_ct,value,1);
        // } else { }
        //   // make backup
        //   int *q_temp = malloc(sizeof(int)*11);
        //   int j = 0;

        //   memcpy(q_temp+1, input_queue, 10);
        //   // memset()
        // q_ct++;



        // AFARM:
        if (value == q_arm_correct[0] && ct_arm == 0) {
          // base case:
          ct_arm = 1;
          q_arm[0]=value;
        } else if ((int)value == q_arm_correct[ct_arm] && ct_arm < 5){
          // fill array if we have 31 (s) in 1st pos.
          q_arm[ct_arm]=value;
          ct_arm++;
        } else {
          ct_arm = 0;
          memset(&q_arm[0], 0, sizeof(q_arm));
        }

        // Check for success:
        if (ct_arm == 5 && memcmp(&q_arm, &q_arm_correct, sizeof(q_arm)) == 0) {
          // React to success + rest vars:
          printf("%s\n", "Winner! :)");
          send_k_message("arm");
          ct_arm = 0;
          memset(&q_arm[0], 0, sizeof(q_arm));
        }


        // AFDISARM:
        if (value == q_disarm_correct[0] && ct_disarm == 0) {
          // base case:
          ct_disarm = 1;
          q_disarm[0]=value;
        } else if ((int)value == q_disarm_correct[ct_disarm] && ct_disarm < 8){
          // fill array if we have 31 (s) in 1st pos.
          q_disarm[ct_disarm]=value;
          ct_disarm++;
        } else {
          ct_disarm = 0;
          memset(&q_disarm[0], 0, sizeof(q_disarm));
        }

        // Check for success:
        if (ct_disarm == 8 && memcmp(&q_disarm, &q_disarm_correct, sizeof(q_disarm)) == 0) {
          // React to success + rest vars:
          printf("%s\n", "Winner! :)");
          send_k_message("disarm");
          ct_disarm = 0;
          memset(&q_disarm[0], 0, sizeof(q_disarm));
        }


        // AFHIDE:
        if (value == q_hide_correct[0] && ct_hide == 0) {
          // base case:
          ct_hide = 1;
          q_hide[0]=value;
        } else if ((int)value == q_hide_correct[ct_hide] && ct_hide < 6){
          // fill array if we have 31 (s) in 1st pos
          q_hide[ct_hide]=value;
          ct_hide++;
        } else {
          ct_hide = 0;
          memset(&q_hide[0], 0, sizeof(q_hide));
        }

        // Check for success:
        if (ct_hide == 6 && memcmp(&q_hide, &q_hide_correct, sizeof(q_hide)) == 0) {
          // React to success + rest vars:
          printf("%s\n", "Winner! :)");
          send_k_message("hide");
          ct_hide = 0;
          memset(&q_hide[0], 0, sizeof(q_hide));
        }


        // AFUNHIDE:
        if (value == q_unhide_correct[0] && ct_unhide == 0) {
          // base case:
          ct_unhide = 1;
          q_unhide[0]=value;
        } else if ((int)value == q_unhide_correct[ct_unhide] && ct_unhide < 8){
          // fill array if we have 31 (s) in 1st pos
          q_unhide[ct_unhide]=value;
          ct_unhide++;
        } else {
          ct_unhide = 0;
          memset(&q_unhide[0], 0, sizeof(q_unhide));
        }

        // Check for success:
        if (ct_unhide == 8 && memcmp(&q_unhide, &q_unhide_correct, sizeof(q_unhide)) == 0) {
          // React to success + rest vars:
          printf("%s\n", "Winner! :)");
          send_k_message("unhide");
          ct_unhide = 0;
          memset(&q_unhide[0], 0, sizeof(q_unhide));
        }




      //   // Pretty q printing for debuggin:
      //   int i = 0;
      //   printf("q_unhide");
      //   for (i = 0; i < 8; ++i)
      //   {
      //     printf("%i ", q_unhide[i]);
      //   }
      //   printf("\n");

      }

  }
 
  return 0;
}


/**
"afhide"
-- value: 30
a-- value: 33
f-- value: 35
h-- value: 23
i-- value: 32
d-- value: 18

"afunhide"
-- value: 30
a-- value: 33
f-- value: 22
u-- value: 49
n-- value: 35
h-- value: 23
i-- value: 32
d-- value: 18
e

*//*
"afarm"
-- value: 30
a-- value: 33
f-- value: 30
a-- value: 19
r-- value: 50
m


"afdisarm"
-- value: 30
a-- value: 33
f-- value: 32
d-- value: 23
i-- value: 31
s-- value: 30
a-- value: 19
r-- value: 50
m
*/