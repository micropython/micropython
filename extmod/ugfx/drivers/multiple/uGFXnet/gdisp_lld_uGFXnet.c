/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

// We need to include stdio.h below. Turn off GFILE_NEED_STDIO just for this file to prevent conflicts
#define GFILE_NEED_STDIO_MUST_BE_OFF

#include "gfx.h"

#if GFX_USE_GDISP

#define GDISP_DRIVER_VMT			GDISPVMT_uGFXnet
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"
#include "uGFXnetProtocol.h"

#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH	640
#endif
#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT	480
#endif
#ifndef GDISP_GFXNET_PORT
	#define GDISP_GFXNET_PORT	GNETCODE_DEFAULT_PORT
#endif
#ifndef GDISP_DONT_WAIT_FOR_NET_DISPLAY
	#define GDISP_DONT_WAIT_FOR_NET_DISPLAY	FALSE
#endif
#ifndef GDISP_GFXNET_UNSAFE_SOCKETS
	#define GDISP_GFXNET_UNSAFE_SOCKETS	FALSE
#endif
#ifndef GDISP_GFXNET_BROKEN_LWIP_ACCEPT
	#define GDISP_GFXNET_BROKEN_LWIP_ACCEPT		FALSE
#endif

#if GINPUT_NEED_MOUSE
	// Include mouse support code
	#define GMOUSE_DRIVER_VMT		GMOUSEVMT_uGFXnet
	#include "../../../src/ginput/ginput_driver_mouse.h"

	// Forward definitions
	static bool_t NMouseInit(GMouse *m, unsigned driverinstance);
	static bool_t NMouseRead(GMouse *m, GMouseReading *prd);

	const GMouseVMT const GMOUSE_DRIVER_VMT[1] = {{
		{
			GDRIVER_TYPE_MOUSE,
			GMOUSE_VFLG_NOPOLL|GMOUSE_VFLG_DYNAMICONLY,
				// Extra flags for testing only
				//GMOUSE_VFLG_TOUCH|GMOUSE_VFLG_SELFROTATION|GMOUSE_VFLG_DEFAULTFINGER
				//GMOUSE_VFLG_CALIBRATE|GMOUSE_VFLG_CAL_EXTREMES|GMOUSE_VFLG_CAL_TEST|GMOUSE_VFLG_CAL_LOADFREE
				//GMOUSE_VFLG_ONLY_DOWN|GMOUSE_VFLG_POORUPDOWN
			sizeof(GMouse),
			_gmouseInitDriver, _gmousePostInitDriver, _gmouseDeInitDriver
		},
		1,				// z_max
		0,				// z_min
		1,				// z_touchon
		0,				// z_touchoff
		{				// pen_jitter
			0,				// calibrate
			0,				// click
			0				// move
		},
		{				// finger_jitter
			0,				// calibrate
			2,				// click
			2				// move
		},
		NMouseInit,		// init
		0,				// deinit
		NMouseRead,		// get
		0,				// calsave
		0				// calload
	}};
#endif

#if GNETCODE_VERSION != GNETCODE_VERSION_1_0
	#error "GDISP: uGFXnet - This driver only support protocol V1.0"
#endif
#if GDISP_LLD_PIXELFORMAT != GNETCODE_PIXELFORMAT
	#error "GDISP: uGFXnet - The driver pixel format must match the protocol"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(WIN32) || GFX_USE_OS_WIN32
	#include <winsock.h>
	#define SOCKET_TYPE				SOCKET
	#define socklen_t		int

	static void StopSockets(void) {
		WSACleanup();
	}
	static void StartSockets(void) {
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
			gfxHalt("GDISP: uGFXnet - WSAStartup failed");
		atexit(StopSockets);
	}

#elif GFX_USE_OS_LINUX || GFX_USE_OS_OSX
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>

	#define closesocket(fd)			close(fd)
	#define ioctlsocket(fd,cmd,arg)	ioctl(fd,cmd,arg)
	#define StartSockets()
	#define SOCKET_TYPE				int

