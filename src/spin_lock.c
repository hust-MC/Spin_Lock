#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

#include <asm/uaccess.h>
#include <asm/spinlock.h>
//#include <stdio.h>
//#include <stdlib.h>

#define DEVICE_NAME "spin_lock"

static char *data = "read\n";
static DEFINE_SPINLOCK(lock);
static char read_flag = 0;

static ssize_t spinLock_read(struct file *file, char __user *buf, size_t count,
		loff_t *ppos)
{
	if (!read_flag)
	{

		int size = strlen(data);
		if (copy_to_user(buf, (void*) data, size))
		{
			return -EINVAL;
		}
		else
		{
			if (spin_trylock(&lock))
			{
				//在此做个延时，用于模拟临界区代码执行
//				mdelay(10000);
				volatile long long a = 0;
				int i, j;
				for (i = 0; i < 100000; i++)
					for (j = 0; j < 100000; j++)
						a++;
				spin_unlock(&lock);
			}
			else
			{
				return -EBUSY;
			}
			read_flag = 1;
			return size;
		}
	}
	else
	{
		read_flag = 0;
		printk("read_flag = 0\n");
		return 0;
	}
}

static ssize_t spinLock_write(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos)
{
//	char *cmd = (char *) malloc(sizeof(char *));
	char cmd[10] = { 0 };
	if (copy_from_user(cmd, (void*) buf, count))
	{
		return -EINVAL;
	}
	else
	{
		if (strcmp("lock\n", cmd) == 0)
		{
			spin_lock(&lock);
			mdelay(10000);
			spin_unlock(&lock);
		}
		else if (strcmp("trylock\n", cmd) == 0)
		{
			if (spin_trylock(&lock))
			{
				printk("spin_lock is available\n");
				spin_unlock(&lock);
			}
			else
			{
				printk("spin_lock is busy\n");
				return -EBUSY;
			}
		}
		return count;
	}
}

static struct file_operations dev_fops = { .owner = THIS_MODULE, .read =
		spinLock_read, .write = spinLock_write };

static struct miscdevice misc = { .minor = MISC_DYNAMIC_MINOR, .name =
DEVICE_NAME, .fops = &dev_fops };

static int spinLock_init(void)
{
	printk("spinLock_init success\n");

	int ret;
	ret = misc_register(&misc);
	return ret;
}

static void spinLock_exit(void)
{
	printk("spinLock_exit success\n");
	misc_deregister(&misc);
}

module_init(spinLock_init);
module_exit(spinLock_exit);

MODULE_LICENSE("GPL");
