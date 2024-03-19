#!/bin/bash

# SLURM directives
#SBATCH --partition=lva
#SBATCH --job-name benchmark
#SBATCH --output /home/cb76/cb761222/perf-oriented-dev/sol/2/a/benchmark.log
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive
#SBATCH --chdir=/home/cb76/cb761222/perf-oriented-dev/tools/load_generator

# Run the Python benchmark script
./exec_with_workstation_heavy.sh "python3 -u /home/cb76/cb761222/perf-oriented-dev/sol/2/a/script.py"
