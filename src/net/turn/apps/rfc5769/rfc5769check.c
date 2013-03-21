/*
 * Copyright (C) 2012 Citrix Systems
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ns_turn_utils.h"
#include "apputils.h"
#include "stun_buffer.h"

//////////////////////////////////////////////////

int main(int argc, const char **argv)
{
	int res = -1;

	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	srandom((unsigned int) time(NULL));

	{
		const unsigned char reqltc[] = "\x00\x01\x00\x60"
			"\x21\x12\xa4\x42"
			"\x78\xad\x34\x33\xc6\xad\x72\xc0\x29\xda\x41\x2e"
			"\x00\x06\x00\x12"
			"\xe3\x83\x9e\xe3\x83\x88\xe3\x83\xaa\xe3\x83\x83"
			"\xe3\x82\xaf\xe3\x82\xb9\x00\x00"
			"\x00\x15\x00\x1c"
			"\x66\x2f\x2f\x34\x39\x39\x6b\x39\x35\x34\x64\x36"
			"\x4f\x4c\x33\x34\x6f\x4c\x39\x46\x53\x54\x76\x79"
			"\x36\x34\x73\x41"
			"\x00\x14\x00\x0b"
			"\x65\x78\x61\x6d\x70\x6c\x65\x2e\x6f\x72\x67\x00"
			"\x00\x08\x00\x14"
			"\xf6\x70\x24\x65\x6d\xd6\x4a\x3e\x02\xb8\xe0\x71"
			"\x2e\x85\xc9\xa2\x8c\xa8\x96\x66";

		u08bits user[] = "\xe3\x83\x9e\xe3\x83\x88\xe3\x83\xaa\xe3\x83\x83"
			"\xe3\x82\xaf\xe3\x82\xb9";

		u08bits realm[33];
		u08bits nonce[29];
		u08bits upwd[33];

		u08bits buf[sizeof(reqltc)];
		memcpy(buf, reqltc, sizeof(reqltc));

		u08bits uname[sizeof(user)];
		memcpy(uname, user, sizeof(user));

		strcpy((char*) realm, "example.org");
		strcpy((char*) upwd, "TheMatrIX");
		strcpy((char*)nonce,"f//499k954d6OL34oL9FSTvy64sA");

		res = stun_check_message_integrity_str(buf, sizeof(reqltc) - 1, uname, realm,
						upwd);

		printf("RFC 5769 message structure, long-term credentials and integrity test result: ");

		if (res > 0) {
			printf("success\n");
		} else if (res == 0) {
			printf("failure on integrity check\n");
			exit(-1);
		} else {
			printf("failure on message structure check\n");
			exit(-1);
		}

		{ //encoding test
			printf("RFC 5769 message encoding test result: ");
			size_t len = 0;
			u16bits message_type = STUN_METHOD_BINDING;
			stun_tid tid;
			u16bits *buf16 = (u16bits*)buf;
			u32bits *buf32 = (u32bits*)buf;
			memcpy(tid.tsx_id,"\x78\xad\x34\x33\xc6\xad\x72\xc0\x29\xda\x41\x2e",12);
			stun_init_buffer_str(buf,&len);
			message_type &= (u16bits)(0x3FFF);
			buf16[0]=nswap16(message_type);
			buf16[1]=0;
			buf32[1]=nswap32(STUN_MAGIC_COOKIE);
			stun_tid_message_cpy(buf, &tid);
			stun_attr_add_integrity_by_user_str(buf, &len, uname, realm, upwd, nonce);
			if(len != (sizeof(reqltc)-1)) {
				printf("failure: length %d, must be %d\n",(int)len,(int)(sizeof(reqltc)-1));
				exit(-1);
			}
			if(memcmp(buf,reqltc,len)) {
				printf("failure: wrong message content\n");
				{
					int lines = 29;
					int line = 0;
					int col = 0;
					int cols = 4;
					for(line = 0;line<lines;line++) {
						for(col = 0; col<cols; col++) {
							u08bits c = buf[line*4+col];
							printf(" %2x",(int)c);
						}
						printf("\n");
					}
				}
				exit(-1);
			}
			printf("success\n");
		}

		//Negative test:
		buf[32] = 10;
		res = stun_check_message_integrity_str(buf, sizeof(reqltc) - 1, uname, realm,
						upwd);

		printf("RFC 5769 NEGATIVE long-term credentials test result: ");

		if (res == 0) {
			printf("success\n");
		} else {
			printf("failure on NEGATIVE long-term credentials check\n");
			exit(-1);
		}
	}

	{
		const unsigned char respv4[] = "\x01\x01\x00\x3c"
			"\x21\x12\xa4\x42"
			"\xb7\xe7\xa7\x01\xbc\x34\xd6\x86\xfa\x87\xdf\xae"
			"\x80\x22\x00\x0b"
			"\x74\x65\x73\x74\x20\x76\x65\x63\x74\x6f\x72\x20"
			"\x00\x20\x00\x08"
			"\x00\x01\xa1\x47\xe1\x12\xa6\x43"
			"\x00\x08\x00\x14"
			"\x2b\x91\xf5\x99\xfd\x9e\x90\xc3\x8c\x74\x89\xf9"
			"\x2a\xf9\xba\x53\xf0\x6b\xe7\xd7"
			"\x80\x28\x00\x04"
			"\xc0\x7d\x4c\x96";

		u08bits buf[sizeof(respv4)];
		memcpy(buf, respv4, sizeof(respv4));

		res = stun_is_command_message_full_check_str(buf, sizeof(respv4) - 1, 1, NULL);
		printf("RFC 5769 message fingerprint test(1) result: ");

		if (res) {
			printf("success\n");
		} else if (res == 0) {
			printf("failure on fingerprint(1) check\n");
			exit(-1);
		}

		buf[27] = 23;

		res = stun_is_command_message_full_check_str(buf, sizeof(respv4) - 1, 1, NULL);
		printf("RFC 5769 NEGATIVE fingerprint test(1) result: ");

		if (!res) {
			printf("success\n");
		} else if (res == 0) {
			printf("failure on NEGATIVE fingerprint check\n");
			exit(-1);
		}

		{
			ioa_addr addr4;
			ioa_addr addr4_test;

			printf("RFC 5769 IPv4 encoding result: ");

			res = stun_attr_get_first_addr_str(buf, sizeof(respv4)-1, STUN_ATTRIBUTE_XOR_MAPPED_ADDRESS, &addr4, NULL);
			if(res < 0) {
				printf("failure on message structure check\n");
				exit(-1);
			}

			make_ioa_addr((u08bits*)"192.0.2.1", 32853, &addr4_test);
			if(addr_eq(&addr4,&addr4_test)) {
				printf("success\n");
			} else {
				printf("failure on IPv4 deconding check\n");
				exit(-1);
			}
		}
	}

	{
		const unsigned char respv6[] = "\x01\x01\x00\x48"
						     "\x21\x12\xa4\x42"
						     "\xb7\xe7\xa7\x01\xbc\x34\xd6\x86\xfa\x87\xdf\xae"
						     "\x80\x22\x00\x0b"
						       "\x74\x65\x73\x74\x20\x76\x65\x63\x74\x6f\x72\x20"
						     "\x00\x20\x00\x14"
						       "\x00\x02\xa1\x47"
						       "\x01\x13\xa9\xfa\xa5\xd3\xf1\x79"
						       "\xbc\x25\xf4\xb5\xbe\xd2\xb9\xd9"
						     "\x00\x08\x00\x14"
						       "\xa3\x82\x95\x4e\x4b\xe6\x7b\xf1\x17\x84\xc9\x7c"
						       "\x82\x92\xc2\x75\xbf\xe3\xed\x41"
						     "\x80\x28\x00\x04"
						       "\xc8\xfb\x0b\x4c";

		u08bits buf[sizeof(respv6)];

		{ //decoding test
			memcpy(buf, respv6, sizeof(respv6));

			res = stun_is_command_message_full_check_str(buf, sizeof(respv6) - 1, 1, NULL);
			printf("RFC 5769 message fingerprint test(2) result: ");

			if (res) {
				printf("success\n");
			} else if (res == 0) {
				printf("failure on fingerprint(2) check\n");
				exit(-1);
			}
		}

		{//negative decoding test
			buf[27] = 23;

			res = stun_is_command_message_full_check_str(buf, sizeof(respv6) - 1, 1, NULL);
			printf("RFC 5769 NEGATIVE fingerprint test(2) result: ");

			if (!res) {
				printf("success\n");
			} else if (res == 0) {
				printf("failure on NEGATIVE fingerprint check\n");
				exit(-1);
			}
		}

		{//IPv6 deconding test
			ioa_addr addr6;
			ioa_addr addr6_test;

			printf("RFC 5769 IPv6 encoding result: ");

			res = stun_attr_get_first_addr_str(buf, sizeof(respv6) - 1,
							STUN_ATTRIBUTE_XOR_MAPPED_ADDRESS, &addr6, NULL);
			if (res < 0) {
				printf("failure on message structure check\n");
				exit(-1);
			}

			make_ioa_addr((u08bits*) "2001:db8:1234:5678:11:2233:4455:6677", 32853, &addr6_test);
			if (addr_eq(&addr6, &addr6_test)) {
				printf("success\n");
			} else {
				printf("failure on IPv6 deconding check\n");
				exit(-1);
			}
		}
	}

	return 0;
}
