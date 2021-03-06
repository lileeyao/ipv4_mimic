//Disclaimer: This source file is based on the implementation of Mark Yates.
//
//Register the protocol and device driver module
//Create functions to send message to the destination VM using IPv4
//Write read and write method for the decive driver
//Register the protocol number
//Create receive function using IPv4.
/*
* @file    cse5361.c
* @Author  Linyao Li
* @Date    2014-11-02
*
*/
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/inet.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <net/protocol.h>
#include <net/sock.h>
#include <net/route.h>
#include <net/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include "/Users/linyaoli/repo/linux_kernel/linux-3.13/include/linux/skbuff.h"

#define CSE536_MAJOR 234
#define IPPROTO_CSE536 234 // my protocol number

#define BUFFER_SIZE 256

static int debug_enable = 0;
module_param(debug_enable, int, 0);
MODULE_PARM_DESC(debug_enable, "Enable module debug mode.");
struct file_operations cse536_fops;

/*
* This struct is used to replace the sk_buff.
*/
struct _buff{
		/*
		* _type:
		* 1 - destination ip number in record.
		* 2 - data.
		*/
		uint8_t     _type;
		uint32_t    _idx;
		uint8_t     _len;
		// A size of 256 bytes buffer.
		uint8_t     _buffer[BUFFER_SIZE];
		uint32_t    _protocol;
		// target machine.
		char*       _target;
		// source machine.
		char*       _source;
};

/*
* FIXME Seems like we dont have to create a new _buff
* TODO Does sk_buff work ?
*/
int local_call(int full_len) {
		struct skbuff *skb = alloc_skb(full_len, GFP_ATOMIC);
		return ip_queue_xmit();
}

/* define structure to represent items in linked list buffer to hold
 * data written but not yet read. */
struct cse536buffer
{
	struct cse536buffer *next;
	char data[BUFFER_SIZE];
};
// head and tail pointers to manage linked list
struct cse536buffer *cse536buffhead, *cse536bufftail;
// destination and local address variables
__be32 cse536_daddr, cse536_saddr;

static int cse536_sendmsg(char *data, size_t len);

static int cse536_open(struct inode *inode, struct file *file)
{
	printk("cse536_open: successful\n");
	return 0;
}
static int cse536_release(struct inode *inode, struct file *file)
{
	printk("cse536_release: successful\n");
	return 0;
}
static ssize_t cse536_read(struct file *file, char *buf, size_t count,
loff_t *ptr)
{
	struct cse536buffer *next;
	ssize_t retCount = count;
	if (count > BUFFER_SIZE)
		retCount = BUFFER_SIZE;

	if ( cse536buffhead != 0 )
	{
		// copy data from linked list head to read buffer
		memcpy(buf, cse536buffhead->data, retCount);
		// remove the head from the list and point to the next item
		next = cse536buffhead->next;
		kfree(cse536buffhead);
		// manage the link list pointers
		if ( next == 0 )
		{
			cse536buffhead = 0;
			cse536bufftail = 0;
		}
		else
		{
			cse536buffhead = next;
		}
	}
	else
	{
		// nothing in buffer, return 0s
		memset(buf, 0, retCount);
	}
	//retCount = sprintf(buf, "cse536");
	printk("cse536_read: returning %d bytes\n", retCount);

	return retCount;
}
static ssize_t cse536_write(struct file *file, const char *buf,
size_t count, loff_t * ppos)
{
	char data[BUFFER_SIZE], address[16];

	if ( buf[0] == 1 ) {	// data contains ip address
		// address starts at byte 2
		memcpy(address, buf+1, 16);
		printk("cse536_write - setting address: %s\n", address);
		cse536_daddr = in_aton(address);
	}
	else
	{
		// initialize data memory to zeros
		memset(data, 0, BUFFER_SIZE);
		// copy write buffer to data array, data starts at 2nd byte in buffer
		memcpy(data, buf+1, count);
		printk("cse536_write - sending message: %s\n", data);
		cse536_sendmsg(data, count);
	}
	printk("cse536_write1: %s\n", buf);
	return -1;
}
// this method will send the message to the destination machine using ipv4
static int cse536_sendmsg(char *data, size_t len)
{
	struct sk_buff *skb;
	struct iphdr *iph;
	struct rtable *rt;
	struct net *net = &init_net;
	unsigned char *skbdata;
	// create and setup a sk_buff
	skb = alloc_skb(sizeof(struct iphdr) + 4096, GFP_ATOMIC);
	skb_reserve(skb, sizeof(struct iphdr) + 1500);
	skbdata = skb_put(skb, len);
	memcpy(skbdata, data, len);
	//configure the ip header parameters.
	skb_push(skb, sizeof(struct iphdr));
	skb_reset_network_header(skb);
	iph = ip_hdr(skb);
	iph->version  = 4;
	iph->ihl      = 5;
	iph->tos      = 0;
	iph->frag_off = 0;
	iph->ttl      = 64;
	iph->daddr    = cse536_daddr;
	iph->saddr    = cse536_saddr;
	iph->protocol = IPPROTO_CSE536;//protocol# 234
	iph->id       = htons(1);
	iph->tot_len  = htons(skb->len);

	rt = ip_route_output(net, cse536_daddr, cse536_saddr, 0,0);
	skb_dst_set(skb, &rt->dst);
	return ip_local_out(skb);//use this function to output
}

