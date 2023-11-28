#!/bin/bash
set -eu -o pipefail
export BOARD="RA4M1_CLICKER"
DT=`date +%Y%m%d%H%M`
make DEBUG=0 BOARD=${BOARD} clean 2>&1 | tee ${BOARD}_build_${DT}.log
make DEBUG=0 BOARD=${BOARD}       2>&1 | tee -a ${BOARD}_build_${DT}.log
#
export BOARD="EK_RA6M2"
DT=`date +%Y%m%d%H%M`
make DEBUG=1 BOARD=${BOARD} clean 2>&1 | tee ${BOARD}_build_${DT}.log
make DEBUG=1 BOARD=${BOARD}       2>&1 | tee -a ${BOARD}_build_${DT}.log
#
export BOARD="EK_RA4M1"
DT=`date +%Y%m%d%H%M`
make DEBUG=0 BOARD=${BOARD} clean 2>&1 | tee ${BOARD}_build_${DT}.log
make DEBUG=0 BOARD=${BOARD}       2>&1 | tee -a ${BOARD}_build_${DT}.log
#
export BOARD="EK_RA4W1"
DT=`date +%Y%m%d%H%M`
make DEBUG=0 BOARD=${BOARD} clean 2>&1 | tee ${BOARD}_build_${DT}.log
make DEBUG=0 BOARD=${BOARD}       2>&1 | tee -a ${BOARD}_build_${DT}.log
#
export BOARD="EK_RA6M1"
DT=`date +%Y%m%d%H%M`
make DEBUG=1 BOARD=${BOARD} clean 2>&1 | tee ${BOARD}_build_${DT}.log
make DEBUG=1 BOARD=${BOARD}       2>&1 | tee -a ${BOARD}_build_${DT}.log
