#include<linux/module.h>

static int __init kernel_module_init(void)
{
	pr_info("init function \n");
	return 0;
}

static void __exit kernel_module_exit(void)
{
	printk(KERN_INFO, "exit function \n");
}

module_init(kernel_module_init);
module_exit(kernel_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("test module");
