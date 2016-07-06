#include "gfx.h"

GHandle 	ghProgressbar;

static void _createWidget(void) {
	GWidgetInit	wi;
 
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
 
	wi.g.y = 10; wi.g.x = 10; wi.g.width = 200; wi.g.height = 20; wi.text = "Progress 1";
	ghProgressbar = gwinProgressbarCreate(0, &wi);
}

int main(void) {
	gfxInit();

	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gdispClear(White);

	_createWidget();

#if 1
	gwinProgressbarSetResolution(ghProgressbar, 10);
	gwinProgressbarStart(ghProgressbar, 500);

	gfxSleepMilliseconds(3000);
	gwinProgressbarReset(ghProgressbar);

	gfxSleepMilliseconds(3000);
	gwinDestroy(ghProgressbar);
#else
	gwinProgressbarSetPosition(ghProgressbar, 42);
	gwinProgressbarIncrement(ghProgressbar);
	gwinProgressbarDecrement(ghProgressbar);
#endif

	while (1) {
		gfxSleepMilliseconds(500);
	}

	return 0;
}

