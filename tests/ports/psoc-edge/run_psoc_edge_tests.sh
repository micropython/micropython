#!/usr/bin/bash

## Move to the tests directory if not already in the tests directory
cd "$(dirname "$0")"
cd ../..
tools_dir="./../tools"
tests_psoc_edge_dir="./ports/psoc-edge"


usage() {
  echo "Usage:"
  echo
  echo "sh run_psoc_edge_tests.sh --test-suite <suite> [[--dev-test <dev_test>  | --dev-stub <dev_stub> ] | [--booard <board> --hil <hil>]]"
  echo "sh run_psoc_edge_tests.sh -t <suite> [[-d <dev_test>  | -s <dev_stub> ] | [-b <board> -h <hil>]]"
  echo
  echo "Mandatory argument:"
  echo 
  echo "  --test-suite, -t  test suite to run"
  echo 
  echo "Available test suites: "
  echo
  echo "  ci-tests          run all continuous integration enabled tests
                            Requires --board and --hil options."
  echo
  echo "  Test suites by hardware configuration:"
  echo "  All the tests in this list require --board and --hil options."  
  echo
  echo "    vfs               run all virtual file system tests"
  echo "    no-ext-hw-single  run all tests which only require a board without external hardware"
  echo "    no-ext-hw-multi   run all tests which require multiple boards without external hardware"
  echo "    ext-hw-single     run all tests which require a single board with external hardware"
  echo "    ext-hw-multi      run all tests which require multiple boards with external hardware"
  echo
  echo "  Test suites as per peripheral:"
  echo
  echo "    time              run time test"
  echo
  echo "Available options:"
  echo 
  echo "Locally, you can specify the test device and stub devices by its port:"
  echo
  echo "  --dev-test, -d    test device (default: /dev/ttyACM0)"
  echo "  --dev-stub, -s    stub device or second test instance (default: /dev/ttyACM1)"
  echo 
  echo "Alternatively, a hardware-in-the-loop (HIL) file and the required board can be provided:"
  echo
  echo "  --board, b        board name"
  echo "  --hil, h          hardware-in-the-loop server name"
  echo
  echo "  --not-fail-fast   continue running tests even if one of them fails"
}

for arg in "$@"; do
  shift
  case "$arg" in
    '--test-suite')       set -- "$@" '-t'   ;;
    '--dev-test')         set -- "$@" '-d'   ;;
    '--dev-stub')         set -- "$@" '-s'   ;;
    '--board')            set -- "$@" '-b'   ;;
    '--hil')              set -- "$@" '-h'   ;;
    '--not-fail-fast')    set -- "$@" '-x'   ;;
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
    x)
       fail_fast=0
       ;;
    *)
       usage
       exit 1
       ;;
  esac
done

if [ -z "${afs}" ]; then
  afs=0
fi

if [ -z "${fail_fast}" ]; then
  fail_fast=1
fi

if [ -n "${board}" ] && [ -n "${hil}" ]; then
    # If the hil and board are provided, the script will use the get-devs.py discover automatically the
    # available devices.
    use_hil=1
    tools_psoc_edge_dir=${tools_dir}/psoc-edge

    echo
    echo "##########################################"
    echo "board          : ${board}"
    echo "hil            : ${hil}"

    devs=($(python ${tools_psoc_edge_dir}/get-devs.py port -b ${board} -y ${tools_psoc_edge_dir}/${hil}-devs.yml))

    if [ "${board}" == "KIT_PSE84_AI" ]; then
      board_version=0.1.0
    fi

    devs_a=($(python ${tools_psoc_edge_dir}/get-devs.py port -b ${board} -y ${tools_psoc_edge_dir}/${hil}-devs.yml --hw-ext ${board_version}.a))
    devs_b=($(python ${tools_psoc_edge_dir}/get-devs.py port -b ${board} -y ${tools_psoc_edge_dir}/${hil}-devs.yml --hw-ext ${board_version}.b))
    devs_c=($(python ${tools_psoc_edge_dir}/get-devs.py port -b ${board} -y ${tools_psoc_edge_dir}/${hil}-devs.yml --hw-ext ${board_version}.c))

    echo
    echo "##########################################"
else
    # Otherwise, we will use the provided devices or the default ones.
    use_hil=0
    if [ -z "${dev_test}" ]; then
      dev_test="/dev/ttyACM0"
    fi
    if [ -z "${dev_stub}" ]; then
      dev_stub="/dev/ttyACM1"
    fi
fi

exit_result=0
update_test_result() {
  last_test_result=$1
  exit_result=$((${exit_result} | ${last_test_result}))
  
  if [ ${exit_result} -ne 0 ]; then
    if [ ${fail_fast} -eq 1 ]; then
      echo "Test failed, exiting..."
      exit ${last_test_result}
    fi
  fi
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

  ./run-tests.py -t port:${test_dev} ${test_dir_flag} ${tests} ${excluded_tests}

  exit_code=$?

  if [ ${exit_code} -ne 0 ]; then
    ./run-tests.py --print-failures
    ./run-tests.py --clean-failures
  fi
  
  update_test_result ${exit_code}
}



time_tests() {
  if [ ${use_hil} -eq 1 ]; then
    dev_test=${devs[0]}
  fi

  run_tests "time" ${dev_test} "${tests_psoc_edge_dir}/board_only_hw/single/time.py"
}

run_ci_tests() {
    time_tests
}

no_ext_hw_single_tests() {
    time_tests
}

case ${test_suite} in
    "ci-tests")
        run_ci_tests
        ;;
    "time")
        time_tests
        ;;
    "no-ext-hw-single")
        no_ext_hw_single_tests
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
