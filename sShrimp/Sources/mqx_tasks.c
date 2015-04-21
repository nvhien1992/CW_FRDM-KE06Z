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
#include "cir_queue.h"

#define NOTIFY_EN 	(1)
#define DEBUG_EN	(1)
#include "debug.h"

#define SIM900_RX_BUFF_MAX_CHAR (128) //char
char sim900_rx_buffer[SIM900_RX_BUFF_MAX_CHAR];

#define NUM_CTRL_MESSAGES 16
#define NUM_RCOM_MESSAGES 16
#define MESSAGE_SIZE sizeof(SWM_msg_t)

SWM_msg_t ctrl_msg_queue[sizeof(LWMSGQ_STRUCT)
		+ NUM_CTRL_MESSAGES * MESSAGE_SIZE];

SWM_msg_t rcom_msg_queue[sizeof(LWMSGQ_STRUCT)
		+ NUM_RCOM_MESSAGES * MESSAGE_SIZE];

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
DEBUG("%d\n", sizeof(SWM_msg_t));
	/* init msg queues */
	_lwmsgq_init((pointer) ctrl_msg_queue, NUM_CTRL_MESSAGES, MESSAGE_SIZE);
	_lwmsgq_init((pointer) rcom_msg_queue, NUM_RCOM_MESSAGES, MESSAGE_SIZE);

	if (_task_create_at(0, REMOTE_COM_TASK, 0, Remote_com_task_stack,
			REMOTE_COM_TASK_STACK_SIZE) == MQX_NULL_TASK_ID ) {
		NOTIFY("Error on creating Remote_com task\n");
	}

	if (_task_create_at(0, KERNEL_LOG_TASK, 0, Kernel_log_task_stack,
			KERNEL_LOG_TASK_STACK_SIZE) == MQX_NULL_TASK_ID ) {
		NOTIFY("Error on creating Remote_com task\n");
	}

	SWM_msg_t msg_ptr;

	_time_delay_ticks(200);
	msg_ptr.cmd = RCOM_START;
	_lwmsgq_send((pointer) rcom_msg_queue, (_mqx_uint*) &msg_ptr, 0);
	_time_delay_ticks(2000);
	msg_ptr.cmd = RCOM_CONFIGURE;
	_lwmsgq_send((pointer) rcom_msg_queue, (_mqx_uint*) &msg_ptr, 0);
	_time_delay_ticks(200);
	msg_ptr.cmd = RCOM_CONNECT_INTERNET;
	_lwmsgq_send((pointer) rcom_msg_queue, (_mqx_uint*) &msg_ptr, 0);
	_time_delay_ticks(200);
	msg_ptr.cmd = RCOM_GET_TIMESTAMP;
	_lwmsgq_send((pointer) rcom_msg_queue, (_mqx_uint*) &msg_ptr, 0);
//	msg_ptr.cmd = RCOM_MAKE_MISSED_CALL;
//	msg_ptr.content.value_ptr = "0947380243";
//	_lwmsgq_send((pointer) rcom_msg_queue, (_mqx_uint*) &msg_ptr, 0);
//	msg_ptr.cmd = RCOM_SEND_SMS_MSG;
//	SIM900_SMS_package_t sms_package = { "0947380243", "nvhien", };
//	msg_ptr.content.value_ptr = (char*) &sms_package;
//	_lwmsgq_send((pointer) rcom_msg_queue, (_mqx_uint*) &msg_ptr, 0);

	_mqx_uint msg = 0;
//	cir_queue_t cir_queue;
//	uint8_t queue[16];
//	cir_queue.queue_p = queue;
//	cir_queue.queue_size = 16;
//	cir_queue_init(&cir_queue, queue, 16);
//	uint8_t count = 0;
//	for (count = 0; count < 17; count++) {
//		cir_queue_add_byte(&cir_queue, count);
//	}
//	DEBUG("head: %ld\n", cir_queue.head);
//	DEBUG("tail: %ld\n", cir_queue.tail);
//	DEBUG("max_q_size: %d\n", cir_queue.queue_size);
//	if (!cir_queue_is_full(&cir_queue)) {
//		DEBUG("data size: %d\n", cir_queue_get_data_size(&cir_queue));
//	} else {
//		DEBUG("full\n");
//	}
	while (1) {
		counter++;

		/* Write your code here ... */
		_lwmsgq_receive((pointer) ctrl_msg_queue, &msg,
				LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);

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

	_mqx_uint msg;

	while (1) {
		counter++;

		/* Write your code here ... */
		_lwmsgq_receive((pointer) rcom_msg_queue, &msg,
				LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);

		remote_com_app((SWM_msg_t*) &msg, &ctrl_msg_queue);
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

	char phone_number_or_sms_content[16];

	while (1) {
		counter++;

		/* Write your code here ... */
		switch (sim900_process_RI_task(phone_number_or_sms_content)) {
		case INCOMING_VOICE_CALL:
			DEBUG("pn: %s\n", phone_number_or_sms_content);
			break;
		case RECEIVED_SMS_MSG:
			DEBUG("sms indx: %d\n", (uint8_t)*phone_number_or_sms_content);
			break;
		default:
			DEBUG("RI_UNKNOWN\n");
			break;
		}
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
