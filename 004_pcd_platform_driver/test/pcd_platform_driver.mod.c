#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xe7cc3b88, "module_layout" },
	{ 0x21ccf193, "class_destroy" },
	{ 0xacda826, "platform_driver_unregister" },
	{ 0x7d7bf51f, "__platform_driver_register" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x5ba17792, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x1ac09638, "device_destroy" },
	{ 0x8d9908e9, "cdev_del" },
	{ 0xb8e50c24, "devm_kfree" },
	{ 0xcd4c837a, "device_create" },
	{ 0xa9833fe3, "cdev_add" },
	{ 0x778e5861, "cdev_init" },
	{ 0xc5850110, "printk" },
	{ 0xfcbac2f7, "devm_kmalloc" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "F77DC56C85A8C38EDA96439");
