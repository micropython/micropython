#!/usr/bin/bash

## Move to the tests directory if not already in the tests directory
cd "$(dirname "$0")"
cd ../..
tools_dir="./../tools"
tests_psoc6_dir="./ports/psoc6"


usage() {
  echo "Usage:"
  echo
  echo "sh run_psoc6_tests.sh --test-suite <suite>: [[--dev-test <dev_test>  | --dev-stub <dev_stub> ] | [--booard <board> --hil <hil>]]"
  echo "sh run_psoc6_tests.sh -t <suite>: [[-d <dev_test>  | -s <dev_stub> ] | [-b <board> -h <hil>]]"
  echo
  echo "Mandatory argument:"
  echo 
  echo "  --test-suite, -t  test suite to run"
  echo 
  echo "Available test suites: "
  echo
  echo "  ci-tests          run all continuous integration enabled tests.
                            Requires the following arguments:

                              --board, b    board name 
                              --hil, h      hardware-in-the-loop server name"
  echo
  echo "  vfs-flash         run virtual filesystem related tests on flash.
                            If followed by -x, runs advance tests too."
  echo "  vfs-sdcard        run virtual filesystem related tests on sd card.
                            If followed by -x, runs advance tests too."
  echo "  no-hw-ext         run machine modules tests not requiring extended hardware"
  echo "  hw-ext            run machine modules tests requiring extended hardware"
  echo "  i2c               run i2c tests"
  echo "  uart              run uart tests"
  echo "  spi               run spi tests"
  echo "  i2s               run i2s tests"
  echo "  bitstream         run bitstream tests"
  echo "  watchdog          run watchdog tests"
  echo "  multi-instance    run multiple board instances tests"
  echo "  help              display this help"
  echo
  echo "Available options:"
  echo
  echo "  --dev-test, -d    test device (default: /dev/ttyACM0)"
  echo "  --dev-stub, -s    stub device or second test instance (default: /dev/ttyACM1)"
  echo
}

for arg in "$@"; do
  shift
  case "$arg" in
    '--test-suite')       set -- "$@" '-t'   ;;
    '--dev-test')         set -- "$@" '-d'   ;;
    '--dev-stub')         set -- "$@" '-s'   ;;
    '--board')            set -- "$@" '-b'   ;;
    '--hil')              set -- "$@" '-h'   ;;
    *)                    set -- "$@" "$arg" ;;
  esac
done

while getopts "b:d:h:s:t:x" o; do
  case "${o}" in
    d)
       dev_test=${OPTARG}
       ;;
    s) 
       dev_stub=${OPTARG}
       ;;
    t) 
       test_suite=${OPTARG}
       ;;
    x)
       afs=1
       ;;      
    b) 
       board=${OPTARG}
       ;;
    h)
       hil=${OPTARG}
       ;;     
    *)
       usage
       exit 1
       ;;
  esac
done

if [ -z "${dev_test}" ]; then
  dev_test="/dev/ttyACM0"
fi

if [ -z "${dev_stub}" ]; then
  dev_stub="/dev/ttyACM1"
fi

if [ -z "${afs}" ]; then
  afs=0
fi

exit_result=0
update_test_result() {
  last_test_result=$1
  exit_result=$((${exit_result} | ${last_test_result}))
}

start_test_info() {
  tests_name=$1
  tests_dev=$2
  stub_dev=$3

  echo
  echo "------------------------------------------"
  echo "running tests  : ${tests_name}"
  if [ -n "${tests_dev}" ]; then
    echo "test dev       : ${tests_dev} "
  fi
  if [ -n "${stub_dev}" ]; then
    echo "stub dev       : ${stub_dev} "
  fi
  echo
}

run_tests() {
  tests_name=$1
  test_dev=$2
  tests=$3
  excluded_tests=$4
  stub_name=$5
  stub_dev=$6
  stub_script=$7

  start_test_info "${tests_name}" "${test_dev}" "${stub_dev}"

  if [ -n "${stub_name}" ]; then
    echo "executing stub : ${stub_name}"
    ${tools_dir}/mpremote/mpremote.py connect ${stub_dev} run --no-follow ${stub_script}
    echo
  fi

  test_dir_flag="-d"
  case ${tests} in *.py)  test_dir_flag="";; esac

  ./run-tests.py --target psoc6 --device ${test_dev} ${test_dir_flag} ${tests} ${excluded_tests}

  update_test_result $?
}

