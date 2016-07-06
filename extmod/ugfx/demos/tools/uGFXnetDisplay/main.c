/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"
#include "drivers/multiple/uGFXnet/uGFXnetProtocol.h"

#ifndef GDISP_GFXNET_PORT
	#define GDISP_GFXNET_PORT	GNETCODE_DEFAULT_PORT
#endif
// This definition is only required for for O/S's that don't support a command line eg ChibiOS
// It is ignored by those that do support a command line.
#ifndef GDISP_GFXNET_HOST
	#define GDISP_GFXNET_HOST		"127.0.0.1"					// Change this to your uGFXnet host.
#endif

// Do we wish to use old style socket calls. Some socket libraries only support the old version.
// It is better to use the new version where possible however as it also supports IPv6.
#ifndef OLD_STYLE_SOCKETS
	#define OLD_STYLE_SOCKETS		FALSE
#endif

// Which operating systems support a command line
#if defined(WIN32) || GFX_USE_OS_WIN32 || GFX_USE_OS_OSX || GFX_USE_OS_LINUX
	#define EMBEDED_OS	FALSE
#else
	#define EMBEDED_OS	TRUE
#endif

#if GNETCODE_VERSION != GNETCODE_VERSION_1_0
	#error "This uGFXnet display only supports protocol V1.0"
#endif
#if GDISP_PIXELFORMAT != GNETCODE_PIXELFORMAT
	#error "Oops - The uGFXnet protocol requires a different pixel format. Try defining GDISP_PIXELFORMAT in your gfxconf.h file."
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(WIN32) || GFX_USE_OS_WIN32
	#if OLD_STYLE_SOCKETS
		#include <winsock.h>
	#else
		#include <ws2tcpip.h>
		#include <winsock2.h>
	#endif
	#define SOCKET_TYPE				SOCKET

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
	#include <lwip/netdb.h>

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
	#if !LWIP_DNS
		#error "GDISP: uGFXnet - LWIP_DNS must be defined in your lwipopts.h file"
	#endif
	#define SOCKET_TYPE				int

	// Mutex protection is required for LWIP
	#if !GDISP_GFXNET_UNSAFE_SOCKETS
		#warning "GDISP: uGFXnet - LWIP sockets are not thread-safe. GDISP_GFXNET_UNSAFE_SOCKETS has been turned on for you."
		#undef GDISP_GFXNET_UNSAFE_SOCKETS
		#define GDISP_GFXNET_UNSAFE_SOCKETS	TRUE
	#endif
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	static GListener				gl;
#endif
static SOCKET_TYPE				netfd = (SOCKET_TYPE)-1;
static font_t					font;

#define STRINGOF_RAW(s)		#s
#define STRINGOF(s)			STRINGOF_RAW(s)

#if EMBEDED_OS
	#define cmd_args
	#define proto_args	void
	#define xhost		GDISP_GFXNET_HOST
	#define xport		STRINGOF(GDISP_GFXNET_HOST)
	#define xportnum	GDISP_GFXNET_HOST
#else
	#define cmd_args	argc, argv
	#define proto_args	int argc, char **argv
	static char *		xhost;
	static char *		xport;
	#if OLD_STYLE_SOCKETS
		static int		xportnum = GDISP_GFXNET_PORT;
	#endif
#endif

/**
 * Get a whole packet of data.
 * Len is specified in the number of uint16_t's we want as our protocol only talks uint16_t's.
 * If the connection closes before we get all the data - the call returns FALSE.
 */
static bool_t getpkt(uint16_t *pkt, int len) {
	int		got;
	int		have;

	// Get the packet of data
	len *= sizeof(uint16_t);
	have = 0;
	while(len && (got = recv(netfd, ((char *)pkt)+have, len, 0)) > 0) {
		have += got;
		len -= got;
	}
	if (len)
		return FALSE;

	// Convert each uint16_t to host order
	for(got = 0, have /= 2; got < have; got++)
		pkt[got] = ntohs(pkt[got]);

	return TRUE;
}

/**
 * Send a whole packet of data.
 * Len is specified in the number of uint16_t's we want to send as our protocol only talks uint16_t's.
 * Note that contents of the packet are modified to ensure it will cross the wire in the correct format.
 * If the connection closes before we send all the data - the call returns FALSE.
 */