static long cse536_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("cse536_ioctl: cmd=%d, arg=%ld\n", cmd, arg);
	return 0;
}
// called by ipv4 when a packet is received associated with my protocol number
int cse536_rcv(struct sk_buff *skb)
{
	// setup a linked list item to add the data to the linked list buffer
	struct cse536buffer *item = kmalloc(sizeof(struct \
	 	cse536buffer), GFP_ATOMIC);
	memset(item->data, 0, 256); // intialize data in buffer item
	memcpy(item->data, skb->data, skb->len); // copy data to buffer item
	// add to the buffer linked list
	if ( cse536buffhead == 0 )
	{
		cse536buffhead = item;
	}
	if ( cse536bufftail != 0 )
	{
		cse536bufftail->next = item;
	}
	cse536bufftail = item;
	item->next = 0;
	return 0;
}
void cse536_err(struct sk_buff *skb, __u32 info)
{
	printk("Error handler called.\n");
}

/* Register with IP layer.  */
static const struct net_protocol cse536_protocol = {
	.handler     = cse536_rcv,
	.err_handler = cse536_err,
	.no_policy   = 1,
};

static int cse536_add_protocol(void)
{
	/* Register protocol with inet layer.  */
	if (inet_add_protocol(&cse536_protocol, IPPROTO_CSE536) < 0)
		return -EAGAIN;
	return 0;
}
static void cse536_del_protocol(void)
{
	inet_del_protocol(&cse536_protocol, IPPROTO_CSE536);
}
// get the local ip address so that it does not have to be set manually
static void getlocaladdress(void)
{
	struct net_device *eth0 = dev_get_by_name(&init_net, "eth0");
	struct in_device *ineth0 = in_dev_get(eth0);

	for_primary_ifa(ineth0){
		cse536_saddr = ifa->ifa_address;
  	} endfor_ifa(ineth0);
}
static int __init cse536_init(void)
{
	int ret;

	printk("cse536 module Init - debug mode is %s\n",
	debug_enable ? "enabled" : "disabled");

	getlocaladdress();
	printk("cse536 module Init - using local address: %pI4\n", &cse536_saddr);

	// initialize buffer
	cse536buffhead = 0;
	cse536bufftail = 0;

	// register my protocol
	ret = cse536_add_protocol();
	if ( ret < 0 ) {
		printk("Error registering cse536 protocol\n");
		goto cse536_fail1;
	}

	ret = register_chrdev(CSE536_MAJOR, "cse5361", &cse536_fops);
	if (ret < 0) {
		printk("Error registering cse536 device\n");
		goto cse536_fail1;
	}
	printk("cse536: registered module successfully!\n");
	/* Init processing here... */
	return 0;
	cse536_fail1:
	return ret;
}
static void __exit cse536_exit(void)
{
	cse536_del_protocol();
	unregister_chrdev(CSE536_MAJOR, "cse5361");
	printk("cse536 module Exit\n");
}

struct file_operations cse536_fops = {
	owner: THIS_MODULE,
	read: cse536_read,
	write: cse536_write,
	unlocked_ioctl: cse536_ioctl,
	open: cse536_open,
	release: cse536_release,
};
module_init(cse536_init);
module_exit(cse536_exit);

MODULE_AUTHOR("Lin-Yao Li");
MODULE_DESCRIPTION("CSE536 Module");
MODULE_LICENSE("GPL");
