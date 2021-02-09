#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf8cdd757, "module_layout" },
	{ 0x709c4323, "__platform_driver_register" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xe42dbab4, "__class_create" },
	{ 0x27e1a049, "printk" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xd2b09ce5, "__kmalloc" },
	{ 0xf86c8d03, "kmem_cache_alloc_trace" },
	{ 0x8200293d, "kmalloc_caches" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "2B9F5B02E50028DED584E00");
MODULE_INFO(rhelversion, "8.3");
