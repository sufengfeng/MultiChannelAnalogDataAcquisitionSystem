/*
*********************************************************************************************************
*
*	ģ������ : STM32�̼��������ļ���
*	�ļ����� : stm32f4xx_conf.h
*	��    �� : V1.0.2
*	˵    �� :	����ST�̼����ṩ���ļ����û����Ը�����Ҫ����ST�̼��������ģ�顣Ϊ�˷������ǰ��������й̼�
*				��ģ�顣
*
*			   ����ļ��� stm32f4xx.h �����������.c�ļ���ֻ��Ҫ include "stm32f10x.h"���ɣ����ص���
* 			   #include stm32f10x_conf.h�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
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
#include "misc.h" /* ����NVIC��SysTick�ĸ߼�����(��CMSIS���) */

/*
	�û�����ѡ���Ƿ�ʹ��ST�̼���Ķ��Թ��ܡ�ʹ�ܶ��Եķ��������֣�
	(1) ��C��������Ԥ�����ѡ���ж���USE_FULL_ASSERT��
	(2) �ڱ��ļ�ȡ��"#define USE_FULL_ASSERT    1"�е�ע�͡�
*/
/* ȡ����������е�ע����̼�������չ��assert_param����ж��� */
/* #define USE_FULL_ASSERT    1 */

#ifdef  USE_FULL_ASSERT
	/*
		assert_param�����ں����βμ�顣���expr��false����������assert_failed()�������淢�������Դ�ļ�
		���кš����expr��true������ִ���κβ�����

		assert_failed() ������stm32f10x_assert.c�ļ�(���ǰ������������ļ�)
	*/
	#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))

	void assert_failed(uint8_t* file, uint32_t line);
#else
	#define assert_param(expr) ((void)0)
#endif

#endif /* __STM32F4xx_CONF_H */

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
