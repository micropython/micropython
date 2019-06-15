#!/bin/sh  

#cp W600.bin ../../Bin/WM_VENUS.bin
#cd ../../Tools
#
#cp ../Bin/version.txt ../Bin/version_bk.txt
#
#./makeimg "../Bin/WM_VENUS.bin" "../Bin/WM_VENUS.img" 0 0 "../Bin/version.txt" E000
#
#./makeimg "../Bin/WM_VENUS.bin" "../Bin/WM_VENUS_SEC.img" 0 0 "../Bin/version.txt" 7E800
#./makeimg_all "../Bin/secboot.img" "../Bin/WM_VENUS.img" "../Bin/WM_VENUS.FLS"
#rm -f "../Bin/WM_VENUS.img"

SOBJ=W600
TARGET=W600
_ENV_TARGET=""
_REVI_TARGET=""

BUILD=build
MKIMG=${BUILD}/makeimg
MKIMG_ALL=${BUILD}/makeimg_all
LOGGER=logger

OS=`uname -o`
[ "${OS}" = "Msys" -o "${OS}" = "Cygwin" ] && {
	MKIMG=${BUILD}/makeimg.exe
	MKIMG_ALL=${BUILD}/makeimg_all.exe
	LOGGER=echo
}

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
    cc tools/makeimg.c -o ${MKIMG}
    cc tools/makeimg_all.c -o ${MKIMG_ALL}

	shell_do_cmd cp tools/version.txt ${BUILD}/version.txt
    shell_do_cmd chmod 661 ${BUILD}/version.txt
	cp ${BUILD}/${SOBJ}.bin ${BUILD}/${SOBJ}.bin.bk
	shell_do_cmd gzip -fv "${BUILD}/${SOBJ}.bin"
	mv ${BUILD}/${SOBJ}.bin.bk ${BUILD}/${SOBJ}.bin
	shell_do_cmd ./${MKIMG} "${BUILD}/${SOBJ}.bin" "${BUILD}/${TARGET}.img" 0 0 "${BUILD}/version.txt" 90000 10100
	shell_do_cmd ./${MKIMG} "${BUILD}/${SOBJ}.bin.gz" "${BUILD}/${TARGET}_GZ.img" 0 1 "${BUILD}/version.txt" 90000 10100 "${BUILD}/${SOBJ}.bin" 
	shell_do_cmd ./${MKIMG} "${BUILD}/${SOBJ}.bin" "${BUILD}/${TARGET}_SEC.img" 0 0 "${BUILD}/version.txt" 90000 10100
	./${MKIMG_ALL} "${WMSDK_PATH}/Bin/secboot.img" "${BUILD}/${TARGET}.img" "${BUILD}/${TARGET}.FLS"
	#rm -rf "../Bin/${TARGET}.img"
	#rm -rf "../Bin/${SOBJ}.bin.gz"
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
