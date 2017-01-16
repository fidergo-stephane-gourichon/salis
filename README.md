# SALIS: A-Life Simulator

## Overview
*SALIS* is my newest artificial life project. Mainly a re-take on Tom Ray's
TIERRA simulation, but with my very own set of tweaks. Having a grasp on TIERRA
will make understanding this simulation a lot easier.

- [video about TIERRA](https://www.youtube.com/watch?v=Wl5rRGVD0QI)
- [read about TIERRA](http://life.ou.edu/pubs/doc/index.html#What)

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

To watch an introductory video about *SALIS*
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

### Instruction set
*SALIS*' organisms read a simple language similar to ASM. This language
consists of 64 instructions, each with an associated name and symbol.
If *halting* is enabled, a program will halt whenever it commits a *fault*.
If not, faulty instructions are ignored.

#### Faults
- IP or SP reaching ends of memory
- Perform search or attempt a jump without a following key
- Writing to a non-locally-allocated or invalid address
- Reading (loading) from an invalid address
- Allocating block of less than minimum allowed size
- Allocating block of more than maximum allowed size
- SP on address non-adjacent to child memory block while allocating
- Swapping, freeing or splitting when not owning 2 memory blocks
- Dividing by zero

#### Instructions
|Name    |Sym.  |Val.  |Description                                                           |
|:-------|:-----|:-----|---------------------------------------------------------------------:|
|NOOP    |      |00    |No operation. Does nothing.                                           |
|NOP1-6  |1-6   |01-06 |No operation. Modifies registers to be used (r1x to r6x).             |
|JMPB-F* |()    |07-08 |Jump to lock matching following key.                                  |
|ADRB-F* |[]    |09-10 |Search for lock matching following key and store on r[1].             |
|IFNZ    |?     |11    |If r[1] is not zero, execute following instruction. Skip otherwise.   |
|ALLB-F* |{}    |12-13 |Allocate block of size stored on r[1]. Store its address on r[2].     |
|BSWP*   |%     |14    |Swap parent and child memory blocks.                                  |
|BCLR*   |&#124;|15    |Free child memory block.                                              |
|SPLT*   |$     |16    |Split. Child memory block becomes new organism.                       |
|HALT*   |z     |17    |Halt. Always stops execution (even if *halting* is disabled).         |
|ADDN    |+     |18    |Add (r[1] = r[2] + r[3]).                                             |
|SUBN    |-     |19    |Subtract (r[1] = r[2] - r[3]).                                        |
|MULN    |*     |20    |Multiply (r[1] = r[2] * r[3]).                                        |
|DIVN*   |/     |21    |Divide (r[1] = r[2] / r[3]). Faults if divisor is zero.               |
|INCN    |^     |22    |Increment (r[1]++).                                                   |
|DECN    |v     |23    |Decrement (r[1]--).                                                   |
|NOTN    |!     |24    |Not (r[1] != r[1]).                                                   |
|SHFL    |<     |25    |Shift left by 1 (r[1] << 1).                                          |
|SHFR    |>     |26    |Shift right by 1 (r[1] >> 1).                                         |
|ZERO    |0     |27    |Put zero (r[1] = 0).                                                  |
|LOAD*   |.     |28    |Load instruction at address pointed by r[1] into r[2].                |
|WRTE*   |:     |29    |Write instruction on r[2] to address pointed by r[1].                 |
|DUPL    |"     |30    |Duplicate value on r[1] into r[2].                                    |
|SWAP    |x     |31    |Swap values on r[1] and r[2].                                         |
|KEYA-P  |a-p   |32-47 |Keys.                                                                 |
|LOKA-P  |A-P   |48-63 |Locks.                                                                |

Instructions that may fault are marked with an asterisk (*) on the table above.

Instructions that modify values on registers may be followed by
up to 3 register modifiers (r[1], r[2] and r[3]). By default, all registers
are set to r1x.

|Sample  |r[1] |r[2] |r[3] |Meaning                                   |
|:-------|:----|:----|:----|-----------------------------------------:|
|+236    |r2x  |r3x  |r6x  |r2x = r3x + r6x                           |
|-22     |r2x  |r2x  |r1x  |r2x = r2x - r1x                           |
|*       |r1x  |r1x  |r1x  |r1x = r1x * r1x                           |
|!5      |r5x  |---  |---  |r5x = !r5x                                |
|"12     |r1x  |r2x  |---  |swap r1x and r2x                          |
|]a2     |r2x  |---  |---  |r2x = address of closest following LOKA   |
|[b      |r1x  |---  |---  |r1x = address of closest previous LOKB    |

## Building instructions
You'll need nothing but a C compiler (C89). A sample makefile (Sample.make)
is provided for GNU Make. Feel free to edit the makefile as needed.
Code should compile easily on all platforms and on all C89 compliant compilers.
If you run into any difficulties, please let me know!