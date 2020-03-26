#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "crypto.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage: %s [-e|-d]\n", argv[0]);
    return 0;
  }

  // First, obtain key and nonce via stdin
  char key[16];
  char nonce[16];
  read(0, key, 16);
  read(0, nonce, 16);

  // Then, obtain data via stdin.
  // Data is variable length, so user must first provide a length (exactly 12 chars).
  // Then, user must provide data of that length.
  char inputLenBytes[12];
  read(0, &inputLenBytes, 12);
  int inputLen = atoi(inputLenBytes);

  byte* inputData = (byte*)malloc(sizeof(byte) * inputLen);
  read(0, inputData, inputLen);

  // Verify data length
  if(inputLen % 16 != 0) {
    printf("Input data must have a length that is a multiple of 16.\n");
    free(inputData);
    return 1;
  }

  if (strncmp(argv[1], "-e", 2) == 0) {
    // Add 16 bytes to ensure room for MAC
    byte* buf = (byte*)malloc(sizeof(byte) * (inputLen + 16));

    encryptAndMac((byte*) nonce, (byte*) key, buf, inputData, inputLen);
    write(1, buf, (inputLen+16));
    fflush(stdout);
    free(buf);
  } else if (strncmp(argv[1], "-d", 2) == 0) {
    size_t msgLen = inputLen - 16; // No MAC

    byte* buf = (byte*)malloc(sizeof(byte) * msgLen);

    int res = decryptAndMac((byte*) nonce, (byte*) key, buf, inputData, msgLen);
    if(res != 0) {
      printf("Invalid MAC!");
      free(buf);
      free(inputData);
      return 1;
    }

    write(1, buf, msgLen);
    fflush(stdout);
    free(buf);
  } else {
    printf("Must select either encrypt (-e) or decrypt (-d) mode.\n");
    free(inputData);
    return 0;
  }
  
  free(inputData);

  return 0;
}
