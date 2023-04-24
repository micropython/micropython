#!/usr/bin/sh

#
# psoc6/run_psoc6_tests.sh -ci
#


echo "executing $0 $* ..."

usage() {
  echo "Usage: $0 -a <run all meaningful test by directory without excluding any> -c <clean results directory and file before running tests> -d <device to be used> -f <run failing tests only> -i <run implemented tests only and exclude known failing tests> -n <run not yet implemented tests only>" 1>&2;
  exit 1;
}


while getopts "acd:fhin" o; do
  case "${o}" in
    a)
       all=1
       ;;
    c)
       cleanResultsDirectoryFirst=1
       ;;
    d)
       device=${OPTARG}
       ;;
    f)
       failing=1
       ;;
    h)
       usage
       ;;
    i)
       implemented=1
       ;;
    n)
       notYetImplemented=1
       ;;
    *)
       usage
       ;;
  esac
done


if [ -z "${all}" ]; then
  all=0
fi


if [ -z "${cleanResultsDirectoryFirst}" ]; then
  cleanResultsDirectoryFirst=0
fi


if [ -z "${device}" ]; then
  device="/dev/ttyACM0"
fi


if [ -z "${failing}" ]; then
  failing=0
fi


if [ -z "${implemented}" ]; then
  implemented=0
fi


if [ -z "${notYetImplemented}" ]; then
  notYetImplemented=0
fi


resultsFile="psoc6_test_results.log"
passResultsFile="psoc6_test_passed.log"
skipResultsFile="psoc6_test_skipped.log"
failResultsFile="psoc6_test_failed.log"


echo
echo "  device            : ${device}"
echo
echo "  results file      : ${resultsFile}"
echo "  pass results file : ${passResultsFile}"
echo "  skip results file : ${skipResultsFile}"
echo "  fail results file : ${failResultsFile}"
echo


if [ ${cleanResultsDirectoryFirst} -eq 1 ]; then

  echo
  echo "  removing results directory and file if existent ..."

  if [ -e results ]; then

    rm -rf results ${resultsFile}

  fi

  echo "  done."
  echo

fi


if [ ${all} -eq 1 ]; then

  echo "  running all tests ..."
  echo

  ./run-tests.py --target psoc6 --device ${device} \
        -d \
          basics \
          extmod \
          feature_check \
          float \
          frozen \
          import \
          inlineasm \
          micropython \
          misc \
          multi_bluetooth \
          multi_net \
          net_hosted \
          net_inet \
          perf_bench \
          psoc6 \
          stress \
          thread \
          unicode \
    | tee -a ${resultsFile}

  echo
  echo "  done."
  echo

fi


if [ ${implemented} -eq 1 ]; then

  echo "  running implemented tests ..."
  echo

  ./run-tests.py --target psoc6 --device ${device} \
          io/builtin_print_file.py \
          unix/time.py \
    | tee -a ${resultsFile}

  echo

  ./run-tests.py --target psoc6 --device ${device} \
        -d \
          basics \
          extmod \
          feature_check \
          float \
          frozen \
          import \
          inlineasm \
          micropython \
          misc \
          perf_bench \
          psoc6 \
          stress \
          unicode \
        \
        -e extmod/vfs_lfs_mtime.py \
        \
        -e feature_check/async_check.py \
        -e feature_check/bytearray.py \
        -e feature_check/byteorder.py \
        -e feature_check/complex.py \
        -e feature_check/const.py \
        -e feature_check/coverage.py \
        -e feature_check/float.py \
        -e feature_check/native_check.py \
        -e feature_check/repl_emacs_check.py \
        -e feature_check/repl_words_move_check.py \
        -e feature_check/set_check.py \
        -e feature_check/slice.py \
        -e feature_check/uio_module.py \
        \
        -e float/math_domain_special.py \
        \
        -e import/gen_context.py \
        -e import/import1a.py \
        -e import/import2a.py \
        -e import/import3a.py \
        -e import/import_file.py \
        -e import/import_long_dyn.py \
        -e import/import_override.py \
        -e import/import_pkg1.py \
        -e import/import_pkg2.py \
        -e import/import_pkg3.py \
        -e import/import_pkg4.py \
        -e import/import_pkg5.py \
        -e import/import_pkg6.py \
        -e import/import_pkg7.py \
        -e import/import_pkg8.py \
        -e import/module_dict.py \
        -e import/try_module.py \
        \
        -e micropython/import_mpy_invalid.py \
        -e micropython/import_mpy_native.py \
        -e micropython/viper_error.py \
        \
        -e perf_bench/core_import_mpy_multi.py \
        -e perf_bench/core_import_mpy_single.py \
        -e 'perf_bench/viper_call.*.py' \
        \
        -e unicode/file1.py \
        -e unicode/file2.py \
        -e unicode/file_invalid.py \
    | tee -a ${resultsFile}

  echo
  echo "  done."
  echo

fi


### not yet enabled/implemented, therefore failing
if [ ${notYetImplemented} -eq 1 ]; then

  echo "  running not yet implemented tests ..."
  echo

  ./run-tests.py --target psoc6 --device ${device} -d \
        multi_bluetooth \
        multi_net \
        net_hosted \
        net_inet \
        thread \
    | tee -a ${resultsFile}

  echo
  echo "  done."
  echo

fi


### all excluded because it is know that these tests fail. In case of folder 
#
#             - cpydiff : please refer to documentetion within tests
#             - cmdline : tests are for command line Python execution and not for embedded MPY
#             - internal_bench : used for measuring run time of certain operations. Will always fail because runtime of Python and MPY will differ.
#             - io      : tests required data files on board flash and a change in the file paths to match the flash mount point
#             - jni     : tests to be run in environment with JAVA available, ie Unix, Windows, ...

# 
### therefore have been placed in this category
###
if [ ${failing} -eq 1 ]; then

  echo "  running failing tests ..."
  echo


  # prepare execution of tests by uploading required files to on-board flash
  ../tools/mpremote/mpremote.py cp internal_bench/bench.py :/flash/bench.py


  ./run-tests.py --target psoc6 --device ${device} -d \
        cmdline \
        cpydiff \
        internal_bench \
        io \
        jni \
    | tee -a ${resultsFile}

  echo
  echo "  done."
  echo

fi


echo
echo "generating pass, skip and fail files ..."

grep -i 'pass ' ${resultsFile} > ${passResultsFile}
grep -i 'skip ' ${resultsFile} > ${skipResultsFile}
grep -i 'FAIL ' ${resultsFile} > ${failResultsFile}

echo "generating pass, skip and fail files done."


echo
echo "executing $0 $* done."
