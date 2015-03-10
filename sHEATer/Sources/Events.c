/* ###################################################################
 **     Filename    : Events.c
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
#include "global.h"

#define DEBUG_EN	1
#define NOTIFY_EN	1
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
}

/*
 ** ===================================================================
 **     Event       :  MISC_TIMER_OnCounterRestart (module Events)
 **
 **     Component   :  MISC_TIMER [TimerUnit_LDD]
 */
/*!
 **     @brief
 **         Called if counter overflow/underflow or counter is
 **         reinitialized by modulo or compare register matching.
 **         OnCounterRestart event and Timer unit must be enabled. See
 **         [SetEventMask] and [GetEventMask] methods. This event is
 **         available only if a [Interrupt] is enabled.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. The pointer passed as
 **                           the parameter of Init method.
 */
/* ===================================================================*/
void MISC_TIMER_OnCounterRestart(LDD_TUserData *UserDataPtr) {
	/* Write your code here ... */
	alarm_callback_timer_isr(ctrl_msg_queue);
	button_callback_timer_isr(button_table, MAX_BUTTONS, ctrl_msg_queue);
//	sensor_callback_timer_isr(&Ve_ref, &LM35, ctrl_msg_queue);
	on_off_blink_callback_timer_isr();
}

/*
 ** ===================================================================
 **     Event       :  ADC_OnMeasurementComplete (module Events)
 **
 **     Component   :  ADC [ADC_LDD]
 */
/*!
 **     @brief
 **         Called after measurement is done, [Interrupt service/event]
 **         is enabled, OnMeasurementComplete event is enabled and ADC
 **         device is enabled. See [SetEventMask()] method or [Event
 **         mask] property group to enable this event and [Enable]
 **         method or [Enabled in init. code] property to enable ADC
 **         device. If DMA is enabled , this event is called after the
 **         configured number of measurements and DMA transfer is done.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. The pointer is passed
 **                           as the parameter of Init method. 
 */
/* ===================================================================*/
void ADC_OnMeasurementComplete(LDD_TUserData *UserDataPtr) {
	/* Write your code here ... */
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
