/*
 * Copyright (C) 2011, 2012 Citrix Systems
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

#include <unistd.h>

#include "apputils.h"
#include "ns_turn_utils.h"
#include "startuclient.h"
#include "ns_turn_msg.h"
#include "uclient.h"
#include "session.h"

#include <openssl/err.h>

/////////////////////////////////////////

#define MAX_CONNECT_EFFORTS (77)

static uint64_t current_reservation_token = 0;
static int allocate_rtcp = 0;
static const int never_allocate_rtcp = 0;

/////////////////////////////////////////

static int get_allocate_address_family(ioa_addr *relay_addr) {
	if(relay_addr->ss.ss_family == AF_INET)
		return STUN_ATTRIBUTE_REQUESTED_ADDRESS_FAMILY_VALUE_DEFAULT;
	else if(relay_addr->ss.ss_family == AF_INET6)
		return STUN_ATTRIBUTE_REQUESTED_ADDRESS_FAMILY_VALUE_IPV6;
	else
		return STUN_ATTRIBUTE_REQUESTED_ADDRESS_FAMILY_VALUE_INVALID;
}

/////////////////////////////////////////

static SSL* tls_connect(ioa_socket_raw fd, ioa_addr *remote_addr)
{
	SSL *ssl = SSL_new(root_tls_ctx);

	if(use_tcp) {
		SSL_set_fd(ssl, fd);
	} else {
#if !defined(BIO_CTRL_DGRAM_QUERY_MTU)
	  UNUSED_ARG(remote_addr);
	  fprintf(stderr,"ERROR: DTLS is not supported.\n");
	  exit(-1);
#else
		/* Create BIO, connect and set to already connected */
		BIO *bio = BIO_new_dgram(fd, BIO_CLOSE);
		//bio = BIO_new_socket(fd, BIO_CLOSE);

		BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &remote_addr->ss);

		SSL_set_bio(ssl, bio, bio);

		{
			struct timeval timeout;
			/* Set and activate timeouts */
			timeout.tv_sec = DTLS_MAX_CONNECT_TIMEOUT;
			timeout.tv_usec = 0;
			BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);
		}

		set_mtu_df(ssl, fd, remote_addr->ss.ss_family, SOSO_MTU, clnet_verbose);
#endif
	}

	SSL_set_max_cert_list(ssl, 655350);

	if (clnet_verbose)
		TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "call SSL_connect...\n");

	int rc = 0;

	do {
		do {
			rc = SSL_connect(ssl);
		} while (rc < 0 && errno == EINTR);
		if (rc > 0) {
			break;
		} else {
			TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "%s: cannot connect\n",
					__FUNCTION__);
			switch (SSL_get_error(ssl, rc)) {
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				usleep(1000);
				continue;
			default: {
				char buf[1025];
				TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "%s (%d)\n",
						ERR_error_string(ERR_get_error(), buf), SSL_get_error(
								ssl, rc));
				exit(-1);
			}
			};
		}
	} while (1);

	if (clnet_verbose && SSL_get_peer_certificate(ssl)) {
		if(use_tcp) {
			TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
				"------TLS---------------------------------------------------\n");
		} else {
			TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
				"------DTLS---------------------------------------------------\n");
		}
		X509_NAME_print_ex_fp(stdout, X509_get_subject_name(
				SSL_get_peer_certificate(ssl)), 1, XN_FLAG_MULTILINE);
		TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "\n\n Cipher: %s\n",
				SSL_CIPHER_get_name(SSL_get_current_cipher(ssl)));
		TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
				"\n------------------------------------------------------------\n\n");
	}

	return ssl;
}

