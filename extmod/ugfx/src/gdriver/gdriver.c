/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GDRIVER

#include "gdriver.h"

#include <string.h>         // For memset

// Define the tables to hold the driver instances.
static GDriver *dhead;
static GDriver *dtail;

// The system initialization.
void _gdriverInit(void) {
}

// The system de-initialization.
void _gdriverDeinit(void) {
	while(dhead)
		gdriverUnRegister(dhead);
}


GDriver *gdriverRegister(const GDriverVMT *vmt, void *param) {
	GDriver *	pd;
	unsigned	dinstance, sinstance;

	// Loop to find the driver instance and the system instance numbers
	dinstance = sinstance = 0;
	for(pd = dhead; pd; pd = pd->driverchain) {
		if (pd->vmt == vmt)
			dinstance++;
		if (pd->vmt->type == vmt->type)
			sinstance++;
	}

	// Get a new driver instance of the correct size and initialize it
	pd = gfxAlloc(vmt->objsize);
	if (!pd)
		return 0;
    memset(pd, 0, vmt->objsize);
	pd->vmt = vmt;
	if (vmt->init && !vmt->init(pd, param, dinstance, sinstance)) {
		gfxFree(pd);
		return 0;
	}

	// Add it to the driver chain (at the end)
	if (dhead)
		dtail->driverchain = pd;
	else
		dhead = pd;
	dtail = pd;

	// Do the post init
	if (vmt->postinit)
		vmt->postinit(pd);

	return pd;
}

void gdriverUnRegister(GDriver *driver) {
	GDriver		*pd;

	// Safety
	if (!driver)
		return;

	// Remove it from the list of drivers
	if (dhead == driver)
		dhead = driver->driverchain;
	else {
		for(pd = dhead; pd->driverchain; pd = pd->driverchain) {
			if (pd->driverchain == driver) {
				pd->driverchain = driver->driverchain;
				if (!pd->driverchain)
					dtail = pd;
				break;
			}
		}
	}

	// Call the deinit()
	if (driver->vmt->deinit)
		driver->vmt->deinit(driver);

	// Cleanup
	gfxFree(driver);
}

GDriver *gdriverGetInstance(uint16_t type, unsigned instance) {
	GDriver		*pd;
	unsigned	sinstance;

	// Loop to find the system instance
	sinstance = 0;
	for(pd = dhead; pd; pd = pd->driverchain) {
		if (pd->vmt->type == type) {
			if (sinstance == instance)
				return pd;
			sinstance++;
		}
	}
	return 0;
}

unsigned gdriverInstanceCount(uint16_t type) {
	GDriver		*pd;
	unsigned	sinstance;

	// Loop to count the system instances
	sinstance = 0;
	for(pd = dhead; pd; pd = pd->driverchain) {
		if (pd->vmt->type == type)
			sinstance++;
	}
	return sinstance;
}

GDriver *gdriverGetNext(uint16_t type, GDriver *driver) {
	driver = driver ? driver->driverchain : dhead;

	while(driver && driver->vmt->type != type)
		driver = driver->driverchain;

	return driver;
}

unsigned gdriverGetDriverInstanceNumber(GDriver *driver) {
	GDriver		*pd;
	unsigned	instance;

	// Loop to find the system instance
	instance = 0;
	for(pd = dhead; pd; pd = pd->driverchain) {
		if (pd == driver)
			return instance;
		if (pd->vmt->type == driver->vmt->type)
			instance++;
	}
	return (unsigned)-1;
}

#endif /* GFX_USE_GDRIVER */