mpremote_vfs_large_file_tests() {
  echo 
  echo "running tests : vfs large files"
  echo
  chmod 777 ${tests_psoc6_dir}/test_scripts/fs.py

  python3 ${tests_psoc6_dir}/test_scripts/fs.py ${dev_test} 0 ${storage_device}
  if [ $? -ne 0 ]; then
    echo "FS test failed"
    exit 1
  fi

  # On device file saving tests for medium and large size takes considerable 
  # amount of time. Hence only when needed, this should be triggered.
  if [ ${afs} -eq 1 ]; then
    python3 ${tests_psoc6_dir}/test_scripts/fs.py ${dev_test} 1 ${storage_device}
    if [ $? -ne 0 ]; then
      echo "FS test failed"
      exit 1
    fi  
  fi
}

vfs_flash_tests() {
  run_tests "file system flash" ${dev_test} \
  "extmod/vfs_basic.py 
   extmod/vfs_lfs_superblock.py
   extmod/vfs_userfs.py"

  storage_device="flash"

  mpremote_vfs_large_file_tests
}

vfs_sdcard_tests() {
  run_tests "file system sdcard" ${dev_test} "${tests_psoc6_dir}/hw_ext/sdcard.py"
  
  storage_device="sd"
  # mpremote_vfs_large_file_tests
}

no_ext_hw_tests() {
  run_tests "no extended hardware" ${dev_test} "${tests_psoc6_dir}" \
  "-e ${tests_psoc6_dir}/wdt.py -e ${tests_psoc6_dir}/wdt_reset_check.py"
}

hw_ext_tests() {
  run_tests "hardware extended" ${dev_test} "${tests_psoc6_dir}/hw_ext" \
  "-e ${tests_psoc6_dir}/hw_ext/i2c.py -e ${tests_psoc6_dir}/hw_ext/sdcard.py -e ${tests_psoc6_dir}/hw_ext/uart.py"
}

i2c_tests() {
  run_tests "i2c" ${dev_test} "${tests_psoc6_dir}/hw_ext/i2c.py"
}

uart_tests() {
  run_tests "uart" ${dev_test} "${tests_psoc6_dir}/hw_ext/uart.py"
}

bitstream_tests() {
  run_tests "bitstream" ${dev_test} "${tests_psoc6_dir}/hw_ext/multi_stub/bitstream_rx.py" \
   "" "bitstream_tx" ${dev_stub} "${tests_psoc6_dir}/hw_ext/multi_stub/bitstream_tx.py"
}

spi_tests() {
  run_tests "spi" ${dev_test} "${tests_psoc6_dir}/hw_ext/multi_stub/spi_master.py" \
  "" "spi_slave" ${dev_stub} "${tests_psoc6_dir}/hw_ext/multi_stub/spi_slave.py"
}

i2s_tests() {
  run_tests "i2s" ${dev_test} "${tests_psoc6_dir}/hw_ext/multi_stub/i2s_rx.py" \
  "" "i2s_tx" ${dev_stub} "${tests_psoc6_dir}/hw_ext/multi_stub/i2s_tx.py"
}

wtd_tests() {
  start_test_info "watchdog timer"

  python3 ${tests_psoc6_dir}/test_scripts/wdt_script.py ${dev_test} 
  if [ $? -ne 0 ]; then
    echo "watchdog test failed"
    exit 1
  fi
}

multi_tests() {
  start_test_info "multiple boards instances" ${dev_test} ${dev_stub}

  multi_tests=$(find ${tests_psoc6_dir}/multi/ -type f -name "*.py")

  ./run-multitests.py -i pyb:${dev_test} -i pyb:${dev_stub} ${multi_tests} 
  
  update_test_result $?
}

