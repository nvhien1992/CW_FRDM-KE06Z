/* ###################################################################
 **     Filename    : Events.c
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
 **         Cpu_OnNMI - void Cpu_OnNMI(void);
 **
 ** ###################################################################*/
/*!
 ** @file Events.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup Events_module Events module documentation
 **  @{
 */
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "mqx_tasks.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#ifdef __cplusplus
extern "C" {
#endif 

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "SIM900A.h"

#define NOTIFY_EN (1)
#define DEBUG_EN (1)
#include "debug.h"

/*
 ** ===================================================================
 **     Event       :  Cpu_OnNMI (module Events)
 **
 **     Component   :  Cpu [MKE06Z128LK4]
 */
/*!
 **     @brief
 **         This event is called when the Non maskable interrupt had
 **         occurred. This event is automatically enabled when the [NMI
 **         interrupt] property is set to 'Enabled'.
 */
/* ===================================================================*/
void Cpu_OnNMI(void) {
	/* Write your code here ... */
	DEBUG("OnNMI\n");
}

/*
 ** ===================================================================
 **     Event       :  MB_UART_OnBlockReceived (module Events)
 **
 **     Component   :  MB_UART [Serial_LDD]
 */
/*!
 **     @brief
 **         This event is called when the requested number of data is
 **         moved to the input buffer.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. This pointer is passed
 **                           as the parameter of Init method.
 */
/* ===================================================================*/
void MB_UART_OnBlockReceived(LDD_TUserData *UserDataPtr) {
	/* Write your code here ... */
	RCOM_uart_rx_callback();
	sim900_uart_callback();
}

/*
 ** ===================================================================
 **     Event       :  MB_RI_OnInterrupt (module Events)
 **
 **     Component   :  MB_RI [ExtInt]
 **     Description :
 **         This event is called when an active signal edge/level has
 **         occurred.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void MB_RI_OnInterrupt(LDD_TUserData *UserDataPtr) {
	/* Write your code here ... */
	sim900_RI_callback();
}

/* END Events */

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
