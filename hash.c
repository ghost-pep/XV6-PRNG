#include "hash.h"
#include "defs.h"

//hash value
static unsigned int h[8];
//message schedule array
static unsigned int w[64];
//sha256 round constant table
unsigned static int k[64] = {
0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/**
 * A simple function to rotate an unsigned integer right by n.
 * @param a - integer to rotate
 * @param n - number of spaces to rotate
 * @return - the rotated value of a
 */
static unsigned int rotright(unsigned int a, int n) {
    unsigned int y = a >> n;
    unsigned int z = a << (sizeof(unsigned int) - n);
    return y | z;
}


/**
 * Initialize the hash values and do pre-processing in order to set up for the
 * internal sha256 function.
 *
 * @param data - the user data that is stored with integers
 * @param size - size of data array
 * @param buffer - the buffer that will contain the 512 bit aligned data with
 * padding
 * @return - the number of integers used for the buffer
 */
static int init(unsigned int *data, int size, unsigned int *buffer) {
    h[0] = 0x6a09e667;
    h[1] = 0xbb67ae85;
    h[2] = 0x3c6ef372;
    h[3] = 0xa54ff53a;
    h[4] = 0x510e527f;
    h[5] = 0x9b05688c;
    h[6] = 0x1f83d9ab;
    h[7] = 0x5be0cd19;

    //determine buffer size
    int l = size * sizeof(unsigned int);
    int bufsize = l + 1 + sizeof(unsigned int);
    int subbufsize = bufsize % 512;
    bufsize = bufsize - subbufsize + 512;

    //check for correct size
    if (size < 0 || bufsize < size) {
        panic("invalid sha256 message size\n");
    }

    //clear the buffer
    memset(buffer, bufsize / sizeof(char), 0);

    //populate the buffer
    int i;
    for (i = 0; i < size; i++) {
        buffer[i] = data[i];
    }

    //populate the buffer's padding
    /* int numzeros = bufsize - 1 - sizeof(unsigned int) - l - 31; */
    /* i = i + 1; */
    /* buffer[i] = 0x80000000; // 1 followed by 0s */
    /* memset((void *)(buffer + i + 1), (bufsize - size - 32) / 32, numzeros / 32); */
    /* int lasttwo = buffer + */ 

    return bufsize / sizeof(unsigned int);
}

/**
 * The internal core operation of the hash function, which includes the
 * compression function, and all of the linear and non-linear operations that
 * provide cryptographic security.
 *
 * @param buffer - the data to hash in an array of integers
 * @param bufsize - the size of the buffer's array not the number of bits of
 * data
 */
static void sha256internal(unsigned int *buffer, int bufsize) {
    //operate on each 512 bit chunk
    //now chunks will be accessed like buffer[chunkid + (0 thru 15)]
    for (int chunkind = 0; chunkind < bufsize; chunkind += 16) {

        //initialize message schedule array
        for (int i = 0; i < 16; i++) {
            w[i] = buffer[chunkind + i];
        }
        for (int i = 16; i < 64; i++) {
            unsigned int s0 = rotright(w[i-15], 7) ^ rotright(w[i-15], 18) ^ (w[i-15] >> 3);
            unsigned int s1 = rotright(w[i-2], 17) ^ rotright(w[i-2], 19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }

        //initialize working variables to current hash value
        unsigned int a = h[0];
        unsigned int b = h[1];
        unsigned int c = h[2];
        unsigned int d = h[3];
        unsigned int e = h[4];
        unsigned int f = h[5];
        unsigned int g = h[6];
        unsigned int h = h[7];

        //compression function
        for (int i = 0; i < 64; i++) {
            unsigned int S0, S1, ch, temp1, temp2, maj;
            S1 = rotright(e, 6) ^ rotright(e, 11) ^ rotright(e, 25);
            ch = (e & f) ^ ((~e) & g);
            temp1 = h + S1 + ch + k[i] + w[i];
            S0 = rotright(a, 2) ^ rotright(a, 13) ^ rotright(a, 22);
            maj = (a & b) ^ (a & c) ^ (b & c);
            temp2 = S0 + maj;
            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }
        //update the current hash value for the next iteration
        h[0] = h[0] + a;
        h[1] = h[1] + b;
        h[2] = h[2] + c;
        h[3] = h[3] + d;
        h[4] = h[4] + e;
        h[5] = h[5] + f;
        h[6] = h[6] + g;
        h[7] = h[7] + h;

    }
}

/**
 * Sets the user's hash value to the computed value.
 */
static void sha256final(unsigned int *hash) {
    for (int i = 0; i < 8; i++) {
        hash[i] = h[i];
    }
}

/**
 * Runs sha256 on the data.
 * @param data - array of data to hash that must be less than a page
 * @param size - size of data
 * @param - hashed value of size 256 bits
 */
void sha256(unsigned int* data, int size, unsigned int *hash) {
    unsigned int *buffer = (unsigned int *) kalloc();
    int bufsize = init(data, size, buffer);
    sha256internal(buffer, bufsize);
    sha256final(hash);
}
