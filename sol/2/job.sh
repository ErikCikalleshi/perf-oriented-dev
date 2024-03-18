#!/bin/bash

# SLURM directives
#SBATCH --partition=lva
#SBATCH --job-name benchmark
#SBATCH --output benchmark.log
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive

# Load necessary modules if needed
# module load <module_name>

# Run the Python benchmark script
#./exec_with_workstation_heavy.sh
/usr/bin/python3 script.py


# NOTE: SBATCH --chdir="DIRNAME"