#else
	#include <lwip/sockets.h>

	#if GDISP_GFXNET_CUSTOM_LWIP_STARTUP
		extern void Start_LWIP(void);				// Where the application does the lwip stack setup
		#define StartSockets()		Start_LWIP();
	#else
		#include "lwipthread.h"
		#define StartSockets()		gfxThreadClose(gfxThreadCreate(wa_lwip_thread, LWIP_THREAD_STACK_SIZE, NORMAL_PRIORITY, lwip_thread, 0))
	#endif

	#if !LWIP_SOCKET
		#error "GDISP: uGFXnet - LWIP_SOCKETS must be defined in your lwipopts.h file"
	#endif
	#if !LWIP_COMPAT_SOCKETS
		#error "GDISP: uGFXnet - LWIP_COMPAT_SOCKETS must be defined in your lwipopts.h file"
	#endif
	#define SOCKET_TYPE				int

	// Mutex protection is required for LWIP
	#if !GDISP_GFXNET_UNSAFE_SOCKETS
		#warning "GDISP: uGFXnet - LWIP sockets are not thread-safe. GDISP_GFXNET_UNSAFE_SOCKETS has been turned on for you."
		#undef GDISP_GFXNET_UNSAFE_SOCKETS
		#define GDISP_GFXNET_UNSAFE_SOCKETS	TRUE
	#endif
#endif

#define GDISP_FLG_CONNECTED			(GDISP_FLG_DRIVER<<0)
#define GDISP_FLG_HAVEDATA			(GDISP_FLG_DRIVER<<1)

/*===========================================================================*/
/* Driver local routines    .                                                */
/*===========================================================================*/

typedef struct netPriv {
	SOCKET_TYPE		netfd;					// The current socket
	unsigned		databytes;				// How many bytes have been read
	uint16_t		data[2];				// Buffer for storing data read.
	#if GINPUT_NEED_MOUSE
		coord_t		mousex, mousey;
		uint16_t	mousebuttons;
		GMouse *	mouse;
	#endif
} netPriv;

static gfxThreadHandle	hThread;

#if GDISP_GFXNET_UNSAFE_SOCKETS
	static gfxMutex	uGFXnetMutex;
	#define MUTEX_INIT		gfxMutexInit(&uGFXnetMutex)
	#define MUTEX_ENTER		gfxMutexEnter(&uGFXnetMutex)
	#define MUTEX_EXIT		gfxMutexExit(&uGFXnetMutex)
#else
	#define MUTEX_INIT
	#define MUTEX_ENTER
	#define MUTEX_EXIT
#endif

/**
 * Send a whole packet of data.
 * Len is specified in the number of uint16_t's we want to send as our protocol only talks uint16_t's.
 * Note that contents of the packet are modified to ensure it will cross the wire in the correct format.
 * If the connection closes before we send all the data - the call returns FALSE.
 */
static bool_t sendpkt(SOCKET_TYPE netfd, uint16_t *pkt, int len) {
	int		i;

	// Convert each uint16_t to network order
	for(i = 0; i < len; i++)
		pkt[i] = htons(pkt[i]);

	// Send it
	len *= sizeof(uint16_t);
	return send(netfd, (const char *)pkt, len, 0) == len;
}

static bool_t newconnection(SOCKET_TYPE clientfd) {
	GDisplay *	g;
    netPriv *	priv;

	// Look for a display that isn't connected
	for(g = 0; (g = (GDisplay *)gdriverGetNext(GDRIVER_TYPE_DISPLAY, (GDriver *)g));) {
		// Ignore displays for other controllers
		#ifdef GDISP_DRIVER_LIST
			if (gvmt(g) != &GDISPVMT_uGFXnet)
				continue;
		#endif
		if (!(g->flags & GDISP_FLG_CONNECTED))
			break;
	}

	// Was anything found?
	if (!g)
		return FALSE;

	// Reset the priv area
	priv = g->priv;
	priv->netfd = clientfd;
	priv->databytes = 0;
	priv->mousebuttons = 0;

	// Send the initialisation data (2 words at a time)
	priv->data[0] = GNETCODE_INIT;
	priv->data[1] = GNETCODE_VERSION;
	sendpkt(priv->netfd, priv->data, 2);
	priv->data[0] = GDISP_SCREEN_WIDTH;
	priv->data[1] = GDISP_SCREEN_HEIGHT;
	sendpkt(priv->netfd, priv->data, 2);
	priv->data[0] = GDISP_LLD_PIXELFORMAT;
	priv->data[1] = 1;							// We have a mouse
	MUTEX_ENTER;
	sendpkt(priv->netfd, priv->data, 2);
	MUTEX_EXIT;

	// The display is now working
	g->flags |= GDISP_FLG_CONNECTED;

	// Send a redraw all
	#if GFX_USE_GWIN && GWIN_NEED_WINDOWMANAGER
		gdispGClear(g, gwinGetDefaultBgColor());
		gwinRedrawDisplay(g, FALSE);
	#endif

	return TRUE;
}

