#ifndef BTC_HEADER_H
#define BTC_HEADER_H

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "sha256.h"

#define BTC_HEADER_SIZE 80

typedef struct {
    int32_t version;
    uint8_t previous_block_hash[32];  
    uint8_t merkle_root[32];          
    uint32_t timestamp;
    uint32_t difficulty;
    uint32_t nonce;
} BTC_Header;

/**
 * @brief Generate random BTC block header
 * @return a BTC_Header pointer that must be freed with BTC_destroy()
*/
BTC_Header* BTC_init();

/**
 * @brief Print block infos
*/
void BTC_printInfo(BTC_Header *block);

/**
 * @brief Destroy a BTC block header
 * @param block : the block you want to destroy (free).
*/
void BTC_destroy(BTC_Header *block);


#endif
