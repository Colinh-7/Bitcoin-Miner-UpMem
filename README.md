# Bitcoin Miner Implementation and Hashrate Testing

## Introduction to UPMEM and the PiM Architecture

UPMEM is a pioneer in processing-in-memory (PiM) technology, designed to optimize performance by bringing computation closer to the data. The PiM architecture leverages dedicated Processing Units (DPU) within the memory itself, significantly reducing data transfer time and power consumption. This project is specifically designed for the PiM architecture, utilizing its unique capabilities to implement a Bitcoin miner and conduct hashrate testing.

## Project Contents

- **Bitcoin**: Contains the C code for our initial implementation of the Bitcoin miner. This miner aims to find the nonce for block 100,000.
- **Hashrate**: Contains the C code for testing the hashrate of the PiM machine.

## Compilation

Both codebases can be compiled on the PiM machine using the `make` command. However, we have added several macros to customize the compilation:

- **NR_TASKLETS**: Specifies the number of tasklets per DPU (or thread).
- **HASH_PER_DPU**: Indicates the number of hashes performed by a DPU. Note that if `NR_TASKLETS=x`, then the DPU will hash `HASH_PER_DPU * x` messages.
- **VERBOSE**: Set to `0` or `1`. Activating this option will display additional information but may reduce performance.

### Example Compilation Command

To compile the code with specific parameters, use the following command:
```bash
make NR_TASKLETS=24 HASH_PER_DPU=500 VERBOSE=0
```

## Running the Program

To execute the program, simply run the following command:
```bash
./host
```
Made with Max P.
