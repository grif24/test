/* 
   reference avx implementation of the dream source code package - 256 bit security 
   Copyright (C) <2018>  <Grif Hughes>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>. 
   */
#include "../common/dream_impl.h"
#include "../common/dream.h"
#include "dream_round256_avx.h"

#include <immintrin.h>

static const uint64_t rc[8] = { 
	0x243F6A8885A308D3,
	0x13198A2E03707344,
	0xA4093822299F31D0,
	0x082EFA98EC4E6C89,
	0x452821E638D01377,
	0xBE5466CF34E90C6C,
	0xA4093822299F31D0,
	0x082EFA98EC4E6C89
};

static void f(uint8_t *state)
{
	uint64_t s[24];
	for(size_t i = 0; i < 24; i++)
		s[i] = load64(state + (i * sizeof *s));

	__m256i as = _mm256_set_epi64x(s[3],s[2],s[1],s[0]);
	__m256i bs = _mm256_set_epi64x(s[7],s[6],s[5],s[4]);
	__m256i cs = _mm256_set_epi64x(s[11],s[10],s[9],s[8]);
	__m256i ds = _mm256_set_epi64x(s[15],s[14],s[13],s[12]);
	__m256i es = _mm256_set_epi64x(s[19],s[18],s[17],s[16]);
	__m256i fs = _mm256_set_epi64x(s[23],s[22],s[21],s[20]);

	ROUND(0);
	ROUND(2);
	ROUND(4);
	ROUND(6);

	 s[0] = _mm256_extract_epi64(as, 0);
	 s[1] = _mm256_extract_epi64(as, 1);
	 s[2] = _mm256_extract_epi64(as, 2);
	 s[3] = _mm256_extract_epi64(as, 3);
	 s[4] = _mm256_extract_epi64(bs, 0);
	 s[5] = _mm256_extract_epi64(bs, 1);
	 s[6] = _mm256_extract_epi64(bs, 2);
	 s[7] = _mm256_extract_epi64(bs, 3);
	 s[8] = _mm256_extract_epi64(cs, 0);
	 s[9] = _mm256_extract_epi64(cs, 1);
	s[10] = _mm256_extract_epi64(cs, 2);
	s[11] = _mm256_extract_epi64(cs, 3);
	s[12] = _mm256_extract_epi64(ds, 0);
	s[13] = _mm256_extract_epi64(ds, 1);
	s[14] = _mm256_extract_epi64(ds, 2);
	s[15] = _mm256_extract_epi64(ds, 3);
	s[16] = _mm256_extract_epi64(es, 0);
	s[17] = _mm256_extract_epi64(es, 1);
	s[18] = _mm256_extract_epi64(es, 2);
	s[19] = _mm256_extract_epi64(es, 3);
	s[20] = _mm256_extract_epi64(fs, 0);
	s[21] = _mm256_extract_epi64(fs, 1);
	s[22] = _mm256_extract_epi64(fs, 2);
	s[23] = _mm256_extract_epi64(fs, 3);

	for(size_t i = 0; i < 24; i++)
		store64(state + (i * sizeof *s), s[i]);
}

#define R 128 

static void absorb(uint8_t *state, const uint8_t *in, size_t in_l, uint8_t pad)
{
	for(; in_l >= R; in += R, in_l -= R) {
		for(size_t i = 0; i < R; i++) 
			state[i] ^= in[i];        
		f(state);
	}
	for(size_t i = 0; i < in_l; i++)
		state[i] ^= in[i];
	state[in_l] ^= pad;
	state[R - 1] ^= 0x80;

	f(state);
}

static void squeeze(uint8_t *state, uint8_t *out, size_t out_l)
{
	for(; out_l > R; out += R, out_l -= R) {
		for(size_t i = 0; i < R; i++)
			out[i] = state[i];
		f(state);
	}
	for(size_t i = 0; i < out_l; i++)
		out[i] = state[i];
}

static void key_init(uint8_t *state, const uint8_t *key, size_t kl, const uint8_t *nonce, size_t nl)
{
        absorb(state, key, kl, PAD_KEYSTREAM);
        absorb(state, nonce, nl, PAD_KEYSTREAM);
}

void dream256_hash(const uint8_t *buf, size_t bl, uint8_t *digest)
{
        uint8_t state[192] = {0};
        absorb(state, buf, bl, PAD_HASH);
        squeeze(state, digest, dream256_DIGEST); 
}

void dream256_wrap(const uint8_t *key,
                const uint8_t *header, size_t hl, 
                const uint8_t *body, size_t bl, 
                uint8_t *cgram, 
                uint8_t *tag)
{
        if(hl >= R)
                return;

        uint8_t state[192] = {0};
        key_init(state, key, dream256_KEY, header, hl);

        size_t P = R - 1;
        for(; bl > P; body += P, cgram += P, bl -= P) {
                for(size_t i = 0; i < P; i++) 
                        cgram[i] = body[i] ^ state[i];
                absorb(state, body, P, PAD_KEYSTREAM); 
        }
        for(size_t i = 0; i < bl; i++)
                cgram[i] = body[i] ^ state[i];
        absorb(state, body, bl, PAD_TAG);
        squeeze(state, tag, dream256_DIGEST);
}

int dream256_unwrap(const uint8_t *key,
                const uint8_t *header, size_t hl,
                const uint8_t *cgram, size_t cl, 
                uint8_t *body, 
                uint8_t *tag)
{
        if(hl >= R)
                return 0;

        uint8_t state[192] = {0};
        key_init(state, key, dream256_KEY, header, hl);

        size_t P = R - 1;
        for(; cl > P; body += P, cgram += P, cl -= P) {
                for(size_t i = 0; i < P; i++) 
                        body[i] = cgram[i] ^ state[i];
                absorb(state, body, P, PAD_KEYSTREAM); 
        }
        for(size_t i = 0; i < cl; i++)
                body[i] = cgram[i] ^ state[i];
        absorb(state, body, cl, PAD_TAG);

        if(cnst_memcmp(state, tag, dream256_DIGEST) == 0) {
                return 1;
        } else {
                for(size_t i = 0; i < cl; i++)
                        body[i] = 0;
                return -1;
        }
}
