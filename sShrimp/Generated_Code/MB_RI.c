/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : MB_RI.c
**     Project     : sShrimp
**     Processor   : MKE06Z128VLK4
**     Component   : ExtInt_LDD
**     Version     : Component 02.156, Driver 01.02, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-04-06, 17:48, # CodeGen: 95
**     Abstract    :
**         This component, "ExtInt_LDD", provide a low level API 
**         for unified access of external interrupts handling
**         across various device designs.
**         The component uses one pin which generates interrupt on 
**         selected edge.
**     Settings    :
**          Component name                                 : MB_RI
**          Pin                                            : PTI6/IRQ
**          Pin signal                                     : 
**          Generate interrupt on                          : rising edge
**          Interrupt                                      : INT_IRQ
**          Interrupt priority                             : high priority
**          ISR Name                                       : MB_RI_Interrupt
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Auto initialization                          : yes
**          Threshold level                                : 0
**     Contents    :
**         Init    - LDD_TDeviceData* MB_RI_Init(LDD_TUserData *UserDataPtr);
**         Enable  - void MB_RI_Enable(LDD_TDeviceData *DeviceDataPtr);
**         Disable - void MB_RI_Disable(LDD_TDeviceData *DeviceDataPtr);
**         GetVal  - bool MB_RI_GetVal(LDD_TDeviceData *DeviceDataPtr);
**         SetEdge - LDD_TError MB_RI_SetEdge(LDD_TDeviceData *DeviceDataPtr, uint8_t Edge);
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
** @file MB_RI.c
** @version 01.02
** @brief
**         This component, "ExtInt_LDD", provide a low level API 
**         for unified access of external interrupts handling
**         across various device designs.
**         The component uses one pin which generates interrupt on 
**         selected edge.
*/         
/*!
**  @addtogroup MB_RI_module MB_RI module documentation
**  @{
*/         

/* MODULE MB_RI. */

#include "Events.h"
#include "MB_RI.h"
/* MQX Lite include files */
#include "mqxlite.h"
#include "mqxlite_prv.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct {
  LDD_RTOS_TISRVectorSettings SavedISRSettings_Int; /* {MQXLite RTOS Adapter} Saved settings of allocated interrupt vector */
  LDD_TUserData *UserData;             /* RTOS device data structure */
} MB_RI_TDeviceData, *MB_RI_TDeviceDataPtr; /* Device data structure type */

/* {MQXLite RTOS Adapter} Static object used for simulation of dynamic driver memory allocation */
static MB_RI_TDeviceData DeviceDataPrv__DEFAULT_RTOS_ALLOC;

/*
** ===================================================================
**     Method      :  MB_RI_Init (component ExtInt_LDD)
*/
/*!
**     @brief
**         This method initializes the associated peripheral(s) and the
**         component internal variables. The method is called
**         automatically as a part of the application initialization
**         code.
**     @param
**         UserDataPtr     - Pointer to the RTOS device
**                           structure. This pointer will be passed to
**                           all events as parameter.
**     @return
**                         - Pointer to the dynamically allocated private
**                           structure or NULL if there was an error.
*/
/* ===================================================================*/
LDD_TDeviceData* MB_RI_Init(LDD_TUserData *UserDataPtr)
{
  /* Allocate LDD device structure */
  MB_RI_TDeviceData *DeviceDataPrv;

  /* {MQXLite RTOS Adapter} Driver memory allocation: Dynamic allocation is simulated by a pointer to the static object */
  DeviceDataPrv = &DeviceDataPrv__DEFAULT_RTOS_ALLOC;
  /* Store the UserData pointer */
  DeviceDataPrv->UserData = UserDataPtr;
  /* Interrupt vector(s) allocation */
  /* {MQXLite RTOS Adapter} Save old and set new interrupt vector (function handler and ISR parameter) */
  /* Note: Exception handler for interrupt is not saved, because it is not modified */
  DeviceDataPrv->SavedISRSettings_Int.isrData = _int_get_isr_data(LDD_ivIndex_INT_IRQ);
  DeviceDataPrv->SavedISRSettings_Int.isrFunction = _int_install_isr(LDD_ivIndex_INT_IRQ, MB_RI_Interrupt, DeviceDataPrv);
  /* SIM_SCGC: IRQ=1 */
  SIM_SCGC |= SIM_SCGC_IRQ_MASK;
  /* Clear interrupt status flag */
  /* IRQ_SC: IRQACK=1 */
  IRQ_SC |= IRQ_SC_IRQACK_MASK;
  /* IRQ_SC: ??=0,IRQPDD=0,IRQEDG=1,IRQPE=1,IRQF=0,IRQACK=0,IRQIE=1,IRQMOD=0 */
  IRQ_SC = (IRQ_SC_IRQEDG_MASK | IRQ_SC_IRQPE_MASK | IRQ_SC_IRQIE_MASK);
  /* NVIC_IPR1: PRI_7=1 */
  NVIC_IPR1 = (uint32_t)((NVIC_IPR1 & (uint32_t)~(uint32_t)(
               NVIC_IP_PRI_7(0x02)
              )) | (uint32_t)(
               NVIC_IP_PRI_7(0x01)
              ));
  /* NVIC_ISER: SETENA31=0,SETENA30=0,SETENA29=0,SETENA28=0,SETENA27=0,SETENA26=0,SETENA25=0,SETENA24=0,SETENA23=0,SETENA22=0,SETENA21=0,SETENA20=0,SETENA19=0,SETENA18=0,SETENA17=0,SETENA16=0,SETENA15=0,SETENA14=0,SETENA13=0,SETENA12=0,SETENA11=0,SETENA10=0,SETENA9=0,SETENA8=0,SETENA7=1,SETENA6=0,SETENA5=0,SETENA4=0,SETENA3=0,SETENA2=0,SETENA1=0,SETENA0=0 */
  NVIC_ISER = NVIC_ISER_SETENA7_MASK;
  /* NVIC_ICER: CLRENA31=0,CLRENA30=0,CLRENA29=0,CLRENA28=0,CLRENA27=0,CLRENA26=0,CLRENA25=0,CLRENA24=0,CLRENA23=0,CLRENA22=0,CLRENA21=0,CLRENA20=0,CLRENA19=0,CLRENA18=0,CLRENA17=0,CLRENA16=0,CLRENA15=0,CLRENA14=0,CLRENA13=0,CLRENA12=0,CLRENA11=0,CLRENA10=0,CLRENA9=0,CLRENA8=0,CLRENA7=0,CLRENA6=0,CLRENA5=0,CLRENA4=0,CLRENA3=0,CLRENA2=0,CLRENA1=0,CLRENA0=0 */
  NVIC_ICER = 0x00U;
  /* Registration of the device structure */
  PE_LDD_RegisterDeviceStructure(PE_LDD_COMPONENT_MB_RI_ID,DeviceDataPrv);
  return ((LDD_TDeviceData *)DeviceDataPrv);
}