static int clnet_connect(uint16_t clnet_remote_port, const char *remote_address,
		const unsigned char* ifname, const char *local_address, int verbose,
		app_ur_conn_info *clnet_info) {

	ioa_addr local_addr;
	evutil_socket_t clnet_fd = -1;

	ioa_addr remote_addr;
	memset((void *) &remote_addr, 0, sizeof(struct sockaddr_storage));
	if (make_ioa_addr((const u08bits*) remote_address, clnet_remote_port,
			&remote_addr) < 0)
		return -1;

	memset((void *) &local_addr, 0, sizeof(struct sockaddr_storage));

	clnet_fd = socket(remote_addr.ss.ss_family, use_tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
	if (clnet_fd < 0) {
		perror("socket");
		exit(-1);
	}

	if (sock_bind_to_device(clnet_fd, ifname) < 0) {
		TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
				"Cannot bind client socket to device %s\n", ifname);
	}

	set_sock_buf_size(clnet_fd, UR_CLIENT_SOCK_BUF_SIZE);

	if (strlen(local_address) > 0) {

		uint16_t localport = random();

		while (1) {

			while (localport < 1024) {
				localport = (uint16_t) random();
			}

			if (make_ioa_addr((const u08bits*) local_address, localport,
					&local_addr) < 0)
				return -1;

			int bindres = addr_bind(clnet_fd, &local_addr);
			if (bindres >= 0) {
				TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "%s: localport=%d\n",
						__FUNCTION__, (int) localport);
				break;
			} else {
				localport = 0;
			}
		}
	}

	if (addr_connect(clnet_fd, &remote_addr) < 0) {
		perror("connect");
		TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
				"%s: cannot connect to remote addr\n", __FUNCTION__);
		exit(-1);
	}

	if (clnet_info) {
		addr_cpy(&(clnet_info->remote_addr), &remote_addr);
		addr_cpy(&(clnet_info->local_addr), &local_addr);
		clnet_info->fd = clnet_fd;
	}

	if(use_secure) {
		clnet_info->ssl = tls_connect(clnet_info->fd, &remote_addr);
		if(!clnet_info->ssl) {
			TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
					"%s: cannot SSL connect to remote addr\n", __FUNCTION__);
			exit(-1);
		}
	}

	addr_debug_print(verbose, &remote_addr, "Connected to");

	usleep(50000);

	return 0;
}

