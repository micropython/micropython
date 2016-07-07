/*
 * Demonstration program for Nano-X graphics.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MWINCLUDECOLORS
#include <microwin/nano-X.h>
#include <GL/gl.h> 
#include <GL/nglx.h> 
#include "ui.h"

static	GR_WINDOW_ID	w1;		/* id for large window */
static	GR_GC_ID	gc1;		/* graphics context for text */

void errorcatcher();			/* routine to handle errors */

void tkSwapBuffers(void)
{
    nglXSwapBuffers(w1);
}

int
ui_loop(int argc,char **argv, const char *name)
{
	GR_EVENT	event;		/* current event */
	GR_IMAGE_ID	id = 0;
        NGLXContext cx;
        int width, height, k;

	if (GrOpen() < 0) {
		fprintf(stderr, "cannot open graphics\n");
		exit(1);
	}
	
        width = 400;
        height = 300;

	GrSetErrorHandler(errorcatcher);

	w1 = GrNewWindow(GR_ROOT_WINDOW_ID, 10, 10, width, height, 4, BLACK, WHITE);

	GrSelectEvents(w1, GR_EVENT_MASK_CLOSE_REQ|GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	GrMapWindow(w1);

	gc1 = GrNewGC();

	GrSetGCForeground(gc1, WHITE);

        cx = nglXCreateContext(NULL, 0);
        nglXMakeCurrent(w1, cx);
        
        init();
        reshape(width, height);

	while (1) {
            GrCheckNextEvent(&event);
            switch(event.type) {
            case GR_EVENT_TYPE_CLOSE_REQ:
                GrFreeImage(id);
                GrClose();
                exit(0);
            case GR_EVENT_TYPE_EXPOSURE:
                break;
            case GR_EVENT_TYPE_KEY_DOWN:
                {
                    GR_EVENT_KEYSTROKE *kp = &event.keystroke;
                    /* XXX: nanoX special keys are totally bugged ! */
                    switch(kp->ch) {
                    case 81:
                        k = KEY_LEFT;
                        break;
                    case 83:
                        k = KEY_RIGHT;
                        break;
                    case 82:
                        k = KEY_UP;
                        break;
                    case 84:
                        k = KEY_DOWN;
                        break;
                    default:
                        k = kp->ch;
                        break;
                    }
                    key(k, 0);
                }
                break;
            default:
                idle();
                break;
            }
        }

	return 0;
}


/*
 * Here on an unrecoverable error.
 */
void
errorcatcher(code, name, id)
	GR_ERROR	code;		/* error code */
	GR_FUNC_NAME	name;		/* function name which failed */
	GR_ID		id;		/* resource id */
{
	GrClose();
	fprintf(stderr, "DEMO ERROR: code %d, function %s, resource id %d\n",
		code, name, id);
	exit(1);
}
