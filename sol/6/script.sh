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

# Compile the matrix multiplication program
gcc -o matrix_multiply matrix_multiply.c -lm

# Function to extract real time from /bin/time output
get_real_time() {
    grep "real" $1 | awk '{print $2}'
}

# Loop over block sizes and run matrix multiplication
echo "block_size, real_time"
for block_size in "${block_sizes[@]}"
do
    
    # Run matrix multiplication with /bin/time and capture output
    /bin/time -f "%e" -o time_output.csv ./matrix_multiply $matrix_size $block_size
    
    # Extract real time from time_output.txt
    real_time=$(get_real_time "time_output.txt")
    echo "$block_size, $real_time"
done

# Clean up temporary files
rm time_output.txt
