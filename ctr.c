//
//  ctr.c
//  aes
//
//  Created by Victor Cochard on 11/24/18.
//  Copyright © 2018 Victor Cochard. All rights reserved.
//

#include "ctr.h"

/**
 * @brief Algorithm for CTR encryption and decryption (the algorithm is the same). Called by the appropriate methods
 */
void ctr(const u_int8_t in[], u_int8_t out[], const u_int8_t sequence[], size_t size, key key, unsigned int keySize) {
    if (keySize != KEY_SIZE) {
        cprintf("ERROR : Only AES-128 has been implemented by now. Only  the first 128-bits of the key will be used\n");
    }
    u_int32_t roundKeys[BLOCK_COLUMNS*(ROUNDS+1)];
    u_int8_t encrypted_sequence[BLOCK_ROWS*BLOCK_COLUMNS];
    // Compute the Round keys needed by AES
    keyExpansion(key, roundKeys, keySize);
    
    for (unsigned int index = 0; index < size; index += 1) {
        if (index % (BLOCK_ROWS*BLOCK_COLUMNS) == 0) {
            // Encrypt the sequence
            aes_encrypt(&sequence[index], &encrypted_sequence[index], roundKeys);
        }
        // And encrypt the message using the encrypted sequance (one-time pad)
        out[index] = encrypted_sequence[index] ^ in[index];
    }
}

void ctr_encrypt(const u_int8_t message[], u_int8_t ciphertext[], size_t size, const u_int8_t sequence[], key key, unsigned int keySize) {
    ctr(message, ciphertext, sequence, size, key, keySize);
}

void ctr_decrypt(const u_int8_t ciphertext[], u_int8_t message[], size_t size, const u_int8_t sequence[], key key, unsigned int keySize) {
    ctr(ciphertext, message, sequence, size, key, keySize);
}
