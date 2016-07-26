/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include STM32_HAL_H

#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_HW_HAS_UGFX

#include "pin.h"
//#include "ugfx_containers.h"
#include "modugfx.h"

#include "genhdr/pins.h"
#include "bufhelper.h"

/// \moduleref ugfx
/// UPDATE ME



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////     Primitives   ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


static GHandle get_ugfx_handle(mp_obj_t in){
	if (MP_OBJ_IS_TYPE(in,&ugfx_container_type))
		return ((ugfx_container_obj_t *)in)->ghContainer;
	else
		return 0;
}

/// \method text(x, y, str, colour)
///
/// Draw the given text to the position `(x, y)` using the given colour.
///
STATIC mp_obj_t ugfx_text(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    mp_uint_t len;
    const char *data = mp_obj_str_get_data(args[3], &len);
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
    int col = mp_obj_get_int(args[4]);


	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinDrawString(gh,x0, y0, data);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_text_obj, 5, 5, ugfx_text);


/// \method line(x1, y1, x2, y2, colour)
///
/// Draw a line from (x1,y1) to (x2,y2) using the given colour.
///
STATIC mp_obj_t ugfx_line(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int x1 = mp_obj_get_int(args[3]);
    int y1 = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);

	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinDrawLine(gh,x0, y0, x1, y1);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_line_obj, 6, 6, ugfx_line);


/// \method thickline(x1, y1, x2, y2, colour, width, round)
///
/// Draw a line with a given thickness from (x1,y1) to (x2,y2) using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_thickline(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int x1 = mp_obj_get_int(args[3]);
    int y1 = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);
    int width = mp_obj_get_int(args[6]);
    bool rnd = (mp_obj_get_int(args[7]) != 0);

	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinDrawThickLine(gh,x0, y0, x1, y1, width, rnd);


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_thickline_obj, 8, 8, ugfx_thickline);

/// \method arc(x1, y1, r, angle1, angle2, colour)
///
/// Draw an arc having a centre point at (x1,y1), radius r, using the given colour.
///
STATIC mp_obj_t ugfx_arc(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int r = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[6]);
    int a1 = mp_obj_get_int(args[4]);
    int a2 = mp_obj_get_int(args[5]);

	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinDrawArc(gh,x0, y0, r, a1, a2);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_arc_obj, 7, 7, ugfx_arc);

/// \method fill_arc(x1, y1, r, angle1, angle2, colour)
///
/// Fill an arc having a centre point at (x1,y1), radius r, using the given colour.
///
STATIC mp_obj_t ugfx_fill_arc(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int r = mp_obj_get_int(args[3]);
	int col = mp_obj_get_int(args[6]);
    int a1 = mp_obj_get_int(args[4]);
    int a2 = mp_obj_get_int(args[5]);

	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinFillArc(gh,x0, y0, r, a1, a2);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_fill_arc_obj, 7, 7, ugfx_fill_arc);


/// \method circle(x1, y1, r, colour)
///
/// Draw a circle having a centre point at (x1,y1), radius r, using the given colour.
///
STATIC mp_obj_t ugfx_circle(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int r = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);

	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinDrawCircle(gh,x0, y0, r);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_circle_obj, 5, 5, ugfx_circle);

/// \method fill_circle(x1, y1, r, colour)
///
/// Fill a circle having a centre point at (x1,y1), radius r, using the given colour.
///
STATIC mp_obj_t ugfx_fill_circle(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int r = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);


	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinFillCircle(gh,x0, y0, r);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_fill_circle_obj, 5, 5, ugfx_fill_circle);



/// \method ellipse(x1, y1, a, b, colour)
///
/// Draw a ellipse having a centre point at (x1,y1), lengths a,b, using the given colour.
///
STATIC mp_obj_t ugfx_ellipse(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);

	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinDrawEllipse(gh,x0, y0, a, b);


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_ellipse_obj, 6, 6, ugfx_ellipse);

