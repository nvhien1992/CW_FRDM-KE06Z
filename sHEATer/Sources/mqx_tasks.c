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
#include <cstring>
#include "global.h"
#include "adc_ll.h"
#include "LCD.h"
#include "array_utils.h"
#include "PID.h"

#define DEBUG_EN	1
#define NOTIFY_EN	1
#include "debug.h"

#define MAX_TEMP_VALUE 		1100 //110 degree Celsious
#define DEFAULT_TEMP_VALUE 	370 //37 degree Celsious
#define CONF_DELTA_TEMP 	5 //0.5 degree
#define MAX_TIME_VALUE 		90 //min
#define MAX_ALARMS 			4
#define ACTIVE_ALARM_INDEX 	0 //index=0 is default
#define FREQ_BUZZER_BLINK 	1

/* Use light weight message queues */
uint32_t ctrl_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

/* global variables (devices) */
button_t button_table[MAX_BUTTONS];
alarm_t a_min_alarm;
sensor_t LM35;
sensor_t Ve_ref;

typedef enum {
	STOP, RUN, HOLD, UNHOLD,
} state_e;

button_t button_template = { NULL, //(*GetVal)()
		UNKNOWN, //dev_id
		0, //hold_time_count
		btn_no_pressed, //current_status
		btn_no_pressed, //old_status
		FALSE , //old_value_reg
		};

alarm_t a_min_alarm = { ALARM, //alarm_id
		FALSE, //is_active
		60, //interval_in_sec
		0, //time_cycle_count
		};

adc_t adc_module = { ADC_Init, //(*Init)()
		ADC_CreateSampleGroup, //(*CreateSampleGroup)()
		ADC_StartSingleMeasurement, //(*StartSingleMeasurement)()
		ADC_Main, //(*Main)()
		ADC_GetMeasuredValues, //(*GetMeasuredValues)()
		};

lcd_pins_t lcd_pins = { { LCD_RS_SetVal, LCD_RS_ClrVal, }, //lcd_rs
		{ LCD_EN_SetVal, LCD_EN_ClrVal, }, //lcd_en
		{ LCD_BL_SetVal, LCD_BL_ClrVal, }, //lcd_bl
		{ LCD_DB4_SetVal, LCD_DB4_ClrVal, }, //lcd_db4
		{ LCD_DB5_SetVal, LCD_DB5_ClrVal, }, //lcd_db5
		{ LCD_DB6_SetVal, LCD_DB6_ClrVal, }, //lcd_db6
		{ LCD_DB7_SetInput, LCD_DB7_SetOutput, LCD_DB7_SetVal, LCD_DB7_ClrVal,
				LCD_DB7_GetVal, }, //lcd_db7
		};

static void update_lcd(uint16_t real_temp, uint16_t conf_temp,
		uint8_t *conf_time_list);

static void update_lcd(uint16_t real_temp, uint16_t conf_temp,
		uint8_t *conf_time_list) {
//	lcd_printf("TMP:%d.%d/%d.%dC\r\nTIME:%d %d %d %d\r\n", real_temp / 10,
//			real_temp % 10, conf_temp / 10, conf_temp % 10, conf_time_list[0],
//			conf_time_list[1], conf_time_list[2], conf_time_list[3]);
	printf("TMP:%d.%d/%d.%dC\nTIME:%d %d %d %d\n", real_temp / 10,
			real_temp % 10, conf_temp / 10, conf_temp % 10, conf_time_list[0],
			conf_time_list[1], conf_time_list[2], conf_time_list[3]);
}

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

	/* Initialize Buttons */
//	memcpy(&button_table[0], &button_template, sizeof(button_t));
//	button_table[0].dev_id = RUN_STOP_BTN;
//	button_table[0].ReadInputPin = RUN_STOP_BTN_GetVal;
	memcpy(&button_table[1], &button_template, sizeof(button_t));
	button_table[1].dev_id = ALARM_BTN;
	button_table[1].GetVal = ALARM_BTN_GetVal;

	memcpy(&button_table[0], &button_template, sizeof(button_t));
	button_table[0].dev_id = ADD_BTN;
	button_table[0].GetVal = ADD_BTN_GetVal;

