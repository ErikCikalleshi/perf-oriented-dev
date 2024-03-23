First of all we need to compile the binaries with the `-pg` flag to generate the profiling information.
I added in the `CMakeLists.txt` the following line to compile the binaries with the `-pg` flag and chose the `Debug` build type.

```cmake
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
```

./npb_bt* to execute the binaries so we can execute gprof on them.

```bash
gprof npb_bt_[X] gmon.out > [X].out
```

Where [X] is either `_s`, `_w`, `_a`, `_b` or `_c` depending on the binary to profile.	

### Metrics
1. % time spent in each function
2. cumulative time: means the time spent in the function and all functions called by it
3. self seconds: time spent in the function itself
4. calls: number of times the function was called
5. self ms/call: self seconds divided by the number of calls
6. total ms/call: cumulative time divided by the number of calls
7. name: name of the function

### _a local execution
```
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 18.29     12.19    12.19      203    60.05    60.05  compute_rhs
 17.98     24.17    11.98      201    59.60    90.68  y_solve
 17.26     35.67    11.50  2317932     0.00     0.00  binvcrhs
 16.24     46.49    10.82      201    53.83    84.91  x_solve
 15.22     56.63    10.14                             z_solve
  8.19     62.09     5.46 146029716     0.00     0.00  matmul_sub
  2.30     63.62     1.53 292059432     0.00     0.00  set_constants
  1.44     64.58     0.96                             matvec_sub
  1.13     65.33     0.75      201     3.73     3.73  add
  0.92     65.94     0.61      203     3.00     3.00  verify
  0.38     66.19     0.25  2317932     0.00     0.00  lhsinit
  0.24     66.35     0.16        1   160.00   160.00  print_results
  0.20     66.48     0.13  4195072     0.00     0.00  exact_solution
  0.09     66.54     0.06        1    60.00   128.17  binvrhs
  0.09     66.60     0.06                             wtime_
  0.05     66.63     0.03        1    30.00    52.87  exact_rhs
  0.02     66.64     0.01        2     5.00    54.38  initialize
  0.00     66.64     0.00      201     0.00    63.05  adi
  0.00     66.64     0.00       22     0.00     0.00  timer_clear
  0.00     66.64     0.00        2     0.00     3.00  elapsed_time
  0.00     66.64     0.00        1     0.00     8.12  error_norm
  0.00     66.64     0.00        1     0.00     0.00  rhs_norm
  0.00     66.64     0.00        1     0.00     0.00  timer_read
  0.00     66.64     0.00        1     0.00     3.00  timer_start
  0.00     66.64     0.00        1     0.00     3.00  timer_stop
```
* Overall time: 66.64 seconds (can see on the last row of the cumulative time)
* The function `compute_rhs` is the most time-consuming function with 18.29% of the total time, taking 60.05 ms per call.
* `y_solve` follows closely with 17.98% (90.68 ms/call) and `binvcrhs` with 17.26%.
* Less impactful functions were for example set_constants with 2.30% and matvec_sub with 1.44%.
    * Functions like printing results, initialization take very little time.

### _c local execution
```
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 19.84    227.94   227.94 2485324800     0.00     0.00  binvcrhs
 18.29    437.97   210.03      201     1.04     1.61  z_solve
 18.11    645.94   207.97      202     1.03     1.03  compute_rhs
 16.70    837.81   191.87      201     0.95     1.52  y_solve
 16.08   1022.46   184.65      201     0.92     1.48  x_solve
  7.96   1113.86    91.40 2485324800     0.00     0.00  matmul_sub
  1.62   1132.45    18.59 2485324800     0.00     0.00  matvec_sub
  0.86   1142.34     9.89      201     0.05     0.05  add
  0.19   1144.55     2.21 15436800     0.00     0.00  lhsinit
  0.19   1146.70     2.16 68026392     0.00     0.00  exact_solution
  0.07   1147.53     0.82 15436800     0.00     0.00  binvrhs
  0.07   1148.34     0.81        1     0.81     1.21  exact_rhs
  0.02   1148.55     0.20        2     0.10     0.92  initialize
  0.00   1148.59     0.04        1     0.04     0.17  error_norm
  0.00   1148.62     0.03        1     0.03     0.03  rhs_norm
  0.00   1148.62     0.01        1     0.01     1.24  verify
  0.00   1148.62     0.00      201     0.00     5.69  adi
  0.00   1148.62     0.00       22     0.00     0.00  timer_clear
  0.00   1148.62     0.00        2     0.00     0.00  elapsed_time
  0.00   1148.62     0.00        2     0.00     0.00  wtime_
  0.00   1148.62     0.00        1     0.00     0.00  print_results
  0.00   1148.62     0.00        1     0.00     0.00  set_constants
  0.00   1148.62     0.00        1     0.00     0.00  timer_read
  0.00   1148.62     0.00        1     0.00     0.00  timer_start
  0.00   1148.62     0.00        1     0.00     0.00  timer_stop
  ```	

* Total execution time: 1148.62 seconds
* The function `binvcrhs` is the most time-consuming function with 19.84% of the total time. It seems that self s/call is also very very small.
* z_solve (18.29%): 201 calls, each taking 1.04 seconds on average.
* compute_rhs (18.11%): 202 calls, 1.03 seconds per call.
* less impactful matmul_sub (7.96%) and matvec_sub (1.62%).

### Conclusion
* The most time-consuming functions are `compute_rhs`, `y_solve`, `binvcrhs`, `x_solve` and `z_solve`.
* In both cases `compute_rhs` is taking alot of time.
* What are possible solutions?
    * Reducing the number of calls of binvcrhs and compute_rhs, especially in the _c case.
    * Algorithmic optimizations in the most time-consuming functions
    * Parallelization of the most time-consuming functions
    * Utilizing memoization, if binvcrhs executes redundant calculations
    * Reviewing the data structures

## LCC3

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  us/call  us/call  name    
 62.51      0.05     0.05   201300     0.25     0.25  binvcrhs
 12.50      0.06     0.01   201300     0.05     0.05  matmul_sub
 12.50      0.07     0.01       62   161.31   161.31  compute_rhs
 12.50      0.08     0.01       61   163.96   491.87  z_solve
  0.00      0.08     0.00   201300     0.00     0.00  matvec_sub
  0.00      0.08     0.00    27792     0.00     0.00  exact_solution
  0.00      0.08     0.00    18300     0.00     0.00  binvrhs
  0.00      0.08     0.00    18300     0.00     0.00  lhsinit
  0.00      0.08     0.00       61     0.00   327.91  x_solve
  0.00      0.08     0.00       61     0.00   327.91  y_solve
  0.00      0.08     0.00        2     0.00     0.00  wtime_
  0.00      0.08     0.00        1     0.00     0.00  error_norm
  0.00      0.08     0.00        1     0.00     0.00  rhs_norm

* Total time is 0.08