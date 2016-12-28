/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

/*
 *  run_loop.h
 *
 *  Created by Matthias Ringwald on 6/6/09.
 */

#ifndef __btstack_run_loop_H
#define __btstack_run_loop_H

#include "btstack_config.h"

#include "btstack_linked_list.h"

#include <stdint.h>

#ifdef HAVE_TIME
#include <sys/time.h>
#endif

#if defined __cplusplus
extern "C" {
#endif
	
typedef struct btstack_data_source {
    btstack_linked_item_t item;
    int  fd;                                 // <-- file descriptor to watch or 0
    int  (*process)(struct btstack_data_source *ds); // <-- do processing
} btstack_data_source_t;

typedef struct btstack_timer_source {
    btstack_linked_item_t item; 
#ifdef HAVE_TIME
    struct timeval timeout;                  // <-- next timeout
#endif
#if defined(HAVE_TICK) || defined(HAVE_TIME_MS)
    uint32_t timeout;                       // timeout in system ticks (HAVE_TICK) or millis (HAVE_TIME_MS)
#endif
    // will be called when timer fired
    void  (*process)(struct btstack_timer_source *ts); 
    void * context;
} btstack_timer_source_t;

// 
typedef struct btstack_run_loop {
	void (*init)(void);
	void (*add_data_source)(btstack_data_source_t *dataSource);
	int  (*remove_data_source)(btstack_data_source_t *dataSource);
	void (*set_timer)(btstack_timer_source_t * timer, uint32_t timeout_in_ms);
	void (*add_timer)(btstack_timer_source_t *timer);
	int  (*remove_timer)(btstack_timer_source_t *timer); 
	void (*execute)(void);
	void (*dump_timer)(void);
	uint32_t (*get_time_ms)(void);
} btstack_run_loop_t;

void btstack_run_loop_timer_dump(void);


/* API_START */

/**
 * @brief Init main run loop. Must be called before any other run loop call.
 *  
 * Use btstack_run_loop_$(btstack_run_loop_TYPE)_get_instance() from btstack_run_loop_$(btstack_run_loop_TYPE).h to get instance 
 */
void btstack_run_loop_init(const btstack_run_loop_t * run_loop);

void btstack_run_loop_deInit(void);

/**
 * @brief Set timer based on current time in milliseconds.
 */
void btstack_run_loop_set_timer(btstack_timer_source_t *a, uint32_t timeout_in_ms);

/**
 * @brief Set callback that will be executed when timer expires.
 */
void btstack_run_loop_set_timer_handler(btstack_timer_source_t *ts, void (*process)(btstack_timer_source_t *_ts));

/**
 * @brief Set context for this timer
 */
void btstack_run_loop_set_timer_context(btstack_timer_source_t *ts, void * context);

/**
 * @brief Get context for this timer
 */
void * btstack_run_loop_get_timer_context(btstack_timer_source_t *ts);

/**
 * @brief Add timer source.
 */
void btstack_run_loop_add_timer(btstack_timer_source_t *timer); 

/**
 * @brief Remove timer source.
 */
int  btstack_run_loop_remove_timer(btstack_timer_source_t *timer);

/**
 * @brief Get current time in ms
 * @note 32-bit ms counter will overflow after approx. 52 days
 */
uint32_t btstack_run_loop_get_time_ms(void);

/**
 * @brief Set data source callback.
 */
void btstack_run_loop_set_data_source_handler(btstack_data_source_t *ds, int (*process)(btstack_data_source_t *_ds));

/**
 * @brief Add/Remove data source.
 */
void btstack_run_loop_add_data_source(btstack_data_source_t *dataSource);
int  btstack_run_loop_remove_data_source(btstack_data_source_t *dataSource);

/**
 * @brief Execute configured run loop. This function does not return.
 */
void btstack_run_loop_execute(void);

/* API_END */

#if defined __cplusplus
}
#endif

#endif // __btstack_run_loop_H
