/*
 * lmap_capture.h:
 * Network packet capture handling.
 *
 * Copyright (c) 2015 Jianan Yang.
 * Email: 553069938@qq.com
 * Github: http://github.com/laoyang103/
 *
 */

#ifndef __LMAP_CAPTURE_H__
#define __LMAP_CAPTURE_H__

#include "lmap.h"

void lmap_capture_open_live(char* interface, char* filterexpr, int promisc);
void lmap_capture_open_offline(char* dumpfile);
void lmap_capture_close(void);

void lmap_capture_dispatch(void);
char* lmap_get_default_interface(void);

#endif  /* __PACKETCAPTURE_H__ */
