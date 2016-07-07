/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "gfx.h"

/**
 * This demo demonstrates many of the GWIN widgets.
 * On the "Radio" tab try playing with the color radio buttons.
 * On the "Checkbox" tab try playing with the "Disable All" checkbox.
 */

/**
 * The image files must be stored on a GFILE file-system.
 * Use either GFILE_NEED_NATIVEFS or GFILE_NEED_ROMFS (or both).
 *
 * The ROMFS uses the file "romfs_files.h" to describe the set of files in the ROMFS.
 */

/**
 * The code can either use the Tabset control or use Radio buttons set to the Tab style.
 * Change this in your gfxconf.h file by defining GWIN_NEED_TABSET (or not). It is
 * defined by default in this demo.
 */

/* Our custom yellow style */
static const GWidgetStyle YellowWidgetStyle = {
	Yellow,							// window background
	HTML2COLOR(0x800000),			// focus

	// enabled color set
	{
		HTML2COLOR(0x0000FF),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0x00E000)		// progress - active area
	},

	// disabled color set
	{
		HTML2COLOR(0xC0C0C0),		// text
		HTML2COLOR(0x808080),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0xC0E0C0)		// progress - active area
	},

	// pressed color set
	{
		HTML2COLOR(0xFF00FF),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x808080),		// fill
		HTML2COLOR(0x00E000),		// progress - active area
	}
};

/* The variables we need */
static font_t		font;
static GListener	gl;
static GHandle		ghConsole;
static GTimer		FlashTimer;

#if GWIN_NEED_TABSET
	static GHandle		ghTabset;
#else
	static GHandle		ghTabButtons, ghTabSliders, ghTabCheckboxes, ghTabLabels, ghTabRadios, ghTabLists, ghTabImages, ghTabProgressbar;
#endif
static GHandle		ghPgButtons, ghPgSliders, ghPgCheckboxes, ghPgLabels, ghPgRadios, ghPgLists, ghPgImages, ghPgProgressbars;
static GHandle		ghButton1, ghButton2, ghButton3, ghButton4;
static GHandle		ghSlider1, ghSlider2, ghSlider3, ghSlider4;
static GHandle		ghCheckbox1, ghCheckbox2, ghCheckbox3, ghCheckDisableAll;
static GHandle		ghLabelSlider1, ghLabelSlider2, ghLabelSlider3, ghLabelSlider4, ghLabelRadio1;
static GHandle		ghRadio1, ghRadio2;
static GHandle		ghRadioBlack, ghRadioWhite, ghRadioYellow;
static GHandle		ghList1, ghList2, ghList3, ghList4;
static GHandle		ghImage1;
static GHandle		ghProgressbar1;
static gdispImage	imgYesNo;

/* Some useful macros */
#define	ScrWidth			gdispGetWidth()
#define	ScrHeight			gdispGetHeight()

#define BUTTON_PADDING		20
#define TAB_HEIGHT			30
#define LABEL_HEIGHT		15
#define BUTTON_WIDTH		50
#define BUTTON_HEIGHT		30
#define LIST_WIDTH			75
#define LIST_HEIGHT			80
#define SLIDER_WIDTH		20
#define CHECKBOX_WIDTH		80
#define CHECKBOX_HEIGHT		20
#define RADIO_WIDTH			50
#define RADIO_HEIGHT		20
#define COLOR_WIDTH			80
#define DISABLEALL_WIDTH	100
#define GROUP_TABS			0
#define GROUP_YESNO			1
#define GROUP_COLORS		2

