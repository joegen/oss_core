/*
 * $Id$
 *
 *  captagent - Homer capture agent. Modular
 *  Duplicate SIP messages in Homer Encapulate Protocol [HEP] [ipv6 version]
 *
 *  Author: Alexandr Dubovikov <alexandr.dubovikov@gmail.com>
 *  (C) Homer Project 2012 (http://www.sipcapture.org)
 *
 *  Copyright (c) 2010-2016 <Alexandr Dubovikov> 
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the <SIPCAPTURE>. The name of the SIPCAPTURE may not be used to 
 * endorse or promote products derived from this software without specific 
 * prior written permission.

 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#ifndef __FAVOR_BSD
#define __FAVOR_BSD
#endif /* __FAVOR_BSD */

#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#ifdef USE_IPV6
#include <netinet/ip6.h>
#endif /* USE_IPV6 */

#include <pcap.h>

#include "OSS/SIP/core_hep.h"

pthread_t call_thread;   
pthread_mutex_t lock;

#ifdef USE_ZLIB
z_stream strm;
#endif /* USE_ZLIB */

int send_hepv3 (con_info_t* coninfo, rc_info_t *rcinfo, unsigned char *data, unsigned int len, unsigned int sendzip) {

    struct hep_generic *hg=NULL;
    void* buffer;
    unsigned int buflen=0, iplen=0,tlen=0;
    hep_chunk_ip4_t src_ip4, dst_ip4;
#ifdef USE_IPV6
    hep_chunk_ip6_t src_ip6, dst_ip6;    
#endif            
    hep_chunk_t payload_chunk;
    hep_chunk_t authkey_chunk;

    hg = malloc(sizeof(struct hep_generic));
    memset(hg, 0, sizeof(struct hep_generic));

    /* header set */
    memcpy(hg->header.id, "\x48\x45\x50\x33", 4);

    /* IP proto */
    hg->ip_family.chunk.vendor_id = htons(0x0000);
    hg->ip_family.chunk.type_id   = htons(0x0001);
    hg->ip_family.data = rcinfo->ip_family;
    hg->ip_family.chunk.length = htons(sizeof(hg->ip_family));
    
    /* Proto ID */
    hg->ip_proto.chunk.vendor_id = htons(0x0000);
    hg->ip_proto.chunk.type_id   = htons(0x0002);
    hg->ip_proto.data = rcinfo->ip_proto;
    hg->ip_proto.chunk.length = htons(sizeof(hg->ip_proto));
    

    /* IPv4 */
    if(rcinfo->ip_family == AF_INET) {
        /* SRC IP */
        src_ip4.chunk.vendor_id = htons(0x0000);
        src_ip4.chunk.type_id   = htons(0x0003);
        inet_pton(AF_INET, rcinfo->src_ip, &src_ip4.data);
        src_ip4.chunk.length = htons(sizeof(src_ip4));            
        
        /* DST IP */
        dst_ip4.chunk.vendor_id = htons(0x0000);
        dst_ip4.chunk.type_id   = htons(0x0004);
        inet_pton(AF_INET, rcinfo->dst_ip, &dst_ip4.data);        
        dst_ip4.chunk.length = htons(sizeof(dst_ip4));
        
        iplen = sizeof(dst_ip4) + sizeof(src_ip4); 
    }
#ifdef USE_IPV6
      /* IPv6 */
    else if(rcinfo->ip_family == AF_INET6) {
        /* SRC IPv6 */
        src_ip6.chunk.vendor_id = htons(0x0000);
        src_ip6.chunk.type_id   = htons(0x0005);
        inet_pton(AF_INET6, rcinfo->src_ip, &src_ip6.data);
        src_ip6.chunk.length = htons(sizeof(src_ip6));
        
        /* DST IPv6 */
        dst_ip6.chunk.vendor_id = htons(0x0000);
        dst_ip6.chunk.type_id   = htons(0x0006);
        inet_pton(AF_INET6, rcinfo->dst_ip, &dst_ip6.data);
        dst_ip6.chunk.length = htons(sizeof(dst_ip6));    
        
        iplen = sizeof(dst_ip6) + sizeof(src_ip6);
    }
#endif
        
    /* SRC PORT */
    hg->src_port.chunk.vendor_id = htons(0x0000);
    hg->src_port.chunk.type_id   = htons(0x0007);
    hg->src_port.data = htons(rcinfo->src_port);
    hg->src_port.chunk.length = htons(sizeof(hg->src_port));
    
    /* DST PORT */
    hg->dst_port.chunk.vendor_id = htons(0x0000);
    hg->dst_port.chunk.type_id   = htons(0x0008);
    hg->dst_port.data = htons(rcinfo->dst_port);
    hg->dst_port.chunk.length = htons(sizeof(hg->dst_port));
    
    
    /* TIMESTAMP SEC */
    hg->time_sec.chunk.vendor_id = htons(0x0000);
    hg->time_sec.chunk.type_id   = htons(0x0009);
    hg->time_sec.data = htonl(rcinfo->time_sec);
    hg->time_sec.chunk.length = htons(sizeof(hg->time_sec));
    

    /* TIMESTAMP USEC */
    hg->time_usec.chunk.vendor_id = htons(0x0000);
    hg->time_usec.chunk.type_id   = htons(0x000a);
    hg->time_usec.data = htonl(rcinfo->time_usec);
    hg->time_usec.chunk.length = htons(sizeof(hg->time_usec));
    
    /* Protocol TYPE */
    hg->proto_t.chunk.vendor_id = htons(0x0000);
    hg->proto_t.chunk.type_id   = htons(0x000b);
    hg->proto_t.data = rcinfo->proto_type;
    hg->proto_t.chunk.length = htons(sizeof(hg->proto_t));
    
    /* Capture ID */
    hg->capt_id.chunk.vendor_id = htons(0x0000);
    hg->capt_id.chunk.type_id   = htons(0x000c);
    hg->capt_id.data = htons(coninfo->capt_id);
    hg->capt_id.chunk.length = htons(sizeof(hg->capt_id));

    /* Payload */
    payload_chunk.vendor_id = htons(0x0000);
    payload_chunk.type_id   = sendzip ? htons(0x0010) : htons(0x000f);
    payload_chunk.length    = htons(sizeof(payload_chunk) + len);
    
    tlen = sizeof(struct hep_generic) + len + iplen + sizeof(hep_chunk_t);

    /* auth key */
    if(coninfo->capt_password != NULL) {

          tlen += sizeof(hep_chunk_t);
          /* Auth key */
          authkey_chunk.vendor_id = htons(0x0000);
          authkey_chunk.type_id   = htons(0x000e);
          authkey_chunk.length    = htons(sizeof(authkey_chunk) + strlen(coninfo->capt_password));
          tlen += strlen(coninfo->capt_password);
    }

    /* total */
    hg->header.length = htons(tlen);

    //fprintf(stderr, "LEN: [%d] vs [%d] = IPLEN:[%d] LEN:[%d] CH:[%d]\n", hg->header.length, ntohs(hg->header.length), iplen, len, sizeof(struct hep_chunk));

    buffer = (void*)malloc(tlen);
    if (buffer==0){
        fprintf(stderr,"ERROR: out of memory\n");
        free(hg);
        return 1;
    }
    
    memcpy((void*) buffer, hg, sizeof(struct hep_generic));
    buflen = sizeof(struct hep_generic);

    /* IPv4 */
    if(rcinfo->ip_family == AF_INET) {
        /* SRC IP */
        memcpy((void*) buffer+buflen, &src_ip4, sizeof(struct hep_chunk_ip4));
        buflen += sizeof(struct hep_chunk_ip4);
        
        memcpy((void*) buffer+buflen, &dst_ip4, sizeof(struct hep_chunk_ip4));
        buflen += sizeof(struct hep_chunk_ip4);
    }
#ifdef USE_IPV6
      /* IPv6 */
    else if(rcinfo->ip_family == AF_INET6) {
        /* SRC IPv6 */
        memcpy((void*) buffer+buflen, &src_ip6, sizeof(struct hep_chunk_ip6));
        buflen += sizeof(struct hep_chunk_ip6);
        
        memcpy((void*) buffer+buflen, &dst_ip6, sizeof(struct hep_chunk_ip6));
        buflen += sizeof(struct hep_chunk_ip6);
    }
#endif

    /* AUTH KEY CHUNK */
    if(coninfo->capt_password != NULL) {

        memcpy((void*) buffer+buflen, &authkey_chunk,  sizeof(struct hep_chunk));
        buflen += sizeof(struct hep_chunk);

        /* Now copying payload self */
        memcpy((void*) buffer+buflen, coninfo->capt_password, strlen(coninfo->capt_password));
        buflen+=strlen(coninfo->capt_password);
    }

    /* PAYLOAD CHUNK */
    memcpy((void*) buffer+buflen, &payload_chunk,  sizeof(struct hep_chunk));
    buflen +=  sizeof(struct hep_chunk);            

    /* Now copying payload self */
    memcpy((void*) buffer+buflen, data, len);    
    buflen+=len;    
    /* send this packet out of our socket */
    coninfo->send_hep_data(buffer, buflen);

    /* FREE */        
    if(buffer) free(buffer);
    if(hg) free(hg);        
    
    return 1;
}


