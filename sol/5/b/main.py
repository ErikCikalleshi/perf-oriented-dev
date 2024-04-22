import subprocess
import itertools
import os

# Function to parse gcc output and extract optimization flags
def parse_optimization_flags(output):
    flags = set()
    for line in output.splitlines():
        flag = line.strip().split()[1]
        flags.add(flag)
    return flags

# Function to compare optimization flags between two optimization levels
def compare_optimization_flags(optimization_level1, optimization_level2):
    flags1 = parse_optimization_flags(optimization_level1)
    flags2 = parse_optimization_flags(optimization_level2)
    return flags2 - flags1  # Flags present in level2 but not in level1

# Function to compile a program with specific optimization flags
def compile_with_flags(program, flags):
    command = ['gcc', '-o', 'compiled_program', program] + list(flags)
    subprocess.run(command, check=True)

# Function to run a compiled program
def run_program():
    subprocess.run(['./compiled_program'], check=True)

# Test cases and their configurations
test_cases = {
    'mmul': {'dir': 'mmul', 'program': 'mmul.c', 'flags': ['-DS=1000']},
    'nbody': {'dir': 'nbody', 'program': 'nbody.c', 'flags': ['-DM=400']},
    'qap': {'dir': 'qap', 'program': 'qap.c', 'flags': ['chr15c.dat']},
    'delannoy': {'dir': 'delannoy', 'program': 'delannoy.c', 'flags': ['13']},
}

# Get optimization flags for -O2 and -O3
opt_flags_o2 = subprocess.check_output(['gcc', '-Q', '--help=optimizers', '-O2'], text=True)
opt_flags_o3 = subprocess.check_output(['gcc', '-Q', '--help=optimizers', '-O3'], text=True)

# Identify flags that change from -O2 to -O3
changed_flags = compare_optimization_flags(opt_flags_o2, opt_flags_o3)

# Compile and run test cases with each changed flag individually
for flag in changed_flags:
    print(f"Testing flag: {flag}")
    for case, config in test_cases.items():
        dir_name = config['dir']
        program = config['program']
        flags = config['flags']
        try:
            os.chdir(dir_name)  # Change directory to where source files are located
            compile_with_flags(program, flags + [f'-{flag}'])
            run_program()
            os.chdir("..")  # Change back to the original directory
        except subprocess.CalledProcessError as e:
            print(f"Error compiling/running {program} with flag {flag}: {e}")
