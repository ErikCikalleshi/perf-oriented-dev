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

mkdir build
cd build

optimization_levels=("0" "1" "2" "3" "s" "fast")

program_names=("mmul" "nbody" "qap" "delannoy")

program_params=("" "400 chr15c.dat" "chr15c.dat" "13")

for opt_level in "${optimization_levels[@]}"; do

    CMAKE_FLAGS="-DCMAKE_C_FLAGS=-O$opt_level"
    
    cmake .. $CMAKE_FLAGS

    for i in "${!program_names[@]}"; do
        program_name="${program_names[$i]}"
        program_param="${program_params[$i]}"
        
        make $program_name
        
        execution_time=$( { time -p ./$program_name $program_param; } 2>&1 | grep real | awk '{print $2}' )

        echo "$opt_level, $execution_time" >> ../${program_name}_results.csv
        
        make clean
    done
done
