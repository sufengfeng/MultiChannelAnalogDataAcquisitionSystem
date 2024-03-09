/*
*********************************************************************************************************
*
*	模块名称 : 三轴磁力计HMC5883L驱动模块
*	文件名称 : bsp_HMC5883L.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2012-10-12 armfly  ST固件库版本 V2.1.0
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_HMC5883L_H
#define _BSP_HMC5883L_H

#define HMC5883L_SLAVE_ADDRESS    0x3C		/* I2C从机地址 */

//****************************************
// 定义HMC5883L内部地址
//****************************************
#define DATA_OUT_X		0x03

typedef struct
{
	uint16_t Mag_X;
	uint16_t Mag_Y;
	uint16_t Mag_Z;

	uint8_t	Status;
}HMC5883L_T;

extern HMC5883L_T g_tHMC5883L;

void bsp_InitHMC5883L(void);

void HMC5883L_ReadData(void);
void HMC5883L_WriteByte(uint8_t _ucRegAddr, uint8_t _ucRegData);
uint8_t HMC5883L_ReadByte(uint8_t _ucRegAddr);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
