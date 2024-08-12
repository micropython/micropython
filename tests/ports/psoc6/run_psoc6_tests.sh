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
  echo "  adc               run adc tests"
  echo "  pin               run pin tests"
  echo "  signal            run signal tests"
  echo "  pwm               run pwm tests"
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

  exit_code=$?

  if [ ${exit_code} -ne 0 ]; then
    ./run-tests.py --print-failures
    ./run-tests.py --clean-failures
  fi
  
  update_test_result ${exit_code}
}

mpremote_vfs_large_file_tests() {
  echo 
  echo "running tests : vfs large files"
  echo
  chmod 777 ${tests_psoc6_dir}/mp_custom/fs.py

  # On device file saving tests for medium and large size takes considerable 
  # amount of time. Hence only when needed, this should be triggered.
  enable_adv_tests="basic"
  if [ ${afs} -eq 1 ]; then
     enable_adv_tests="adv"
  fi

  python3 ${tests_psoc6_dir}/mp_custom/fs.py ${dev_test} ${enable_adv_tests} ${storage_device}
  
  update_test_result $?
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
  run_tests "file system sdcard" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/single/sdcard.py" 
  
  storage_device="sd"
  mpremote_vfs_large_file_tests
}

no_ext_hw_tests() {
  run_tests "no extended hardware" ${dev_test} "${tests_psoc6_dir}/board_only_hw/single" \
  "-e ${tests_psoc6_dir}/board_only_hw/single/wdt.py \
   -e ${tests_psoc6_dir}/board_only_hw/single/wdt_reset_check.py"
}

adc_tests() {
  run_tests "adc" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/single/adc.py"
}

pwm_tests() {
  run_tests "pwm" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/single/pwm.py"
}

pin_tests() {
  run_tests "pin" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/single/pin.py"
}

signal_tests() {
  run_tests "signal" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/single/signal.py"
}

i2c_tests() {
  run_tests "i2c" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/single/i2c.py"
}

uart_tests() {
  run_tests "uart" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/single/uart.py"
}

bitstream_tests() {
  run_tests "bitstream" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/multi/bitstream_rx.py" \
   "" "bitstream_tx" ${dev_stub} "${tests_psoc6_dir}/board_ext_hw/multi/bitstream_tx.py"
}

spi_tests() {
  run_tests "spi" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/multi/spi_master.py" \
  "" "spi_slave" ${dev_stub} "${tests_psoc6_dir}/board_ext_hw/multi/spi_slave.py"
}

i2s_tests() {
  run_tests "i2s" ${dev_test} "${tests_psoc6_dir}/board_ext_hw/multi/i2s_rx.py" \
  "" "i2s_tx" ${dev_stub} "${tests_psoc6_dir}/board_ext_hw/multi/i2s_tx.py"
}

wdt_tests() {
  run_tests "wdt" ${dev_test} "${tests_psoc6_dir}/board_only_hw/single/wdt.py"
  sleep 2
  run_tests "wdt reset check" ${dev_test} "${tests_psoc6_dir}/board_only_hw/single/wdt_reset_check.py"
}

multi_tests() {
  start_test_info "multiple boards instances" ${dev_test} ${dev_stub}

  multi_tests=$(find ${tests_psoc6_dir}/board_only_hw/multi/ -type f -name "*.py")

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

    # TODO: This mess needs to be solved in a future script rework using yml files to define the compatible boards requirements
    board_version=0.5.0
    if [ "${board}" == "CY8CKIT-062S2-AI" ]; then
      board_version=0.1.0
    fi

    devs_a=($(python ${tools_psoc6_dir}/get-devs.py port -b ${board} -y ${tools_psoc6_dir}/${hil_name}-devs.yml --hw-ext ${board_version}.a))
    devs_b=($(python ${tools_psoc6_dir}/get-devs.py port -b ${board} -y ${tools_psoc6_dir}/${hil_name}-devs.yml --hw-ext ${board_version}.b))
    devs_c=($(python ${tools_psoc6_dir}/get-devs.py port -b ${board} -y ${tools_psoc6_dir}/${hil_name}-devs.yml --hw-ext ${board_version}.c))

    dev_test=${devs[0]}
    vfs_flash_tests  

    if [ "${board}" == "CY8CPROTO-062-4343W" ]; then
      dev_test=${devs_b[0]}
      vfs_sdcard_tests
    fi

    dev_test=${devs[0]}
    no_ext_hw_tests

    dev_test=${devs_a[0]}
    pin_tests
    signal_tests

    dev_test=${devs_a[0]}
    pwm_tests

    if [ "${board}" == "CY8CPROTO-062-4343W" ] || [ "${board}" == "CY8CPROTO-063-BLE" ]; then
      dev_test=${devs_a[0]} 
    else
      if [ "${board}" == "CY8CKIT-062S2-AI" ]; then
        dev_test=${devs_b[0]}
      fi
    fi
    adc_tests

    if [ "${board}" == "CY8CPROTO-062-4343W" ]; then
      dev_test=${devs_b[0]} 
    else
      if [ "${board}" == "CY8CPROTO-063-BLE" ] || [ "${board}" == "CY8CKIT-062S2-AI" ]; then
        dev_test=${devs_a[0]}
      fi
    fi
    i2c_tests
  
    if [ "${board}" == "CY8CPROTO-062-4343W" ] || [ "${board}" == "CY8CKIT-062S2-AI" ]; then
      dev_test=${devs_a[0]} 
    else
      if [ "${board}" == "CY8CPROTO-063-BLE" ]; then
        dev_test=${devs_b[0]}
      fi
    fi
    uart_tests

    if [ "${board}" == "CY8CPROTO-062-4343W" ] || [ "${board}" == "CY8CPROTO-063-BLE" ]; then
      dev_test=${devs_a[0]}
      dev_stub=${devs_b[0]}
      # else
      #   if [ "${board}" == "CY8CKIT-062S2-AI" ]; then
      #     dev_test=${devs_c[0]}
      #     dev_stub=${devs_b[0]}
      #   fi
      spi_tests
    fi

    if [ "${board}" == "CY8CPROTO-062-4343W" ] || [ "${board}" == "CY8CPROTO-063-BLE" ]; then
      dev_test=${devs_b[0]}
      dev_stub=${devs_a[0]}
    else
      if [ "${board}" == "CY8CKIT-062S2-AI" ]; then
        dev_test=${devs_c[0]}
        dev_stub=${devs_b[0]}
      fi
    fi
    i2s_tests
    
    # TODO: Bitstream not working for CY8CPROTO-062-4343W. Fix needed.
    # if [ "${board}" == "CY8CPROTO-062-4343W" ] || [ "${board}" == "CY8CPROTO-063-BLE" ]; then
    if [ "${board}" == "CY8CPROTO-063-BLE" ]; then
      dev_test=${devs_a[0]}
      dev_stub=${devs_b[0]}
      bitstream_tests
    # else
    #   if [ "${board}" == "CY8CKIT-062S2-AI" ]; then
    #     dev_test=${devs_c[0]}
    #     dev_stub=${devs_b[0]}
    #   fi
    fi
    # bitstream_tests

    dev_test=${devs[0]}
    wdt_tests

    dev_test=${devs[0]}
    dev_stub=${devs[1]}
    multi_tests

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
    "pin")
        pin_tests
        ;;
    "signal")
        signal_tests
      ;;
    "adc")
        adc_tests
        ;;
    "pwm")
        pwm_tests
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
        wdt_tests
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
