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
