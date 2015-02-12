/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : Cpu.c
**     Project     : sHEATer
**     Processor   : MKE06Z128VLK4
**     Component   : MKE06Z128LK4
**     Version     : Component 01.011, Driver 01.00, CPU db: 3.00.000
**     Datasheet   : MKE06P80M48SF0RM, Rev. 1, Dec 2013
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-02-12, 14:12, # CodeGen: 99
**     Abstract    :
**
**     Settings    :
**
**     Contents    :
**         No public methods
**
**     (c) Freescale Semiconductor, Inc.
**     2004 All Rights Reserved
**
**     Copyright : 1997 - 2014 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file Cpu.c
** @version 01.00
** @brief
**
*/         
/*!
**  @addtogroup Cpu_module Cpu module documentation
**  @{
*/         

/* MODULE Cpu. */

/* MQX Lite include files */
#include "mqxlite.h"
#include "mqxlite_prv.h"
#include "Cpu.h"
#include "Events.h"
#include "mqx_tasks.h"
#include "Init_Config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
** ===================================================================
**     Method      :  Common_Init (component MKE06Z128LK4)
**     Description :
**         Initialization of registers for that there is no 
**         initialization component.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#if CPU_COMMON_INIT
void Common_Init(void)
{
  /* {MQXLite RTOS Adapter} Set new interrupt vector (function handler and ISR parameter) */
}

#endif /* CPU_COMMON_INIT */

/*
** ===================================================================
**     Method      :  Components_Init (component MKE06Z128LK4)
**     Description :
**         Initialization of components (with exception for Peripheral
**         Initialization Components which are initialized in 
**         Peripherals_Init() method).
**         For example, if automatic initialization feature 
**         is enabled in LDD component then its Init method call 
**         is executed in Components_Init() method.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#if CPU_COMPONENTS_INIT
void Components_Init(void)
{
  /* ### Serial_LDD "IO1" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)IO1_Init(NULL);
  /* ### BitIO_LDD "LCD_RS" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)LCD_RS_Init(NULL);
  /* ### BitIO_LDD "LCD_RW" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)LCD_RW_Init(NULL);
  /* ### BitIO_LDD "LCD_EN" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)LCD_EN_Init(NULL);
  /* ### BitIO_LDD "LCD_DB4" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)LCD_DB4_Init(NULL);
  /* ### BitIO_LDD "LCD_DB5" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)LCD_DB5_Init(NULL);
  /* ### BitIO_LDD "LCD_DB6" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)LCD_DB6_Init(NULL);
  /* ### BitIO_LDD "LCD_DB7" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)LCD_DB7_Init(NULL);
  /* ### BitIO_LDD "LCD_BL" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)LCD_BL_Init(NULL);
  /* ### BitIO_LDD "RUN_STOP_BTN" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)RUN_STOP_BTN_Init(NULL);
  /* ### BitIO_LDD "ALARM_BTN" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)ALARM_BTN_Init(NULL);
  /* ### BitIO_LDD "LED" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)LED_Init(NULL);
  /* ### BitIO_LDD "BUZZER" component auto initialization. Auto initialization feature can be disabled by component property "Auto initialization". */
  (void)BUZZER_Init(NULL);
}
#endif /* CPU_COMPONENTS_INIT */

/*
** ===================================================================
**     Method      :  Cpu_INT_NMIInterrupt (component MKE06Z128LK4)
**
**     Description :
**         This ISR services the Non Maskable Interrupt interrupt.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void Cpu_INT_NMIInterrupt(void)
{
  Cpu_OnNMI();
}


#ifdef __cplusplus
}
#endif

/* END Cpu. */

