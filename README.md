# SALIS: A-Life Simulator

## Overview
*SALIS* is a simulation similar to Tierra.
On it, self-replicating organisms compete for space and time.
Over time, organisms evolve into novel forms.

To watch an introductory video about *SALIS*,
[go here.](https://www.youtube.com/watch?v=jCFmOCvy6po)

### Details
- *SALIS* is an API, so an UI must be written to communicate with it
- *SALIS* is written in C
- *SALIS* must be compiled as a static library (e.g. *libsalis.a*)

### Organisms consist of
- Registers
- Instruction pointer
- Seeker pointer
- Two associated memory blocks

### Queue
- Newborn organisms are placed on top of the queue
- Organisms are killed at the bottom of the queue
- Organisms halt when they execute faulty instructions (faults)

### Faults
- IP or SP reaching ends of memory
- Perform search without a key
- Writing to non-local, allocated address
- Allocating block of less than minimum size
- Allocating block of more than maximum size
- SP on non-adjacent address when allocating block
- Swapping when not owning 2 memory blocks
- Splitting when not owning 2 memory blocks
- Dividing by zero

## Building instructions
You'll need nothing but a C compiler (C89). A sample makefile (Sample.make)
is provided for GNU Make. Feel free to edit the makefile as needed.
Code should compile easily on all platforms and on all C89 compliant compilers.
If you run into any difficulties, please let me know!