static bool_t rxdata(SOCKET_TYPE fd) {
	GDisplay *	g;
    netPriv *	priv;
    int			len;

	// Look for a display that is connected and the socket descriptor matches
	for(g = 0; (g = (GDisplay *)gdriverGetNext(GDRIVER_TYPE_DISPLAY, (GDriver *)g));) {
		// Ignore displays for other controllers
		#ifdef GDISP_DRIVER_LIST
			if (gvmt(g) != &GDISPVMT_uGFXnet)
				continue;
		#endif
		priv = g->priv;
		if ((g->flags & GDISP_FLG_CONNECTED) && priv->netfd == fd)
			break;
	}
	if (!g)
		gfxHalt("GDISP: uGFXnet - Got data from unrecognized connection");

	if ((g->flags & GDISP_FLG_HAVEDATA)) {
		// The higher level is still processing the previous data.
		//	Give it a chance to run by coming back to this data.
		gfxSleepMilliseconds(1);
		return TRUE;
	}

	/* handle data from a client */
	MUTEX_ENTER;
	if ((len = recv(fd, ((char *)priv->data)+priv->databytes, sizeof(priv->data)-priv->databytes, 0)) <= 0) {
		// Socket closed or in error state
		MUTEX_EXIT;
		g->flags &= ~GDISP_FLG_CONNECTED;
		return FALSE;
	}
	MUTEX_EXIT;

	// Do we have a full reply yet
	priv->databytes += len;
	if (priv->databytes < sizeof(priv->data))
		return TRUE;
	priv->databytes = 0;

	// Convert network byte or to host byte order
	priv->data[0] = ntohs(priv->data[0]);
	priv->data[1] = ntohs(priv->data[1]);

	// Process the data received
	switch(priv->data[0]) {
	#if GINPUT_NEED_MOUSE
		case GNETCODE_MOUSE_X:		priv->mousex = priv->data[1];		break;
		case GNETCODE_MOUSE_Y:		priv->mousey = priv->data[1];		break;
		case GNETCODE_MOUSE_B:
			priv->mousebuttons = priv->data[1];
			// Treat the button event as the sync signal
			_gmouseWakeup(priv->mouse);
			break;
	#endif
	case GNETCODE_CONTROL:
	case GNETCODE_READ:
		g->flags |= GDISP_FLG_HAVEDATA;
		break;
	case GNETCODE_KILL:
		gfxHalt("GDISP: uGFXnet - Display sent KILL command");
		break;

	default:
		// Just ignore unrecognised data
		break;
	}
	return TRUE;
}

