#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include "af_common.h"

// Mouse:
#include <linux/keyboard.h>
#include <linux/jiffies.h>
#include <linux/timer.h>


MODULE_LICENSE("GPL");

/*  state 0: disarmed. default 
    state 1: armed.         */

int state = 0;
static struct timer_list countdown_timer;


static int device_open(struct inode *inode, struct file *file)
{
    // printk(KERN_INFO "af_force: file Opened");
    return 0;
}

int key_notify(struct notifier_block *nblock, unsigned long code, void *_param) {
    struct keyboard_notifier_param *param = _param;

    int ret = NOTIFY_OK;

    if (code == KBD_KEYCODE) {
        if(param->down){
            mod_timer(&countdown_timer, jiffies + 5 * HZ);
            // printk(KERN_DEBUG "af_force: Resetting timer\n");
        }        
    }
    return ret;
}

// Correct sig:
// long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
static long device_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
    char *msg = (char *)args;
    switch(cmd)
    {   
        case IOCTL_CMD:
            printk(KERN_INFO "af_force: got msg %s\n", (char *)args);

            // Arm/Disarm Logic:
            // strcmp 0 is match
            // printk("strcmp result: %i\n", strcmp(msg, "arm"));
            if (strcmp(msg, "arm") == 0) {
                // arm
                printk(KERN_INFO "af_force: arming...\n");
                state = 1;

            } else if (strcmp(msg, "disarm") == 0) {
                // disarm
                printk(KERN_INFO "af_force: disarming...\n");
                state = 0;
            }

            break;
    }
    return 0;
}

void detonate (unsigned long data)
{
    if (state == 1 ) {
        printk(KERN_DEBUG "af_force: Kaboom!\n"); 
    } else {
        printk(KERN_DEBUG "af_force: It's quiet...\n"); 
    }
}

static int device_release(struct inode *inode, struct file *file)
{
    // printk(KERN_INFO "af_force: file Released");
    return 0;
}

static struct notifier_block nb = {
    .notifier_call = key_notify
};

static struct class *my_class;

static struct file_operations fops={
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl
};

static int hello_init(void)
{   
    // Module Registration:
    major_no = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_INFO "af_force: Major_no is %d\n", major_no);

    my_class = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(my_class, NULL, MKDEV(major_no,0), NULL, DEVICE_NAME);
    printk(KERN_INFO "af_force: Device Initialized in kernel.\n");

    // Timer setup:
    setup_timer(&countdown_timer, detonate, 0);
    mod_timer(&countdown_timer, jiffies + 5 * HZ);   
    printk( "af_force: Starting timer to fire in 5s (%ld)\n", jiffies );

    register_keyboard_notifier(&nb);
    return 0;
}




static void hello_exit(void)
{
    // Timer Deconstruction:
    del_timer(&countdown_timer);
    unregister_keyboard_notifier(&nb);

    // Module Deregistration:
    device_destroy(my_class,MKDEV(major_no,0));
    class_unregister(my_class);
    class_destroy(my_class);
    unregister_chrdev(major_no, DEVICE_NAME);
    printk(KERN_INFO "af_force: Closed & Unregistered.\n");
}



module_init(hello_init);
module_exit(hello_exit);