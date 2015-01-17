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
#include <stdio.h>

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
	
	while (1) {
		counter++;

		/* Write your code here ... */
		GPIO1_ToggleFieldBits(device_data, led_red, 0xFFFFFFFF);
		_time_delay_ticks(100);
	}
}

/*
** ===================================================================
**     Event       :  adc_task (module mqx_tasks)
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
void adc_task(uint32_t task_init_data)
{
	int counter = 0;

	uint16_t value[AD1_STATIC_GROUP_0_SAMPLE_COUNT];
	LDD_ADC_TSample SampleGroupPtr;
	LDD_TDeviceData *device_data = NULL;

	device_data = AD1_Init(NULL);
	SampleGroupPtr.ChannelIdx = 0;
	AD1_CreateSampleGroup(device_data, &SampleGroupPtr, 1);  
  
	while(1) {
		counter++;
		
		/* Write your code here ... */
		if (AD1_StartSingleMeasurement(device_data) != ERR_OK) {
			printf("error\n");
			continue;
		}
		AD1_GetMeasuredValues(device_data, &value);
		printf("dimmer: %d\n", value[0] * 100 / 4096);
	}
}

/*
** ===================================================================
**     Event       :  pwm_task (module mqx_tasks)
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
void pwm_task(uint32_t task_init_data)
{
	int counter = 0;
	
	LDD_TDeviceData *device_data = NULL;

	device_data = PWM1_Init(NULL);
	PWM1_SetDutyMS(device_data, 10);
	while(1) {
		counter++;
		
		/* Write your code here ... */
		_time_delay_ticks(100);
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
