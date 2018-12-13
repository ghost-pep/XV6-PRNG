//
//  main.c
//  aes
//
//  Created by Victor Cochard on 11/14/18.
//  Copyright © 2018 Victor Cochard. All rights reserved.
//

#include "aes.h"

typedef u_int8_t block[BLOCK_LENGTH];

unsigned char * byteOfState(unsigned int row, unsigned int column, block state) {
    return &state[(row<<BLOCK_SHIFT)+column];
}

void loadToState(const u_int8_t in[BLOCK_ROWS*BLOCK_COLUMNS], block state) {
    for (unsigned int col = 0; col<BLOCK_COLUMNS; col += 1) {
        for (unsigned int row = 0; row<BLOCK_ROWS; row += 1) {
            *byteOfState(row, col, state) = in[(col*BLOCK_ROWS)+row];
        }
    }
}

void loadFromState(block state, u_int8_t out[BLOCK_ROWS*BLOCK_COLUMNS]) {
    for (unsigned int col = 0; col<BLOCK_COLUMNS; col += 1) {
        for (unsigned int row = 0; row<BLOCK_ROWS; row += 1) {
            out[(col*BLOCK_ROWS)+row] = *byteOfState(row, col, state);
        }
    }
}



const u_int16_t sBox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};

const u_int16_t invSBox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
};

const u_int32_t rcon[] = {
    0x00000000, 0x01000000, 0x02000000, 0x04000000,
    0x08000000, 0x10000000, 0x20000000, 0x40000000,
    0x80000000, 0x1B000000, 0x36000000 // for 128-bit blocks, Rijndael never uses more than 10 rcon values. First one is only a padding
};



int subBytes(block state) {
    for (unsigned char index = 0; index < BLOCK_LENGTH; index += 1) {
        state[index] = sBox[state[index]];
    }
    return 0;
}

int invSubBytes(block state) {
    for (unsigned char index = 0; index < BLOCK_LENGTH; index += 1) {
        state[index] = invSBox[state[index]];
    }
    return 0;
}


int shiftRows(block state) {
    for (unsigned int row = 0; row < BLOCK_ROWS; row += 1) {
        char storedRow[4];

        for (unsigned int column = 0; column < BLOCK_COLUMNS; column += 1) {
            storedRow[column] = *byteOfState(row, column, state);
        }

        for (unsigned int column = 0; column < BLOCK_COLUMNS; column += 1) {
            *byteOfState(row, column, state) = storedRow[(column + row)&3];
            // Here, the &3 replaces the %BLOCK_COLUMNS. Only works if BLOCK_COLUMNS=4
        }
    }
    return 0;
}

int invShiftRows(block state) {
    for (unsigned int row = 0; row < BLOCK_ROWS; row += 1) {
        char storedRow[4];

        for (unsigned int column = 0; column < BLOCK_COLUMNS; column += 1) {
            storedRow[column] = *byteOfState(row, column, state);
        }

        for (unsigned int column = 0; column < BLOCK_COLUMNS; column += 1) {
            *byteOfState(row, column, state) = storedRow[(column - row)&3];
            // Here, the &3 replaces the %BLOCK_COLUMNS. Only works if BLOCK_COLUMNS=4
        }
    }
    return 0;
}


unsigned char xTimes(u_int8_t i) {
    static const u_int8_t POLYNOMIAL_MODULUS = 0x1b;
    char needToXor = i & 0x80;
    i <<= 1;
    return needToXor ? i^POLYNOMIAL_MODULUS : i;
}

u_int8_t mult(u_int8_t mul, u_int8_t i) {
    u_int8_t result = 0;
    if (mul&1) {
        result ^= i;
    }
    mul >>= 1;
    while (mul != 0) {
        i = xTimes(i);
        if (mul&1) {
            result ^= i;
        }
        mul >>= 1;
    }
    return result;
}


int mixColumns(block state) {
    for (unsigned int column = 0; column<BLOCK_COLUMNS; column += 1) {
        char storedColumn[4];
        for (unsigned row = 0; row<BLOCK_ROWS; row += 1) {
            storedColumn[row] = *byteOfState(row, column, state);
        }

        *byteOfState(0, column, state) =    mult(2 ,storedColumn[0]) ^
                                            mult(3 ,storedColumn[1]) ^
                                            storedColumn[2] ^
                                            storedColumn[3];

        *byteOfState(1, column, state) =    storedColumn[0] ^
                                            mult(2 ,storedColumn[1]) ^
                                            mult(3 ,storedColumn[2]) ^
                                            storedColumn[3];

        *byteOfState(2, column, state) =    storedColumn[0] ^
                                            storedColumn[1] ^
                                            mult(2 ,storedColumn[2]) ^
                                            mult(3 ,storedColumn[3]);

        *byteOfState(3, column, state) =    mult(3 ,storedColumn[0]) ^
                                            storedColumn[1] ^
                                            storedColumn[2] ^
                                            mult(2 ,storedColumn[3]);
    }
    return 0;
}

