#ifndef SALIS_COMPILER_H
#define SALIS_COMPILER_H

sbool sc_symbValid  ( sbyte symb );
sbyte sc_symbToInst ( sbyte symb );
sbyte sc_instToSymb ( sbyte inst );

sbool sc_compile    ( const char * flnm, suint addr );

#endif