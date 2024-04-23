#!/bin/bash

# SLURM directives
#SBATCH --partition=lva
#SBATCH --job-name benchmark
#SBATCH --output benchmark.log
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive

# Set GCC version
# module load gcc/12.2.0-gcc-8.5.0-p4pe45v
echo "python"

python3 main.py