#if !GWIN_NEED_TABSET
	// Wrap tabs onto the next line if they don't fit.
	static void settabtext(GWidgetInit *pwi, char *txt) {
		if (pwi->g.x >= ScrWidth) {
			pwi->g.x = 0;
			pwi->g.y += pwi->g.height;
		}
		pwi->text = txt;
		pwi->g.width = gdispGetStringWidth(pwi->text, font) + BUTTON_PADDING;
		if (pwi->g.x + pwi->g.width > ScrWidth) {
			pwi->g.x = 0;
			pwi->g.y += pwi->g.height;
		}
	}

	/**
	 * Set the visibility of widgets based on which tab is selected.
	 */
	static void setTab(GHandle tab) {
		/* Make sure everything is invisible first */
		gwinHide(ghPgButtons);
		gwinHide(ghPgSliders);
		gwinHide(ghPgCheckboxes);
		gwinHide(ghPgLabels);
		gwinHide(ghPgRadios);
		gwinHide(ghPgLists);
		gwinHide(ghPgImages);
		gwinHide(ghPgProgressbars);

		/* Turn on widgets depending on the tab selected */
		if (tab == ghTabButtons)
			gwinShow(ghPgButtons);
		else if (tab == ghTabSliders)
			gwinShow(ghPgSliders);
		else if (tab == ghTabCheckboxes)
			gwinShow(ghPgCheckboxes);
		else if (tab == ghTabLabels)
			gwinShow(ghPgLabels);
		else if (tab == ghTabRadios)
			gwinShow(ghPgRadios);
		else if (tab == ghTabLists)
			gwinShow(ghPgLists);
		else if (tab == ghTabImages)
			gwinShow(ghPgImages);
		else if (tab == ghTabProgressbar)
			gwinShow(ghPgProgressbars);
	}
#endif

// Wrap buttons onto the next line if they don't fit.
static void setbtntext(GWidgetInit *pwi, coord_t maxwidth, char *txt) {
	if (pwi->g.x >= maxwidth) {
		pwi->g.x = 5;
		pwi->g.y += pwi->g.height+1;
	}
	pwi->text = txt;
	pwi->g.width = gdispGetStringWidth(pwi->text, font) + BUTTON_PADDING;
	if (pwi->g.x + pwi->g.width > maxwidth) {
		pwi->g.x = 5;
		pwi->g.y += pwi->g.height+1;
	}
}

/**
 * Create all the widgets.
 * With the exception of the Pages they are all initially visible.
 *
 * This routine is complicated by the fact that we want a dynamic
 * layout so it looks good on small and large displays.
 * It is tested to work on 320x272 as a minimum LCD size.
 */
