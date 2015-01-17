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

/* Definitions for LW Message Queue Component */
#define NUM_MESSAGES            4
#define MSG_SIZE                1

/* Use light weight message queues */
uint_32 ctrl_msg_queue[sizeof(LWMSGQ_STRUCT)/sizeof(uint_32) + NUM_MESSAGES * MSG_SIZE];

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
void Ctrl_task(uint32_t task_init_data)
{
	int counter = 0;
	
	/* Initialize PWM */
	LDD_TDeviceData *device_data = PWM1_Init(NULL);
	
	/* Initialize msg queue */
	_mqx_uint msg = 0;
	_lwmsgq_init((pointer)ctrl_msg_queue, NUM_MESSAGES, MSG_SIZE);
	
	/* Create ADC task */
	_mqx_uint created_task = _task_create_at(0, ADC_TASK, 100, ADC_task_stack, ADC_TASK_STACK_SIZE);
	if(created_task == MQX_NULL_TASK_ID) {
		puts("Error on creating ADC task.\n");
	}
	while(1) {
		counter++;
		
		/* Write your code here ... */
		_lwmsgq_receive((pointer)ctrl_msg_queue, &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
		PWM1_SetRatio16(device_data, msg/(pow(2, 12))*100); //adc 12bits
		printf("duty_cycle: %d\n", (uint8_t)(msg/(pow(2, 12))*100));
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
void ADC_task(uint32_t task_init_data)
{
	int counter = 0;

	LWEVENT_STRUCT adc_lwevent;
    if (_lwevent_create(&adc_lwevent, 0) != MQX_OK) {
        printf("Make event failed\n");
        _task_block();
    }
	
	/* Initialize ADC component */
	_mqx_uint value = 0;
	LDD_TDeviceData *device_data = AD1_Init((LDD_TUserData*)(&adc_lwevent));
	LDD_ADC_TSample SampleGroupPtr;
	SampleGroupPtr.ChannelIdx = 0;
	AD1_CreateSampleGroup(device_data, &SampleGroupPtr, 1);
  
	while(1) {
		counter++;
		
		/* Write your code here ... */
		_time_delay_ticks(task_init_data);
		AD1_StartSingleMeasurement(device_data);
		if (_lwevent_wait_ticks(&adc_lwevent, 0x01, TRUE, 0) != MQX_OK) {
		  printf("Event Wait failed\n");
		  _task_block();
		}
		if (_lwevent_clear(&adc_lwevent, 0x01) != MQX_OK) {
		  printf("Event Clear failed\n");
		  _task_block();
		}
		AD1_GetMeasuredValues(device_data, &value);
		_lwmsgq_send((pointer)ctrl_msg_queue, &value, LWMSGQ_SEND_BLOCK_ON_FULL);
	}
}

/* END mqx_tasks */

#ifdef __cplusplus
}  /* extern "C" */
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
