import os
import subprocess
from statistics import mean, variance
import csv
import scipy.stats as stats
import math

def run_benchmark(benchmark, repetitions):
    elapsed_times = []
    user_times = []
    system_times = []
    max_mem_sizes = []

    for _ in range(repetitions):
        result = subprocess.run(["/bin/time", "-f", "%e;%U;%S;%M"] + benchmark.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output_lines = result.stderr.decode().strip().split("\n")
        
        # Extracting time and memory information from the first line
        time_info = output_lines[0].split(";")
        elapsed_time = float(time_info[0])
        user_time = float(time_info[1])
        system_time = float(time_info[2])
        max_mem_size = int(time_info[3])

        elapsed_times.append(elapsed_time)
        user_times.append(user_time)
        system_times.append(system_time)
        max_mem_sizes.append(max_mem_size)

    avg_elapsed_time = mean(elapsed_times)
    avg_user_time = mean(user_times)
    avg_system_time = mean(system_times)
    avg_max_mem_size = mean(max_mem_sizes)
    var_elapsed_time = variance(elapsed_times)
    var_user_time = variance(user_times)
    var_system_time = variance(system_times)
    var_max_mem_size = variance(max_mem_sizes)

    return {
        "benchmark": benchmark,
        "avg_elapsed_time_s": avg_elapsed_time,
        "avg_user_time_s": avg_user_time,
        "avg_system_time_s": avg_system_time,
        "avg_max_mem_size_kb": avg_max_mem_size,
        "var_elapsed_time_s2": var_elapsed_time,
        "var_user_time_s2": var_user_time,
        "var_system_time_s2": var_system_time,
        "var_max_mem_size_kb2": var_max_mem_size
    }

def main():
    # Define benchmarks and repetitions
    benchmarks = [
        './delannoy 13'
    ]

    REPS = 5
    confidence_level = 0.95
    max_confidence_interval_width = 0.01  # Adjust this to change the desired precision

    # System information
    print("System Configuration:")
    os.system("uname -a")
    os.system("free -m")

    results_list = []
    confidence_interval_width = 0  # Initialize confidence_interval_width
    for benchmark in benchmarks:
        repetitions = REPS
        results = run_benchmark(benchmark, repetitions)
        while True:
            results_list.append(results)
            print(f"{results['benchmark']}:\n"
                f"Avg Elapsed Time - {results['avg_elapsed_time_s']:.4f}s, "
                f"Avg User Time - {results['avg_user_time_s']:.4f}s, "
                f"Avg System Time - {results['avg_system_time_s']:.4f}s, "
                f"Avg Max Mem Size - {results['avg_max_mem_size_kb']:.4f}KB,\n"
                f"Var Elapsed Time - {results['var_elapsed_time_s2']:.4f}s^2, "
                f"Var User Time - {results['var_user_time_s2']:.4f}s^2, "
                f"Var System Time - {results['var_system_time_s2']:.4f}s^2, "
                f"Var Max Mem Size - {results['var_max_mem_size_kb2']:.4f}KB^2")
        
            mean = results['avg_elapsed_time_s']
            std_dev = math.sqrt(results['var_elapsed_time_s2'])
            confidence_interval = stats.norm.interval(confidence_level, loc=mean, scale=std_dev/math.sqrt(repetitions))
            confidence_interval_width = confidence_interval[1] - confidence_interval[0]

            print(f"After {repetitions} repetitions, the confidence interval width is {confidence_interval_width}")
            

            if confidence_interval_width <= max_confidence_interval_width or repetitions >= 100:
                break  # Confidence interval reached, stop repetitions
            else:
                repetitions += 1  # Increase repetitions

    # Write results to CSV
    with open("benchmark_results.csv", "w", newline="") as csvfile:
        fieldnames = ["benchmark", "avg_elapsed_time_s", "avg_user_time_s", "avg_system_time_s", "avg_max_mem_size_kb", "var_elapsed_time_s2", "var_user_time_s2", "var_system_time_s2", "var_max_mem_size_kb2"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

        writer.writeheader()
        for result in results_list:
            writer.writerow(result)

if __name__ == "__main__":
    main()