/// \method fill_ellipse(x1, y1, a, b, colour)
///
/// Fill a ellipse having a centre point at (x1,y1), lengths a,b, using the given colour.
///
STATIC mp_obj_t ugfx_fill_ellipse(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);

	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinFillEllipse(gh,x0, y0, a, b);


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_fill_ellipse_obj, 6, 6, ugfx_fill_ellipse);

/// \method area(x1, y1, a, b, colour)
///
/// Fill area from (x,y), with lengths x1,y1, using the given colour.
///
STATIC mp_obj_t ugfx_area(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);

	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinFillArea(gh,x0, y0, a, b);


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_area_obj, 6, 6, ugfx_area);

/// \method box(x1, y1, a, b, colour)
///
/// Draw a box from (x,y), with lengths x1,y1, using the given colour.
///
STATIC mp_obj_t ugfx_box(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);

	GHandle gh = get_ugfx_handle(args[0]);
	gwinSetColor(gh,col);
	gwinDrawBox(gh,x0, y0, a, b);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_box_obj, 6, 6, ugfx_box);


/// \method polygon(x1, y1, array, colour)
///
/// Draw a polygon starting at (x1,y1), using the aarray of points, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_polygon(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int col = mp_obj_get_int(args[4]);

	point ar[20];

	mp_obj_t *mp_arr;
	mp_obj_t *mp_arr2;
	uint len;
	uint len2;
	mp_obj_get_array(args[3], &len, &mp_arr);

	if (len <= 20){
		int i,j;
		j = 0;
		for (i = 0; i < len; i++){
			mp_obj_get_array(mp_arr[i], &len2, &mp_arr2);
			if (len2 == 2){
				point p = {mp_obj_get_int(mp_arr2[0]), mp_obj_get_int(mp_arr2[1])};
				ar[j++] = p;
			}
		}
		GHandle gh = get_ugfx_handle(args[0]);
		gwinSetColor(gh,col);
		gwinDrawPoly(gh, x0, y0, ar, i);
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_polygon_obj, 5, 5, ugfx_polygon);


/// \method fill_polygon(x1, y1, array, colour)
///
/// Draw a polygon starting at (x1,y1), using the aarray of points, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t ugfx_fill_polygon(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int col = mp_obj_get_int(args[4]);

	point ar[20];

	mp_obj_t *mp_arr;
	mp_obj_t *mp_arr2;
	uint len;
	uint len2;
	mp_obj_get_array(args[3], &len, &mp_arr);

	if (len <= 20){
		int i,j;
		j = 0;
		for (i = 0; i < len; i++){
			mp_obj_get_array(mp_arr[i], &len2, &mp_arr2);
			if (len2 == 2){
				point p = {mp_obj_get_int(mp_arr2[0]), mp_obj_get_int(mp_arr2[1])};
				ar[j++] = p;
			}
		}
		GHandle gh = get_ugfx_handle(args[0]);
		gwinSetColor(gh,col);
		gwinFillConvexPoly(gh, x0, y0, ar, i);
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_fill_polygon_obj, 5, 5, ugfx_fill_polygon);


