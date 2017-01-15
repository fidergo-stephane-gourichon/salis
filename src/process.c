#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <salis.h>

static sbool   g_isInit;
static suint   g_count;
static suint   g_cap;
static suint   g_first;
static suint   g_last;
static SProc * g_procs;
static sbool   g_fhalt;

void
sp_init ( void )
{
        suint ps = sizeof ( SProc );

        assert ( !g_isInit );
        assert ( !s_isInit ());

        g_isInit = STRUE;
        g_cap    = 1;
        g_first  = SNULL;
        g_last   = SNULL;
        g_procs  = calloc ( g_cap, ps );

        assert ( g_procs );
}

void
sp_quit ( void )
{
        assert ( g_isInit );
        assert ( s_isInit ());

        free ( g_procs );

        g_isInit = SFALSE;
        g_count  = 0;
        g_cap    = 0;
        g_first  = 0;
        g_last   = 0;
        g_procs  = NULL;
        g_fhalt  = SFALSE;
}

void
sp_save ( FILE * file )
{
        assert ( g_isInit );
        assert ( file );

        fwrite ( &g_isInit, sizeof ( sbool ), 1, file );
        fwrite ( &g_count,  sizeof ( suint ), 1, file );
        fwrite ( &g_cap,    sizeof ( suint ), 1, file );
        fwrite ( &g_first,  sizeof ( suint ), 1, file );
        fwrite ( &g_last,   sizeof ( suint ), 1, file );
        fwrite ( &g_fhalt,  sizeof ( sbool ), 1, file );

        fwrite ( g_procs, sizeof ( SProc ), g_cap, file );
}

void
sp_load ( FILE * file )
{
        assert ( !g_isInit );
        assert ( file );

        fread ( &g_isInit, sizeof ( sbool ), 1, file );
        fread ( &g_count,  sizeof ( suint ), 1, file );
        fread ( &g_cap,    sizeof ( suint ), 1, file );
        fread ( &g_first,  sizeof ( suint ), 1, file );
        fread ( &g_last,   sizeof ( suint ), 1, file );
        fread ( &g_fhalt,  sizeof ( sbool ), 1, file );

        g_procs = calloc ( g_cap, sizeof ( SProc ));

        assert ( g_procs );

        fread ( g_procs, sizeof ( SProc ), g_cap, file );
}

sbool
sp_isInit ( void )
{
        return g_isInit;
}

suint
sp_getCount ( void )
{
        return g_count;
}

suint
sp_getCap ( void )
{
        return g_cap;
}

suint
sp_getFirst ( void )
{
        return g_first;
}

suint
sp_getLast ( void )
{
        return g_last;
}

SProc *
sp_getProc ( suint pidx )
{
        assert ( g_isInit );
        assert ( pidx < g_cap );

        return &g_procs [ pidx ];
}

suint *
sp_getPData ( suint pidx )
{
        assert ( g_isInit );
        assert ( pidx < g_cap );

        return ( suint *) &g_procs [ pidx ];
}

static sbool
isFree ( const SProc * proc )
{
        sbool s1;

        assert ( g_isInit );
        assert ( proc );

        s1 = !!( proc -> b1s );

#ifndef NDEBUG
        if ( !s1 ) {
                assert ( !proc -> b1p );
                assert ( !proc -> b2p );
                assert ( !proc -> b2s );
        }
#endif

        return !s1;
}

sbool
sp_isFree ( suint pidx )
{
        assert ( g_isInit );
        assert ( pidx < g_cap );

        return isFree ( sp_getProc ( pidx ));
}

static sbool
inBlock ( suint lo, suint sz, suint addr )
{
        assert ( g_isInit );
        assert ( sm_isValid ( lo ));
        assert ( sm_isValid ( addr ));

        return ( addr >= lo ) && ( addr < ( lo + sz ));
}

static sbool
isLocal ( const SProc * proc, suint addr )
{
        assert ( g_isInit );
        assert ( proc );
        assert ( sm_isValid ( addr ));

        if ( inBlock ( proc -> b1p, proc -> b1s, addr ) ||
             inBlock ( proc -> b2p, proc -> b2s, addr )) {
                return STRUE;
        }

        return SFALSE;
}

sbool
sp_isLocal ( suint pidx, suint addr )
{
        SProc * pr;

        assert ( g_isInit );
        assert ( pidx < g_cap );
        assert ( sm_isValid ( addr ));

        pr = sp_getProc ( pidx );

        return isLocal ( pr, addr );
}

