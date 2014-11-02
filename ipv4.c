//Register the protocol and device driver module
//Create functions to send message to the destination VM using IPv4
//Write read and write method for the decive driver
//Register the protocol number
//Create receive function using IPv4.
/*
 * @file    ipv4.c
 * @Author  Linyao Li
 * @Date    2014-11-02
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/ip.h>

#define BUFFER_SIZE 256
#define PROTOCOL_NUM 2333

struct _ip_pack {
    /*
     * _type:
     * 1 - destination ip number in record.
     * 2 - data.
     */
    unsigned char _type;
    unsigned char _buffer[BUFFER_SIZE];
};

/*
 * @struct sk_buff *skb
 *
 */
int call_ip_local_out(struct sk_buff *skb) {
    ip_local_out();
    return 0;
}
