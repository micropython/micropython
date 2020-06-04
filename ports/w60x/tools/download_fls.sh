#configure the serial port number to be operated, ex:
#SERIAL_NAME=COM3
#SERIAL_NAME=ttyUSB3
#SERIAL_NAME=tty.usbserial3
#you can use "wm_tool -l" to view the locally available serial port number.
SERIAL_NAME=ttyUSB0

TARGET=wm_w600
_ENV_TARGET=""
_REVI_TARGET=""

BUILD=build
DLIMG=${BUILD}/wm_tool
LOGGER=logger

OS=`uname -o`
if [ "${OS}" = "Msys" -o "${OS}" = "Cygwin" ]; then
{
	DLIMG=tools/wm_tool.exe
	LOGGER=echo
}
else
{
	gcc ./tools/wm_tool.c -lpthread -Wall -O2 -o ${BUILD}/wm_tool
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
    if [ "$SERIAL_NAME" = "" ]; then
    {
        echo -e "\033[31munable to download because no serial port is configured.\033[0m"
        echo -e "please open the \033[35mdownload_img.sh\033[0m file (e.g: \033[32mvim ./tools/download_fls.sh\033[0m),"
        echo -e "and configure \033[36mSERIAL_NAME\033[0m to the serial port number you are using."
		exit $retval
    }
    fi

#	./${DLIMG} -c ${SERIAL_NAME} -ds 2M -dl "${BUILD}/${TARGET}.fls" -eo all -ws 115200 -rs at
#	./${DLIMG} -c ${SERIAL_NAME} -ds 2M -dl "${BUILD}/${TARGET}.fls" -eo all -rs rts
	./${DLIMG} -c ${SERIAL_NAME} -ds 2M -dl "${BUILD}/${TARGET}.fls" -eo secboot -rs rts
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