sbool
sp_isOnMB1 ( suint pidx, suint addr )
{
        SProc * pr;

        assert ( g_isInit );
        assert ( pidx < g_cap );
        assert ( sm_isValid ( addr ));

        pr = sp_getProc ( pidx );

        return inBlock ( pr -> b1p, pr -> b1s, addr );
}

sbool
sp_isOnMB2 ( suint pidx, suint addr )
{
        SProc * pr;

        assert ( g_isInit );
        assert ( pidx < g_cap );
        assert ( sm_isValid ( addr ));

        pr = sp_getProc ( pidx );

        return inBlock ( pr -> b2p, pr -> b2s, addr );
}

sbool
sp_getFHalt ( void )
{
        return g_fhalt;
}

void
sp_setFHalt ( sbool halt )
{
        assert ( g_isInit );

        g_fhalt = halt;
}

static void
pRealloc ( suint lock )
{
        suint   nc;
        suint   ps;
        SProc * nd;
        suint   pi;

        assert ( g_isInit );
        assert ( g_count == g_cap );
        assert ( lock < g_cap );

        nc = g_cap * 2;
        ps = sizeof ( SProc );
        nd = calloc ( nc, ps );

        assert ( nd );

        /* copy all forward */
        pi = lock;

        while ( 1 ) {
                suint oa = pi % g_cap;

                memcpy ( nd + pi, g_procs + oa, ps );

                if ( oa == g_last ) {
                        g_last = pi;
                        break;
                }

                pi ++;
        }

        if ( lock == g_first ) {
                goto end;
        }

        /* copy all backward */
        pi = ( lock - 1 ) % nc;

        while ( 1 ) {
                suint oa = pi % g_cap;

                memcpy ( nd + pi, g_procs + oa, ps );

                if ( oa == g_first ) {
                        g_first = pi;
                        break;
                }

                pi --;
                pi %= nc;
        }

        end:
        free ( g_procs );

        g_cap   = nc;
        g_procs = nd;
}

static suint
newProc ( suint lock )
{
        assert ( g_isInit );

        if ( g_count == g_cap ) {
                pRealloc ( lock );
        }

        g_count ++;

        if ( g_count == 1 ) {
                g_first = 0;
                g_last  = 0;

                return 0;
        }

        g_last ++;
        g_last %= g_cap;

        return g_last;
}

void
sp_create ( suint mb1p, suint mb1s, suint lock, sbool isal )
{
        suint   mi;
        suint   pi;
        SProc * pr;

        assert ( g_isInit );
        assert ( sm_isValid ( mb1p ));
        assert ( sm_isValid ( mb1p + mb1s - 1 ));

        if ( isal ) {
                goto end;
        }

        for ( mi = 0; mi < mb1s; mi ++ ) {
                suint sa = mb1p + mi;

                assert ( !sm_isUsed ( sa ));

                sm_alloc ( sa );
        }


        end:
        pi = newProc ( lock );
        pr = sp_getProc ( pi );

        pr -> b1p  = mb1p;
        pr -> b1s  = mb1s;
        pr -> b2p  = mb1p;
        pr -> ip   = mb1p;
        pr -> sp   = mb1p;
}

static void
freeBlock ( suint addr, suint size )
{
        suint mi;

        for ( mi = 0; mi < size; mi ++ ) {
                suint ma = addr + mi;

                assert ( sm_isValid ( ma ));
                assert ( sm_isUsed ( ma ));

                sm_dealloc ( ma );
        }
}

static void
freeProcMem ( const SProc * proc )
{
        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        freeBlock ( proc -> b1p, proc -> b1s );
        freeBlock ( proc -> b2p, proc -> b2s );
}

void
sp_kill ( void )
{
        suint   nf;
        SProc * lp;

        assert ( g_isInit );
        assert ( g_first != SNULL );
        assert ( g_last  != SNULL );

        nf = g_first;
        lp = sp_getProc ( nf );

        assert ( !sp_isFree ( nf ));

        freeProcMem ( lp );
        memset ( lp, 0, sizeof ( SProc ));

        g_count --;

        if ( g_first == g_last ) {
                g_first = SNULL;
                g_last  = SNULL;

                return;
        }

        g_first ++;
        g_first %= g_cap;
}

static void
onFault ( SProc * proc )
{
        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        if ( !g_fhalt ) {
                proc -> ip ++;
                proc -> sp = proc -> ip;
        }
}

