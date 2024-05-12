#!/bin/bash

# SBATCH directives
#SBATCH --partition=lva
#SBATCH --job-name benchmark
#SBATCH --output benchmark.log
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive

gcc -o malloctest malloctest.c -lpthread

# Run malloctest with specified parameters
time ./malloctest 1 500 1000000 10 1000

gcc -o malloctest_custom malloctest_custom.c -lpthread

# Run malloctest_custom with specified parameters
time ./malloctest_custom 1 500 1000000 10 1000