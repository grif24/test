/* 
   reference implementation of the dream source code package
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
#ifndef DREAM_IMPL_H
#define DREAM_IMPL_H

#include <string.h>
#include <stdint.h>

static inline void store64(uint8_t *d, uint64_t w)
{
        memcpy(d, &w, sizeof w);
        /*
        d[0] = (uint8_t)(w >>  0);
        d[1] = (uint8_t)(w >>  8);
        d[2] = (uint8_t)(w >> 16);
        d[3] = (uint8_t)(w >> 24);
        d[4] = (uint8_t)(w >> 32);
        d[5] = (uint8_t)(w >> 40);
        d[6] = (uint8_t)(w >> 48);
        d[7] = (uint8_t)(w >> 56);
        */
}

static inline uint64_t load64(const uint8_t *s)
{
        uint64_t w;
        memcpy(&w, s, sizeof w);
        return w;
        /*
        return ((uint64_t )(s[0]) << 0) |
                ((uint64_t )(s[1]) << 8) |
                ((uint64_t )(s[2]) << 16) |
                ((uint64_t )(s[3]) << 24) |
                ((uint64_t )(s[4]) << 32) |
                ((uint64_t )(s[5]) << 40) |
                ((uint64_t )(s[6]) << 48) |
                ((uint64_t )(s[7]) << 56) ;
                */
}

static inline uint64_t ror64(const uint64_t v, const unsigned n)
{
        return (v >> n) | (v << (64 - n));
}

static inline int cnst_memcmp(const void *a, const void *b, const unsigned size)
{
        const uint8_t *_a = (const uint8_t *)a;
        const uint8_t *_b = (const uint8_t *)b;
        uint8_t result = 0;

        for(unsigned i = 0; i < size; i++) {
                result |= _a[i] ^ _b[i];
        }

        return result;
}
#endif
