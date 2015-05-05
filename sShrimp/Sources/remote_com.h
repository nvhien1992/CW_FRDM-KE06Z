/**
 @brief Implementing functions for SWM Project
 @author Huynh Trung Tin <tinvuong51003405@gmail.com>
 Nguyen Van Hien <nvhien1992@gmail.com>
 @copyright Copyright (C) 2015 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */

#ifndef REMOTE_COM_H_
#define REMOTE_COM_H_

#include "message_type.h"
#include "SIM900A.h"
#include "cir_queue.h"

/*=======================================================================
 ===========================SHARED DEFINITIONS===========================
 =======================================================================*/
/**
 * 
 */
void remote_com_app(pointer rcom_msg_queue, pointer controller_msg_queue,
		cir_queue_t *rcom_to_controller_queue,
		cir_queue_t *controller_to_rcom_queue);

/**
 * 
 */
void remote_com_RI_processing(pointer rcom_msg_queue,
		pointer controller_msg_queue, cir_queue_t *rcom_to_controller_queue);

#endif //REMOTE_COM_H_