run_ci_tests() {
    board=$1
    hil_name=$2 
    tools_psoc6_dir=${tools_dir}/psoc6

    echo
    echo "##########################################"
    echo "running tests  : ci-tests"
    echo "board          : ${board}"
    echo "hil            : ${hil_name}"

    devs=($(python ${tools_psoc6_dir}/get-devs.py port -b ${board} -y ${tools_psoc6_dir}/${hil_name}-devs.yml))
    devs_a=($(python ${tools_psoc6_dir}/get-devs.py port -b ${board} -y ${tools_psoc6_dir}/${hil_name}-devs.yml --hw-ext 0.5.0.a))
    devs_b=($(python ${tools_psoc6_dir}/get-devs.py port -b ${board} -y ${tools_psoc6_dir}/${hil_name}-devs.yml --hw-ext 0.5.0.b))

    dev_test=${devs[0]}
    vfs_flash_tests  

    if [ "${board}" == "CY8CPROTO-062-4343W" ]; then
      dev_test=${devs_b[0]}
      vfs_sdcard_tests
    fi

    dev_test=${devs[0]}
    no_ext_hw_tests

    dev_test=${devs_a[0]}
    hw_ext_tests

    if [ "${board}" == "CY8CPROTO-062-4343W" ]; then
      i2c_dev=${devs_b[0]} 
    else
      if [ "${board}" == "CY8CPROTO-063-BLE" ]; then
        i2c_dev=${devs_a[0]}
      fi
    fi
    dev_test=${i2c_dev} 
    i2c_tests
  
    if [ "${board}" == "CY8CPROTO-062-4343W" ]; then
      uart_dev=${devs_a[0]} 
    else
      if [ "${board}" == "CY8CPROTO-063-BLE" ]; then
        uart_dev=${devs_b[0]}
      fi
    fi
    dev_test=${uart_dev} 
    uart_tests

    dev_test=${devs_a[0]}
    dev_stub=${devs_b[0]}
    spi_tests

    dev_test=${devs_b[0]}
    dev_stub=${devs_a[0]}
    i2s_tests

    dev_test=${devs_a[0]}
    dev_stub=${devs_b[0]}
    bitstream_tests

    dev_test=${devs[0]}
    wtd_tests

    # Commented until multi-instance tests are fixed
    # dev_test=${devs[0]}
    # dev_test=${devs[1]}
    # multi_tests
    echo
    echo "##########################################"
}

case ${test_suite} in
    "ci-tests")
        run_ci_tests ${board} ${hil}
        ;;
    "vfs-flash")
        vfs_flash_tests
        ;;
   "vfs-sdcard")
        vfs_sdcard_tests
        ;;
    "no-hw-ext")
        no_ext_hw_tests
        ;;
    "hw-ext")
        hw_ext_tests
        ;;
    "i2c")
        i2c_tests
        ;;
    "spi")
        spi_tests
        ;;
    "i2s")
        i2s_tests
        ;;
    "uart")
        uart_tests
        ;;
    "bitstream")
        bitstream_tests
        ;;
    "watchdog")
        wtd_tests
        ;;
    "multi-instance")
        multi_tests
        ;;
   "help")
        usage
        ;;
   *)
        usage
        exit 1
        ;;
esac

exit ${exit_result}


#######################################################################################################################################################

######      These are currently not used. To be refactor in future. ##########
######      Currently only used as reference                       ##########



# if [ ${all} -eq 1 ]; then

#   echo "  running all tests ..."
#   echo

#   echo "  setting up a network connection ..."
#   echo
  
#   ../tools/mpremote/mpremote.py run psoc6/test_scripts/network_on.py

#   ./run-tests.py --target psoc6 --device ${device0} \
#         -d \
#           basics \
#           extmod \
#           feature_check \
#           float \
#           frozen \
#           import \
#           inlineasm \
#           micropython \
#           misc \
#           multi_bluetooth \
#           multi_net \
#           net_hosted \
#           net_inet \
#           perf_bench \
#           psoc6 \
#           stress \
#           thread \
#           unicode \
#     | tee -a ${resultsFile}

#   echo
#   echo "  done."
#   echo

# fi


# if [ ${wifi} -eq 1 ]; then

#   echo "  setting up a network connection ..."
#   echo
  
#   ../tools/mpremote/mpremote.py run psoc6/test_scripts/network_on.py


#   echo "  running wifi tests ..."
#   echo

#   ./run-tests.py --target psoc6 --device ${device0} \
#         -d \
#           net_hosted \
#           net_inet \
#         \
#         -e net_inet/ssl_cert.py \
#         -e net_inet/test_tls_nonblock.py \
#         -e net_inet/tls_text_errors.py \
#         \
#         -e net_hosted/accept_nonblock.py \
#         -e net_hosted/accept_timeout.py \
#         -e net_hosted/connect_nonblock_xfer.py \
#         -e net_hosted/ssl_getpeercert.py \
#         -e net_hosted/uasyncio_start_server.py \
#     | tee -a ${resultsFile}

#   echo
#   echo "  done."
#   echo

# fi

# if [ ${implemented} -eq 1 ]; then

#   echo "  running implemented tests ..."
#   echo

