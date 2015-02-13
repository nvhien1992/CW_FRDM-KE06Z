/* ###################################################################
 **     Filename    : mqx_tasks.c
 **     Project     : sHEATer
 **     Processor   : MKE06Z128VLK4
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2015-01-17, 11:17, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Settings    :
 **     Contents    :
 **         Ctrl_task - void Ctrl_task(uint32_t task_init_data);
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
#include <math.h>
#include "global.h"
#include "config.h"

#define DEBUG_EN	1
#define NOTIFY_EN	1
#include "debug.h"

#define MAX_TEMP 1100 //MAX_TEMP/10
#define DEFAULT_TEMP 370 //DEFAULT_TEMP/10
#define CONF_DELTA 5 // 0.5
/* Definitions for LW Message Queue Component */
#define NUM_MESSAGES            4
#define MSG_SIZE                1

/* Use light weight message queues */
uint32_t ctrl_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

button_t button_table[MAX_BUTTONS];
temp_sensor_t LM35;

LWEVENT_STRUCT adc_lwevent;
LWEVENT_STRUCT btn_lwevent;
LWEVENT_STRUCT a_min_lwevent;

typedef enum {
	STOP, RUN, HOLD, UNHOLD,
} state_e;

/*
 ** ===================================================================
 **     Event       :  Ctrl_task (module mqx_tasks)
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
void Ctrl_task(uint32_t task_init_data) {
	int counter = 0;
	conf_arr_s conf_arr;
	uint16_t real_temp = 0;
	uint16_t conf_temp = DEFAULT_TEMP;
	uint16_t alarm_time = 0;
	state_e sys_state = STOP;
	state_e alarm_btn_state = UNHOLD;

	/* initialize Buttons */
