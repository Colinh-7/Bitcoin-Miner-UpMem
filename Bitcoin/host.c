#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>

#include "btc_header.h"

#define DPU_BINARY "./dpu"

int main() {
    
    struct dpu_set_t dpu_set, dpu; 
    uint32_t num_dpus;
    struct timeval start, end;
    double exec_time, hash_per_sec;

    // Get a block
    BTC_Header *block = BTC_init();

    // Alloc DPUs and load the binary
    DPU_ASSERT(dpu_alloc(DPU_ALLOCATE_ALL, NULL, &dpu_set));
    DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &num_dpus));
    DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL)); 

    printf("Number of DPU available : %u\n", num_dpus);
    BTC_printInfo(block);

    // Send the block to DPUs
    DPU_FOREACH(dpu_set, dpu) {
        DPU_ASSERT(dpu_copy_to(dpu, "DPU_BUFFER", 0, block, BTC_HEADER_SIZE)); 
        // Update nonce range
        block->nonce += HASH_PER_DPU * NR_TASKLETS;      
    }

    printf("Start mining...\n");

    // Start timer
    gettimeofday(&start, NULL);

    // Launch the DPU
    DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));

    // End timer
    gettimeofday(&end, NULL);

    DPU_FOREACH(dpu_set, dpu) {
        dpu_log_read(dpu, stdout);
    }

    // Execution time calculation
    exec_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Execution time : %lf sec.\n", exec_time);

    // Free
    dpu_free(dpu_set);
    BTC_destroy(block);

    return 0;
}