static void createWidgets(void) {
	GWidgetInit		wi;
	coord_t			border, pagewidth;

	gwinWidgetClearInit(&wi);

	// Calculate page borders based on screen size
	border = ScrWidth < 450 ? 1 : 5;

	// Create the Tabs
	#if GWIN_NEED_TABSET
		wi.g.show = TRUE;
		wi.g.x = border; wi.g.y = 0;
		wi.g.width = ScrWidth - 2*border; wi.g.height = ScrHeight-wi.g.y-border;
		ghTabset			= gwinTabsetCreate(0, &wi, GWIN_TABSET_BORDER);
		ghPgButtons			= gwinTabsetAddTab(ghTabset, "Buttons", FALSE);
		ghPgSliders			= gwinTabsetAddTab(ghTabset, "Sliders", FALSE);
		ghPgCheckboxes		= gwinTabsetAddTab(ghTabset, "Checkbox", FALSE);
		ghPgRadios			= gwinTabsetAddTab(ghTabset, "Radios", FALSE);
		ghPgLists			= gwinTabsetAddTab(ghTabset, "Lists", FALSE);
		ghPgLabels			= gwinTabsetAddTab(ghTabset, "Labels", FALSE);
		ghPgImages			= gwinTabsetAddTab(ghTabset, "Images", FALSE);
		ghPgProgressbars	= gwinTabsetAddTab(ghTabset, "Progressbar", FALSE);

		pagewidth = gwinGetInnerWidth(ghTabset)/2;

		// Console - we apply some special colors before making it visible
		//	We put the console on the tabset itself rather than a tab-page.
		//	This makes it appear on every page :)
		wi.g.parent = ghTabset;
		wi.g.x = pagewidth;
		wi.g.width = pagewidth;
		ghConsole = gwinConsoleCreate(0, &wi.g);
	    gwinSetColor(ghConsole, Black);
	    gwinSetBgColor(ghConsole, HTML2COLOR(0xF0F0F0));

	#else
		wi.g.show = TRUE; wi.customDraw = gwinRadioDraw_Tab;
		wi.g.height = TAB_HEIGHT; wi.g.y = 0;
		wi.g.x = 0; setbtntext(&wi, ScrWidth, "Buttons");
		ghTabButtons     = gwinRadioCreate(0, &wi, GROUP_TABS);
		wi.g.x += wi.g.width; settabtext(&wi, "Sliders");
		ghTabSliders     = gwinRadioCreate(0, &wi, GROUP_TABS);
		wi.g.x += wi.g.width; settabtext(&wi, "Checkbox");
		ghTabCheckboxes  = gwinRadioCreate(0, &wi, GROUP_TABS);
		wi.g.x += wi.g.width; settabtext(&wi, "Radios");
		ghTabRadios      = gwinRadioCreate(0, &wi, GROUP_TABS);
		wi.g.x += wi.g.width; settabtext(&wi, "Lists");
		ghTabLists       = gwinRadioCreate(0, &wi, GROUP_TABS);
		wi.g.x += wi.g.width; settabtext(&wi, "Labels");
		ghTabLabels      = gwinRadioCreate(0, &wi, GROUP_TABS);
		wi.g.x += wi.g.width; settabtext(&wi, "Images");
		ghTabImages      = gwinRadioCreate(0, &wi, GROUP_TABS);
		wi.g.x += wi.g.width; settabtext(&wi, "Progressbar");
		ghTabProgressbar = gwinRadioCreate(0, &wi, GROUP_TABS);
		wi.g.y += wi.g.height;
		wi.customDraw = 0;

		// Create the Pages
		wi.g.show = FALSE;
		wi.g.x = border; wi.g.y += border;
		wi.g.width = ScrWidth/2 - border; wi.g.height = ScrHeight-wi.g.y-border;
		ghPgButtons			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
		ghPgSliders			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
		ghPgCheckboxes		= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
		ghPgRadios			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
		ghPgLists			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
		ghPgLabels			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
		ghPgImages			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
		ghPgProgressbars	= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
		wi.g.show = TRUE;

		// Console - we apply some special colors before making it visible
		wi.g.x = ScrWidth/2+border;
		wi.g.width = ScrWidth/2 - 2*border;
		ghConsole = gwinConsoleCreate(0, &wi.g);
	    gwinSetColor(ghConsole, Black);
	    gwinSetBgColor(ghConsole, HTML2COLOR(0xF0F0F0));

	    pagewidth = gwinGetInnerWidth(ghPgButtons);
	#endif

    // Buttons
	wi.g.parent = ghPgButtons;
	wi.g.width = BUTTON_WIDTH; wi.g.height = BUTTON_HEIGHT; wi.g.y = 5;
	wi.g.x = 5; setbtntext(&wi, pagewidth, "Button 1");
	ghButton1 = gwinButtonCreate(0, &wi);
	wi.g.x += wi.g.width+3; setbtntext(&wi, pagewidth, "Button 2");
	ghButton2 = gwinButtonCreate(0, &wi);
	wi.g.x += wi.g.width+3; setbtntext(&wi, pagewidth, "Button 3");
	ghButton3 = gwinButtonCreate(0, &wi);
	wi.g.x += wi.g.width+3; setbtntext(&wi, pagewidth, "Button 4");
	ghButton4 = gwinButtonCreate(0, &wi);

	// Horizontal Sliders
	wi.g.parent = ghPgSliders;
	wi.g.width = pagewidth - 10; wi.g.height = SLIDER_WIDTH;
	wi.g.x = 5; wi.g.y = 5; wi.text = "S1";
	ghSlider1 = gwinSliderCreate(0, &wi);
	gwinSliderSetPosition(ghSlider1, 33);
	wi.g.y += wi.g.height + 1; wi.text = "S2";
	ghSlider2 = gwinSliderCreate(0, &wi);
	gwinSliderSetPosition(ghSlider2, 86);

	// Vertical Sliders
	wi.g.y += wi.g.height + 5;
	wi.g.width = SLIDER_WIDTH; wi.g.height = gwinGetInnerHeight(ghPgSliders) - 5 - wi.g.y;
	wi.g.x = 5; wi.text = "S3";
	ghSlider3 = gwinSliderCreate(0, &wi);
	gwinSliderSetPosition(ghSlider3, 13);
	wi.g.x += wi.g.width+1; wi.text = "S4";
	ghSlider4 = gwinSliderCreate(0, &wi);
	gwinSliderSetPosition(ghSlider4, 76);

	// Checkboxes - for the 2nd and 3rd checkbox we apply special drawing before making it visible
	wi.g.parent = ghPgCheckboxes;
	wi.g.width = CHECKBOX_WIDTH; wi.g.height = CHECKBOX_HEIGHT; wi.g.x = 5;
	wi.g.y = 5; wi.text = "C1";
	ghCheckbox1 = gwinCheckboxCreate(0, &wi);
	wi.customDraw = gwinCheckboxDraw_CheckOnRight;
	wi.g.y += wi.g.height+1; wi.text = "C2";
	ghCheckbox2 = gwinCheckboxCreate(0, &wi);
	wi.customDraw = gwinCheckboxDraw_Button;
	wi.g.y += wi.g.height+1; wi.text = "C3"; wi.g.width = BUTTON_WIDTH; wi.g.height = BUTTON_HEIGHT;
	ghCheckbox3 = gwinCheckboxCreate(0, &wi);
	wi.g.y += wi.g.height+1; wi.text = "Disable All";
	wi.customDraw = 0; wi.g.width = DISABLEALL_WIDTH; wi.g.height = CHECKBOX_HEIGHT;
	ghCheckDisableAll = gwinCheckboxCreate(0, &wi);

    // Labels
	wi.g.parent = ghPgLabels;
	wi.g.width = pagewidth-10;	wi.g.height = LABEL_HEIGHT;
	wi.g.x = wi.g.y = 5; wi.text = "N/A";
	ghLabelSlider1 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelSlider1, 100, "Slider 1:");
	wi.g.y += LABEL_HEIGHT + 2;
	ghLabelSlider2 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelSlider2, 100, "Slider 2:");
	wi.g.y += LABEL_HEIGHT + 2;
	ghLabelSlider3 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelSlider3, 100, "Slider 3:");
	wi.g.y += LABEL_HEIGHT + 2;
	ghLabelSlider4 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelSlider4, 100, "Slider 4:");
	wi.g.y += LABEL_HEIGHT + 2;
	ghLabelRadio1 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelRadio1, 100, "RadioButton 1:");


	// Radio Buttons
	wi.g.parent = ghPgRadios;
	wi.g.width = RADIO_WIDTH; wi.g.height = RADIO_HEIGHT; wi.g.y = 5;
	wi.g.x = 5; wi.text = "Yes";
	ghRadio1 = gwinRadioCreate(0, &wi, GROUP_YESNO);
	wi.g.x += wi.g.width; wi.text = "No"; if (wi.g.x + wi.g.width > pagewidth) { wi.g.x = 5; wi.g.y += RADIO_HEIGHT; }
	ghRadio2 = gwinRadioCreate(0, &wi, GROUP_YESNO);
	gwinRadioPress(ghRadio1);
	wi.g.width = COLOR_WIDTH; wi.g.y += RADIO_HEIGHT+5;
	wi.g.x = 5; wi.text = "Black";
	ghRadioBlack = gwinRadioCreate(0, &wi, GROUP_COLORS);
	wi.g.x += wi.g.width; wi.text = "White"; if (wi.g.x + wi.g.width > pagewidth) { wi.g.x = 5; wi.g.y += RADIO_HEIGHT; }
	ghRadioWhite = gwinRadioCreate(0, &wi, GROUP_COLORS);
	wi.g.x += wi.g.width; wi.text = "Yellow"; if (wi.g.x + wi.g.width > pagewidth) { wi.g.x = 5; wi.g.y += RADIO_HEIGHT; }
	ghRadioYellow = gwinRadioCreate(0, &wi, GROUP_COLORS);
	gwinRadioPress(ghRadioWhite);

	// Lists
	border = pagewidth < 10+2*LIST_WIDTH ? 2 : 5;
	wi.g.parent = ghPgLists;
	wi.g.width = LIST_WIDTH; wi.g.height = LIST_HEIGHT; wi.g.y = border;
	wi.g.x = border; wi.text = "L1";
	ghList1 = gwinListCreate(0, &wi, FALSE);
	gwinListAddItem(ghList1, "Item 0", FALSE);
	gwinListAddItem(ghList1, "Item 1", FALSE);
	gwinListAddItem(ghList1, "Item 2", FALSE);
	gwinListAddItem(ghList1, "Item 3", FALSE);
	gwinListAddItem(ghList1, "Item 4", FALSE);
	gwinListAddItem(ghList1, "Item 5", FALSE);
	gwinListAddItem(ghList1, "Item 6", FALSE);
	gwinListAddItem(ghList1, "Item 7", FALSE);
	gwinListAddItem(ghList1, "Item 8", FALSE);
	gwinListAddItem(ghList1, "Item 9", FALSE);
	gwinListAddItem(ghList1, "Item 10", FALSE);
	gwinListAddItem(ghList1, "Item 11", FALSE);
	gwinListAddItem(ghList1, "Item 12", FALSE);
	gwinListAddItem(ghList1, "Item 13", FALSE);
	wi.text = "L2"; wi.g.x += LIST_WIDTH+border; if (wi.g.x + LIST_WIDTH > pagewidth) { wi.g.x = border; wi.g.y += LIST_HEIGHT+border; }
	ghList2 = gwinListCreate(0, &wi, TRUE);
	gwinListAddItem(ghList2, "Item 0", FALSE);
	gwinListAddItem(ghList2, "Item 1", FALSE);
	gwinListAddItem(ghList2, "Item 2", FALSE);
	gwinListAddItem(ghList2, "Item 3", FALSE);
	gwinListAddItem(ghList2, "Item 4", FALSE);
	gwinListAddItem(ghList2, "Item 5", FALSE);
	gwinListAddItem(ghList2, "Item 6", FALSE);
	gwinListAddItem(ghList2, "Item 7", FALSE);
	gwinListAddItem(ghList2, "Item 8", FALSE);
	gwinListAddItem(ghList2, "Item 9", FALSE);
	gwinListAddItem(ghList2, "Item 10", FALSE);
	gwinListAddItem(ghList2, "Item 11", FALSE);
	gwinListAddItem(ghList2, "Item 12", FALSE);
	gwinListAddItem(ghList2, "Item 13", FALSE);
	wi.text = "L3"; wi.g.x += LIST_WIDTH+border; if (wi.g.x + LIST_WIDTH > pagewidth) { wi.g.x = border; wi.g.y += LIST_HEIGHT+border; }
	ghList3 = gwinListCreate(0, &wi, TRUE);
	gwinListAddItem(ghList3, "Item 0", FALSE);
	gwinListAddItem(ghList3, "Item 1", FALSE);
	gwinListAddItem(ghList3, "Item 2", FALSE);
	gwinListAddItem(ghList3, "Item 3", FALSE);
	gdispImageOpenFile(&imgYesNo, "image_yesno.gif");
	gwinListItemSetImage(ghList3, 1, &imgYesNo);
	gwinListItemSetImage(ghList3, 3, &imgYesNo);
	wi.text = "L4"; wi.g.x += LIST_WIDTH+border; if (wi.g.x + LIST_WIDTH > pagewidth) { wi.g.x = border; wi.g.y += LIST_HEIGHT+border; }
	ghList4 = gwinListCreate(0, &wi, TRUE);
	gwinListAddItem(ghList4, "Item 0", FALSE);
	gwinListAddItem(ghList4, "Item 1", FALSE);
	gwinListAddItem(ghList4, "Item 2", FALSE);
	gwinListAddItem(ghList4, "Item 3", FALSE);
	gwinListAddItem(ghList4, "Item 4", FALSE);
	gwinListAddItem(ghList4, "Item 5", FALSE);
	gwinListAddItem(ghList4, "Item 6", FALSE);
	gwinListAddItem(ghList4, "Item 7", FALSE);
	gwinListAddItem(ghList4, "Item 8", FALSE);
	gwinListAddItem(ghList4, "Item 9", FALSE);
	gwinListAddItem(ghList4, "Item 10", FALSE);
	gwinListAddItem(ghList4, "Item 11", FALSE);
	gwinListAddItem(ghList4, "Item 12", FALSE);
	gwinListAddItem(ghList4, "Item 13", FALSE);
	gwinListSetScroll(ghList4, scrollSmooth);

	// Image
	wi.g.parent = ghPgImages;
	wi.g.x = wi.g.y = 0; wi.g.width = pagewidth; wi.g.height = gwinGetInnerHeight(ghPgImages);
	ghImage1 = gwinImageCreate(0, &wi.g);
	gwinImageOpenFile(ghImage1, "romfs_img_ugfx.gif");

	// Progressbar
	wi.g.parent = ghPgProgressbars;
	wi.g.width = pagewidth-10; wi.g.height = SLIDER_WIDTH; wi.g.y = 5;
	wi.g.x = 5; wi.text = "Progressbar 1";
	ghProgressbar1 = gwinProgressbarCreate(0, &wi);
	gwinProgressbarSetResolution(ghProgressbar1, 10);
}

