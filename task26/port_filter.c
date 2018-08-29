#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/netfilter.h>
#include <net/netlink.h>
#include <linux/netfilter_decnet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>


#define sysfs_name "mysys"


static unsigned short port = htons(80);


static ssize_t sysfs_read(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
    return sprintf(buf, "sysfs_read (/sys/kernel/%s) called\n", sysfs_name);
}


static ssize_t sysfs_write(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t len)
{
    sscanf(buf, "%hd", &port);
	pr_info("the blocked port is changed to  %d\n", port);
    port = htons(port);
    return len;
}


static int number;

static struct kobject *kobj;
static struct kobj_attribute kobj_attr =
    __ATTR(number, 0664, sysfs_read, sysfs_write);


static unsigned int my_hook(void *priv, struct sk_buff *skb,
                               const struct nf_hook_state *state)
{
    struct iphdr *_iphdr = ip_hdr(skb);

    struct tcphdr *th;
    struct udphdr *uh;

    if (_iphdr->protocol == IPPROTO_UDP) {

        uh = udp_hdr(skb);

        if(uh->source == port || uh->dest == port) {
            return NF_DROP;
        }

    }

    if (_iphdr->protocol == IPPROTO_TCP) {

        th = tcp_hdr(skb);

        if(th->source == port || th->dest == port) {
            return NF_DROP;
        }
    }


    return NF_ACCEPT;
}


static const struct nf_hook_ops hook_ops = {
	.hook		= my_hook,
	.pf		    = PF_INET,
	.hooknum	= NF_DN_POST_ROUTING,
	.priority	= NF_DN_PRI_FIRST,
};

static int __init _sysfs_init(void)
{
    /* Создание sysfs */
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

    /* Создание хука */
    int rv = 0;

	rv = nf_register_net_hook(&init_net, &hook_ops);

    pr_info("My hook is registered!\n");

	return rv;
}


static void __exit _sysfs_exit(void)
{
    nf_unregister_net_hook(&init_net, &hook_ops);

    kobject_put(kobj);
    sysfs_remove_file(kobj, &kobj_attr.attr);

    pr_info("Bye!");
}


module_init(_sysfs_init);
module_exit(_sysfs_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SYS FS");
MODULE_AUTHOR("Fedotov P. A.");