static bool_t sendpkt(uint16_t *pkt, int len) {
	int		i;

	// Convert each uint16_t to network order
	for(i = 0; i < len; i++)
		pkt[i] = htons(pkt[i]);

	// Send it
	len *= sizeof(uint16_t);
	return send(netfd, (const char *)pkt, len, 0) == len;
}

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	/**
	 * We use a separate thread to capture mouse events and send them down the pipe.
	 * We do the send in a single transaction to prevent it getting interspersed with
	 * any reply we need to send on the main thread.
	 */
	static DECLARE_THREAD_STACK(waNetThread, 512);
	static DECLARE_THREAD_FUNCTION(NetThread, param) {
		GEventMouse				*pem;
		uint16_t				cmd[2];
		uint16_t				lbuttons;
		coord_t					lx, ly;
		(void)					param;

		// Initialize the mouse and the listener.
		geventListenerInit(&gl);
		geventAttachSource(&gl, ginputGetMouse(0), GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA);
		lbuttons = 0;
		lx = ly = -1;

		while(1) {
			// Get a (mouse) event
			pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
			if (pem->type != GEVENT_MOUSE && pem->type != GEVENT_TOUCH)
				continue;

			// Nothing to do if the socket is not open
			if (netfd == (SOCKET)-1)
				continue;

			// Nothing to do if the mouse data has not changed
			if (lx == pem->x && ly == pem->y && lbuttons == pem->buttons)
				continue;

			// Transfer mouse data that has changed
			if (lx != pem->x) {
				lx = pem->x;
				cmd[0] = GNETCODE_MOUSE_X;
				cmd[1] = lx;
				sendpkt(cmd, 2);
			}
			if (ly != pem->y) {
				ly = pem->y;
				cmd[0] = GNETCODE_MOUSE_Y;
				cmd[1] = ly;
				sendpkt(cmd, 2);
			}
			// We always send the buttons as it also acts as a mouse sync signal
			lbuttons = pem->buttons;
			cmd[0] = GNETCODE_MOUSE_B;
			cmd[1] = lbuttons;
			sendpkt(cmd, 2);
		}
		return 0;
	}
#endif

/**
 * Do the connection to the remote host.
 * We have two prototypes here - one for embedded systems and one for systems with a command line.
 * We have two methods of using the sockets library - one very old style and the other the more modern approach.
 */
static SOCKET_TYPE doConnect(proto_args) {
	SOCKET_TYPE			fd;

	#if !EMBEDED_OS
		(void)			argc;

		// Parse the command line arguments
		xhost = 0;
		xport = 0;
		while (*++argv) {
			if (!xhost)
				xhost = argv[0];
			else if (!xport) {
				xport = argv[0];
				#if OLD_STYLE_SOCKETS
					if (sscanf(xport, "%i", &xportnum) != 1 || xportnum >= 65536 || xportnum <= 0) {
						fprintf(stderr, "Error: Bad port specification '%s'\n\n", xport);
						goto usage;
					}
				#endif
			} else {
				fprintf(stderr, "Error: Unknown argument '%s'\n\n", argv[0]);
				goto usage;
			}
		}

		// Check the command line arguments were valid.
		if (!xport)
			xport = STRINGOF(GDISP_GFXNET_PORT);
		if (!xhost) {
		usage:
			fprintf(stderr, "Usage: uGFXnetDisplay host [port]\n");
			exit(1);
		}
	#endif

	#if OLD_STYLE_SOCKETS
		struct sockaddr_in	serv_addr;
		struct hostent *	h;

		h = gethostbyname(xhost);
		if (!h)
			// Error: Unable to find an ip-address for the specified server
			return (SOCKET_TYPE)-1;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_port = htons(xportnum);
		serv_addr.sin_family = h->h_addrtype;
		memcpy(&serv_addr.sin_addr, h->h_addr_list[0], h->h_length);
		if ((fd = socket(serv_addr.sin_family, SOCK_STREAM, 0)) == (SOCKET_TYPE)-1)
			// Error: Socket failed
			return (SOCKET_TYPE)-1;
		if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
			// Error: Could not connect to the specified server
			closesocket(fd);
			fd = (SOCKET_TYPE)-1;
		}

	#else
		struct addrinfo	hints, *servinfo, *p;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		fd = (SOCKET_TYPE)-1;

		if (getaddrinfo(xhost, xport, &hints, &servinfo) != 0)
			// Error: Unable to find an ip-address for the specified server
			return (SOCKET_TYPE)-1;
		for(p = servinfo; p; p = p->ai_next) {
			if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == (SOCKET_TYPE)-1)
				continue;
			if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
				closesocket(fd);
				fd = (SOCKET_TYPE)-1;
				continue;
			}
			break;
		}
		freeaddrinfo(servinfo);
	#endif

	return fd;
}

