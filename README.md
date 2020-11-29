# mips-symulator-in-C
Implemented in C symulator of MIPS32

Program that simulates the MIPS microarchitecture (single cycle execution). 
With implemented basic operations of the MIPS microarchitecture, such as fetching instructions from the instruction memory,
decoding instructions, executing the proper arithmetic operations, reading data from the register file or the data memory, 
and determining the address of the next instructions.
Simulator read an executable file that contains several instructions (represented in hexadecimal).
The simulator provides two modes: debug and run mode.
In the debug mode, the simulator show the CC, PC, and register values after executing an instruction every cycle.
In the run mode, your simulator will run the given executable file and show the CC, PC, and register values at the end of the simulation.
