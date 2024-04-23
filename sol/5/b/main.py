import subprocess
import os
import csv

os.chdir("/home/cb76/cb761222/perf-oriented-dev/small_samples")

programs = {
    "mmul": [],
    "nbody": ["400"],
    "qap": ["chr15c.dat"],
    "delannoy": ["13"],
    # "npb_bt": ["W"],
    # "ssca": ["15"]
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
    for flag in changed_flags:
        output_filename = f"{prog}_{flag}"

        build_dir = f"build_{prog}"
        os.makedirs(build_dir, exist_ok=True)

        os.chdir(build_dir)

        cmd = ["gcc", "-o", output_filename, f"../{prog}/{prog}.c", "-lm", "-Wno-unused-result", "-Wall", "-Wextra", "-pedantic", "-O2"] + [f"{flag}"]
        subprocess.run(cmd)

        benchmark = f"./{prog}_{flag}" + " " + " ".join(config)
        result = subprocess.run(["/bin/time", "-f", "%e;%U;%S;%M"] + benchmark.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        real_time = result.stderr.decode().strip().split(";")[0]  # Extract real time
        timing_data.append((flag, real_time))
        os.chdir("..")
    

    csv_filename = f"{prog}.csv"
    with open(csv_filename, mode='w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Flag", "Real Time"])
        for flag, real_time in timing_data:
            writer.writerow([flag, real_time])

print("Execution completed and timing data saved to CSV files.")
