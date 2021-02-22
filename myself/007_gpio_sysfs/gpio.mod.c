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
	{ 0x8a8d74a6, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x204b1373, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x54a3ce9b, __VMLINUX_SYMBOL_STR(of_property_read_string) },
	{ 0xa1108ad2, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0xe83dde6b, __VMLINUX_SYMBOL_STR(of_get_next_available_child) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0xb81960ca, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0xca671112, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0xf0845bbf, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "F3A07FA9B18C8E7433F32DF");