/**
 * Our main function.
 * There are two prototypes - one for systems with a command line and one for embedded systems without one.
 */
int main(proto_args) {
	uint16_t			cmd[5];
	unsigned			cnt;


	// Initialize and clear the display
	gfxInit();
	font = gdispOpenFont("UI2");

	// Open the connection
	gdispDrawStringBox(0, 0, gdispGetWidth(), gdispGetHeight(), "Connecting to host...", font, White, justifyCenter);
	StartSockets();
	netfd = doConnect(cmd_args);
	if (netfd == (SOCKET_TYPE)-1)
		gfxHalt("Could not connect to the specified server");
	gdispClear(Black);

	// Get the initial packet from the host
	if (!getpkt(cmd, 2)) goto alldone;
	if (cmd[0] != GNETCODE_INIT || cmd[1] != GNETCODE_VERSION)
		gfxHalt("Oops - The protocol doesn't look like one we understand");

	// Get the rest of the initial arguments
	if (!getpkt(cmd, 4)) goto alldone;						// cmd[] = width, height, pixelformat, hasmouse

	// We will ignore size mismatches but the pixel format must match
	if (cmd[2] != GDISP_PIXELFORMAT)
		gfxHalt("Oops - The remote display is using a different pixel format to us.\nTry defining GDISP_PIXELFORMAT in your gfxconf.h file.");

	#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
		// Start the mouse thread if needed
		if (cmd[3])
			gfxThreadClose(gfxThreadCreate(waNetThread, sizeof(waNetThread), HIGH_PRIORITY, NetThread, 0));
	#endif

	// Process incoming instructions
	while(getpkt(cmd, 1)) {
		switch(cmd[0]) {
		case GNETCODE_FLUSH:
			gdispFlush();
			break;
		case GNETCODE_PIXEL:
			if (!getpkt(cmd, 3)) goto alldone;				// cmd[] = x, y, color
			gdispDrawPixel(cmd[0], cmd[1], cmd[2]);
			break;
		case GNETCODE_FILL:
			if (!getpkt(cmd, 5)) goto alldone;				// cmd[] = x, y, cx, cy, color
			gdispFillArea(cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
			break;
		case GNETCODE_BLIT:
			if (!getpkt(cmd, 4)) goto alldone;				// cmd[] = x, y, cx, cy		- Followed by cx * cy pixels
			gdispStreamStart(cmd[0],cmd[1],cmd[2],cmd[3]);
			for(cnt = (unsigned)cmd[2] * cmd[3]; cnt; cnt--) {
				if (!getpkt(cmd, 1)) goto alldone;
				gdispStreamColor(cmd[0]);
			}
			gdispStreamStop();
			break;
		#if GDISP_NEED_PIXELREAD
			case GNETCODE_READ:
				if (!getpkt(cmd, 2)) goto alldone;				// cmd[] = x, y				- Response is GNETCODE_READ,color
				cmd[1] = gdispGetPixelColor(cmd[0], cmd[1]);
				cmd[0] = GNETCODE_READ;
				if (!sendpkt(cmd, 2)) goto alldone;
				break;
		#endif
		#if GDISP_NEED_SCROLL
			case GNETCODE_SCROLL:
				if (!getpkt(cmd, 5)) goto alldone;				// cmd[] = x, y, cx, cy, lines
				gdispVerticalScroll(cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], Black);
				break;
		#endif
		case GNETCODE_CONTROL:
			if (!getpkt(cmd, 2)) goto alldone;				// cmd[] = what,data		- Response is GNETCODE_CONTROL, 0x0000 (fail) or GNETCODE_CONTROL, 0x0001 (success)
			gdispControl(cmd[0], (void *)(unsigned)cmd[1]);
			switch(cmd[0]) {
			case GDISP_CONTROL_ORIENTATION:
				cmd[1] = (uint16_t)gdispGetOrientation() == cmd[1] ? 1 : 0;
				break;
			case GDISP_CONTROL_POWER:
				cmd[1] = (uint16_t)gdispGetPowerMode() == cmd[1] ? 1 : 0;
				break;
			case GDISP_CONTROL_BACKLIGHT:
				cmd[1] = (uint16_t)gdispGetBacklight() == cmd[1] ? 1 : 0;
				break;
			default:
				cmd[1] = 0;
				break;
			}
			cmd[0] = GNETCODE_CONTROL;
			if (!sendpkt(cmd, 2)) goto alldone;
			break;
		default:
			gfxHalt("Oops - The host has sent invalid commands");
			break;
		}
	}

alldone:
	closesocket(netfd);
	gfxHalt("Connection closed");
	return 0;
}
