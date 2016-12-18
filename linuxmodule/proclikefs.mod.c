#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xbff2abe1, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x74cf6b1f, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x1288e1cc, __VMLINUX_SYMBOL_STR(make_bad_inode) },
	{ 0x754d539c, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0xfbf4ab75, __VMLINUX_SYMBOL_STR(kill_anon_super) },
	{ 0xadf60b26, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0xe2d5255a, __VMLINUX_SYMBOL_STR(strcmp) },
	{ 0x66c5d033, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xfbea016a, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0x3541809, __VMLINUX_SYMBOL_STR(inode_init_once) },
	{ 0x4f9aaf56, __VMLINUX_SYMBOL_STR(module_put) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x42a76691, __VMLINUX_SYMBOL_STR(d_drop) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xdf3deb56, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x11ab856d, __VMLINUX_SYMBOL_STR(register_filesystem) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xcc0d2cce, __VMLINUX_SYMBOL_STR(unregister_filesystem) },
	{ 0x254326c4, __VMLINUX_SYMBOL_STR(try_module_get) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "91AF36BD4084183584D5A8A");
