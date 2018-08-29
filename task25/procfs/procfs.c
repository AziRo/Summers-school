#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


struct proc_dir_entry *proc_file_entry;


#define procfs_name "myprocfile"


static int proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "procfile_read (/proc/%s) called\n", procfs_name);
  return 0;
}


static int proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, proc_show, NULL);
}


static ssize_t proc_write(struct file *filp, const char *buff, size_t len,
				loff_t * off)
{
	pr_info("Sorry, this operation isn't supported.\n");
	return -EINVAL;
}


static const struct file_operations proc_file_fops = {
 	.owner = THIS_MODULE,
	.open = proc_open,
	.read = seq_read,
	.write = proc_write,
	.llseek = seq_lseek,
	.release = single_release,
};


static int __init procfs_init(void)
{
	proc_file_entry = proc_create(procfs_name, 0, NULL, &proc_file_fops);

	if (proc_file_entry == NULL) {
		remove_proc_entry(procfs_name, NULL);
		pr_alert("Error: Could not initialize /proc/%s\n", procfs_name);
		return -ENOMEM;
	}

	pr_info("/proc/%s created\n", procfs_name);

	return 0;
}


static void __exit procfs_exit(void)
{
	remove_proc_entry(procfs_name, NULL);
	pr_info("Bye!");
}


module_init(procfs_init);
module_exit(procfs_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PROC FS");
MODULE_AUTHOR("Fedotov P. A.");
