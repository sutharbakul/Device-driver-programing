#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
  printk(KERN_ALERT"======================\n\n");
	printk(KERN_ALERT"Hello module init\n\n");
  printk(KERN_ALERT"======================\n\n");
	return 0;
}

static void hello_exit(void)
{
  printk(KERN_ALERT"======================\n\n");
	printk(KERN_ALERT "Goodbye\n\n");
    printk(KERN_ALERT"======================\n\n");
}

module_init(hello_init);
module_exit(hello_exit);
