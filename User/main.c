/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : ������
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2012-08-08 armfly  ST�̼���V3.5.0�汾��
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"		/* ���Ҫ��ST�Ĺ̼��⣬�����������ļ� */

/* ���������������̷������� */
#define EXAMPLE_NAME	"F4-029_AD7606ģ������"
#define EXAMPLE_DATE	"2013-02-01"
#define DEMO_VER		"1.0"

/* �������ļ��ڵ��õĺ������� */
static void PrintfLogo(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;
	uint8_t ucRefresh = 0;
	uint8_t ucFifoMode;
	int8_t l_sTmpSendBuf[18];
	
	/*
		����ST�̼���������ļ��Ѿ�ִ����CPUϵͳʱ�ӵĳ�ʼ�������Բ����ٴ��ظ�����ϵͳʱ�ӡ�
		�����ļ�������CPU��ʱ��Ƶ�ʡ��ڲ�Flash�����ٶȺͿ�ѡ���ⲿSRAM FSMC��ʼ����

		ϵͳʱ��ȱʡ����Ϊ72MHz�������Ҫ���ģ������޸ģ�
		\Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x\system_stm32f10x.c
		������ϵͳʱ�ӵĺꡣ
	*/

	bsp_Init();
	
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		/* ʹ�� GPIOʱ�� */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	
	PrintfLogo();		/* ��ӡ����Logo������1 */

	ucFifoMode = 0;	 	/* AD7606������ͨ����ģʽ */
	
	bsp_InitAD7606();	/* ����AD7606���õ�GPIO */

	AD7606_SetOS(AD_OS_NO);		/* �޹����� */
	AD7606_SetInputRange(0);	/* 0��ʾ��������Ϊ����5V, 1��ʾ����10V */

	bsp_StartAutoTimer(0, 500);	/* ����1��200ms���Զ���װ�Ķ�ʱ�� */
	// bsp_StartAutoTimer(1, 500);	/* ����1��200ms���Զ���װ�Ķ�ʱ�� */
	AD7606_StartConvst();		/* ����1��ת�� */
	ucRefresh = 0;

	ucFifoMode = 1;	 				/* AD7606����FIFO����ģʽ */
	printf("\r\nAD7606����FIFO����ģʽ (200KHz 8ͨ��ͬ���ɼ�)...\r\n");
	AD7606_StartRecord(200000);		/* ����200kHz�������� */
	
	l_sTmpSendBuf[0]=0xA5;l_sTmpSendBuf[17]=0x5A;
	while (1)
	{
		int i;
		int16_t p_tmpAdcValue[8];		/* ��ǰADCֵ, �з����� */
		CPU_IDLE();		/* �������bsp_timer.h �ж��壬Ŀǰ����Ϊ�ա��û������޸������ʵ��CPU���ߺ�ι�� */
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
			/* ��ӡADC������� */
			printf("Range = %d, OS = %d, ", g_tAD7606.ucRange, g_tAD7606.ucOS);
			printf("CH1 = %6d, CH2 = %6d, CH3 = %6d, CH4 = %6d, ",
				g_tAD7606.sNowAdc[0], g_tAD7606.sNowAdc[1], g_tAD7606.sNowAdc[2], g_tAD7606.sNowAdc[3]);
			printf("CH5 = %6d, CH6 = %6d, CH7 = %6d, CH8 = %6d\r\n",
				g_tAD7606.sNowAdc[4], g_tAD7606.sNowAdc[5], g_tAD7606.sNowAdc[6], g_tAD7606.sNowAdc[7]);
		}		
		
		if (ucFifoMode == 0)	/* AD7606 ��ͨ����ģʽ */
		{
			if (bsp_CheckTimer(0))	
			{		
				/* ÿ��500ms ����һ��. ���������ת�� */
				AD7606_ReadNowAdc();		/* ��ȡ������� */
				AD7606_StartConvst();		/* �����´�ת�� */
				
				ucRefresh = 1;	/* ˢ����ʾ */
			}
		}
		else
		{
			/* 
				��FIFO����ģʽ��bsp_AD7606�Զ����вɼ������ݴ洢��FIFO��������
				�������ͨ������ĺ�����ȡ:			
				uint8_t AD7606_ReadFifo(uint16_t *_usReadAdc)
			
				����Խ����ݱ��浽SD�������߱��浽�ⲿSRAM��

				����δ��FIFO�е����ݽ��д������д�ӡ��ǰ���µ�����ֵ��
			
				����������ܼ�ʱ��ȡFIFO���ݣ���ô AD7606_FifoFull() �������档
			
				8ͨ��200K����ʱ�����ݴ����� = 200 000 * 2 * 8 = 3.2MB/S			
			*/
			
			if (bsp_CheckTimer(0))	
			{
				;// ucRefresh = 1;	/* ˢ����ʾ */
			}
		}		

		/* ��������ɺ�̨systick�жϷ������ʵ�֣�����ֻ��Ҫ����bsp_GetKey��ȡ��ֵ���ɡ������������
		�ȴ��������£��������ǿ�����whileѭ���������������� */
		ucKeyCode = bsp_GetKey();	/* ��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		if (0)
		//if (ucKeyCode != KEY_NONE)
		{
			/*
				���ڰ��������¼���ȱʡ��bsp_button.c �������TAMPER��WAKEUP��USER����ҡ��OK���ĵ����¼�
				�������Ӧ�ó�����Ҫ�����������緽������ĵ����¼�������Ҫ���޸�һ��bsp_button.c�ļ�
			*/
			switch (ucKeyCode)
			{
				case KEY_DOWN_K1:			/* K1������ �л����� */
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

				case KEY_DOWN_K2:			/* K2������ */
					ucFifoMode = 1;	 				/* AD7606����FIFO����ģʽ */
					printf("\r\nAD7606����FIFO����ģʽ (200KHz 8ͨ��ͬ���ɼ�)...\r\n");
					AD7606_StartRecord(200000);		/* ����200kHz�������� */
					break;

				case KEY_DOWN_K3:			/* K3������ */
					AD7606_StopRecord();	/* ֹͣ��¼ */				
					ucFifoMode = 0;	 		/* AD7606������ͨ����ģʽ */				
					printf("\r\nAD7606������ͨ����ģʽ(0.5�붨ʱ�ɼ�)...\r\n");
					break;

				case JOY_DOWN_U:			/* ҡ��UP������ */
					if (g_tAD7606.ucOS < 6)
					{
						g_tAD7606.ucOS++;
					}
					ucRefresh = 1;
					break;

				case JOY_DOWN_D:			/* ҡ��DOWN������ */
					if (g_tAD7606.ucOS > 0)
					{
						g_tAD7606.ucOS--;
					}
					ucRefresh = 1;
					break;

				case JOY_DOWN_L:			/* ҡ��LEFT������ */
					break;

				case JOY_DOWN_R:			/* ҡ��RIGHT������ */
					break;

				case JOY_DOWN_OK:			/* ҡ��OK������ */
					break;

				default:
					/* �����ļ�ֵ������ */
					break;
			}
		}
	}
}


/*
*********************************************************************************************************
*	�� �� ��: PrintfLogo
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	printf("* ��������   : %s\r\n", EXAMPLE_NAME);	/* ��ӡ�������� */
	printf("* ���̰汾   : %s\r\n", DEMO_VER);		/* ��ӡ���̰汾 */
	printf("* ��������   : %s\r\n", EXAMPLE_DATE);	/* ��ӡ�������� */

	/* ��ӡST�̼���汾����3���������stm32f10x.h�ļ��� */
	printf("* �̼���汾 : %d.%d.%d\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \n\r");	/* ��ӡһ�пո� */
	printf("* QQ    : 1295744630 \r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* Copyright www.armfly.com ����������\r\n");
	printf("*************************************************************\n\r");
}
