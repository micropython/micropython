/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/gwin_label.h
 * @brief	GWIN label widget header file
 *
 * @defgroup Label Label
 * @ingroup Widgets
 *
 * @brief		Simple label widget.
 *
 * @details		Setting the dimensions of the widget to 0 will automatically
 *				set the labels dimensions to fit the entire text. Note that the
 *				dimensions of the label will change every time the text is changed
 *				through gwinSetText().
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in your gfxconf.h
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GDISP_NEED_TEXT must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_LABEL must be set to TRUE in your gfxconf.h
 * @pre			The fonts you want to use must be enabled in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_LABEL_H
#define _GWIN_LABEL_H

// This file is included within "src/gwin/gwin_widget.h"

/**
 * @brief	The internal label flags
 * @note	Used only for writing a custom draw routine.
 * @{
 */
#define GLABEL_FLG_WAUTO		0x01
#define GLABEL_FLG_HAUTO		0x02
#define GLABEL_FLG_BORDER		0x04
/** @} */

// An label window
typedef struct GLabelObject {
	GWidgetObject	w;

	#if GWIN_LABEL_ATTRIBUTE
		coord_t         tab;
		const char*		attr;
	#endif
} GLabelObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Create a label widget.
 * @details				A label widget is a simple window which has a static text.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] widget	The label structure to initialise. If this is NULL, the structure is dynamically allocated.
 * @param[in] pInit		The initialisation parameters to use.
 *
 * @return				NULL if there is no resultat drawing area, otherwise the widget handle.
 *
 * @api
 */
GHandle gwinGLabelCreate(GDisplay *g, GLabelObject *widget, GWidgetInit *pInit);
#define gwinLabelCreate(w, pInit)			gwinGLabelCreate(GDISP, w, pInit)

/**
 * @brief				Border settings for the default rendering routine
 *
 * @param[in] gh		The widget handle (must be a label handle)
 * @param[in] border	Shall a border be rendered?
 *
 * @api
 */
void gwinLabelSetBorder(GHandle gh, bool_t border);

#if GWIN_LABEL_ATTRIBUTE || defined(__DOXYGEN__)
	/**
	 * @brief               Add an text attribute in front of the normal label text
	 * @details              Often you want to display a text like this:
	 *                          Current IP: 192.168.1.42
	 *                      In that case, the actual IP will be variable, the text in front of it
	 *                      always remains the same. The static text is called the attribute and can be
	 *                      set using this function.
	 *                      Furthermore, the tab can be set in order to vertically align multiple labels.
	 *                      Please check out the website for further explanation, illustraions and usage
	 *                      examples.
	 *
	 * @note                The attribute text is not copied into private memory and so it
	 *						must be a constant string, not one allocated in a stack buffer.
	 * @note				Use of this construct is discouraged. The appropriate way is to
	 *						create two labels - one for the static text and one for the
	 *						dynamic text.
	 *
	 * @param[in] gh        The widget handle (must be a label handle)
	 * @param[in] tab       The distance of the label text from the left widget edge
	 * @param[in] attr      The attribute to be displayed
	 *
	 * @api
	 */
	void gwinLabelSetAttribute(GHandle gh, coord_t tab, const char* attr);
#endif

/**
 * @defgroup Renderings_Label Renderings
 *
 * @brief				Built-in rendering functions for the label widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different label drawing styles.
 *
 * @note				In your custom label drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				The built-in functions below ignore the param parameter.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				Renders a label with the text left jestified.
 *
 * @note				This is the default rendering function.
 *
 * @param[in] gw		The widget object (must be a label object)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinLabelDrawJustifiedLeft(GWidgetObject *gw, void *param);

/**
 * @brief				Renders a label with the text right jestified.
 *
 * @param[in] gw		The widget object (must be a label object)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinLabelDrawJustifiedRight(GWidgetObject *gw, void *param);

/**
 * @brief				Renders a label with the text center jestified.
 *
 * @param[in] gw		The widget object (must be a label object)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinLabelDrawJustifiedCenter(GWidgetObject *gw, void *param);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // _GWIN_LABEL_H
/** @} */
