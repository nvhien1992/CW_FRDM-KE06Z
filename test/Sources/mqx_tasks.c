/* ###################################################################
 **     Filename    : mqx_tasks.c
 **     Project     : test
 **     Processor   : MKE06Z128VLK4
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2015-01-10, 20:34, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Settings    :
 **     Contents    :
 **         gpio_task - void gpio_task(uint32_t task_init_data);
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

/* Definitions for LW Message Queue Component */
#define NUM_MESSAGES		16
#define MSG_SIZE			1

/* Use light weight message queues */
uint32_t ctrl_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

LWEVENT_STRUCT an_event;

/*
 ** ===================================================================
 **     Event       :  gpio_task (module mqx_tasks)
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
void gpio_task(uint32_t task_init_data) {
	int counter = 0;

	LDD_TDeviceData *device_data = NULL;
	device_data = GPIO1_Init(NULL);

	_mqx_uint msg = 1;
	/* init msg queue */
	_lwmsgq_init((pointer) ctrl_msg_queue, NUM_MESSAGES, MSG_SIZE);
	
	/* init lwevent */
	_lwevent_create(&an_event, LWEVENT_AUTO_CLEAR);

	_mqx_uint task_id = _task_create_at(0, CONSOLE_TASK, 0, console_task_stack,
			CONSOLE_TASK_STACK_SIZE);
	if (task_id == MQX_NULL_TASK_ID ) {
		printf("Error on creating console task\n");
	}

	while (1) {
		counter++;

		/* Write your code here ... */
		_lwevent_wait_for(&an_event, EVT_BIT_MASK, TRUE, NULL);
		GPIO1_ToggleFieldBits(device_data, led_red, 0xFFFFFFFF);
		_time_delay_ticks(10);
		_lwmsgq_send((pointer) ctrl_msg_queue, &msg, LWMSGQ_SEND_BLOCK_ON_FULL);
	}
}

/*
 ** ===================================================================
 **     Event       :  console_task (module mqx_tasks)
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
void console_task(uint32_t task_init_data) {
	int counter = 0;

	_mqx_uint msg = 0;

	while (1) {
		counter++;

		/* Write your code here ... */
		_lwmsgq_receive((pointer) ctrl_msg_queue, &msg,
				LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
		if (msg == 1) {
			printf("received msg\n");
		}
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
