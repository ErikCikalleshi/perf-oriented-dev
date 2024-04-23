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

cd ..
mkdir build
cd build

optimization_levels=("0" "1" "2" "3" "s" "fast")
echo "Optimization Level,Execution Time (s)" > ../npb_bt_w.csv

for opt_level in "${optimization_levels[@]}"; do
    CMAKE_FLAGS="-DCMAKE_C_FLAGS=-O$opt_level"
    
    cmake .. $CMAKE_FLAGS

    make

    execution_time=$( { time -p ./npb_bt_w; } 2>&1 | grep real | awk '{print $2}' )

    echo "$opt_level, $execution_time" >> ../npb_bt_w.csv

    # Clean up
    make clean
done
