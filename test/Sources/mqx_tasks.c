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

#define SYSTICK_PERIOD 5 //ms
typedef struct {
	/* ==== user can modify this content ==== */
	uint8_t digit;
	uint16_t buffer[3];
} mesg_t;

/* Definitions for LW Message Queue Component */
#define NUM_MESSAGES		(16)
#define REAL_MSG_SIZE		(sizeof(mesg_t))
/* using when declare msg queue buffer */
#define MSG_SIZE_ALIGNED	((REAL_MSG_SIZE % sizeof(_mqx_max_type) == 0) ? \
							REAL_MSG_SIZE : \
							sizeof(_mqx_max_type)*(REAL_MSG_SIZE / sizeof(_mqx_max_type) + 1))
/* using when initialize msg queue because of reading 4bytes once time in receiving */
#define MSG_SIZE_MAX_TYPE_ALIGNED	(MSG_SIZE_ALIGNED / sizeof(_mqx_max_type))

/* Use light weight message queues */
uint8_t ctrl_msg_queue[sizeof(LWMSGQ_STRUCT) + NUM_MESSAGES * MSG_SIZE_ALIGNED];

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

	printf("real msg size: %d\n", REAL_MSG_SIZE);
	printf("msg size aligned: %d\n", MSG_SIZE_ALIGNED);
	printf("msg size aligned with mqx_max_type: %d\n",
			MSG_SIZE_MAX_TYPE_ALIGNED);

	mesg_t msg;
	msg.digit = 0;
	/* init msg queue */
	_lwmsgq_init((pointer) ctrl_msg_queue, NUM_MESSAGES,
			MSG_SIZE_MAX_TYPE_ALIGNED);

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
		_mqx_uint ticks = 2 * 1000 / SYSTICK_PERIOD; //timeout 2s
		if (_lwevent_wait_ticks(&an_event, EVT_BIT_MASK, TRUE,
				ticks) == LWEVENT_WAIT_TIMEOUT) {
			printf("lwevent timeout\n");
		}
		GPIO1_ToggleFieldBits(device_data, led_red, 0xFFFFFFFF);
		_time_delay_ticks(10);
		msg.digit++;
		msg.buffer[0] = 25;
		msg.buffer[1] = 5;
		msg.buffer[2] = 92;
		_lwmsgq_send((pointer) ctrl_msg_queue, (_mqx_max_type_ptr) &msg,
				LWMSGQ_SEND_BLOCK_ON_FULL);
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

	mesg_t msg;

	while (1) {
		counter++;

		/* Write your code here ... */
		_lwmsgq_receive((pointer) ctrl_msg_queue, (_mqx_max_type_ptr) &msg,
				LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
		printf("digit: %d\nbuffer: %d-%d-%d\n", msg.digit, msg.buffer[0],
				msg.buffer[1], msg.buffer[2]);
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
