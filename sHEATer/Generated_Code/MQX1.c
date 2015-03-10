/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : MQX1.c
**     Project     : sHEATer
**     Processor   : MKE06Z128VLK4
**     Version     : Component 01.110, Driver 01.00, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-03-10, 09:14, # CodeGen: 112
**     Abstract    :
**         MQX Lite RTOS Adapter component.
**     Settings    :
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
** @file MQX1.c
** @version 01.00
** @brief
**         MQX Lite RTOS Adapter component.
*/         
/*!
**  @addtogroup MQX1_module MQX1 module documentation
**  @{
*/         

/* MODULE MQX1. */

#include "MQX1.h"
#include "mqx_tasks.h"



/* Task stacks definition */
uint8_t CTRL_task_stack[CTRL_TASK_STACK_SIZE];

/* Task stacks array of pointers */
const uint8_t * mqx_task_stack_pointers[] = {
/* "CTRL" task                         */  CTRL_task_stack,
/* Stack array end                     */  NULL
};

/* Kernel Data Structure */
volatile KERNEL_DATA_STRUCT            _mqx_kernel_data_struct;
#define __KERNEL_DATA_START            (pointer)(&_mqx_kernel_data_struct)
#define __KERNEL_DATA_END              (pointer)((uchar_ptr)&_mqx_kernel_data_struct + sizeof(_mqx_kernel_data_struct))

/* Task ready queue */
READY_Q_STRUCT                         mqx_static_ready_queue[MQX_READY_QUEUE_ITEMS];

/* Table of 'C' handlers for interrupts. */
INTERRUPT_TABLE_STRUCT                 mqx_static_isr_table[MQX_INTERRUPT_TABLE_ITEMS];


/* Interrupt stack */
uint8_t mqx_interrupt_stack[MQX_INTERRUPT_STACK_SIZE + PSP_STACK_ALIGNMENT + 1];
/* Idle task stack */
uint8_t mqx_idle_task_stack[sizeof(TD_STRUCT) + PSP_IDLE_STACK_SIZE + PSP_STACK_ALIGNMENT + 1];

/* MQX Lite initialization structure */
const MQXLITE_INITIALIZATION_STRUCT  MQX_init_struct =
{
/* PROCESSOR NUMBER                 */  1,
/* START OF KERNEL MEMORY           */  __KERNEL_DATA_START,
/* END OF KERNEL MEMORY             */  __KERNEL_DATA_END,
/* MQX_HARDWARE_INTERRUPT_LEVEL_MAX */  1,
/* INTERRUPT STACK SIZE             */  sizeof(mqx_interrupt_stack),
/* INTERRUPT STACK LOCATION         */  mqx_interrupt_stack,
/* IDLE_TASK STACK SIZE             */  sizeof(mqx_idle_task_stack),
/* IDLE_TASK STACK_LOCATION         */  mqx_idle_task_stack,
/* TASK TEMPLATE LIST               */  (TASK_TEMPLATE_STRUCT_PTR)&MQX_template_list[0]
};

/*
** ===================================================================
**     Method      :  MQX1_OnCounterRestart (component MQXLite)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void SystemTimer1_OnCounterRestart(LDD_TUserData *UserDataPtr)
{
    _time_notify_kernel();
}


/* Function returns number of ticks per second for system timer */
uint32_t  SystemTimer1_GetTicsPerSecond(LDD_TDeviceData *DeviceDataPtr)
{
    uint32_t period_ticks;

    SystemTimer1_GetPeriodTicks(DeviceDataPtr, &period_ticks);
    return SystemTimer1_GetInputFrequency(DeviceDataPtr) / period_ticks;
}


static LDD_TDeviceDataPtr SystemTimerDeviceDataPtr;

uint32_t system_timer_init(const void * timer_data_ptr)
{
    uint32_t hw_ticks_per_tick;
    KERNEL_DATA_STRUCT_PTR kernel_data = _mqx_kernel_data;

    /* Parameter is not used, suppress unused argument warning */
    (void)timer_data_ptr;

    SystemTimerDeviceDataPtr  = SystemTimer1_Init(NULL);

    /* Set number of system ticks per second */
    kernel_data->TICKS_PER_SECOND = SystemTimer1_GetTicsPerSecond(SystemTimerDeviceDataPtr);

    _time_set_hwtick_function((MQX_GET_HWTICKS_FPTR)&SystemTimer1_GetCounterValue, (pointer)SystemTimerDeviceDataPtr);

    /* Set number of hardware ticks per system tick */
    SystemTimer1_GetPeriodTicks(SystemTimerDeviceDataPtr, &hw_ticks_per_tick);
    _time_set_hwticks_per_tick(hw_ticks_per_tick);
    
    return MQX_OK;
}

uint32_t system_timer_start(const void * timer_data_ptr)
{
    /* Parameter is not used, suppress unused argument warning */
    (void)timer_data_ptr;                 
    
    SystemTimer1_Enable(SystemTimerDeviceDataPtr);
    
    return MQX_OK;
}


/*
** ===================================================================
**     Method      :  MQX1_PEX_RTOS_InstallInterrupt (component MQXLite)
**
**     Description :
**         Installs the interrupt service routine through the RTOS.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void PEX_RTOS_InstallInterrupt(IRQInterruptIndex IntVector, void (*IsrFunction)(void *), void *IsrParam)
{
  if (IntVector >= 15) {
    /* Interrupts are install through the MQX standard ISR installation mechanism */
    (void)_int_install_isr(IntVector, (INT_ISR_FPTR)IsrFunction, IsrParam);
  } else {
    /* Interrupts are install through the MQX kernel ISR installation mechanism */
    (void)_int_install_kernel_isr(IntVector, (INT_KERNEL_ISR_FPTR)IsrFunction);
  }
}



/* END MQX1. */

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
