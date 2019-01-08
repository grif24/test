/* 
   reference avx implementation of the dream source code package
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
#ifndef DREAM_ROUND_AVX_H
#define DREAM_ROUND_AVX_H

#define _mm_rotr_epi32(a,n) (_mm_or_si128(_mm_srli_epi32(a,n), _mm_slli_epi32(a,32-n)))
#define Q(a,b,c,r) 							        \
	do { 									\
		b = _mm_add_epi32(b, a); 					\
		c = _mm_add_epi32(c, _mm_rotr_epi32(b,r));		        \
		a = _mm_xor_si128(a, c);		                        \
	} while(0)
#define MIX(a,b,c)                            				        \
	do {                                    				\
		Q(a,b,c,16);						        \
		Q(a,b,c,31);						        \
		Q(a,b,c,24);						        \
	} while(0)
#define SLICE(b,c)								\
	do { 									\
		b = _mm_shuffle_epi32(b, _MM_SHUFFLE(0, 3, 2, 1));	        \
		c = _mm_shuffle_epi32(c, _MM_SHUFFLE(1, 0, 3, 2));	        \
	} while(0)
#define UNSLICE(b,c)								\
	do { 									\
		b = _mm_shuffle_epi32(b, _MM_SHUFFLE(2, 1, 0, 3));	        \
		c = _mm_shuffle_epi32(c, _MM_SHUFFLE(1, 0, 3, 2));	        \
	} while(0)
#define ROUND(n)                                				\
	do {                                    				\
		as[0] ^= rc[n];							\
										\
		MIX(as,cs,es); 						        \
		SLICE(cs,es);						        \
		MIX(bs,ds,fs); 						        \
		SLICE(ds,fs);						        \
										\
		as[0] ^= rc[n+1];						\
										\
		MIX(bs,cs,es); 						        \
		UNSLICE(cs,es);						        \
		MIX(as,ds,fs); 						        \
		UNSLICE(ds,fs);						        \
	} while(0)
#endif