static sbool
seek ( SProc * proc, sbool fwrd )
{
        suint na;
        sbyte ni;
        sbyte si;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        na = proc -> ip + 1;

        if ( !sm_isValid ( na )) {
                onFault ( proc );
                return SFALSE;
        }

        ni = sm_getInst ( na );

        if ( !si_isKey ( ni )) {
                onFault ( proc );
                return SFALSE;
        }

        si = sm_getInst ( proc -> sp );

        if ( si_klMatch ( ni, si )) {
                return STRUE;
        }

        proc -> sp += ( fwrd ? 1 : -1 );

        return SFALSE;
}

static void
jump ( SProc * proc )
{
#ifndef NDEBUG
        sbyte ni;
        sbyte si;
#endif

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

#ifndef NDEBUG
        ni = sm_getInst ( proc -> ip + 1 );
        si = sm_getInst ( proc -> sp );

        assert ( si_isKey ( ni ));
        assert ( si_isLock ( si ));
        assert ( si_klMatch ( ni, si ));
#endif

        proc -> ip = proc -> sp;
}

static suint *
getMod ( SProc * proc, suint addr )
{
        sbyte mi;
        sbyte of;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));
        assert ( sm_isValid ( addr ));

        mi = sm_getInst ( addr );
        of = mi - SNOP1;

        assert ( si_isMod ( mi ));

        return &proc -> reg [ of ];
}

static void
setMods ( SProc * proc, suint ** regs, sbyte qtty, sbool ofst )
{
        sbyte ri;
        suint ma;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        assert ( regs );
        assert ( qtty && ( qtty < 4 ));

        ma = proc -> ip + ( ofst ? 2 : 1 );

        for ( ri = 0; ri < qtty; ri ++ ) {
                regs [ ri ] = &proc -> reg [ 0 ];
        }

        for ( ri = 0; ri < qtty; ri ++ ) {
                sbyte mi;
                suint mn = ma + ri;

                if ( !sm_isValid ( mn )) {
                        break;
                }

                mi = sm_getInst ( mn );

                if ( !si_isMod ( mi )) {
                        break;
                }

                regs [ ri ] = getMod ( proc, ma + ri );
        }
}

static void
addr ( SProc * proc )
{
#ifndef NDEBUG
        sbyte ni;
        sbyte si;
#endif

        suint * rg;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

#ifndef NDEBUG
        ni = sm_getInst ( proc -> ip + 1 );
        si = sm_getInst ( proc -> sp );

        assert ( si_isKey ( ni ));
        assert ( si_isLock ( si ));
        assert ( si_klMatch ( ni, si ));
#endif

        setMods ( proc, &rg, 1, STRUE );

        *rg = proc -> sp;

        proc -> ip ++;
        proc -> sp = proc -> ip;
}

static void
ifnz ( SProc * proc )
{
        suint * rg;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        setMods ( proc, &rg, 1, SFALSE );

        if ( sm_isValid ( proc -> ip + 1 )) {
                sbyte nx = sm_getInst ( proc -> ip + 1 );
                sbool im = si_isMod ( nx );

                if ( im ) {
                        proc -> ip ++;
                }
        }

        proc -> ip += (( *rg ) ? 1 : 2 );
        proc -> sp  = proc -> ip;
}

static void
freeBlock2 ( SProc * proc )
{
        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));
        assert ( proc -> b2p != proc -> b1p );

        freeBlock ( proc -> b2p, proc -> b2s );

        proc -> b2p = proc -> b1p;
        proc -> b2s = 0;
}

static void
alloc ( SProc * proc, sbool fwrd )
{
        suint * rg [ 2 ];
        suint   bs;
        suint   in;
        sbool   us;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        /* halt if already allocated */
        if (( proc -> ip == proc -> sp ) && proc -> b2s ) {
                onFault ( proc );
                return;
        }

        setMods ( proc, rg, 2, SFALSE );

        bs = *rg [ 0 ];

        /* halt if block size is of invalid size */
        if ( bs < sm_getMinBSize () || bs > sm_getMaxBSize ()) {
                onFault ( proc );
                return;
        }

        /* allocation was successful */
        if ( bs == proc -> b2s ) {
                proc -> ip ++;
                proc -> sp = proc -> ip;
                *rg [ 1 ]  = proc -> b2p;

                return;
        }

        in = fwrd ? 1 : -1;
        us = sm_isUsed ( proc -> sp );

        /* crashed into allocated space */
        if ( us ) {
                if ( proc -> b2p != proc -> b1p ) {
                        freeBlock2 ( proc );
                }

                proc -> sp += in;

                return;
        }

        assert ( !sm_isUsed ( proc -> sp ));

        /* sp must stay adjecent to allocated memory block */
        if ( proc -> b2s ) {
                suint of = ( fwrd ? proc -> b2s : ( suint ) -1);
                suint ad = proc -> b2p + of;

                if ( ad != proc -> sp ) {
                        onFault ( proc );
                        return;
                }
        }

        /* enlarge block */
        sm_alloc ( proc -> sp );

        proc -> b2s ++;

        if (( proc -> b1p == proc -> b2p ) || !fwrd ) {
                proc -> b2p = proc -> sp;
        }

        proc -> sp += in;
}

