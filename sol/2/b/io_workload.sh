#!/bin/bash
echo "Current directory: $(pwd)"
# Kill any existing instances of load_generator
killall load_generator &> /dev/null

# Start multiple instances of load_generator in the background
load_generator 1000000 1000000 &> /dev/null &
load_generator 1000000 1000000 &> /dev/null &
load_generator 1000000 1000000 &> /dev/null &
load_generator 1000000 1000000 &> /dev/null &
load_generator 1000000 1000000 &> /dev/null &
load_generator 1000000 1000000 &> /dev/null &
load_generator 1000000 1000000 &> /dev/null &


#time -p nice -n 100 $1
nice -n 1000 $1
killall load_generator &> /dev/null