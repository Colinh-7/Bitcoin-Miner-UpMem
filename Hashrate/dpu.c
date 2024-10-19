#include <defs.h>
#include <mram.h>
#include <perfcounter.h>
#include <stdio.h>
#include <string.h>

#include "sha256.h"
#include "common.h"

size_t bufferlen(unsigned char *buffer) {
    size_t len;
    for (len=BUFF_SIZE; len>0; --len) {
        if (buffer[len-1]) break;
    }
    return len;
}

/**
 * @brief Hash using SHA2 algorithm
 * @param message Message to hash
 * @param buffer Final hash buffer
*/
void sha256(unsigned char *message, BYTE *buff) {
    SHA256_CTX ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, message, bufferlen(message));
    sha256_final(&ctx, buff);
}

__mram_noinit uint8_t DPU_BUFFER[BUFF_SIZE];                // MRAM buffer from host to dpu

int main() {
    __dma_aligned uint8_t message[BUFF_SIZE];           // Will be read from DPU_BUFFER
    uint8_t tasklet_id = me();                          // Tasklet id (Thread id)
    BYTE buff[SHA256_BLOCK_SIZE];

    // Read the value sended from host in the MRAM
    mram_read(DPU_BUFFER, message, BUFF_SIZE);

    // Add tasklet number to value
    uint64_t nb;
    memcpy(&nb, message, sizeof(uint64_t));
    nb += tasklet_id * HASH_PER_DPU;


    for (int i=0; i<HASH_PER_DPU; ++i) {
        nb += i;
        memcpy(message, &nb, sizeof(uint64_t));
        sha256(message, buff);
    }

    if (VERBOSE)
        printf("Tasklet %u hashed %d messages.\n", tasklet_id, HASH_PER_DPU);

    return 0;
}
