#include <assert.h>
#include <stdlib.h>
#include <salis.h>

#define MBST_MASK 0x80
#define USED_MASK 0x40
#define INST_MASK 0x3f

static sbool   g_isInit;
static suint   g_order;
static suint   g_size;
static suint   g_mbsc;
static suint   g_used;
static suint   g_cap;
static sbyte * g_data;

void
sm_init ( suint ordr )
{
        suint bs = sizeof ( *g_data );

        assert ( !g_isInit );
        assert ( !s_isInit ());
        assert ( INST_MASK == SINST_COUNT - 1 );

        g_isInit = STRUE;
        g_order  = ordr;
        g_size   = 1 << ordr;
        g_cap    = g_size / 2;
        g_data   = calloc ( g_size, bs );

        assert ( g_data );
}

void
sm_quit ( void )
{
        assert ( g_isInit );
        assert ( s_isInit ());

        free ( g_data );

        g_isInit = SFALSE;
        g_order  = 0;
        g_size   = 0;
        g_mbsc   = 0;
        g_used   = 0;
        g_cap    = 0;
        g_data   = NULL;
}

void
sm_save ( FILE * file )
{
        assert ( g_isInit );
        assert ( file );

        fwrite ( &g_isInit, sizeof ( sbool ), 1, file );
        fwrite ( &g_order,  sizeof ( suint ), 1, file );
        fwrite ( &g_size,   sizeof ( suint ), 1, file );
        fwrite ( &g_mbsc,   sizeof ( suint ), 1, file );
        fwrite ( &g_used,   sizeof ( suint ), 1, file );
        fwrite ( &g_cap,    sizeof ( suint ), 1, file );

        fwrite ( g_data, sizeof ( sbyte ), g_size, file );
}

void
sm_load ( FILE * file )
{
        assert ( !g_isInit );
        assert ( file );

        fread ( &g_isInit, sizeof ( sbool ), 1, file );
        fread ( &g_order,  sizeof ( suint ), 1, file );
        fread ( &g_size,   sizeof ( suint ), 1, file );
        fread ( &g_mbsc,   sizeof ( suint ), 1, file );
        fread ( &g_used,   sizeof ( suint ), 1, file );
        fread ( &g_cap,    sizeof ( suint ), 1, file );

        g_data = calloc ( g_size, sizeof ( *g_data ));

        assert ( g_data );

        fread ( g_data, sizeof ( sbyte ), g_size, file );
}

sbool
sm_isInit ( void )
{
        return g_isInit;
}

suint
sm_getOrder ( void )
{
        return g_order;
}

suint
sm_getSize ( void )
{
        return g_size;
}

suint
sm_getMBSCount ( void )
{
        return g_mbsc;
}

suint
sm_getUsed ( void )
{
        return g_used;
}

suint
sm_getCap ( void )
{
        return g_cap;
}

sbool
sm_overCap ( void )
{
        assert ( g_isInit );

        return g_used > g_cap;
}

sbool
sm_isValid ( suint addr )
{
        assert ( g_isInit );

        return addr < g_size;
}

sbool
sm_isMBStart ( suint addr )
{
        assert ( g_isInit );
        assert ( sm_isValid ( addr ));

        return !!( g_data [ addr ] & MBST_MASK );
}

sbool
sm_isUsed ( suint addr )
{
        assert ( g_isInit );
        assert ( sm_isValid ( addr ));

        return !!( g_data [ addr ] & USED_MASK );
}

void
sm_setMBStart ( suint addr )
{
        assert ( g_isInit );
        assert ( sm_isValid ( addr ));
        assert ( !sm_isMBStart ( addr ));

        g_data [ addr ] ^= MBST_MASK;
        g_mbsc ++;

        assert ( sm_isMBStart ( addr ));
        assert ( g_mbsc );
        assert ( g_mbsc <= g_size );
}

void
sm_unsetMBStart ( suint addr )
{
        assert ( g_isInit );
        assert ( g_mbsc );
        assert ( sm_isValid ( addr ));
        assert ( sm_isMBStart ( addr ));

        g_data [ addr ] ^= MBST_MASK;
        g_mbsc --;

        assert ( !sm_isMBStart ( addr ));
        assert ( g_mbsc <= g_size );
}

void
sm_alloc ( suint addr )
{
        assert ( g_isInit );
        assert ( sm_isValid ( addr ));
        assert ( !sm_isUsed ( addr ));

        g_data [ addr ] ^= USED_MASK;
        g_used ++;

        assert ( sm_isUsed ( addr ));
        assert ( g_used );
        assert ( g_used <= g_size );
}

void
sm_dealloc ( suint addr )
{
        assert ( g_isInit );
        assert ( g_used );
        assert ( sm_isValid ( addr ));
        assert ( sm_isUsed ( addr ));

        g_data [ addr ] ^= USED_MASK;
        g_used --;

        assert ( !sm_isUsed ( addr ));
        assert ( g_used <= g_size );
}

sbyte
sm_getInst ( suint addr )
{
        assert ( g_isInit );
        assert ( sm_isValid ( addr ));

        return g_data [ addr ] & INST_MASK;
}

void
sm_setInst ( suint addr, sbyte inst )
{
        assert ( g_isInit );
        assert ( sm_isValid ( addr ));
        assert ( si_isInst ( inst ));

        g_data [ addr ] &= ( MBST_MASK | USED_MASK );
        g_data [ addr ] |= inst;
}
