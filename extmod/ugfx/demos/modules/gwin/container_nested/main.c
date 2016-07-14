#include "gfx.h"

static GListener    gl;
static GHandle      ghContainer1, ghContainer2, ghContainer3;
static GHandle      ghButton1, ghButton2, ghButton3, ghButton4;
static GHandle      ghCheckbox1, ghCheckbox2, ghCheckbox3;

static void createWidgets(void) {
    GWidgetInit wi;

    // Apply some default values for GWIN
    gwinWidgetClearInit(&wi);
    wi.g.show = FALSE;

    // Container 1
    wi.g.width = 300;
    wi.g.height = 220;
    wi.g.y = 40;
    wi.g.x = 10;
    wi.text = "Container 1";
    ghContainer1 = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);

    wi.g.show = TRUE;

    // Container 2
    wi.g.width = 100;
    wi.g.height = 150;
    wi.g.y = 55;
    wi.g.x = 10;
    wi.g.parent = ghContainer1;
    wi.text = "Container 2";
    ghContainer2 = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);

    // Container 3
    wi.g.width = 100;
    wi.g.height = 150;
    wi.g.y = 55;
    wi.g.x = 185;
    wi.g.parent = ghContainer1;
    wi.text = "Container 3";
    ghContainer3 = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);

    // Button 1
    wi.g.width = 80;
    wi.g.height = 20;
    wi.g.y = 10;
    wi.g.x = 10;
    wi.text = "Button 1";
    wi.g.parent = ghContainer2;
    ghButton1 = gwinButtonCreate(0, &wi);

    // Button 2
    wi.g.width = 80;
    wi.g.height = 20;
    wi.g.y = 40;
    wi.g.x = 10;
    wi.text = "Button 2";
    wi.g.parent = ghContainer2;
    ghButton2 = gwinButtonCreate(0, &wi);

    // Button 3
    wi.g.width = 80;
    wi.g.height = 20;
    wi.g.y = 10;
    wi.g.x = 10;
    wi.text = "Button 3";
    wi.g.parent = ghContainer3;
    ghButton3 = gwinButtonCreate(0, &wi);

    // Button 4
    wi.g.width = 80;
    wi.g.height = 20;
    wi.g.y = 40;
    wi.g.x = 10;
    wi.text = "Button 4";
    wi.g.parent = ghContainer3;
    ghButton4 = gwinButtonCreate(0, &wi);

    // Checkbox 1
    wi.g.width = 80;
    wi.g.height = 20;
    wi.g.y = 25;
    wi.g.x = 10;
    wi.text = " Visible C2";
    wi.g.parent = ghContainer1;
    ghCheckbox1 = gwinCheckboxCreate(0, &wi);

    // Checkbox 2
    wi.g.width = 80;
    wi.g.height = 20;
    wi.g.y = 25;
    wi.g.x = 185;
    wi.text = " Visible C3";
    wi.g.parent = ghContainer1;
    ghCheckbox2 = gwinCheckboxCreate(0, &wi);

    // Checkbox 3
    wi.g.width = 80;
    wi.g.height = 20;
    wi.g.y = 10;
    wi.g.x = 10;
    wi.text = " Visible C1";
    wi.g.parent = 0;
    ghCheckbox3 = gwinCheckboxCreate(0, &wi);


    // Make the container become visible - therefore all its children
    // become visible as well
    gwinShow(ghContainer1);
}

int main(void) {
    GEvent* pe;

    // Initialize the display
    gfxInit();

    // Set the widget defaults
    gwinSetDefaultFont(gdispOpenFont("*"));
    gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
    gdispClear(White);

    // Create the widget
    createWidgets();

    // We want to listen for widget events
    geventListenerInit(&gl);
    gwinAttachListener(&gl);

    // Set the initial state of the checkboxes
    gwinCheckboxCheck(ghCheckbox1, TRUE);
    gwinCheckboxCheck(ghCheckbox2, TRUE);
    gwinCheckboxCheck(ghCheckbox3, TRUE);

    while(1) {
        // Get an Event
        pe = geventEventWait(&gl, TIME_INFINITE);

        switch(pe->type) {
            case GEVENT_GWIN_CHECKBOX:
                if (((GEventGWinCheckbox*)pe)->gwin == ghCheckbox1) {
                    gwinSetVisible(ghContainer2, gwinCheckboxIsChecked(ghCheckbox1));
                }
                else if (((GEventGWinCheckbox*)pe)->gwin == ghCheckbox2) {
                    gwinSetVisible(ghContainer3, gwinCheckboxIsChecked(ghCheckbox2));
                }
                else if (((GEventGWinCheckbox*)pe)->gwin == ghCheckbox3) {
                    gwinSetVisible(ghContainer1, gwinCheckboxIsChecked(ghCheckbox3));
                }
                break;
        }
    }

    return 0;
}
