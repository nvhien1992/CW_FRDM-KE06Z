/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : task_template_list.c
**     Project     : sHEATer
**     Processor   : MKE06Z128VLK4
**     Version     : Component 01.110, Driver 01.00, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-02-12, 15:51, # CodeGen: 100
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
** @file task_template_list.c                                                  
** @version 01.00
*/         
/*!
**  @addtogroup task_template_list_module task_template_list module documentation
**  @{
*/         

#include "task_template_list.h"
#include "mqx_tasks.h"
  
#define TASK_TEMPLATE_LIST_END         {0, 0, 0, 0, 0, 0, 0}

/* MQX task template list */
const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
  /* Task: CTRL */
  {
  /* Task number                      */  CTRL_TASK,
  /* Entry point                      */  (TASK_FPTR)Ctrl_task,
  /* Stack size                       */  CTRL_TASK_STACK_SIZE,
  /* Task priority                    */  8U,
  /* Task name                        */  "ctrl",
  /* Task attributes                  */  (MQX_AUTO_START_TASK),
  /* Task parameter                   */  (uint32_t)(0)
  },
  /* Task: ADC */
  {
  /* Task number                      */  ADC_TASK,
  /* Entry point                      */  (TASK_FPTR)ADC_task,
  /* Stack size                       */  ADC_TASK_STACK_SIZE,
  /* Task priority                    */  7U,
  /* Task name                        */  "adc",
  /* Task attributes                  */  (0),
  /* Task parameter                   */  (uint32_t)(0)
  },
  /* Task: Button */
  {
  /* Task number                      */  BUTTON_TASK,
  /* Entry point                      */  (TASK_FPTR)Button_task,
  /* Stack size                       */  BUTTON_TASK_STACK_SIZE,
  /* Task priority                    */  6U,
  /* Task name                        */  "button",
  /* Task attributes                  */  (0),
  /* Task parameter                   */  (uint32_t)(0)
  },
  /* Task: Timer */
  {
  /* Task number                      */  TIMER_TASK,
  /* Entry point                      */  (TASK_FPTR)Timer_task,
  /* Stack size                       */  TIMER_TASK_STACK_SIZE,
  /* Task priority                    */  7U,
  /* Task name                        */  "timer",
  /* Task attributes                  */  (0),
  /* Task parameter                   */  (uint32_t)(0)
  },
  TASK_TEMPLATE_LIST_END
};

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
