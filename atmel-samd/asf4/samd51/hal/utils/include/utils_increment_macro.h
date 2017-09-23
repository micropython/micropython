/**
 * \file
 *
 * \brief Increment macro.
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

#ifndef _UTILS_INCREMENT_MACRO_H
#define _UTILS_INCREMENT_MACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Compile time increment, result value is entire integer literal
 *
 * \param[in] val - value to be incremented (254 max)
 */
#define INC_VALUE(val) SP_INC_##val

// Preprocessor increment implementation
#define SP_INC_0 1
#define SP_INC_1 2
#define SP_INC_2 3
#define SP_INC_3 4
#define SP_INC_4 5
#define SP_INC_5 6
#define SP_INC_6 7
#define SP_INC_7 8
#define SP_INC_8 9
#define SP_INC_9 10
#define SP_INC_10 11
#define SP_INC_11 12
#define SP_INC_12 13
#define SP_INC_13 14
#define SP_INC_14 15
#define SP_INC_15 16
#define SP_INC_16 17
#define SP_INC_17 18
#define SP_INC_18 19
#define SP_INC_19 20
#define SP_INC_20 21
#define SP_INC_21 22
#define SP_INC_22 23
#define SP_INC_23 24
#define SP_INC_24 25
#define SP_INC_25 26
#define SP_INC_26 27
#define SP_INC_27 28
#define SP_INC_28 29
#define SP_INC_29 30
#define SP_INC_30 31
#define SP_INC_31 32
#define SP_INC_32 33
#define SP_INC_33 34
#define SP_INC_34 35
#define SP_INC_35 36
#define SP_INC_36 37
#define SP_INC_37 38
#define SP_INC_38 39
#define SP_INC_39 40
#define SP_INC_40 41
#define SP_INC_41 42
#define SP_INC_42 43
#define SP_INC_43 44
#define SP_INC_44 45
#define SP_INC_45 46
#define SP_INC_46 47
#define SP_INC_47 48
#define SP_INC_48 49
#define SP_INC_49 50
#define SP_INC_50 51
#define SP_INC_51 52
#define SP_INC_52 53
#define SP_INC_53 54
#define SP_INC_54 55
#define SP_INC_55 56
#define SP_INC_56 57
#define SP_INC_57 58
#define SP_INC_58 59
#define SP_INC_59 60
#define SP_INC_60 61
#define SP_INC_61 62
#define SP_INC_62 63
#define SP_INC_63 64
#define SP_INC_64 65
#define SP_INC_65 66
#define SP_INC_66 67
#define SP_INC_67 68
#define SP_INC_68 69
#define SP_INC_69 70
#define SP_INC_70 71
#define SP_INC_71 72
#define SP_INC_72 73
#define SP_INC_73 74
#define SP_INC_74 75
#define SP_INC_75 76
#define SP_INC_76 77
#define SP_INC_77 78
#define SP_INC_78 79
#define SP_INC_79 80
#define SP_INC_80 81
#define SP_INC_81 82
#define SP_INC_82 83
#define SP_INC_83 84
#define SP_INC_84 85
#define SP_INC_85 86
#define SP_INC_86 87
#define SP_INC_87 88
#define SP_INC_88 89
#define SP_INC_89 90
#define SP_INC_90 91
#define SP_INC_91 92
#define SP_INC_92 93
#define SP_INC_93 94
#define SP_INC_94 95
#define SP_INC_95 96
#define SP_INC_96 97
#define SP_INC_97 98
#define SP_INC_98 99
#define SP_INC_99 100
#define SP_INC_100 101
#define SP_INC_101 102
#define SP_INC_102 103
#define SP_INC_103 104
#define SP_INC_104 105
#define SP_INC_105 106
#define SP_INC_106 107
#define SP_INC_107 108
#define SP_INC_108 109
#define SP_INC_109 110
#define SP_INC_110 111
#define SP_INC_111 112
#define SP_INC_112 113
#define SP_INC_113 114
#define SP_INC_114 115
#define SP_INC_115 116
#define SP_INC_116 117
#define SP_INC_117 118
#define SP_INC_118 119
#define SP_INC_119 120
#define SP_INC_120 121
#define SP_INC_121 122
#define SP_INC_122 123
#define SP_INC_123 124
#define SP_INC_124 125
#define SP_INC_125 126
#define SP_INC_126 127
#define SP_INC_127 128
#define SP_INC_128 129
#define SP_INC_129 130
#define SP_INC_130 131
#define SP_INC_131 132
#define SP_INC_132 133
#define SP_INC_133 134
#define SP_INC_134 135
#define SP_INC_135 136
#define SP_INC_136 137
#define SP_INC_137 138
#define SP_INC_138 139
#define SP_INC_139 140
#define SP_INC_140 141
#define SP_INC_141 142
#define SP_INC_142 143
#define SP_INC_143 144
#define SP_INC_144 145
#define SP_INC_145 146
#define SP_INC_146 147
#define SP_INC_147 148
#define SP_INC_148 149
#define SP_INC_149 150
#define SP_INC_150 151
#define SP_INC_151 152
#define SP_INC_152 153
#define SP_INC_153 154
#define SP_INC_154 155
#define SP_INC_155 156
#define SP_INC_156 157
#define SP_INC_157 158
#define SP_INC_158 159
#define SP_INC_159 160
#define SP_INC_160 161
#define SP_INC_161 162
#define SP_INC_162 163
#define SP_INC_163 164
#define SP_INC_164 165
#define SP_INC_165 166
#define SP_INC_166 167
#define SP_INC_167 168
#define SP_INC_168 169
#define SP_INC_169 170
#define SP_INC_170 171
#define SP_INC_171 172
#define SP_INC_172 173
#define SP_INC_173 174
#define SP_INC_174 175
#define SP_INC_175 176
#define SP_INC_176 177
#define SP_INC_177 178
#define SP_INC_178 179
#define SP_INC_179 180
#define SP_INC_180 181
#define SP_INC_181 182
#define SP_INC_182 183
#define SP_INC_183 184
#define SP_INC_184 185
#define SP_INC_185 186
#define SP_INC_186 187
#define SP_INC_187 188
#define SP_INC_188 189
#define SP_INC_189 190
#define SP_INC_190 191
#define SP_INC_191 192
#define SP_INC_192 193
#define SP_INC_193 194
#define SP_INC_194 195
#define SP_INC_195 196
#define SP_INC_196 197
#define SP_INC_197 198
#define SP_INC_198 199
#define SP_INC_199 200
#define SP_INC_200 201
#define SP_INC_201 202
#define SP_INC_202 203
#define SP_INC_203 204
#define SP_INC_204 205
#define SP_INC_205 206
#define SP_INC_206 207
#define SP_INC_207 208
#define SP_INC_208 209
#define SP_INC_209 210
#define SP_INC_210 211
#define SP_INC_211 212
#define SP_INC_212 213
#define SP_INC_213 214
#define SP_INC_214 215
#define SP_INC_215 216
#define SP_INC_216 217
#define SP_INC_217 218
#define SP_INC_218 219
#define SP_INC_219 220
#define SP_INC_220 221
#define SP_INC_221 222
#define SP_INC_222 223
#define SP_INC_223 224
#define SP_INC_224 225
#define SP_INC_225 226
#define SP_INC_226 227
#define SP_INC_227 228
#define SP_INC_228 229
#define SP_INC_229 230
#define SP_INC_230 231
#define SP_INC_231 232
#define SP_INC_232 233
#define SP_INC_233 234
#define SP_INC_234 235
#define SP_INC_235 236
#define SP_INC_236 237
#define SP_INC_237 238
#define SP_INC_238 239
#define SP_INC_239 240
#define SP_INC_240 241
#define SP_INC_241 242
#define SP_INC_242 243
#define SP_INC_243 244
#define SP_INC_244 245
#define SP_INC_245 246
#define SP_INC_246 247
#define SP_INC_247 248
#define SP_INC_248 249
#define SP_INC_249 250
#define SP_INC_250 251
#define SP_INC_251 252
#define SP_INC_252 253
#define SP_INC_253 254
#define SP_INC_254 255

#ifdef __cplusplus
}
#endif
#endif /* _UTILS_INCREMENT_MACRO_H */
