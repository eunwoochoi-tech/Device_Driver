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
	{ 0xc5850110, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "AAE940AEA3E3D40BB4075A1");