static int clnet_allocate(int verbose,
		app_ur_conn_info *clnet_info,
		ioa_addr *relay_addr,
		int af) {

	int af_cycle = 0;

	int allocate_finished;

	beg_allocate:

	allocate_finished=0;

	while (!allocate_finished && af_cycle++ < 32) {

		int allocate_sent = 0;

		stun_buffer message;
		if(current_reservation_token)
			af = STUN_ATTRIBUTE_REQUESTED_ADDRESS_FAMILY_VALUE_DEFAULT;
		stun_set_allocate_request(&message, 1800, af);
		if(dont_fragment)
			stun_attr_add(&message, STUN_ATTRIBUTE_DONT_FRAGMENT, NULL, 0);
		if(!no_rtcp) {
		  if (!never_allocate_rtcp && allocate_rtcp) {
		    uint64_t reservation_token = ioa_ntoh64(current_reservation_token);
		    stun_attr_add(&message, STUN_ATTRIBUTE_RESERVATION_TOKEN,
				  (char*) (&reservation_token), 8);
		  } else {
		    stun_attr_add_even_port(&message, 1);
		  }
		}

		if(clnet_info->nonce[0]) {
			if(stun_attr_add_integrity_by_user_str(message.buf, (size_t*)&(message.len), g_uname,
							clnet_info->realm, g_upwd, clnet_info->nonce)<0) {
				TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO," Cannot add integrity to the message\n");
				return -1;
			}
		}

		stun_attr_add_fingerprint_str(message.buf,(size_t*)&(message.len));

		while (!allocate_sent) {

			int len = send_buffer(clnet_info, &message);

			if (len > 0) {
				if (verbose) {
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "allocate sent\n");
				}
				allocate_sent = 1;
			} else {
				perror("send");
				exit(1);
			}
		}

		////////////<<==allocate send

		////////allocate response==>>
		{
			int allocate_received = 0;
			stun_buffer message;
			while (!allocate_received) {

				int len = recv_buffer(clnet_info, &message);

				if (len > 0) {
					if (verbose) {
						TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
								"allocate response received: \n");
					}
					message.len = len;
					int err_code = 0;
					u08bits err_msg[129];
					if (stun_is_success_response(&message)) {
						allocate_received = 1;
						allocate_finished = 1;
						if (verbose) {
							TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "success\n");
						}
						if (stun_attr_get_first_addr(&message,
								STUN_ATTRIBUTE_XOR_RELAYED_ADDRESS, relay_addr,
								NULL) < 0) {
							TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
									"%s: !!!: relay addr cannot be received\n",
									__FUNCTION__);
							return -1;
						} else {
							if (verbose) {
								ioa_addr remote_addr;
								memcpy(&remote_addr, relay_addr,
										sizeof(remote_addr));
								addr_debug_print(verbose, &remote_addr,
										"Received relay addr");
							}
						}
						stun_attr_ref rt_sar = stun_attr_get_first_by_type(
								&message, STUN_ATTRIBUTE_RESERVATION_TOKEN);
						uint64_t rtv = stun_attr_get_reservation_token_value(rt_sar);
						current_reservation_token = rtv;
						if (verbose)
							TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
									"%s: rtv=%llu\n", __FUNCTION__, rtv);
					} else if (stun_is_challenge_response_str(message.buf, (size_t)message.len,
									&err_code,err_msg,sizeof(err_msg),
									clnet_info->realm,clnet_info->nonce)) {
						goto beg_allocate;
					} else if (stun_is_error_response(&message, &err_code,err_msg,sizeof(err_msg))) {
						allocate_received = 1;
						TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "error %d (%s)\n",
								      err_code,(char*)err_msg);
						if (err_code != 437) {
							allocate_finished = 1;
							current_reservation_token = 0;
							return -1;
						} else {
							TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
									"trying allocate again...\n", err_code);
							sleep(5);
						}
					} else {
						TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
									"unknown allocate response\n");
						/* Try again ? */
					}
				} else {
					perror("recv");
					exit(-1);
					break;
				}
			}
		}
	}
	////////////<<== allocate response received

	if(!allocate_finished) {
	  TURN_LOG_FUNC(TURN_LOG_LEVEL_ERROR,
			"Cannot complete Allocation\n");
	  exit(-1);
	}

	allocate_rtcp = !allocate_rtcp;

	if (1) {

	  af_cycle = 0;

		beg_refresh:

	  if(af_cycle++>32) {
	    TURN_LOG_FUNC(TURN_LOG_LEVEL_ERROR,
			  "Cannot complete Refresh\n");
	    exit(-1);
	  }

		//==>>refresh request, for an example only:
		{
			int refresh_sent = 0;

			stun_buffer message;
			stun_init_request(STUN_METHOD_REFRESH, &message);
			uint32_t lt = htonl(600);
			stun_attr_add(&message, STUN_ATTRIBUTE_LIFETIME, (const char*) &lt,
					4);

			if(clnet_info->nonce[0]) {
				if(stun_attr_add_integrity_by_user_str(message.buf, (size_t*)&(message.len), g_uname,
							clnet_info->realm, g_upwd, clnet_info->nonce)<0) {
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO," Cannot add integrity to the message\n");
					return -1;
				}
			}

			stun_attr_add_fingerprint_str(message.buf,(size_t*)&(message.len));

			while (!refresh_sent) {

				int len = send_buffer(clnet_info, &message);

				if (len > 0) {
					if (verbose) {
						TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "refresh sent\n");
					}
					refresh_sent = 1;
				} else {
					perror("send");
					exit(1);
				}
			}
		}

		////////refresh response==>>
		{
			int refresh_received = 0;
			stun_buffer message;
			while (!refresh_received) {

				int len = recv_buffer(clnet_info, &message);

				if (len > 0) {
					if (verbose) {
						TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
								"refresh response received: \n");
					}
					message.len = len;
					int err_code = 0;
					u08bits err_msg[129];
					if (stun_is_success_response(&message)) {
						refresh_received = 1;
						if (verbose) {
							TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "success\n");
						}
					} else if (stun_is_challenge_response_str(message.buf, (size_t)message.len,
										&err_code,err_msg,sizeof(err_msg),
										clnet_info->realm,clnet_info->nonce)) {
							goto beg_refresh;
					} else if (stun_is_error_response(&message, &err_code,err_msg,sizeof(err_msg))) {
						refresh_received = 1;
						TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "error %d (%s)\n",
								      err_code,(char*)err_msg);
						return -1;
					} else {
						TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "unknown refresh response\n");
						/* Try again ? */
					}
				} else {
					perror("recv");
					exit(-1);
					break;
				}
			}
		}
	}

	return 0;
}

