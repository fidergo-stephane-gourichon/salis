SALIS: Simple A-Life Simulator
==============================

Overview
--------
Salis is a simulation similar to Tierra. On it, organisms
compete for space and time. Organisms evolve and replicate.

Requisites
----------
- Salis will be written in C89
- Salis is an API

Organisms
---------
- Registers
- Instruction pointer
- Seeker pointer
- Two associated memory blocks

Queue
-----
- Newborn organisms are placed on top of the queue
- Organisms are killed at the bottom of the queue
- Organisms halt when they execute faulty instructions (faults)

Faults
------
- IP or SP reaching ends of memory
- Perform search without a key
- Writing to non-local, allocated address
- Allocating block of less than minimum size
- Allocating block of more than maximum size
- SP on non-adjacent address when allocating block
- Swapping when not owning 2 memory blocks
- Splitting when not owning 2 memory blocks
- Dividing by zero