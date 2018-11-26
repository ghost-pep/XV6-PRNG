struct stat;
struct rtcdate;
typedef uint fd_set;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(char*, int);
int mknod(char*, short, short);
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);

/**
 * @brief Generates a pseudrandom string of bytes. The users will use this
 * function to retrieve random data.
 *
 * @param str the pointer to the pseudorandom string of bytes
 * @param bytes the number of bytes of random data to generate
 * @return int 0 on success and -1 on error
 */
int random(char *str, int nbytes);

// Select system call waits if all fds block
// @param {fd_set *} readfds - Set of fds to read
// @param {fd_set *} writefds - Set of fds to write
// @return {int} -1 on error
int select(int, fd_set *, fd_set *);

/**
 * @brief : Encrypts a message using CTR and AES-128.
 * Calls ctr_encrypt in ctr.h
 */
int encrypt(const u_int8_t message[], u_int8_t ciphertext[], size_t size, const u_int8_t sequence[], u_int32_t key[4]);

/**
 * @brief : Decrypts a message using CTR and AES-128.
 * Calls ctr_decrypt in ctr.h
 */
int decrypt(const u_int8_t ciphertext[], u_int8_t message[], size_t size, const u_int8_t sequence[], u_int32_t key[4]);


// ulib.c
int stat(char*, struct stat*);
char* strcpy(char*, char*);
void *memmove(void*, void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, char*, ...);
char* gets(char*, int max);
uint strlen(char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