//	button_table[0].dev_id = RUN_STOP_BTN;
//	button_table[0].ReadInputPin = RUN_STOP_BTN_GetVal;
//
//	button_table[1].dev_id = ALARM_BTN;
//	button_table[1].ReadInputPin = ALARM_BTN_GetVal;

	button_table[0].dev_id = ADD_BTN;
	button_table[0].ReadInputPin = ADD_BTN_GetVal;
	
	button_table[1].dev_id = SUB_BTN;
	button_table[1].ReadInputPin = SUB_BTN_GetVal;

	uint8_t index = 0;
	for (index = 0; index < MAX_BUTTONS; index++) {
		button_table[index].arg = NULL;
		button_table[index].current_status = btn_no_pressed;
		button_table[index].old_status = btn_no_pressed;
		button_table[index].old_value_reg = false;
		button_table[index].hold_time_count = 0;
	}

	/* Initialize ADC component */
	LDD_TDeviceData *device_data = ADC_SS_Init(NULL );
	LDD_ADC_TSample SampleGroupPtr;
	SampleGroupPtr.ChannelIdx = 0;
	ADC_SS_CreateSampleGroup(device_data, &SampleGroupPtr, 1);

	LM35.StartMeasurement = ADC_SS_StartSingleMeasurement;
	LM35.ADCPolling = ADC_SS_Main;
	LM35.GetADCValue = ADC_SS_GetMeasuredValues;
	LM35.arg = device_data;
	LM35.dev_id = TEMP_SS;
	LM35.temp_value = 0.0;

	/* Initialize Led/Buzzer */
	on_off_dev_t status_led;
	status_led.ClearPin = LED_ClrVal;
	status_led.SetPin = LED_SetVal;
	status_led.arg = NULL;
	status_led.is_on_in_blink = false;
	turn_off(&status_led);

	on_off_dev_t buzzer;
	buzzer.ClearPin = BUZZER_ClrVal;
	buzzer.SetPin = BUZZER_SetVal;
	buzzer.arg = NULL;
	buzzer.is_on_in_blink = false;
	turn_off(&buzzer);

	/* Initialize the configuring array */
	init_conf_array(&conf_arr);

	/* start misc timer */
	MISC_TIMER_Init(NULL );

	/* Initialize msg queue */
	_mqx_uint msg = 0;
	_lwmsgq_init((pointer) ctrl_msg_queue, NUM_MESSAGES, MSG_SIZE);

	/* Create ADC task */
	_mqx_uint created_task = _task_create_at(0, ADC_TASK, 100, ADC_task_stack,
			ADC_TASK_STACK_SIZE);
	if (created_task == MQX_NULL_TASK_ID ) {
		NOTIFY("Error on creating ADC task.\n");
	}

	/* Create Button task */
	created_task = _task_create_at(0, BUTTON_TASK, 0, Button_task_stack,
			BUTTON_TASK_STACK_SIZE);
	if (created_task == MQX_NULL_TASK_ID ) {
		NOTIFY("Error on creating Button task.\n");
	}

	/* Create Button task */
	created_task = _task_create_at(0, TIMER_TASK, 0, Timer_task_stack,
			TIMER_TASK_STACK_SIZE);
	if (created_task == MQX_NULL_TASK_ID ) {
		NOTIFY("Error on creating Timer task.\n");
	}

	while (1) {
		counter++;

		/* Write your code here ... */
		_lwmsgq_receive((pointer) ctrl_msg_queue, &msg,
				LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL );
		
		switch (sys_state) {
		case STOP: //======//
			switch ((uint8_t) (msg >> 16)) {
			case (uint8_t) RUN_STOP_BTN: //change status
				if ((uint8_t) msg != btn_no_pressed) {
					turn_on(&status_led);
					sys_state = RUN;
				}
				break;
			case (uint8_t) ADD_BTN: //increase conf_temp
				if ((uint8_t) msg != (uint8_t) btn_no_pressed) {
					if (conf_temp < MAX_TEMP) {
						conf_temp += CONF_DELTA;
						printf("TEMP: %d.%d/%d.%dC\n", real_temp/10, real_temp%10, conf_temp/10, conf_temp%10);
					}
				}
				break;
			case (uint8_t) SUB_BTN: //decrease conf_temp
				if ((uint8_t) msg != (uint8_t) btn_no_pressed) {
					if (conf_temp > (real_temp + 10)) {
						conf_temp -= CONF_DELTA;
						printf("TEMP: %d.%d/%d.%dC\n", real_temp/10, real_temp%10, conf_temp/10, conf_temp%10);
					}
				}
				break;
			case (uint8_t) TEMP_SS: //update on LCD
				real_temp = (uint16_t) msg;
				break;
			default:
				break;
			}
			break;
		case RUN: //======//
			switch ((uint8_t) (msg >> 16)) {
			case (uint8_t) RUN_STOP_BTN: //change status
				if ((uint8_t) msg != btn_no_pressed) {
					sys_state = STOP;
					turn_off(&buzzer);
					turn_off(&status_led);
				}
				break;
			case (uint8_t) ALARM_BTN: //add/remove alarm
				if (buzzer.current_status != ON) {
					turn_off(&buzzer);
				}
				if ((uint8_t) msg == (uint8_t) btn_on_hold) {
					alarm_btn_state = HOLD;
				} else if ((uint8_t) msg == (uint8_t) btn_no_pressed) {
					alarm_btn_state = UNHOLD;
				}
				break;
			case (uint8_t) ADD_BTN: //increase alarm value
				if (buzzer.current_status != ON) {
					turn_off(&buzzer);
				}
				if (alarm_btn_state == HOLD) {
					if ((uint8_t) msg == (uint8_t) btn_pressed) {

					}
				} else {
					if ((uint8_t) msg != (uint8_t) btn_no_pressed) {

					}
				}
				break;
			case (uint8_t) SUB_BTN: //decrease alarm value
				if (buzzer.current_status != ON) {
					turn_off(&buzzer);
				}
				if (alarm_btn_state == HOLD) {
					if ((uint8_t) msg == (uint8_t) btn_pressed) {

					}
				} else {
					if ((uint8_t) msg == (uint8_t) btn_on_hold) {

					}
				}
				break;
			case (uint8_t) TEMP_SS: //control heater
				break;
			case (uint8_t) ALARM: //1min period
				DEBUG("1min\n");
				break;
			default:
				break;
			}
			break;
		default:
			NOTIFY("Unknown state!\n");
			break;
		}
	}
}

