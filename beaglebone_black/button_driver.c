#include <linux/module.h>
#include <linux/interrupt.h>

static int __init button_init(void)
{
	return 0;
}

static void __exit button_exit(void)
{

}

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("BBB Button Add");
