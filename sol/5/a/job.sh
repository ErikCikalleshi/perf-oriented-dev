#!/bin/bash

# SLURM directives
#SBATCH --partition=lva
#SBATCH --job-name benchmark
#SBATCH --output benchmark.log
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive

# Set GCC version
module load gcc/12.2.0-gcc-8.5.0-p4pe45v

# Run CMake to generate build files
mkdir build
cd build

# Optimization levels to iterate over
optimization_levels=("0" "1" "2" "3" "s" "fast")

# Program names
program_names=("mmul" "nbody" "qap" "delannoy")

# Program parameters
program_params=("" "400 chr15c.dat" "chr15c.dat" "13")

# Iterate over optimization levels
for opt_level in "${optimization_levels[@]}"; do
    # Set additional flags for CMake
    CMAKE_FLAGS="-DCMAKE_C_FLAGS=-O$opt_level"
    
    # Generate build files with CMake
    cmake .. $CMAKE_FLAGS
    # Iterate over programs
    for i in "${!program_names[@]}"; do
        program_name="${program_names[$i]}"
        program_param="${program_params[$i]}"
        
        # Compile the program
        make $program_name
        
        # Capture execution time
        execution_time=$( { time -p ./$program_name $program_param; } 2>&1 | grep real | awk '{print $2}' )

        # Write result to CSV
        echo "$opt_level, $execution_time" >> ../${program_name}_results.csv
        
        # Clean up
        make clean
    done
done
