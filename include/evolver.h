#ifndef SALIS_EVOLVER_H
#define SALIS_EVOLVER_H

void  se_init      ( void );
void  se_quit      ( void );

void  se_save      ( FILE * file );
void  se_load      ( FILE * file );

sbool se_isInit    ( void );

suint se_getWRate  ( void );
suint se_getCRate  ( void );
suint se_getPRate  ( void );

suint se_getWNext  ( void );
suint se_getCNext  ( void );
suint se_getPNext  ( void );

void  se_setWRate  ( suint rate );
void  se_setCRate  ( suint rate );
void  se_setPRate  ( suint rate );
void  se_setNRate  ( suint rate );

sbyte se_wMutate   ( sbyte inst );
void  se_cMutate   ( void );
void  se_pMutate   ( void );
void  se_nMutate   ( void );

#endif