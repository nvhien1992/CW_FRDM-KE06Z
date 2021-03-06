/**
 @file SWM_controller.h
 @brief
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright &copy; 2015, <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b>, All rights reserved. 
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
