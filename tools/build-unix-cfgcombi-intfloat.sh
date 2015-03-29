#!/bin/bash
# Builds unix port of Micro Python with all combinations of the following configurations:
#   32-bit and 64-bit (3, 6)
#   none, longlong, and multi-precision long integer type (n, l, m)
#   single-precision and double-precision floating-point type (f, d)
#
# For each combination, a binary is produced called 'micropython-XXX'
# where 'XXX' is the configuration used (using characters listed between brackets above).

if [[ ! -r modtermios.c ]]; then
	echo "Script must be executed from unix port directory"
	exit 1
fi

function do_build() {
	echo "Building $3-bit: $1 , $2 ($4)"
	export CFLAGS_EXTRA="-m$3 -DMICROPY_LONGINT_IMPL=$1 -DMICROPY_FLOAT_IMPL=$2"
	export LDFLAGS_EXTRA=-m$3
	make -W mpconfigport.h PROG=micropython-$4 all
}

LIN_IDENT=(n l m)
LIN_TYPES=(MICROPY_LONGINT_IMPL_NONE MICROPY_LONGINT_IMPL_LONGLONG MICROPY_LONGINT_IMPL_MPZ)
FLT_IDENT=(f d)
FLT_TYPES=(MICROPY_FLOAT_IMPL_FLOAT MICROPY_FLOAT_IMPL_DOUBLE)

make clean
for (( LIN=0; LIN<${#LIN_TYPES[*]}; LIN++ )); do
	for (( FLT=0; FLT<${#FLT_TYPES[*]}; FLT++ )); do
		LIN_TY=${LIN_TYPES[$LIN]}
		LIN_ID=${LIN_IDENT[$LIN]}
		FLT_TY=${FLT_TYPES[$FLT]}
		FLT_ID=${FLT_IDENT[$FLT]}
		do_build $LIN_TY $FLT_TY 32 3${LIN_ID}${FLT_ID} || exit
	done
done

make clean
for (( LIN=0; LIN<${#LIN_TYPES[*]}; LIN++ )); do
	for (( FLT=0; FLT<${#FLT_TYPES[*]}; FLT++ )); do
		LIN_TY=${LIN_TYPES[$LIN]}
		LIN_ID=${LIN_IDENT[$LIN]}
		FLT_TY=${FLT_TYPES[$FLT]}
		FLT_ID=${FLT_IDENT[$FLT]}
		do_build $LIN_TY $FLT_TY 64 6${LIN_ID}${FLT_ID} || exit
	done
done
