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
#include "sim900_int.h"

#define NOTIFY_EN 	(1)
#define DEBUG_EN	(1)
#include "debug.h"

#define SIM900_RX_BUFF_MAX_CHAR (256) //char
uint32_t ctrl_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

uint32_t remote_com_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

uart_t uart = { MB_UART_Init, //
		MB_UART_SendBlock, //
		MB_UART_ReceiveBlock, //
		NULL, //
		NULL , //
		};

SIM900_pins_t sim900_pins = { { MB_NRST_SetVal, MB_NRST_ClrVal, }, //nRST
		{ MB_NPW_SetVal, MB_NPW_ClrVal, }, //nPW
		{ MB_DTR_SetVal, MB_DTR_ClrVal, }, //DTR
		{ MB_STATUS_GetVal, }, //STATUS
		{ MB_RI_Init, }, //RI
		};

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

	/* init msg queues */
	_lwmsgq_init((pointer) ctrl_msg_queue, NUM_MESSAGES, MSG_SIZE);
	_lwmsgq_init((pointer) remote_com_msg_queue, NUM_MESSAGES, MSG_SIZE);
	
	char sim900_rx_buffer[SIM900_RX_BUFF_MAX_CHAR];

	RCOM_set_uart(&uart);
	RCOM_set_rx_buf(sim900_rx_buffer, SIM900_RX_BUFF_MAX_CHAR);
	RCOM_init();
	sim900_pin_init(&sim900_pins);
	DEBUG("init-ed\n");
	
	RCOM_job_result_t j_s;
	sim900_start_device(&j_s);
	DEBUG("started\n");
	
	sim900_config_device(&j_s);
	DEBUG("config-ed\n");
	
	sim900_send_sms(&j_s, "0947380243", "nvhien");
	DEBUG("sent\n");
	
//	if (_task_create_at(0, REMOTE_COM_TASK, 0, Remote_com_task_stack,
//			REMOTE_COM_TASK_STACK_SIZE) == MQX_NULL_TASK_ID ) {
//		NOTIFY("Error on creating Remote_com task\n");
//	}

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

//	char sim900_rx_buffer[SIM900_RX_BUFF_MAX_CHAR];
//
//	RCOM_set_uart(&uart);
//	RCOM_set_rx_buf(sim900_rx_buffer, SIM900_RX_BUFF_MAX_CHAR);
//	RCOM_init();
//	sim900_pin_init(&sim900_pins);
//	RCOM_job_result_t j_s;
//	sim900_start_device(&j_s);
//	sim900_send_sms(&j_s, "0947380243", "nvhien");

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
