include header in main source file.
	#include "prof.h"

compile with these flags:
	-finstrument-functions
	-rdynamic
	-Wl,-no-pie

example:
	gcc -g -finstrument-functions -rdynamic -Wl,-no-pie main.c

(optional) disable address space layout randomization:
	echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

example output:
--> main 0x7fffffffe2f0
 --> print_prime 0x7fffffffe2c0
  --> isprime 0x7fffffffe290
  <-- isprime 0x7fffffffe290 4763
  --> isprime 0x7fffffffe290
  <-- isprime 0x7fffffffe290 2129
 <-- print_prime 0x7fffffffe2c0 20088
<-- main 0x7fffffffe2f0 75345
    ^    ^              ^
    |    |              time duration (nanoseconds)
    |    \_____________ frame address of the current function
    \__________________ function name

only tested on x86_64 linux, c, clang, gcc.
