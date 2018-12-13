//
//  aes.h
//  aes
//
//  Created by Victor Cochard on 11/19/18.
//  Copyright © 2018 Victor Cochard. All rights reserved.
//

#ifndef aes_h
#define aes_h

#include "types.h"
#include "defs.h"

#define BLOCK_SHIFT 2
#define BLOCK_ROWS 4
typedef u_int32_t key[];

// Will be replaced by variables as we implement AES 192 & 256
#define BLOCK_COLUMNS 4
#define BLOCK_LENGTH 16

#define AES_128_KEY_SIZE 4
#define AES_192_KEY_SIZE 6
#define AES_256_KEY_SIZE 8

#define KEY_SIZE_ROUNDS(n) (6 + (n))

/**
 Uses the master key to span al the children keys needed by the algorithm

 @param key Master key
 @param roundKeys Pointer where the keys will be written. Array must be of size BLOCK_COLUMNS*(ROUNDS+1)
 @param keySize Size of the master key (in words)
 */
void keyExpansion(key key, u_int32_t roundKeys[], unsigned int keySize);

/**
 Encrypts the input using the given Round Keys.

 @param in Plaintext to be encrypted (128-bits block)
 @param out Pointer where the ciphertext will be saved (128-bits)
 @param roundKeys Keys to give to the algorithm (Size : BLOCK_COLUMNS*(ROUNDS+1))
 @param keySize Size of the master key (in words)
 */
void aes_encrypt(const u_int8_t in[BLOCK_LENGTH], u_int8_t out[BLOCK_LENGTH], u_int32_t roundKeys[], unsigned int keySize);

/**
 Decrypts the input using the given Round Keys.

 @param in Ciphertext to be encrypted (128-bits block)
 @param out Pointer where the plaintext will be saved (128-bits)
 @param roundKeys Keys to give to the algorithm (Size : BLOCK_COLUMNS*(ROUNDS+1))
 @param keySize Size of the master key (in words)
 */
void aes_decrypt(const u_int8_t in[BLOCK_LENGTH], u_int8_t out[BLOCK_LENGTH], u_int32_t roundKeys[], unsigned int keySize);


#endif /* aes_h */
