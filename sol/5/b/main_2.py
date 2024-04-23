import subprocess
import os
import csv

os.chdir("/home/cb76/cb761222/perf-oriented-dev/larger_samples/npb_bt")
# List of programs and their configurations
programs = {
    # "mmul": [],
    # "nbody": ["400"],
    # "qap": ["chr15c.dat"],
    # "delannoy": ["13"],
    "npb_bt_w": [""],
    # "ssca2": ["15"]
}

# Compiler optimization flags at -O2 and -O3 levels
O2_flags = ["-O2"]
O3_flags = ["-O3"]

# Get compiler optimization flags at -O3 level
output = subprocess.check_output(["gcc", "-Q", "--help=optimizers", "-O3"]).decode()

for line in output.split('\n'):
    if line.startswith('  -'):
        parts = line.split()
        if '=' in parts[0]:
            continue
        if '[enabled]' in line:
            O3_flags.append(parts[0])

# Get compiler optimization flags at -O2 level
output = subprocess.check_output(["gcc", "-Q", "--help=optimizers", "-O2"]).decode()
for line in output.split('\n'):
    if line.startswith('  -'):
        parts = line.split()
        if '=' in parts[0]:
            continue
        if '[enabled]' in line:
            O2_flags.append(parts[0])

# Determine the set of flags that changes from -O2 to -O3
changed_flags = set(O3_flags) - set(O2_flags)

# Execute programs with each individual optimization flag toggled to -O3 level
results = {}
for prog, config in programs.items():
    timing_data = []
    build_dir = f"build_{prog}"
    os.makedirs(build_dir, exist_ok=True)
    for flag in changed_flags:
        # Construct the output filename
        output_filename = f"{prog}_{flag}"
        # Change to the build directory
        os.chdir(build_dir)
        # Run cmake
        cmake_cmd = ["cmake", "..", f"-DCMAKE_C_FLAGS_RELEASE={flag}"]
        subprocess.run(cmake_cmd)
        # Run make
        subprocess.run(["make"], shell=True)
        # Run the program
        benchmark = f"./{prog}" + " " + " ".join(config)
        result = subprocess.run(["/bin/time", "-f", "%e"] + benchmark.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        real_time = result.stderr.decode().strip().split("\n")
        timing_data.append((flag, real_time[-1]))
        print(real_time[-1])
        subprocess.run(["make clean"], shell=True)
        os.chdir("..")
    
    # Save timing data to CSV for the current program
    csv_filename = f"{prog}.csv"
    with open(csv_filename, mode='w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Flag", "Real Time"])
        for flag, real_time in timing_data:
            writer.writerow([flag, real_time])

print("Execution completed and timing data saved to CSV files.")