static int turn_channel_bind(int verbose, uint16_t *chn,
		app_ur_conn_info *clnet_info, ioa_addr *peer_addr) {

	beg_bind:

	{
		int cb_sent = 0;

		stun_buffer message;

		*chn = stun_set_channel_bind_request(&message, peer_addr, 0);

		if(clnet_info->nonce[0]) {
			if(stun_attr_add_integrity_by_user_str(message.buf, (size_t*)&(message.len), g_uname,
						clnet_info->realm, g_upwd, clnet_info->nonce)<0) {
				TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO," Cannot add integrity to the message\n");
				return -1;
			}
		}

		stun_attr_add_fingerprint_str(message.buf,(size_t*)&(message.len));

		while (!cb_sent) {

			int len = send_buffer(clnet_info, &message);
			if (len > 0) {
				if (verbose) {
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "cb sent\n");
				}
				cb_sent = 1;
			} else {
				perror("send");
				exit(1);
			}
		}
	}

	////////////<<==channel bind send

	////////channel bind response==>>

	{
		int cb_received = 0;
		stun_buffer message;
		while (!cb_received) {

			int len = recv_buffer(clnet_info, &message);
			if (len > 0) {
				if (verbose) {
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
							"cb response received: \n");
				}
				int err_code = 0;
				u08bits err_msg[129];
				if (stun_is_success_response(&message)) {
					cb_received = 1;
					if (verbose) {
						TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "success: 0x%x\n",
								(int) (*chn));
					}
				} else if (stun_is_challenge_response_str(message.buf, (size_t)message.len,
										&err_code,err_msg,sizeof(err_msg),
										clnet_info->realm,clnet_info->nonce)) {
										goto beg_bind;
				} else if (stun_is_error_response(&message, &err_code,err_msg,sizeof(err_msg))) {
					cb_received = 1;
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "channel bind: error %d (%s)\n",
							      err_code,(char*)err_msg);
					return -1;
				} else {
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "unknown channel bind response\n");
					/* Try again ? */
				}
			} else {
				perror("recv");
				exit(-1);
				break;
			}
		}
	}

	return 0;
}

static int turn_create_permission(int verbose, app_ur_conn_info *clnet_info,
		ioa_addr *peer_addr) {

	beg_cp:

	{
		int cp_sent = 0;

		stun_buffer message;

		stun_init_request(STUN_METHOD_CREATE_PERMISSION, &message);
		stun_attr_add_addr(&message, STUN_ATTRIBUTE_XOR_PEER_ADDRESS, peer_addr);

		if(clnet_info->nonce[0]) {
			if(stun_attr_add_integrity_by_user_str(message.buf, (size_t*)&(message.len), g_uname,
						clnet_info->realm, g_upwd, clnet_info->nonce)<0) {
				TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO," Cannot add integrity to the message\n");
				return -1;
			}
		}

		stun_attr_add_fingerprint_str(message.buf,(size_t*)&(message.len));

		while (!cp_sent) {

			int len = send_buffer(clnet_info, &message);

			if (len > 0) {
				if (verbose) {
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "cp sent\n");
				}
				cp_sent = 1;
			} else {
				perror("send");
				exit(1);
			}
		}
	}

	////////////<<==create permission send

	////////create permission response==>>

	{
		int cp_received = 0;
		stun_buffer message;
		while (!cp_received) {

			int len = recv_buffer(clnet_info, &message);
			if (len > 0) {
				if (verbose) {
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO,
							"cp response received: \n");
				}
				int err_code = 0;
				u08bits err_msg[129];
				if (stun_is_success_response(&message)) {
					cp_received = 1;
					if (verbose) {
						TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "success\n");
					}
				} else if (stun_is_challenge_response_str(message.buf, (size_t)message.len,
									&err_code,err_msg,sizeof(err_msg),
									clnet_info->realm,clnet_info->nonce)) {
					goto beg_cp;
				} else if (stun_is_error_response(&message, &err_code,err_msg,sizeof(err_msg))) {
					cp_received = 1;
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "create permission error %d (%s)\n",
							      err_code,(char*)err_msg);
					return -1;
				} else {
					TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "unknown create permission response\n");
					/* Try again ? */
				}
			} else {
				perror("recv");
				exit(-1);
			}
		}
	}

	return 0;
}