/*
** ===================================================================
**     Method      :  MB_RI_Enable (component ExtInt_LDD)
*/
/*!
**     @brief
**         Enable the component - the external events are accepted.
**         This method is available only if HW module allows
**         enable/disable of the interrupt.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by <Init> method.
*/
/* ===================================================================*/
void MB_RI_Enable(LDD_TDeviceData *DeviceDataPtr)
{
  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  IRQ_PDD_ClearInterrupt(IRQ_BASE_PTR);
  IRQ_PDD_EnableInterrupt(IRQ_BASE_PTR);
}

/*
** ===================================================================
**     Method      :  MB_RI_Disable (component ExtInt_LDD)
*/
/*!
**     @brief
**         Disable the component - the external events are not accepted.
**         This method is available only if HW module allows
**         enable/disable of the interrupt.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by <Init> method.
*/
/* ===================================================================*/
void MB_RI_Disable(LDD_TDeviceData *DeviceDataPtr)
{
  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  IRQ_PDD_DisableInterrupt(IRQ_BASE_PTR);
}

/*
** ===================================================================
**     Method      :  MB_RI_Interrupt (component ExtInt_LDD)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes event(s) of the component.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void MB_RI_Interrupt(LDD_RTOS_TISRParameter _isrParameter)
{
  /* {MQXLite RTOS Adapter} ISR parameter is passed as parameter from RTOS interrupt dispatcher */
  MB_RI_TDeviceDataPtr DeviceDataPrv = (MB_RI_TDeviceDataPtr)_isrParameter;

  /* Clear the interrupt flag */
  IRQ_PDD_ClearInterrupt(IRQ_BASE_PTR);
  /* Call OnInterrupt event */
  MB_RI_OnInterrupt(DeviceDataPrv->UserData);
}

/*
** ===================================================================
**     Method      :  MB_RI_GetVal (component ExtInt_LDD)
*/
/*!
**     @brief
**         Returns the actual value of the input pin of the component.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by <Init> method.
**     @return
**                         - Returned input value. Possible values:
**                           <false> - logical "0" (Low level) <true> -
**                           logical "1" (High level)
*/
/* ===================================================================*/
bool MB_RI_GetVal(LDD_TDeviceData *DeviceDataPtr)
{
  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  if ((GPIO_PDD_GetPortDataInput(GPIOC_BASE_PTR) & MB_RI_PIN_MASK) != 0U) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*
** ===================================================================
**     Method      :  MB_RI_SetEdge (component ExtInt_LDD)
*/
/*!
**     @brief
**         Sets the edge type for this component that generates the
**         interrupt.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by <Init> method.
**     @param
**         Edge            - Edge type:
**                           0 - falling edge
**                           1 - rising edge
**                           2 - both edges
**                           3 - low level
**                           4 - high level
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_RANGE - Value is out of range
*/
/* ===================================================================*/
LDD_TError MB_RI_SetEdge(LDD_TDeviceData *DeviceDataPtr, uint8_t Edge)
{

  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  /* Edge value test - this test can be disabled by setting the "Ignore range checking"
     property to the "yes" value in the "Configuration inspector" */
  if (Edge > 4U) {
    return ERR_RANGE;                  /* Invalid edge parameter */
  }
  switch (Edge) {
    case 0:                            /* Falling edge */
      IRQ_PDD_SetDetectionMode(IRQ_BASE_PTR, IRQ_PDD_EDGE_MODE);
      IRQ_PDD_SetEdgePolarity(IRQ_BASE_PTR, IRQ_PDD_FALLING_LOW);
      break;
    case 1:                            /* Rising edge */
      IRQ_PDD_SetDetectionMode(IRQ_BASE_PTR, IRQ_PDD_EDGE_MODE);
      IRQ_PDD_SetEdgePolarity(IRQ_BASE_PTR, IRQ_PDD_RISING_HIGH);
      break;
    case 3:                            /* Low level */
      IRQ_PDD_SetDetectionMode(IRQ_BASE_PTR, IRQ_PDD_LEVEL_MODE);
      IRQ_PDD_SetEdgePolarity(IRQ_BASE_PTR, IRQ_PDD_FALLING_LOW);
      break;
    case 4:                            /* High level */
      IRQ_PDD_SetDetectionMode(IRQ_BASE_PTR, IRQ_PDD_LEVEL_MODE);
      IRQ_PDD_SetEdgePolarity(IRQ_BASE_PTR, IRQ_PDD_RISING_HIGH);
      break;
    default:
      return ERR_RANGE;
  }
  return ERR_OK;
}

/* END MB_RI. */

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
