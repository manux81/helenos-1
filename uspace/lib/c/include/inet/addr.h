/*
 * Copyright (c) 2013 Jiri Svoboda
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup libc
 * @{
 */
/** @file
 */

#ifndef LIBC_INET_ADDR_H_
#define LIBC_INET_ADDR_H_

#include <stdint.h>
#include <net/in.h>

#define INET_ADDR_SIZE  16

/** Node address */
typedef struct {
	uint16_t family;
	uint8_t addr[INET_ADDR_SIZE];
} inet_addr_t;

/** Network address */
typedef struct {
	/** Address family */
	uint16_t family;
	
	/** Address */
	uint8_t addr[INET_ADDR_SIZE];
	
	/** Number of valid bits */
	uint8_t prefix;
} inet_naddr_t;

extern int inet_addr_family(const char *, uint16_t *);

extern int inet_addr_parse(const char *, inet_addr_t *);
extern int inet_naddr_parse(const char *, inet_naddr_t *);

extern int inet_addr_format(inet_addr_t *, char **);
extern int inet_naddr_format(inet_naddr_t *, char **);

extern int inet_addr_pack(inet_addr_t *, uint32_t *);
extern int inet_naddr_pack(inet_naddr_t *, uint32_t *, uint8_t *);

extern void inet_addr_unpack(uint32_t, inet_addr_t *);
extern void inet_naddr_unpack(uint32_t, uint8_t, inet_naddr_t *);

extern int inet_addr_sockaddr_in(inet_addr_t *, sockaddr_in_t *);
extern void inet_naddr_addr(inet_naddr_t *, inet_addr_t *);

extern void inet_addr(inet_addr_t *, uint8_t, uint8_t, uint8_t, uint8_t);
extern void inet_naddr(inet_naddr_t *, uint8_t, uint8_t, uint8_t, uint8_t,
    uint8_t);

extern void inet_addr_any(inet_addr_t *);
extern void inet_naddr_any(inet_naddr_t *);

extern int inet_addr_compare(inet_addr_t *, inet_addr_t *);
extern int inet_addr_is_any(inet_addr_t *);

#endif

/** @}
 */