#   ./run-tests.py --target psoc6 --device ${device0} \
#           io/builtin_print_file.py \
#     | tee -a ${resultsFile}

#   echo

#   ./run-tests.py --target psoc6 --device ${device0} \
#         -d \
#           basics \
#           extmod \
#           feature_check \
#           float \
#           frozen \
#           import \
#           inlineasm \
#           micropython \
#           misc \
#           perf_bench \
#           psoc6 \
#           stress \
#           unicode \
#         \
#         -e basics/async_for \
#         -e basics/builtin_pow3_intbig.py \
#         -e basics/builtin_print.py \
#         -e basics/fun_largestate.py \
#         -e basics/list_compare.py \
#         -e basics/tuple_compare.py \
#         -e basics/unpack1.py \
#         \
#         -e extmod/re_stack_overflow.py \
#         -e extmod/socket_udp_nonblock.py \

#         \
#         -e feature_check/async_check.py \
#         -e feature_check/bytearray.py \
#         -e feature_check/byteorder.py \
#         -e feature_check/complex.py \
#         -e feature_check/const.py \
#         -e feature_check/coverage.py \
#         -e feature_check/float.py \
#         -e feature_check/io_module.py \
#         -e feature_check/native_check.py \
#         -e feature_check/repl_emacs_check.py \
#         -e feature_check/repl_words_move_check.py \
#         -e feature_check/set_check.py \
#         -e feature_check/slice.py \
#         \
#         -e float/math_domain_special.py \
#         \
#         -e import/builtin_ext.py \
#         -e import/gen_context.py \
#         -e import/import1a.py \
#         -e import/import2a.py \
#         -e import/import3a.py \
#         -e import/import_broken.py \
#         -e import/import_circular.py \
#         -e import/import_file.py \
#         -e import/import_long_dyn.py \
#         -e import/import_override.py \
#         -e import/import_pkg1.py \
#         -e import/import_pkg2.py \
#         -e import/import_pkg3.py \
#         -e import/import_pkg4.py \
#         -e import/import_pkg5.py \
#         -e import/import_pkg6.py \
#         -e import/import_pkg7.py \
#         -e import/import_pkg8.py \
#         -e import/import_pkg9.py \
#         -e import/module_dict.py \
#         -e import/try_module.py \
#         \
#         -e micropython/extreme_exc.py \
#         -e micropython/memstats.py \
#         \
#         -e perf_bench/core_import_mpy_multi.py \
#         -e perf_bench/core_import_mpy_single.py \
#         -e 'perf_bench/viper_call.*.py' \
#         \
#         -e 'stress/recursi.*.py' \
#         \
#         -e unicode/file1.py \
#         -e unicode/file2.py \
#         -e unicode/file_invalid.py \
#         \
#         -e psoc6/wdt.py \
#         -e psoc6/wdt_reset_check.py \
#     | tee -a ${resultsFile}
  
#   echo
#   echo "  done."
#   echo

# fi

# ### not yet enabled/implemented, therefore failing
# if [ ${notYetImplemented} -eq 1 ]; then

#   echo "  running not yet implemented tests ..."
#   echo

#   ./run-tests.py --target psoc6 --device ${device0} -d \
#         multi_bluetooth \
#         multi_net \
#         thread \
#     | tee -a ${resultsFile}

#   echo
#   echo "  done."
#   echo

# fi

# ### all excluded because it is know that these tests fail. In case of folder 
# #
# #             - cpydiff : please refer to documentetion within tests
# #             - cmdline : tests are for command line Python execution and not for embedded MPY
# #             - internal_bench : used for measuring run time of certain operations. Will always fail because runtime of Python and MPY will differ.
# #             - io      : tests required data files on board flash and a change in the file paths to match the flash mount point
# #             - jni     : tests to be run in environment with JAVA available, ie Unix, Windows, ...

# # 
# ### therefore have been placed in this category
# ###
# if [ ${failing} -eq 1 ]; then

#   echo "  running failing tests ..."
#   echo


#   # prepare execution of tests by uploading required files to on-board flash
#   ../tools/mpremote/mpremote.py cp internal_bench/bench.py :/flash/bench.py


#   ./run-tests.py --target psoc6 --device ${device0} -d \
#         cmdline \
#         cpydiff \
#         internal_bench \
#         io \
#         jni \
#     | tee -a ${resultsFile}

#   echo
#   echo "  done."
#   echo

# fi

#######################################################################################################################################################