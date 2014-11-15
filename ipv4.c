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
#include <inttypes.h>

#include "/Users/linyaoli/repo/linux_kernel/linux-3.13/include/linux/skbuff.h"

#define BUFFER_SIZE 256
#define PROTOCOL_NUM 2333

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

int _write() {
    return 0;
}

int _read() {
    return 0;
}

int _receive() {

}