static DECLARE_THREAD_STACK(waNetThread, 512);
static DECLARE_THREAD_FUNCTION(NetThread, param) {
	SOCKET_TYPE			listenfd, fdmax, i, clientfd;
	socklen_t			len;
	fd_set				master, read_fds;
    struct sockaddr_in	addr;
	(void)param;

	// Start the sockets layer
	StartSockets();
	gfxSleepMilliseconds(100);					// Make sure the thread has time to start.

	/* clear the master and temp sets */
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == (SOCKET_TYPE)-1)
		gfxHalt("GDISP: uGFXnet - Socket failed");

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(GDISP_GFXNET_PORT);

	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		gfxHalt("GDISP: uGFXnet - Bind failed");

    if (listen(listenfd, 10) == -1)
		gfxHalt("GDISP: uGFXnet - Listen failed");


    /* add the listener to the master set */
    FD_SET(listenfd, &master);

    /* keep track of the biggest file descriptor */
    fdmax = listenfd; /* so far, it's this one*/

	#if GDISP_GFXNET_BROKEN_LWIP_ACCEPT
		#warning "Using GDISP_GFXNET_BROKEN_LWIP_ACCEPT limits the number of displays and the use of GFXNET. Avoid if possible!"
		len = sizeof(addr);
		if((clientfd = accept(listenfd, (struct sockaddr *)&addr, &len)) == (SOCKET_TYPE)-1)
			gfxHalt("GDISP: uGFXnet - Accept failed");
		//printf("New connection from %s on socket %d\n", inet_ntoa(addr.sin_addr), clientfd);

		if (!newconnection(clientfd)) {
			// No Just close the connection
			closesocket(clientfd);
			gfxHalt("GDISP: uGFXnet - Can't find display for connection");
			return 0;
		}

		// Save the descriptor
		FD_SET(clientfd, &master);
		if (clientfd > fdmax) fdmax = clientfd;
	#endif

    /* loop */
    for(;;) {
		/* copy it */
		read_fds = master;
		if (select(fdmax+1, &read_fds, 0, 0, 0) == -1)
			gfxHalt("GDISP: uGFXnet - Select failed");

		// Run through the existing connections looking for data to be read
		for(i = 0; i <= fdmax; i++) {
			if(!FD_ISSET(i, &read_fds))
				continue;

			// Handle new connections
			if(i == listenfd) {

				// Accept the connection
				len = sizeof(addr);
				if((clientfd = accept(listenfd, (struct sockaddr *)&addr, &len)) == (SOCKET_TYPE)-1)
					gfxHalt("GDISP: uGFXnet - Accept failed");
				//printf("New connection from %s on socket %d\n", inet_ntoa(addr.sin_addr), clientfd);

				// Can we handle it?
				if (!newconnection(clientfd)) {

					// No - Just close the connection
					closesocket(clientfd);

					//printf("Rejected connection as all displays are already connected\n");
					continue;
				}

				// Save the descriptor
				FD_SET(clientfd, &master);
				if (clientfd > fdmax) fdmax = clientfd;
				continue;
			}

			// Handle data from a client
			if (!rxdata(i)) {
				closesocket(i);
				FD_CLR(clientfd, &master);
			}
		}
	}
    return 0;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	netPriv	*	priv;

	// Initialise the receiver thread (if it hasn't been done already)
	if (!hThread) {
		MUTEX_INIT;
		hThread = gfxThreadCreate(waNetThread, sizeof(waNetThread), HIGH_PRIORITY, NetThread, 0);
		gfxThreadClose(hThread);
	}

	// Create a private area for this window
	if (!(priv = gfxAlloc(sizeof(netPriv))))
		gfxHalt("GDISP: uGFXnet - Memory allocation failed");
	memset(priv, 0, sizeof(netPriv));
	g->priv = priv;
	g->board = 0;			// no board interface for this controller

	// Create the associated mouse
	#if GINPUT_NEED_MOUSE
		priv->mouse = (GMouse *)gdriverRegister((const GDriverVMT const *)GMOUSE_DRIVER_VMT, g);
	#endif

	// Initialise the GDISP structure
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Backlight = 100;
	g->g.Contrast = 50;
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;

	return TRUE;
}

#if GDISP_HARDWARE_FLUSH
	LLDSPEC void gdisp_lld_flush(GDisplay *g) {
		netPriv	*	priv;
		uint16_t	buf[1];

		#if GDISP_DONT_WAIT_FOR_NET_DISPLAY
			if (!(g->flags & GDISP_FLG_CONNECTED))
				return;
		#else
			while(!(g->flags & GDISP_FLG_CONNECTED))
				gfxSleepMilliseconds(200);
		#endif

		priv = g->priv;
		buf[0] = GNETCODE_FLUSH;
		MUTEX_ENTER;
		sendpkt(priv->netfd, buf, 1);
		MUTEX_EXIT;
	}
#endif

