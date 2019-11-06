SOBJ=wm_w600
TARGET=wm_w600
_ENV_TARGET=""
_REVI_TARGET=""

BUILD=build
MKIMG=${BUILD}/wm_tool
LOGGER=logger

OS=`uname -o`
if [ "${OS}" = "Msys" -o "${OS}" = "Cygwin" ]; then
{
	MKIMG=tools/wm_tool.exe
	SECBOOT=`cygpath.exe -w "${WMSDK_PATH}/Bin/secboot.img"`
	LOGGER=echo
}
else
{
	gcc ./tools/wm_tool.c -lpthread -Wall -O2 -o ${BUILD}/wm_tool
	SECBOOT=${WMSDK_PATH}/Bin/secboot.img
}
fi

SHELL_DO_CMD_FLAG=1

shell_do_cmd()
{
	local retval=0
	#echo "Execute -- $@"
	[ 1 = ${SHELL_DO_CMD_FLAG} ] && {
		$@
	} || {
		$@ 0>/dev/null 1>/dev/null 2>/dev/null
	}
	retval=$?
	[ 0 != $retval ] && {
		${LOGGER} "Execute $@"
		echo -e "Execute \033[31mFAILURE\033[0m: $@"
		exit $retval
	}
}


main()
{
	./${MKIMG} -b "${BUILD}/${SOBJ}.bin" -sb "${SECBOOT}" -fc compress -it 1M -ua 90000 -ra 10100 -df -o "${BUILD}/${TARGET}"
}

usage()
{
	echo -ne "\n$0 [-e ENV | -r RVERSION | -h]\n\n"
	echo -ne "\t-e ENV   Compiler Environment, default is NOT SETTED\n"
	echo -ne "\t-h       This help information\n"
	echo -ne "\t-r REVISION    SVN-Reversion number, default is NOT SETTED\n"
	echo -ne "\n"
}

while getopts ":e:hr:" opt
do
	case ${opt} in
		e)
			_ENV_TARGET="${OPTARG}-"
			;;
		h)
			usage
			exit 0
			;;
		r)
			_REVI_TARGET="-${OPTARG}"
			;;
	esac
done

TARGET=${_ENV_TARGET}${TARGET}${_REVI_TARGET}
main
