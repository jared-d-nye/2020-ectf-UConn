// Encryption plans
#define BLOCK_LEN 16
#define ELEMENT_BYTE_LEN 8
#define ROUNDS 32

typedef unsigned char byte;

// Encrypts ptext and places the ciphertext in ctext.
// Expects:
// 1. ptext, ctext must be of size >= msgLen 
// 2. iv and key must both be of size BLOCK_LEN
// 3. msgLen must be a multiple of BLOCK_LEN.
//      => If your msg size is not a multiple of BLOCK_LEN, pad it with n = (ctextLen % BLOCK_LEN) zeroes.  Then, chop off n bytes from the returned ptext.
void encryptD(const byte* iv, const byte* key, byte* ctext, const byte* ptext, unsigned int msgLen);

// Decrypts ctext and places the plaintext in ptext.
// Expects:
// 1. ptext, ctext must be of size >= msgLen 
// 2. iv and key must both be of size BLOCK_LEN
// 3. msgLen must be a multiple of BLOCK_LEN.
//      => If your msg size is not a multiple of BLOCK_LEN, pad it with n = (ctextLen % BLOCK_LEN) zeroes.  Then, chop off n bytes from the returned ptext.
void decryptD(const byte* iv, const byte* key, byte* ptext, const byte* ctext, unsigned int msgLen);

// MACs ptext and places the MAC in mac.
// Expects:
// 1. ptext must be of size >= msgLen
// 2. msgLen must be a multiple of BLOCK_LEN
//      => If your msg size is not a multiple of BLOCK_LEN, pad it with n = (ctextLen % BLOCK_LEN) zeroes.  Then, chop off n bytes from the returned ptext.
// 3. key, mac must be of size BLOCK_LEN
void cbcMac(const byte* key, byte* mac, const byte* ptext, unsigned int msgLen);

// Wrapper for encrypt and cbcMac.
// 1. ptext must be of size msgLen 
// 2. ctext must be of size msgLen + BLOCK_LEN
// 3. iv and key must both be of size BLOCK_LEN
// 4. msgLen must be a multiple of BLOCK_LEN.
//      => If your msg size is not a multiple of BLOCK_LEN, pad it with n = (ctextLen % BLOCK_LEN) zeroes.  Then, chop off n bytes from the returned ptext.
// Encrypts and MACs the message.  The first 2*ELEMENT_BYTE_LEN bytes of ctext will be set to the MAC; the remainder will be set to the ciphertext data. 
// Uses the "encrypt-then-MAC" strategy.
void encryptAndMac(const byte* iv, const byte* key, byte* ctext, const byte* ptext, unsigned int msgLen);

// Wrapper for cbcMac and decrypt.
// 1. ptext must be of size msgLen 
// 2. ctext must be of size msgLen + BLOCK_LEN
// 3. iv and key must both be of size BLOCK_LEN
// 4. msgLen must be a multiple of BLOCK_LEN.
//      => If your msg size is not a multiple of BLOCK_LEN, pad it with n = (ctextLen % BLOCK_LEN) zeroes.  Then, chop off n bytes from the returned ptext.
// 5. The first 2*ELEMENT_BYTE_LEN bytes of ctext must contain the MAC; the rest must contain the ciphertext data. [This requirement is met if encryptAndMac is create ctext.]
// First verifies the MAC.  If verification fails, returns -1.  Otherwise, decrypts the data, places the plaintext in ptext, and returns 0. 
// Assumes the "encrypt-then-MAC" strategy.
int decryptAndMac(const byte* iv, const byte* key, byte* ptext, const byte* ctext, unsigned int msgLen);

// Increments iv by incrementBy, places result in ivNew
// Expects:
// 1. ivNew, iv must be of size EXACTLY ivLen
// Returns 0 on success, != 0 on fail.
void incrementIv(byte* ivNew, const byte* iv, unsigned int ivLen, unsigned int incrementBy);
void incrementIvInPlace(byte* iv, unsigned int ivLen, unsigned int incrementBy);
