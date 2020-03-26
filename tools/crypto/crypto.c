#include <stdint.h>
#include <string.h>
#include "./crypto.h"

#define ROR(x, r) ((x >> r) | (x << (64 - r)))
#define ROL(x, r) ((x << r) | (x >> (64 - r)))
#define R(x, y, k) (x = ROR(x, 8), x += y, x ^= k, y = ROL(y, 3), y ^= x)

void encryptRaw(uint64_t ct[2],
             uint64_t const pt[2],            
             uint64_t const K[2])
{
   uint64_t y = pt[0], x = pt[1], b = K[0], a = K[1];

   R(x, y, b);
   for (int i = 0; i < ROUNDS - 1; i++) {
      R(a, b, i);
      R(x, y, b);
   }

   ct[0] = y;
   ct[1] = x;
}

void incrementIvInPlace(byte* iv, unsigned int ivLen, unsigned int incrementBy) {
        int failCode = 0;
        while(incrementBy > 0) {
                for(int byteToIncrement = 0; byteToIncrement < ivLen; byteToIncrement++) {
                        if(iv[byteToIncrement] == 255) {
                                iv[byteToIncrement] = 0;
                        } else {
                                iv[byteToIncrement] += 1;
                                break;
                        }

                        if(byteToIncrement == ivLen - 1) {
                            byteToIncrement = -1; // Wiil be incr to 0 on next run
                        }
                }

                incrementBy -= 1;
        }
}

void encryptD(const byte* iv, const byte* key, byte* ctext, const byte* ptext, unsigned int msgLen) {
        int numberOfWords = msgLen/ELEMENT_BYTE_LEN;
        int numberOfIterations = numberOfWords / 2;

        uint64_t ctTmp[2];
        uint64_t ptIv[2];
        uint64_t k[2];

        memcpy(ptIv, iv, 2*ELEMENT_BYTE_LEN);
        memcpy(k, key, 2*ELEMENT_BYTE_LEN);

        for(int i = 0; i < numberOfIterations; i++) {
                encryptRaw(ctTmp, ptIv, k);
                
                ctTmp[0] ^= *((uint64_t*) (ptext + 2*i*ELEMENT_BYTE_LEN));
                ctTmp[1] ^= *((uint64_t*) (ptext + 2*i*ELEMENT_BYTE_LEN + ELEMENT_BYTE_LEN));
                memcpy(ctext + 2*i*ELEMENT_BYTE_LEN, ctTmp, 2*ELEMENT_BYTE_LEN);

                incrementIvInPlace((byte*) ptIv, 2*ELEMENT_BYTE_LEN, 1);
        }
}

void decryptD(const byte* iv, const byte* key, byte* ptext, const byte* ctext, unsigned int msgLen) {
        int numberOfWords = msgLen/ELEMENT_BYTE_LEN;
        int numberOfIterations = numberOfWords / 2;

        uint64_t ptIv[2];
        uint64_t ctIv[2];
        uint64_t k[2];

        memcpy(ptext, ctext, msgLen);
        memcpy(ptIv, iv, 2*ELEMENT_BYTE_LEN);
        memcpy(k, key, 2*ELEMENT_BYTE_LEN);

        for(int i = 0; i < numberOfIterations; i++) {
                encryptRaw(ctIv, ptIv, k);

                ctIv[0] ^= *((uint64_t*) (ctext + 2*i*ELEMENT_BYTE_LEN));
                ctIv[1] ^= *((uint64_t*) (ctext + 2*i*ELEMENT_BYTE_LEN + ELEMENT_BYTE_LEN));
                memcpy(ptext + 2*i*ELEMENT_BYTE_LEN, ctIv, 2*ELEMENT_BYTE_LEN);

                incrementIvInPlace((byte*) ptIv, 2*ELEMENT_BYTE_LEN, 1);
        }
}

void cbcMac(const byte* key, byte* mac, const byte* ptext, unsigned int msgLen) {
        int numberOfWords = msgLen/ELEMENT_BYTE_LEN;
        int numberOfIterations = numberOfWords / 2;

        uint64_t tmp[2];
        uint64_t k[2];

        memset(tmp, 0, 2*ELEMENT_BYTE_LEN);
        memcpy(k, key, 2*ELEMENT_BYTE_LEN);
        
        for(int i = 0; i < numberOfIterations; i++) {
            tmp[0] ^= *((uint64_t*) (ptext + 2*i*ELEMENT_BYTE_LEN));
            tmp[1] ^= *((uint64_t*) (ptext + 2*i*ELEMENT_BYTE_LEN + ELEMENT_BYTE_LEN));

            encryptRaw((uint64_t*) mac, tmp, k);
            memcpy(tmp, mac, 2*ELEMENT_BYTE_LEN);
        }
}

void encryptAndMac(const byte* iv, const byte* key, byte* ctext, const byte* ptext, unsigned int msgLen) {
  encryptD(iv, key, ctext + 2*ELEMENT_BYTE_LEN, ptext, msgLen);
  cbcMac(key, ctext, ctext + 2*ELEMENT_BYTE_LEN, msgLen);
}

int decryptAndMac(const byte* iv, const byte* key, byte* ptext, const byte* ctext, unsigned int msgLen) {
    byte canonicalMac[2*ELEMENT_BYTE_LEN];
    cbcMac(key, canonicalMac, ctext + 2*ELEMENT_BYTE_LEN, msgLen);

    if(memcmp(canonicalMac, ctext, 2*ELEMENT_BYTE_LEN) != 0) {
        return -1;
    }

    decryptD(iv, key, ptext, ctext + 2*ELEMENT_BYTE_LEN, msgLen);

    return 0;
}

void incrementIv(byte* ivNew, const byte* iv, unsigned int ivLen, unsigned int incrementBy) {
        memcpy(ivNew, iv, ivLen);

        while(incrementBy > 0) {
                for(int byteToIncrement = 0; byteToIncrement < ivLen; byteToIncrement++) {
                        if(ivNew[byteToIncrement] == 255) {
                                ivNew[byteToIncrement] = 0;
                        } else {
                                ivNew[byteToIncrement] += 1;
                                break;
                        }

                        if(byteToIncrement == ivLen - 1) {
                          byteToIncrement = -1; // Wiil be incr to 0 on next run
                        }
                }

                incrementBy -= 1;
        }
}
