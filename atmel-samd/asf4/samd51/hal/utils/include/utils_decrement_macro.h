/**
 * \file
 *
 * \brief Decrement macro.
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

#ifndef _UTILS_DECREMENT_MACRO_H
#define _UTILS_DECREMENT_MACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Compile time decrement, result value is entire integer literal
 *
 * \param[in] val - value to be decremented
 */
#define DEC_VALUE(val) DEC_##val

// Preprocessor increment implementation
#define DEC_256 255
#define DEC_255 254
#define DEC_254 253
#define DEC_253 252
#define DEC_252 251
#define DEC_251 250
#define DEC_250 249
#define DEC_249 248
#define DEC_248 247
#define DEC_247 246
#define DEC_246 245
#define DEC_245 244
#define DEC_244 243
#define DEC_243 242
#define DEC_242 241
#define DEC_241 240
#define DEC_240 239
#define DEC_239 238
#define DEC_238 237
#define DEC_237 236
#define DEC_236 235
#define DEC_235 234
#define DEC_234 233
#define DEC_233 232
#define DEC_232 231
#define DEC_231 230
#define DEC_230 229
#define DEC_229 228
#define DEC_228 227
#define DEC_227 226
#define DEC_226 225
#define DEC_225 224
#define DEC_224 223
#define DEC_223 222
#define DEC_222 221
#define DEC_221 220
#define DEC_220 219
#define DEC_219 218
#define DEC_218 217
#define DEC_217 216
#define DEC_216 215
#define DEC_215 214
#define DEC_214 213
#define DEC_213 212
#define DEC_212 211
#define DEC_211 210
#define DEC_210 209
#define DEC_209 208
#define DEC_208 207
#define DEC_207 206
#define DEC_206 205
#define DEC_205 204
#define DEC_204 203
#define DEC_203 202
#define DEC_202 201
#define DEC_201 200
#define DEC_200 199
#define DEC_199 198
#define DEC_198 197
#define DEC_197 196
#define DEC_196 195
#define DEC_195 194
#define DEC_194 193
#define DEC_193 192
#define DEC_192 191
#define DEC_191 190
#define DEC_190 189
#define DEC_189 188
#define DEC_188 187
#define DEC_187 186
#define DEC_186 185
#define DEC_185 184
#define DEC_184 183
#define DEC_183 182
#define DEC_182 181
#define DEC_181 180
#define DEC_180 179
#define DEC_179 178
#define DEC_178 177
#define DEC_177 176
#define DEC_176 175
#define DEC_175 174
#define DEC_174 173
#define DEC_173 172
#define DEC_172 171
#define DEC_171 170
#define DEC_170 169
#define DEC_169 168
#define DEC_168 167
#define DEC_167 166
#define DEC_166 165
#define DEC_165 164
#define DEC_164 163
#define DEC_163 162
#define DEC_162 161
#define DEC_161 160
#define DEC_160 159
#define DEC_159 158
#define DEC_158 157
#define DEC_157 156
#define DEC_156 155
#define DEC_155 154
#define DEC_154 153
#define DEC_153 152
#define DEC_152 151
#define DEC_151 150
#define DEC_150 149
#define DEC_149 148
#define DEC_148 147
#define DEC_147 146
#define DEC_146 145
#define DEC_145 144
#define DEC_144 143
#define DEC_143 142
#define DEC_142 141
#define DEC_141 140
#define DEC_140 139
#define DEC_139 138
#define DEC_138 137
#define DEC_137 136
#define DEC_136 135
#define DEC_135 134
#define DEC_134 133
#define DEC_133 132
#define DEC_132 131
#define DEC_131 130
#define DEC_130 129
#define DEC_129 128
#define DEC_128 127
#define DEC_127 126
#define DEC_126 125
#define DEC_125 124
#define DEC_124 123
#define DEC_123 122
#define DEC_122 121
#define DEC_121 120
#define DEC_120 119
#define DEC_119 118
#define DEC_118 117
#define DEC_117 116
#define DEC_116 115
#define DEC_115 114
#define DEC_114 113
#define DEC_113 112
#define DEC_112 111
#define DEC_111 110
#define DEC_110 109
#define DEC_109 108
#define DEC_108 107
#define DEC_107 106
#define DEC_106 105
#define DEC_105 104
#define DEC_104 103
#define DEC_103 102
#define DEC_102 101
#define DEC_101 100
#define DEC_100 99
#define DEC_99 98
#define DEC_98 97
#define DEC_97 96
#define DEC_96 95
#define DEC_95 94
#define DEC_94 93
#define DEC_93 92
#define DEC_92 91
#define DEC_91 90
#define DEC_90 89
#define DEC_89 88
#define DEC_88 87
#define DEC_87 86
#define DEC_86 85
#define DEC_85 84
#define DEC_84 83
#define DEC_83 82
#define DEC_82 81
#define DEC_81 80
#define DEC_80 79
#define DEC_79 78
#define DEC_78 77
#define DEC_77 76
#define DEC_76 75
#define DEC_75 74
#define DEC_74 73
#define DEC_73 72
#define DEC_72 71
#define DEC_71 70
#define DEC_70 69
#define DEC_69 68
#define DEC_68 67
#define DEC_67 66
#define DEC_66 65
#define DEC_65 64
#define DEC_64 63
#define DEC_63 62
#define DEC_62 61
#define DEC_61 60
#define DEC_60 59
#define DEC_59 58
#define DEC_58 57
#define DEC_57 56
#define DEC_56 55
#define DEC_55 54
#define DEC_54 53
#define DEC_53 52
#define DEC_52 51
#define DEC_51 50
#define DEC_50 49
#define DEC_49 48
#define DEC_48 47
#define DEC_47 46
#define DEC_46 45
#define DEC_45 44
#define DEC_44 43
#define DEC_43 42
#define DEC_42 41
#define DEC_41 40
#define DEC_40 39
#define DEC_39 38
#define DEC_38 37
#define DEC_37 36
#define DEC_36 35
#define DEC_35 34
#define DEC_34 33
#define DEC_33 32
#define DEC_32 31
#define DEC_31 30
#define DEC_30 29
#define DEC_29 28
#define DEC_28 27
#define DEC_27 26
#define DEC_26 25
#define DEC_25 24
#define DEC_24 23
#define DEC_23 22
#define DEC_22 21
#define DEC_21 20
#define DEC_20 19
#define DEC_19 18
#define DEC_18 17
#define DEC_17 16
#define DEC_16 15
#define DEC_15 14
#define DEC_14 13
#define DEC_13 12
#define DEC_12 11
#define DEC_11 10
#define DEC_10 9
#define DEC_9 8
#define DEC_8 7
#define DEC_7 6
#define DEC_6 5
#define DEC_5 4
#define DEC_4 3
#define DEC_3 2
#define DEC_2 1
#define DEC_1 0

#ifdef __cplusplus
}
#endif
#endif /* _UTILS_DECREMENT_MACRO_H */
