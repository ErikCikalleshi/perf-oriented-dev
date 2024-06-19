#!/bin/bash

# SBATCH directives
#SBATCH --partition=lva
#SBATCH --job-name benchmark
#SBATCH --output benchmark.log
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive

perf record /home/cb76/cb761222/perf-oriented-dev/lua/interpreter/lua fib.lua 
# perf annotate --stdio luaD_precall
# perf annotate --stdio luaV_execute
perf report
