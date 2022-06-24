/**
  ******************************************************************************
  * @file    system_stm32f0xx.h
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    13-October-2021
  * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer System Header File.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32f0xx_system
  * @{
  */  
  
/**
  * @brief Define to prevent recursive inclusion
  */
#ifndef __SYSTEM_STM32F0XX_H
#define __SYSTEM_STM32F0XX_H

#ifdef __cplusplus
 extern "C" {
#endif 

/** @addtogroup STM32F0xx_System_Includes
  * @{
  */

/**
  * @}
  */


/** @addtogroup STM32F0xx_System_Exported_types
  * @{
  */

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */

/**
  * @}
  */

/** @addtogroup STM32F0xx_System_Exported_Constants
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F0xx_System_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F0xx_System_Exported_Functions
  * @{
  */
  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_STM32F0XX_H */

/**
  * @}
  */
  
/**
  * @}
  */  
