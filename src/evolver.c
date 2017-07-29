#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <salis.h>

#define PMUT_OFFSET 4

sbool g_isInit;

suint g_wRate;
suint g_cRate;
suint g_pRate;

suint g_wNext;
suint g_cNext;
suint g_pNext;

void
se_init ( void )
{
        assert ( !g_isInit );
        assert ( !s_isInit ());

        srand ( time ( NULL ));

        g_isInit = STRUE;
}

void
se_quit ( void )
{
        assert ( g_isInit );
        assert ( s_isInit ());

        g_isInit = SFALSE;
        g_wRate  = 0;
        g_cRate  = 0;
        g_pRate  = 0;
        g_wNext  = 0;
        g_cNext  = 0;
        g_pNext  = 0;
}

void
se_save ( FILE * file )
{
        assert ( g_isInit );
        assert ( file );

        fwrite ( &g_isInit, sizeof ( sbool ), 1, file );
        fwrite ( &g_wRate,  sizeof ( suint ), 1, file );
        fwrite ( &g_cRate,  sizeof ( suint ), 1, file );
        fwrite ( &g_pRate,  sizeof ( suint ), 1, file );
        fwrite ( &g_wNext,  sizeof ( suint ), 1, file );
        fwrite ( &g_cNext,  sizeof ( suint ), 1, file );
        fwrite ( &g_pNext,  sizeof ( suint ), 1, file );
}

void
se_load ( FILE * file )
{
        assert ( !g_isInit );
        assert ( file );

        fread ( &g_isInit, sizeof ( sbool ), 1, file );
        fread ( &g_wRate,  sizeof ( suint ), 1, file );
        fread ( &g_cRate,  sizeof ( suint ), 1, file );
        fread ( &g_pRate,  sizeof ( suint ), 1, file );
        fread ( &g_wNext,  sizeof ( suint ), 1, file );
        fread ( &g_cNext,  sizeof ( suint ), 1, file );
        fread ( &g_pNext,  sizeof ( suint ), 1, file );
}

sbool
se_isInit ( void )
{
        return g_isInit;
}

suint
se_getWRate ( void )
{
        return g_wRate;
}

suint
se_getCRate ( void )
{
        return g_cRate;
}

suint
se_getPRate ( void )
{
        return g_pRate;
}

suint
se_getWNext ( void )
{
        return g_wNext;
}

suint
se_getCNext ( void )
{
        return g_cNext;
}

suint
se_getPNext ( void )
{
        return g_pNext;
}

void
se_setWRate ( suint rate )
{
        assert ( g_isInit );

        g_wRate = rate;
        g_wNext = rate;
}

void
se_setCRate ( suint rate )
{
        assert ( g_isInit );

        g_cRate = rate;
        g_cNext = rate;
}

void
se_setPRate ( suint rate )
{
        assert ( g_isInit );

        g_pRate = rate;
        g_pNext = rate;
}

static suint
rUint ( void )
{
        suint res = 0;

        res  =  rand() & 0xff;
        res |= (rand() & 0xff) << 8;
        res |= (rand() & 0xff) << 16;
        res |= (rand() & 0xff) << 24;

        return res;
}

sbyte
se_wMutate ( sbyte inst )
{
        assert ( g_isInit );

        if ( !g_wRate ) {
                g_wNext = 0;
                return inst;
        }

        if ( !g_wNext ) {
                g_wNext = rUint () % ( g_wRate * 2 );
                return rUint () % SINST_COUNT;
        } else {
                g_wNext --;
                return inst;
        }
}

static void
cosmicRay ( void )
{
        suint av;
        sbyte iv;

        assert ( g_isInit );

        av = rUint () % sm_getSize ();
        iv = rUint () % SINST_COUNT;

        sm_setInst ( av, iv );
}

void
se_cMutate ( void )
{
        assert ( g_isInit );

        if ( !g_cRate ) {
                g_cNext = 0;
                return;
        }

        if ( !g_cNext ) {
                g_cNext = rUint () % ( g_cRate * 2 );
                cosmicRay ();
        } else {
                g_cNext --;
        }
}

static void
procMutate ( void )
{
        suint   pi;
        suint   ei;
        sbool   in;
        suint * dt;

        assert ( g_isInit );

        if ( !sp_getCount ()) {
                return;
        }

        pi = rUint () % sp_getCount ();

        if ( sp_isFree ( pi )) {
                return;
        }

        ei = rUint () % ( SPELM_COUNT - PMUT_OFFSET );
        in = rUint () % 2;
        dt = sp_getPData ( pi ) + PMUT_OFFSET + ei;

        *dt += in ? 1 : -1;
}

void
se_pMutate ( void )
{
        assert ( g_isInit );

        if ( !g_pRate ) {
                g_pNext = 0;
                return;
        }

        if ( !g_pNext ) {
                g_pNext = rUint () % ( g_pRate * 2 );
                procMutate ();
        } else {
                g_pNext --;
        }
}