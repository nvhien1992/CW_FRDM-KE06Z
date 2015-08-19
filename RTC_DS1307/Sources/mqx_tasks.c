/* ###################################################################
 **     Filename    : mqx_tasks.c
 **     Project     : RTC_DS1307
 **     Processor   : MKE06Z128VLK4
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2015-08-19, 14:33, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Settings    :
 **     Contents    :
 **         Task1_task - void Task1_task(uint32_t task_init_data);
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
#include "DS1307.h"

/*
 ** ===================================================================
 **     Event       :  Task1_task (module mqx_tasks)
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
void Task1_task(uint32_t task_init_data) {
	int counter = 0;

	RTC_reinit();

	uint8_t time_buf[7];
	time_buf[0] = 30;
	time_buf[1] = 15;
	time_buf[2] = 13;
	time_buf[3] = 4;
	time_buf[4] = 12;
	time_buf[5] = 8;
	time_buf[6] = 15;

	RTC_set_time(RTC_TIME, time_buf);
	uint8_t get_time[7];
	RTC_get_time(RTC_TIME, get_time);
	uint8_t i = 0;
	for (i = 0; i < 7; i++) {
		printf("time: %d\n", get_time[i]);
	}

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