int send_hepv2 (con_info_t* coninfo, rc_info_t *rcinfo, unsigned char *data, unsigned int len) {

    void* buffer;            
    struct hep_hdr hdr;
    struct hep_timehdr hep_time;
    struct hep_iphdr hep_ipheader;
    unsigned int totlen=0, buflen=0;
#ifdef USE_IPV6
    struct hep_ip6hdr hep_ip6header;
#endif /* USE IPV6 */

    /* Version && proto */
    hdr.hp_v = 2;
    hdr.hp_f = rcinfo->ip_family;
    hdr.hp_p = rcinfo->ip_proto;
    hdr.hp_sport = htons(rcinfo->src_port); /* src port */
    hdr.hp_dport = htons(rcinfo->dst_port); /* dst port */

    /* IP version */    
    switch (hdr.hp_f) {        
                case AF_INET:
                    totlen  = sizeof(struct hep_iphdr);
                    break;
#ifdef USE_IPV6                    
                case AF_INET6:
                    totlen = sizeof(struct hep_ip6hdr);
                    break;
#endif /* USE IPV6 */
                    
    }
    
    hdr.hp_l = totlen + sizeof(struct hep_hdr);
    
    /* COMPLETE LEN */
    totlen += sizeof(struct hep_hdr);
    totlen += len;

    totlen += sizeof(struct hep_timehdr);
      hep_time.tv_sec = rcinfo->time_sec;
      hep_time.tv_usec = rcinfo->time_usec;
      hep_time.captid = coninfo->capt_id;

    /*buffer for ethernet frame*/
    buffer = (void*)malloc(totlen);
    if (buffer==0){
    	fprintf(stderr,"ERROR: out of memory\n");
        goto error;
    }

    /* copy hep_hdr */
    memcpy((void*) buffer, &hdr, sizeof(struct hep_hdr));
    buflen = sizeof(struct hep_hdr);

    switch (hdr.hp_f) {

    	case AF_INET:
        	/* Source && Destination ipaddresses*/
        	inet_pton(AF_INET, rcinfo->src_ip, &hep_ipheader.hp_src);
        	inet_pton(AF_INET, rcinfo->dst_ip, &hep_ipheader.hp_dst);

                /* copy hep ipheader */
                memcpy((void*)buffer + buflen, &hep_ipheader, sizeof(struct hep_iphdr));
                buflen += sizeof(struct hep_iphdr);

                break;
#ifdef USE_IPV6
	case AF_INET6:

                inet_pton(AF_INET6, rcinfo->src_ip, &hep_ip6header.hp6_src);
                inet_pton(AF_INET6, rcinfo->dst_ip, &hep_ip6header.hp6_dst);                        

                /* copy hep6 ipheader */
                memcpy((void*)buffer + buflen, &hep_ip6header, sizeof(struct hep_ip6hdr));
                buflen += sizeof(struct hep_ip6hdr);
                break;
#endif /* USE_IPV6 */
     }

     /* Version 2 has timestamp, captnode ID */
     	/* TIMING  */
      memcpy((void*)buffer + buflen, &hep_time, sizeof(struct hep_timehdr));
      buflen += sizeof(struct hep_timehdr);


     memcpy((void *)(buffer + buflen) , (void*)(data), len);
     buflen +=len;
     /* send this packet out of our socket */
     coninfo->send_hep_data(buffer, buflen);
     
     /* FREE */
     if(buffer) free(buffer);

     return 1;

error:
     if(buffer) free(buffer);
     return 0;                     
}

