
STATIC mp_obj_t _alphafunc(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t func = mp_obj_get_int_truncated(a0);
    uint32_t ref = mp_obj_get_int_truncated(a1);
    C4(self, ((9 << 24) | ((func & 7) << 8) | ((ref & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(alphafunc_obj, _alphafunc);

STATIC mp_obj_t _begin(mp_obj_t self , mp_obj_t a0) {
    uint32_t prim = mp_obj_get_int_truncated(a0);
    C4(self, ((31 << 24) | ((prim & 15)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(begin_obj, _begin);

STATIC mp_obj_t _bitmapextformat(mp_obj_t self , mp_obj_t a0) {
    uint32_t fmt = mp_obj_get_int_truncated(a0);
    C4(self, ((46 << 24) | (fmt & 65535))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bitmapextformat_obj, _bitmapextformat);

STATIC mp_obj_t _bitmaphandle(mp_obj_t self , mp_obj_t a0) {
    uint32_t handle = mp_obj_get_int_truncated(a0);
    C4(self, ((5 << 24) | ((handle & 31)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bitmaphandle_obj, _bitmaphandle);

STATIC mp_obj_t _bitmaplayouth(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t linestride = mp_obj_get_int_truncated(a0);
    uint32_t height = mp_obj_get_int_truncated(a1);
    C4(self, ((40 << 24) | (((linestride) & 3) << 2) | (((height) & 3)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaplayouth_obj, _bitmaplayouth);

STATIC mp_obj_t _bitmaplayout(size_t n_args, const mp_obj_t *args) {
    uint32_t format = mp_obj_get_int_truncated(args[1]);
    uint32_t linestride = mp_obj_get_int_truncated(args[2]);
    uint32_t height = mp_obj_get_int_truncated(args[3]);
    C4(args[0], ((7 << 24) | ((format & 31) << 19) | ((linestride & 1023) << 9) | ((height & 511)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitmaplayout_obj, 4, 4, _bitmaplayout);

STATIC mp_obj_t _bitmapsizeh(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t width = mp_obj_get_int_truncated(a0);
    uint32_t height = mp_obj_get_int_truncated(a1);
    C4(self, ((41 << 24) | (((width) & 3) << 2) | (((height) & 3)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmapsizeh_obj, _bitmapsizeh);

STATIC mp_obj_t _bitmapsize(size_t n_args, const mp_obj_t *args) {
    uint32_t filter = mp_obj_get_int_truncated(args[1]);
    uint32_t wrapx = mp_obj_get_int_truncated(args[2]);
    uint32_t wrapy = mp_obj_get_int_truncated(args[3]);
    uint32_t width = mp_obj_get_int_truncated(args[4]);
    uint32_t height = mp_obj_get_int_truncated(args[5]);
    C4(args[0], ((8 << 24) | ((filter & 1) << 20) | ((wrapx & 1) << 19) | ((wrapy & 1) << 18) | ((width & 511) << 9) | ((height & 511)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitmapsize_obj, 6, 6, _bitmapsize);

STATIC mp_obj_t _bitmapsource(mp_obj_t self , mp_obj_t a0) {
    uint32_t addr = mp_obj_get_int_truncated(a0);
    C4(self, ((1 << 24) | ((addr & 0xffffff)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bitmapsource_obj, _bitmapsource);

STATIC mp_obj_t _bitmapswizzle(size_t n_args, const mp_obj_t *args) {
    uint32_t r = mp_obj_get_int_truncated(args[1]);
    uint32_t g = mp_obj_get_int_truncated(args[2]);
    uint32_t b = mp_obj_get_int_truncated(args[3]);
    uint32_t a = mp_obj_get_int_truncated(args[4]);
    C4(args[0], ((47 << 24) | ((r & 7) << 9) | ((g & 7) << 6) | ((b & 7) << 3) | ((a & 7)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitmapswizzle_obj, 5, 5, _bitmapswizzle);

STATIC mp_obj_t _bitmaptransforma(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t a = mp_obj_get_int_truncated(a0);
    uint32_t p = mp_obj_get_int_truncated(a1);
    C4(self, ((21 << 24) | ((p & 1) << 17) | ((a & 131071)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransforma_obj, _bitmaptransforma);

STATIC mp_obj_t _bitmaptransformb(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t b = mp_obj_get_int_truncated(a0);
    uint32_t p = mp_obj_get_int_truncated(a1);
    C4(self, ((22 << 24) | ((p & 1) << 17) | ((b & 131071)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransformb_obj, _bitmaptransformb);

STATIC mp_obj_t _bitmaptransformc(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t c = mp_obj_get_int_truncated(a0);
    uint32_t p = mp_obj_get_int_truncated(a1);
    C4(self, ((23 << 24) | ((p & 1) << 17) | ((c & 16777215)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransformc_obj, _bitmaptransformc);

STATIC mp_obj_t _bitmaptransformd(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t d = mp_obj_get_int_truncated(a0);
    uint32_t p = mp_obj_get_int_truncated(a1);
    C4(self, ((24 << 24) | ((p & 1) << 17) | ((d & 131071)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransformd_obj, _bitmaptransformd);

STATIC mp_obj_t _bitmaptransforme(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t e = mp_obj_get_int_truncated(a0);
    uint32_t p = mp_obj_get_int_truncated(a1);
    C4(self, ((25 << 24) | ((p & 1) << 17) | ((e & 131071)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransforme_obj, _bitmaptransforme);

STATIC mp_obj_t _bitmaptransformf(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t f = mp_obj_get_int_truncated(a0);
    uint32_t p = mp_obj_get_int_truncated(a1);
    C4(self, ((26 << 24) | ((p & 1) << 17) | ((f & 16777215)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransformf_obj, _bitmaptransformf);

STATIC mp_obj_t _blendfunc(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t src = mp_obj_get_int_truncated(a0);
    uint32_t dst = mp_obj_get_int_truncated(a1);
    C4(self, ((11 << 24) | ((src & 7) << 3) | ((dst & 7)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(blendfunc_obj, _blendfunc);

STATIC mp_obj_t _call(mp_obj_t self , mp_obj_t a0) {
    uint32_t dest = mp_obj_get_int_truncated(a0);
    C4(self, ((29 << 24) | ((dest & 65535)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(call_obj, _call);

STATIC mp_obj_t _cell(mp_obj_t self , mp_obj_t a0) {
    uint32_t cell = mp_obj_get_int_truncated(a0);
    C4(self, ((6 << 24) | ((cell & 127)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cell_obj, _cell);

STATIC mp_obj_t _clearcolora(mp_obj_t self , mp_obj_t a0) {
    uint32_t alpha = mp_obj_get_int_truncated(a0);
    C4(self, ((15 << 24) | ((alpha & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(clearcolora_obj, _clearcolora);

STATIC mp_obj_t _clearcolorrgb(size_t n_args, const mp_obj_t *args) {
    uint32_t red = mp_obj_get_int_truncated(args[1]);
    uint32_t green = mp_obj_get_int_truncated(args[2]);
    uint32_t blue = mp_obj_get_int_truncated(args[3]);
    C4(args[0], ((2 << 24) | ((red & 255) << 16) | ((green & 255) << 8) | ((blue & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(clearcolorrgb_obj, 4, 4, _clearcolorrgb);

STATIC mp_obj_t _clear(size_t n_args, const mp_obj_t *args) {
    uint32_t c = mp_obj_get_int_truncated(args[1]);
    uint32_t s = mp_obj_get_int_truncated(args[2]);
    uint32_t t = mp_obj_get_int_truncated(args[3]);
    C4(args[0], ((38 << 24) | ((c & 1) << 2) | ((s & 1) << 1) | ((t & 1)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(clear_obj, 4, 4, _clear);

STATIC mp_obj_t _clearstencil(mp_obj_t self , mp_obj_t a0) {
    uint32_t s = mp_obj_get_int_truncated(a0);
    C4(self, ((17 << 24) | ((s & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(clearstencil_obj, _clearstencil);

STATIC mp_obj_t _cleartag(mp_obj_t self , mp_obj_t a0) {
    uint32_t s = mp_obj_get_int_truncated(a0);
    C4(self, ((18 << 24) | ((s & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cleartag_obj, _cleartag);

STATIC mp_obj_t _colora(mp_obj_t self , mp_obj_t a0) {
    uint32_t alpha = mp_obj_get_int_truncated(a0);
    C4(self, ((16 << 24) | ((alpha & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(colora_obj, _colora);

STATIC mp_obj_t _colormask(size_t n_args, const mp_obj_t *args) {
    uint32_t r = mp_obj_get_int_truncated(args[1]);
    uint32_t g = mp_obj_get_int_truncated(args[2]);
    uint32_t b = mp_obj_get_int_truncated(args[3]);
    uint32_t a = mp_obj_get_int_truncated(args[4]);
    C4(args[0], ((32 << 24) | ((r & 1) << 3) | ((g & 1) << 2) | ((b & 1) << 1) | ((a & 1)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(colormask_obj, 5, 5, _colormask);

STATIC mp_obj_t _colorrgb(size_t n_args, const mp_obj_t *args) {
    uint32_t red = mp_obj_get_int_truncated(args[1]);
    uint32_t green = mp_obj_get_int_truncated(args[2]);
    uint32_t blue = mp_obj_get_int_truncated(args[3]);
    C4(args[0], ((4 << 24) | ((red & 255) << 16) | ((green & 255) << 8) | ((blue & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(colorrgb_obj, 4, 4, _colorrgb);

STATIC mp_obj_t _display(mp_obj_t self ) {

    C4(self, ((0 << 24))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_obj, _display);

STATIC mp_obj_t _end(mp_obj_t self ) {

    C4(self, ((33 << 24))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(end_obj, _end);

STATIC mp_obj_t _jump(mp_obj_t self , mp_obj_t a0) {
    uint32_t dest = mp_obj_get_int_truncated(a0);
    C4(self, ((30 << 24) | ((dest & 65535)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(jump_obj, _jump);

STATIC mp_obj_t _linewidth(mp_obj_t self , mp_obj_t a0) {
    uint32_t width = mp_obj_get_int_truncated(a0);
    C4(self, ((14 << 24) | ((width & 4095)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(linewidth_obj, _linewidth);

STATIC mp_obj_t _macro(mp_obj_t self , mp_obj_t a0) {
    uint32_t m = mp_obj_get_int_truncated(a0);
    C4(self, ((37 << 24) | ((m & 1)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(macro_obj, _macro);

STATIC mp_obj_t _nop(mp_obj_t self ) {

    C4(self, ((45 << 24))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(nop_obj, _nop);

STATIC mp_obj_t _palettesource(mp_obj_t self , mp_obj_t a0) {
    uint32_t addr = mp_obj_get_int_truncated(a0);
    C4(self, ((42 << 24) | (((addr) & 4194303)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(palettesource_obj, _palettesource);

STATIC mp_obj_t _pointsize(mp_obj_t self , mp_obj_t a0) {
    uint32_t size = mp_obj_get_int_truncated(a0);
    C4(self, ((13 << 24) | ((size & 8191)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pointsize_obj, _pointsize);

STATIC mp_obj_t _restorecontext(mp_obj_t self ) {

    C4(self, ((35 << 24))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(restorecontext_obj, _restorecontext);

STATIC mp_obj_t _return(mp_obj_t self ) {

    C4(self, ((36 << 24))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(return_obj, _return);

STATIC mp_obj_t _savecontext(mp_obj_t self ) {

    C4(self, ((34 << 24))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(savecontext_obj, _savecontext);

STATIC mp_obj_t _scissorsize(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t width = mp_obj_get_int_truncated(a0);
    uint32_t height = mp_obj_get_int_truncated(a1);
    C4(self, ((28 << 24) | ((width & 4095) << 12) | ((height & 4095)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(scissorsize_obj, _scissorsize);

STATIC mp_obj_t _scissorxy(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t x = mp_obj_get_int_truncated(a0);
    uint32_t y = mp_obj_get_int_truncated(a1);
    C4(self, ((27 << 24) | ((x & 2047) << 11) | ((y & 2047)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(scissorxy_obj, _scissorxy);

STATIC mp_obj_t _stencilfunc(size_t n_args, const mp_obj_t *args) {
    uint32_t func = mp_obj_get_int_truncated(args[1]);
    uint32_t ref = mp_obj_get_int_truncated(args[2]);
    uint32_t mask = mp_obj_get_int_truncated(args[3]);
    C4(args[0], ((10 << 24) | ((func & 7) << 16) | ((ref & 255) << 8) | ((mask & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stencilfunc_obj, 4, 4, _stencilfunc);

STATIC mp_obj_t _stencilmask(mp_obj_t self , mp_obj_t a0) {
    uint32_t mask = mp_obj_get_int_truncated(a0);
    C4(self, ((19 << 24) | ((mask & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(stencilmask_obj, _stencilmask);

STATIC mp_obj_t _stencilop(mp_obj_t self , mp_obj_t a0, mp_obj_t a1) {
    uint32_t sfail = mp_obj_get_int_truncated(a0);
    uint32_t spass = mp_obj_get_int_truncated(a1);
    C4(self, ((12 << 24) | ((sfail & 7) << 3) | ((spass & 7)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(stencilop_obj, _stencilop);

STATIC mp_obj_t _tagmask(mp_obj_t self , mp_obj_t a0) {
    uint32_t mask = mp_obj_get_int_truncated(a0);
    C4(self, ((20 << 24) | ((mask & 1)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tagmask_obj, _tagmask);

STATIC mp_obj_t _tag(mp_obj_t self , mp_obj_t a0) {
    uint32_t s = mp_obj_get_int_truncated(a0);
    C4(self, ((3 << 24) | ((s & 255)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tag_obj, _tag);

STATIC mp_obj_t _vertextranslatex(mp_obj_t self , mp_obj_t a0) {
    uint32_t x = mp_obj_get_int_truncated(a0);
    C4(self, ((43 << 24) | (((x) & 131071)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vertextranslatex_obj, _vertextranslatex);

STATIC mp_obj_t _vertextranslatey(mp_obj_t self , mp_obj_t a0) {
    uint32_t y = mp_obj_get_int_truncated(a0);
    C4(self, ((44 << 24) | (((y) & 131071)))
);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vertextranslatey_obj, _vertextranslatey);
#define N_METHODS 47
#define METHOD_SETUP do { stem_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_AlphaFunc), MP_OBJ_FROM_PTR(&alphafunc_obj) }; stem_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Begin), MP_OBJ_FROM_PTR(&begin_obj) }; stem_locals_dict_table[2] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapExtFormat), MP_OBJ_FROM_PTR(&bitmapextformat_obj) }; stem_locals_dict_table[3] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapHandle), MP_OBJ_FROM_PTR(&bitmaphandle_obj) }; stem_locals_dict_table[4] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapLayoutH), MP_OBJ_FROM_PTR(&bitmaplayouth_obj) }; stem_locals_dict_table[5] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapLayout), MP_OBJ_FROM_PTR(&bitmaplayout_obj) }; stem_locals_dict_table[6] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapSizeH), MP_OBJ_FROM_PTR(&bitmapsizeh_obj) }; stem_locals_dict_table[7] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapSize), MP_OBJ_FROM_PTR(&bitmapsize_obj) }; stem_locals_dict_table[8] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapSource), MP_OBJ_FROM_PTR(&bitmapsource_obj) }; stem_locals_dict_table[9] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapSwizzle), MP_OBJ_FROM_PTR(&bitmapswizzle_obj) }; stem_locals_dict_table[10] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapTransformA), MP_OBJ_FROM_PTR(&bitmaptransforma_obj) }; stem_locals_dict_table[11] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapTransformB), MP_OBJ_FROM_PTR(&bitmaptransformb_obj) }; stem_locals_dict_table[12] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapTransformC), MP_OBJ_FROM_PTR(&bitmaptransformc_obj) }; stem_locals_dict_table[13] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapTransformD), MP_OBJ_FROM_PTR(&bitmaptransformd_obj) }; stem_locals_dict_table[14] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapTransformE), MP_OBJ_FROM_PTR(&bitmaptransforme_obj) }; stem_locals_dict_table[15] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BitmapTransformF), MP_OBJ_FROM_PTR(&bitmaptransformf_obj) }; stem_locals_dict_table[16] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_BlendFunc), MP_OBJ_FROM_PTR(&blendfunc_obj) }; stem_locals_dict_table[17] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Call), MP_OBJ_FROM_PTR(&call_obj) }; stem_locals_dict_table[18] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Cell), MP_OBJ_FROM_PTR(&cell_obj) }; stem_locals_dict_table[19] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ClearColorA), MP_OBJ_FROM_PTR(&clearcolora_obj) }; stem_locals_dict_table[20] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ClearColorRGB), MP_OBJ_FROM_PTR(&clearcolorrgb_obj) }; stem_locals_dict_table[21] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Clear), MP_OBJ_FROM_PTR(&clear_obj) }; stem_locals_dict_table[22] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ClearStencil), MP_OBJ_FROM_PTR(&clearstencil_obj) }; stem_locals_dict_table[23] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ClearTag), MP_OBJ_FROM_PTR(&cleartag_obj) }; stem_locals_dict_table[24] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ColorA), MP_OBJ_FROM_PTR(&colora_obj) }; stem_locals_dict_table[25] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ColorMask), MP_OBJ_FROM_PTR(&colormask_obj) }; stem_locals_dict_table[26] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ColorRGB), MP_OBJ_FROM_PTR(&colorrgb_obj) }; stem_locals_dict_table[27] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Display), MP_OBJ_FROM_PTR(&display_obj) }; stem_locals_dict_table[28] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_End), MP_OBJ_FROM_PTR(&end_obj) }; stem_locals_dict_table[29] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Jump), MP_OBJ_FROM_PTR(&jump_obj) }; stem_locals_dict_table[30] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_LineWidth), MP_OBJ_FROM_PTR(&linewidth_obj) }; stem_locals_dict_table[31] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Macro), MP_OBJ_FROM_PTR(&macro_obj) }; stem_locals_dict_table[32] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Nop), MP_OBJ_FROM_PTR(&nop_obj) }; stem_locals_dict_table[33] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_PaletteSource), MP_OBJ_FROM_PTR(&palettesource_obj) }; stem_locals_dict_table[34] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_PointSize), MP_OBJ_FROM_PTR(&pointsize_obj) }; stem_locals_dict_table[35] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_RestoreContext), MP_OBJ_FROM_PTR(&restorecontext_obj) }; stem_locals_dict_table[36] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Return), MP_OBJ_FROM_PTR(&return_obj) }; stem_locals_dict_table[37] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_SaveContext), MP_OBJ_FROM_PTR(&savecontext_obj) }; stem_locals_dict_table[38] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ScissorSize), MP_OBJ_FROM_PTR(&scissorsize_obj) }; stem_locals_dict_table[39] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_ScissorXY), MP_OBJ_FROM_PTR(&scissorxy_obj) }; stem_locals_dict_table[40] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_StencilFunc), MP_OBJ_FROM_PTR(&stencilfunc_obj) }; stem_locals_dict_table[41] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_StencilMask), MP_OBJ_FROM_PTR(&stencilmask_obj) }; stem_locals_dict_table[42] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_StencilOp), MP_OBJ_FROM_PTR(&stencilop_obj) }; stem_locals_dict_table[43] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_TagMask), MP_OBJ_FROM_PTR(&tagmask_obj) }; stem_locals_dict_table[44] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_Tag), MP_OBJ_FROM_PTR(&tag_obj) }; stem_locals_dict_table[45] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_VertexTranslateX), MP_OBJ_FROM_PTR(&vertextranslatex_obj) }; stem_locals_dict_table[46] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_VertexTranslateY), MP_OBJ_FROM_PTR(&vertextranslatey_obj) }; } while (0)
#define ROM_DECLS { MP_ROM_QSTR(MP_QSTR_AlphaFunc), MP_ROM_PTR(&alphafunc_obj) }, { MP_ROM_QSTR(MP_QSTR_Begin), MP_ROM_PTR(&begin_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapExtFormat), MP_ROM_PTR(&bitmapextformat_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapHandle), MP_ROM_PTR(&bitmaphandle_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapLayoutH), MP_ROM_PTR(&bitmaplayouth_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapLayout), MP_ROM_PTR(&bitmaplayout_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapSizeH), MP_ROM_PTR(&bitmapsizeh_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapSize), MP_ROM_PTR(&bitmapsize_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapSource), MP_ROM_PTR(&bitmapsource_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapSwizzle), MP_ROM_PTR(&bitmapswizzle_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapTransformA), MP_ROM_PTR(&bitmaptransforma_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapTransformB), MP_ROM_PTR(&bitmaptransformb_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapTransformC), MP_ROM_PTR(&bitmaptransformc_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapTransformD), MP_ROM_PTR(&bitmaptransformd_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapTransformE), MP_ROM_PTR(&bitmaptransforme_obj) }, { MP_ROM_QSTR(MP_QSTR_BitmapTransformF), MP_ROM_PTR(&bitmaptransformf_obj) }, { MP_ROM_QSTR(MP_QSTR_BlendFunc), MP_ROM_PTR(&blendfunc_obj) }, { MP_ROM_QSTR(MP_QSTR_Call), MP_ROM_PTR(&call_obj) }, { MP_ROM_QSTR(MP_QSTR_Cell), MP_ROM_PTR(&cell_obj) }, { MP_ROM_QSTR(MP_QSTR_ClearColorA), MP_ROM_PTR(&clearcolora_obj) }, { MP_ROM_QSTR(MP_QSTR_ClearColorRGB), MP_ROM_PTR(&clearcolorrgb_obj) }, { MP_ROM_QSTR(MP_QSTR_Clear), MP_ROM_PTR(&clear_obj) }, { MP_ROM_QSTR(MP_QSTR_ClearStencil), MP_ROM_PTR(&clearstencil_obj) }, { MP_ROM_QSTR(MP_QSTR_ClearTag), MP_ROM_PTR(&cleartag_obj) }, { MP_ROM_QSTR(MP_QSTR_ColorA), MP_ROM_PTR(&colora_obj) }, { MP_ROM_QSTR(MP_QSTR_ColorMask), MP_ROM_PTR(&colormask_obj) }, { MP_ROM_QSTR(MP_QSTR_ColorRGB), MP_ROM_PTR(&colorrgb_obj) }, { MP_ROM_QSTR(MP_QSTR_Display), MP_ROM_PTR(&display_obj) }, { MP_ROM_QSTR(MP_QSTR_End), MP_ROM_PTR(&end_obj) }, { MP_ROM_QSTR(MP_QSTR_Jump), MP_ROM_PTR(&jump_obj) }, { MP_ROM_QSTR(MP_QSTR_LineWidth), MP_ROM_PTR(&linewidth_obj) }, { MP_ROM_QSTR(MP_QSTR_Macro), MP_ROM_PTR(&macro_obj) }, { MP_ROM_QSTR(MP_QSTR_Nop), MP_ROM_PTR(&nop_obj) }, { MP_ROM_QSTR(MP_QSTR_PaletteSource), MP_ROM_PTR(&palettesource_obj) }, { MP_ROM_QSTR(MP_QSTR_PointSize), MP_ROM_PTR(&pointsize_obj) }, { MP_ROM_QSTR(MP_QSTR_RestoreContext), MP_ROM_PTR(&restorecontext_obj) }, { MP_ROM_QSTR(MP_QSTR_Return), MP_ROM_PTR(&return_obj) }, { MP_ROM_QSTR(MP_QSTR_SaveContext), MP_ROM_PTR(&savecontext_obj) }, { MP_ROM_QSTR(MP_QSTR_ScissorSize), MP_ROM_PTR(&scissorsize_obj) }, { MP_ROM_QSTR(MP_QSTR_ScissorXY), MP_ROM_PTR(&scissorxy_obj) }, { MP_ROM_QSTR(MP_QSTR_StencilFunc), MP_ROM_PTR(&stencilfunc_obj) }, { MP_ROM_QSTR(MP_QSTR_StencilMask), MP_ROM_PTR(&stencilmask_obj) }, { MP_ROM_QSTR(MP_QSTR_StencilOp), MP_ROM_PTR(&stencilop_obj) }, { MP_ROM_QSTR(MP_QSTR_TagMask), MP_ROM_PTR(&tagmask_obj) }, { MP_ROM_QSTR(MP_QSTR_Tag), MP_ROM_PTR(&tag_obj) }, { MP_ROM_QSTR(MP_QSTR_VertexTranslateX), MP_ROM_PTR(&vertextranslatex_obj) }, { MP_ROM_QSTR(MP_QSTR_VertexTranslateY), MP_ROM_PTR(&vertextranslatey_obj) }
