/*
*********************************************************************************************************
*
*	模块名称 : STM32固件库配置文件。
*	文件名称 : stm32f4xx_conf.h
*	版    本 : V1.0.2
*	说    明 :	这是ST固件库提供的文件。用户可以根据需要包含ST固件库的外设模块。为了方便我们包含了所有固件
*				库模块。
*
*			   这个文件被 stm32f4xx.h 包含，因此在.c文件中只需要 include "stm32f10x.h"即可，不必单独
* 			   #include stm32f10x_conf.h文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __STM32F4xx_CONF_H
#define __STM32F4xx_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment the line below to enable peripheral header file inclusion */
#include "stm32f4xx_adc.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dbgmcu.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_fsmc.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_wwdg.h"
#include "misc.h" /* 用于NVIC和SysTick的高级函数(与CMSIS相关) */

/*
	用户可以选择是否使能ST固件库的断言供能。使能断言的方法有两种：
	(1) 在C编译器的预定义宏选项中定义USE_FULL_ASSERT。
	(2) 在本文件取消"#define USE_FULL_ASSERT    1"行的注释。
*/
/* 取消下面代码行的注释则固件库代码会展开assert_param宏进行断言 */
/* #define USE_FULL_ASSERT    1 */

#ifdef  USE_FULL_ASSERT
	/*
		assert_param宏用于函数形参检查。如果expr是false，它将调用assert_failed()函数报告发生错误的源文件
		和行号。如果expr是true，将不执行任何操作。

		assert_failed() 函数在stm32f10x_assert.c文件(这是安富莱建立的文件)
	*/
	#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))

	void assert_failed(uint8_t* file, uint32_t line);
#else
	#define assert_param(expr) ((void)0)
#endif

#endif /* __STM32F4xx_CONF_H */

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
