/*
 * SWM_controller.h
 *
 *  Created on: Apr 22, 2015
 *      Author: nvhie_000
 */

#ifndef SWM_CONTROLLER_H_
#define SWM_CONTROLLER_H_

#include "PE_Types.h"
#include "mqxlite.h"
#include "SIM900A.h"
#include "sync_buffer.h"

/**
 * 
 */
void controller_app(pointer rcom_msg_queue, pointer controller_msg_queue);

#endif /* SWM_CONTROLLER_H_ */