int start_connection(uint16_t clnet_remote_port,
		     const char *remote_address, 
		     const unsigned char* ifname, const char *local_address,
		     int verbose,
		     app_ur_conn_info *clnet_info,
		     uint16_t *chn,
		     app_ur_conn_info *clnet_info_rtcp,
		     uint16_t *chn_rtcp) {

	ioa_addr relay_addr;
	ioa_addr relay_addr_rtcp;

	if (clnet_connect(clnet_remote_port, remote_address, ifname, local_address,
			verbose, clnet_info) < 0) {
	  exit(-1);
	}

	if(!no_rtcp) {
	  if (clnet_connect(clnet_remote_port, remote_address, ifname, local_address,
			  verbose, clnet_info_rtcp) < 0) {
	    exit(-1);
	  }
	}

	if(!no_rtcp) {
	  if (clnet_allocate(verbose, clnet_info, &relay_addr, get_allocate_address_family(&peer_addr)) < 0) {
	    exit(-1);
	  }
	  
	  if (clnet_allocate(verbose, clnet_info_rtcp, &relay_addr_rtcp, get_allocate_address_family(&peer_addr)) < 0) {
	    exit(-1);
	  }
	} else {
	  if (clnet_allocate(verbose, clnet_info, &relay_addr, get_allocate_address_family(&peer_addr)) < 0) {
	    exit(-1);
	  }
	}

	if (!use_send_method) {
		ioa_addr some_addr;
		addr_cpy(&some_addr,&peer_addr);
		addr_set_port(&some_addr,addr_get_port(&some_addr)+1);
		if (turn_channel_bind(verbose, chn, clnet_info, &some_addr) < 0) {
			exit(-1);
		}
		if (turn_channel_bind(verbose, chn, clnet_info, &some_addr) < 0) {
			exit(-1);
		}
		if (turn_channel_bind(verbose, chn, clnet_info, &peer_addr) < 0) {
			exit(-1);
		}
		if (turn_channel_bind(verbose, chn, clnet_info, &peer_addr) < 0) {
			exit(-1);
		}
		if(!no_rtcp) {
		  if (turn_channel_bind(verbose, chn_rtcp, clnet_info_rtcp, &peer_addr) < 0) {
		    exit(-1);
		  }
		}
	} else {
		if (turn_create_permission(verbose, clnet_info, &peer_addr) < 0) {
			exit(-1);
		}
		if(!no_rtcp) {
		  if (turn_create_permission(verbose, clnet_info_rtcp, &peer_addr)
		      < 0) {
		    exit(-1);
		  }
		}
	}

	addr_cpy(&(clnet_info->peer_addr), &peer_addr);
	if(!no_rtcp) 
	  addr_cpy(&(clnet_info_rtcp->peer_addr), &peer_addr);

	return 0;
}


