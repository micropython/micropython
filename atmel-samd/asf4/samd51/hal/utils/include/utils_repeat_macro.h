/**
 * \file
 *
 * \brief Repeat macro.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _UTILS_REPEAT_MACRO_H
#define _UTILS_REPEAT_MACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * \brief Sequently repeates specified macro for n times (255 max).
 *
 * Specified macro shall have two arguments:  macro(arg, i)
 *   arg - user defined argument, which have the same value for all iterations.
 *   i - iteration number; numbering begins from zero and increments on each
 *   iteration.
 *
 * \param[in] macro - macro to be repeated
 * \param[in] arg - user defined argument for repeated macro
 * \param[in] n - total number of iterations (255 max)
 */
#define REPEAT_MACRO(macro, arg, n) REPEAT_MACRO_I(macro, arg, n)

/*
 * \brief Second level is needed to get integer literal from "n" if it is
 *        defined as macro
 */
#define REPEAT_MACRO_I(macro, arg, n) REPEAT##n(macro, arg, 0)

#define REPEAT1(macro, arg, n) macro(arg, n)
#define REPEAT2(macro, arg, n) macro(arg, n) REPEAT1(macro, arg, INC_VALUE(n))
#define REPEAT3(macro, arg, n) macro(arg, n) REPEAT2(macro, arg, INC_VALUE(n))
#define REPEAT4(macro, arg, n) macro(arg, n) REPEAT3(macro, arg, INC_VALUE(n))
#define REPEAT5(macro, arg, n) macro(arg, n) REPEAT4(macro, arg, INC_VALUE(n))
#define REPEAT6(macro, arg, n) macro(arg, n) REPEAT5(macro, arg, INC_VALUE(n))
#define REPEAT7(macro, arg, n) macro(arg, n) REPEAT6(macro, arg, INC_VALUE(n))
#define REPEAT8(macro, arg, n) macro(arg, n) REPEAT7(macro, arg, INC_VALUE(n))
#define REPEAT9(macro, arg, n) macro(arg, n) REPEAT8(macro, arg, INC_VALUE(n))
#define REPEAT10(macro, arg, n) macro(arg, n) REPEAT9(macro, arg, INC_VALUE(n))
#define REPEAT11(macro, arg, n) macro(arg, n) REPEAT10(macro, arg, INC_VALUE(n))
#define REPEAT12(macro, arg, n) macro(arg, n) REPEAT11(macro, arg, INC_VALUE(n))
#define REPEAT13(macro, arg, n) macro(arg, n) REPEAT12(macro, arg, INC_VALUE(n))
#define REPEAT14(macro, arg, n) macro(arg, n) REPEAT13(macro, arg, INC_VALUE(n))
#define REPEAT15(macro, arg, n) macro(arg, n) REPEAT14(macro, arg, INC_VALUE(n))
#define REPEAT16(macro, arg, n) macro(arg, n) REPEAT15(macro, arg, INC_VALUE(n))
#define REPEAT17(macro, arg, n) macro(arg, n) REPEAT16(macro, arg, INC_VALUE(n))
#define REPEAT18(macro, arg, n) macro(arg, n) REPEAT17(macro, arg, INC_VALUE(n))
#define REPEAT19(macro, arg, n) macro(arg, n) REPEAT18(macro, arg, INC_VALUE(n))
#define REPEAT20(macro, arg, n) macro(arg, n) REPEAT19(macro, arg, INC_VALUE(n))
#define REPEAT21(macro, arg, n) macro(arg, n) REPEAT20(macro, arg, INC_VALUE(n))
#define REPEAT22(macro, arg, n) macro(arg, n) REPEAT21(macro, arg, INC_VALUE(n))
#define REPEAT23(macro, arg, n) macro(arg, n) REPEAT22(macro, arg, INC_VALUE(n))
#define REPEAT24(macro, arg, n) macro(arg, n) REPEAT23(macro, arg, INC_VALUE(n))
#define REPEAT25(macro, arg, n) macro(arg, n) REPEAT24(macro, arg, INC_VALUE(n))
#define REPEAT26(macro, arg, n) macro(arg, n) REPEAT25(macro, arg, INC_VALUE(n))
#define REPEAT27(macro, arg, n) macro(arg, n) REPEAT26(macro, arg, INC_VALUE(n))
#define REPEAT28(macro, arg, n) macro(arg, n) REPEAT27(macro, arg, INC_VALUE(n))
#define REPEAT29(macro, arg, n) macro(arg, n) REPEAT28(macro, arg, INC_VALUE(n))
#define REPEAT30(macro, arg, n) macro(arg, n) REPEAT29(macro, arg, INC_VALUE(n))
#define REPEAT31(macro, arg, n) macro(arg, n) REPEAT30(macro, arg, INC_VALUE(n))
#define REPEAT32(macro, arg, n) macro(arg, n) REPEAT31(macro, arg, INC_VALUE(n))
#define REPEAT33(macro, arg, n) macro(arg, n) REPEAT32(macro, arg, INC_VALUE(n))
#define REPEAT34(macro, arg, n) macro(arg, n) REPEAT33(macro, arg, INC_VALUE(n))
#define REPEAT35(macro, arg, n) macro(arg, n) REPEAT34(macro, arg, INC_VALUE(n))
#define REPEAT36(macro, arg, n) macro(arg, n) REPEAT35(macro, arg, INC_VALUE(n))
#define REPEAT37(macro, arg, n) macro(arg, n) REPEAT36(macro, arg, INC_VALUE(n))
#define REPEAT38(macro, arg, n) macro(arg, n) REPEAT37(macro, arg, INC_VALUE(n))
#define REPEAT39(macro, arg, n) macro(arg, n) REPEAT38(macro, arg, INC_VALUE(n))
#define REPEAT40(macro, arg, n) macro(arg, n) REPEAT39(macro, arg, INC_VALUE(n))
#define REPEAT41(macro, arg, n) macro(arg, n) REPEAT40(macro, arg, INC_VALUE(n))
#define REPEAT42(macro, arg, n) macro(arg, n) REPEAT41(macro, arg, INC_VALUE(n))
#define REPEAT43(macro, arg, n) macro(arg, n) REPEAT42(macro, arg, INC_VALUE(n))
#define REPEAT44(macro, arg, n) macro(arg, n) REPEAT43(macro, arg, INC_VALUE(n))
#define REPEAT45(macro, arg, n) macro(arg, n) REPEAT44(macro, arg, INC_VALUE(n))
#define REPEAT46(macro, arg, n) macro(arg, n) REPEAT45(macro, arg, INC_VALUE(n))
#define REPEAT47(macro, arg, n) macro(arg, n) REPEAT46(macro, arg, INC_VALUE(n))
#define REPEAT48(macro, arg, n) macro(arg, n) REPEAT47(macro, arg, INC_VALUE(n))
#define REPEAT49(macro, arg, n) macro(arg, n) REPEAT48(macro, arg, INC_VALUE(n))
#define REPEAT50(macro, arg, n) macro(arg, n) REPEAT49(macro, arg, INC_VALUE(n))
#define REPEAT51(macro, arg, n) macro(arg, n) REPEAT50(macro, arg, INC_VALUE(n))
#define REPEAT52(macro, arg, n) macro(arg, n) REPEAT51(macro, arg, INC_VALUE(n))
#define REPEAT53(macro, arg, n) macro(arg, n) REPEAT52(macro, arg, INC_VALUE(n))
#define REPEAT54(macro, arg, n) macro(arg, n) REPEAT53(macro, arg, INC_VALUE(n))
#define REPEAT55(macro, arg, n) macro(arg, n) REPEAT54(macro, arg, INC_VALUE(n))
#define REPEAT56(macro, arg, n) macro(arg, n) REPEAT55(macro, arg, INC_VALUE(n))
#define REPEAT57(macro, arg, n) macro(arg, n) REPEAT56(macro, arg, INC_VALUE(n))
#define REPEAT58(macro, arg, n) macro(arg, n) REPEAT57(macro, arg, INC_VALUE(n))
#define REPEAT59(macro, arg, n) macro(arg, n) REPEAT58(macro, arg, INC_VALUE(n))
#define REPEAT60(macro, arg, n) macro(arg, n) REPEAT59(macro, arg, INC_VALUE(n))
#define REPEAT61(macro, arg, n) macro(arg, n) REPEAT60(macro, arg, INC_VALUE(n))
#define REPEAT62(macro, arg, n) macro(arg, n) REPEAT61(macro, arg, INC_VALUE(n))
#define REPEAT63(macro, arg, n) macro(arg, n) REPEAT62(macro, arg, INC_VALUE(n))
#define REPEAT64(macro, arg, n) macro(arg, n) REPEAT63(macro, arg, INC_VALUE(n))
#define REPEAT65(macro, arg, n) macro(arg, n) REPEAT64(macro, arg, INC_VALUE(n))
#define REPEAT66(macro, arg, n) macro(arg, n) REPEAT65(macro, arg, INC_VALUE(n))
#define REPEAT67(macro, arg, n) macro(arg, n) REPEAT66(macro, arg, INC_VALUE(n))
#define REPEAT68(macro, arg, n) macro(arg, n) REPEAT67(macro, arg, INC_VALUE(n))
#define REPEAT69(macro, arg, n) macro(arg, n) REPEAT68(macro, arg, INC_VALUE(n))
#define REPEAT70(macro, arg, n) macro(arg, n) REPEAT69(macro, arg, INC_VALUE(n))
#define REPEAT71(macro, arg, n) macro(arg, n) REPEAT70(macro, arg, INC_VALUE(n))
#define REPEAT72(macro, arg, n) macro(arg, n) REPEAT71(macro, arg, INC_VALUE(n))
#define REPEAT73(macro, arg, n) macro(arg, n) REPEAT72(macro, arg, INC_VALUE(n))
#define REPEAT74(macro, arg, n) macro(arg, n) REPEAT73(macro, arg, INC_VALUE(n))
#define REPEAT75(macro, arg, n) macro(arg, n) REPEAT74(macro, arg, INC_VALUE(n))
#define REPEAT76(macro, arg, n) macro(arg, n) REPEAT75(macro, arg, INC_VALUE(n))
#define REPEAT77(macro, arg, n) macro(arg, n) REPEAT76(macro, arg, INC_VALUE(n))
#define REPEAT78(macro, arg, n) macro(arg, n) REPEAT77(macro, arg, INC_VALUE(n))
#define REPEAT79(macro, arg, n) macro(arg, n) REPEAT78(macro, arg, INC_VALUE(n))
#define REPEAT80(macro, arg, n) macro(arg, n) REPEAT79(macro, arg, INC_VALUE(n))
#define REPEAT81(macro, arg, n) macro(arg, n) REPEAT80(macro, arg, INC_VALUE(n))
#define REPEAT82(macro, arg, n) macro(arg, n) REPEAT81(macro, arg, INC_VALUE(n))
#define REPEAT83(macro, arg, n) macro(arg, n) REPEAT82(macro, arg, INC_VALUE(n))
#define REPEAT84(macro, arg, n) macro(arg, n) REPEAT83(macro, arg, INC_VALUE(n))
#define REPEAT85(macro, arg, n) macro(arg, n) REPEAT84(macro, arg, INC_VALUE(n))
#define REPEAT86(macro, arg, n) macro(arg, n) REPEAT85(macro, arg, INC_VALUE(n))
#define REPEAT87(macro, arg, n) macro(arg, n) REPEAT86(macro, arg, INC_VALUE(n))
#define REPEAT88(macro, arg, n) macro(arg, n) REPEAT87(macro, arg, INC_VALUE(n))
#define REPEAT89(macro, arg, n) macro(arg, n) REPEAT88(macro, arg, INC_VALUE(n))
#define REPEAT90(macro, arg, n) macro(arg, n) REPEAT89(macro, arg, INC_VALUE(n))
#define REPEAT91(macro, arg, n) macro(arg, n) REPEAT90(macro, arg, INC_VALUE(n))
#define REPEAT92(macro, arg, n) macro(arg, n) REPEAT91(macro, arg, INC_VALUE(n))
#define REPEAT93(macro, arg, n) macro(arg, n) REPEAT92(macro, arg, INC_VALUE(n))
#define REPEAT94(macro, arg, n) macro(arg, n) REPEAT93(macro, arg, INC_VALUE(n))
#define REPEAT95(macro, arg, n) macro(arg, n) REPEAT94(macro, arg, INC_VALUE(n))
#define REPEAT96(macro, arg, n) macro(arg, n) REPEAT95(macro, arg, INC_VALUE(n))
#define REPEAT97(macro, arg, n) macro(arg, n) REPEAT96(macro, arg, INC_VALUE(n))
#define REPEAT98(macro, arg, n) macro(arg, n) REPEAT97(macro, arg, INC_VALUE(n))
#define REPEAT99(macro, arg, n) macro(arg, n) REPEAT98(macro, arg, INC_VALUE(n))
#define REPEAT100(macro, arg, n) macro(arg, n) REPEAT99(macro, arg, INC_VALUE(n))
#define REPEAT101(macro, arg, n) macro(arg, n) REPEAT100(macro, arg, INC_VALUE(n))
#define REPEAT102(macro, arg, n) macro(arg, n) REPEAT101(macro, arg, INC_VALUE(n))
#define REPEAT103(macro, arg, n) macro(arg, n) REPEAT102(macro, arg, INC_VALUE(n))
#define REPEAT104(macro, arg, n) macro(arg, n) REPEAT103(macro, arg, INC_VALUE(n))
#define REPEAT105(macro, arg, n) macro(arg, n) REPEAT104(macro, arg, INC_VALUE(n))
#define REPEAT106(macro, arg, n) macro(arg, n) REPEAT105(macro, arg, INC_VALUE(n))
#define REPEAT107(macro, arg, n) macro(arg, n) REPEAT106(macro, arg, INC_VALUE(n))
#define REPEAT108(macro, arg, n) macro(arg, n) REPEAT107(macro, arg, INC_VALUE(n))
#define REPEAT109(macro, arg, n) macro(arg, n) REPEAT108(macro, arg, INC_VALUE(n))
#define REPEAT110(macro, arg, n) macro(arg, n) REPEAT109(macro, arg, INC_VALUE(n))
#define REPEAT111(macro, arg, n) macro(arg, n) REPEAT110(macro, arg, INC_VALUE(n))
#define REPEAT112(macro, arg, n) macro(arg, n) REPEAT111(macro, arg, INC_VALUE(n))
#define REPEAT113(macro, arg, n) macro(arg, n) REPEAT112(macro, arg, INC_VALUE(n))
#define REPEAT114(macro, arg, n) macro(arg, n) REPEAT113(macro, arg, INC_VALUE(n))
#define REPEAT115(macro, arg, n) macro(arg, n) REPEAT114(macro, arg, INC_VALUE(n))
#define REPEAT116(macro, arg, n) macro(arg, n) REPEAT115(macro, arg, INC_VALUE(n))
#define REPEAT117(macro, arg, n) macro(arg, n) REPEAT116(macro, arg, INC_VALUE(n))
#define REPEAT118(macro, arg, n) macro(arg, n) REPEAT117(macro, arg, INC_VALUE(n))
#define REPEAT119(macro, arg, n) macro(arg, n) REPEAT118(macro, arg, INC_VALUE(n))
#define REPEAT120(macro, arg, n) macro(arg, n) REPEAT119(macro, arg, INC_VALUE(n))
#define REPEAT121(macro, arg, n) macro(arg, n) REPEAT120(macro, arg, INC_VALUE(n))
#define REPEAT122(macro, arg, n) macro(arg, n) REPEAT121(macro, arg, INC_VALUE(n))
#define REPEAT123(macro, arg, n) macro(arg, n) REPEAT122(macro, arg, INC_VALUE(n))
#define REPEAT124(macro, arg, n) macro(arg, n) REPEAT123(macro, arg, INC_VALUE(n))
#define REPEAT125(macro, arg, n) macro(arg, n) REPEAT124(macro, arg, INC_VALUE(n))
#define REPEAT126(macro, arg, n) macro(arg, n) REPEAT125(macro, arg, INC_VALUE(n))
#define REPEAT127(macro, arg, n) macro(arg, n) REPEAT126(macro, arg, INC_VALUE(n))
#define REPEAT128(macro, arg, n) macro(arg, n) REPEAT127(macro, arg, INC_VALUE(n))
#define REPEAT129(macro, arg, n) macro(arg, n) REPEAT128(macro, arg, INC_VALUE(n))
#define REPEAT130(macro, arg, n) macro(arg, n) REPEAT129(macro, arg, INC_VALUE(n))
#define REPEAT131(macro, arg, n) macro(arg, n) REPEAT130(macro, arg, INC_VALUE(n))
#define REPEAT132(macro, arg, n) macro(arg, n) REPEAT131(macro, arg, INC_VALUE(n))
#define REPEAT133(macro, arg, n) macro(arg, n) REPEAT132(macro, arg, INC_VALUE(n))
#define REPEAT134(macro, arg, n) macro(arg, n) REPEAT133(macro, arg, INC_VALUE(n))
#define REPEAT135(macro, arg, n) macro(arg, n) REPEAT134(macro, arg, INC_VALUE(n))
#define REPEAT136(macro, arg, n) macro(arg, n) REPEAT135(macro, arg, INC_VALUE(n))
#define REPEAT137(macro, arg, n) macro(arg, n) REPEAT136(macro, arg, INC_VALUE(n))
#define REPEAT138(macro, arg, n) macro(arg, n) REPEAT137(macro, arg, INC_VALUE(n))
#define REPEAT139(macro, arg, n) macro(arg, n) REPEAT138(macro, arg, INC_VALUE(n))
#define REPEAT140(macro, arg, n) macro(arg, n) REPEAT139(macro, arg, INC_VALUE(n))
#define REPEAT141(macro, arg, n) macro(arg, n) REPEAT140(macro, arg, INC_VALUE(n))
#define REPEAT142(macro, arg, n) macro(arg, n) REPEAT141(macro, arg, INC_VALUE(n))
#define REPEAT143(macro, arg, n) macro(arg, n) REPEAT142(macro, arg, INC_VALUE(n))
#define REPEAT144(macro, arg, n) macro(arg, n) REPEAT143(macro, arg, INC_VALUE(n))
#define REPEAT145(macro, arg, n) macro(arg, n) REPEAT144(macro, arg, INC_VALUE(n))
#define REPEAT146(macro, arg, n) macro(arg, n) REPEAT145(macro, arg, INC_VALUE(n))
#define REPEAT147(macro, arg, n) macro(arg, n) REPEAT146(macro, arg, INC_VALUE(n))
#define REPEAT148(macro, arg, n) macro(arg, n) REPEAT147(macro, arg, INC_VALUE(n))
#define REPEAT149(macro, arg, n) macro(arg, n) REPEAT148(macro, arg, INC_VALUE(n))
#define REPEAT150(macro, arg, n) macro(arg, n) REPEAT149(macro, arg, INC_VALUE(n))
#define REPEAT151(macro, arg, n) macro(arg, n) REPEAT150(macro, arg, INC_VALUE(n))
#define REPEAT152(macro, arg, n) macro(arg, n) REPEAT151(macro, arg, INC_VALUE(n))
#define REPEAT153(macro, arg, n) macro(arg, n) REPEAT152(macro, arg, INC_VALUE(n))
#define REPEAT154(macro, arg, n) macro(arg, n) REPEAT153(macro, arg, INC_VALUE(n))
#define REPEAT155(macro, arg, n) macro(arg, n) REPEAT154(macro, arg, INC_VALUE(n))
#define REPEAT156(macro, arg, n) macro(arg, n) REPEAT155(macro, arg, INC_VALUE(n))
#define REPEAT157(macro, arg, n) macro(arg, n) REPEAT156(macro, arg, INC_VALUE(n))
#define REPEAT158(macro, arg, n) macro(arg, n) REPEAT157(macro, arg, INC_VALUE(n))
#define REPEAT159(macro, arg, n) macro(arg, n) REPEAT158(macro, arg, INC_VALUE(n))
#define REPEAT160(macro, arg, n) macro(arg, n) REPEAT159(macro, arg, INC_VALUE(n))
#define REPEAT161(macro, arg, n) macro(arg, n) REPEAT160(macro, arg, INC_VALUE(n))
#define REPEAT162(macro, arg, n) macro(arg, n) REPEAT161(macro, arg, INC_VALUE(n))
#define REPEAT163(macro, arg, n) macro(arg, n) REPEAT162(macro, arg, INC_VALUE(n))
#define REPEAT164(macro, arg, n) macro(arg, n) REPEAT163(macro, arg, INC_VALUE(n))
#define REPEAT165(macro, arg, n) macro(arg, n) REPEAT164(macro, arg, INC_VALUE(n))
#define REPEAT166(macro, arg, n) macro(arg, n) REPEAT165(macro, arg, INC_VALUE(n))
#define REPEAT167(macro, arg, n) macro(arg, n) REPEAT166(macro, arg, INC_VALUE(n))
#define REPEAT168(macro, arg, n) macro(arg, n) REPEAT167(macro, arg, INC_VALUE(n))
#define REPEAT169(macro, arg, n) macro(arg, n) REPEAT168(macro, arg, INC_VALUE(n))
#define REPEAT170(macro, arg, n) macro(arg, n) REPEAT169(macro, arg, INC_VALUE(n))
#define REPEAT171(macro, arg, n) macro(arg, n) REPEAT170(macro, arg, INC_VALUE(n))
#define REPEAT172(macro, arg, n) macro(arg, n) REPEAT171(macro, arg, INC_VALUE(n))
#define REPEAT173(macro, arg, n) macro(arg, n) REPEAT172(macro, arg, INC_VALUE(n))
#define REPEAT174(macro, arg, n) macro(arg, n) REPEAT173(macro, arg, INC_VALUE(n))
#define REPEAT175(macro, arg, n) macro(arg, n) REPEAT174(macro, arg, INC_VALUE(n))
#define REPEAT176(macro, arg, n) macro(arg, n) REPEAT175(macro, arg, INC_VALUE(n))
#define REPEAT177(macro, arg, n) macro(arg, n) REPEAT176(macro, arg, INC_VALUE(n))
#define REPEAT178(macro, arg, n) macro(arg, n) REPEAT177(macro, arg, INC_VALUE(n))
#define REPEAT179(macro, arg, n) macro(arg, n) REPEAT178(macro, arg, INC_VALUE(n))
#define REPEAT180(macro, arg, n) macro(arg, n) REPEAT179(macro, arg, INC_VALUE(n))
#define REPEAT181(macro, arg, n) macro(arg, n) REPEAT180(macro, arg, INC_VALUE(n))
#define REPEAT182(macro, arg, n) macro(arg, n) REPEAT181(macro, arg, INC_VALUE(n))
#define REPEAT183(macro, arg, n) macro(arg, n) REPEAT182(macro, arg, INC_VALUE(n))
#define REPEAT184(macro, arg, n) macro(arg, n) REPEAT183(macro, arg, INC_VALUE(n))
#define REPEAT185(macro, arg, n) macro(arg, n) REPEAT184(macro, arg, INC_VALUE(n))
#define REPEAT186(macro, arg, n) macro(arg, n) REPEAT185(macro, arg, INC_VALUE(n))
#define REPEAT187(macro, arg, n) macro(arg, n) REPEAT186(macro, arg, INC_VALUE(n))
#define REPEAT188(macro, arg, n) macro(arg, n) REPEAT187(macro, arg, INC_VALUE(n))
#define REPEAT189(macro, arg, n) macro(arg, n) REPEAT188(macro, arg, INC_VALUE(n))
#define REPEAT190(macro, arg, n) macro(arg, n) REPEAT189(macro, arg, INC_VALUE(n))
#define REPEAT191(macro, arg, n) macro(arg, n) REPEAT190(macro, arg, INC_VALUE(n))
#define REPEAT192(macro, arg, n) macro(arg, n) REPEAT191(macro, arg, INC_VALUE(n))
#define REPEAT193(macro, arg, n) macro(arg, n) REPEAT192(macro, arg, INC_VALUE(n))
#define REPEAT194(macro, arg, n) macro(arg, n) REPEAT193(macro, arg, INC_VALUE(n))
#define REPEAT195(macro, arg, n) macro(arg, n) REPEAT194(macro, arg, INC_VALUE(n))
#define REPEAT196(macro, arg, n) macro(arg, n) REPEAT195(macro, arg, INC_VALUE(n))
#define REPEAT197(macro, arg, n) macro(arg, n) REPEAT196(macro, arg, INC_VALUE(n))
#define REPEAT198(macro, arg, n) macro(arg, n) REPEAT197(macro, arg, INC_VALUE(n))
#define REPEAT199(macro, arg, n) macro(arg, n) REPEAT198(macro, arg, INC_VALUE(n))
#define REPEAT200(macro, arg, n) macro(arg, n) REPEAT199(macro, arg, INC_VALUE(n))
#define REPEAT201(macro, arg, n) macro(arg, n) REPEAT200(macro, arg, INC_VALUE(n))
#define REPEAT202(macro, arg, n) macro(arg, n) REPEAT201(macro, arg, INC_VALUE(n))
#define REPEAT203(macro, arg, n) macro(arg, n) REPEAT202(macro, arg, INC_VALUE(n))
#define REPEAT204(macro, arg, n) macro(arg, n) REPEAT203(macro, arg, INC_VALUE(n))
#define REPEAT205(macro, arg, n) macro(arg, n) REPEAT204(macro, arg, INC_VALUE(n))
#define REPEAT206(macro, arg, n) macro(arg, n) REPEAT205(macro, arg, INC_VALUE(n))
#define REPEAT207(macro, arg, n) macro(arg, n) REPEAT206(macro, arg, INC_VALUE(n))
#define REPEAT208(macro, arg, n) macro(arg, n) REPEAT207(macro, arg, INC_VALUE(n))
#define REPEAT209(macro, arg, n) macro(arg, n) REPEAT208(macro, arg, INC_VALUE(n))
#define REPEAT210(macro, arg, n) macro(arg, n) REPEAT209(macro, arg, INC_VALUE(n))
#define REPEAT211(macro, arg, n) macro(arg, n) REPEAT210(macro, arg, INC_VALUE(n))
#define REPEAT212(macro, arg, n) macro(arg, n) REPEAT211(macro, arg, INC_VALUE(n))
#define REPEAT213(macro, arg, n) macro(arg, n) REPEAT212(macro, arg, INC_VALUE(n))
#define REPEAT214(macro, arg, n) macro(arg, n) REPEAT213(macro, arg, INC_VALUE(n))
#define REPEAT215(macro, arg, n) macro(arg, n) REPEAT214(macro, arg, INC_VALUE(n))
#define REPEAT216(macro, arg, n) macro(arg, n) REPEAT215(macro, arg, INC_VALUE(n))
#define REPEAT217(macro, arg, n) macro(arg, n) REPEAT216(macro, arg, INC_VALUE(n))
#define REPEAT218(macro, arg, n) macro(arg, n) REPEAT217(macro, arg, INC_VALUE(n))
#define REPEAT219(macro, arg, n) macro(arg, n) REPEAT218(macro, arg, INC_VALUE(n))
#define REPEAT220(macro, arg, n) macro(arg, n) REPEAT219(macro, arg, INC_VALUE(n))
#define REPEAT221(macro, arg, n) macro(arg, n) REPEAT220(macro, arg, INC_VALUE(n))
#define REPEAT222(macro, arg, n) macro(arg, n) REPEAT221(macro, arg, INC_VALUE(n))
#define REPEAT223(macro, arg, n) macro(arg, n) REPEAT222(macro, arg, INC_VALUE(n))
#define REPEAT224(macro, arg, n) macro(arg, n) REPEAT223(macro, arg, INC_VALUE(n))
#define REPEAT225(macro, arg, n) macro(arg, n) REPEAT224(macro, arg, INC_VALUE(n))
#define REPEAT226(macro, arg, n) macro(arg, n) REPEAT225(macro, arg, INC_VALUE(n))
#define REPEAT227(macro, arg, n) macro(arg, n) REPEAT226(macro, arg, INC_VALUE(n))
#define REPEAT228(macro, arg, n) macro(arg, n) REPEAT227(macro, arg, INC_VALUE(n))
#define REPEAT229(macro, arg, n) macro(arg, n) REPEAT228(macro, arg, INC_VALUE(n))
#define REPEAT230(macro, arg, n) macro(arg, n) REPEAT229(macro, arg, INC_VALUE(n))
#define REPEAT231(macro, arg, n) macro(arg, n) REPEAT230(macro, arg, INC_VALUE(n))
#define REPEAT232(macro, arg, n) macro(arg, n) REPEAT231(macro, arg, INC_VALUE(n))
#define REPEAT233(macro, arg, n) macro(arg, n) REPEAT232(macro, arg, INC_VALUE(n))
#define REPEAT234(macro, arg, n) macro(arg, n) REPEAT233(macro, arg, INC_VALUE(n))
#define REPEAT235(macro, arg, n) macro(arg, n) REPEAT234(macro, arg, INC_VALUE(n))
#define REPEAT236(macro, arg, n) macro(arg, n) REPEAT235(macro, arg, INC_VALUE(n))
#define REPEAT237(macro, arg, n) macro(arg, n) REPEAT236(macro, arg, INC_VALUE(n))
#define REPEAT238(macro, arg, n) macro(arg, n) REPEAT237(macro, arg, INC_VALUE(n))
#define REPEAT239(macro, arg, n) macro(arg, n) REPEAT238(macro, arg, INC_VALUE(n))
#define REPEAT240(macro, arg, n) macro(arg, n) REPEAT239(macro, arg, INC_VALUE(n))
#define REPEAT241(macro, arg, n) macro(arg, n) REPEAT240(macro, arg, INC_VALUE(n))
#define REPEAT242(macro, arg, n) macro(arg, n) REPEAT241(macro, arg, INC_VALUE(n))
#define REPEAT243(macro, arg, n) macro(arg, n) REPEAT242(macro, arg, INC_VALUE(n))
#define REPEAT244(macro, arg, n) macro(arg, n) REPEAT243(macro, arg, INC_VALUE(n))
#define REPEAT245(macro, arg, n) macro(arg, n) REPEAT244(macro, arg, INC_VALUE(n))
#define REPEAT246(macro, arg, n) macro(arg, n) REPEAT245(macro, arg, INC_VALUE(n))
#define REPEAT247(macro, arg, n) macro(arg, n) REPEAT246(macro, arg, INC_VALUE(n))
#define REPEAT248(macro, arg, n) macro(arg, n) REPEAT247(macro, arg, INC_VALUE(n))
#define REPEAT249(macro, arg, n) macro(arg, n) REPEAT248(macro, arg, INC_VALUE(n))
#define REPEAT250(macro, arg, n) macro(arg, n) REPEAT249(macro, arg, INC_VALUE(n))
#define REPEAT251(macro, arg, n) macro(arg, n) REPEAT250(macro, arg, INC_VALUE(n))
#define REPEAT252(macro, arg, n) macro(arg, n) REPEAT251(macro, arg, INC_VALUE(n))
#define REPEAT253(macro, arg, n) macro(arg, n) REPEAT252(macro, arg, INC_VALUE(n))
#define REPEAT254(macro, arg, n) macro(arg, n) REPEAT253(macro, arg, INC_VALUE(n))
#define REPEAT255(macro, arg, n) macro(arg, n) REPEAT254(macro, arg, INC_VALUE(n))

#ifdef __cplusplus
}
#endif

#include <utils_increment_macro.h>
#endif /* _UTILS_REPEAT_MACRO_H */
