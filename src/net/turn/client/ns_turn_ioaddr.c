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

#include "ns_turn_ioaddr.h"

//////////////////////////////////////////////////////////////

u32bits get_ioa_addr_len(const ioa_addr* addr) {
  if(addr->ss.ss_family == AF_INET) return sizeof(struct sockaddr_in);
  else if(addr->ss.ss_family == AF_INET6) return sizeof(struct sockaddr_in6);
  return 0;
}

///////////////////////////////////////////////////////////////

void addr_set_any(ioa_addr *addr) {
  ns_bzero(addr,sizeof(ioa_addr));
}

int addr_any(const ioa_addr* addr) {
  if(addr->ss.ss_family == AF_INET) {
    return ((addr->s4.sin_addr.s_addr==0)&&(addr->s4.sin_port==0));
  } else if(addr->ss.ss_family == AF_INET6) {
    if(addr->s6.sin6_port!=0) return 0;
    else {
      size_t i;
      for(i=0;i<sizeof(addr->s6.sin6_addr);i++) 
	if(((const s08bits*)&(addr->s6.sin6_addr))[i]) return 0;
    }
  }

  return 1;
}

int addr_any_no_port(const ioa_addr* addr) {
  if(addr->ss.ss_family == AF_INET) {
    return (addr->s4.sin_addr.s_addr==0);
  } else if(addr->ss.ss_family == AF_INET6) {
    size_t i;
    for(i=0;i<sizeof(addr->s6.sin6_addr);i++) 
      if(((const s08bits*)(&(addr->s6.sin6_addr)))[i]) return 0;
  }

  return 1;
}

static u32bits hash_int32(u32bits a)
{
	return (a+nswap32(a));
}

static u64bits hash_int64(u64bits a)
{
	return (a+nswap64(a));
}

u32bits addr_hash(const ioa_addr *addr)
{
	u32bits ret = 0;
	if (addr->ss.ss_family == AF_INET) {
		ret = hash_int32(addr->s4.sin_addr.s_addr + addr->s4.sin_port);
	} else {
		const u64bits *a = (const u64bits *) (&(addr->s6.sin6_addr));
		ret = (u32bits)((hash_int64(a[0])<<3) + (hash_int64(a[1] + addr->s6.sin6_port)));
	}
	return ret;
}

u32bits addr_hash_no_port(const ioa_addr *addr)
{
	u32bits ret = 0;
	if (addr->ss.ss_family == AF_INET) {
		ret = hash_int32(addr->s4.sin_addr.s_addr);
	} else {
		const u64bits *a = (const u64bits *) (&(addr->s6.sin6_addr));
		ret = (u32bits)((hash_int64(a[0])<<3) + (hash_int64(a[1])));
	}
	return ret;
}

void addr_cpy(ioa_addr* dst, const ioa_addr* src) {
  ns_bcopy(src,dst,sizeof(ioa_addr));
}

void addr_cpy4(ioa_addr* dst, const struct sockaddr_in* src) {
  ns_bcopy(src,dst,sizeof(struct sockaddr_in));
}

void addr_cpy6(ioa_addr* dst, const struct sockaddr_in6* src) {
  ns_bcopy(src,dst,sizeof(struct sockaddr_in6));
}

int addr_eq(const ioa_addr* a1, const ioa_addr *a2) {
  if(!a1) return (!a2);

  if(a1->ss.ss_family == a2->ss.ss_family) {
    if(a1->ss.ss_family == AF_INET) {
      if((int)a1->s4.sin_addr.s_addr == (int)a2->s4.sin_addr.s_addr 
	 && a1->s4.sin_port == a2->s4.sin_port) {
	return 1;
      }
    } else if(a1->ss.ss_family == AF_INET6) {
      const u64bits *p1=(const u64bits *)(&(a1->s6.sin6_addr));
      const u64bits *p2=(const u64bits *)(&(a2->s6.sin6_addr));
      if(p1[0]==p2[0] && p1[1]==p2[1] && a1->s6.sin6_port == a2->s6.sin6_port) {
	return 1;
      }
    }
  }

  return 0;
}

int addr_eq_no_port(const ioa_addr* a1, const ioa_addr *a2) {
  if(!a1) return (!a2);
  
  if(a1->ss.ss_family == a2->ss.ss_family) {
    if(a1->ss.ss_family == AF_INET) {
      if((int)a1->s4.sin_addr.s_addr == (int)a2->s4.sin_addr.s_addr) {
	return 1;
      }
    } else if(a1->ss.ss_family == AF_INET6) {
	const u64bits *p1=(const u64bits *)(&(a1->s6.sin6_addr));
	const u64bits *p2=(const u64bits *)(&(a2->s6.sin6_addr));
	if(p1[0]==p2[0] && p1[1]==p2[1]) {
	  return 1;
	}
    }
  }
  return 0;
}

