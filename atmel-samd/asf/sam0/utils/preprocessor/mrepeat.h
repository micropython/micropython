/**
 * \file
 *
 * \brief Preprocessor macro repeating utils.
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _MREPEAT_H_
#define _MREPEAT_H_

/**
 * \defgroup group_sam0_utils_mrepeat Preprocessor - Macro Repeat
 *
 * \ingroup group_sam0_utils
 *
 * @{
 */

#include "preprocessor.h"

/** Maximal number of repetitions supported by MREPEAT. */
#define MREPEAT_LIMIT   256

/** \brief Macro repeat.
 *
 * This macro represents a horizontal repetition construct.
 *
 * \param[in] count  The number of repetitious calls to macro. Valid values
 *                   range from 0 to MREPEAT_LIMIT.
 * \param[in] macro  A binary operation of the form macro(n, data). This macro
 *                   is expanded by MREPEAT with the current repetition number
 *                   and the auxiliary data argument.
 * \param[in] data   Auxiliary data passed to macro.
 *
 * \return       <tt>macro(0, data) macro(1, data) ... macro(count - 1, data)</tt>
 */
#define MREPEAT(count, macro, data) TPASTE2(MREPEAT, count) (macro, data)

#define MREPEAT0(  macro, data)
#define MREPEAT1(  macro, data)    MREPEAT0(  macro, data)   macro(  0, data)
#define MREPEAT2(  macro, data)    MREPEAT1(  macro, data)   macro(  1, data)
#define MREPEAT3(  macro, data)    MREPEAT2(  macro, data)   macro(  2, data)
#define MREPEAT4(  macro, data)    MREPEAT3(  macro, data)   macro(  3, data)
#define MREPEAT5(  macro, data)    MREPEAT4(  macro, data)   macro(  4, data)
#define MREPEAT6(  macro, data)    MREPEAT5(  macro, data)   macro(  5, data)
#define MREPEAT7(  macro, data)    MREPEAT6(  macro, data)   macro(  6, data)
#define MREPEAT8(  macro, data)    MREPEAT7(  macro, data)   macro(  7, data)
#define MREPEAT9(  macro, data)    MREPEAT8(  macro, data)   macro(  8, data)
#define MREPEAT10( macro, data)    MREPEAT9(  macro, data)   macro(  9, data)
#define MREPEAT11( macro, data)    MREPEAT10( macro, data)   macro( 10, data)
#define MREPEAT12( macro, data)    MREPEAT11( macro, data)   macro( 11, data)
#define MREPEAT13( macro, data)    MREPEAT12( macro, data)   macro( 12, data)
#define MREPEAT14( macro, data)    MREPEAT13( macro, data)   macro( 13, data)
#define MREPEAT15( macro, data)    MREPEAT14( macro, data)   macro( 14, data)
#define MREPEAT16( macro, data)    MREPEAT15( macro, data)   macro( 15, data)
#define MREPEAT17( macro, data)    MREPEAT16( macro, data)   macro( 16, data)
#define MREPEAT18( macro, data)    MREPEAT17( macro, data)   macro( 17, data)
#define MREPEAT19( macro, data)    MREPEAT18( macro, data)   macro( 18, data)
#define MREPEAT20( macro, data)    MREPEAT19( macro, data)   macro( 19, data)
#define MREPEAT21( macro, data)    MREPEAT20( macro, data)   macro( 20, data)
#define MREPEAT22( macro, data)    MREPEAT21( macro, data)   macro( 21, data)
#define MREPEAT23( macro, data)    MREPEAT22( macro, data)   macro( 22, data)
#define MREPEAT24( macro, data)    MREPEAT23( macro, data)   macro( 23, data)
#define MREPEAT25( macro, data)    MREPEAT24( macro, data)   macro( 24, data)
#define MREPEAT26( macro, data)    MREPEAT25( macro, data)   macro( 25, data)
#define MREPEAT27( macro, data)    MREPEAT26( macro, data)   macro( 26, data)
#define MREPEAT28( macro, data)    MREPEAT27( macro, data)   macro( 27, data)
#define MREPEAT29( macro, data)    MREPEAT28( macro, data)   macro( 28, data)
#define MREPEAT30( macro, data)    MREPEAT29( macro, data)   macro( 29, data)
#define MREPEAT31( macro, data)    MREPEAT30( macro, data)   macro( 30, data)
#define MREPEAT32( macro, data)    MREPEAT31( macro, data)   macro( 31, data)
#define MREPEAT33( macro, data)    MREPEAT32( macro, data)   macro( 32, data)
#define MREPEAT34( macro, data)    MREPEAT33( macro, data)   macro( 33, data)
#define MREPEAT35( macro, data)    MREPEAT34( macro, data)   macro( 34, data)
#define MREPEAT36( macro, data)    MREPEAT35( macro, data)   macro( 35, data)
#define MREPEAT37( macro, data)    MREPEAT36( macro, data)   macro( 36, data)
#define MREPEAT38( macro, data)    MREPEAT37( macro, data)   macro( 37, data)
#define MREPEAT39( macro, data)    MREPEAT38( macro, data)   macro( 38, data)
#define MREPEAT40( macro, data)    MREPEAT39( macro, data)   macro( 39, data)
#define MREPEAT41( macro, data)    MREPEAT40( macro, data)   macro( 40, data)
#define MREPEAT42( macro, data)    MREPEAT41( macro, data)   macro( 41, data)
#define MREPEAT43( macro, data)    MREPEAT42( macro, data)   macro( 42, data)
#define MREPEAT44( macro, data)    MREPEAT43( macro, data)   macro( 43, data)
#define MREPEAT45( macro, data)    MREPEAT44( macro, data)   macro( 44, data)
#define MREPEAT46( macro, data)    MREPEAT45( macro, data)   macro( 45, data)
#define MREPEAT47( macro, data)    MREPEAT46( macro, data)   macro( 46, data)
#define MREPEAT48( macro, data)    MREPEAT47( macro, data)   macro( 47, data)
#define MREPEAT49( macro, data)    MREPEAT48( macro, data)   macro( 48, data)
#define MREPEAT50( macro, data)    MREPEAT49( macro, data)   macro( 49, data)
#define MREPEAT51( macro, data)    MREPEAT50( macro, data)   macro( 50, data)
#define MREPEAT52( macro, data)    MREPEAT51( macro, data)   macro( 51, data)
#define MREPEAT53( macro, data)    MREPEAT52( macro, data)   macro( 52, data)
#define MREPEAT54( macro, data)    MREPEAT53( macro, data)   macro( 53, data)
#define MREPEAT55( macro, data)    MREPEAT54( macro, data)   macro( 54, data)
#define MREPEAT56( macro, data)    MREPEAT55( macro, data)   macro( 55, data)
#define MREPEAT57( macro, data)    MREPEAT56( macro, data)   macro( 56, data)
#define MREPEAT58( macro, data)    MREPEAT57( macro, data)   macro( 57, data)
#define MREPEAT59( macro, data)    MREPEAT58( macro, data)   macro( 58, data)
#define MREPEAT60( macro, data)    MREPEAT59( macro, data)   macro( 59, data)
#define MREPEAT61( macro, data)    MREPEAT60( macro, data)   macro( 60, data)
#define MREPEAT62( macro, data)    MREPEAT61( macro, data)   macro( 61, data)
#define MREPEAT63( macro, data)    MREPEAT62( macro, data)   macro( 62, data)
#define MREPEAT64( macro, data)    MREPEAT63( macro, data)   macro( 63, data)
#define MREPEAT65( macro, data)    MREPEAT64( macro, data)   macro( 64, data)
#define MREPEAT66( macro, data)    MREPEAT65( macro, data)   macro( 65, data)
#define MREPEAT67( macro, data)    MREPEAT66( macro, data)   macro( 66, data)
#define MREPEAT68( macro, data)    MREPEAT67( macro, data)   macro( 67, data)
#define MREPEAT69( macro, data)    MREPEAT68( macro, data)   macro( 68, data)
#define MREPEAT70( macro, data)    MREPEAT69( macro, data)   macro( 69, data)
#define MREPEAT71( macro, data)    MREPEAT70( macro, data)   macro( 70, data)
#define MREPEAT72( macro, data)    MREPEAT71( macro, data)   macro( 71, data)
#define MREPEAT73( macro, data)    MREPEAT72( macro, data)   macro( 72, data)
#define MREPEAT74( macro, data)    MREPEAT73( macro, data)   macro( 73, data)
#define MREPEAT75( macro, data)    MREPEAT74( macro, data)   macro( 74, data)
#define MREPEAT76( macro, data)    MREPEAT75( macro, data)   macro( 75, data)
#define MREPEAT77( macro, data)    MREPEAT76( macro, data)   macro( 76, data)
#define MREPEAT78( macro, data)    MREPEAT77( macro, data)   macro( 77, data)
#define MREPEAT79( macro, data)    MREPEAT78( macro, data)   macro( 78, data)
#define MREPEAT80( macro, data)    MREPEAT79( macro, data)   macro( 79, data)
#define MREPEAT81( macro, data)    MREPEAT80( macro, data)   macro( 80, data)
#define MREPEAT82( macro, data)    MREPEAT81( macro, data)   macro( 81, data)
#define MREPEAT83( macro, data)    MREPEAT82( macro, data)   macro( 82, data)
#define MREPEAT84( macro, data)    MREPEAT83( macro, data)   macro( 83, data)
#define MREPEAT85( macro, data)    MREPEAT84( macro, data)   macro( 84, data)
#define MREPEAT86( macro, data)    MREPEAT85( macro, data)   macro( 85, data)
#define MREPEAT87( macro, data)    MREPEAT86( macro, data)   macro( 86, data)
#define MREPEAT88( macro, data)    MREPEAT87( macro, data)   macro( 87, data)
#define MREPEAT89( macro, data)    MREPEAT88( macro, data)   macro( 88, data)
#define MREPEAT90( macro, data)    MREPEAT89( macro, data)   macro( 89, data)
#define MREPEAT91( macro, data)    MREPEAT90( macro, data)   macro( 90, data)
#define MREPEAT92( macro, data)    MREPEAT91( macro, data)   macro( 91, data)
#define MREPEAT93( macro, data)    MREPEAT92( macro, data)   macro( 92, data)
#define MREPEAT94( macro, data)    MREPEAT93( macro, data)   macro( 93, data)
#define MREPEAT95( macro, data)    MREPEAT94( macro, data)   macro( 94, data)
#define MREPEAT96( macro, data)    MREPEAT95( macro, data)   macro( 95, data)
#define MREPEAT97( macro, data)    MREPEAT96( macro, data)   macro( 96, data)
#define MREPEAT98( macro, data)    MREPEAT97( macro, data)   macro( 97, data)
#define MREPEAT99( macro, data)    MREPEAT98( macro, data)   macro( 98, data)
#define MREPEAT100(macro, data)    MREPEAT99( macro, data)   macro( 99, data)
#define MREPEAT101(macro, data)    MREPEAT100(macro, data)   macro(100, data)
#define MREPEAT102(macro, data)    MREPEAT101(macro, data)   macro(101, data)
#define MREPEAT103(macro, data)    MREPEAT102(macro, data)   macro(102, data)
#define MREPEAT104(macro, data)    MREPEAT103(macro, data)   macro(103, data)
#define MREPEAT105(macro, data)    MREPEAT104(macro, data)   macro(104, data)
#define MREPEAT106(macro, data)    MREPEAT105(macro, data)   macro(105, data)
#define MREPEAT107(macro, data)    MREPEAT106(macro, data)   macro(106, data)
#define MREPEAT108(macro, data)    MREPEAT107(macro, data)   macro(107, data)
#define MREPEAT109(macro, data)    MREPEAT108(macro, data)   macro(108, data)
#define MREPEAT110(macro, data)    MREPEAT109(macro, data)   macro(109, data)
#define MREPEAT111(macro, data)    MREPEAT110(macro, data)   macro(110, data)
#define MREPEAT112(macro, data)    MREPEAT111(macro, data)   macro(111, data)
#define MREPEAT113(macro, data)    MREPEAT112(macro, data)   macro(112, data)
#define MREPEAT114(macro, data)    MREPEAT113(macro, data)   macro(113, data)
#define MREPEAT115(macro, data)    MREPEAT114(macro, data)   macro(114, data)
#define MREPEAT116(macro, data)    MREPEAT115(macro, data)   macro(115, data)
#define MREPEAT117(macro, data)    MREPEAT116(macro, data)   macro(116, data)
#define MREPEAT118(macro, data)    MREPEAT117(macro, data)   macro(117, data)
#define MREPEAT119(macro, data)    MREPEAT118(macro, data)   macro(118, data)
#define MREPEAT120(macro, data)    MREPEAT119(macro, data)   macro(119, data)
#define MREPEAT121(macro, data)    MREPEAT120(macro, data)   macro(120, data)
#define MREPEAT122(macro, data)    MREPEAT121(macro, data)   macro(121, data)
#define MREPEAT123(macro, data)    MREPEAT122(macro, data)   macro(122, data)
#define MREPEAT124(macro, data)    MREPEAT123(macro, data)   macro(123, data)
#define MREPEAT125(macro, data)    MREPEAT124(macro, data)   macro(124, data)
#define MREPEAT126(macro, data)    MREPEAT125(macro, data)   macro(125, data)
#define MREPEAT127(macro, data)    MREPEAT126(macro, data)   macro(126, data)
#define MREPEAT128(macro, data)    MREPEAT127(macro, data)   macro(127, data)
#define MREPEAT129(macro, data)    MREPEAT128(macro, data)   macro(128, data)
#define MREPEAT130(macro, data)    MREPEAT129(macro, data)   macro(129, data)
#define MREPEAT131(macro, data)    MREPEAT130(macro, data)   macro(130, data)
#define MREPEAT132(macro, data)    MREPEAT131(macro, data)   macro(131, data)
#define MREPEAT133(macro, data)    MREPEAT132(macro, data)   macro(132, data)
#define MREPEAT134(macro, data)    MREPEAT133(macro, data)   macro(133, data)
#define MREPEAT135(macro, data)    MREPEAT134(macro, data)   macro(134, data)
#define MREPEAT136(macro, data)    MREPEAT135(macro, data)   macro(135, data)
#define MREPEAT137(macro, data)    MREPEAT136(macro, data)   macro(136, data)
#define MREPEAT138(macro, data)    MREPEAT137(macro, data)   macro(137, data)
#define MREPEAT139(macro, data)    MREPEAT138(macro, data)   macro(138, data)
#define MREPEAT140(macro, data)    MREPEAT139(macro, data)   macro(139, data)
#define MREPEAT141(macro, data)    MREPEAT140(macro, data)   macro(140, data)
#define MREPEAT142(macro, data)    MREPEAT141(macro, data)   macro(141, data)
#define MREPEAT143(macro, data)    MREPEAT142(macro, data)   macro(142, data)
#define MREPEAT144(macro, data)    MREPEAT143(macro, data)   macro(143, data)
#define MREPEAT145(macro, data)    MREPEAT144(macro, data)   macro(144, data)
#define MREPEAT146(macro, data)    MREPEAT145(macro, data)   macro(145, data)
#define MREPEAT147(macro, data)    MREPEAT146(macro, data)   macro(146, data)
#define MREPEAT148(macro, data)    MREPEAT147(macro, data)   macro(147, data)
#define MREPEAT149(macro, data)    MREPEAT148(macro, data)   macro(148, data)
#define MREPEAT150(macro, data)    MREPEAT149(macro, data)   macro(149, data)
#define MREPEAT151(macro, data)    MREPEAT150(macro, data)   macro(150, data)
#define MREPEAT152(macro, data)    MREPEAT151(macro, data)   macro(151, data)
#define MREPEAT153(macro, data)    MREPEAT152(macro, data)   macro(152, data)
#define MREPEAT154(macro, data)    MREPEAT153(macro, data)   macro(153, data)
#define MREPEAT155(macro, data)    MREPEAT154(macro, data)   macro(154, data)
#define MREPEAT156(macro, data)    MREPEAT155(macro, data)   macro(155, data)
#define MREPEAT157(macro, data)    MREPEAT156(macro, data)   macro(156, data)
#define MREPEAT158(macro, data)    MREPEAT157(macro, data)   macro(157, data)
#define MREPEAT159(macro, data)    MREPEAT158(macro, data)   macro(158, data)
#define MREPEAT160(macro, data)    MREPEAT159(macro, data)   macro(159, data)
#define MREPEAT161(macro, data)    MREPEAT160(macro, data)   macro(160, data)
#define MREPEAT162(macro, data)    MREPEAT161(macro, data)   macro(161, data)
#define MREPEAT163(macro, data)    MREPEAT162(macro, data)   macro(162, data)
#define MREPEAT164(macro, data)    MREPEAT163(macro, data)   macro(163, data)
#define MREPEAT165(macro, data)    MREPEAT164(macro, data)   macro(164, data)
#define MREPEAT166(macro, data)    MREPEAT165(macro, data)   macro(165, data)
#define MREPEAT167(macro, data)    MREPEAT166(macro, data)   macro(166, data)
#define MREPEAT168(macro, data)    MREPEAT167(macro, data)   macro(167, data)
#define MREPEAT169(macro, data)    MREPEAT168(macro, data)   macro(168, data)
#define MREPEAT170(macro, data)    MREPEAT169(macro, data)   macro(169, data)
#define MREPEAT171(macro, data)    MREPEAT170(macro, data)   macro(170, data)
#define MREPEAT172(macro, data)    MREPEAT171(macro, data)   macro(171, data)
#define MREPEAT173(macro, data)    MREPEAT172(macro, data)   macro(172, data)
#define MREPEAT174(macro, data)    MREPEAT173(macro, data)   macro(173, data)
#define MREPEAT175(macro, data)    MREPEAT174(macro, data)   macro(174, data)
#define MREPEAT176(macro, data)    MREPEAT175(macro, data)   macro(175, data)
#define MREPEAT177(macro, data)    MREPEAT176(macro, data)   macro(176, data)
#define MREPEAT178(macro, data)    MREPEAT177(macro, data)   macro(177, data)
#define MREPEAT179(macro, data)    MREPEAT178(macro, data)   macro(178, data)
#define MREPEAT180(macro, data)    MREPEAT179(macro, data)   macro(179, data)
#define MREPEAT181(macro, data)    MREPEAT180(macro, data)   macro(180, data)
#define MREPEAT182(macro, data)    MREPEAT181(macro, data)   macro(181, data)
#define MREPEAT183(macro, data)    MREPEAT182(macro, data)   macro(182, data)
#define MREPEAT184(macro, data)    MREPEAT183(macro, data)   macro(183, data)
#define MREPEAT185(macro, data)    MREPEAT184(macro, data)   macro(184, data)
#define MREPEAT186(macro, data)    MREPEAT185(macro, data)   macro(185, data)
#define MREPEAT187(macro, data)    MREPEAT186(macro, data)   macro(186, data)
#define MREPEAT188(macro, data)    MREPEAT187(macro, data)   macro(187, data)
#define MREPEAT189(macro, data)    MREPEAT188(macro, data)   macro(188, data)
#define MREPEAT190(macro, data)    MREPEAT189(macro, data)   macro(189, data)
#define MREPEAT191(macro, data)    MREPEAT190(macro, data)   macro(190, data)
#define MREPEAT192(macro, data)    MREPEAT191(macro, data)   macro(191, data)
#define MREPEAT193(macro, data)    MREPEAT192(macro, data)   macro(192, data)
#define MREPEAT194(macro, data)    MREPEAT193(macro, data)   macro(193, data)
#define MREPEAT195(macro, data)    MREPEAT194(macro, data)   macro(194, data)
#define MREPEAT196(macro, data)    MREPEAT195(macro, data)   macro(195, data)
#define MREPEAT197(macro, data)    MREPEAT196(macro, data)   macro(196, data)
#define MREPEAT198(macro, data)    MREPEAT197(macro, data)   macro(197, data)
#define MREPEAT199(macro, data)    MREPEAT198(macro, data)   macro(198, data)
#define MREPEAT200(macro, data)    MREPEAT199(macro, data)   macro(199, data)
#define MREPEAT201(macro, data)    MREPEAT200(macro, data)   macro(200, data)
#define MREPEAT202(macro, data)    MREPEAT201(macro, data)   macro(201, data)
#define MREPEAT203(macro, data)    MREPEAT202(macro, data)   macro(202, data)
#define MREPEAT204(macro, data)    MREPEAT203(macro, data)   macro(203, data)
#define MREPEAT205(macro, data)    MREPEAT204(macro, data)   macro(204, data)
#define MREPEAT206(macro, data)    MREPEAT205(macro, data)   macro(205, data)
#define MREPEAT207(macro, data)    MREPEAT206(macro, data)   macro(206, data)
#define MREPEAT208(macro, data)    MREPEAT207(macro, data)   macro(207, data)
#define MREPEAT209(macro, data)    MREPEAT208(macro, data)   macro(208, data)
#define MREPEAT210(macro, data)    MREPEAT209(macro, data)   macro(209, data)
#define MREPEAT211(macro, data)    MREPEAT210(macro, data)   macro(210, data)
#define MREPEAT212(macro, data)    MREPEAT211(macro, data)   macro(211, data)
#define MREPEAT213(macro, data)    MREPEAT212(macro, data)   macro(212, data)
#define MREPEAT214(macro, data)    MREPEAT213(macro, data)   macro(213, data)
#define MREPEAT215(macro, data)    MREPEAT214(macro, data)   macro(214, data)
#define MREPEAT216(macro, data)    MREPEAT215(macro, data)   macro(215, data)
#define MREPEAT217(macro, data)    MREPEAT216(macro, data)   macro(216, data)
#define MREPEAT218(macro, data)    MREPEAT217(macro, data)   macro(217, data)
#define MREPEAT219(macro, data)    MREPEAT218(macro, data)   macro(218, data)
#define MREPEAT220(macro, data)    MREPEAT219(macro, data)   macro(219, data)
#define MREPEAT221(macro, data)    MREPEAT220(macro, data)   macro(220, data)
#define MREPEAT222(macro, data)    MREPEAT221(macro, data)   macro(221, data)
#define MREPEAT223(macro, data)    MREPEAT222(macro, data)   macro(222, data)
#define MREPEAT224(macro, data)    MREPEAT223(macro, data)   macro(223, data)
#define MREPEAT225(macro, data)    MREPEAT224(macro, data)   macro(224, data)
#define MREPEAT226(macro, data)    MREPEAT225(macro, data)   macro(225, data)
#define MREPEAT227(macro, data)    MREPEAT226(macro, data)   macro(226, data)
#define MREPEAT228(macro, data)    MREPEAT227(macro, data)   macro(227, data)
#define MREPEAT229(macro, data)    MREPEAT228(macro, data)   macro(228, data)
#define MREPEAT230(macro, data)    MREPEAT229(macro, data)   macro(229, data)
#define MREPEAT231(macro, data)    MREPEAT230(macro, data)   macro(230, data)
#define MREPEAT232(macro, data)    MREPEAT231(macro, data)   macro(231, data)
#define MREPEAT233(macro, data)    MREPEAT232(macro, data)   macro(232, data)
#define MREPEAT234(macro, data)    MREPEAT233(macro, data)   macro(233, data)
#define MREPEAT235(macro, data)    MREPEAT234(macro, data)   macro(234, data)
#define MREPEAT236(macro, data)    MREPEAT235(macro, data)   macro(235, data)
#define MREPEAT237(macro, data)    MREPEAT236(macro, data)   macro(236, data)
#define MREPEAT238(macro, data)    MREPEAT237(macro, data)   macro(237, data)
#define MREPEAT239(macro, data)    MREPEAT238(macro, data)   macro(238, data)
#define MREPEAT240(macro, data)    MREPEAT239(macro, data)   macro(239, data)
#define MREPEAT241(macro, data)    MREPEAT240(macro, data)   macro(240, data)
#define MREPEAT242(macro, data)    MREPEAT241(macro, data)   macro(241, data)
#define MREPEAT243(macro, data)    MREPEAT242(macro, data)   macro(242, data)
#define MREPEAT244(macro, data)    MREPEAT243(macro, data)   macro(243, data)
#define MREPEAT245(macro, data)    MREPEAT244(macro, data)   macro(244, data)
#define MREPEAT246(macro, data)    MREPEAT245(macro, data)   macro(245, data)
#define MREPEAT247(macro, data)    MREPEAT246(macro, data)   macro(246, data)
#define MREPEAT248(macro, data)    MREPEAT247(macro, data)   macro(247, data)
#define MREPEAT249(macro, data)    MREPEAT248(macro, data)   macro(248, data)
#define MREPEAT250(macro, data)    MREPEAT249(macro, data)   macro(249, data)
#define MREPEAT251(macro, data)    MREPEAT250(macro, data)   macro(250, data)
#define MREPEAT252(macro, data)    MREPEAT251(macro, data)   macro(251, data)
#define MREPEAT253(macro, data)    MREPEAT252(macro, data)   macro(252, data)
#define MREPEAT254(macro, data)    MREPEAT253(macro, data)   macro(253, data)
#define MREPEAT255(macro, data)    MREPEAT254(macro, data)   macro(254, data)
#define MREPEAT256(macro, data)    MREPEAT255(macro, data)   macro(255, data)

/** @} */

#endif  /* _MREPEAT_H_ */
