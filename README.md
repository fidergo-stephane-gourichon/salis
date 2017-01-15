# SALIS: A-Life Simulator

## Overview
*SALIS* is my newest artificial life project. Mainly a re-take on Tom Ray's
TIERRA simulation, but with my very own set of tweaks. Having a grasp on TIERRA
will make understanding this simulation a lot easier.

[Video about Tierra](https://www.youtube.com/watch?v=Wl5rRGVD0QI)
[Read about Tierra](http://life.ou.edu/pubs/doc/index.html#What)

For those that already know TIERRA, the main differences between it and SALIS
are:
- the replacement of templates with key-lock instruction pairs
- the addition of a SEEKER POINTER to all organisms

The seeker pointer is an attempt to bring extra spatial and temporal coherence
to the simulation. Allocation, reads and writes will take more time when done
between addresses that are far away, as a consequence of the SP having to
travel those distances at a speed of 1 byte per update. In other words, in
SALIS information can't travel faster than 1 byte per update (SALIS' speed of
light, if you will).

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
- Organisms may halt on, or ignore, faulty instructions (faults)

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