#if GDISP_HARDWARE_DRAWPIXEL
	LLDSPEC void gdisp_lld_draw_pixel(GDisplay *g) {
		netPriv	*	priv;
		uint16_t	buf[4];

		#if GDISP_DONT_WAIT_FOR_NET_DISPLAY
			if (!(g->flags & GDISP_FLG_CONNECTED))
				return;
		#else
			while(!(g->flags & GDISP_FLG_CONNECTED))
				gfxSleepMilliseconds(200);
		#endif

		priv = g->priv;
		buf[0] = GNETCODE_PIXEL;
		buf[1] = g->p.x;
		buf[2] = g->p.y;
		buf[3] = gdispColor2Native(g->p.color);
		MUTEX_ENTER;
		sendpkt(priv->netfd, buf, 4);
		MUTEX_EXIT;
	}
#endif

/* ---- Optional Routines ---- */

#if GDISP_HARDWARE_FILLS
	LLDSPEC void gdisp_lld_fill_area(GDisplay *g) {
		netPriv	*	priv;
		uint16_t	buf[6];

		#if GDISP_DONT_WAIT_FOR_NET_DISPLAY
			if (!(g->flags & GDISP_FLG_CONNECTED))
				return;
		#else
			while(!(g->flags & GDISP_FLG_CONNECTED))
				gfxSleepMilliseconds(200);
		#endif

		priv = g->priv;
		buf[0] = GNETCODE_FILL;
		buf[1] = g->p.x;
		buf[2] = g->p.y;
		buf[3] = g->p.cx;
		buf[4] = g->p.cy;
		buf[5] = gdispColor2Native(g->p.color);
		MUTEX_ENTER;
		sendpkt(priv->netfd, buf, 6);
		MUTEX_EXIT;
	}
#endif

#if GDISP_HARDWARE_BITFILLS
	LLDSPEC void gdisp_lld_blit_area(GDisplay *g) {
		netPriv	*	priv;
		pixel_t	*	buffer;
		uint16_t	buf[5];
		coord_t		x, y;

		#if GDISP_DONT_WAIT_FOR_NET_DISPLAY
			if (!(g->flags & GDISP_FLG_CONNECTED))
				return;
		#else
			while(!(g->flags & GDISP_FLG_CONNECTED))
				gfxSleepMilliseconds(200);
		#endif

		// Make everything relative to the start of the line
		buffer = g->p.ptr;
		buffer += g->p.x2*g->p.y1;

		priv = g->priv;
		buf[0] = GNETCODE_BLIT;
		buf[1] = g->p.x;
		buf[2] = g->p.y;
		buf[3] = g->p.cx;
		buf[4] = g->p.cy;
		MUTEX_ENTER;
		sendpkt(priv->netfd, buf, 5);

		for(y = 0; y < g->p.cy; y++, buffer += g->p.x2 - g->p.cx) {
			for(x = 0; x < g->p.cx; x++, buffer++) {
				buf[0] = gdispColor2Native(buffer[0]);
				sendpkt(priv->netfd, buf, 1);
			}
		}
		MUTEX_EXIT;
	}
#endif

#if GDISP_HARDWARE_PIXELREAD
	LLDSPEC	color_t gdisp_lld_get_pixel_color(GDisplay *g) {
		netPriv	*	priv;
		uint16_t	buf[3];
		color_t		data;

		#if GDISP_DONT_WAIT_FOR_NET_DISPLAY
			if (!(g->flags & GDISP_FLG_CONNECTED))
				return 0;
		#else
			while(!(g->flags & GDISP_FLG_CONNECTED))
				gfxSleepMilliseconds(200);
		#endif

		priv = g->priv;
		buf[0] = GNETCODE_READ;
		buf[1] = g->p.x;
		buf[2] = g->p.y;
		MUTEX_ENTER;
		sendpkt(priv->netfd, buf, 3);
		MUTEX_EXIT;

		// Now wait for a reply
		while(!(g->flags & GDISP_FLG_HAVEDATA) || priv->data[0] != GNETCODE_READ)
			gfxSleepMilliseconds(1);

		data = gdispNative2Color(priv->data[1]);
		g->flags &= ~GDISP_FLG_HAVEDATA;

		return data;
	}
#endif

