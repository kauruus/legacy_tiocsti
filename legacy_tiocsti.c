#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/kprobes.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Kauruus");

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};

typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);

static bool *tty_legacy_tiocsti;

static int legacy_tiocsti_init(void)
{
	kallsyms_lookup_name_t kallsyms_lookup_name;
	int ret;

	ret = register_kprobe(&kp);
	if (ret < 0) {
		pr_err("legacy_tiocsti_init: kprobe failed");
		return ret;
	}
	kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
	unregister_kprobe(&kp);

	tty_legacy_tiocsti = (void *)kallsyms_lookup_name("tty_legacy_tiocsti");
	if (tty_legacy_tiocsti == NULL) {
		pr_err("legacy_tiocsti_init: kallsyms_lookup_name failed to lookup tty_legacy_tiocsti");
		return -1;
	}

	*tty_legacy_tiocsti = 1;
	return 0;
}

static void legacy_tiocsti_cleanup(void)
{
	if (tty_legacy_tiocsti != NULL) {
		*tty_legacy_tiocsti = 0;
	}
}

module_init(legacy_tiocsti_init);
module_exit(legacy_tiocsti_cleanup);
