/* ###################################################################
 **     Filename    : mqx_tasks.c
 **     Project     : EEPROM_CAT25512
 **     Processor   : MKE06Z128VLK4
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2015-07-07, 21:50, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Settings    :
 **     Contents    :
 **         Test_task - void Test_task(uint32_t task_init_data);
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
#include "CAT25512.h"

/*
 ** ===================================================================
 **     Event       :  Test_task (module mqx_tasks)
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
void Test_task(uint32_t task_init_data) {
	int counter = 0;
	uint16_t num = 255;
	SROM_Reinit();
	uint8_t buff[num];
	uint16_t i = 0;
	for (i = 0; i < num; i++) {
		buff[i] = i + 1;
	}

	if (SROM_WriteValidated(0, num, buff, 1) != SROM_ERR_OK) {
		printf("write validated fail\n");
	} else {
		printf("write validated ok\n");
	}
	uint8_t rbuf[num];
	if (SROM_ReadValidated(0, num, rbuf, 1) != SROM_ERR_OK) {
		printf("read validated fail\n");
	} else {
		printf("read validated ok\n");
	}
	
	uint16_t indx = 0;
	for (indx = 0; indx < num; indx++) {
		printf("%d\n", rbuf[indx]);
	}
	
	for (i = 0; i < num; i++) {
		buff[i] = num - i;
	}

	if (SROM_WriteValidated(63, num, buff, 1) != SROM_ERR_OK) {
		printf("write validated fail\n");
	} else {
		printf("write validated ok\n");
	}
	if (SROM_ReadValidated(63, num, rbuf, 1) != SROM_ERR_OK) {
		printf("read validated fail\n");
	} else {
		printf("read validated ok\n");
	}
	
	for (indx = 0; indx < num; indx++) {
		printf("%d\n", rbuf[indx]);
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
