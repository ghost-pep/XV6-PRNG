#include "types.h"
#include "user.h"

int main(void) {
    u_int8_t random_seq[32];
    // Creating a 256-bits random sequence
    printf(1, "Creating the pseudo-random sequence\n");
    random((char*)random_seq, 32);
    const char message[29] = "Hello, world. I am a message";
    char ciphertext[29];
    u_int32_t key[4] = {0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10};
    encrypt((u_int8_t*)message, (u_int8_t*)ciphertext, 29, random_seq, key);
    printf(1, "Encrypted message is : %s\n", ciphertext);
    decrypt((u_int8_t*)ciphertext, (u_int8_t*)ciphertext, 29, random_seq, key);
    printf(1, "Decrypted message is : %s\n", ciphertext);
    printf(1, "Message is %s\n", (strcmp(message, ciphertext) == 0) ? "correct" : "incorrect");
    exit();
}
