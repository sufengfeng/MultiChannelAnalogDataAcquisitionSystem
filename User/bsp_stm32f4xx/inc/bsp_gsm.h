/*
*********************************************************************************************************
*
*	ģ������ : ��ΪGPRSģ��MG323��������
*	�ļ����� : bsp_gsm.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_GSM_H
#define __BSP_GSM_H

/* ���ⲿ���õĺ������� */
void bsp_InitGSM(void);
void GSM_PowerOn(void);
void GSM_PowerOff(void);
void GSM_SendAT(char *_Cmd);
void GSM_SetAutoReport(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
