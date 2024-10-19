#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <time.h>
#include <sys/time.h>

#include "sha256.h"
#include "common.h"

#define DPU_BINARY "./dpu"

int main(int argc, char *argv[]) {
    struct dpu_set_t dpu_set,                   // dpu_set will contain all allocated DPU
                     d;                         // for the DPU_FOREACH
    uint32_t dpu_nb;                            // Numbers of DPU
    uint8_t buffer[BUFF_SIZE];                  // buffer to write in the MRAM for the DPUs
    uint64_t message = 0;                       // Message for the DPU
    uint32_t index;
    struct timeval start, end;
    double exec_time, hash_per_sec;

    // Set the buffer to 0
    memset(buffer, 0, BUFF_SIZE);

    // DPU_ASSERT return exit statut when an error has occured. It's better to use it but not mandatory
    DPU_ASSERT(dpu_alloc(DPU_ALLOCATE_ALL, NULL, &dpu_set));    // dpu_alloc allocate DPUs
    DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));            // dpu_load load the binary inside all allocated DPUs

    DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &dpu_nb));
    printf("Number of DPU available : %u\n", dpu_nb);

    // We send a value to each DPU
    DPU_FOREACH(dpu_set, d) {
        DPU_ASSERT(dpu_copy_to(d, "DPU_BUFFER", 0, &buffer, BUFF_SIZE));
        message += NR_TASKLETS * HASH_PER_DPU;
        memcpy(buffer, &message, sizeof(uint64_t));
    }

    // Start timer
    gettimeofday(&start, NULL);

    // Launch the DPU
    DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));

    // End timer
    gettimeofday(&end, NULL);

    if (VERBOSE) {
        DPU_FOREACH(dpu_set, d) {
            DPU_ASSERT(dpu_log_read(d, stdout));
        }
    }

    // Calculation hash per second
    exec_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;
    
    hash_per_sec = (double)message / exec_time / 1000000;
    printf("Number of hash calculations : %ld\n", message);
    printf("Exec : %lf sec.\n", exec_time);
    printf("Hash rate : %.2lf MH/s\n", hash_per_sec);

    // All allocated DPUs must be freed
    dpu_free(dpu_set);

    return 0;
}
