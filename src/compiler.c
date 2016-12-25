#include <assert.h>
#include <salis.h>

#define SINST( NAME, SYMB ) case SYMB: return STRUE;
#define SILST( NAME, SYMB ) SINST( NAME, SYMB )

sbool
sc_symbValid ( sbyte symb )
{
        switch ( symb ) {
                SINST_LIST
        }

        return SFALSE;
}

#undef SINST
#undef SILST

#define SINST( NAME, SYMB ) case SYMB: return NAME;
#define SILST( NAME, SYMB ) SINST( NAME, SYMB )

sbyte
sc_symbToInst ( sbyte symb )
{
        assert ( sc_symbValid ( symb ));

        switch ( symb ) {
                SINST_LIST
        }

        assert ( 0 );

        return 0;
}

#undef SINST
#undef SILST

#define SINST( NAME, SYMB ) case NAME: return SYMB;
#define SILST( NAME, SYMB ) SINST( NAME, SYMB )

sbyte
sc_instToSymb ( sbyte inst )
{
        assert ( inst < SINST_COUNT );

        switch ( inst ) {
                SINST_LIST
        }

        assert ( 0 );

        return 0;
}

#undef SINST
#undef SILST

sbool
sc_compile ( const char * flnm, suint addr )
{
        FILE * fl;
        suint  sz;

        if ( !flnm )               { return SFALSE; }
        if ( !sm_isValid ( addr )) { return SFALSE; }

        fl = fopen ( flnm, "r" );
        sz = 0;

        if ( !fl ) { return SFALSE; }

        while ( 1 ) {
                suint sm = fgetc ( fl );
                sbyte in = 0;
                suint oa = 0;

                if ( !sc_symbValid ( sm )) {
                        return SFALSE;
                }

                in = sc_symbToInst ( sm );
                oa = addr + sz;

                assert ( si_isInst ( in ));

                if ( !sm_isValid ( oa )) {
                        return SFALSE;
                }

                sm_setInst ( oa, in );

                sz ++;
        }

        return STRUE;
}
