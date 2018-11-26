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
#define KEY_SIZE 4  // Size in words
typedef u_int32_t key[KEY_SIZE];

// Will be replaced by variables as we implement AES 192 & 256
#define BLOCK_COLUMNS 4
#define BLOCK_LENGTH 16
#define ROUNDS 10

/**
 Uses the master key to span al the children keys needed by the algorithm

 @param key Master key
 @param roundKeys Pointer where the keys will be written. Array must be of size BLOCK_COLUMNS*(ROUNDS+1)
 @param keySize Size of the master key (in words)
 */
void keyExpansion(key key, u_int32_t roundKeys[BLOCK_COLUMNS*(ROUNDS+1)], unsigned int keySize);

/**
 Encrypts the input using the given Round Keys.

 @param in Plaintext to be encrypted (128-bits block)
 @param out Pointer where the ciphertext will be saved (128-bits)
 @param roundKeys Keys to give to the algorithm (Size : BLOCK_COLUMNS*(ROUNDS+1))
 */
void aes_encrypt(const u_int8_t in[BLOCK_ROWS*BLOCK_COLUMNS], u_int8_t out[BLOCK_ROWS*BLOCK_COLUMNS], u_int32_t roundKeys[BLOCK_COLUMNS*(ROUNDS+1)]);

/**
 Decrypts the input using the given Round Keys.
 
 @param in Ciphertext to be encrypted (128-bits block)
 @param out Pointer where the plaintext will be saved (128-bits)
 @param roundKeys Keys to give to the algorithm (Size : BLOCK_COLUMNS*(ROUNDS+1))
 */
void aes_decrypt(const u_int8_t in[BLOCK_ROWS*BLOCK_COLUMNS], u_int8_t out[BLOCK_ROWS*BLOCK_COLUMNS], u_int32_t roundKeys[BLOCK_COLUMNS*(ROUNDS+1)]);


/**
 Non-functional method only for compilation purposes. DO NOT CALL !!!!!
 */
//void aesblockcipher(char*, int*, char*);

#endif /* aes_h */