static void
bswap ( SProc * proc )
{
        suint pt;
        suint st;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        if ( !proc -> b2s ) {
                onFault ( proc );
                return;
        }

        assert ( proc -> b2p != proc -> b1p );

        pt = proc -> b1p;
        st = proc -> b1s;

        proc -> b1p = proc -> b2p;
        proc -> b1s = proc -> b2s;

        proc -> b2p = pt;
        proc -> b2s = st;

        proc -> ip ++;
        proc -> sp = proc -> ip;
}

static void
bclear ( SProc * proc )
{
        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        if ( !proc -> b2s ) {
                onFault ( proc );
                return;
        }

        assert ( proc -> b2p != proc -> b1p );
        freeBlock2 ( proc );

        proc -> ip ++;
        proc -> sp = proc -> ip;
}

static void
split ( SProc * proc )
{
        suint pi;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        if ( !proc -> b2s ) {
                onFault ( proc );
                return;
        }

        pi = proc - g_procs;

        sp_create ( proc -> b2p, proc -> b2s, pi, STRUE );

        proc = &g_procs [ pi ];

        assert ( proc );

        proc -> b2p = proc -> b1p;
        proc -> b2s = 0;

        proc -> ip ++;
        proc -> sp = proc -> ip;
}

static void
r3op ( SProc * proc, sbyte inst )
{
        suint * rg [ 3 ];

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        setMods ( proc, rg, 3, SFALSE );

        /* halt if division by zero */
        if (( inst == SDIVN ) && ( *rg [ 2 ] == 0 )) {
                onFault ( proc );
                return;
        }

        switch ( inst ) {
        case SADDN:
                *rg [ 0 ] = *rg [ 1 ] + *rg [ 2 ];
                break;
        case SSUBN:
                *rg [ 0 ] = *rg [ 1 ] - *rg [ 2 ];
                break;
        case SMULN:
                *rg [ 0 ] = *rg [ 1 ] * *rg [ 2 ];
                break;
        case SDIVN:
                *rg [ 0 ] = *rg [ 1 ] / *rg [ 2 ];
                break;
        default:
                assert ( 0 );
        }

        proc -> ip ++;
        proc -> sp = proc -> ip;
}

static void
r1op ( SProc * proc, sbyte inst )
{
        suint * rg;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        setMods ( proc, &rg, 1, SFALSE );

        switch ( inst ) {
        case SINCN:
                ( *rg ) ++;
                break;
        case SDECN:
                ( *rg ) --;
                break;
        case SNOTN:
                *rg = !( *rg );
                break;
        case SSHFL:
                *rg <<= 1;
                break;
        case SSHFR:
                *rg >>= 1;
                break;
        case SZERO:
                *rg = 0;
                break;
        default:
                assert ( 0 );
        }

        proc -> ip ++;
        proc -> sp = proc -> ip;
}

static void
load ( SProc * proc )
{
        suint * rg [ 2 ];

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        setMods ( proc, rg, 2, SFALSE );

        if ( !sm_isValid ( *rg [ 0 ] )) {
                onFault ( proc );
                return;
        }

        if ( proc -> sp == *rg [ 0 ] ) {
                *rg [ 1 ] = sm_getInst ( *rg [ 0 ] );

                proc -> ip ++;
                proc -> sp = proc -> ip;

                return;
        }

        if ( proc -> sp > *rg [ 0 ] ) {
                proc -> sp --;
        } else {
                proc -> sp ++;
        }
}

static sbool
writable ( SProc * proc, suint addr )
{
        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        if ( !sm_isValid ( addr ))   { return SFALSE; }
        if ( !sm_isUsed ( addr ))    { return STRUE; }
        if ( isLocal ( proc, addr )) { return STRUE; }

        return SFALSE;
}

