#include <assert.h>
#include <salis.h>

#define MLINE_SZ 64
#define PLINE_SZ  6

sbool
sl_load ( const char * flnm )
{
        FILE * fl;

        assert ( flnm );

        fl = fopen ( flnm, "rb" );

        if ( !fl ) {
                return SFALSE;
        }

        s_load  ( fl );
        se_load ( fl );
        sm_load ( fl );
        sp_load ( fl );

        fclose ( fl );

        return STRUE;
}

sbool
sl_save ( const char * flnm )
{
        FILE * fl;

        assert ( flnm );

        fl = fopen ( flnm, "wb" );

        if ( !fl ) {
                return SFALSE;
        }

        s_save  ( fl );
        se_save ( fl );
        sm_save ( fl );
        sp_save ( fl );

        fclose ( fl );

        return STRUE;
}