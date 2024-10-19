#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btc_header.h"

/*========================================
            Local functions
========================================*/

/***********Prototypes***********/

void randomHash(uint8_t *data);
void printHash(uint8_t *data);
void hexCharToByte(const char ch, unsigned char *byte);
void hexStringToBytes(const char *hexStr, unsigned char *bytes, size_t bytesLen);

/**********Functions***********/

void randomHash(uint8_t *data) {
    for (size_t i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        data[i] = rand() % 256;
    }
}

void printHash(uint8_t *data) {
    for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        printf("%02x", data[i]);
    }
}

void hexCharToByte(const char ch, unsigned char *byte) {
    if (ch >= '0' && ch <= '9') *byte = ch - '0';
    else if (ch >= 'a' && ch <= 'f') *byte = 10 + ch - 'a';
    else if (ch >= 'A' && ch <= 'F') *byte = 10 + ch - 'A';
}

void hexStringToBytes(const char *hexStr, unsigned char *bytes, size_t bytesLen) {
    unsigned char byte;
    for (size_t i = 0; i < bytesLen * 2; i += 2) {
        byte = 0;
        hexCharToByte(hexStr[i], &byte);
        byte <<= 4;
        unsigned char tempByte;
        hexCharToByte(hexStr[i + 1], &tempByte);
        byte |= tempByte; 
        bytes[i / 2] = byte;
    }
}


/*=======================================
            BTC functions
=======================================*/

BTC_Header* BTC_init() {
    // Block allocation
    BTC_Header *block = (BTC_Header*)malloc(sizeof(BTC_Header));

    const char* previous_block_hash_hex = "000000000002d01c1fccc21636b607dfd930d31d01c3a62104612a1719011250";
    const char* merkle_root_hex = "f3e94742aca4b5ef85488dc37c06c3282295ffec960994b2c0d5ac2a25a95766";

    // Field filling
    block->version = 1;
    hexStringToBytes(previous_block_hash_hex, block->previous_block_hash, sizeof(block->previous_block_hash));
    hexStringToBytes(merkle_root_hex, block->merkle_root, sizeof(block->merkle_root));
    block->timestamp = 1293623863;
    block->difficulty = 453281356;
    block->nonce = 0; //274148111 is the valid nonce
    return block;
}

void BTC_printInfo(BTC_Header *block) {
    printf("Block infos : \n- version : %d\n", block->version);
    printf("- previous block hash : "); printHash(block->previous_block_hash);
    printf("\n- merkle root : "); printHash(block->merkle_root);
    printf("\n- timestamp : %u\n", block->timestamp);
    printf("- difficulty : %d\n", block->difficulty);
    printf("- nonce : %d\n", block->nonce);
}

void BTC_destroy(BTC_Header *block) {
    free(block);
}