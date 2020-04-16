#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>           /* this is the file structure, file open read close */
#include <linux/cdev.h>         /* this is for character device, makes cdev avilable*/
#include <linux/semaphore.h>    /* this is for the semaphore*/
#include <linux/uaccess.h>      /* this is for copy_user vice vers*/

int init_chardev(void);
void cleanup_chardev(void);
static int device_open(struct inode *,struct file *);
static int device_release(struct inode *,struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static loff_t device_lseek(struct file *file, loff_t offset, int orig);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 80

#define BUFFER_SIZE 1024
static char device_buffer[BUFFER_SIZE];

struct cdev *mcdev;     /* this is the name of my char driver that i will be registering*/
struct semaphore sem;
static int Major;
static char *msg_Ptr;
static int Device_Open = 0;
dev_t dev_num;
int ret;

/* inode reffers to the actual file on disk*/
static int device_open(struct inode *inode, struct file *filp)
{
	if(down_interruptible(&sem) != 0) {
		printk(KERN_ALERT "charDev : the device has been opened by some other device, unbale to open lock\n");
		return -1;
	}

	printk(KERN_INFO "charDev : device opened succesfully\n");
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *filp)
{
	up(&sem);
	printk(KERN_INFO "charDev : device has been closed\n");
	return ret;
}

static ssize_t device_read(struct file *fp, char *buff, size_t length, loff_t *ppos)
{
	int maxbytes;           /* maximum bytes that can be read from ppos to BUFFER_SIZE*/
	int bytes_to_read;      /* gives the number of bytes to read*/
	int bytes_read;         /* number of bytes actually read*/
	maxbytes = BUFFER_SIZE - *ppos;

	if(maxbytes < length)
		bytes_to_read = maxbytes;
	else
		bytes_to_read = length;

	if (bytes_to_read == 0)
		printk(KERN_INFO "charDev : Reached the end of the device\n");
	
	bytes_read = bytes_to_read - copy_to_user(buff, device_buffer + *ppos, bytes_to_read);
	printk(KERN_INFO "charDev : device has been read %d\n", bytes_read);

	*ppos += bytes_read;
	printk(KERN_INFO "charDev : device has been read\n");

	return bytes_read;
}

static ssize_t device_write(struct file *fp, const char *buff, size_t length, loff_t *ppos)
{
	int maxbyte;
	int byte_to_write;
	int bytes_writen;

	maxbyte = BUFFER_SIZE - *ppos;
	if(maxbyte < length)
		byte_to_write = maxbyte;
	else
		byte_to_write = length;

	bytes_writen = byte_to_write - copy_from_user(device_buffer + *ppos, buff, byte_to_write);
	printk(KERN_INFO "charDev : device has been written %d\n", bytes_writen);
	*ppos += bytes_writen;
	printk(KERN_INFO "charDev : device has been written\n");

	return bytes_writen;
}

static loff_t device_lseek(struct file *file, loff_t offset, int orig)
{
	loff_t new_pos = 0;
	printk(KERN_INFO "charDev : lseek function in work\n");
	switch (orig) {
	case 0 :        /*seek set*/
					new_pos = offset;
					break;
	case 1 :        /*seek cur*/
					new_pos = file->f_pos + offset;
					break;
	case 2 :        /*seek end*/
					new_pos = BUFFER_SIZE - offset;
					break;
	}
	if (new_pos > BUFFER_SIZE)
					new_pos = BUFFER_SIZE;
	if (new_pos < 0)
					new_pos = 0;
	file->f_pos = new_pos;
	return new_pos;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.write = device_write,
	.read = device_read,
	.release = device_release,
	.llseek = device_lseek
};

int init_chardev(void)
{
	/* we will get the major number dynamically this is recommended please read ldd3*/
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
					printk(KERN_ALERT " charDev : failed to allocate major number\n");
					return ret;
	} else
					printk(KERN_INFO " charDev : mjor number allocated succesful\n");

	Major = MAJOR(dev_num);

	printk(KERN_INFO "charDev : major number of our device is %d\n", Major);
	printk(KERN_INFO "charDev : to use mknod /dev/%s c %d 0\n", DEVICE_NAME, Major);

	mcdev = cdev_alloc(); /* create, allocate and initialize our cdev structure*/
	mcdev->ops = &fops;   /* fops stand for our file operations*/
	mcdev->owner = THIS_MODULE;

	/* we have created and initialized our cdev structure now we need to
	add it to the kernel*/
	ret = cdev_add(mcdev, dev_num, 1);
	if (ret < 0) {
					printk(KERN_ALERT "charDev : device adding to the kerknel failed\n");
					return ret;
	} else
					printk(KERN_INFO "charDev : device additing to the kernel succesful\n");

	sema_init(&sem, 1); /* initial value to one*/

	return SUCCESS;
}

void cleanup_chardev(void)
{
	cdev_del(mcdev); /*removing the structure that we added previously*/
	printk(KERN_INFO " CharDev : removed the mcdev from kernel\n");

	unregister_chrdev_region(dev_num, 1);
	printk(KERN_INFO  " CharDev : unregistered the device numbers\n");
	printk(KERN_ALERT " charDev : character driver is exiting\n");
}

MODULE_AUTHOR("Bakul Suthar");
MODULE_DESCRIPTION("A BASIC CHAR DRIVER");

module_init(init_chardev);
module_exit(cleanup_chardev);