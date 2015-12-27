/*
 * lmap_core.h:
 * handle all mac address.
 *
 * Copyright (c) 2015 Jianan Yang.
 * Email: 553069938@qq.com
 * Github: http://github.com/laoyang103/
 *
 */

#ifdef __LMAP_CORE_H__
#define __LMAP_CORE_H__

#include "lmap.h"

#define LMAP_HASH_MAC(mac) (u_int32_t )((mac[1]<<16) + (mac[3]<<8) + mac[5]);

typedef struct _lmap_node_t {
    u_int32_t key;
    u_char mac[6];
    u_int32_t ip;
    u_int32_t link[1024];
    u_int32_t numlink;
} lmap_node_t;

void lmap_core_update_pair(u_char *srcmac, u_int32_t srcip, u_char *dstmac);

#endif
