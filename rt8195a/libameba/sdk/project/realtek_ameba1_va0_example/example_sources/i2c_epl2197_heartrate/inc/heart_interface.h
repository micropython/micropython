/*
 * heart_interface.h
 *
 *  Created on: 2014/4/29
 *      Author: 01004
 */

#ifndef HEART_INTERFACE_H_
#define HEART_INTERFACE_H_

#define MIN_HEART_RATE 	48
#define MAX_HEART_RATE 	180

extern int g_heartrate;

typedef void (*hr_callback)(int);

/*
 * If there is no g-sensor, fill x, y, z in 0.
 */
void add_PPG_XYZ(int ppg, short xx, short yy, short zz);

/*
 * A callback to handle heartrate events.
 */
void register_callback(hr_callback callback);

/*
 * Ex: report_period = 25.
 * it means report a heart rate every 25 samples.
 */
void start(int report_period);

void reset(void);

void stop(void);


#endif /* HEART_INTERFACE_H_ */
