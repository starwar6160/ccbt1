/* sha1.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
 
#ifndef SHA1_H_
#define SHA1_H_

#include <stdint.h>
#include "cpu.h"

/** \fn sha1(sha1_hash_t *dest, const void* msg, uint32_t length_b)
 * \brief hashing a message which in located entirely in RAM
 * This function automatically hashes a message which is entirely in RAM with
 * the SHA-1 hashing algorithm.
 * \param dest pointer to the hash value destination
 * \param msg  pointer to the message which should be hashed
 * \param length_b length of the message in bits
 */ 
void sha1(void *dest, const void* msg, uint32_t length_b);

#endif /*SHA1_H_*/
