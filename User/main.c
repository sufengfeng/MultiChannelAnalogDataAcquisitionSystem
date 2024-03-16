/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : 主程序
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2012-08-08 armfly  ST固件库V3.5.0版本。
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"		/* 如果要用ST的固件库，必须包含这个文件 */

/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"F4-029_AD7606模块例程"
#define EXAMPLE_DATE	"2013-02-01"
#define DEMO_VER		"1.0"

/* 仅允许本文件内调用的函数声明 */
static void PrintfLogo(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;
	uint8_t ucRefresh = 0;
	uint8_t ucFifoMode;
	int8_t l_sTmpSendBuf[18];
	
	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		系统时钟缺省配置为72MHz，如果需要更改，可以修改：
		\Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x\system_stm32f10x.c
		中配置系统时钟的宏。
	*/

	bsp_Init();
	
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		/* 使能 GPIO时钟 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	
	PrintfLogo();		/* 打印例程Logo到串口1 */

	ucFifoMode = 0;	 	/* AD7606进入普通工作模式 */
	
	bsp_InitAD7606();	/* 配置AD7606所用的GPIO */

	AD7606_SetOS(AD_OS_NO);		/* 无过采样 */
	AD7606_SetInputRange(0);	/* 0表示输入量程为正负5V, 1表示正负10V */

	bsp_StartAutoTimer(0, 500);	/* 启动1个200ms的自动重装的定时器 */
	// bsp_StartAutoTimer(1, 500);	/* 启动1个200ms的自动重装的定时器 */
	AD7606_StartConvst();		/* 启动1次转换 */
	ucRefresh = 0;

	ucFifoMode = 1;	 				/* AD7606进入FIFO工作模式 */
	printf("\r\nAD7606进入FIFO工作模式 (200KHz 8通道同步采集)...\r\n");
	AD7606_StartRecord(200000);		/* 启动200kHz采样速率 */
	
	l_sTmpSendBuf[0]=0xA5;l_sTmpSendBuf[17]=0x5A;
	while (1)
	{
		int i;
		int16_t p_tmpAdcValue[8];		/* 当前ADC值, 有符号数 */
		CPU_IDLE();		/* 这个宏在bsp_timer.h 中定义，目前定义为空。用户可以修改这个宏实现CPU休眠和喂狗 */
		// uint8_t AD7606_ReadFifo(uint16_t *_usReadAdc);
		if(AD7606_ReadFifo((uint16_t *)p_tmpAdcValue)){
			GPIO_ToggleBits(GPIOC,GPIO_Pin_13);
			memcpy(l_sTmpSendBuf+1,p_tmpAdcValue,8*2);
			for(i=0;i<18;i++){
				Usart_SendByte(USART1,l_sTmpSendBuf[i]);
			}
			// comSendBuf(COM1, (uint8_t *)l_sTmpSendBuf, 18);
		}
		if (ucRefresh == 1) 
		{
			ucRefresh = 0;
			GPIO_ToggleBits(GPIOC,GPIO_Pin_13);
			/* 打印ADC采样结果 */
			printf("Range = %d, OS = %d, ", g_tAD7606.ucRange, g_tAD7606.ucOS);
			printf("CH1 = %6d, CH2 = %6d, CH3 = %6d, CH4 = %6d, ",
				g_tAD7606.sNowAdc[0], g_tAD7606.sNowAdc[1], g_tAD7606.sNowAdc[2], g_tAD7606.sNowAdc[3]);
			printf("CH5 = %6d, CH6 = %6d, CH7 = %6d, CH8 = %6d\r\n",
				g_tAD7606.sNowAdc[4], g_tAD7606.sNowAdc[5], g_tAD7606.sNowAdc[6], g_tAD7606.sNowAdc[7]);
		}		
		
		if (ucFifoMode == 0)	/* AD7606 普通工作模式 */
		{
			if (bsp_CheckTimer(0))	
			{		
				/* 每隔500ms 进来一次. 由软件启动转换 */
				AD7606_ReadNowAdc();		/* 读取采样结果 */
				AD7606_StartConvst();		/* 启动下次转换 */
				
				ucRefresh = 1;	/* 刷新显示 */
			}
		}
		else
		{
			/* 
				在FIFO工作模式，bsp_AD7606自动进行采集，数据存储在FIFO缓冲区。
				结果可以通过下面的函数读取:			
				uint8_t AD7606_ReadFifo(uint16_t *_usReadAdc)
			
				你可以将数据保存到SD卡，或者保存到外部SRAM。

				本例未对FIFO中的数据进行处理，进行打印当前最新的样本值。
			
				如果主程序不能及时读取FIFO数据，那么 AD7606_FifoFull() 将返回真。
			
				8通道200K采样时，数据传输率 = 200 000 * 2 * 8 = 3.2MB/S			
			*/
			
			if (bsp_CheckTimer(0))	
			{
				;// ucRefresh = 1;	/* 刷新显示 */
			}
		}		

		/* 按键检测由后台systick中断服务程序实现，我们只需要调用bsp_GetKey读取键值即可。这个函数不会
		等待按键按下，这样我们可以在while循环内做其他的事情 */
		ucKeyCode = bsp_GetKey();	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
		if (0)
		//if (ucKeyCode != KEY_NONE)
		{
			/*
				对于按键弹起事件，缺省的bsp_button.c 仅检测了TAMPER、WAKEUP、USER键、摇杆OK键的弹起事件
				如果您的应用程序需要其它键（比如方向键）的弹起事件，您需要简单修改一下bsp_button.c文件
			*/
			switch (ucKeyCode)
			{
				case KEY_DOWN_K1:			/* K1键按下 切换量程 */
					if (g_tAD7606.ucRange == 0)
					{
						AD7606_SetInputRange(1);
					}
					else
					{
						AD7606_SetInputRange(0);
					}
					ucRefresh = 1;
					break;

				case KEY_DOWN_K2:			/* K2键按下 */
					ucFifoMode = 1;	 				/* AD7606进入FIFO工作模式 */
					printf("\r\nAD7606进入FIFO工作模式 (200KHz 8通道同步采集)...\r\n");
					AD7606_StartRecord(200000);		/* 启动200kHz采样速率 */
					break;

				case KEY_DOWN_K3:			/* K3键按下 */
					AD7606_StopRecord();	/* 停止记录 */				
					ucFifoMode = 0;	 		/* AD7606进入普通工作模式 */				
					printf("\r\nAD7606进入普通工作模式(0.5秒定时采集)...\r\n");
					break;

				case JOY_DOWN_U:			/* 摇杆UP键按下 */
					if (g_tAD7606.ucOS < 6)
					{
						g_tAD7606.ucOS++;
					}
					ucRefresh = 1;
					break;

				case JOY_DOWN_D:			/* 摇杆DOWN键按下 */
					if (g_tAD7606.ucOS > 0)
					{
						g_tAD7606.ucOS--;
					}
					ucRefresh = 1;
					break;

				case JOY_DOWN_L:			/* 摇杆LEFT键按下 */
					break;

				case JOY_DOWN_R:			/* 摇杆RIGHT键按下 */
					break;

				case JOY_DOWN_OK:			/* 摇杆OK键按下 */
					break;

				default:
					/* 其他的键值不处理 */
					break;
			}
		}
	}
}


/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST固件库版本，这3个定义宏在stm32f10x.h文件中 */
	printf("* 固件库版本 : %d.%d.%d\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \n\r");	/* 打印一行空格 */
	printf("* QQ    : 1295744630 \r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
}
