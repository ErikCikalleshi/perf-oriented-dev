## Exercise 1

```
massif-visualizer [massif-data-file] => massif.out.%pid
```

As we can see both programs need to allocate more or less the same amount of memory eventho the workload is different

### a
![alt text](a.png)

### s
![alt text](s.png)

Runtime with massif
3.3s

Runtime without massif
0.3s

As for ssca2 

![alt text](ssca2.png)

The program's memory usage starts at 8.5 MB of the heap. We firtsly generate "Scal Data" three times. It stabilizes for a while before experiencing a sudden peak to 25.69 MB for the betweennessCentrality() of function for all three generated sets. Then it followes a drop to 10 MB. Afterward, it fluctuates between around 8 to 25 MB Towards the end, it reaches another peak of 20.99 MB. In general the computeGraph() functions are stable as they need with time a little more memory. 


The perturbation in execution time caused by using Massif can be potentially massive.

Runtime with massif
1.25min

Runtime without massif
0.51s


## Exercise 2




a)
```	
 Performance counter stats for './npb_bt_a':
     6,851,824,686      L1-dcache-load-misses:u   #    4.37% of all L1-dcache accesses  (10.71%)
   156,737,123,628      L1-dcache-loads:u                                             (14.29%)
     4,765,522,189      L1-dcache-prefetch-misses:u                                     (14.29%)
               398      L1-dcache-prefetches:u                                        (14.29%)
     2,427,321,825      L1-dcache-store-misses:u                                      (14.29%)
    73,845,614,252      L1-dcache-stores:u                                            (14.29%)
        48,642,427      L1-icache-load-misses:u   #    0.03% of all L1-icache accesses  (14.29%)
   141,648,487,195      L1-icache-loads:u                                             (14.29%)
       311,232,083      LLC-load-misses:u         #   52.23% of all LL-cache accesses  (14.29%)
       595,937,592      LLC-loads:u                                                   (14.29%)
       501,148,957      LLC-prefetch-misses:u                                         (7.14%)
       803,756,368      LLC-prefetches:u                                              (7.14%)
        32,216,513      LLC-store-misses:u                                            (7.14%)
       456,636,730      LLC-stores:u                                                  (7.14%)
     1,832,202,799      branch-load-misses:u                                          (10.71%)
     1,852,559,669      branch-loads:u                                                (14.28%)
         1,198,828      dTLB-load-misses:u        #    0.00% of all dTLB cache accesses  (14.28%)
   156,727,712,592      dTLB-loads:u                                                  (14.28%)
           304,426      dTLB-store-misses:u                                           (14.28%)
    73,961,570,202      dTLB-stores:u                                                 (14.28%)
             5,488      iTLB-load-misses:u        #    0.00% of all iTLB cache accesses  (14.28%)
   383,391,530,027      iTLB-loads:u                                                  (14.28%)
               560      node-load-misses:u                                            (14.28%)
       301,907,089      node-loads:u                                                  (14.28%)
             1,484      node-prefetch-misses:u                                        (7.14%)
       475,242,662      node-prefetches:u                                             (7.14%)
                 0      node-store-misses:u                                           (7.14%)
        25,147,355      node-stores:u                                                 (7.14%)

      73.650137967 seconds time elapsed

      72.723236000 seconds user
       0.012604000 seconds sys
```	

s)
```
 Performance counter stats for './ssca2 17':

     4,130,196,129      L1-dcache-load-misses:u   #   36.42% of all L1-dcache accesses  (10.71%)
    11,341,237,747      L1-dcache-loads:u                                             (14.28%)
       483,874,456      L1-dcache-prefetch-misses:u                                     (14.28%)
           836,558      L1-dcache-prefetches:u                                        (14.28%)
       647,540,900      L1-dcache-store-misses:u                                      (14.29%)
     2,647,752,876      L1-dcache-stores:u                                            (14.29%)
           546,805      L1-icache-load-misses:u   #    0.00% of all L1-icache accesses  (14.29%)
    32,075,225,580      L1-icache-loads:u                                             (14.29%)
       290,288,003      LLC-load-misses:u         #   10.22% of all LL-cache accesses  (14.29%)
     2,840,829,137      LLC-loads:u                                                   (14.29%)
         1,229,459      LLC-prefetch-misses:u                                         (7.14%)
         4,755,109      LLC-prefetches:u                                              (7.14%)
        39,641,754      LLC-store-misses:u                                            (7.14%)
     1,911,332,731      LLC-stores:u                                                  (7.14%)
    10,819,311,257      branch-load-misses:u                                          (10.72%)
     5,853,271,583      branch-loads:u                                                (14.29%)
       729,368,723      dTLB-load-misses:u        #    6.41% of all dTLB cache accesses  (14.29%)
    11,384,895,643      dTLB-loads:u                                                  (14.29%)
       140,237,284      dTLB-store-misses:u                                           (14.29%)
     2,668,668,444      dTLB-stores:u                                                 (14.29%)
           235,872      iTLB-load-misses:u        #    0.00% of all iTLB cache accesses  (14.29%)
    34,355,222,136      iTLB-loads:u                                                  (14.29%)
               300      node-load-misses:u                                            (14.29%)
       288,325,059      node-loads:u                                                  (14.29%)
             1,329      node-prefetch-misses:u                                        (7.14%)
         1,130,419      node-prefetches:u                                             (7.14%)
                 0      node-store-misses:u                                           (7.14%)
        39,532,648      node-stores:u                                                 (7.14%)

      26.739826313 seconds time elapsed

      26.348368000 seconds user
       0.014802000 seconds sys
```

L1 Cache:
- ssca2 has a significantly higher L1 cache miss rate (36.42%) compared to npb_bt_a (4.37%).

LLC (Last Level Cache):
- npb_bt_a has a higher LLC miss rate (52.23%) compared to ssca2 (10.22%).

Branches:
- ssca2 has a much higher number of branch load misses, indicating potentially less optimized branch prediction.

TLB (Translation Lookaside Buffer):
- ssca2 has more dTLB load misses, suggesting more frequent translation cache misses.


### Conclusion 
In summary, ssca2 experiences higher cache miss rates across different levels compared to npb_bt_a. This suggests potential inefficiencies in memory access and branch prediction in ssca2. Further optimization efforts may be needed to enhance its performance.

### Time influence
npb_bt_a:

time with perf: 1:16.39
time without perf: 1:13.95

ssca2:
time with perf: 0:31.75
time without perf: 0:32.53