int send_hep (con_info_t* coninfo, rc_info_t *rcinfo, unsigned char *data, unsigned int len) {

	unsigned char *zipData = NULL;
        int sendzip = 0;

#ifdef USE_ZLIB
        int status = 0;
        unsigned long dlen;

        if(coninfo->compress && coninfo->version == 3) {
                //dlen = len/1000+len*len+13;

                dlen = compressBound(len);

                zipData  = malloc(dlen); /* give a little bit memmory */

                /* do compress */
                status = compress( zipData, &dlen, data, len );
                if( status != Z_OK ){
                      fprintf(stderr, "data couldn't be compressed\n");
                      sendzip = 0;
                      if(zipData) free(zipData); /* release */
                }                   
                else {              
                        sendzip = 1;
                        len = dlen;
                }
        }

#endif /* USE_ZLIB */

        switch(coninfo->version) {
        
            case 3:
		return send_hepv3(coninfo, rcinfo, sendzip  ? zipData : data , len, sendzip);
                break;
                
            case 2:            
            case 1:        
                return send_hepv2(coninfo, rcinfo, data, len);                    
                break;
                
            default:
                fprintf(stderr, "Unsupported HEP version [%d]\n", coninfo->version);                
                break;
        }

	if(zipData) free(zipData);
        
        return 0;
}