/// \method get_width()
///
/// Gets current width of the screen in pixels
///
STATIC mp_obj_t ugfx_containers_get_width(mp_obj_t self_in) {
    return mp_obj_new_int(gwinGetWidth(get_ugfx_handle(self_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_containers_get_width_obj, ugfx_containers_get_width);



/// \method get_height()
///
/// Gets current width of the screen in pixels
///
STATIC mp_obj_t ugfx_containers_get_height(mp_obj_t self_in) {
    return mp_obj_new_int(gwinGetHeight(get_ugfx_handle(self_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_containers_get_height_obj, ugfx_containers_get_height);




/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////     Container    ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


/// \classmethod \constructor(x, y, a, b, text, {style})
///
/// Construct an Container object. Need to call .show() after creation
STATIC mp_obj_t ugfx_container_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 4, 5, false);


    //const char *text = mp_obj_str_get_str(args[4]);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);

    // create container object
    ugfx_container_obj_t *ctr = m_new_obj(ugfx_container_obj_t);
    ctr->base.type = &ugfx_container_type;


	//setup container options
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);

	wi.g.show = FALSE;

	// Apply the container parameters
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
	//wi.g.parent = ;
	wi.text = 0;//text;

	if (n_args == 5){
		ugfx_style_obj_t *st = args[4];
		if (MP_OBJ_IS_TYPE(args[4], &ugfx_style_type)){
			wi.customStyle = &(st->style);
			ctr->style = &(st->style);
		}
		else
			nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires a 'Style' object as input"));
	}
	else
		ctr->style = 0;

	// Create the actual container
	ctr->ghContainer = gwinContainerCreate(NULL, &wi, GWIN_CONTAINER_BORDER);


	return ctr;
}

/*
/// \method set_style()
///
/// sets the style used to draw new objects
STATIC mp_obj_t ugfx_container_set_style(mp_obj_t self_in, mp_obj_t style) {
    ugfx_container_obj_t *self = self_in;
	ugfx_style_obj_t *st = style;

	if (MP_OBJ_IS_TYPE(style, &ugfx_style_type)){
		GWidgetObject *gw = &(GWidgetObject)(self->ghContainer);
		gw->pstyle = &(st->style);
	}
	else
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires a 'Style' object as input"));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_container_set_style_obj, ugfx_container_set_style);
*/

/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_container_destroy(mp_obj_t self_in) {
    ugfx_container_obj_t *self = self_in;

	gwinDestroy(self->ghContainer);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_container_destroy_obj, ugfx_container_destroy);

/// \method show()
///
/// shows the container and all its children
STATIC mp_obj_t ugfx_container_show(mp_obj_t self_in) {
    ugfx_container_obj_t *self = self_in;

        gwinShow(self->ghContainer);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_container_show_obj, ugfx_container_show);

/// \method hide()
///
/// hides the container and all its children
STATIC mp_obj_t ugfx_container_hide(mp_obj_t self_in) {
    ugfx_container_obj_t *self = self_in;

        gwinHide(self->ghContainer);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_container_hide_obj, ugfx_container_hide);


STATIC const mp_map_elem_t ugfx_container_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_container_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_container_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_show), (mp_obj_t)&ugfx_container_show_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_hide), (mp_obj_t)&ugfx_container_hide_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_height), (mp_obj_t)&ugfx_containers_get_height_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_width), (mp_obj_t)&ugfx_containers_get_width_obj},

	{ MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&ugfx_text_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_line), (mp_obj_t)&ugfx_line_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_box), (mp_obj_t)&ugfx_box_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_area), (mp_obj_t)&ugfx_area_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_thickline), (mp_obj_t)&ugfx_thickline_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_circle), (mp_obj_t)&ugfx_circle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill_circle), (mp_obj_t)&ugfx_fill_circle_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_ellipse), (mp_obj_t)&ugfx_ellipse_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill_ellipse), (mp_obj_t)&ugfx_fill_ellipse_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_arc), (mp_obj_t)&ugfx_arc_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_fill_arc), (mp_obj_t)&ugfx_fill_arc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_polygon), (mp_obj_t)&ugfx_polygon_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill_polygon), (mp_obj_t)&ugfx_fill_polygon_obj },


    //{ MP_OBJ_NEW_QSTR(MP_QSTR_set_style), (mp_obj_t)&ugfx_container_set_style_obj},
	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_container_locals_dict, ugfx_container_locals_dict_table);

const mp_obj_type_t ugfx_container_type = {
    { &mp_type_type },
    .name = MP_QSTR_Container,
    .make_new = ugfx_container_make_new,
    .locals_dict = (mp_obj_t)&ugfx_container_locals_dict,
};




