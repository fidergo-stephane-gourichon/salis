#include <assert.h>
#include <salis.h>

sbool
si_isInst ( sbyte inst )
{
        return inst < SINST_COUNT;
}

static sbool
between ( sbyte inst, sbyte lo, sbyte hi )
{
        assert ( si_isInst ( inst ));
        assert ( lo < SINST_COUNT );
        assert ( hi < SINST_COUNT );

        if ( inst < lo ) { return SFALSE; }
        if ( inst > hi ) { return SFALSE; }

        return STRUE;
}

sbool
si_isMod ( sbyte inst )
{
        assert ( si_isInst ( inst ));

        return between ( inst, SNOP1, SNOP6 );
}

sbool
si_isKey ( sbyte inst )
{
        assert ( si_isInst ( inst ));

        return between ( inst, SKEYA, SKEYP );
}

sbool
si_isLock ( sbyte inst )
{
        assert ( si_isInst ( inst ));

        return between ( inst, SLOKA, SLOKP );
}

sbool
si_klMatch ( sbyte key, sbyte lock )
{
        sbyte ki;
        sbyte li;

        assert ( si_isKey ( key ));
        assert ( si_isInst ( lock ));

        ki = key - SKEYA;
        li = lock - SLOKA;

        return ki == li;
}