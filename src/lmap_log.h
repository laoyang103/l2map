/*
 * lmap_log.h:
 * Logging functions.
 *
 * Copyright (c) 2015 Jianan Yang.
 * Email: 553069938@qq.com
 * Github: http://github.com/laoyang103/
 *
 */

#ifndef __LMAP_LOG_H__
#define __LMAP_LOG_H__

#include <lmap.h>

typedef enum { 
    LOG_ERROR = 0, 
    LOG_WARNING, 
    LOG_INFO
} loglevel_t;

loglevel_t get_loglevel(void);
void set_loglevel(loglevel_t level);

void log_msg(loglevel_t level, const char *fmt, ...);

#endif /* __LOG_H__ */
