#!/bin/bash

# SLURM directives
#SBATCH --partition=lva
#SBATCH --job-name benchmark
#SBATCH --output /home/cb76/cb761222/perf-oriented-dev/sol/2/b/benchmark.log
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive
#SBATCH --chdir=/scratch/cb761222

# Load necessary modules if needed
# module load <module_name>

# Run the Python benchmark script
/home/cb76/cb761222/perf-oriented-dev/sol/2/b/io_workload.sh "python3 -u /home/cb76/cb761222/perf-oriented-dev/sol/2/b/script.py"

