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
#include <cstring>
#include "message_type.h"
#include "remote_com.h"
#include "SWM_controller.h"
#include "cir_queue.h"

#define NOTIFY_EN 	(1)
#define DEBUG_EN	(1)
#include "debug.h"

#define SIM900_RX_BUFF_MAX_CHAR (128) //char
char sim900_rx_buffer[SIM900_RX_BUFF_MAX_CHAR];

#define NUM_CTRL_MESSAGES 16
#define NUM_RCOM_MESSAGES 16
#define MESSAGE_SIZE sizeof(SWM_msg_t)

SWM_msg_t controller_msg_queue[sizeof(LWMSGQ_STRUCT)
		+ NUM_CTRL_MESSAGES * MESSAGE_SIZE];

SWM_msg_t rcom_msg_queue[sizeof(LWMSGQ_STRUCT)
		+ NUM_RCOM_MESSAGES * MESSAGE_SIZE];

#define RCOM_TO_CONTROLLER_BUFFER_SIZE (512)
char controller_to_rcom_buffer[RCOM_TO_CONTROLLER_BUFFER_SIZE];

#define CONTROLLER_TO_RCOM_BUFFER_SIZE (512)
char rcom_to_controller_buffer[CONTROLLER_TO_RCOM_BUFFER_SIZE];

cir_queue_t controller_to_rcom_cir_queue;
cir_queue_t rcom_to_controller_cir_queue;

uart_t SIM900_uart = { MB_UART_Init, //
		MB_UART_SendBlock, //
		MB_UART_ReceiveBlock, //
		NULL, //
		NULL, //
		};

SIM900_pins_t sim900_pins = { { MB_NRST_SetVal, MB_NRST_ClrVal, }, //nRST
		{ MB_NPW_SetVal, MB_NPW_ClrVal, }, //nPW
		{ MB_DTR_SetVal, MB_DTR_ClrVal, }, //DTR
		{ MB_STATUS_GetVal, }, //STATUS
		{ MB_RI_Enable, MB_RI_Disable, MB_RI_GetVal }, //RI
		};

static uint8_t get_systick_period_in_ms(void);

static uint8_t get_systick_period_in_ms(void) {
	uint32_t num_clock_period = 0; //HW tick.
	uint32_t input_freq_in_hz = SystemTimer1_GetInputFrequency(NULL);
	SystemTimer1_GetPeriodTicks(NULL, &num_clock_period);

	return 1000 * num_clock_period / input_freq_in_hz;
}

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
	_lwmsgq_init((pointer) controller_msg_queue, NUM_CTRL_MESSAGES,
			MESSAGE_SIZE);
	_lwmsgq_init((pointer) rcom_msg_queue, NUM_RCOM_MESSAGES, MESSAGE_SIZE);

	cir_queue_init(&controller_to_rcom_cir_queue,
			(uint8_t*) controller_to_rcom_buffer,
			CONTROLLER_TO_RCOM_BUFFER_SIZE);

	cir_queue_init(&rcom_to_controller_cir_queue,
			(uint8_t*) rcom_to_controller_buffer,
			RCOM_TO_CONTROLLER_BUFFER_SIZE);

	if (_task_create_at(0, REMOTE_COM_TASK, 0, Remote_com_task_stack,
			REMOTE_COM_TASK_STACK_SIZE) == MQX_NULL_TASK_ID ) {
		NOTIFY("Error on creating Remote_com task\n");
	}

	if (_task_create_at(0, KERNEL_LOG_TASK, 0, Kernel_log_task_stack,
			KERNEL_LOG_TASK_STACK_SIZE) == MQX_NULL_TASK_ID ) {
		NOTIFY("Error on creating Remote_com task\n");
	}

	while (1) {
		counter++;

		/* Write your code here ... */
		controller_app((pointer) rcom_msg_queue, (pointer) controller_msg_queue,
				&rcom_to_controller_cir_queue, &controller_to_rcom_cir_queue);
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

	DEBUG("initializing...\n");
	RCOM_set_uart(&SIM900_uart);
	RCOM_set_rx_buf(sim900_rx_buffer, SIM900_RX_BUFF_MAX_CHAR);
	RCOM_init(get_systick_period_in_ms());
	sim900_init(&sim900_pins);

	DEBUG("routing pin setting\n");
	/* route desired pins into peripherals */
	Pin_Settings_Init();

	/* create RI_process task to processing incoming call and received SMS */
	if (_task_create_at(0, RI_PROCCESS_TASK, 0, RI_proccess_task_stack,
			RI_PROCCESS_TASK_STACK_SIZE) == MQX_NULL_TASK_ID ) {
		NOTIFY("Error on creating RI_process task\n");
	}

	while (1) {
		counter++;

		/* Write your code here ... */

		remote_com_app((pointer) rcom_msg_queue, (pointer) controller_msg_queue,
				&rcom_to_controller_cir_queue, &controller_to_rcom_cir_queue);
	}
}

/*
 ** ===================================================================
 **     Event       :  RI_proccess_task (module mqx_tasks)
 **
 **     Component   :  Task3 [MQXLite_task]
 **     Description :
 **         MQX task routine. The routine is generated into mqx_tasks.c
 **         file.
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **         task_init_data  - 
 **     Returns     : Nothing
 ** ===================================================================
 */
void RI_proccess_task(uint32_t task_init_data) {
	int counter = 0;

	while (1) {
		counter++;

		/* Write your code here ... */
		remote_com_RI_processing((pointer) rcom_msg_queue,
				(pointer) controller_msg_queue, &rcom_to_controller_cir_queue);
	}
}

/*
 ** ===================================================================
 **     Event       :  Kernel_log_task (module mqx_tasks)
 **
 **     Component   :  Task4 [MQXLite_task]
 **     Description :
 **         MQX task routine. The routine is generated into mqx_tasks.c
 **         file.
 **     Parameters  :
 **         NAME            - DESCRIPTION
 **         task_init_data  - 
 **     Returns     : Nothing
 ** ===================================================================
 */
void Kernel_log_task(uint32_t task_init_data) {
	int counter = 0;

	while (1) {
		counter++;

		/* Write your code here ... */
		_time_delay_ticks(60000 / 5);
		_klog_show_stack_usage();
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
