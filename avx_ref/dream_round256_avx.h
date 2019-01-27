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

#define _mm256_rotr_epi64(a,n) (_mm256_or_si256(_mm256_srli_epi64(a,n), _mm256_slli_epi64(a,64-n)))

#define Q(a,b,c,r) 							        \
	do { 									\
		b = _mm256_add_epi64(b, a); 					\
		c = _mm256_add_epi64(c, _mm256_rotr_epi64(b,r));		\
		a = _mm256_xor_si256(a, c);		                        \
	} while(0)
#define MIX(a,b,c)                            				        \
	do {                                    				\
		Q(a,b,c,32);						        \
		Q(a,b,c,15);						        \
		Q(a,b,c,48);						        \
	} while(0)
#define SLICE(b,c)								\
	do { 									\
		b = _mm256_permute4x64_epi64(b, _MM_SHUFFLE(0, 3, 2, 1));	\
		c = _mm256_permute4x64_epi64(c, _MM_SHUFFLE(1, 0, 3, 2));	\
	} while(0)
#define UNSLICE(b,c)								\
	do { 									\
		b = _mm256_permute4x64_epi64(b, _MM_SHUFFLE(2, 1, 0, 3));	\
		c = _mm256_permute4x64_epi64(c, _MM_SHUFFLE(1, 0, 3, 2));	\
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
