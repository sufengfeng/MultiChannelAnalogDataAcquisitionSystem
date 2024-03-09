/*
*********************************************************************************************************
*
*	ģ������ : ����ͷ����BSPģ��(For OV7670)
*	�ļ����� : bsp_camera.h
*	��    �� : V1.0
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __DCMI_H
#define __DCMI_H

#define OV7670_SLAVE_ADDRESS	0x42		/* I2C ���ߵ�ַ */

void bsp_InitCamera(void);
uint16_t OV_ReadID(void);
void CAM_Start(uint32_t _uiDispMemAddr);
void CAM_Stop(void);

#endif

