/*
*********************************************************************************************************
*
*	模块名称 : CPU内部Flash驱动模块
*	文件名称 : bsp_cpu_flash.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/


#ifndef __BSP_CPU_FLASH_H
#define __BSP_CPU_FLASH_H


/* 定义 STM32F103ZE 内部Flash 容量和扇区大小 */
#define FLASH_SIZE			(512 * 1024)	/* Flash 容量 */
#define FLASH_BASE_ADDR		(0x08000000)	/* Flash 物理地址(基地址) */

#define SECTOR_SIZE			0x400			/* E 系列是2K字节,其它是1K */
#define SECTOR_MASK			0xFFFFFC00

/* 注意： 下面函数中的Flash地址是偏移地址, 不是物理地址 */
uint8_t bsp_ReadCpuFlash(uint8_t *_ucpDst, uint32_t _ulSrcAddr, uint32_t _ulSize);
uint8_t bsp_WriteCpuFlash(uint32_t _ulWrAddr, uint8_t *_ucpSrc, uint32_t _uiWrLen);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