/**
 * Set the value of the labels
 */
static void setLabels(void) {
	char tmp[20];

	// The sliders
	snprintg(tmp, sizeof(tmp), "%d%%", gwinSliderGetPosition(ghSlider1));
	gwinSetText(ghLabelSlider1, tmp, TRUE);
	snprintg(tmp, sizeof(tmp), "%d%%", gwinSliderGetPosition(ghSlider2));
	gwinSetText(ghLabelSlider2, tmp, TRUE);
	snprintg(tmp, sizeof(tmp), "%d%%", gwinSliderGetPosition(ghSlider3));
	gwinSetText(ghLabelSlider3, tmp, TRUE);
	snprintg(tmp, sizeof(tmp), "%d%%", gwinSliderGetPosition(ghSlider4));
	gwinSetText(ghLabelSlider4, tmp, TRUE);

	// The radio buttons
	if (gwinRadioIsPressed(ghRadio1))
		gwinSetText(ghLabelRadio1, "Yes", TRUE);
	else if (gwinRadioIsPressed(ghRadio2))
		gwinSetText(ghLabelRadio1, "No", TRUE);
}

/**
 * Control the progress bar auto-increment
 */
static void setProgressbar(bool_t onoff) {
	if (onoff)
		gwinProgressbarStart(ghProgressbar1, 500);
	else {
		gwinProgressbarStop(ghProgressbar1);		// Stop the progress bar
		gwinProgressbarReset(ghProgressbar1);
	}
}

