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

/*=======================================================================
 ===========================SHARED DEFINITIONS===========================
 =======================================================================*/
/**
 * 
 */
void remote_com_app(SWM_msg_t *swm_msg, void *dest_msg_queue);

#endif //REMOTE_COM_H_

