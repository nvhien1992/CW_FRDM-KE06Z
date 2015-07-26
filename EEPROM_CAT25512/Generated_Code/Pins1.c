/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : Pins1.c
**     Project     : EEPROM_CAT25512
**     Processor   : MKE06Z128VLK4
**     Component   : PinSettings
**     Version     : Component 01.002, Driver 1.1, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-07-10, 17:23, # CodeGen: 16
**     Abstract    :
**
**     Settings    :
**
**     Contents    :
**         Init - void Pins1_Init(void);
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
** @file Pins1.c
** @version 1.1
** @brief
**
*/         
/*!
**  @addtogroup Pins1_module Pins1 module documentation
**  @{
*/         

/* MODULE Pins1. */

#include "Pins1.h"
#include "PE_Types.h"
#include "IO_Map.h"

/*
** ===================================================================
**     Method      :  Pins1_Init (component PinSettings)
*/
/*!
**     @brief
**         This method sets registers according routing settings. Call
**         this method in user code to route desired pins into
**         peripherals. The method is useful for reinitialization HW
**         after some routing changes.
*/
/* ===================================================================*/
void Pins1_Init(void)
{
  /* SIM_SOPT0: SWDE=1,RSTPE=1,NMIE=1 */
  SIM_SOPT0 |= SIM_SOPT0_SWDE_MASK |
               SIM_SOPT0_RSTPE_MASK |
               SIM_SOPT0_NMIE_MASK;
  /* SIM_PINSEL0: SPI0PS=1 */
  SIM_PINSEL0 |= SIM_PINSEL_SPI0PS_MASK;
  /* SIM_PINSEL1: UART1PS=0 */
  SIM_PINSEL1 &= (uint32_t)~(uint32_t)(SIM_PINSEL1_UART1PS_MASK);
  /* PORT_PUE0: PTDPE7=0,PTDPE6=0,PTDPE5=0,PTDPE4=0,PTDPE3=0,PTDPE2=0,PTDPE1=0,PTDPE0=0,PTCPE7=0,PTCPE6=0,PTCPE5=0,PTCPE4=1,PTCPE3=0,PTCPE2=0,PTCPE1=0,PTCPE0=0,PTBPE7=0,PTBPE6=0,PTBPE5=0,PTBPE4=0,PTBPE3=0,PTBPE2=0,PTBPE1=0,PTBPE0=0,PTAPE7=0,PTAPE6=0,PTAPE5=0,PTAPE4=0,PTAPE3=0,PTAPE2=0,PTAPE1=0,PTAPE0=0 */
  PORT_PUE0 = PORT_PUE0_PTCPE4_MASK;
  /* PORT_HDRVE: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,PTH1=0,PTH0=0,PTE1=0,PTE0=0,PTD1=0,PTD0=0,PTB5=0,PTB4=0 */
  PORT_HDRVE = 0x00U;
  /* PORT_PUE1: PTHPE7=0,PTHPE6=0,PTHPE5=0,PTHPE4=0,PTHPE3=0,PTHPE2=0,PTHPE1=0,PTHPE0=0,PTGPE7=0,PTGPE6=0,PTGPE5=0,PTGPE4=0,PTGPE3=0,PTGPE2=0,PTGPE1=0,PTGPE0=0,PTFPE7=0,PTFPE6=0,PTFPE5=0,PTFPE4=0,PTFPE3=0,PTFPE2=0,PTFPE1=0,PTFPE0=0,PTEPE7=0,PTEPE6=0,PTEPE5=0,PTEPE3=0,PTEPE2=0,PTEPE1=0,PTEPE0=0 */
  PORT_PUE1 &= (uint32_t)~(uint32_t)(
                PORT_PUE1_PTHPE7_MASK |
                PORT_PUE1_PTHPE6_MASK |
                PORT_PUE1_PTHPE5_MASK |
                PORT_PUE1_PTHPE4_MASK |
                PORT_PUE1_PTHPE3_MASK |
                PORT_PUE1_PTHPE2_MASK |
                PORT_PUE1_PTHPE1_MASK |
                PORT_PUE1_PTHPE0_MASK |
                PORT_PUE1_PTGPE7_MASK |
                PORT_PUE1_PTGPE6_MASK |
                PORT_PUE1_PTGPE5_MASK |
                PORT_PUE1_PTGPE4_MASK |
                PORT_PUE1_PTGPE3_MASK |
                PORT_PUE1_PTGPE2_MASK |
                PORT_PUE1_PTGPE1_MASK |
                PORT_PUE1_PTGPE0_MASK |
                PORT_PUE1_PTFPE7_MASK |
                PORT_PUE1_PTFPE6_MASK |
                PORT_PUE1_PTFPE5_MASK |
                PORT_PUE1_PTFPE4_MASK |
                PORT_PUE1_PTFPE3_MASK |
                PORT_PUE1_PTFPE2_MASK |
                PORT_PUE1_PTFPE1_MASK |
                PORT_PUE1_PTFPE0_MASK |
                PORT_PUE1_PTEPE7_MASK |
                PORT_PUE1_PTEPE6_MASK |
                PORT_PUE1_PTEPE5_MASK |
                PORT_PUE1_PTEPE3_MASK |
                PORT_PUE1_PTEPE2_MASK |
                PORT_PUE1_PTEPE1_MASK |
                PORT_PUE1_PTEPE0_MASK
               );
  /* PORT_PUE2: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,PTIPE6=0,PTIPE5=0,PTIPE4=0,PTIPE3=0,PTIPE2=0,PTIPE1=0,PTIPE0=0 */
  PORT_PUE2 = 0x00U;
}


/* END Pins1. */
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