#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>


static int __init test_init(void)
{
        pr_info("Hello World!\n");
        return 0;
}


static void __exit test_exit(void)
{
        pr_info("Bye World!\n");
}


module_init(test_init);
module_exit(test_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("My first module");
MODULE_AUTHOR("Fedotov P. A.");
