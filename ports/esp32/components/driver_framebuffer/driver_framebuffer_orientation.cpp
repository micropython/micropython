/*
 * The functions in this file translate coordinates between
 * the point-of-view of the user and the point-of-view of the
 * buffer in memory when changing the orientation of the framebuffer
 * or the orientation of a compositor window.
 * 
 * Additionally the functions in this file allow for setting and 
 * querying the orientation for both the global framebuffer and
 * for each of the compositor windows.
 */

#include "include/driver_framebuffer_internal.h"

#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE

#define TAG "fb-orientation"

/* Variables */
enum Orientation globalOrientation;

/* Private functions */
inline enum Orientation* _getOrientationContext(Window* window)
{
	if (window == NULL) {
		//No window provided, use global context
		return &globalOrientation;
	} else {
		//Window provided, use window context
		return &window->orientation;
	}
}

inline void _getSizeContext(Window* window, int16_t* width, int16_t* height)
{
	if (window == NULL) {
		//No window provided, use global context
		*width       = FB_WIDTH;
		*height      = FB_HEIGHT;
	} else {
		//Window provided, use window context
		*width       = window->width;
		*height      = window->height;
	}
}

/* Public functions */

enum Orientation driver_framebuffer_get_orientation(Window* window)
{
	enum Orientation *orientation = _getOrientationContext(window);
	return *orientation;
}

void driver_framebuffer_set_orientation(Window* window, enum Orientation newOrientation)
{
	enum Orientation *orientation = _getOrientationContext(window);
	*orientation = newOrientation;
}

uint16_t driver_framebuffer_get_orientation_angle(Window* window)
{
	enum Orientation *orientation = _getOrientationContext(window);
	switch (*orientation) {
#ifdef CONFIG_DRIVER_FRAMEBUFFER_FLIP
		case portrait:
			return 270;
		case reverse_landscape:
			return 0;
		case reverse_portrait:
			return 90;
		default:
		case landscape:
			return 180;
#else
		case portrait:
			return 90;
		case reverse_landscape:
			return 180;
		case reverse_portrait:
			return 270;
		default:
		case landscape:
			return 0;
#endif
	}
}

void driver_framebuffer_set_orientation_angle(Window* window, uint16_t angle)
{
	enum Orientation *orientation = _getOrientationContext(window);
#ifdef CONFIG_DRIVER_FRAMEBUFFER_FLIP
	switch (angle % 360) {
		case 270:
			*orientation = portrait;
			break;
		case 0:
			*orientation = reverse_landscape;
			break;
		case 90:
			*orientation = reverse_portrait;
			break;
		default:
			*orientation = landscape;
			break;
	}
#else
	switch (angle % 360) {
		case 90:
			*orientation = portrait;
			break;
		case 180:
			*orientation = reverse_landscape;
			break;
		case 270:
			*orientation = reverse_portrait;
			break;
		default:
			*orientation = landscape;
			break;
	}
#endif
}

bool driver_framebuffer_orientation_apply(Window* window, int16_t* x, int16_t* y)
{
	enum Orientation *orientation = _getOrientationContext(window);
	int16_t width, height;
	_getSizeContext(window, &width, &height);
	
	if (*orientation == portrait || *orientation == reverse_portrait) { //90 degrees
		int16_t t = *y;
		*y = *x;
		*x = (width-1)-t;
	}
	
	if (*orientation == reverse_landscape || *orientation == portrait) { //180 degrees
		*y = (height-1)-*y;
		*x = (width-1)-*x;
	}
	
	return (*x >= 0) && (*x < width) && (*y >= 0) && (*y < height);
}

void driver_framebuffer_orientation_revert(Window* window, int16_t* x, int16_t* y)
{
	enum Orientation *orientation = _getOrientationContext(window);
	int16_t width, height;
	_getSizeContext(window, &width, &height);

	//printf("Orientation revert %u, %u\n", *x, *y);

	if (*orientation == reverse_landscape || *orientation == portrait) { //90 degrees
		int16_t t = *x;
		*x = *y;
		*y = (width-1)-t;
	}

	if (*orientation == portrait || *orientation == reverse_portrait) { //180 degrees
		*y = (height-1)-*y;
		*x = (width-1)-*x;
	}
}

void driver_framebuffer_orientation_revert_square(Window* window, int16_t* x0, int16_t* y0, int16_t* x1, int16_t* y1)
{
	enum Orientation *orientation = _getOrientationContext(window);
	int16_t width, height;
	_getSizeContext(window, &width, &height);

	if (*orientation == reverse_landscape || *orientation == portrait) { //90 degrees
		int16_t tx0 = *x0;
		int16_t ty0 = *y0;
		*x0 = width-*x1-1;
		//printf("x0 = %d - %d - 1\n", width, *x1);
		*y0 = height-*y1-1;
		//printf("y0 = %d - %d - 1\n", height, *y1);
		*x1 = width-tx0-1;
		//printf("x1 = %d - %d - 1\n", width, tx0);
		*y1 = height-ty0-1;
		//printf("y1 = %d - %d - 1\n", height, ty0);
	}

	if (*orientation == portrait || *orientation == reverse_portrait) { //180 degrees
		int16_t tx0 = *x0;
		int16_t tx1 = *x1;
		int16_t ty1 = *y1;
		*x0 = *y0;
		*y0 = width-tx1-1;
		*x1 = ty1;
		*y1 = width-tx0-1;
	}
}

void driver_framebuffer_get_orientation_size(Window* window, int16_t* width, int16_t* height)
{
	enum Orientation *orientation = _getOrientationContext(window);
	if ((*orientation == portrait) || (*orientation == reverse_portrait))  {
		_getSizeContext(window, height, width); //Swapped
	} else {
		_getSizeContext(window, width, height); //Normal
	}
}

#endif /* CONFIG_DRIVER_FRAMEBUFFER_ENABLE */
