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

#define Q(a,b,c,d,r1,r2) 							        \
	do { 									\
		c = _mm256_add_epi64(c, a); 					\
		d = _mm256_add_epi64(d, b); 					\
		a = _mm256_xor_si256(a, d);		\
		b = _mm256_xor_si256(b, c);		\
		c = _mm256_add_epi64(c, _mm256_rotr_epi64(a,r1)); 					\
		d = _mm256_add_epi64(d, _mm256_rotr_epi64(b,r2)); 					\
	} while(0)
#define SLICE(b,c,d)								\
	do { 									\
		b = _mm256_permute4x64_epi64(b, _MM_SHUFFLE(0, 3, 2, 1));	\
		c = _mm256_permute4x64_epi64(c, _MM_SHUFFLE(1, 0, 3, 2));	\
		d = _mm256_permute4x64_epi64(d, _MM_SHUFFLE(2, 1, 0, 3));	\
	} while(0)
#define UNSLICE(b,c,d)								\
	do { 									\
		b = _mm256_permute4x64_epi64(b, _MM_SHUFFLE(2, 1, 0, 3));	\
		c = _mm256_permute4x64_epi64(c, _MM_SHUFFLE(1, 0, 3, 2));	\
		d = _mm256_permute4x64_epi64(d, _MM_SHUFFLE(0, 3, 2, 1));	\
	} while(0)
#define MIX(a,b,c,d)                            				        \
	do {                                    				\
		Q(a,b,c,d,11,32);						        \
		Q(a,b,c,d,48,53);						        \
		Q(a,b,c,d,61,24);						        \
	} while(0)
#define ROUND(n)                                				\
	do {                                    				\
		as[0] ^= rc[n];							\
										\
		MIX(as,bs,cs,ds); 						        \
		SLICE(bs,cs,ds);						        \
										\
		as[0] ^= rc[n+1];						\
										\
		MIX(as,bs,cs,ds); 						        \
		UNSLICE(bs,cs,ds);						        \
	} while(0)
#endif
