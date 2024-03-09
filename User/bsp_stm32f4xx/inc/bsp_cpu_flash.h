/*
*********************************************************************************************************
*
*	ģ������ : CPU�ڲ�Flash����ģ��
*	�ļ����� : bsp_cpu_flash.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/


#ifndef __BSP_CPU_FLASH_H
#define __BSP_CPU_FLASH_H


/* ���� STM32F103ZE �ڲ�Flash ������������С */
#define FLASH_SIZE			(512 * 1024)	/* Flash ���� */
#define FLASH_BASE_ADDR		(0x08000000)	/* Flash �����ַ(����ַ) */

#define SECTOR_SIZE			0x400			/* E ϵ����2K�ֽ�,������1K */
#define SECTOR_MASK			0xFFFFFC00

/* ע�⣺ ���溯���е�Flash��ַ��ƫ�Ƶ�ַ, ���������ַ */
uint8_t bsp_ReadCpuFlash(uint8_t *_ucpDst, uint32_t _ulSrcAddr, uint32_t _ulSize);
uint8_t bsp_WriteCpuFlash(uint32_t _ulWrAddr, uint8_t *_ucpSrc, uint32_t _uiWrLen);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

