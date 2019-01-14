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
#ifndef DREAM_H 
#define DREAM_H

#include <stdint.h>
#include <stddef.h>

#define PAD_HASH 0x07
#define PAD_KEYSTREAM 0x05
#define PAD_TAG 0x06

enum dream128_cnst {
        dream128_DIGEST = 32,
        dream128_KEY = 16
};

enum dream256_cnst {
        dream256_DIGEST = 64,
        dream256_KEY = 32
};


size_t dream128_update(uint8_t *state, const uint8_t *in, size_t in_l);
void dream128_final(uint8_t *state, const uint8_t *in, size_t in_l);
void dream128_hash(const uint8_t *buf, size_t bl, uint8_t *digest);
void dream128_wrap(const uint8_t *key,
                const uint8_t *header, size_t hl, 
                const uint8_t *body, size_t bl, 
                uint8_t *cgram, 
                uint8_t *tag);
int dream128_unwrap(const uint8_t *key,
                const uint8_t *header, size_t hl,
                const uint8_t *cgram, size_t cl, 
                uint8_t *body, 
                const uint8_t *tag);

size_t dream256_update(uint8_t *state, const uint8_t *in, size_t in_l);
void dream256_final(uint8_t *state, const uint8_t *in, size_t in_l);
void dream256_hash(const uint8_t *buf, size_t bl, uint8_t *digest);
void dream256_wrap(const uint8_t *key,
                const uint8_t *header, size_t hl, 
                const uint8_t *body, size_t bl, 
                uint8_t *cgram, 
                uint8_t *tag);
int dream256_unwrap(const uint8_t *key,
                const uint8_t *header, size_t hl,
                const uint8_t *cgram, size_t cl, 
                uint8_t *body, 
                const uint8_t *tag);
#endif
