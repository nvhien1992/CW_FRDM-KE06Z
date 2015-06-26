/**
 @file remote_com.h
 @brief Declare functions for remote communication in SWM Project.
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright &copy; 2015, <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b>, All rights reserved. 
 */

#ifndef REMOTE_COM_H_
#define REMOTE_COM_H_

#include "PE_Types.h"
#include "mqxlite.h"
#include "SIM900A.h"
#include "sync_buffer.h"

/*=======================================================================
 ===========================SHARED DEFINITIONS===========================
 =======================================================================*/
/**
 * 
 */
void remote_com_app(pointer rcom_msg_queue, pointer controller_msg_queue,
		sync_buffer_t *rcom_to_controller_sync_buffer);

/**
 * 
 */
void remote_com_RI_processing(pointer rcom_msg_queue,
		pointer controller_msg_queue,
		sync_buffer_t *ri_to_controller_sync_buffer);

#endif //REMOTE_COM_H_
