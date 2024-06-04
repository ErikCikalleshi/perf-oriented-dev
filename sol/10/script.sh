#!/bin/bash

# SBATCH directives
#SBATCH --partition=lva
#SBATCH --job-name benchmark
#SBATCH --output benchmark.log
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive

# Delete the existing CSV file
rm -f results_local.csv
rm -f error_log.txt

# Define the parameters for the benchmark
data_structures=("tiered_list" "unrolled_linked_list" )
ins_del_ratios=(0 1 10 50)
read_write_ratios=(100 99 90 50)
element_sizes=(8 512 8192)
num_elements=(10 1000 100000 10000000)
num_chunks=(1 10 100) 

gcc -O3 -o tiered_list tiered_array.c -Wall -Wextra -pedantic
gcc -O3 -o unrolled_linked_list unrolled_linked_list.c -Wall -Wextra -pedantic

# Initialize a counter for the test sequence
test_sequence=1

# Write the header for the CSV file
echo "idx,data structure,Ins/Del Ratio,Read/Write Ratio,Element Size,Number of Elements, Number of Chunks, Operations Completed" >> results_local.csv

# Loop over each combination of parameters
for ds in ${data_structures[@]}; do
    for idr in ${ins_del_ratios[@]}; do
        for rwr in ${read_write_ratios[@]}; do
            # Check if the sum of ins_del_ratios and read_write_ratios is 100
            if (( idr + rwr == 100 )); then
                for es in ${element_sizes[@]}; do
                    for ne in ${num_elements[@]}; do
                        for nc in ${num_chunks[@]}; do
                            # Print the arguments
                            echo "Arguments: $ds $nc $ne $es $idr $rwr"
                            
                            ./$ds $nc $ne $es $idr $rwr >> temp_output.txt 2>&1  # Redirect output to a temporary file
                            if [ $? -eq 0 ]; then  # Check if command execution was successful
                                echo -n "$test_sequence,$ds,$idr,$rwr,$es,$ne,$nc," >> results_local.csv
                                cat temp_output.txt >> results_local.csv  # Append output to CSV file
                                ((test_sequence++))
                            else
                                # Save the error message to another .txt file
                                echo "Error: $ds $nc $ne $es $idr $rwr" >> error_log.txt
                                cat temp_output.txt >> error_log.txt
                            fi 
                            rm temp_output.txt  # Remove temporary file
                        done
                    done
                done
            fi
        done
    done
done
echo "" >> results_local.csv  # Add a newline after each set of operations