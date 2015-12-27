/*
 * lmap_options.c:
 * Options parsing.
 *
 * Copyright (c) 2015 Jianan Yang.
 * Email: 553069938@qq.com
 * Github: http://github.com/laoyang103/
 *
 */

#include "lmap.h"
#include "lmap_options.h"

#include <getopt.h>                     // for optarg, optind, optopt, etc

static lmap_options_t options = {
    FALSE, 
    TRUE, 
    NULL, 
    NULL, 
    "", 
    6666
};

static void validate_options(options_t* options);
static void usage(FILE *fp);

/*
 * Handle command-line options
 */
lmap_options_t* lmap_parse_options(int argc, char *argv[])
{
    int c;
    char optstring[] = "hi:pP:r:v";

    opterr = 0; /* extern in getopt.h */
    while ((c = getopt(argc, argv, optstring)) != -1) {
        switch(c) {
            case 'h':
                usage(stdout);
                exit (0);

            case 'i':
                if (options.dumpfile) {
                    log_msg(LOG_ERROR, "can't specify -i and -r");
                    unexpected_exit (-1);
                }
                options.interface = optarg;
                break;

            case 'v':
                options.verbose = TRUE;
                break;

            case 'p':
                options.promisc = FALSE;
                break;

            case 'P':
                options.port = atoi(optarg);
                if (options.port > 65535 || options.port < 0) {
                    log_msg(LOG_ERROR, "web server listen port illegal, 
                            range is 0-65535", optarg);
                    unexpected_exit (-1);
                }
                break;
            case 'r':
                if (options.interface) {
                    log_msg(LOG_ERROR, "can't specify -i and -r");
                    unexpected_exit (-1);
                }
                options.dumpfile = optarg;
                break;

            default:
                if (strchr(optstring, optopt))
                    log_msg(LOG_ERROR, "option -%c requires an argument", optopt);
                else
                    log_msg(LOG_ERROR, "unrecognised option -%c", optopt);
                usage(stderr);
                unexpected_exit (1);
        }
    }

    /* Build up filter. */
    if (optind < argc) {
        if (options.dumpfile)
            log_msg(LOG_WARNING, "filter code ignored with dump file");
        else {
            char **a;
            int l;
            for (a = argv + optind, l = sizeof("tcp and ()"); *a; l += strlen(*a) + 1, ++a);
            options.filterexpr = calloc(l, 1);
            strcpy(options.filterexpr, "tcp and (");
            for (a = argv + optind; *a; ++a) {
                strcat(options.filterexpr, *a);
                if (*(a + 1)) strcat(options.filterexpr, " ");
            }
            strcat(options.filterexpr, ")");
        }
    } else options.filterexpr = "tcp";

    log_msg(LOG_INFO, "using filter expression `%s'", options.filterexpr);

    validate_options(&options);

    return &options;
}

void validate_options(options_t* options)
{
#ifdef NO_DISPLAY_WINDOW
    if (!options->adjunct) {
        /*
         * TODO: assume adjuct mode by default if we were compiled without
         * display support.
         */
        log_msg(LOG_ERROR, "this version of driftnet was compiled without display support");
        log_msg(LOG_ERROR, "use the -a option to run it in adjunct mode");
        unexpected_exit (-1);
    }
#endif /* !NO_DISPLAY_WINDOW */

    if (!options->dumpfile) {
        if (!options->interface) {
            /* TODO: on linux works "any" for all interfaces */
            options->interface = get_default_interface();
        }
    }

    /* Let's not be too fascist about option checking.... */
    if (options->max_tmpfiles && !options->adjunct) {
        log_msg(LOG_WARNING, "-m only makes sense with -a");
        options->max_tmpfiles = 0;
    }

    if (options->adjunct && options->newpfx)
        log_msg(LOG_WARNING, "-x ignored -a");

    if (options->mpeg_player_specified && !(options->extract_type & m_audio))
        log_msg(LOG_WARNING, "-M only makes sense with -s");

    if (options->mpeg_player_specified && options->adjunct)
        log_msg(LOG_WARNING, "-M ignored with -a");

    if (options->max_tmpfiles && options->adjunct)
        log_msg(LOG_INFO, "a maximum of %d images will be buffered", options->max_tmpfiles);

    if (options->beep && options->adjunct)
        log_msg(LOG_WARNING, "can't beep in adjunct mode");
}

/* usage:
 * Print usage information. */
void usage(FILE *fp)
{
    fprintf(fp,
"lmap, version %s\n"
"Capture traffic and display link layer topology with svg.\n"
"\n"
"Synopsis: lmap [options] [filter code]\n"
"\n"
"Options:\n"
"\n"
"  -h               Display this help message.\n"
"  -v               Verbose operation.\n"
"  -p               Do not put the listening interface into promiscuous mode.\n"
"  -i interface     Select the interface on which to listen (default: all\n"
"                   interfaces).\n"
"  -P port          Specified port for web server which display svg topology\n"
"                   default is 6666.\n"
"  -r file          Instead of listening on an interface, read captured\n"
"                   packets from a pcap dump file; file can be a named pipe\n"
"                   for use with Kismet or similar.\n"
"\n"
"Filter code can be specified after any options in the manner of tcpdump(8).\n"
"The filter code will be evaluated as `tcp and (user filter code)'\n"
"\n"
"lmap, copyright (c) 2015-6 Jianan Yang<553069938@qq.com>\n"
"home page: http://www.github.com/laoyang103/lmap/\n"
"\n"
"This program is free software; you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation; either version 2 of the License, or\n"
"(at your option) any later version.\n"
"\n",
            "1.0.0");
}
