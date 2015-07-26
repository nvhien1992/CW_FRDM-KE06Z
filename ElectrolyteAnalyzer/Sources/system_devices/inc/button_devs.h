/*
 * button_devs.h
 *
 *  Created on: Jul 3, 2015
 *      Author: nvhie_000
 */

#ifndef BUTTON_DEVS_H_
#define BUTTON_DEVS_H_

#include "PE_Types.h"
#include "mqxlite.h"

typedef uint8_t btn_id_t;
#define YES_BTN_ID (0x01)
#define NO_BTN_ID (0x10)
#define UNKNOWN_BTN_ID (0x00)

void buttons_init(void);

void buttons_processing_in_timer_isr(void);

void enable_buttons(void);

void disable_buttons(void);

btn_id_t waiting_button_pressed(void);

#endif /* BUTTON_DEVS_H_ */
