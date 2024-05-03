#!/bin/bash

# SBATCH directives
#SBATCH --partition=lva
#SBATCH --job-name benchmark
#SBATCH --output benchmark.log
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive

# Define matrix size
matrix_size=2048

# Define block sizes to test (in powers of 2)
block_sizes=(16 32 64 128 256)

# Load required modules or set environment variables if needed
# gcc -o mmul_native mmul_native.c

# real_time=$(/bin/time -f "%e" ./mmul_native 2048 2>&1 >/dev/null)

# echo "$real_time"

# Compile the matrix multiplication program
gcc -o mmul mmul.c

# Function to run matrix multiplication and extract real time
optimization_flags=(-O0 -O1 -O2 -O3 -Os -Ofast)

# Loop over block sizes and run matrix multiplication
echo "opt_flag,block_size,real_time"
for block_size in "${block_sizes[@]}"
do
    for opt_flag in "${optimization_flags[@]}"
    do
        gcc -o mmul mmul.c $opt_flag
         # Run matrix multiplication and capture output
        result=$(/bin/time -f "%e %P" ./mmul $matrix_size $block_size 2>&1)
        if [[ "$result" == *"Verification: OK"* ]]; then
            # Capture real time using /bin/time separately
            real_time=$(/bin/time -f "%e" ./mmul $matrix_size $block_size 2>&1 >/dev/null)
        
            echo "$opt_flag,$block_size,$real_time"
        fi
    done
done
