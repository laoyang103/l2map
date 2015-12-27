/*
 * lmap_capture.h:
 * Network packet capture handling.
 *
 * Copyright (c) 2015 Jianan Yang.
 * Email: 553069938@qq.com
 * Github: http://github.com/laoyang103/
 *
 */

#include <pcap.h>

#include "lmap_capture.h"

static void lmap_process_packet(u_char *user, const struct pcap_pkthdr *hdr, const u_char *pkt);

#define SNAPLEN 262144      /* largest chunk of data we accept from pcap */

/* ugh. */
static pcap_t *lmap_pc = NULL;

void lmap_capture_open_offline(char* dumpfile)
{
    char ebuf[PCAP_ERRBUF_SIZE];

    if (!(lmap_pc = pcap_open_offline(dumpfile, ebuf))) {
        log_msg(LOG_ERROR, "pcap_open_offline: %s", ebuf);
        unexpected_exit (-1);
    }

    log_msg(LOG_INFO, "reading packets from %s", dumpfile);
}

void lmap_capture_open_live(char* interface, char* filterexpr, int promisc)
{
    char ebuf[PCAP_ERRBUF_SIZE];
    struct bpf_program filter;

    if (!(lmap_pc = pcap_open_live(interface, SNAPLEN, promisc, 1000, ebuf))) {
        log_msg(LOG_ERROR, "pcap_open_live: %s", ebuf);

        if (getuid() != 0)
            log_msg(LOG_ERROR, "perhaps you need to be root?");
        else if (!interface)
            log_msg(LOG_ERROR, "perhaps try selecting an interface with the -i option?");

        unexpected_exit (-1);
    }

    /* Only apply a filter to live packets. Is this right? */
    if (pcap_compile(lmap_pc, &filter, filterexpr, 1, 0) == -1) {
        log_msg(LOG_ERROR, "pcap_compile: %s", pcap_geterr(lmap_pc));
        unexpected_exit (-1);
    }

    if (pcap_setfilter(lmap_pc, &filter) == -1) {
        log_msg(LOG_ERROR, "pcap_setfilter: %s", pcap_geterr(lmap_pc));
        unexpected_exit (-1);
    }

    log_msg(LOG_INFO, "listening on %s%s",
        interface ? interface : "all interfaces",
        promisc ? " in promiscuous mode" : "");
}

void lmap_capture_close(void)
{
    if (lmap_pc != NULL)
        pcap_close(lmap_pc);
}

char* lmap_get_default_interface(void)
{
    char ebuf[PCAP_ERRBUF_SIZE];
    char *interface;

    interface = pcap_lookupdev(ebuf);

    if (!interface) {
        log_msg(LOG_ERROR, "pcap_lookupdev: %s", ebuf);
        log_msg(LOG_ERROR, "try specifying an interface with -i");
        log_msg(LOG_ERROR, "or a pcap capture file with -f");
        unexpected_exit (-1);
    }

    return interface;
}

void lmap_capture_dispatch(void)
{
    lmap_pcap_dispatch(lmap_pc, -1, lmap_process_packet, NULL);
}

/* 
 * lmap_process_packet:
 * Callback which processes a packet captured by libpcap. 
 */
static void lmap_process_packet(u_char *user, const struct pcap_pkthdr *hdr, const u_char *pkt)
{
    u_short ptype = 0;
    u_int32_t srcip = 0;
    struct ip * iph = NULL;
    struct ether_header *eth = NULL;

    eth = (struct ether_header *)pkt;
    ptype = ntohs(eth->ether_type);
    if (ETHERTYPE_IP == ptype) {
        iph = (struct ip *)(pkt + sizeof(struct ether_header));
        srcip = iph->saddr;
    }
    lmap_core_update_pair(eth->ether_shost, srcip, eth->ether_dhost);
}

