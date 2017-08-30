#!/bin/sh
#
# This script generates statistics (build size, speed) for successive
# revisions of the code.  It checks out git commits one an a time, compiles
# various ports to determine their size, and runs pystone on the unix port.
# Results are collected in the output file.
#
# Note: you will need to copy this file out of the tools directory before
# executing because it does not exist in old revisions of the repository.

# check that we are in the root directory of the repository
if [ ! -d py -o ! -d unix -o ! -d stmhal ]; then
    echo "script must be run from root of the repository"
    exit 1
fi

# output file for the data; data is appended if file already exists
output=codestats.dat

# utility programs
RM=/bin/rm
AWK=awk
MAKE="make -j2"

# these are the binaries that are built; some have 2 or 3 depending on version
bin_unix=unix/micropython
bin_stmhal=stmhal/build-PYBV10/firmware.elf
bin_barearm_1=bare-arm/build/flash.elf
bin_barearm_2=bare-arm/build/firmware.elf
bin_minimal=minimal/build/firmware.elf
bin_cc3200_1=cc3200/build/LAUNCHXL/application.axf
bin_cc3200_2=cc3200/build/LAUNCHXL/release/application.axf
bin_cc3200_3=cc3200/build/WIPY/release/application.axf

# start at zero size; if build fails reuse previous valid size
size_unix="0"
size_stmhal="0"
size_barearm="0"
size_minimal="0"
size_cc3200="0"

# start at zero pystones
pystones="0"

# this code runs pystone and averages the results
pystoneavg=/tmp/pystoneavg.py
cat > $pystoneavg << EOF
import pystone
samples = [pystone.pystones(300000)[1] for i in range(5)]
samples.sort()
stones = sum(samples[1:-1]) / (len(samples) - 2) # exclude smallest and largest
print("stones %g" % stones)
EOF

function get_size() {
    if [ -r $2 ]; then
        size $2 | tail -n1 | $AWK '{print $1}'
    else
        echo $1
    fi
}

function get_size2() {
    if [ -r $2 ]; then
        size $2 | tail -n1 | $AWK '{print $1}'
    elif [ -r $3 ]; then
        size $3 | tail -n1 | $AWK '{print $1}'
    else
        echo $1
    fi
}

function get_size3() {
    if [ -r $2 ]; then
        size $2 | tail -n1 | $AWK '{print $1}'
    elif [ -r $3 ]; then
        size $3 | tail -n1 | $AWK '{print $1}'
    elif [ -r $4 ]; then
        size $4 | tail -n1 | $AWK '{print $1}'
    else
        echo $1
    fi
}

# get the last revision in the data file; or start at v1.0 if no file
if [ -r $output ]; then
    last_rev=$(tail -n1 $output | $AWK '{print $1}')
else
    echo "# hash size_unix size_stmhal size_barearm size_minimal size_cc3200 pystones" > $output
    last_rev="v1.0"
fi

# get a list of hashes between last revision (exclusive) and master
hashes=$(git log --format=format:"%H" --reverse ${last_rev}..master)
#hashes=$(git log --format=format:"%H" --reverse ${last_rev}..master | $AWK '{if (NR % 10 == 0) print $0}') # do every 10th one

for hash in $hashes; do

    #### checkout the revision ####

    git checkout $hash
    if [ $? -ne 0 ]; then
        echo "aborting"
        exit 1
    fi

    #### apply patches to get it to build ####

    if grep -q '#if defined(MP_CLOCKS_PER_SEC) && (MP_CLOCKS_PER_SEC == 1000000) // POSIX' unix/modtime.c; then
        echo apply patch
        git apply - << EOF
diff --git a/unix/modtime.c b/unix/modtime.c
index 77d2945..dae0644 100644
--- a/unix/modtime.c
+++ b/unix/modtime.c
@@ -55,10 +55,8 @@ void msec_sleep_tv(struct timeval *tv) {
 #define MP_CLOCKS_PER_SEC CLOCKS_PER_SEC
 #endif
 
-#if defined(MP_CLOCKS_PER_SEC) && (MP_CLOCKS_PER_SEC == 1000000) // POSIX
-#define CLOCK_DIV 1000.0
-#elif defined(MP_CLOCKS_PER_SEC) && (MP_CLOCKS_PER_SEC == 1000) // WIN32
-#define CLOCK_DIV 1.0
+#if defined(MP_CLOCKS_PER_SEC)
+#define CLOCK_DIV (MP_CLOCKS_PER_SEC / 1000.0F)
 #else
 #error Unsupported clock() implementation
 #endif
EOF
    fi

    #### unix ####

    $RM $bin_unix
    $MAKE -C unix CFLAGS_EXTRA=-DNDEBUG
    size_unix=$(get_size $size_unix $bin_unix)

    # undo patch if it was applied
    git checkout unix/modtime.c

    #### stmhal ####

    $RM $bin_stmhal
    $MAKE -C stmhal board=PYBV10
    size_stmhal=$(get_size $size_stmhal $bin_stmhal)

    #### bare-arm ####

    $RM $bin_barearm_1 $bin_barearm_2
    $MAKE -C bare-arm
    size_barearm=$(get_size2 $size_barearm $bin_barearm_1 $bin_barearm_2)

    #### minimal ####

    if [ -r minimal/Makefile ]; then
        $RM $bin_minimal
        $MAKE -C minimal CROSS=1
        size_minimal=$(get_size $size_minimal $bin_minimal)
    fi

    #### cc3200 ####

    if [ -r cc3200/Makefile ]; then
        $RM $bin_cc3200_1 $bin_cc3200_2 $bin_cc3200_3
        $MAKE -C cc3200 BTARGET=application
        size_cc3200=$(get_size3 $size_cc3200 $bin_cc3200_1 $bin_cc3200_2 $bin_cc3200_3)
    fi

    #### run pystone ####

    if [ -x $bin_unix ]; then
        new_pystones=$($bin_unix $pystoneavg)
        # only update the variable if pystone executed successfully
        if echo $new_pystones | grep -q "^stones"; then
            pystones=$(echo $new_pystones | $AWK '{print $2}')
        fi
    fi

    #### output data for this commit ####

    echo "$hash $size_unix $size_stmhal $size_barearm $size_minimal $size_cc3200 $pystones" >> $output

done

# checkout master and cleanup
git checkout master
$RM $pystoneavg
