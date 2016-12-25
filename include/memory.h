#ifndef SALIS_MEMORY_H
#define SALIS_MEMORY_H

void  sm_init      ( suint ordr );
void  sm_quit      ( void );

void  sm_save      ( FILE * file );
void  sm_load      ( FILE * file );

sbool sm_isInit    ( void );
suint sm_getOrder  ( void );
suint sm_getSize   ( void );
suint sm_getUsed   ( void );
sbool sm_overCap   ( void );

sbool sm_isValid   ( suint addr );
sbool sm_isUsed    ( suint addr );
void  sm_alloc     ( suint addr );
void  sm_dealloc   ( suint addr );
sbyte sm_getInst   ( suint addr );
void  sm_setInst   ( suint addr, sbyte inst );

#endif