#include "gfx.h"

static GHandle      ghContainer;
static GHandle      ghButton;

static void createWidgets(void) {
    GWidgetInit wi;

    // Apply some default values for GWIN
    gwinWidgetClearInit(&wi);

    // Apply the container parameters
    wi.g.show = FALSE;
    wi.g.width = 200;
    wi.g.height = 150;
    wi.g.y = 10;
    wi.g.x = 10;
    wi.text = "Container";
    ghContainer = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
    wi.g.show = TRUE;

    // Apply the button parameters
    wi.g.width = 120;
    wi.g.height = 30;
    wi.g.y = 10;
    wi.g.x = 10;
    wi.text = "Button";
    wi.g.parent = ghContainer;
    ghButton = gwinButtonCreate(0, &wi);

    // Make the container become visible - therefore all its children
    // become visible as well
    gwinShow(ghContainer);
}

int main(void) {
    // Initialize the display
    gfxInit();

    // Set the widget defaults
    gwinSetDefaultFont(gdispOpenFont("*"));
    gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
    gdispClear(White);

    // Create the widget
    createWidgets();

    while(1) {
    	gfxSleepMilliseconds(1000);
    }

    return 0;
}
