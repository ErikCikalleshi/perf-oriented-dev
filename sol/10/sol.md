erikck@DESKTOP-PSB6OBO:~/perf-oriented-dev/sol/10$ valgrind --leak-check=full --show-leak-kinds=all ./unrolled 10 10000000 512 0 100
==32536== Memcheck, a memory error detector
==32536== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==32536== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==32536== Command: ./unrolled 10 10000000 512 0 100
==32536==
==32536== Warning: set address range perms: large range [0x59c87040, 0x18af57040) (undefined)
==32536== Warning: set address range perms: large range [0x18af58040, 0x2bc228040) (undefined)
==32536== Warning: set address range perms: large range [0x2bc229040, 0x3ed4f9040) (undefined)
Killed

Arguments: tiered_list 1 1000 8 1 99
slurmstepd-n001: error: **_ JOB 115104 ON n001 CANCELLED AT 2024-06-04T20:55:20 DUE TO TIME LIMIT _**
slurmstepd-n001: error: Detected 1 oom_kill event in StepId=115104.batch. Some of the step tasks have been OOM Killed.
