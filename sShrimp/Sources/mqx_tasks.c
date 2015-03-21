/* ###################################################################
 **     Filename    : mqx_tasks.c
 **     Project     : sShrimp
 **     Processor   : MKE06Z128VLK4
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2015-03-03, 18:15, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Settings    :
 **     Contents    :
 **         Control_task - void Control_task(uint32_t task_init_data);
 **
 ** ###################################################################*/
/*!
 ** @file mqx_tasks.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup mqx_tasks_module mqx_tasks module documentation
 **  @{
 */
/* MODULE mqx_tasks */

#include "Cpu.h"
#include "Events.h"
#include "mqx_tasks.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#ifdef __cplusplus
extern "C" {
#endif 

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "global.h"

#define NOTIFY_EN 	(1)
#define DEBUG_EN	(1)
#include "debug.h"

uint32_t ctrl_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

uint32_t remote_com_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

/*
 ** ===================================================================
 **     Event       :  Control_task (module mqx_tasks)
 **
 **     Component   :  Task1 [MQXLite_task]
 **     Description :
 **         MQX task routine. The routine is generated into mqx_tasks.c
 **         file.
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **         task_init_data  - 
 **     Returns     : Nothing
 ** ===================================================================
 */
void Control_task(uint32_t task_init_data) {
	int counter = 0;

	/* init msg queue */
	_lwmsgq_init((pointer) ctrl_msg_queue, NUM_MESSAGES, MSG_SIZE);

	if (_task_create_at(0, REMOTE_COM_TASK, 0, Remote_com_task_stack,
			REMOTE_COM_TASK_STACK_SIZE) == MQX_NULL_TASK_ID ) {
		NOTIFY("Error on creating Remote_com task\n");
	}

	while (1) {
		counter++;

		/* Write your code here ... */

	}
}

/*
 ** ===================================================================
 **     Event       :  Remote_com_task (module mqx_tasks)
 **
 **     Component   :  Task2 [MQXLite_task]
 **     Description :
 **         MQX task routine. The routine is generated into mqx_tasks.c
 **         file.
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **         task_init_data  - 
 **     Returns     : Nothing
 ** ===================================================================
 */
void Remote_com_task(uint32_t task_init_data) {
	int counter = 0;

	/* init msg queue */
	_lwmsgq_init((pointer) remote_com_msg_queue, NUM_MESSAGES, MSG_SIZE);

	while (1) {
		counter++;

		/* Write your code here ... */

	}
}

/* END mqx_tasks */

#ifdef __cplusplus
} /* extern "C" */
#endif 

/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.3 [05.09]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
