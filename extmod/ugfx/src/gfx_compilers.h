/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    gfx_compilers.h
 * @brief   GFX compiler support header file.
 *
 * @addtogroup GFX
 *
 * @brief	Main module to glue all the others together
 *
 * @{
 */

#ifndef _GFX_COMPILERS_H
#define _GFX_COMPILERS_H

/**
 * @name    GFX compilation options
 * @{
 */
 	/**
	 * @brief	Should various inline ugfx functions be non-inline.
	 * @details	Defaults to FALSE
	 * @note	Generally there is no need to set this to TRUE as it will have huge performance impacts
	 *			in the driver level.
	 */
	#ifndef GFX_NO_INLINE
		#define GFX_NO_INLINE			FALSE
	#endif

	// Set the no inline value
	#if GFX_NO_INLINE
		#define GFXINLINE
	#endif

	/**
	 * @brief	Enable compiler specific code
	 * @details	Auto detected by default but it can be overridden in gfxconf.h
	 * @note	This is setting enables optimisations and code options that are compiler specific.
	 * @note	If the compiler can't be auto-detected it is set to GFX_COMPILER_UNKNOWN
	 * @{
	 */
	#ifndef GFX_COMPILER
		#define GFX_COMPILER			GFX_COMPILER_UNKNOWN
	#endif
	#define GFX_COMPILER_UNKNOWN		0		//**< Unknown compiler
	#define GFX_COMPILER_ACC			1		//**< ACC Compiler
	#define GFX_COMPILER_ALTIUM			2		//**< Altium MicroBlaze C
	#define GFX_COMPILER_ALTIUMHW		3		//**< Altium C-to-Hardware
	#define GFX_COMPILER_AMSTERDAM		4		//**< Amsterdam Compiler Kit
	#define GFX_COMPILER_ARMCC			5		//**< ARM Compiler
	#define GFX_COMPILER_AZTEC			6		//**< Aztec C
	#define GFX_COMPILER_BORLAND		7		//**< Borland C++
	#define GFX_COMPILER_CC65			8		//**< CC65
	#define GFX_COMPILER_CLANG			9		//**< CLang (LLVM) compiler
	#define GFX_COMPILER_COMEAU			10		//**< Comeau C++
	#define GFX_COMPILER_COMPAQ			11		//**< Compaq C
	#define GFX_COMPILER_COMPCERT		12		//**< Compcert Compiler
	#define GFX_COMPILER_CONVEX			13		//**< Convex C
	#define GFX_COMPILER_CRAY			14		//**< Cray C/C++
	#define GFX_COMPILER_CYGWIN			15		//**< Cygwin (x86) unix emulator compiler for windows
	#define GFX_COMPILER_DAIB			16		//**< Diab C/C++
	#define GFX_COMPILER_DEC			17		//**< The older DEC C Compiler
	#define GFX_COMPILER_DICE			18		//**< DICE C Compiler
	#define GFX_COMPILER_DIGNUS			19		//**< Dignus Systems C++ Compiler
	#define GFX_COMPILER_DJGPP			20		//**< DJGPP
	#define GFX_COMPILER_DMARS			21		//**< Digital Mars
	#define GFX_COMPILER_EDG			22		//**< EDG C++
	#define GFX_COMPILER_EKOPATH		23		//**< EKOPath Compiler
	#define GFX_COMPILER_FUJITSU		24		//**< Fujitsu C++ Compiler
	#define GFX_COMPILER_GCC			25		//**< Standard GCC/G++
	#define GFX_COMPILER_GREENHILL		26		//**< Green Hill C/C++
	#define GFX_COMPILER_HIGHC			27		//**< Metaware High C/C++
	#define GFX_COMPILER_HP				28		//**< HP C/aC++
	#define GFX_COMPILER_IAR			29		//**< IAR C/C++
	#define GFX_COMPILER_IBMXL			30		//**< IBM XL C/C++ Compiler
	#define GFX_COMPILER_IMAGECRAFT		31		//**< ImageCraft C Compiler
	#define GFX_COMPILER_INTEL			32		//**< Intel ICC/ICPC Compiler
	#define GFX_COMPILER_KAI			33		//**< Kai C++
	#define GFX_COMPILER_KEIL			34		//**< Keil (use this when working with uVision IDE)
	#define GFX_COMPILER_LCC			35		//**< LCC
	#define GFX_COMPILER_METROWORKS		36		//**< Metroworks
	#define GFX_COMPILER_MICROTEC		37		//**< Microtec C/C++
	#define GFX_COMPILER_MICROWAY		38		//**< Microway NDP C
	#define GFX_COMPILER_MINGW32		39		//**< MingW32 (x86) compiler for windows
	#define GFX_COMPILER_MINGW64		40		//**< MingW64 (x64) compiler for windows
	#define GFX_COMPILER_MIPSPRO		41		//**< MIPS Pro
	#define GFX_COMPILER_MIRACLE		42		//**< Miracle C
	#define GFX_COMPILER_MPW			43		//**< MPW C++
	#define GFX_COMPILER_NORCROFT		44		//**< Norcroft ARM
	#define GFX_COMPILER_NWCC			45		//**< NWCC
	#define GFX_COMPILER_OPEN64			46		//**< Open64
	#define GFX_COMPILER_OSS			47		//**< Oracle Solaris Studio
	#define GFX_COMPILER_PACIFIC		48		//**< Pacific C
	#define GFX_COMPILER_PALM			49		//**< Palm C/C++
	#define GFX_COMPILER_PELLES			50		//**< Pelles C
	#define GFX_COMPILER_PGCC			51		//**< Portland PGCC/PGCPP
	#define GFX_COMPILER_RENESAS		52		//**< Renesas C/C++
	#define GFX_COMPILER_SASC			53		//**< SAS/C
	#define GFX_COMPILER_SCO			54		//**< SCO OpenServer
	#define GFX_COMPILER_SDCC			55		//**< Small Device C Compiler
	#define GFX_COMPILER_SN				56		//**< SN Compiler
	#define GFX_COMPILER_STRATUS		57		//**< Stratus VOS C
	#define GFX_COMPILER_SYMANTEC		58		//**< Symantec C++
	#define GFX_COMPILER_TENDRA			59		//**< TenDRA C/C++
	#define GFX_COMPILER_THINK			60		//**< Think C
	#define GFX_COMPILER_TI				61		//**< Texas Instruments C/C++
	#define GFX_COMPILER_TINYC			62		//**< Tiny C
	#define GFX_COMPILER_TURBOC			63		//**< Borland Turbo C
	#define GFX_COMPILER_ULTIMATE		64		//**< Ultimate C/C++
	#define GFX_COMPILER_USL			65		//**< USL C
	#define GFX_COMPILER_VBCC			66		//**< VBCC
	#define GFX_COMPILER_VS				67		//**< Microsoft Visual Studio
	#define GFX_COMPILER_WATCOM			68		//**< Watcom
	#define GFX_COMPILER_ZTC			69		//**< Zortech C++
	/** @} */

	/************************************ Start Compiler Auto-Detection *************************
	 *
	 * Do not alter the order of these unless you know what you are doing as some compilers try
	 *	to emulate other compilers. GCC in particular is commonly emulated and is also used as
	 *	a code base for other compiler variants.
	 */
	#if GFX_COMPILER == GFX_COMPILER_UNKNOWN
		#undef GFX_COMPILER
		#if defined(__MINGW32__)
			#define GFX_COMPILER	GFX_COMPILER_MINGW32
		#elif defined(__MINGW64__)
			#define GFX_COMPILER	GFX_COMPILER_MINGW64
		#elif defined(__CYGWIN__)
			#define GFX_COMPILER	GFX_COMPILER_CYGWIN
		#elif defined(__KEIL__) || defined(__CA__) || defined(__C51__) || defined(__CX51__) || defined(__C166__) || defined(__C251__) \
				|| (defined(__CC_ARM) && defined(__EDG__))
			#define GFX_COMPILER	GFX_COMPILER_KEIL
		#elif defined(__clang__) || defined(__llvm__)
			#define GFX_COMPILER	GFX_COMPILER_CLANG
		#elif defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__TCL)
			#define GFX_COMPILER	GFX_COMPILER_INTEL
		#elif defined(__GNUC__) || defined(__GNUG__)
			#define GFX_COMPILER	GFX_COMPILER_GCC
		#elif defined(__CC_ARM)
			#define GFX_COMPILER	GFX_COMPILER_ARMCC
		#elif defined(__HP_cc) || defined(__HP_aCC)
			#define GFX_COMPILER	GFX_COMPILER_HP
		#elif defined(__IBMC__) || defined(__IBMCPP__) || defined(__xlc__) || defined(__xlC__)
			#define GFX_COMPILER	GFX_COMPILER_IBMXL
		#elif defined(_MSC_VER)
			#define GFX_COMPILER	GFX_COMPILER_VS
		#elif defined(__PGI)
			#define GFX_COMPILER	GFX_COMPILER_PGCC
		#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
			#define GFX_COMPILER	GFX_COMPILER_OSS
		#elif defined(__TURBOC__)
			#define GFX_COMPILER	GFX_COMPILER_TURBOC
		#elif defined(__BORLANDC__)
			#define GFX_COMPILER	GFX_COMPILER_BORLAND
		#elif defined(__COMO__)
			#define GFX_COMPILER	GFX_COMPILER_COMEAU
		#elif defined(__DECC) || defined(__VAXC) || defined(VAXC) || defined(__DECCXX)
			#define GFX_COMPILER	GFX_COMPILER_COMPAQ
		#elif defined(__osf__) && defined(__LANGUAGE_C__)
			#define GFX_COMPILER	GFX_COMPILER_DEC
		#elif defined(_CRAYC)
			#define GFX_COMPILER	GFX_COMPILER_CRAY
		#elif defined(__DCC__)
			#define GFX_COMPILER	GFX_COMPILER_DAIB
		#elif defined(__DMC__)
			#define GFX_COMPILER	GFX_COMPILER_DMARS
		#elif defined(__KCC)
			#define GFX_COMPILER	GFX_COMPILER_KAI
		#elif defined(__LCC__)
			#define GFX_COMPILER	GFX_COMPILER_LCC
		#elif defined(__HIGHC__)
			#define GFX_COMPILER	GFX_COMPILER_HIGHC
		#elif defined(__MWERKS__) || defined(__CWCC__)
			#define GFX_COMPILER	GFX_COMPILER_METROWORKS
		#elif defined(__sgi)
			#define GFX_COMPILER	GFX_COMPILER_MIPSPRO
		#elif defined(__MRC__)
			#define GFX_COMPILER	GFX_COMPILER_MPW
		#elif defined(__CC_NORCROFT)
			#define GFX_COMPILER	GFX_COMPILER_NORCROFT
		#elif defined(__SASC__)
			#define GFX_COMPILER	GFX_COMPILER_SASC
		#elif defined( _SCO_DS )
			#define GFX_COMPILER	GFX_COMPILER_SCO
		#elif defined(__TINYC__)
			#define GFX_COMPILER	GFX_COMPILER_TINYC
		#elif defined( __USLC__ )
			#define GFX_COMPILER	GFX_COMPILER_USL
		#elif defined(__WATCOMC__)
			#define GFX_COMPILER	GFX_COMPILER_WATCOM
		#elif defined(__AZTEC_C__) || defined(AZTEC_C)
			#define GFX_COMPILER	GFX_COMPILER_AZTEC
		#elif defined(__CC65__)
			#define GFX_COMPILER	GFX_COMPILER_CC65
		#elif defined(__convexc__)
			#define GFX_COMPILER	GFX_COMPILER_CONVEX
		#elif defined(__COMPCERT__)
			#define GFX_COMPILER	GFX_COMPILER_COMPCERT
		#elif defined(_DICE)
			#define GFX_COMPILER	GFX_COMPILER_DICE
		#elif defined(__SYSC__)
			#define GFX_COMPILER	GFX_COMPILER_DIGNUS
		#elif defined(__DJGPP__) || defined(__GO32__)
			#define GFX_COMPILER	GFX_COMPILER_DJGPP
		#elif defined(__EDG__)
			#define GFX_COMPILER	GFX_COMPILER_EDG
		#elif defined(__PATHCC__)
			#define GFX_COMPILER	GFX_COMPILER_EKOPATH
		#elif defined(__FCC_VERSION)
			#define GFX_COMPILER	GFX_COMPILER_FUJITSU
		#elif defined(__ghs__)
			#define GFX_COMPILER	GFX_COMPILER_GREENHILL
		#elif defined(__IAR_SYSTEMS_ICC__)
			#define GFX_COMPILER	GFX_COMPILER_IAR
		#elif defined(_MRI)
			#define GFX_COMPILER	GFX_COMPILER_MICROTEC
		#elif defined(__NDPC__) || defined(__NDPX__)
			#define GFX_COMPILER	GFX_COMPILER_MICROWAY
		#elif defined(MIRACLE)
			#define GFX_COMPILER	GFX_COMPILER_MIRACLE
		#elif defined(__NWCC__)
			#define GFX_COMPILER	GFX_COMPILER_NWCC
		#elif defined(__OPEN64__)
			#define GFX_COMPILER	GFX_COMPILER_OPEN64
		#elif defined(__PACIFIC__)
			#define GFX_COMPILER	GFX_COMPILER_PACIFIC
		#elif defined(_PACC_VER)
			#define GFX_COMPILER	GFX_COMPILER_PALM
		#elif defined(__POCC__)
			#define GFX_COMPILER	GFX_COMPILER_PELLES
		#elif defined(__RENESAS__) || defined(__HITACHI__)
			#define GFX_COMPILER	GFX_COMPILER_RENESAS
		#elif defined(SDCC)
			#define GFX_COMPILER	GFX_COMPILER_SDCC
		#elif defined(__SNC__)
			#define GFX_COMPILER	GFX_COMPILER_SN
		#elif defined(__VOSC__)
			#define GFX_COMPILER	GFX_COMPILER_STRATUS
		#elif defined(__TenDRA__)
			#define GFX_COMPILER	GFX_COMPILER_TENDRA
		#elif defined(__TI_COMPILER_VERSION__)
			#define GFX_COMPILER	GFX_COMPILER_TI
		#elif defined(THINKC3) || defined(THINKC4)
			#define GFX_COMPILER	GFX_COMPILER_THINK
		#elif defined(_UCC)
			#define GFX_COMPILER	GFX_COMPILER_ULTIMATE
		#elif defined(__VBCC__)
			#define GFX_COMPILER	GFX_COMPILER_VBCC
		#elif defined(__ZTC__)
			#define GFX_COMPILER	GFX_COMPILER_ZTC
		#elif defined(__SC__)
			#define GFX_COMPILER	GFX_COMPILER_SYMANTEC
		#elif defined(_ACC_)
			#define GFX_COMPILER	GFX_COMPILER_ACC
		#elif defined(__CMB__)
			#define GFX_COMPILER	GFX_COMPILER_ALTIUM
		#elif defined(__CHC__)
			#define GFX_COMPILER	GFX_COMPILER_ALTIUMHW
		#elif defined(__ACK__)
			#define GFX_COMPILER	GFX_COMPILER_AMSTERDAM
		#elif defined(__IMAGECRAFT__)
			#define GFX_COMPILER	GFX_COMPILER_IMAGECRAFT

		#else
			#define GFX_COMPILER	GFX_COMPILER_UNKNOWN
		#endif
	#endif
	/************************************ End Compiler Auto-Detection *************************/

	/************************************ Start Compiler Settings *****************************
	 *
	 *  Please keep this list in alphabetical order to keep it easier to maintain
	 */
	#undef GFX_COMPILER_NAME
	#undef GFX_COMPILER_TESTED
	#undef GFX_COMPILER_VERSION_MAJOR
	#undef GFX_COMPILER_VERSION_MINOR
	#undef GFX_COMPILER_VERSION_PATCH
	#undef GFX_COMPILER_VERSION_BUILD
	#if GFX_COMPILER ==  GFX_COMPILER_ACC
		#define GFX_COMPILER_NAME					"ACC"
	#elif GFX_COMPILER ==  GFX_COMPILER_ALTIUM
		#define GFX_COMPILER_NAME					"Altium MicroBlaze C"
		#ifdef __BUILD__
			#define GFX_COMPILER_VERSION_MAJOR		((__BUILD__)/1000000)
			#define GFX_COMPILER_VERSION_MINOR		(((__BUILD__)/1000)%1000)
			#define GFX_COMPILER_VERSION_PATCH		((__BUILD__)%1000)
		#else
			#define GFX_COMPILER_VERSION_MAJOR		((__VERSION__)/1000)
			#define GFX_COMPILER_VERSION_MINOR		((__VERSION__)%1000)
			#define GFX_COMPILER_VERSION_PATCH		(__REVISION__)
		#endif
	#elif GFX_COMPILER ==  GFX_COMPILER_ALTIUMHW
		#define GFX_COMPILER_NAME					"Altium C-to-Hardware"
		#ifdef __BUILD__
			#define GFX_COMPILER_VERSION_MAJOR		((__BUILD__)/1000000)
			#define GFX_COMPILER_VERSION_MINOR		(((__BUILD__)/1000)%1000)
			#define GFX_COMPILER_VERSION_PATCH		((__BUILD__)%1000)
		#else
			#define GFX_COMPILER_VERSION_MAJOR		((__VERSION__)/1000)
			#define GFX_COMPILER_VERSION_MINOR		((__VERSION__)%1000)
			#define GFX_COMPILER_VERSION_PATCH		(__REVISION__)
		#endif
	#elif GFX_COMPILER ==  GFX_COMPILER_AMSTERDAM
		#define GFX_COMPILER_NAME					"Amsterdam Compiler Kit"
	#elif GFX_COMPILER == GFX_COMPILER_ARMCC
		#define GFX_COMPILER_NAME					"ARMCC"
		#define GFX_COMPILER_VERSION_MAJOR			((__ARMCC_VERSION)/100000)
		#define GFX_COMPILER_VERSION_MINOR			(((__ARMCC_VERSION)/10000)%10)
		#define GFX_COMPILER_VERSION_PATCH			(((__ARMCC_VERSION)/1000)%10)
		#define GFX_COMPILER_VERSION_BUILD			((__ARMCC_VERSION)%1000)
		#define __LITTLE_IF_NOT_BIG__				// Oops - Defines __BIG_ENDIAN but not __LITTLE_ENDIAN
	#elif GFX_COMPILER == GFX_COMPILER_AZTEC
		#define GFX_COMPILER_NAME					"Aztec"
		#define GFX_COMPILER_VERSION_MAJOR			((__VERSION)/100)
		#define GFX_COMPILER_VERSION_MINOR			((__VERSION)%100)
	#elif GFX_COMPILER == GFX_COMPILER_BORLAND
		#define GFX_COMPILER_NAME					"Borland C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__BORLANDC__)/0x100)
		#define GFX_COMPILER_VERSION_MINOR			(((((__BORLANDC__)%0x100)/0x10)*10) + ((__BORLANDC__)%0x10))
	#elif GFX_COMPILER == GFX_COMPILER_CC65
		#define GFX_COMPILER_NAME					"CC65"
		#define GFX_COMPILER_VERSION_MAJOR			((__CC65__)/0x100)
		#define GFX_COMPILER_VERSION_MINOR			(((__CC65__)/0x10)%0x10)
		#define GFX_COMPILER_VERSION_PATCH			((__CC65__)%0x10)
	#elif GFX_COMPILER == GFX_COMPILER_CLANG
		#define GFX_COMPILER_NAME					"CLang (LLVM)"
		#define GFX_COMPILER_TESTED					TRUE
		#define GFX_COMPILER_VERSION_MAJOR			(__clang_major__)
		#define GFX_COMPILER_VERSION_MINOR			(__clang_minor__)
		#define GFX_COMPILER_VERSION_PATCH			(__clang_patchlevel__)
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)));
	#elif GFX_COMPILER == GFX_COMPILER_COMEAU
		#define GFX_COMPILER_NAME					"Comeau C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__COMO_VERSION__)/100)
		#define GFX_COMPILER_VERSION_MINOR			((__COMO_VERSION__)%100)
	#elif GFX_COMPILER == GFX_COMPILER_COMPAQ
		#define GFX_COMPILER_NAME					"Compaq C"
		#define GFX_COMPILER_VERSION_MAJOR			((__DECC_VER)/10000000)
		#define GFX_COMPILER_VERSION_MINOR			(((__DECC_VER)/100000)%100)
		#define GFX_COMPILER_VERSION_PATCH			((__DECC_VER)%10000)
	#elif GFX_COMPILER == GFX_COMPILER_COMPCERT
		#define GFX_COMPILER_NAME					"Compcert"
	#elif GFX_COMPILER == GFX_COMPILER_CONVEX
		#define GFX_COMPILER_NAME					"Convex C"
	#elif GFX_COMPILER == GFX_COMPILER_CRAY
		#define GFX_COMPILER_NAME					"Cray C/C++"
		#define GFX_COMPILER_VERSION_MAJOR			(_RELEASE)
		#define GFX_COMPILER_VERSION_MINOR			(_RELEASE_MINOR)
	#elif GFX_COMPILER == GFX_COMPILER_CYGWIN
		#define GFX_COMPILER_NAME					"Cygwin"
		#define GFX_COMPILER_TESTED					TRUE
		#define GFX_COMPILER_VERSION_MAJOR			(__GNUC__)
		#define GFX_COMPILER_VERSION_MINOR			(__GNUC_MINOR__)
		#ifdef __GNUC_PATCHLEVEL__
			#define GFX_COMPILER_VERSION_PATCH		(__GNUC_PATCHLEVEL__)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
	#elif GFX_COMPILER == GFX_COMPILER_DAIB
		#define GFX_COMPILER_NAME					"Diab C/C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__VERSION_NUMBER__)/1000)
		#define GFX_COMPILER_VERSION_MINOR			(((__VERSION_NUMBER__)/100)%10)
		#define GFX_COMPILER_VERSION_PATCH			((__VERSION_NUMBER__)%100)
	#elif GFX_COMPILER == GFX_COMPILER_DEC
		#define GFX_COMPILER_NAME					"DEC"
	#elif GFX_COMPILER == GFX_COMPILER_DICE
		#define GFX_COMPILER_NAME					"DICE C"
	#elif GFX_COMPILER == GFX_COMPILER_DIGNUS
		#define GFX_COMPILER_NAME					"Dignus Systems C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__SYSC_VER__)/10000)
		#define GFX_COMPILER_VERSION_MINOR			(((__SYSC_VER__)/100)%100)
		#define GFX_COMPILER_VERSION_PATCH			((__SYSC_VER__)%100)
	#elif GFX_COMPILER == GFX_COMPILER_DJGPP
		#define GFX_COMPILER_NAME					"DJGPP"
		#ifdef __DJGPP__
			#define GFX_COMPILER_VERSION_MAJOR		(__DJGPP__)
			#define GFX_COMPILER_VERSION_MINOR		(__DJGPP_MINOR__)
		#else
			#define GFX_COMPILER_VERSION_MAJOR		(1)
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_DMARS
		#define GFX_COMPILER_NAME					"Digital Mars"
		#define GFX_COMPILER_VERSION_MAJOR			((__DMC__)/0x100)
		#define GFX_COMPILER_VERSION_MINOR			(((__DMC__)/0x10)%0x10)
		#define GFX_COMPILER_VERSION_PATCH			((__DMC__)%0x10)
	#elif GFX_COMPILER == GFX_COMPILER_EDG
		#define GFX_COMPILER_NAME					"EDG C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__EDG_VERSION__)/100)
		#define GFX_COMPILER_VERSION_MINOR			((__EDG_VERSION__)%100)
	#elif GFX_COMPILER == GFX_COMPILER_EKOPATH
		#define GFX_COMPILER_NAME					"EKOPath"
		#define GFX_COMPILER_VERSION_MAJOR			(__PATHCC__)
		#define GFX_COMPILER_VERSION_MINOR			(__PATHCC_MINOR__)
		#define GFX_COMPILER_VERSION_PATCH			(__PATHCC_PATCHLEVEL__)
	#elif GFX_COMPILER == GFX_COMPILER_FUJITSU
		#define GFX_COMPILER_NAME					"Fujitsu C++"
	#elif GFX_COMPILER == GFX_COMPILER_GCC
		#define GFX_COMPILER_NAME					"GCC"
		#define GFX_COMPILER_TESTED					TRUE
		#define GFX_COMPILER_VERSION_MAJOR			(__GNUC__)
		#define GFX_COMPILER_VERSION_MINOR			(__GNUC_MINOR__)
		#ifdef __GNUC_PATCHLEVEL__
			#define GFX_COMPILER_VERSION_PATCH		(__GNUC_PATCHLEVEL__)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
	#elif GFX_COMPILER == GFX_COMPILER_GREENHILL
		#define GFX_COMPILER_NAME					"Green Hill C/C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__GHS_VERSION_NUMBER__)/100)
		#define GFX_COMPILER_VERSION_MINOR			(((__GHS_VERSION_NUMBER__)/10)%10)
		#define GFX_COMPILER_VERSION_PATCH			((__GHS_VERSION_NUMBER__)%10)
	#elif GFX_COMPILER == GFX_COMPILER_HIGHC
		#define GFX_COMPILER_NAME					"Metaware High C/C++"
	#elif GFX_COMPILER == GFX_COMPILER_HP
		#define GFX_COMPILER_NAME					"HP C/aC++"
		#ifdef __HP_aCC
			#if __HP_aCC == 1
				#define GFX_COMPILER_VERSION_MAJOR	(1)
				#define GFX_COMPILER_VERSION_MINOR	(15)
			#else
				#define GFX_COMPILER_VERSION_MAJOR	((__HP_aCC)/10000)
				#define GFX_COMPILER_VERSION_MINOR	(((__HP_aCC)/100)%100)
				#define GFX_COMPILER_VERSION_PATCH	((__HP_aCC)%100)
			#endif
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_IAR
		#define GFX_COMPILER_NAME					"IAR C++"
		#define GFX_COMPILER_TESTED					TRUE
		#define GFX_COMPILER_VERSION_MAJOR			((__VER__)/100)
		#define GFX_COMPILER_VERSION_MINOR			((__VER__)%100)
	#elif GFX_COMPILER == GFX_COMPILER_IBMXL
		#define GFX_COMPILER_NAME					"IBM XL C/C++"
		#ifdef __xlC__
			#define GFX_COMPILER_VERSION_MAJOR		((__xlC__)/0x100)
			#define GFX_COMPILER_VERSION_MINOR		((__xlC__)%0x100)
			#define GFX_COMPILER_VERSION_PATCH		((__xlC_ver__)/0x100)
			#define GFX_COMPILER_VERSION_BUILD		((__xlC_ver__)%0x100)
		#elif defined(__COMPILER_VER__)
			#define GFX_COMPILER_VERSION_MAJOR		(((__COMPILER_VER__)/0x1000000)%0x10)
			#define GFX_COMPILER_VERSION_MINOR		(((__COMPILER_VER__)/0x10000)%0x100)
			#define GFX_COMPILER_VERSION_PATCH		((__COMPILER_VER__)/0x10000)
		#elif defined(__IBMC__)
			#define GFX_COMPILER_VERSION_MAJOR		((__IBMC__)/100)
			#define GFX_COMPILER_VERSION_MINOR		(((__IBMC__)/10)%10)
			#define GFX_COMPILER_VERSION_PATCH		((__IBMC__)%10)
		#elif defined(__IBMCPP__)
			#define GFX_COMPILER_VERSION_MAJOR		((__IBMCPP__)/100)
			#define GFX_COMPILER_VERSION_MINOR		(((__IBMCPP__)/10)%10)
			#define GFX_COMPILER_VERSION_PATCH		((__IBMCPP__)%10)
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_IMAGECRAFT
		#define GFX_COMPILER_NAME					"Imagecraft C"
	#elif GFX_COMPILER == GFX_COMPILER_INTEL
		#define GFX_COMPILER_NAME					"Intel ICC/ICPC"
		#ifdef __INTEL_COMPILER
			#define GFX_COMPILER_VERSION_MAJOR		((__INTEL_COMPILER)/100)
			#define GFX_COMPILER_VERSION_MINOR		(((__INTEL_COMPILER)/10)%10)
			#define GFX_COMPILER_VERSION_PATCH		((__INTEL_COMPILER)%10)
			#ifdef __INTEL_COMPILER_BUILD_DATE
				#define GFX_COMPILER_VERSION_BUILD	(__INTEL_COMPILER_BUILD_DATE)
			#endif
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_KAI
		#define GFX_COMPILER_NAME					"Kai C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__KCC_VERSION)/0x1000)
		#define GFX_COMPILER_VERSION_MINOR			(((__KCC_VERSION)/0x100)%0x10)
		#define GFX_COMPILER_VERSION_PATCH			((__KCC_VERSION)%0x100)
	#elif GFX_COMPILER == GFX_COMPILER_KEIL
		#define GFX_COMPILER_NAME					"Keil"
		#define GFX_COMPILER_TESTED					TRUE
		#if defined(__ARMCC_VERSION)
			#define GFX_COMPILER_VERSION_MAJOR		((__ARMCC_VERSION)/1000000)
			#define GFX_COMPILER_VERSION_MINOR		(((__ARMCC_VERSION)/10000)%100)
			#define GFX_COMPILER_VERSION_PATCH		((__ARMCC_VERSION)%10000)
			#ifdef __EDG_VERSION__
				#define GFX_COMPILER_VERSION_BUILD	(__EDG_VERSION__)
			#endif
		#elif defined(__CA__)
			#define GFX_COMPILER_VERSION_MAJOR		((__CA__)/100)
			#define GFX_COMPILER_VERSION_MINOR		((__CA__)%100)
		#elif defined(__C166__)
			#define GFX_COMPILER_VERSION_MAJOR		((__C166__)/100)
			#define GFX_COMPILER_VERSION_MINOR		((__C166__)%100)
		#elif defined(__C51__)
			#define GFX_COMPILER_VERSION_MAJOR		((__C51__)/100)
			#define GFX_COMPILER_VERSION_MINOR		((__C51__)%100)
		#elif defined(__C251__)
			#define GFX_COMPILER_VERSION_MAJOR		((__C251__)/100)
			#define GFX_COMPILER_VERSION_MINOR		((__C251__)%100)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
		#pragma anon_unions							// Allow anonymous unions
		#pragma diag_remark 1293					// Turn off warning: assignment in condition
		#pragma diag_remark 83						// Turn off warning: type qualifier specified more than once
		#pragma diag_remark 767						// Turn off warning: conversion from pointer to smaller integer
		#pragma diag_remark 188						// Turn off warning: enumerated type mixed with another type
		#ifndef GFXINLINE							// Get the Keil definition for inline
			#define GFXINLINE	__inline
		#endif
		#define __LITTLE_IF_NOT_BIG__				// Oops - Defines __BIG_ENDIAN but not __LITTLE_ENDIAN
	#elif GFX_COMPILER == GFX_COMPILER_LCC
		#define GFX_COMPILER_NAME					"LCC"
	#elif GFX_COMPILER == GFX_COMPILER_METROWORKS
		#define GFX_COMPILER_NAME					"Metroworks CodeWarrior"
		#define GFX_COMPILER_VERSION_MAJOR			((__MWERKS__)/0x1000)
		#define GFX_COMPILER_VERSION_MINOR			(((__MWERKS__)/0x100)%0x10)
		#define GFX_COMPILER_VERSION_PATCH			((__MWERKS__)%100)
	#elif GFX_COMPILER == GFX_COMPILER_MICROTEC
		#define GFX_COMPILER_NAME					"Microtec C/C++"
	#elif GFX_COMPILER == GFX_COMPILER_MICROWAY
		#define GFX_COMPILER_NAME					"Microway NDP C"
	#elif GFX_COMPILER == GFX_COMPILER_MINGW32
		#define GFX_COMPILER_NAME					"MingW32"
		#define GFX_COMPILER_TESTED					TRUE
		#define GFX_COMPILER_VERSION_MAJOR			(__GNUC__)
		#define GFX_COMPILER_VERSION_MINOR			(__GNUC_MINOR__)
		#ifdef __GNUC_PATCHLEVEL__
			#define GFX_COMPILER_VERSION_PATCH		(__GNUC_PATCHLEVEL__)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
	#elif GFX_COMPILER == GFX_COMPILER_MINGW64
		#define GFX_COMPILER_NAME					"MingW64"
		#define GFX_COMPILER_VERSION_MAJOR			(__GNUC__)
		#define GFX_COMPILER_VERSION_MINOR			(__GNUC_MINOR__)
		#ifdef __GNUC_PATCHLEVEL__
			#define GFX_COMPILER_VERSION_PATCH		(__GNUC_PATCHLEVEL__)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
	#elif GFX_COMPILER == GFX_COMPILER_MIPSPRO
		#define GFX_COMPILER_NAME					"MIPS Pro"
		#ifdef _SGI_COMPILER_VERSION
			#define GFX_COMPILER_VERSION_MAJOR		((_SGI_COMPILER_VERSION)/100)
			#define GFX_COMPILER_VERSION_MINOR		(((_SGI_COMPILER_VERSION)/10)%10)
			#define GFX_COMPILER_VERSION_PATCH		((_SGI_COMPILER_VERSION)%10)
		#else
			#define GFX_COMPILER_VERSION_MAJOR		((_COMPILER_VERSION)/100)
			#define GFX_COMPILER_VERSION_MINOR		(((_COMPILER_VERSION)/10)%10)
			#define GFX_COMPILER_VERSION_PATCH		((_COMPILER_VERSION)%10)
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_MIRACLE
		#define GFX_COMPILER_NAME					"Miracle C"
	#elif GFX_COMPILER == GFX_COMPILER_MPW
		#define GFX_COMPILER_NAME					"MPW C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__MRC__)/0x100)
		#define GFX_COMPILER_VERSION_MINOR			((__MRC__)%0x100)
	#elif GFX_COMPILER == GFX_COMPILER_NORCROFT
		#define GFX_COMPILER_NAME					"Norcroft C"
	#elif GFX_COMPILER == GFX_COMPILER_NWCC
		#define GFX_COMPILER_NAME					"NWCC"
	#elif GFX_COMPILER == GFX_COMPILER_OPEN64
		#define GFX_COMPILER_NAME					"Open64"
		#define GFX_COMPILER_VERSION_MAJOR			(__OPENCC__)
		#define GFX_COMPILER_VERSION_MINOR			(__OPENCC_MINOR__)
	#elif GFX_COMPILER == GFX_COMPILER_OSS
		#define GFX_COMPILER_NAME					"Oracle Solaris Studio"
		#ifdef __SUNPRO_C >= 0x1000
			#define GFX_COMPILER_VERSION_MAJOR		((__SUNPRO_C)/0x1000)
			#define GFX_COMPILER_VERSION_MINOR		(((((__SUNPRO_C)/0x100)%0x10)*10) + (((__SUNPRO_C)/0x10)%10))
			#define GFX_COMPILER_VERSION_PATCH		((__SUNPRO_C)%0x10)
		#else
			#define GFX_COMPILER_VERSION_MAJOR		((__SUNPRO_C)/0x100)
			#define GFX_COMPILER_VERSION_MINOR		(((__SUNPRO_C)/0x10)%0x10)
			#define GFX_COMPILER_VERSION_PATCH		((__SUNPRO_C)%0x10)
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_PACIFIC
		#define GFX_COMPILER_NAME					"Pacific C"
	#elif GFX_COMPILER == GFX_COMPILER_PALM
		#define GFX_COMPILER_NAME					"Palm C/C++"
		#define GFX_COMPILER_VERSION_MAJOR			((_PACC_VER)/0x10000000)
		#define GFX_COMPILER_VERSION_MINOR			(((_PACC_VER)/0x100000)%0x100)
		#define GFX_COMPILER_VERSION_PATCH			(((_PACC_VER)/0x1000)%0x100)
		#define GFX_COMPILER_VERSION_BUILD			((_PACC_VER)%0x1000)
	#elif GFX_COMPILER == GFX_COMPILER_PELLES
		#define GFX_COMPILER_NAME					"Pelles C"
		#define GFX_COMPILER_VERSION_MAJOR			((__POCC__)/100)
		#define GFX_COMPILER_VERSION_MINOR			((__POCC__)%100)
	#elif GFX_COMPILER == GFX_COMPILER_PGCC
		#define GFX_COMPILER_NAME					"Portland PGCC/PGCPP"
		#define GFX_COMPILER_VERSION_MAJOR			(__PGIC__)
		#define GFX_COMPILER_VERSION_MINOR			(__PGIC_MINOR__)
		#define GFX_COMPILER_VERSION_PATCH			(__PGIC_PATCHLEVEL__)
	#elif GFX_COMPILER == GFX_COMPILER_RENESAS
		#define GFX_COMPILER_NAME					"Renesas C/C++"
		#ifdef __HITACHI_VERSION__
			#define GFX_COMPILER_VERSION_MAJOR		((__HITACHI_VERSION__)/0x100)
			#define GFX_COMPILER_VERSION_MINOR		((__HITACHI_VERSION__)%0x100)
		#elif __RENESAS_VERSION__ < 0x10000
			#define GFX_COMPILER_VERSION_MAJOR		((__RENESAS_VERSION__)/0x100)
			#define GFX_COMPILER_VERSION_MINOR		((__RENESAS_VERSION__)%0x100)
		#else
			#define GFX_COMPILER_VERSION_MAJOR		((__RENESAS_VERSION__)/0x1000000)
			#define GFX_COMPILER_VERSION_MINOR		(((__RENESAS_VERSION__)/0x10000)%0x100)
			#define GFX_COMPILER_VERSION_PATCH		(((__RENESAS_VERSION__)/0x100)%0x100)
			#define GFX_COMPILER_VERSION_BUILD		((__RENESAS_VERSION__)%0x100)
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_SASC
		#define GFX_COMPILER_NAME					"SAS/C"
		#ifdef __SASC__
			#define GFX_COMPILER_VERSION_MAJOR		((__SASC__)/100)
			#define GFX_COMPILER_VERSION_MINOR		((__SASC__)%100)
		#else
			#define GFX_COMPILER_VERSION_MAJOR		(__VERSION__)
			#define GFX_COMPILER_VERSION_MINOR		(__REVISION__)
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_SCO
		#define GFX_COMPILER_NAME					"SCO OpenServer"
	#elif GFX_COMPILER == GFX_COMPILER_SDCC
		#define GFX_COMPILER_NAME					"Small Device C"
		#define GFX_COMPILER_VERSION_MAJOR			((SDCC)/100)
		#define GFX_COMPILER_VERSION_MINOR			(((SDCC)/10)%10)
		#define GFX_COMPILER_VERSION_PATCH			((SDCC)%10)
	#elif GFX_COMPILER == GFX_COMPILER_SN
		#define GFX_COMPILER_NAME					"SN"
	#elif GFX_COMPILER == GFX_COMPILER_STRATUS
		#define GFX_COMPILER_NAME					"Stratus VOS C"
		#define GFX_COMPILER_VERSION_MAJOR			(__VOSC__)
	#elif GFX_COMPILER == GFX_COMPILER_SYMANTEC
		#define GFX_COMPILER_NAME					"Symantec C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__SC__)/0x100)
		#define GFX_COMPILER_VERSION_MINOR			((__SC__)%0x100)
	#elif GFX_COMPILER == GFX_COMPILER_TENDRA
		#define GFX_COMPILER_NAME					"TenDRA C/C++"
	#elif GFX_COMPILER == GFX_COMPILER_THINK
		#define GFX_COMPILER_NAME					"Think C"
		#ifdef THINKC4
			#define GFX_COMPILER_VERSION_MAJOR		(4)
		#else
			#define GFX_COMPILER_VERSION_MAJOR		(3)
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_TI
		#define GFX_COMPILER_NAME					"Texas Instruments C/C++"
		#define GFX_COMPILER_VERSION_MAJOR			((SDCC)/1000000)
		#define GFX_COMPILER_VERSION_MINOR			(((SDCC)/1000)%1000)
		#define GFX_COMPILER_VERSION_PATCH			((SDCC)%1000)
	#elif GFX_COMPILER == GFX_COMPILER_TINYC
		#define GFX_COMPILER_NAME					"Tiny C"
		#define GFX_COMPILER_TESTED					TRUE
	#elif GFX_COMPILER == GFX_COMPILER_TURBOC
		#define GFX_COMPILER_NAME					"Borland Turbo C/C++"
		#if __TURBOC__ < 0x295 || __TURBOC__ >= 0x400
			#define GFX_COMPILER_VERSION_MAJOR		((__TURBOC__)/0x100)
			#define GFX_COMPILER_VERSION_MINOR		((__TURBOC__)%0x100)
		#elif __TURBOC__ == 0x295
			#define GFX_COMPILER_VERSION_MAJOR		(1)
			#define GFX_COMPILER_VERSION_MINOR		(0)
		#elif __TURBOC__ == 0x296
			#define GFX_COMPILER_VERSION_MAJOR		(1)
			#define GFX_COMPILER_VERSION_MINOR		(1)
		#elif __TURBOC__ == 0x297
			#define GFX_COMPILER_VERSION_MAJOR		(2)
			#define GFX_COMPILER_VERSION_MINOR		(0)
		#endif
	#elif GFX_COMPILER == GFX_COMPILER_ULTIMATE
		#define GFX_COMPILER_NAME					"Ultimate C/C++"
		#define GFX_COMPILER_VERSION_MAJOR			(_MAJOR_REV)
		#define GFX_COMPILER_VERSION_MINOR			(_MINOR_REV)
	#elif GFX_COMPILER == GFX_COMPILER_USL
		#define GFX_COMPILER_NAME					"USL C"
		#define GFX_COMPILER_VERSION_MAJOR			((__SCO_VERSION__)/100000000)
		#define GFX_COMPILER_VERSION_MINOR			(((__SCO_VERSION__)/1000000)%100)
		#define GFX_COMPILER_VERSION_BUILD			((__SCO_VERSION__)%1000000)
	#elif GFX_COMPILER == GFX_COMPILER_VBCC
		#define GFX_COMPILER_NAME					"VBCC"
	#elif GFX_COMPILER == GFX_COMPILER_VS
		#define GFX_COMPILER_NAME					"Microsoft Visual Studio"
		#ifdef _MSC_FULL_VER
			#if _MSC_FULL_VER < 100000000
				#define GFX_COMPILER_VERSION_MAJOR	((_MSC_FULL_VER)/1000000)
				#define GFX_COMPILER_VERSION_MINOR	(((_MSC_FULL_VER)/10000)%100)
				#define GFX_COMPILER_VERSION_PATCH	(((_MSC_FULL_VER)/10000)%10000)
			#else
				#define GFX_COMPILER_VERSION_MAJOR	((_MSC_FULL_VER)/10000000)
				#define GFX_COMPILER_VERSION_MINOR	(((_MSC_FULL_VER)/100000)%100)
				#define GFX_COMPILER_VERSION_PATCH	(((_MSC_FULL_VER)/100000)%10000)
			#endif
		#else
			#define GFX_COMPILER_VERSION_MAJOR		((_MSC_VER)/100)
			#define GFX_COMPILER_VERSION_MINOR		((_MSC_VER)%100)
		#endif
		#ifdef _MSC_BUILD
			#define GFX_COMPILER_VERSION_BUILD		(_MSC_BUILD)
		#endif
		#define DEPRECATED(msg)						__declspec(deprecated(msg))
	#elif GFX_COMPILER == GFX_COMPILER_WATCOM
		#define GFX_COMPILER_NAME					"Watcom C"
		#define GFX_COMPILER_VERSION_MAJOR			((__WATCOMC__)/100)
		#define GFX_COMPILER_VERSION_MINOR			((__WATCOMC__)%100)
	#elif GFX_COMPILER == GFX_COMPILER_ZTC
		#define GFX_COMPILER_NAME					"Zortech C++"
		#define GFX_COMPILER_VERSION_MAJOR			((__ZTC__)/0x100)
		#define GFX_COMPILER_VERSION_MINOR			(((__ZTC__)/0x10)%0x10)
		#define GFX_COMPILER_VERSION_PATCH			((__ZTC__)%0x10)
	#endif
	#ifndef GFX_COMPILER_TESTED
		#define GFX_COMPILER_TESTED		FALSE
	#endif
	/************************************ End Compiler Settings *************************/

	/**
	 * @brief	The name of the compiler set or auto-detected
	 * @details	Read-Only
	 * @note	If you successfully compile ugfx with an unknown compiler
	 * 			please report back to us via the ugfx forum so that we can add
	 * 			it to our list of compilers to detect.
	 */
	#ifndef GFX_COMPILER_NAME
		#define GFX_COMPILER_NAME		"Unknown"
		#warning "You are using an unknown compiler. Please report this on the ugfx forum"
	#endif

	/**
	 * @brief	This compiler is tested with ugfx
	 * @details	Read-Only
	 * @note	If you successfully compile ugfx with an un-tested compiler
	 * 			please report back to us via the ugfx forum any compile errors
	 * 			or warnings so that we can add it to our list of tested compilers.
	 * @note	Although the compiler being used may be tested, it may have been
	 * 			tested with a different compiler version. Please report any problems
	 * 			on the ugfx forum.
	 */
	#if !GFX_COMPILER_TESTED
		#warning "You are using an un-tested compiler. Please report any compile errors or warnings on the ugfx forum"
	#endif

	/**
	 * @brief	The major version number of the compiler
	 * @details	Read-Only. It will be set to 0 if it can't be automatically detected.
	 * @note	If you know how to detect a version number for a compiler and we haven't,
	 * 			please report this on the ugfx forum.
	 */
	#ifndef GFX_COMPILER_VERSION_MAJOR
		#define GFX_COMPILER_VERSION_MAJOR	0
	#endif
	/**
	 * @brief	The minor version number of the compiler
	 * @details	Read-Only. It will be set to 0 if it can't be automatically detected.
	 * @note	If you know how to detect a version number for a compiler and we haven't,
	 * 			please report this on the ugfx forum.
	 */
	#ifndef GFX_COMPILER_VERSION_MINOR
		#define GFX_COMPILER_VERSION_MINOR	0
	#endif
	/**
	 * @brief	The patch version number of the compiler
	 * @details	Read-Only. It will be set to 0 if it can't be automatically detected.
	 * @note	If you know how to detect a version number for a compiler and we haven't,
	 * 			please report this on the ugfx forum.
	 */
	#ifndef GFX_COMPILER_VERSION_PATCH
		#define GFX_COMPILER_VERSION_PATCH	0
	#endif
	/**
	 * @brief	The build number of the compiler
	 * @details	Read-Only. It will be set to 0 if it can't be automatically detected.
	 * @note	If you know how to detect a version number for a compiler and we haven't,
	 * 			please report this on the ugfx forum.
	 */
	#ifndef GFX_COMPILER_VERSION_BUILD
		#define GFX_COMPILER_VERSION_BUILD	0
	#endif

	/**
	 * @brief	Enable CPU specific code
	 * @details	Auto detected by default but it can be overridden in gfxconf.h
	 * @note	This is setting enables optimisations and code options that are CPU specific.
	 * @note	If the CPU can't be auto-detected it is set to GFX_CPU_UNKNOWN
	 * @note	Auto-detection is particularly weak currently particularly for ARM and other
	 * 			non-Intel platforms. This doesn't normally matter very much as everything is
	 * 			handled with more conservative code. Where you might want to add a manual CPU
	 * 			define is for operating systems such as RAW32 that use the generic ugfx thread code
	 * 			rather than operating system defined threading routines. For these platforms specifying
	 * 			the CPU can make a decent performance improvement.
	 * @{
	 */
	#ifndef GFX_CPU
		#define GFX_CPU					GFX_CPU_UNKNOWN
	#endif
	#define GFX_CPU_UNKNOWN				0		//**< Unknown cpu
	#define GFX_CPU_CORTEX_M0			0x01	//**< Cortex M0
	#define GFX_CPU_CORTEX_M1			0x02	//**< Cortex M1
	#define GFX_CPU_CORTEX_M2			0x03	//**< Cortex M2
	#define GFX_CPU_CORTEX_M3			0x04	//**< Cortex M3
	#define GFX_CPU_CORTEX_M4			0x05	//**< Cortex M4
	#define GFX_CPU_CORTEX_M4_FP		0x06	//**< Cortex M4 with hardware floating point
	#define GFX_CPU_CORTEX_M7			0x07	//**< Cortex M7
	#define GFX_CPU_CORTEX_M7_FP		0x08	//**< Cortex M7 with hardware floating point
	#define GFX_CPU_X86					0x10	//**< Intel x86
	#define GFX_CPU_X64					0x11	//**< Intel x64
	#define GFX_CPU_IA64				0x12	//**< Intel Itanium
	#define GFX_CPU_POWERPC32			0x20	//**< PowerPC
	#define GFX_CPU_POWERPC64			0x21	//**< PowerPC
	#define GFX_CPU_SPARC				0x22	//**< Sparc
	/** @} */

	/************************************ Start CPU Auto-Detection *****************************/
	#if GFX_CPU == GFX_CPU_UNKNOWN
		#undef GFX_CPU
		#if defined(__ia64) || defined(__itanium__) || defined(_M_IA64)
			#define GFX_CPU		GFX_CPU_IA64
		#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)
			#if defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(__64BIT__) || defined(_LP64) || defined(__LP64__)
				#define GFX_CPU		GFX_CPU_POWERPC64
			#else
				#define GFX_CPU		GFX_CPU_POWERPC32
			#endif
		#elif defined(__sparc)
			#define GFX_CPU		GFX_CPU_SPARC
		#elif defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || defined(_WIN64)
			#define GFX_CPU		GFX_CPU_X64
		#elif defined(__i386) || defined(_M_IX86) || defined(_WIN32)
			#define GFX_CPU		GFX_CPU_X86
		#else
			#define GFX_CPU		GFX_CPU_UNKNOWN
		#endif
	#endif
	/************************************ End CPU Auto-Detection *****************************/

	/************************************ Start CPU Settings *****************************/
	#if GFX_CPU == GFX_CPU_X86
		#undef GFX_CPU_NO_ALIGNMENT_FAULTS
		#define GFX_CPU_NO_ALIGNMENT_FAULTS		TRUE
		#undef GFX_CPU_ENDIAN
		#define GFX_CPU_ENDIAN					GFX_CPU_ENDIAN_LITTLE
	#elif GFX_CPU == GFX_CPU_X64
		#undef GFX_CPU_ENDIAN
		#define GFX_CPU_ENDIAN					GFX_CPU_ENDIAN_LITTLE
	#endif
	/************************************ End CPU Settings *****************************/

	/**
	 * @brief   Does this CPU automatically handle alignment faults
	 * @details	Defaults to FALSE
	 * @note	Setting this to TRUE can decrease code size and increase speed but
	 * 			it should not be turned on with a CPU that can generate
	 * 			alignment segfaults.
	 * @note	If you are unsure leave this as FALSE as that generates
	 * 			the more conservative code.
	 * @note	For some CPU's this can be auto-detected.
	 */
	#ifndef GFX_CPU_NO_ALIGNMENT_FAULTS
		#define GFX_CPU_NO_ALIGNMENT_FAULTS		FALSE
	#endif

	/**
	 * @brief	The endianness of the CPU
	 * @details	Auto detected by default but it can be overridden in gfxconf.h
	 * @note	This is setting enables optimisations that are cpu endian specific.
	 * @note	If the endianness can't be auto-detected it is set to GFX_CPU_ENDIAN_UNKNOWN
	 * 			and safe (but potentially slow) code is generated
	 * @{
	 */
	#ifndef GFX_CPU_ENDIAN
		#define GFX_CPU_ENDIAN			GFX_CPU_ENDIAN_UNKNOWN
	#endif
	#define GFX_CPU_ENDIAN_UNKNOWN		0				//**< Unknown endianness
	#define GFX_CPU_ENDIAN_LITTLE		0x03020100		//**< Little endian
	#define GFX_CPU_ENDIAN_BIG			0x00010203		//**< Big endian
	#define GFX_CPU_ENDIAN_WBDWL		0x02030001		//**< Words are big endian, DWords are little endian eg. Honeywell 316
	#define GFX_CPU_ENDIAN_WLDWB		0x01000302		//**< Words are little endian, DWords are big endian eg PDP-11
	/** @} */

	/************************************ Start Endianness Auto-Detection *****************************/
	#if GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_UNKNOWN
		#undef GFX_CPU_ENDIAN
		#if (defined(__BYTE_ORDER__)&&defined(__ORDER_BIG_ENDIAN__)&&(__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)) 			\
				|| (defined(__BYTE_ORDER)&&defined(__BIG_ENDIAN)&&(__BYTE_ORDER == __BIG_ENDIAN))						\
				|| defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN) 							\
				|| defined(__MIPSEB__) || defined(__MIPSEB) || defined(_MIPSEB) 										\
				|| defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(__ARMEB__)
			#define GFX_CPU_ENDIAN			GFX_CPU_ENDIAN_BIG
		#elif (defined(__BYTE_ORDER__)&&defined(__ORDER_LITTLE_ENDIAN__)&&(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))	\
				|| (defined(__BYTE_ORDER)&&defined(_LITTLE_ENDIAN)&&(__BYTE_ORDER == _LITTLE_ENDIAN))					\
				|| defined(__LITTLE_ENDIAN__) || defined(__LITTLE_ENDIAN) || defined(_LITTLE_ENDIAN)					\
				|| defined(__MIPSEL__) || defined(__MIPSEL) || defined(_MIPSEL)											\
				|| defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(__ARMEL__)									\
				|| defined(__LITTLE_IF_NOT_BIG__)
			#define GFX_CPU_ENDIAN			GFX_CPU_ENDIAN_LITTLE
		#else
			#define GFX_CPU_ENDIAN			GFX_CPU_ENDIAN_UNKNOWN
		#endif
	#endif
	/************************************ End Endianness Auto-Detection *****************************/

	/**
	 * @brief   Mark a function as deprecated.
	 */
	#ifndef DEPRECATED
		#define DEPRECATED(msg)
	#endif

	/**
	 * @brief   Mark a function as inline.
	 */
	#ifndef GFXINLINE
		#define GFXINLINE	inline
	#endif

/** @} */

#endif /* _GFX_COMPILERS_H */
/** @} */