int start_c2c_connection(uint16_t clnet_remote_port,
		const char *remote_address, const unsigned char* ifname,
		const char *local_address, int verbose, app_ur_conn_info *clnet_info1,
		uint16_t *chn1, app_ur_conn_info *clnet_info1_rtcp,
		uint16_t *chn1_rtcp,
		app_ur_conn_info *clnet_info2, uint16_t *chn2,
		app_ur_conn_info *clnet_info2_rtcp,
		uint16_t *chn2_rtcp) {

	ioa_addr relay_addr1;
	ioa_addr relay_addr1_rtcp;

	ioa_addr relay_addr2;
	ioa_addr relay_addr2_rtcp;

	if (clnet_connect(clnet_remote_port, remote_address, ifname, local_address,
			verbose, clnet_info1) < 0) {
		exit(-1);
	}

	if(!no_rtcp) 
	  if (clnet_connect(clnet_remote_port, remote_address, ifname, local_address,
			  verbose, clnet_info1_rtcp) < 0) {
	    exit(-1);
	  }

	if (clnet_connect(clnet_remote_port, remote_address, ifname, local_address,
			verbose, clnet_info2) < 0) {
		exit(-1);
	}

	if(!no_rtcp) 
	  if (clnet_connect(clnet_remote_port, remote_address, ifname, local_address,
			  verbose, clnet_info2_rtcp) < 0) {
	    exit(-1);
	  }

	if(!no_rtcp) {
	  if (clnet_allocate(verbose, clnet_info1, &relay_addr1, default_address_family)
	      < 0) {
	    exit(-1);
	  }
	  
	  if (clnet_allocate(verbose, clnet_info1_rtcp,
			   &relay_addr1_rtcp, default_address_family) < 0) {
	    exit(-1);
	  }
	  
	  if (clnet_allocate(verbose, clnet_info2, &relay_addr2, default_address_family)
	      < 0) {
	    exit(-1);
	  }
	  
	  if (clnet_allocate(verbose, clnet_info2_rtcp,
			   &relay_addr2_rtcp, default_address_family) < 0) {
	    exit(-1);
	  }
	} else {
	  if (clnet_allocate(verbose, clnet_info1, &relay_addr1, default_address_family)
	      < 0) {
	    exit(-1);
	  }	  
	  if (clnet_allocate(verbose, clnet_info2, &relay_addr2, default_address_family)
	      < 0) {
	    exit(-1);
	  }
	}

	if (!use_send_method) {
		if (turn_channel_bind(verbose, chn1, clnet_info1, &relay_addr2) < 0) {
			exit(-1);
		}
		if(!no_rtcp)
		  if (turn_channel_bind(verbose, chn1_rtcp, clnet_info1_rtcp,
					&relay_addr2_rtcp) < 0) {
		    exit(-1);
		  }
		if (turn_channel_bind(verbose, chn2, clnet_info2, &relay_addr1) < 0) {
			exit(-1);
		}
		if(!no_rtcp)
		  if (turn_channel_bind(verbose, chn2_rtcp, clnet_info2_rtcp,
					&relay_addr1_rtcp) < 0) {
		    exit(-1);
		  }
	} else {
		if (turn_create_permission(verbose, clnet_info1, &relay_addr2) < 0) {
			exit(-1);
		}
		if(!no_rtcp)
		  if (turn_create_permission(verbose, clnet_info1_rtcp, &relay_addr2_rtcp)
		      < 0) {
		    exit(-1);
		  }
		if (turn_create_permission(verbose, clnet_info2, &relay_addr1) < 0) {
			exit(-1);
		}
		if(!no_rtcp)
		  if (turn_create_permission(verbose, clnet_info2_rtcp, &relay_addr1_rtcp)
		      < 0) {
		    exit(-1);
		  }
	}

	addr_cpy(&(clnet_info1->peer_addr), &relay_addr2);
	if(!no_rtcp)
	  addr_cpy(&(clnet_info1_rtcp->peer_addr), &relay_addr2_rtcp);
	addr_cpy(&(clnet_info2->peer_addr), &relay_addr1);
	if(!no_rtcp)
	  addr_cpy(&(clnet_info2_rtcp->peer_addr), &relay_addr1_rtcp);

	return 0;
}