/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////       Graph      ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct _ugfx_graph_t {
    mp_obj_base_t base;

	GHandle ghGraph;
	GGraphStyle gStyle;
	GGraphObject gGObject;

} ugfx_graph_obj_t;

/// \classmethod \constructor(x, y, a, b, text, {style})
///
/// Construct an Graph object. Need to call .show() after creation
STATIC mp_obj_t ugfx_graph_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 6, 6, false);

    // create graph object
    ugfx_graph_obj_t *ctr = m_new_obj(ugfx_graph_obj_t);
    ctr->base.type = &ugfx_graph_type;

	//const char *text = mp_obj_str_get_str(args[4]);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);

	//setup graph options
	GWindowInit	wi;

	wi.show = FALSE;

	// Apply the graph parameters
	wi.width = a;
	wi.height = b;
	wi.y = y;
	wi.x = x;
	wi.parent = 0;

	GGraphStyle gs = {
		{ GGRAPH_POINT_SQUARE, 5, Red },        // Point
		{ GGRAPH_LINE_DOT, 2, Pink },           // Line
		{ GGRAPH_LINE_SOLID, 0, Black },        // X axis
		{ GGRAPH_LINE_SOLID, 0, Black },        // Y axis
		{ GGRAPH_LINE_DASH, 5, Gray, 50 },      // X grid
		{ GGRAPH_LINE_DOT, 7, Gray, 50 },     // Y grid
		GWIN_GRAPH_STYLE_POSITIVE_AXIS_ARROWS   // Flags
	};

	ctr->gStyle = gs;


	// Create the actual graph
	ctr->ghGraph = gwinGraphCreate(&(ctr->gGObject), &wi);

	gwinGraphSetOrigin(ctr->ghGraph, mp_obj_get_int(args[4]), mp_obj_get_int(args[5]));
    gwinGraphSetStyle(ctr->ghGraph, &ctr->gStyle);

	return ctr;
}


/// \method origin()
///
/// sets the origin, in pixels relative to the top right corner
STATIC mp_obj_t ugfx_graph_origin(mp_obj_t self_in, mp_obj_t x, mp_obj_t y) {
    ugfx_graph_obj_t *self = self_in;

	gwinGraphSetOrigin(self->ghGraph, mp_obj_get_int(x), mp_obj_get_int(y));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ugfx_graph_origin_obj, ugfx_graph_origin);


/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_graph_destroy(mp_obj_t self_in) {
    ugfx_graph_obj_t *self = self_in;

	gwinDestroy(self->ghGraph);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_graph_destroy_obj, ugfx_graph_destroy);

/// \method show()
///
/// shows the graph and all its children
STATIC mp_obj_t ugfx_graph_show(mp_obj_t self_in) {
    ugfx_graph_obj_t *self = self_in;

    gwinShow(self->ghGraph);
	gwinGraphDrawAxis(self->ghGraph);


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_graph_show_obj, ugfx_graph_show);

/// \method hide()
///
/// shows the graph and all its children
STATIC mp_obj_t ugfx_graph_hide(mp_obj_t self_in) {
    ugfx_graph_obj_t *self = self_in;

        gwinHide(self->ghGraph);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_graph_hide_obj, ugfx_graph_hide);

/// \method set_arrows(flags)
///
/// Sets which axes have arrows and what direction
///
/// Avaliable flags: ARROWS_X_POS, ARROWS_Y_POS, ARROWS_X_NEG, ARROWS_Y_NEG
///
/// Example usage: set_arrows(ARROWS_X_POS | ARROWS_X_NEG)
///
STATIC mp_obj_t ugfx_graph_set_arrows(mp_obj_t self_in, mp_obj_t flags) {
    ugfx_graph_obj_t *self = self_in;

    self->gStyle.flags = mp_obj_get_int(flags);
	gwinGraphSetStyle(self->ghGraph, &self->gStyle);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_graph_set_arrows_obj, ugfx_graph_set_arrows);


