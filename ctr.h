//
//  ctr.h
//  aes
//
//  Created by Victor Cochard on 11/24/18.
//  Copyright © 2018 Victor Cochard. All rights reserved.
//

#ifndef ctr_h
#define ctr_h

#include "aes.h"
#include "types.h"
#include "defs.h"


/**
 Encrypts the message using CTR-AES

 @param message Message to encrypt
 @param ciphertext Pointer to the future encrypted message
 @param size Size of message & ciphertext
 @param sequence Pseudo-Random sequence to use for CTR. Must be of size 'size' rounded up by 128-bits (16 bytes)
 @param key AES-Key to use for encryption
 @param keySize Size of the key
 */
void ctr_encrypt(const u_int8_t message[], u_int8_t ciphertext[], size_t size, const u_int8_t sequence[], key key, unsigned int keySize);


/**
 Decrypts the message using CTR-AES
 
 @param ciphertext Message to decrypt
 @param message Pointer to the future plaintext
 @param size Size of ciphertext & message
 @param sequence Pseudo-Random sequence to use for CTR. Must be of size 'size' rounded up by 128-bits (16 bytes)
 @param key AES-Key to use for encryption
 @param keySize Size of the key
 */
void ctr_decrypt(const u_int8_t ciphertext[], u_int8_t message[], size_t size, const u_int8_t sequence[], key key, unsigned int keySize);

#endif /* ctr_h */
