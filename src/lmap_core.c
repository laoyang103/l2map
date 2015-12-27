/*
 * lmap_core.c:
 * handle all mac address.
 *
 * Copyright (c) 2015 Jianan Yang.
 * Email: 553069938@qq.com
 * Github: http://github.com/laoyang103/
 *
 */

#include "lmap_core.h"

/* 
 * Use three bytes to hash a mac address.
 * For Example: b4:6d:83:5f:09:dd
 *                 ^     ^     ^
 * hash bytes is 6d-5f-dd.
 * 2^24 = 16777216, we use 16777216 + 10
 */
static lmap_node_t lmap_topo[16777226] = {0};

void lmap_core_update_pair(u_char *srcmac, u_int32_t srcip, u_char *dstmac)
{
    u_int32_t i = 0;
    u_int32_t srckey = LMAP_HASH_MAC(srcmac);
    u_int32_t dstkey = LMAP_HASH_MAC(dstmac);

    lmap_node_t *node = lmap_topo[srckey];
    node->key = srckey; node->ip = srcip;
    strncpy(node->mac, srcmac, 6);

    /* Find wether dstmac in srcnode link list */
    for (i = 0; i < node->numlink; i++) {
        if (node->link[i] == dstkey) break;
    }
    /* If not insert it */
    if (i == node->numlink) {
        node->link[node->numlink] = dstkey;
        if (node->numlink < 1000) node->numlink ++;
    }
}