/// \method plot( point(s)_x, point(s)_y, {new_series} )
///
/// Plot either an array, or a point. Optional second parameter specifies whether
///    to start a new series or join onto previous
STATIC mp_obj_t ugfx_graph_plot(mp_uint_t n_args, const mp_obj_t *args) {
	if (n_args < 3)
		return mp_const_none;
    ugfx_graph_obj_t *self = args[0];

	mp_obj_t *itemsx;
	mp_obj_t *itemsy;
	mp_uint_t len1;
	mp_uint_t len2;

	if (n_args == 4){
		if (mp_obj_get_int(args[3]))
			gwinGraphStartSet(self->ghGraph);
	}

	//if is array...
	if (MP_OBJ_IS_INT(args[1]) && MP_OBJ_IS_INT(args[2]))
		gwinGraphDrawPoint(self->ghGraph, mp_obj_get_int(args[1]),mp_obj_get_int(args[2]));
	else
	{
		mp_obj_get_array(args[1], &len1, &itemsx);
		mp_obj_get_array(args[2], &len2, &itemsy);

		if (len1 != len2)
			nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires x and y to be the same length"));

		int i;
		for (i = 0; i < len1; i++){
			gwinGraphDrawPoint(self->ghGraph, mp_obj_get_int(itemsx[i]), mp_obj_get_int(itemsy[i]));
		}
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_graph_plot_obj, 3, 4, ugfx_graph_plot);


/// \method set_style( thing_to_change, shape, size, colour, {spacing}  )
///
/// Changes the style of either points, lines, axis or grids.
///
///   thing_to_change = {STYLE_POINT, STYLE_LINE, STYLE_XAXIS, STYLE_YAXIS, STYLE_XGRID, STYLE_YGRID}
///
///   shape = point options: {POINT_NONE, POINT_DOT, POINT_SQUARE, POINT_CIRCLE}
///           line/axis/grid options: {LINE_NONE, LINE_SOLID, LINE_DOT, LINE_DASH}
///
///   size = number of pixels wide
///
///   colour = use UGFX.Red, UGFX.Orange,...  or UGFX.html_color(0xRRGGBB) to ensure the right format
///
///   spacing = only valid for grid
STATIC mp_obj_t ugfx_graph_set_style(mp_uint_t n_args, const mp_obj_t *args) {
	if (n_args < 5)
		return mp_const_none;
    ugfx_graph_obj_t *self = args[0];

	GGraphLineStyle ls;
	GGraphGridStyle gds;
	GGraphPointStyle gps;

	gps.type = ls.type = gds.type = mp_obj_get_int(args[2]);
	gps.size = ls.size = gds.size = mp_obj_get_int(args[3]);
	gps.color = ls.color = gds.color = mp_obj_get_int(args[4]);

	int item = mp_obj_get_int(args[1]);

	switch(item){
		case 0:  //point
			self->gStyle.point = gps;
			gwinGraphSetStyle(self->ghGraph, &self->gStyle);
			break;
		case 1:	  //line
			self->gStyle.line = ls;
			gwinGraphSetStyle(self->ghGraph, &self->gStyle);
			break;
		case 2:	  //x axis
			self->gStyle.xaxis = ls;
			gwinGraphSetStyle(self->ghGraph, &self->gStyle);
			break;
		case 3:	  //y axis
			self->gStyle.yaxis = ls;
			gwinGraphSetStyle(self->ghGraph, &self->gStyle);
			break;
		case 4:	  //x grid
			if (n_args < 6)
				nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires grid spacing"));
			gds.spacing = mp_obj_get_int(args[5]);
			self->gStyle.xgrid = gds;
			gwinGraphSetStyle(self->ghGraph, &self->gStyle);
			break;
		case 5:	  //y grid
			if (n_args < 6)
				nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires grid spacing"));
			gds.spacing = mp_obj_get_int(args[5]);
			self->gStyle.ygrid = gds;
			gwinGraphSetStyle(self->ghGraph, &self->gStyle);
			break;
		default:
			break;
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_graph_set_style_obj, 5, 6, ugfx_graph_set_style);





STATIC const mp_map_elem_t ugfx_graph_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_graph_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_graph_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_show), (mp_obj_t)&ugfx_graph_show_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_hide), (mp_obj_t)&ugfx_graph_hide_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_origin), (mp_obj_t)&ugfx_graph_origin_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_plot), (mp_obj_t)&ugfx_graph_plot_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_appearance), (mp_obj_t)&ugfx_graph_set_style_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_arrows), (mp_obj_t)&ugfx_graph_set_arrows_obj},
	{ MP_OBJ_NEW_QSTR(MP_QSTR_height), (mp_obj_t)&ugfx_containers_get_height},
    { MP_OBJ_NEW_QSTR(MP_QSTR_width), (mp_obj_t)&ugfx_containers_get_width},


	//class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_STYLE_POINT),        MP_OBJ_NEW_SMALL_INT(0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STYLE_LINE),         MP_OBJ_NEW_SMALL_INT(1) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STYLE_XAXIS),        MP_OBJ_NEW_SMALL_INT(2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STYLE_YAXIS),        MP_OBJ_NEW_SMALL_INT(3) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STYLE_XGRID),        MP_OBJ_NEW_SMALL_INT(4) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STYLE_YGRID),        MP_OBJ_NEW_SMALL_INT(5) },

	{ MP_OBJ_NEW_QSTR(MP_QSTR_POINT_NONE),         MP_OBJ_NEW_SMALL_INT(GGRAPH_POINT_NONE) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_POINT_DOT),          MP_OBJ_NEW_SMALL_INT(GGRAPH_POINT_DOT) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_POINT_SQUARE),       MP_OBJ_NEW_SMALL_INT(GGRAPH_POINT_SQUARE) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_POINT_CIRCLE),       MP_OBJ_NEW_SMALL_INT(GGRAPH_POINT_CIRCLE) },

	{ MP_OBJ_NEW_QSTR(MP_QSTR_LINE_NONE),          MP_OBJ_NEW_SMALL_INT(GGRAPH_LINE_NONE) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_LINE_SOLID),         MP_OBJ_NEW_SMALL_INT(GGRAPH_LINE_SOLID) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_LINE_DOT),           MP_OBJ_NEW_SMALL_INT(GGRAPH_LINE_DOT) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_LINE_DASH),          MP_OBJ_NEW_SMALL_INT(GGRAPH_LINE_DASH) },

	{ MP_OBJ_NEW_QSTR(MP_QSTR_ARROWS_X_POS),       MP_OBJ_NEW_SMALL_INT(GWIN_GRAPH_STYLE_XAXIS_POSITIVE_ARROWS) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_ARROWS_Y_POS),       MP_OBJ_NEW_SMALL_INT(GWIN_GRAPH_STYLE_YAXIS_POSITIVE_ARROWS) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_ARROWS_X_NEG),       MP_OBJ_NEW_SMALL_INT(GWIN_GRAPH_STYLE_XAXIS_NEGATIVE_ARROWS) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_ARROWS_Y_NEG),       MP_OBJ_NEW_SMALL_INT(GWIN_GRAPH_STYLE_YAXIS_NEGATIVE_ARROWS) },

};

STATIC MP_DEFINE_CONST_DICT(ugfx_graph_locals_dict, ugfx_graph_locals_dict_table);

const mp_obj_type_t ugfx_graph_type = {
    { &mp_type_type },
    .name = MP_QSTR_Graph,
    .make_new = ugfx_graph_make_new,
    .locals_dict = (mp_obj_t)&ugfx_graph_locals_dict,
};


#endif // MICROPY_HW_HAS_UGFX
