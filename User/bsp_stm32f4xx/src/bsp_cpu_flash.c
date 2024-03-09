/*
*********************************************************************************************************
*
*	模块名称 : CPU内部Flash驱动模块
*	文件名称 : bsp_cpu_flash.c
*	版    本 : V1.0
*	说    明 : 实现读写CPU内部Flash， 实现参数访问接口
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-01-01 armfly  正式发布
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*********************************************************************************************************
*/

#include "bsp.h"

static uint8_t WriteSector(uint32_t _ulWrAddr, uint8_t *_ucpSrc, uint16_t _uiWrLen);
static void _ProgSector(uint32_t _ulWrAddr,	uint8_t *_ucpSrc, uint16_t _uiWrLen);
static int _WriteFlash(uint32_t _ulWrAddr, uint8_t *_ucpSrc, int _uiWrLen);

/*
*********************************************************************************************************
*	函 数 名: bsp_ReadCpuFlash
*	功能说明: 从CPU内部Flash指定地址 _ulSrcAddr 开始读取_ulSize个数据到 _ucpDst.
*	形    参：
*	uint8_t *_ucpDst,		: 目标缓冲区地址
*	uint32_t _ulSrcAddr,	: 起始地址,偏移地址
*	uint32_t _ulSize		: 长度,单位:字节
*	返 回 值: 0-成功，1-数据长度或地址溢出
*********************************************************************************************************
*/
uint8_t bsp_ReadCpuFlash(uint8_t *_ucpDst, uint32_t _ulSrcAddr, uint32_t _ulSize)
{
	uint32_t i;
	uint16_t uiTemp;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if (_ulSrcAddr >= FLASH_SIZE)
	{
		return 0;
	}

	/* 长度为0时不继续操作,否则起始地址为奇地址会出错 */
	if (_ulSize == 0)
	{
		return 0;
	}

	_ulSrcAddr = _ulSrcAddr + FLASH_BASE_ADDR;

	/* Flash是16bit数据线,每次可读两字节 */
	/* 当传入的要读取地址为奇数时的处理 */
	if (_ulSrcAddr & 0x01)
	{
		_ulSrcAddr--;
		((uint8_t *) _ucpDst)[0] = (((uint16_t *) _ulSrcAddr)[0]) >> 8;
		_ucpDst += 1;
		_ulSrcAddr += 2;
		_ulSize--;
	}

	/* (uint16_t *)只能用偶数地址 */
	for (i = 0; i < _ulSize / 2; i++)
	{
		uiTemp = ((uint16_t *) _ulSrcAddr)[i];
		_ucpDst[2 * i] = (uint8_t)uiTemp;
		_ucpDst[2 * i + 1] = uiTemp >> 8;
	}

	if (_ulSize % 2)
	{
		_ucpDst[_ulSize - 1] = ((uint8_t *) _ulSrcAddr)[_ulSize - 1] ;
	}

	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_WriteCpuFlash
*	功能说明: 写Flash,写入的数据长度最大为FLASH_SIZE，支持连续跨扇区操作。
*	形    参：
*			_ulWrAddr : 目标地址,偏移地址
*			_ucpSrc   : 数据源指针
*			_uiWrLen  : 数据长度,单位:字节
*	返 回 值: 0-成功，1-数据长度或地址溢出，2-写Flash出错(估计Flash寿命到)
*********************************************************************************************************
*/
uint8_t bsp_WriteCpuFlash(uint32_t _ulWrAddr, uint8_t *_ucpSrc, uint32_t _uiWrLen)
{
	int i;
	int iRet = 0;

	if (_ulWrAddr + _uiWrLen > FLASH_SIZE)
	{
		return 1;
	}

	for (i = 0; i < _uiWrLen / SECTOR_SIZE; i++)
	{
		iRet = _WriteFlash(_ulWrAddr, _ucpSrc, SECTOR_SIZE);
		if (iRet != 0)
		{
			return iRet;
		}
		_ucpSrc += SECTOR_SIZE;
		_ulWrAddr += SECTOR_SIZE;
		_uiWrLen -= SECTOR_SIZE;
	}

	if (_uiWrLen > 0)
	{
		iRet = _WriteFlash(_ulWrAddr, _ucpSrc, _uiWrLen);
	}
	return iRet;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_CmpCpuFlash
*	功能说明: 比较Flash指定地址的数据
*	形    参：
*			_ulSrcAddr : 起始地址,偏移地址
*			*_ucpTar   : 数据缓冲区
*			_ulSize    : 长度,单位:字节
*	返 回 值: 0 = 相等, 1 = 不等
*********************************************************************************************************
*/
static uint8_t bsp_CmpCpuFlash(uint32_t _ulSrcAddr, uint8_t *_ucpTar, uint32_t _ulSize)
{
	uint32_t i;
	uint16_t uiTemp;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if (_ulSrcAddr >= FLASH_SIZE)
	{
		return 1;
	}

	/* 长度为0时不继续操作,否则起始地址为奇地址会出错 */
	if (_ulSize == 0)
	{
		return 0;
	}

	_ulSrcAddr = _ulSrcAddr + FLASH_BASE_ADDR;


	/* Flash是16bit数据线,每次可读两字节 */
	/* 当传入的要读取地址为奇数时的处理 */
	if (_ulSrcAddr & 0x01)
	{
		_ulSrcAddr--;
		if (_ucpTar[0] != ((((uint16_t *)_ulSrcAddr)[0]) >> 8))
		{
			return 1;
		}
		_ucpTar += 1;
		_ulSrcAddr += 2;
		_ulSize--;
	}

	/* (uint16_t *)只能用偶数地址 */
	for (i = 0; i < _ulSize / 2; i++)
	{
		uiTemp = ((uint16_t *) _ulSrcAddr)[i];

		if (_ucpTar[2 * i] != (uint8_t)uiTemp)
		{
			return 1;
		}

		if (_ucpTar[2 * i + 1] != (uiTemp >> 8))
		{
			return 1;
		}
	}

	if (_ulSize % 2)
	{
		if (_ucpTar[_ulSize - 1] != ((uint8_t *) _ulSrcAddr)[_ulSize - 1])
		{
			return 1;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: _ProgSector
*	功能说明: 编程一个扇区
*	形    参：
*			_ulSrcAddr : 起始地址,偏移地址
*			*_ucpTar   : 数据缓冲区
*			_ulSize    : 长度,单位:字节
*	返 回 值: 0 = 相等, 1 = 不等
*********************************************************************************************************
*/
static void _ProgSector(uint32_t _ulWrAddr,	uint8_t *_ucpSrc, uint16_t _uiWrLen)
{
	uint16_t *pusTemp;
	FLASH_Status FLASHStatus;
	uint32_t Address, EndAddr;

	if ((_uiWrLen == 0) ||  (_uiWrLen % 2) != 0)
	{
		return;
	}

	Address = _ulWrAddr += FLASH_BASE_ADDR;
	EndAddr = Address + _uiWrLen;
	FLASHStatus = FLASH_COMPLETE;
	pusTemp = (uint16_t *)_ucpSrc;
	while((Address < EndAddr) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramHalfWord(Address, *pusTemp++);
		Address = Address + 2;
	}
}

/*
*********************************************************************************************************
*	函 数 名: WriteSector
*	功能说明: 写1个扇区并校验,如果不正确则再重写两次。本函数自动完成擦除操作。
*	形    参：
*			_ulWrAddr : 目标地址,偏移地址
*			_ucpSrc   : 数据源指针
*			_uiWrLen  : 长度, 不能大于 SECTOR_SIZE (4K)
*	返 回 值: 0 = 相等, 1 = 不等
*********************************************************************************************************
*/
static uint8_t WriteSector(uint32_t _ulWrAddr, uint8_t *_ucpSrc, uint16_t _uiWrLen)
{
	uint16_t i;
	uint16_t j;				/* 用于延时 */
	uint32_t ulFirstAddr;		/* 扇区首址 */
	uint8_t ucaFlashBuf[SECTOR_SIZE];
	uint8_t cRet;

	/* 如果偏移地址超过芯片容量，则退出 */
	if (_ulWrAddr >= FLASH_SIZE)
	{
		return 0;
	}

	/* 如果数据长度大于扇区容量，则退出 */
	if (_uiWrLen > SECTOR_SIZE)
	{
		return 0;
	}

	/* 长度为0时不继续操作,直接认为成功,否则起始地址为奇地址会出错 */
	if (_uiWrLen == 0)
	{
		return 1;
	}

	/* 如果FLASH中的数据没有变化,则不写FLASH */
	bsp_ReadCpuFlash(ucaFlashBuf, _ulWrAddr, _uiWrLen);
	if (memcmp(ucaFlashBuf, _ucpSrc, _uiWrLen) == 0)
	{
		return 1;
	}

	ulFirstAddr = _ulWrAddr & SECTOR_MASK;

	if (_uiWrLen == SECTOR_SIZE)		/* 整个扇区都改写 */
	{
		for	(i = 0; i < SECTOR_SIZE; i++)
		{
			ucaFlashBuf[i] = _ucpSrc[i];
		}
	}
	else						/* 改写部分数据 */
	{
		/* 先将整个扇区的数据读出 */
		/*
		为了函数便于移植和提执行效率，以下语句使用 ReadFlash() 而不是
		MyMemCpy(); ReadFlash 是按16bit访问，效率要高些。
		*/
		bsp_ReadCpuFlash(ucaFlashBuf, ulFirstAddr, SECTOR_SIZE);

		/* 再用新数据覆盖 */
		i = _ulWrAddr & (SECTOR_SIZE - 1);
		memcpy(&ucaFlashBuf[i], _ucpSrc, _uiWrLen);
	}

	/* 写完之后进行校验，如果不正确则重写，最多3次 */
	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_Unlock();
	cRet = 1;
	for (i = 0; i < 3; i++)
	{
		FLASH_ErasePage(ulFirstAddr + FLASH_BASE_ADDR);			/* 擦除1个扇区 */
		_ProgSector(ulFirstAddr, ucaFlashBuf, SECTOR_SIZE);		/* 编程一个扇区 */

		/* 注意:如下语句不能移植到串行 Flash,需要将 ulFirstAddr 开始的数据读到RAM */
		if (bsp_CmpCpuFlash(_ulWrAddr, _ucpSrc, _uiWrLen) == 0)
		{
			cRet = 1;
			break;
		}
		else
		{
			if (bsp_CmpCpuFlash(_ulWrAddr, _ucpSrc, _uiWrLen) == 0)
			{
				cRet = 1;
				break;
			}

			/* 不能调用 OSTimeDly(50), 因为中断和操作系统调度可能关闭.
			直接硬延时可靠些 */
			for (j = 0; j < 10000; j++);
		}
	}

	FLASH_Lock();
	return cRet;
}

/*
*********************************************************************************************************
*	函 数 名: _WriteFlash
*	功能说明: 写Flash,写入的数据长度最大为SECTOR_SIZE，支持跨扇区操作。
*	形    参：
*			_ulWrAddr : 目标地址,偏移地址
*			_ucpSrc   : 数据源指针
*			_uiWrLen  : 数据长度,单位:字节
*	返 回 值: 0-成功，1-数据长度或地址溢出，2-写Flash出错(估计Flash寿命到)
*********************************************************************************************************
*/
static int _WriteFlash(uint32_t _ulWrAddr, uint8_t *_ucpSrc, int _uiWrLen)
{
	uint16_t i;
	uint16_t j;
	uint16_t uiTemp;

	/* 如果偏移地址超过芯片容量，则退出 */
	if ((_ulWrAddr + _uiWrLen) >= FLASH_SIZE)
	{
		return 1;
	}

	/* 长度为0时不继续操作,直接认为成功,否则起始地址为奇地址会出错 */
	if (_uiWrLen == 0)
	{
		return 0;
	}

	i = (uint16_t)(_ulWrAddr & (SECTOR_SIZE - 1));
	uiTemp = i + _uiWrLen;
	if (uiTemp > SECTOR_SIZE)	/* 跨扇区 */
	{
		j = uiTemp - SECTOR_SIZE;
		uiTemp = SECTOR_SIZE - i;

		/* 先写第1个扇区 */
		if (WriteSector(_ulWrAddr, _ucpSrc, uiTemp) == 0)
		{
			return 2;
		}

		/* 再写第2个扇区 */
		if (WriteSector(_ulWrAddr + uiTemp, _ucpSrc + uiTemp, j) == 0)
		{
			return 2;
		}
	}
	else						/* 没有跨扇区 */
	{
		if (WriteSector(_ulWrAddr, _ucpSrc, _uiWrLen) == 0)
		{
			return 2;
		}
	}
	return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
