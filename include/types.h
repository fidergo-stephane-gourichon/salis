#ifndef SALIS_TYPES_H
#define SALIS_TYPES_H

#include <limits.h>

#if USHRT_MAX == 0xffffffff

typedef unsigned short suint;

#elif UINT_MAX == 0xffffffff

typedef unsigned int suint;

#elif ULONG_MAX == 0xffffffff

typedef unsigned long suint;

#endif

#define SNULL (( suint ) -1 )

#if UCHAR_MAX == 0xff

typedef unsigned char sbyte;
typedef unsigned char sbool;

#define SFALSE 0
#define STRUE  1

#endif

#endif