/*
 ** ===================================================================
 **     Event       :  ADC_task (module mqx_tasks)
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
void ADC_task(uint32_t task_init_data) {
	int counter = 0;
	uint16_t sensor_value;
	_mqx_uint msg;

	/* create lw event */
	if (_lwevent_create(&adc_lwevent, LWEVENT_AUTO_CLEAR) != MQX_OK) {
		NOTIFY("Make event failed\n");
		_task_block();
	}

	while (1) {
		counter++;

		/* Write your code here ... */
		if (_lwevent_wait_ticks(&adc_lwevent, (_mqx_uint) ADC_EVT_BIT_MASK,
				TRUE, 0) != MQX_OK) {
			NOTIFY("Event Wait failed\n");
			_task_block();
		}
		sensor_value = (uint16_t) (get_sensor_value(&LM35) * 10);
		msg = (_mqx_uint) (((uint32_t) get_sensor_id(&LM35) << 16)
				| sensor_value);

		_lwmsgq_send((pointer) ctrl_msg_queue, &msg, LWMSGQ_SEND_BLOCK_ON_FULL);
	}
}

/*
 ** ===================================================================
 **     Event       :  Button_task (module mqx_tasks)
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
void Button_task(uint32_t task_init_data) {
	int counter = 0;
	uint8_t index = 0;
	uint16_t btn_status;
	_mqx_uint msg = 0;

	/* create lw event */
	if (_lwevent_create(&btn_lwevent, LWEVENT_AUTO_CLEAR) != MQX_OK) {
		NOTIFY("Make btn event failed\n");
		_task_block();
	}

	while (1) {
		counter++;

		/* Write your code here ... */
		if (_lwevent_wait_ticks(&btn_lwevent, (_mqx_uint) BTN_EVT_BIT_MASK,
				TRUE, 0) != MQX_OK) {
			NOTIFY("BTN Event Wait failed\n");
			_task_block();
		}
		for (index = 0; index < MAX_BUTTONS; index++) {
			if (is_changed_status(&button_table[index])) {
				btn_status = (uint16_t) get_button_status(&button_table[index]);
				msg = (_mqx_uint) (((uint32_t) get_button_id(
						&button_table[index]) << 16) | btn_status);
				_lwmsgq_send((pointer) ctrl_msg_queue, &msg,
						LWMSGQ_SEND_BLOCK_ON_FULL);
			}
		}
	}
}

/*
 ** ===================================================================
 **     Event       :  Timer_task (module mqx_tasks)
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
void Timer_task(uint32_t task_init_data) {
	int counter = 0;
	_mqx_uint msg = 0;

	/* create lw event */
	if (_lwevent_create(&a_min_lwevent, LWEVENT_AUTO_CLEAR) != MQX_OK) {
		NOTIFY("Make a_min event failed\n");
		_task_block();
	}

	while (1) {
		counter++;

		/* Write your code here ... */
		if (_lwevent_wait_ticks(&a_min_lwevent, (_mqx_uint) A_MIN_EVT_BIT_MASK,
				TRUE, 0) != MQX_OK) {
			NOTIFY("A_MIN Event Wait failed\n");
			_task_block();
		}
		msg = (_mqx_uint) ((uint32_t) (ALARM << 16));
		_lwmsgq_send((pointer) ctrl_msg_queue, &msg, LWMSGQ_SEND_BLOCK_ON_FULL);
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