/**
 * Set the enabled state of every widget (except the tabs etc)
 */
static void setEnabled(bool_t ena) {
	gwinSetEnabled(ghPgButtons, ena);
	gwinSetEnabled(ghPgSliders, ena);
	gwinSetEnabled(ghPgLabels, ena);
	gwinSetEnabled(ghPgRadios, ena);
	gwinSetEnabled(ghPgLists, ena);
	gwinSetEnabled(ghPgImages, ena);
	gwinSetEnabled(ghPgProgressbars, ena);
	// Checkboxes we need to do individually so we don't disable the checkbox to re-enable everything
	gwinSetEnabled(ghCheckbox1, ena);
	gwinSetEnabled(ghCheckbox2, ena);
	gwinSetEnabled(ghCheckbox3, ena);
	//gwinSetEnabled(ghCheckDisableAll, TRUE);
}

static void FlashOffFn(void *param) {
	(void)	param;

	gwinNoFlash(ghCheckbox3);
}

int main(void) {
	GEvent *			pe;

	// Initialize the display
	gfxInit();

	// Set the widget defaults
	font = gdispOpenFont("*");			// Get the first defined font.
	gwinSetDefaultFont(font);
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gdispClear(White);

	// Create the gwin windows/widgets
	createWidgets();

    // Assign toggles and dials to specific buttons & sliders etc.
	#if GINPUT_NEED_TOGGLE
		gwinAttachToggle(ghButton1, 0, 0);
		gwinAttachToggle(ghButton2, 0, 1);
	#endif
	#if GINPUT_NEED_DIAL
		gwinAttachDial(ghSlider1, 0, 0);
		gwinAttachDial(ghSlider3, 0, 1);
	#endif

	// Make the console visible
	gwinShow(ghConsole);
	gwinClear(ghConsole);

    // We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);
	gtimerInit(&FlashTimer);

	#if !GWIN_NEED_TABSET
		// Press the Tab we want visible
		gwinRadioPress(ghTabButtons);
	#endif

	while(1) {
		// Get an Event
		pe = geventEventWait(&gl, TIME_INFINITE);

		switch(pe->type) {
		case GEVENT_GWIN_BUTTON:
			gwinPrintf(ghConsole, "Button %s\n", gwinGetText(((GEventGWinButton *)pe)->gwin));
			break;

		case GEVENT_GWIN_SLIDER:
			gwinPrintf(ghConsole, "Slider %s=%d\n", gwinGetText(((GEventGWinSlider *)pe)->gwin), ((GEventGWinSlider *)pe)->position);
			break;

		case GEVENT_GWIN_CHECKBOX:
			gwinPrintf(ghConsole, "Checkbox %s=%s\n", gwinGetText(((GEventGWinCheckbox *)pe)->gwin), ((GEventGWinCheckbox *)pe)->isChecked ? "Checked" : "UnChecked");

			// If it is the Disable All checkbox then do that.
			if (((GEventGWinCheckbox *)pe)->gwin == ghCheckDisableAll) {
				gwinPrintf(ghConsole, "%s All\n", ((GEventGWinCheckbox *)pe)->isChecked ? "Disable" : "Enable");
				setEnabled(!((GEventGWinCheckbox *)pe)->isChecked);

			// If it is the toggle button checkbox start the flash.
			} else if (((GEventGWinCheckbox *)pe)->gwin == ghCheckbox3) {
				gwinFlash(ghCheckbox3);
				gtimerStart(&FlashTimer, FlashOffFn, 0, FALSE, 3000);
			}
			break;

		case GEVENT_GWIN_LIST:
			gwinPrintf(ghConsole, "List %s Item %d %s\n", gwinGetText(((GEventGWinList *)pe)->gwin), ((GEventGWinList *)pe)->item,
					gwinListItemIsSelected(((GEventGWinList *)pe)->gwin, ((GEventGWinList *)pe)->item) ? "Selected" : "Unselected");
			break;

		case GEVENT_GWIN_RADIO:
			gwinPrintf(ghConsole, "Radio Group %u=%s\n", ((GEventGWinRadio *)pe)->group, gwinGetText(((GEventGWinRadio *)pe)->gwin));

			switch(((GEventGWinRadio *)pe)->group) {
			#if !GWIN_NEED_TABSET
				case GROUP_TABS:

					// Set control visibility depending on the tab selected
					setTab(((GEventGWinRadio *)pe)->gwin);

					// We show the state of some of the GUI elements here
					setProgressbar(((GEventGWinRadio *)pe)->gwin == ghTabProgressbar);
					if (((GEventGWinRadio *)pe)->gwin == ghTabLabels)
						setLabels();
					break;
			#endif

			case GROUP_COLORS:
				{
					const GWidgetStyle	*pstyle;

					gwinPrintf(ghConsole, "Change Color Scheme\n");

					if (((GEventGWinRadio *)pe)->gwin == ghRadioYellow)
						pstyle = &YellowWidgetStyle;
					else if (((GEventGWinRadio *)pe)->gwin == ghRadioBlack)
						pstyle = &BlackWidgetStyle;
					else
						pstyle = &WhiteWidgetStyle;

					// Clear the screen to the new color
					gdispClear(pstyle->background);

					// Update the style on all controls
					gwinSetDefaultStyle(pstyle, TRUE);
				}
				break;
			}
			break;

		#if GWIN_NEED_TABSET
			case GEVENT_GWIN_TABSET:
				gwinPrintf(ghConsole, "TabPage %u (%s)\n", ((GEventGWinTabset *)pe)->nPage, gwinTabsetGetTitle(((GEventGWinTabset *)pe)->ghPage));

				// We show the state of some of the GUI elements here
				setProgressbar(((GEventGWinTabset *)pe)->ghPage == ghPgProgressbars);
				if (((GEventGWinTabset *)pe)->ghPage == ghPgLabels)
					setLabels();
				break;
		#endif

		default:
			gwinPrintf(ghConsole, "Unknown %d\n", pe->type);
			break;
		}
	}
	return 0;
}

