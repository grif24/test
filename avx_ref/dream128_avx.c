/* 
   reference avx implementation of the dream source code package - 128 bit security 
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
#include "dream_round128_avx.h"

#include <emmintrin.h>
#include <smmintrin.h>

static const uint32_t rc[8] = { 
        0x243F6A88,
        0x85A308D3,
        0x13198A2E,
        0x03707344,
        0xA4093822,
        0x299F31D0,
        0x082EFA98,
        0xEC4E6C89
};

static void f(uint8_t *state)
{
	uint32_t s[24];
	for(size_t i = 0; i < 24; i++)
		s[i] = load32(state + (i * sizeof *s));

	__m128i as = _mm_set_epi32(s[3],s[2],s[1],s[0]);
	__m128i bs = _mm_set_epi32(s[7],s[6],s[5],s[4]);
	__m128i cs = _mm_set_epi32(s[11],s[10],s[9],s[8]);
	__m128i ds = _mm_set_epi32(s[15],s[14],s[13],s[12]);
	__m128i es = _mm_set_epi32(s[19],s[18],s[17],s[16]);
	__m128i fs = _mm_set_epi32(s[23],s[22],s[21],s[20]);

	ROUND(0);
	ROUND(2);
	ROUND(4);
	ROUND(6);

	 s[0] = _mm_extract_epi32(as, 0);
	 s[1] = _mm_extract_epi32(as, 1);
	 s[2] = _mm_extract_epi32(as, 2);
	 s[3] = _mm_extract_epi32(as, 3);
	 s[4] = _mm_extract_epi32(bs, 0);
	 s[5] = _mm_extract_epi32(bs, 1);
	 s[6] = _mm_extract_epi32(bs, 2);
	 s[7] = _mm_extract_epi32(bs, 3);
	 s[8] = _mm_extract_epi32(cs, 0);
	 s[9] = _mm_extract_epi32(cs, 1);
	s[10] = _mm_extract_epi32(cs, 2);
	s[11] = _mm_extract_epi32(cs, 3);
	s[12] = _mm_extract_epi32(ds, 0);
	s[13] = _mm_extract_epi32(ds, 1);
	s[14] = _mm_extract_epi32(ds, 2);
	s[15] = _mm_extract_epi32(ds, 3);
	s[16] = _mm_extract_epi32(es, 0);
	s[17] = _mm_extract_epi32(es, 1);
	s[18] = _mm_extract_epi32(es, 2);
	s[19] = _mm_extract_epi32(es, 3);
	s[20] = _mm_extract_epi32(fs, 0);
	s[21] = _mm_extract_epi32(fs, 1);
	s[22] = _mm_extract_epi32(fs, 2);
	s[23] = _mm_extract_epi32(fs, 3);

	for(size_t i = 0; i < 24; i++)
		store32(state + (i * sizeof *s), s[i]);
}

#define R 64 

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

void dream128_hash(const uint8_t *buf, size_t bl, uint8_t *digest)
{
        uint8_t state[96] = {0};
        absorb(state, buf, bl, PAD_HASH);
        squeeze(state, digest, dream128_DIGEST); 
}

void dream128_wrap(const uint8_t *key,
                const uint8_t *header, size_t hl, 
                const uint8_t *body, size_t bl, 
                uint8_t *cgram, 
                uint8_t *tag)
{
        if(hl >= R)
                return;

        uint8_t state[96] = {0};
        key_init(state, key, dream128_KEY, header, hl);

        size_t P = R - 1;
        for(; bl > P; body += P, cgram += P, bl -= P) {
                for(size_t i = 0; i < P; i++) 
                        cgram[i] = body[i] ^ state[i];
                absorb(state, body, P, PAD_KEYSTREAM); 
        }
        for(size_t i = 0; i < bl; i++)
                cgram[i] = body[i] ^ state[i];
        absorb(state, body, bl, PAD_TAG);
        squeeze(state, tag, dream128_DIGEST);
}

int dream128_unwrap(const uint8_t *key,
                const uint8_t *header, size_t hl,
                const uint8_t *cgram, size_t cl, 
                uint8_t *body, 
                uint8_t *tag)
{
        if(hl >= R)
                return 0;

        uint8_t state[96] = {0};
        key_init(state, key, dream128_KEY, header, hl);

        size_t P = R - 1;
        for(; cl > P; body += P, cgram += P, cl -= P) {
                for(size_t i = 0; i < P; i++) 
                        body[i] = cgram[i] ^ state[i];
                absorb(state, body, P, PAD_KEYSTREAM); 
        }
        for(size_t i = 0; i < cl; i++)
                body[i] = cgram[i] ^ state[i];
        absorb(state, body, cl, PAD_TAG);

        if(cnst_memcmp(state, tag, dream128_DIGEST) == 0) {
                return 1;
        } else {
                for(size_t i = 0; i < cl; i++)
                        body[i] = 0;
                return -1;
        }
}
