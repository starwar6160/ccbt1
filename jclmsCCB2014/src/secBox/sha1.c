/* sha1.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h> 
#include <stdint.h>
#include "sha1.h"

/********************************************************************************************************/
/* some helping functions */
static uint32_t rotl32(uint32_t n, uint8_t bits)
{
	return ((n << bits) | (n >> (32 - bits)));
}

static uint32_t change_endian32(uint32_t x)
{
	return (((x) << 24) | ((x) >> 24) | (((x) & 0x0000ff00) << 8) | (((x) & 0x00ff0000) >> 8));
}

/* three SHA-1 inner functions */
static uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
{
	return ((x & y) | ((~x) & z));
}

static uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
{
	return ((x & y) | (x & z) | (y & z));
}

static uint32_t parity(uint32_t x, uint32_t y, uint32_t z)
{
	return (x ^ y ^ z);
}

typedef struct {
	uint32_t h[5];
	uint64_t length;
} sha1_ctx_t;

/** \fn sha1_nextBlock(sha1_ctx_t *state, const void* block)
 *  \brief process one input block
 * This function processes one input block and updates the hash context 
 * accordingly
 * \param state pointer to the state variable to update
 * \param block pointer to the message block to process
 */
static void sha1_nextBlock (sha1_ctx_t *state, uint32_t *block)
{
	uint8_t t, i;
	uint32_t a[5], w[17], temp;
	uint32_t k[] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};
    uint32_t (*f[])(uint32_t x, uint32_t y, uint32_t z) = {ch, parity, maj, parity};

	/* load the state */
	for(i = 0; i < 5; ++i)
		a[i] = state->h[i];

	/* the fun stuff */
	for(t = 0; t < 80; ++t){
        if(t < 16)
        #if BYTE_ORDER == LITTLE_ENDIAN
            w[16] = change_endian32(block[t]);
        #else
            w[16] = block[t];
        #endif
		else
			w[16] = rotl32(w[13] ^ w[8] ^ w[2] ^ w[0], 1);

		temp = rotl32(a[0], 5) + f[t / 20](a[1], a[2], a[3]) + a[4] + w[16] + k[t / 20];
		a[4] = a[3]; a[3] = a[2]; a[2] = rotl32(a[1], 30); a[1] = a[0]; a[0] = temp;
        
        for(i = 0; i < 16; ++i)
            w[i] = w[i + 1];
	}

	/* update the state */
	for(i = 0; i < 5; ++i)
		state->h[i] += a[i];
	
	state->length += 512;
}

/** \fn sha1_ctx2hash(sha1_hash_t *dest, sha1_ctx_t *state)
 * \brief convert a state variable into an actual hash value
 * Writes the hash value corresponding to the state to the memory pointed by dest.
 * \param dest pointer to the hash value destination
 * \param state pointer to the hash context
 */ 
static void sha1_lastBlock(sha1_ctx_t *state, uint32_t *block, uint16_t length)
{
	uint8_t buf[64]; /* local block */
	uint8_t i, mod = length % 8, len = (length + 7) >> 3;
    
	memset(buf, 0, 64);
    
    if(len) memcpy(buf, block, len);

    if(!mod) ++len;
    else buf[len] &= 0xff << (8 - mod);
    
	/* set the final one bit */
	buf[len] |= (uint8_t)0x80 >> mod;

     /* not enouth space for 64bit length value */
	if (length + 1 > 448){
		sha1_nextBlock(state, (uint32_t *)buf);
		state->length -= 512;
		memset(buf, 0, 64);
	}
    
    state->length += length;
	/* store the 64bit length value */
	for (i = 0; i < 8; ++i)
#if BYTE_ORDER == LITTLE_ENDIAN
		buf[56 + i] = ((uint8_t*)&state->length)[7 - i];
#else
		buf[56 + i] = ((uint8_t*)&state->length)[i];
#endif
	sha1_nextBlock(state, (uint32_t *)buf);
}

/** \fn sha1(sha1_hash_t *dest, const void* msg, uint32_t length_b)
 * \brief hashing a message which in located entirely in RAM
 * This function automatically hashes a message which is entirely in RAM with
 * the SHA-1 hashing algorithm.
 * \param dest pointer to the hash value destination
 * \param msg  pointer to the message which should be hashed
 * \param length_b length of the message in bits
 */ 
void sha1 (void *dest, const void* msg, uint32_t length)
{
	uint8_t i;
	sha1_ctx_t s;
    uint32_t *src = (uint32_t *)msg;
	
	s.h[0] = 0x67452301;
	s.h[1] = 0xefcdab89;
	s.h[2] = 0x98badcfe;
	s.h[3] = 0x10325476;
	s.h[4] = 0xc3d2e1f0;
	s.length = 0;
    
	while(length >= 512){
		sha1_nextBlock(&s, src);
        src += 16; /* increment pointer to next block */
		length -= 512;
	}
    
	sha1_lastBlock(&s, src, length);
    
	for(i = 0; i < 5; ++i)
#if BYTE_ORDER == LITTLE_ENDIAN
		((uint32_t*)dest)[i] = change_endian32(s.h[i]);
#else
		((uint32_t*)dest)[i] = s.h[i];
#endif
}
