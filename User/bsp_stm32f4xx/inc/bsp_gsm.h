/*
*********************************************************************************************************
*
*	模块名称 : 华为GPRS模块MG323驱动程序
*	文件名称 : bsp_gsm.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_GSM_H
#define __BSP_GSM_H

/* 供外部调用的函数声明 */
void bsp_InitGSM(void);
void GSM_PowerOn(void);
void GSM_PowerOff(void);
void GSM_SendAT(char *_Cmd);
void GSM_SetAutoReport(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
