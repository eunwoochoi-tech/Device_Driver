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

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x1346194c, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x431e43b0, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x57ce8be4, __VMLINUX_SYMBOL_STR(devm_kfree) },
	{ 0xb57e2aa5, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xcfbad749, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x6b357db4, __VMLINUX_SYMBOL_STR(of_match_device) },
	{ 0x6f93c30d, __VMLINUX_SYMBOL_STR(of_property_read_variable_u32_array) },
	{ 0x54a3ce9b, __VMLINUX_SYMBOL_STR(of_property_read_string) },
	{ 0xca671112, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0x96ae2cce, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x15f3a041, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0x8a8d74a6, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xf0845bbf, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xa1108ad2, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0xce98ee1f, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x961de4ba, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "EE232C50BE295DE4169BFE7");
