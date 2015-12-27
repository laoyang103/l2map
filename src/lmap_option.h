/*
 * lmap_options.h:
 * Options parsing.
 *
 * Copyright (c) 2015 Jianan Yang.
 * Email: 553069938@qq.com
 * Github: http://github.com/laoyang103/
 *
 */

#ifndef __LMAP_OPTIONS_H__
#define __LMAP_OPTIONS_H__

typedef struct _lmap_options_t {
    int verbose;
    int promisc;
    char *dumpfile;
    char *interface;
    char *filterexpr;
    int port;
} lmap_options_t;

lmap_options_t* lmap_parse_options(int argc, char *argv[]);

#endif /* __LMAP_OPTIONS_H__ */
