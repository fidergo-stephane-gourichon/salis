#ifndef SALIS_PROCESS_H
#define SALIS_PROCESS_H

#define SPROC_RGCNT 6
#define SPELM_COUNT ( 6 + SPROC_RGCNT )

struct SProc_t
{
        suint b1p;
        suint b1s;
        suint b2p;
        suint b2s;

        suint ip;
        suint sp;

        suint reg [ SPROC_RGCNT ];
};

typedef struct SProc_t SProc;

void    sp_init     ( void );
void    sp_quit     ( void );

void    sp_save     ( FILE * file );
void    sp_load     ( FILE * file );

sbool   sp_isInit   ( void );
suint   sp_getCount ( void );
suint   sp_getCap   ( void );
suint   sp_getFirst ( void );
suint   sp_getLast  ( void );
SProc * sp_getProc  ( suint pidx );
suint * sp_getPData ( suint pidx );
sbool   sp_isFree   ( suint pidx );
sbool   sp_isLocal  ( suint pidx, suint addr );
sbool   sp_isOnMB1  ( suint pidx, suint addr );
sbool   sp_isOnMB2  ( suint pidx, suint addr );

void    sp_create   ( suint mb1p, suint mb1s, suint lock, sbool isal );
void    sp_kill     ( void );

void    sp_cycle    ( suint pidx );

#endif