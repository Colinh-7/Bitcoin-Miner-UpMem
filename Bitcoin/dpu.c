#include <defs.h>
#include <mram.h>
#include <stdio.h>
#include <string.h>
#include "sha256.h"

/*================================
            BTC_Header
================================*/

#define BTC_HEADER_SIZE 80

typedef struct {
    int32_t version;
    uint8_t previous_block_hash[32];  
    uint8_t merkle_root[32];          
    uint32_t timestamp;
    uint32_t difficulty;
    uint32_t nonce;
} BTC_Header;

/*================================
        BTC mining functions
================================*/

void init_target(uint32_t bits, BYTE *target, size_t targetLen) {
    memset(target, 0, targetLen);
    uint32_t coefficient = bits & 0xFFFFFF;
    int exponent = (bits >> 24) & 0xFF;
    int shift = exponent - 3;
    if (shift < targetLen) {
        for (int i = 0; i < 3; i++) {
            target[targetLen - 1 - shift - i] = (coefficient >> (8 * i)) & 0xFF;
        }
    }
}

int check_block_validity(const BYTE *hash, const BYTE *target) {
    for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
        if (hash[i] < target[i]) {
            return -1; 
        } else if (hash[i] > target[i]) {
            return 1;
        }
    }
    return 0; 
}

void big_endian(uint8_t *start, size_t size) {
    size_t i, j;
    for (i = 0, j = size - 1; i < j; i++, j--) {
        uint8_t temp = start[i];
        start[i] = start[j];
        start[j] = temp;
    }
}

void BTC_serialize(const BTC_Header *header, BYTE *output) {
    size_t offset = 0;
    
    memcpy(output + offset, &header->version, sizeof(header->version));
    offset += sizeof(header->version);

    uint8_t previous_block_hash[32];
    memcpy(previous_block_hash, header->previous_block_hash, sizeof(header->previous_block_hash));
    big_endian(previous_block_hash, sizeof(previous_block_hash));
    memcpy(output + offset, previous_block_hash, sizeof(previous_block_hash));
    offset += sizeof(previous_block_hash);

    // Reverse merkle root
    uint8_t merkle_root[32];
    memcpy(merkle_root, header->merkle_root, sizeof(header->merkle_root));
    big_endian(merkle_root, sizeof(merkle_root));
    memcpy(output + offset, merkle_root, sizeof(merkle_root));
    offset += sizeof(merkle_root);

    memcpy(output + offset, &header->timestamp, sizeof(header->timestamp));
    offset += sizeof(header->timestamp);

    memcpy(output + offset, &header->difficulty, sizeof(header->difficulty));
    offset += sizeof(header->difficulty);

    memcpy(output + offset, &header->nonce, sizeof(header->nonce));
    offset += sizeof(header->nonce);
}

void little_endian_to_big(unsigned char* bytes, int length) {
    for (int i = 0; i < length / 2; i++) {
        unsigned char temp = bytes[i];
        bytes[i] = bytes[length - i - 1];
        bytes[length - i - 1] = temp;
    }
}

/*================================
            SHA2
================================*/

void double_sha256(unsigned char *data, BYTE *buff) {
    SHA256_CTX ctx, ctx2;
    uint8_t temp[32];

    sha256_init(&ctx);
    sha256_update(&ctx, data, BTC_HEADER_SIZE);
    sha256_final(&ctx, temp);

    sha256_init(&ctx2);
    sha256_update(&ctx2, temp, SHA256_BLOCK_SIZE);
    sha256_final(&ctx2, buff);
}

/*================================
            MRAM
================================*/

__mram_noinit uint8_t DPU_BUFFER[BTC_HEADER_SIZE];

/*================================
            MAIN
================================*/

int main() {
   
    __dma_aligned uint8_t buffer[BTC_HEADER_SIZE];
    BYTE digest[SHA256_BLOCK_SIZE];
    BYTE target[SHA256_BLOCK_SIZE];
    BYTE serializedBlock[BTC_HEADER_SIZE];
    int block_validity;

    // Read the value sended from host in the MRAM
    mram_read(DPU_BUFFER, buffer, BTC_HEADER_SIZE);

    // Convert data into BTC_Header block
    BTC_Header *block = (BTC_Header*)buffer;

    // Update starting nonce
    block->nonce += me() * HASH_PER_DPU;

    // Init target
    init_target(block->difficulty, target, sizeof(target));

    // Mining
    for (uint32_t i=0; i<HASH_PER_DPU; ++i) {
        // Serialize the btc header
        BTC_serialize(block, serializedBlock);

        // Hash the block
        double_sha256(serializedBlock, digest);

        // Conversion
        little_endian_to_big(digest, SHA256_BLOCK_SIZE);

        // Check validity
        block_validity = check_block_validity(digest, target);

        if (block_validity <= 0) {
            printf("Block valid : nonce(%d)\n", block->nonce);
            break;
        }
        else block->nonce++;
    }
    return 0;
}