int invMixColumns(block state) {
    for (unsigned int column = 0; column<BLOCK_COLUMNS; column += 1) {
        char storedColumn[4];
        for (unsigned row = 0; row<BLOCK_ROWS; row += 1) {
            storedColumn[row] = *byteOfState(row, column, state);
        }

        *byteOfState(0, column, state) =    mult(0xe ,storedColumn[0]) ^
                                            mult(0xb ,storedColumn[1]) ^
                                            mult(0xd ,storedColumn[2]) ^
                                            mult(0x9 ,storedColumn[3]);

        *byteOfState(1, column, state) =    mult(0x9 ,storedColumn[0]) ^
                                            mult(0xe ,storedColumn[1]) ^
                                            mult(0xb ,storedColumn[2]) ^
                                            mult(0xd ,storedColumn[3]);

        *byteOfState(2, column, state) =    mult(0xd ,storedColumn[0]) ^
                                            mult(0x9 ,storedColumn[1]) ^
                                            mult(0xe ,storedColumn[2]) ^
                                            mult(0xb ,storedColumn[3]);

        *byteOfState(3, column, state) =    mult(0xb ,storedColumn[0]) ^
                                            mult(0xd ,storedColumn[1]) ^
                                            mult(0x9 ,storedColumn[2]) ^
                                            mult(0xe ,storedColumn[3]);
    }
    return 0;
}


int addRoundKey(block state, u_int32_t key[]) {
    for (unsigned int col = 0; col < BLOCK_COLUMNS; col += 1) {
        *byteOfState(0, col, state) ^= (key[col] >> 24) & 0xFF;
        *byteOfState(1, col, state) ^= (key[col] >> 16) & 0xFF;
        *byteOfState(2, col, state) ^= (key[col] >> 8) & 0xFF;
        *byteOfState(3, col, state) ^= key[col] & 0xFF;
    }
    return 0;
}


u_int32_t rotWord(u_int32_t word) {
    u_int32_t msb = word >> 24;
    return (word << 8) | msb;
}


u_int32_t subWord(u_int32_t word) {
    u_int8_t *bytes = (u_int8_t *) &word;
    bytes[0] = sBox[bytes[0]];
    bytes[1] = sBox[bytes[1]];
    bytes[2] = sBox[bytes[2]];
    bytes[3] = sBox[bytes[3]];
    return word;
}

void keyExpansion(key key, u_int32_t roundKeys[], unsigned int keySize) {
    const unsigned int rounds = 6 + keySize;
    // First key is master one
    for (int index = 0; index<keySize; index += 1) {
        roundKeys[index] = key[index];
    }

    // Then we compute the other round keys
    for (int index = keySize; index<BLOCK_COLUMNS*(rounds+1); index += 1) {
        u_int32_t temp = roundKeys[index-1]; //Works if keySize>0
        if (index%keySize == 0) {
            temp = subWord(rotWord(temp))^rcon[index/keySize];
        } else if (keySize>6&&index%keySize==4) {
            temp = subWord(temp);   // Only happens in AES-256 (keySize = 8)
        }

        roundKeys[index] = roundKeys[index-keySize] ^ temp;
    }
}

//void printBlock(block b) {
//    for (int row = 0; row<BLOCK_ROWS; row += 1) {
//        for (int col = 0; col<BLOCK_COLUMNS; col+=1) {
//            printf("%.2x ", *byteOfState(row, col, b));
//        }
//        printf("\n");
//    }
//    printf("\n");
//}
//
//void printRoundKeys(u_int32_t roundKeys[], unsigned int size) {
//    for (int i = 0; i<size; i += 1) {
//        printf("%d : %x\n", i, roundKeys[i]);
//    }
//}

void aes_encrypt(const u_int8_t in[BLOCK_LENGTH], u_int8_t out[BLOCK_LENGTH], u_int32_t roundKeys[], unsigned int keySize) {
    unsigned int rounds = 6 + keySize;

    block state;
    loadToState(in, state);

    addRoundKey(state, &roundKeys[0]);

    for (unsigned int round = 1; round<rounds; round += 1) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, &roundKeys[round*BLOCK_COLUMNS]);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, &roundKeys[rounds*BLOCK_COLUMNS]);

    loadFromState(state, out);
}


void aes_decrypt(const u_int8_t in[BLOCK_LENGTH], u_int8_t out[BLOCK_LENGTH], u_int32_t roundKeys[], unsigned int keySize) {
    unsigned int rounds = 6 + keySize;

    block state;
    loadToState(in, state);

    addRoundKey(state, &roundKeys[rounds*BLOCK_COLUMNS]);

    for (unsigned int round = rounds - 1; round>0; round -= 1) {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, &roundKeys[round*BLOCK_COLUMNS]);
        invMixColumns(state);
    }

    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, &roundKeys[0]);

    loadFromState(state, out);
}
