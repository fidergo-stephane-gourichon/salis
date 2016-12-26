### SALIS: Simple A-Life Simulator

## Overview
Salis is a simulation similar to Tierra.
On it, self-replicating organisms compete for space and time.
Over time, organisms evolve into novel forms.

To watch an introductory video about SALIS, go here:
[SALIS Intro](https://www.youtube.com/watch?v=jCFmOCvy6po)

## Summary
- Salis is written in C
- Salis is an API, so an UI must be written to communicate with it

## Organisms consist of
- Registers
- Stack
- Instruction pointer
- Seeker pointer
- Two associated memory blocks

## Queue
- Newborn organisms are placed on top of the queue
- Organisms are killed at the bottom of the queue
- Organisms halt when they execute faulty instructions (faults)

## Faults
- IP or SP reaching ends of memory
- Perform search without a key
- Writing to non-local, allocated address
- Allocating block of less than minimum size
- Allocating block of more than maximum size
- SP on non-adjacent address when allocating block
- Swapping when not owning 2 memory blocks
- Splitting when not owning 2 memory blocks
- Dividing by zero