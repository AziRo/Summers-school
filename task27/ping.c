#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/netfilter.h>
#include <linux/net.h>
#include <linux/netfilter_decnet.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/netpoll.h>
#include <net/netlink.h>
#include <net/sock.h>
#include <net/inet_common.h>
#include <linux/string.h>
#include <linux/random.h>
#include <linux/uio.h>


#define sysfs_name "mysys"

#define _IPv4 0x800
#define _CRC16 0x8005


static unsigned char dst_addr[4] = {8,8,8,8};
static unsigned short port = 80;
static struct sockaddr_in address;
static struct socket *sock;


static u32 create_address(u8 *ip)
{
    u32 addr = 0;
    int i;
    for (i = 0; i < 4; ++i)
    {
        addr += ip[i];
        if(i == 3) break;
        addr <<= 8;
    }
    return addr;
}


static u16 gen_crc16(const u16 *data, u16 size)
{
    int i = 0;
    u32 crc = 0;

    while (i < size) {
        crc = crc + data[i];
        if (crc > 0xFFFF) {
            crc -= 0x10000;
            crc += 0x1;
        }

        ++i;
    }

    return ~crc;
}


void construct_header(struct msghdr * msg, struct sockaddr_in * address){
    msg->msg_name = address;
    msg->msg_namelen = sizeof(struct sockaddr_in);
    msg->msg_control = NULL;
    msg->msg_controllen = 0;
    msg->msg_flags = 0;
}


static ssize_t sysfs_read(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
    return sprintf(buf, "sysfs_read (/sys/kernel/%s) called\n", sysfs_name);
}


static ssize_t sysfs_write(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t len)
{
    int i;
    struct icmphdr icmph;
    struct msghdr msg;
    struct kvec vec;
    int size_pkt = sizeof(icmph);
    mm_segment_t oldmm;

    sscanf(buf,"%d.%d.%d.%d",&dst_addr[0],&dst_addr[1],&dst_addr[2],&dst_addr[3]);
    pr_info("Ping to  %s\n", buf);


    address.sin_addr.s_addr = htonl(create_address(dst_addr));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    construct_header(&msg, &address);

    vec.iov_len = size_pkt;
    vec.iov_base = &icmph;

    memset((void *)&icmph, 0, sizeof(icmph));

    /* Заполнение ICMP заголовка */
    icmph.type = 8;
    icmph.code = 0;
    icmph.un.echo.id = htons(get_random_u32() % 65000);

    for (i = 0; i < 20; ++i) {
        /* Заполнение ICMP заголовка */
        icmph.un.echo.sequence = htons(i + 1);
        icmph.checksum = 0;
        icmph.checksum = gen_crc16((u16 *)&(icmph), sizeof(icmph)/2);

        oldmm = get_fs(); set_fs(KERNEL_DS);
        kernel_sendmsg(sock, &msg, &vec, 1, size_pkt);
        set_fs(oldmm);

    }
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

    struct icmphdr *icmph;

    if (_iphdr->protocol == IPPROTO_ICMP
        && _iphdr->saddr == address.sin_addr.s_addr) {

        icmph = icmp_hdr(skb);

        if (icmph->type == 0) {
            pr_info("from %d.%d.%d.%d icmp_seq=%d ttl=%d\n", dst_addr[0],
                    dst_addr[1],dst_addr[2],dst_addr[3],
                    htons(icmph->un.echo.sequence), _iphdr->ttl);
            return NF_DROP;
        }

    }

    return NF_ACCEPT;
}


static const struct nf_hook_ops hook_ops = {
	.hook		= my_hook,
	.pf		    = PF_INET,
	.hooknum	= NF_DN_PRE_ROUTING,
	.priority	= NF_DN_PRI_FIRST,
};


static int __init _ping_init(void)
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

    /* Инициализация сокета */
    mm_segment_t fs;
    struct timeval tv = {0, 100000};

    error = sock_create(AF_INET, SOCK_RAW, IPPROTO_ICMP, &sock);

    if (error < 0) {
        pr_err("Error %d while creating socket\n", error);
        sock_release(sock);
        return error;
    }

    pr_info("Socket created\n");

    fs = get_fs();
    set_fs(KERNEL_DS);
    kernel_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    set_fs(fs);

	return rv;
}


static void __exit _ping_exit(void)
{
    nf_unregister_net_hook(&init_net, &hook_ops);

    kobject_put(kobj);
    sysfs_remove_file(kobj, &kobj_attr.attr);

    sock_release(sock);

    pr_info("Bye!");
}


module_init(_ping_init);
module_exit(_ping_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SYS FS");
MODULE_AUTHOR("Fedotov P. A.");
