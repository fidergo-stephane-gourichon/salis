#include <assert.h>
#include <stdlib.h>
#include <salis.h>

static sbool g_isInit;
static suint g_cycles;
static suint g_epoch;

void
s_init ( suint ordr )
{
        assert ( !g_isInit );

        sm_init ( ordr );
        sp_init ();
        se_init ();

        g_isInit = STRUE;
}

void
s_quit ( void )
{
        assert ( g_isInit );

        se_quit ();
        sp_quit ();
        sm_quit ();

        g_isInit = SFALSE;
        g_cycles = 0;
        g_epoch  = 0;
}

void
s_save ( FILE * file )
{
        assert ( g_isInit );
        assert ( file );

        fwrite ( &g_isInit, sizeof ( sbool ), 1, file );
        fwrite ( &g_cycles, sizeof ( suint ), 1, file );
        fwrite ( &g_epoch,  sizeof ( suint ), 1, file );
}

void
s_load ( FILE * file )
{
        assert ( !g_isInit );
        assert ( file );

        fread ( &g_isInit, sizeof ( sbool ), 1, file );
        fread ( &g_cycles, sizeof ( suint ), 1, file );
        fread ( &g_epoch,  sizeof ( suint ), 1, file );
}

sbool
s_isInit ( void )
{
        return g_isInit;
}

suint
s_getCycles ( void )
{
        return g_cycles;
}

suint
s_getEpoch ( void )
{
        return g_epoch;
}

void
s_cycle ( void )
{
        suint pidx;

        assert ( g_isInit );
        assert ( sm_isInit ());
        assert ( sp_isInit ());
        assert ( se_isInit ());

        g_cycles ++;

        if ( !g_cycles ) {
                g_epoch ++;
        }

        if ( !sp_getCount ()) {
                goto end;
        }

        pidx = sp_getLast ();

        while ( 1 ) {
                sp_cycle ( pidx );

                if ( pidx == sp_getFirst ()) {
                        break;
                }

                pidx --;
                pidx %= sp_getCap ();
        }

        while ( sm_overCap ()) {
                sp_kill ();
        }

        end:
        se_cMutate ();
        se_pMutate ();
}