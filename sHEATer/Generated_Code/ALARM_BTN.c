/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : ALARM_BTN.c
**     Project     : sHEATer
**     Processor   : MKE06Z128VLK4
**     Component   : BitIO_LDD
**     Version     : Component 01.033, Driver 01.03, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-02-08, 21:04, # CodeGen: 68
**     Abstract    :
**         The HAL BitIO component provides a low level API for unified
**         access to general purpose digital input/output pins across
**         various device designs.
**
**         RTOS drivers using HAL BitIO API are simpler and more
**         portable to various microprocessors.
**     Settings    :
**          Component name                                 : ALARM_BTN
**          Pin for I/O                                    : PTH4/KBI1_P28/I2C1_SCL
**          Pin signal                                     : 
**          Direction                                      : Input
**          Initialization                                 : 
**            Init. direction                              : Input
**            Init. value                                  : 0
**            Auto initialization                          : yes
**          Safe mode                                      : no
**     Contents    :
**         Init   - LDD_TDeviceData* ALARM_BTN_Init(LDD_TUserData *UserDataPtr);
**         GetVal - bool ALARM_BTN_GetVal(LDD_TDeviceData *DeviceDataPtr);
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
** @file ALARM_BTN.c
** @version 01.03
** @brief
**         The HAL BitIO component provides a low level API for unified
**         access to general purpose digital input/output pins across
**         various device designs.
**
**         RTOS drivers using HAL BitIO API are simpler and more
**         portable to various microprocessors.
*/         
/*!
**  @addtogroup ALARM_BTN_module ALARM_BTN module documentation
**  @{
*/         

/* MODULE ALARM_BTN. */

/* MQX Lite include files */
#include "mqxlite.h"
#include "mqxlite_prv.h"
#include "ALARM_BTN.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct {
  LDD_TUserData *UserDataPtr;          /* Pointer to user data */
} ALARM_BTN_TDeviceData;               /* Device data structure type */

typedef ALARM_BTN_TDeviceData *ALARM_BTN_TDeviceDataPtr ; /* Pointer to the device data structure. */

/* {MQXLite RTOS Adapter} Static object used for simulation of dynamic driver memory allocation */
static ALARM_BTN_TDeviceData DeviceDataPrv__DEFAULT_RTOS_ALLOC;
/*
** ===================================================================
**     Method      :  ALARM_BTN_Init (component BitIO_LDD)
*/
/*!
**     @brief
**         Initializes the device. Allocates memory for the device data
**         structure, allocates interrupt vectors and sets interrupt
**         priority, sets pin routing, sets timing, etc. If the "Enable
**         in init. code" is set to "yes" value then the device is also
**         enabled(see the description of the Enable() method). In this
**         case the Enable() method is not necessary and needn't to be
**         generated. 
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer will be
**                           passed as an event or callback parameter.
**     @return
**                         - Pointer to the dynamically allocated private
**                           structure or NULL if there was an error.
*/
/* ===================================================================*/
LDD_TDeviceData* ALARM_BTN_Init(LDD_TUserData *UserDataPtr)
{
  /* Allocate device structure */
  ALARM_BTN_TDeviceDataPtr DeviceDataPrv;

  /* {MQXLite RTOS Adapter} Driver memory allocation: Dynamic allocation is simulated by a pointer to the static object */
  DeviceDataPrv = &DeviceDataPrv__DEFAULT_RTOS_ALLOC;
  DeviceDataPrv->UserDataPtr = UserDataPtr; /* Store the RTOS device structure */
  /* Configure pin as input */
  /* GPIOB_PDDR: PDD&=~0x10000000 */
  GPIOB_PDDR &= (uint32_t)~(uint32_t)(GPIO_PDDR_PDD(0x10000000));
  /* GPIOB_PIDR: PID&=~0x10000000 */
  GPIOB_PIDR &= (uint32_t)~(uint32_t)(GPIO_PIDR_PID(0x10000000));
  /* Registration of the device structure */
  PE_LDD_RegisterDeviceStructure(PE_LDD_COMPONENT_ALARM_BTN_ID,DeviceDataPrv);
  return ((LDD_TDeviceData *)DeviceDataPrv);
}
/*
** ===================================================================
**     Method      :  ALARM_BTN_GetVal (component BitIO_LDD)
*/
/*!
**     @brief
**         Returns the input/output value. If the direction is [input]
**         then the input value of the pin is read and returned. If the
**         direction is [output] then the last written value is read
**         and returned (see <Safe mode> property for limitations).
**         This method cannot be disabled if direction is [input].
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by <Init> method.
**     @return
**                         - Input or output value. Possible values:
**                           <false> - logical "0" (Low level)
**                           <true> - logical "1" (High level)
*/
/* ===================================================================*/
bool ALARM_BTN_GetVal(LDD_TDeviceData *DeviceDataPtr)
{
  uint32_t PortData;                   /* Port data masked according to the bit used */

  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */
  PortData = GPIO_PDD_GetPortDataInput(ALARM_BTN_MODULE_BASE_ADDRESS) & ALARM_BTN_PORT_MASK;
  return (PortData != 0U) ? (bool)TRUE : (bool)FALSE;
}

/* END ALARM_BTN. */

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