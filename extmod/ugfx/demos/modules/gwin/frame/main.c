#include "gfx.h"
#include <stdlib.h>

static GListener    gl;
static GHandle      ghFrame1;
static GHandle      ghSliderR, ghSliderG, ghSliderB;
static GHandle      ghButton1, ghButton2, ghButton3;
static GHandle      ghWindow1;

static void _updateColor(void) {
    uint32_t color;

    color  = (unsigned)gwinSliderGetPosition(ghSliderR) << 16;
    color |= (unsigned)gwinSliderGetPosition(ghSliderG) <<  8;
    color |= (unsigned)gwinSliderGetPosition(ghSliderB) <<  0;

    gwinSetBgColor(ghWindow1, HTML2COLOR(color));
    gwinClear(ghWindow1);
}

static void _createWidgets(void) {
    GWidgetInit wi;

    // Apply some default values for GWIN
    gwinWidgetClearInit(&wi);
    wi.g.show = TRUE;

    // Create a surprise label behind the frame window
    wi.g.width = 100;
    wi.g.height = 20;
    wi.g.y = 100;
    wi.g.x = 150;
    wi.text = "Surprise!";
    gwinLabelCreate(0, &wi);

    // Apply the frame parameters
    wi.g.width = 300;
    wi.g.height = 200;
    wi.g.y = 10;
    wi.g.x = 10;
    wi.text = "Colorpicker";
    ghFrame1 = gwinFrameCreate(0, &wi, GWIN_FRAME_BORDER | GWIN_FRAME_CLOSE_BTN | GWIN_FRAME_MINMAX_BTN);

    // Apply the button parameters
    wi.g.width = 60;
    wi.g.height = 20;
    wi.g.x = 10;
    wi.g.y = 10;
    wi.text = "Random";
    wi.g.parent = ghFrame1;
    ghButton1 = gwinButtonCreate(0, &wi);

    // Apply the slider parameters
    wi.g.width = 200;
    wi.g.height = 20;
    wi.g.x = 80;
    wi.g.y += 0;
    wi.text = "Red";
    wi.g.parent = ghFrame1;
    ghSliderR = gwinSliderCreate(0, &wi);
    gwinSliderSetRange(ghSliderR, 0, 255);
    gwinSliderSetPosition(ghSliderR, 180);

    // Apply the button parameters
    wi.g.width = 60;
    wi.g.height = 20;
    wi.g.x = 10;
    wi.g.y += 25;
    wi.text = "Random";
    wi.g.parent = ghFrame1;
    ghButton2 = gwinButtonCreate(0, &wi);

    // Apply the slider parameters
    wi.g.width = 200;
    wi.g.height = 20;
    wi.g.x = 80;
    wi.g.y += 0;
    wi.text = "Green";
    wi.g.parent = ghFrame1;
    ghSliderG = gwinSliderCreate(0, &wi);
    gwinSliderSetRange(ghSliderG, 0, 255);
    gwinSliderSetPosition(ghSliderG, 60);

    // Apply the button parameters
    wi.g.width = 60;
    wi.g.height = 20;
    wi.g.x = 10;
    wi.g.y += 25;
    wi.text = "Random";
    wi.g.parent = ghFrame1;
    ghButton3 = gwinButtonCreate(0, &wi);

    // Apply the slider parameters
    wi.g.width = 200;
    wi.g.height = 20;
    wi.g.x = 80;
    wi.g.y += 0;
    wi.text = "Blue";
    wi.g.parent = ghFrame1;
    ghSliderB = gwinSliderCreate(0, &wi);
    gwinSliderSetRange(ghSliderB, 0, 255);
    gwinSliderSetPosition(ghSliderB, 235);

    // Color Preview
    wi.g.width = 270;
    wi.g.height = 65;
    wi.g.x = 10;
    wi.g.y = 90;
    ghWindow1 = gwinWindowCreate(0, &wi.g);

    _updateColor();
}

int main(void) {
    GEvent* pe;

    // Initialize the display
    gfxInit();

    // Set the widget defaults
    gwinSetDefaultFont(gdispOpenFont("*"));
    gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
    gdispClear(White);

    // create the widget
    _createWidgets();

    // We want to listen for widget events
    geventListenerInit(&gl);
    gwinAttachListener(&gl);

    while(1) {
        // Get an Event
        pe = geventEventWait(&gl, TIME_INFINITE);

        switch(pe->type) {
            case GEVENT_GWIN_SLIDER:
                if (((GEventGWinSlider *)pe)->gwin == ghSliderR || \
                                                      ghSliderG || \
                                                      ghSliderB ) {
                    _updateColor();
                }
                break;

            case GEVENT_GWIN_BUTTON:
                if (((GEventGWinButton *)pe)->gwin == ghButton1) {
                    gwinSliderSetPosition(ghSliderR, rand() % 256);
                } else if (((GEventGWinButton *)pe)->gwin == ghButton2) {
                    gwinSliderSetPosition(ghSliderG, rand() % 256);
                } else if (((GEventGWinButton *)pe)->gwin == ghButton3) {
                    gwinSliderSetPosition(ghSliderB, rand() % 256);
                }

                _updateColor();

            default:
                break;
        }
    }

    return 0;
}
