#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>


#define sysfs_name "mysys"


static ssize_t sysfs_read(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
    return sprintf(buf, "sysfs_read (/sys/kernel/%s) called\n", sysfs_name);
}


static ssize_t sysfs_write(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t len)
{
	pr_info("Sorry, this operation isn't supported.\n");
	return len;
}


static int number;

static struct kobject *kobj;
static struct kobj_attribute kobj_attr =
    __ATTR(number, 0664, sysfs_read, sysfs_write);


static int __init _sysfs_init(void)
{
	kobj = kobject_create_and_add(sysfs_name, kernel_kobj);

    if (!kobj) {
        return -ENOMEM;
    }

    int error = 0;
    error = sysfs_create_file(kobj, &kobj_attr.attr);

    if (error) {
        pr_err("sysfs_create_file failed: %d!\n", error);
        kobject_put(kobj);
        sysfs_remove_file(kobj, &kobj_attr.attr);
        return -ENOMEM;
    }

    number = 123;

	pr_info("/sys/kernel/%s created\n", sysfs_name);

	return 0;
}


static void __exit _sysfs_exit(void)
{
    kobject_put(kobj);
    sysfs_remove_file(kobj, &kobj_attr.attr);

    pr_info("Bye!");
}


module_init(_sysfs_init);
module_exit(_sysfs_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SYS FS");
MODULE_AUTHOR("Fedotov P. A.");