static void
write ( SProc * proc )
{
        suint * rg [ 2 ];

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        setMods ( proc, rg, 2, SFALSE );

        if ( !sm_isValid ( *rg [ 0 ] )) {
                onFault ( proc );
                return;
        }

        if ( !si_isInst ( *rg [ 1 ] )) {
                onFault ( proc );
                return;
        }

        if ( proc -> sp == *rg [ 0 ] ) {
                if ( !writable ( proc, *rg [ 0 ] )) {
                        onFault ( proc );
                        return;
                }

                sm_setInst ( *rg [ 0 ], se_wMutate ( *rg [ 1 ] ));

                proc -> ip ++;
                proc -> sp = proc -> ip;

                return;
        }

        if ( proc -> sp > *rg [ 0 ] ) {
                proc -> sp --;
        } else {
                proc -> sp ++;
        }
}

static void
r2op ( SProc * proc, sbyte inst )
{
        suint * rg [ 2 ];
        suint   v0;
        suint   v1;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        setMods ( proc, rg, 2, SFALSE );

        v0 = *rg [ 0 ];
        v1 = *rg [ 1 ];

        switch ( inst ) {
        case SDUPL:
                *rg [ 1 ] = v0;
                break;
        case SSWAP:
                *rg [ 0 ] = v1;
                *rg [ 1 ] = v0;
                break;
        default:
                assert ( 0 );
        }

        proc -> ip ++;
        proc -> sp = proc -> ip;
}

#ifndef NDEBUG

static void
isFreeValid ( SProc * proc )
{
        suint * pd;
        suint   ei;

        assert ( g_isInit );
        assert ( proc );
        assert ( isFree ( proc ));

        pd = sp_getPData ( proc - g_procs );

        for ( ei = 0; ei < SPELM_COUNT; ei ++ ) {
                assert ( !pd [ ei ] );
        }
}

static void
isUsedValid ( SProc * proc )
{
        suint mi;

        assert ( g_isInit );
        assert ( proc );
        assert ( !isFree ( proc ));

        assert ( proc -> b1s );

        if ( proc -> b2p != proc -> b1p ) {
                assert ( proc -> b2s );
        }

        for ( mi = 0; mi < proc -> b1s; mi ++ ) {
                suint ma = proc -> b1p + mi;

                assert ( sm_isValid ( ma ));
                assert ( sm_isUsed  ( ma ));
        }

        for ( mi = 0; mi < proc -> b2s; mi ++ ) {
                suint ma = proc -> b2p + mi;

                assert ( sm_isValid ( ma ));
                assert ( sm_isUsed  ( ma ));
        }
}

void
sp_isValid ( suint pidx )
{
        SProc * pr;

        assert ( g_isInit );
        assert ( pidx < g_cap );

        pr = sp_getProc ( pidx );

        if ( isFree ( pr )) {
                isFreeValid ( pr );
        } else {
                isUsedValid ( pr );
        }
}

#endif

void
sp_cycle ( suint pidx )
{
        SProc * pr;
        sbyte   in;

        assert ( g_isInit );
        assert ( pidx < g_cap );
        assert ( !sp_isFree ( pidx ));

        pr = sp_getProc ( pidx );

        if ( !sm_isValid ( pr -> ip ) || !sm_isValid ( pr -> sp )) {
                onFault ( pr );
                return;
        }

        in = sm_getInst ( pr -> ip );

        switch ( in ) {
        case SJMPB:
                if ( seek ( pr, SFALSE )) {
                        jump ( pr );
                }
                break;
        case SJMPF:
                if ( seek ( pr, STRUE )) {
                        jump ( pr );
                }
                break;
        case SADRB:
                if ( seek ( pr, SFALSE )) {
                        addr ( pr );
                }
                break;
        case SADRF:
                if ( seek ( pr, STRUE )) {
                        addr ( pr );
                }
                break;
        case SIFNZ:
                ifnz ( pr );
                break;
        case SALLB:
                alloc ( pr, SFALSE );
                break;
        case SALLF:
                alloc ( pr, STRUE );
                break;
        case SBSWP:
                bswap ( pr );
                break;
        case SBCLR:
                bclear ( pr );
                break;
        case SSPLT:
                split ( pr );
                break;
        case SHALT:
                return;
        case SADDN:
        case SSUBN:
        case SMULN:
        case SDIVN:
                r3op ( pr, in );
                break;
        case SINCN:
        case SDECN:
        case SNOTN:
        case SSHFL:
        case SSHFR:
        case SZERO:
                r1op ( pr, in );
                break;
        case SLOAD:
                load ( pr );
                break;
        case SWRTE:
                write ( pr );
                break;
        case SDUPL:
        case SSWAP:
                r2op ( pr, in );
                break;
        default:
                pr -> ip ++;
                pr -> sp = pr -> ip;
        }
}