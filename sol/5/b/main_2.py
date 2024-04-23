import subprocess
import os
import csv

os.chdir("/home/cb76/cb761222/perf-oriented-dev/larger_samples/npb_bt")

programs = {
    "mmul": [],
    "nbody": ["400"],
    "qap": ["chr15c.dat"],
    "delannoy": ["13"],
    "npb_bt_w": [""],
    "ssca2": ["15"]
}

O2_flags = ["-O2"]
O3_flags = ["-O3"]

output = subprocess.check_output(["gcc", "-Q", "--help=optimizers", "-O3"]).decode()

for line in output.split('\n'):
    if line.startswith('  -'):
        parts = line.split()
        if '=' in parts[0]:
            continue
        if '[enabled]' in line:
            O3_flags.append(parts[0])

output = subprocess.check_output(["gcc", "-Q", "--help=optimizers", "-O2"]).decode()
for line in output.split('\n'):
    if line.startswith('  -'):
        parts = line.split()
        if '=' in parts[0]:
            continue
        if '[enabled]' in line:
            O2_flags.append(parts[0])

changed_flags = set(O3_flags) - set(O2_flags)

results = {}
for prog, config in programs.items():
    timing_data = []
    build_dir = f"build_{prog}"
    os.makedirs(build_dir, exist_ok=True)
    for flag in changed_flags:

        output_filename = f"{prog}_{flag}"

        os.chdir(build_dir)

        cmake_cmd = ["cmake", "..", f"-DCMAKE_C_FLAGS_RELEASE={flag}"]
        subprocess.run(cmake_cmd)

        subprocess.run(["make"], shell=True)

        benchmark = f"./{prog}" + " " + " ".join(config)
        result = subprocess.run(["/bin/time", "-f", "%e"] + benchmark.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        real_time = result.stderr.decode().strip().split("\n")
        timing_data.append((flag, real_time[-1]))
        print(real_time[-1])
        subprocess.run(["make clean"], shell=True)
        os.chdir("..")
    
    csv_filename = f"{prog}.csv"
    with open(csv_filename, mode='w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Flag", "Real Time"])
        for flag, real_time in timing_data:
            writer.writerow([flag, real_time])

print("Execution completed and timing data saved to CSV files.")