//	memcpy(&button_table[1], &button_template, sizeof(button_t));
//	button_table[1].dev_id = SUB_BTN;
//	button_table[1].ReadInputPin = SUB_BTN_GetVal;

	/* Initialize ADC components */
	adc_init(&adc_module);
	LM35.channel_index = 0;
	LM35.dev_id = TEMP_SS;
	LM35.sensor_value = 0;
	Ve_ref.channel_index = 1;

	/* Initialize Led/Buzzer */
	on_off_dev_t status_led;
	status_led.ClrVal = LED_ClrVal;
	status_led.SetVal = LED_SetVal;
	status_led.is_on_in_blink = FALSE;
	turn_off(&status_led);

	on_off_dev_t buzzer;
	buzzer.ClrVal = BUZZER_ClrVal;
	buzzer.SetVal = BUZZER_SetVal;
	buzzer.is_on_in_blink = FALSE;
	turn_off(&buzzer);

	/* Initialize alarm array */
	array_t alarm_arr;
	uint8_t time_list[MAX_ALARMS];
	init_array(&alarm_arr, time_list, MAX_ALARMS, sizeof(uint8_t));

	/* Initialize LCD charactor */
//	init_lcd(&lcd_pins);

	/* Initialize msg queue */
	_mqx_uint msg = 0;
	_lwmsgq_init((pointer) ctrl_msg_queue, NUM_MESSAGES, MSG_SIZE);

	/* Start misc timer */
	MISC_TIMER_Init(NULL );

	uint16_t real_temp = 0;
	uint16_t conf_temp = DEFAULT_TEMP_VALUE;
	uint8_t conf_time = 0;
	state_e sys_state = RUN; //STOP;
	state_e alarm_btn_state = UNHOLD;

	uint8_t num_alarm = 0;
	uint8_t first_time_value = 1;
	DEBUG("init-ed\n");
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
					sys_state = RUN;
					turn_on(&status_led);
					if (get_num_valid_element(&alarm_arr) > 0) {
						enable_alarm(&a_min_alarm);
					}
				}
				break;
			case (uint8_t) ADD_BTN: //increase conf_temp
				if ((uint8_t) msg != (uint8_t) btn_no_pressed) {
					if (conf_temp < MAX_TEMP_VALUE) {
						conf_temp += CONF_DELTA_TEMP;
						/* update LCD */
						update_lcd(real_temp, conf_temp, time_list);
					}
				}
				break;
			case (uint8_t) SUB_BTN: //decrease conf_temp
				if ((uint8_t) msg != (uint8_t) btn_no_pressed) {
					if (conf_temp > (real_temp + 10)) {
						conf_temp -= CONF_DELTA_TEMP;
						/* update LCD */
						update_lcd(real_temp, conf_temp, time_list);
					}
				}
				break;
			case (uint8_t) TEMP_SS: //update on LCD
				real_temp = (uint16_t) msg;
				update_lcd(real_temp, conf_temp, time_list);
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
					disable_alarm(&a_min_alarm);
					turn_off(&buzzer);
					turn_off(&status_led);
				}
				break;
			case (uint8_t) ALARM_BTN: //add/remove alarm
				if (buzzer.current_status != OFF) {
					turn_off(&buzzer);
				}
				if ((uint8_t) msg == (uint8_t) btn_on_hold) {
					alarm_btn_state = HOLD;
				} else if ((uint8_t) msg == (uint8_t) btn_no_pressed) {
					alarm_btn_state = UNHOLD;
				}
				break;
			case (uint8_t) ADD_BTN:
				if (buzzer.current_status != OFF) {
					turn_off(&buzzer);
				}

				num_alarm = get_num_valid_element(&alarm_arr);

				if (alarm_btn_state == HOLD) { //alarm button is hold
					if ((uint8_t) msg == (uint8_t) btn_pressed) {
						/* add a new alarm */
						if ((num_alarm > 0) && (num_alarm < MAX_ALARMS)) {
							insert_element_at_index(&alarm_arr,
									&first_time_value, num_alarm);
							/* update LCD */
							update_lcd(real_temp, conf_temp, time_list);
						}
					}
				} else { //un-hold
					if ((uint8_t) msg != (uint8_t) btn_no_pressed) {
						/* add the 1st alarm */
						if (num_alarm == 0) {
							insert_element_at_index(&alarm_arr,
									&first_time_value, num_alarm);
							restart_alarm(&a_min_alarm);
							/* update LCD */
							update_lcd(real_temp, conf_temp, time_list);
						}
						/* increase alarm value */
						else {
							/* get conf_time */
							if (num_alarm == 1) { //only 1 alarm in queue
								get_element_at_index(&alarm_arr, &conf_time,
										ACTIVE_ALARM_INDEX);
							} else { //more than 1 alarm in queue
								get_element_at_index(&alarm_arr, &conf_time,
										ACTIVE_ALARM_INDEX + 1);
							}
							/* modify and save conf_time just get */
							if (conf_time < MAX_TIME_VALUE) {
								conf_time++;
								if (num_alarm == 1) { //only 1 alarm in queue
									modify_element_at_index(&alarm_arr,
											&conf_time, ACTIVE_ALARM_INDEX);
								} else { //more than 1 alarm in queue
									modify_element_at_index(&alarm_arr,
											&conf_time, ACTIVE_ALARM_INDEX + 1);
								}
								restart_alarm(&a_min_alarm);
								/* update LCD */
								update_lcd(real_temp, conf_temp, time_list);
							}
						} //end else
					} //end if()
				} //end else
				break;
			case (uint8_t) SUB_BTN:
				if (buzzer.current_status != OFF) {
					turn_off(&buzzer);
				}

				num_alarm = get_num_valid_element(&alarm_arr);

				if (alarm_btn_state == HOLD) { //alarm btn is hold
					if ((uint8_t) msg == (uint8_t) btn_pressed) {
						/* remove the last alarm */
						if (num_alarm > 0) {
							delete_element_at_index(&alarm_arr, num_alarm - 1);
							/* update LCD */
							update_lcd(real_temp, conf_temp, time_list);
						}
					}
				} else { //un-hold
					/* decrease alarm value */
					if ((uint8_t) msg != (uint8_t) btn_no_pressed) {
						if (num_alarm > 0) {
							/* get conf_time */
							get_element_at_index(&alarm_arr, &conf_time,
									num_alarm - 1);
							/* modify and save conf_time just get */
							conf_time--;
							if (conf_time == 0) {
								delete_element_at_index(&alarm_arr,
										num_alarm - 1);
								if (get_num_valid_element(&alarm_arr) == 0) {
									disable_alarm(&a_min_alarm);
								}
							} else {
								modify_element_at_index(&alarm_arr, &conf_time,
										num_alarm - 1);
								restart_alarm(&a_min_alarm);
							}
							/* update LCD */
							update_lcd(real_temp, conf_temp, time_list);
						} //end if()
					} //end if()
				} //end else
				break;
			case (uint8_t) TEMP_SS: //control heater
				real_temp = (uint16_t) msg;
				/* calculate (PID) */
				//TODO
				/* control heater */
				//TODO
				/* update LCD */
				update_lcd(real_temp, conf_temp, time_list);
				break;
			case (uint8_t) ALARM: //1min period
				if (get_num_valid_element(&alarm_arr) > 0) {
					/* get conf_time */
					get_element_at_index(&alarm_arr, &conf_time,
							ACTIVE_ALARM_INDEX);
					/* modify and save conf_time */
					conf_time--;
					if (conf_time == 0) {
						delete_element_at_index(&alarm_arr, ACTIVE_ALARM_INDEX);
						blink(&buzzer, FREQ_BUZZER_BLINK);
					} else {
						modify_element_at_index(&alarm_arr, &conf_time,
								ACTIVE_ALARM_INDEX);
					}
					/* update LCD */
					update_lcd(real_temp, conf_temp, time_list);
				}
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
