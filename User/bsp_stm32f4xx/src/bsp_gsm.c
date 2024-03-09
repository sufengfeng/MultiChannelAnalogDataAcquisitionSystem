/*
*********************************************************************************************************
*
*	模块名称 : 华为GPRS模块MG323驱动程序
*	文件名称 : bsp_gsm.c
*	版    本 : V1.0
*	说    明 : 封装MG323模块相关的AT命令
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-02-01 armfly  正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
	安富莱STM32-F4 开发板口线分配：
	GPRS_TERM_ON   ： PB15
*/

/*
	AT+CIND=<mode>[,<mode>[,<mode>...]] 设置指示事件是否上报
		+CIND: 5,99,1,0,1,0,0,0,4

	AT+CREG?  查询当前网络状态

	AT+CSQ 查询信号质量命令

	AT+CIMI 查询SIM 卡的IMSI 号。

	AT+CIND? 读取当前的指示状态

	ATA 接听命令
	ATH 挂断连接命令
	
	AT^SWSPATH=<n>  切换音频通道
*/

/* 按键口对应的RCC时钟 */
#define RCC_TERM_ON 	RCC_AHB1Periph_GPIOB

#define PORT_TERM_ON	GPIOB
#define PIN_TERM_ON		GPIO_Pin_15

/* STM32和MG323的TERM_ON引脚间有1个NPN三极管，因此需要反相 */
#define TERM_ON_1()		GPIO_ResetBits(PORT_TERM_ON, PIN_TERM_ON);
#define TERM_ON_0()		GPIO_SetBits(PORT_TERM_ON, PIN_TERM_ON);

/*
*********************************************************************************************************
*	函 数 名: bsp_InitGSM
*	功能说明: 配置无线模块相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitGSM(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_AHB1PeriphClockCmd(RCC_TERM_ON, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */

	GPIO_InitStructure.GPIO_Pin = PIN_TERM_ON;
	GPIO_Init(PORT_TERM_ON, &GPIO_InitStructure);

	/* CPU的串口配置已经由 bsp_uart_fifo.c 中的 bsp_InitUart() 做了 */
}

/*
*********************************************************************************************************
*	函 数 名: GSM_PowerOn
*	功能说明: 给MG323模块上电
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void GSM_PowerOn(void)
{
	/*
		根据MG323手册，模块上电后延迟250ms，然后驱动 TERM_ON口线为低电平 750ms 之后驱动为高，完成开机时序
	*/
	TERM_ON_1();
	bsp_DelayMS(250);
	TERM_ON_0();
	bsp_DelayMS(750);
	TERM_ON_1();
}

/*
*********************************************************************************************************
*	函 数 名: GSM_PowerOff
*	功能说明: 控制MG323模块关机
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void GSM_PowerOff(void)
{
	/* 硬件关机 */
	TERM_ON_0();

	/* 也可以软件关机 */
	//GSM_SendAT("AT^SMSO");
}


/*
*********************************************************************************************************
*	函 数 名: GSM_SendAT
*	功能说明: 向GSM模块发送AT命令。 本函数自动在AT字符串口增加<CR>字符
*	形    参: _Str : AT命令字符串，不包括末尾的回车<CR>. 以字符0结束
*	返 回 值: 无
*********************************************************************************************************
*/
void GSM_SendAT(char *_Cmd)
{
	comSendBuf(COM6, (uint8_t *)_Cmd, strlen(_Cmd));
	comSendBuf(COM6, "\r", 1);
}

/*
*********************************************************************************************************
*	函 数 名: GSM_SetAutoReport
*	功能说明: 设置事件自动上报
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void GSM_SetAutoReport(void)
{
	GSM_SendAT("AT+CIND=1,1,1,1,1,1,1,1,1");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
