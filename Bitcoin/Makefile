CC = gcc
DPU_CC = dpu-upmem-dpurte-clang
CFLAGS = --std=c99
DPU_LIBS = $(shell dpu-pkg-config --cflags --libs dpu)
SRC_DPU = dpu.c sha256.c
SRC_HOST = host.c btc_header.c
COMMON_HEADER = sha256.h

ifndef VERBOSE
VERBOSE := 0
endif

NR_TASKLETS := 1

ifndef HASH_PER_DPU
HASH_PER_DPU := 500
endif

VERBOSE_FLAG = -DVERBOSE=$(VERBOSE)
NR_TASKLETS_FLAG = -DNR_TASKLETS=$(NR_TASKLETS)
HASH_PER_DPU_FLAG = -DHASH_PER_DPU=$(HASH_PER_DPU)

all: dpu host

dpu: $(SRC_DPU) $(COMMON_HEADER)
	$(DPU_CC) $(CFLAGS) $(NR_TASKLETS_FLAG) $(HASH_PER_DPU_FLAG) $(VERBOSE_FLAG) -o dpu $(SRC_DPU)

host: $(SRC_HOST) $(COMMON_HEADER)
	$(CC) $(CFLAGS) $(NR_TASKLETS_FLAG) $(HASH_PER_DPU_FLAG) $(VERBOSE_FLAG) $(SRC_HOST) -o host $(DPU_LIBS)

clean:
	rm -f dpu host