int make_ioa_addr(const u08bits* saddr, int port, ioa_addr *addr) {

  if(!saddr || !addr) return -1;

  ns_bzero(addr, sizeof(ioa_addr));
  if((strlen((const s08bits*)saddr) == 0)||
     (inet_pton(AF_INET, (const s08bits*)saddr, &addr->s4.sin_addr) == 1)) {
    addr->s4.sin_family = AF_INET;
#if defined(__BSD_VISIBLE) /* no easy test for this one */
    addr->s4.sin_len = sizeof(struct sockaddr_in);
#endif
    addr->s4.sin_port = nswap16(port);
  } else if (inet_pton(AF_INET6, (const s08bits*)saddr, &addr->s6.sin6_addr) == 1) {
    addr->s6.sin6_family = AF_INET6;
#if defined(SIN6_LEN) /* this define is required by IPv6 if used */
    addr->s6.sin6_len = sizeof(struct sockaddr_in6);
#endif
    addr->s6.sin6_port = nswap16(port);
  } else {
    return -1;
  }

  return 0;
}

int addr_to_string(const ioa_addr* addr, u08bits* saddr)
{

	if (addr && saddr) {

		s08bits addrtmp[129];

		if (addr->ss.ss_family == AF_INET) {
			inet_ntop(AF_INET, &addr->s4.sin_addr, addrtmp, INET_ADDRSTRLEN);
		} else if (addr->ss.ss_family == AF_INET6) {
			inet_ntop(AF_INET6, &addr->s6.sin6_addr, addrtmp, INET6_ADDRSTRLEN);
		} else {
			return -1;
		}

		sprintf((s08bits*)saddr, "%s:%d", addrtmp, addr_get_port(addr));

		return 0;
	}

	return -1;
}

void addr_set_port(ioa_addr* addr, int port) {
  if(addr) {
    if(addr->s4.sin_family == AF_INET) {
      addr->s4.sin_port = nswap16(port);
    } else if(addr->s6.sin6_family == AF_INET6) {
      addr->s6.sin6_port = nswap16(port);
    }
  }
}

int addr_get_port(const ioa_addr* addr) {
  if(addr->s4.sin_family == AF_INET) {
    return nswap16(addr->s4.sin_port);
  } else if(addr->s6.sin6_family == AF_INET6) {
    return nswap16(addr->s6.sin6_port);
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////

void ioa_addr_range_set(ioa_addr_range* range, const ioa_addr* addr_min, const ioa_addr* addr_max) {
  if(range) {
    if(addr_min) addr_cpy(&(range->min),addr_min);
    else addr_set_any(&(range->min));
    if(addr_max) addr_cpy(&(range->max),addr_max);
    else addr_set_any(&(range->max));
  }
}

int addr_less_eq(const ioa_addr* addr1, const ioa_addr* addr2) {

  if(!addr1) return 1;
  else if(!addr2) return 0;
  else {
    if(addr1->ss.ss_family != addr2->ss.ss_family) return (addr1->ss.ss_family < addr2->ss.ss_family);
    else if(addr1->ss.ss_family == AF_INET) {
      return ((u32bits)(addr1->s4.sin_addr.s_addr) <= (u32bits)(addr2->s4.sin_addr.s_addr));
    } else if(addr1->ss.ss_family == AF_INET6) {
      int i;
      for(i=0;i<16;i++) {
	if((u08bits)(((const s08bits*)&(addr1->s6.sin6_addr))[i]) > (u08bits)(((const s08bits*)&(addr2->s6.sin6_addr))[i])) 
	  return 0;
      }
      return 1;
    } else return 1;
  }
}

int ioa_addr_in_range(const ioa_addr_range* range, const ioa_addr* addr) {

  if(range && addr) {
    if(addr_any(&(range->min)) || addr_less_eq(&(range->min),addr)) {
      if(addr_any(&(range->max))) {
	return 1;
      } else {
	return addr_less_eq(addr,&(range->max));
      }
    }
  }

  return 0;
}

void ioa_addr_range_cpy(ioa_addr_range* dest, const ioa_addr_range* src) {
  if(dest && src) {
    addr_cpy(&(dest->min),&(src->min));
    addr_cpy(&(dest->max),&(src->max));
  }
}

//////////////////////////////////////////////////////////////////////////////

