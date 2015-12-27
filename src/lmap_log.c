/*
 * lmap_log.c:
 * Logging functions.
 *
 * Copyright (c) 2015 Jianan Yang.
 * Email: 553069938@qq.com
 * Github: http://github.com/laoyang103/
 *
 */

#include "lmap_log.h"

static loglevel_t loglevel = LOG_WARNING;

static char* get_levelstring(loglevel_t level);
static char* get_timestring(void);

loglevel_t get_loglevel(void)
{
    return loglevel;
}

void set_loglevel(loglevel_t level)
{
    loglevel = level;
}

void log_msg(loglevel_t level, const char *fmt, ...)
{
    int n;
    size_t size = 100;     /* guess we need no more than 100 bytes */
    char *msg;
    va_list ap;
    char *levelstring;
    char *timestring;

    if (get_loglevel() < level) return;

    msg = xmalloc(size);

    while (1) {
        va_start(ap, fmt);
        n = vsnprintf(msg, size, fmt, ap);
        va_end(ap);

        /* have it */
        if (n > -1 && n < size)
            break;

        /* try again with more space */
        if (n > -1)    /* glibc 2.1 */
            size = n+1; /* precisely what is needed */
        else           /* glibc 2.0 */
            size *= 2;  /* twice the old size */

        msg = xrealloc (msg, size);
    }

    levelstring = get_levelstring(level);
    timestring  = get_timestring();

    fprintf(stderr, "%s [%s] %s: %s\n", timestring, PROGNAME, levelstring, msg);

    xfree(msg);
}

static char* get_timestring(void)
{
    time_t timee;
    struct tm *timeinfo;
    static char time_s[80];

    time ( &timee );
    timeinfo = localtime ( &timee );

    strftime (time_s, 80, "%a %b %d %H:%M:%S %Y", timeinfo);

    return time_s;
}

static char* get_levelstring(loglevel_t level)
{
    switch (level) {
        case LOG_INFO:
            return "INFO";

        case LOG_WARNING:
            return "WARNING";

        case LOG_ERROR:
            return "ERROR";

        default:
            fprintf(stderr, PROGNAME ": get_level_string(): internal error");
            unexpected_exit(-1);
    }
    return NULL;
}
