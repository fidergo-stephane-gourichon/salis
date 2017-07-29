#ifndef SALIS_H
#define SALIS_H

#include <stdio.h>

#include <types.h>
#include <instset.h>
#include <compiler.h>
#include <loader.h>

#include <memory.h>
#include <process.h>
#include <evolver.h>

void  s_init      ( suint ordr );
void  s_quit      ( void );

void  s_save      ( FILE * file );
void  s_load      ( FILE * file );

sbool s_isInit    ( void );
suint s_getCycles ( void );
suint s_getEpoch  ( void );

void  s_cycle     ( void );

#endif