#if GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL
	LLDSPEC void gdisp_lld_vertical_scroll(GDisplay *g) {
		netPriv	*	priv;
		uint16_t	buf[6];

		#if GDISP_DONT_WAIT_FOR_NET_DISPLAY
			if (!(g->flags & GDISP_FLG_CONNECTED))
				return;
		#else
			while(!(g->flags & GDISP_FLG_CONNECTED))
				gfxSleepMilliseconds(200);
		#endif

		priv = g->priv;
		buf[0] = GNETCODE_SCROLL;
		buf[1] = g->p.x;
		buf[2] = g->p.y;
		buf[3] = g->p.cx;
		buf[4] = g->p.cy;
		buf[5] = g->p.y1;
		MUTEX_ENTER;
		sendpkt(priv->netfd, buf, 6);
		MUTEX_EXIT;
	}
#endif

#if GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		netPriv	*	priv;
		uint16_t	buf[3];
		bool_t		allgood;

		#if GDISP_DONT_WAIT_FOR_NET_DISPLAY
			if (!(g->flags & GDISP_FLG_CONNECTED))
				return;
		#else
			while(!(g->flags & GDISP_FLG_CONNECTED))
				gfxSleepMilliseconds(200);
		#endif

		// Check if we might support the code
		switch(g->p.x) {
		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			break;
		case GDISP_CONTROL_POWER:
			if (g->g.Powermode == (powermode_t)g->p.ptr)
				return;
			break;
		case GDISP_CONTROL_BACKLIGHT:
			if (g->g.Backlight == (uint16_t)(int)g->p.ptr)
				return;
			if ((uint16_t)(int)g->p.ptr > 100)
				g->p.ptr = (void *)100;
			break;
		default:
			return;
		}

		// Send the command
		priv = g->priv;
		buf[0] = GNETCODE_CONTROL;
		buf[1] = g->p.x;
		buf[2] = (uint16_t)(int)g->p.ptr;
		MUTEX_ENTER;
		sendpkt(priv->netfd, buf, 3);
		MUTEX_EXIT;

		// Now wait for a reply
		while(!(g->flags & GDISP_FLG_HAVEDATA) || priv->data[0] != GNETCODE_CONTROL)
			gfxSleepMilliseconds(1);

		// Extract the return status
		allgood = priv->data[1] ? TRUE : FALSE;
		g->flags &= ~GDISP_FLG_HAVEDATA;

		// Do nothing more if the operation failed
		if (!allgood) return;

		// Update the local stuff
		switch(g->p.x) {
		case GDISP_CONTROL_ORIENTATION:
			switch((orientation_t)g->p.ptr) {
				case GDISP_ROTATE_0:
				case GDISP_ROTATE_180:
					g->g.Width = GDISP_SCREEN_WIDTH;
					g->g.Height = GDISP_SCREEN_HEIGHT;
					break;
				case GDISP_ROTATE_90:
				case GDISP_ROTATE_270:
					g->g.Height = GDISP_SCREEN_WIDTH;
					g->g.Width = GDISP_SCREEN_HEIGHT;
					break;
				default:
					return;
			}
			g->g.Orientation = (orientation_t)g->p.ptr;
			break;
		case GDISP_CONTROL_POWER:
			g->g.Powermode = (powermode_t)g->p.ptr;
			break;
		case GDISP_CONTROL_BACKLIGHT:
			g->g.Backlight = (uint16_t)(int)g->p.ptr;
			break;
		}
	}
#endif

#if GINPUT_NEED_MOUSE
	static bool_t NMouseInit(GMouse *m, unsigned driverinstance) {
		(void)	m;
		(void)	driverinstance;
		return TRUE;
	}
	static bool_t NMouseRead(GMouse *m, GMouseReading *pt) {
		GDisplay *	g;
		netPriv	*	priv;

		g = m->display;
		priv = g->priv;

		pt->x = priv->mousex;
		pt->y = priv->mousey;
		pt->z = (priv->mousebuttons & GINPUT_MOUSE_BTN_LEFT) ? 1 : 0;
		pt->buttons = priv->mousebuttons;
		return TRUE;
	}
#endif /* GINPUT_NEED_MOUSE */

#endif /* GFX_USE_GDISP */
