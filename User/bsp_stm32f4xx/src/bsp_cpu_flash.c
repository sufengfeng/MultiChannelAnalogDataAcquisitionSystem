/*
*********************************************************************************************************
*
*	ģ������ : CPU�ڲ�Flash����ģ��
*	�ļ����� : bsp_cpu_flash.c
*	��    �� : V1.0
*	˵    �� : ʵ�ֶ�дCPU�ڲ�Flash�� ʵ�ֲ������ʽӿ�
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-01-01 armfly  ��ʽ����
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*********************************************************************************************************
*/

#include "bsp.h"

static uint8_t WriteSector(uint32_t _ulWrAddr, uint8_t *_ucpSrc, uint16_t _uiWrLen);
static void _ProgSector(uint32_t _ulWrAddr,	uint8_t *_ucpSrc, uint16_t _uiWrLen);
static int _WriteFlash(uint32_t _ulWrAddr, uint8_t *_ucpSrc, int _uiWrLen);

/*
*********************************************************************************************************
*	�� �� ��: bsp_ReadCpuFlash
*	����˵��: ��CPU�ڲ�Flashָ����ַ _ulSrcAddr ��ʼ��ȡ_ulSize�����ݵ� _ucpDst.
*	��    �Σ�
*	uint8_t *_ucpDst,		: Ŀ�껺������ַ
*	uint32_t _ulSrcAddr,	: ��ʼ��ַ,ƫ�Ƶ�ַ
*	uint32_t _ulSize		: ����,��λ:�ֽ�
*	�� �� ֵ: 0-�ɹ���1-���ݳ��Ȼ��ַ���
*********************************************************************************************************
*/
uint8_t bsp_ReadCpuFlash(uint8_t *_ucpDst, uint32_t _ulSrcAddr, uint32_t _ulSize)
{
	uint32_t i;
	uint16_t uiTemp;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if (_ulSrcAddr >= FLASH_SIZE)
	{
		return 0;
	}

	/* ����Ϊ0ʱ����������,������ʼ��ַΪ���ַ����� */
	if (_ulSize == 0)
	{
		return 0;
	}

	_ulSrcAddr = _ulSrcAddr + FLASH_BASE_ADDR;

	/* Flash��16bit������,ÿ�οɶ����ֽ� */
	/* �������Ҫ��ȡ��ַΪ����ʱ�Ĵ��� */
	if (_ulSrcAddr & 0x01)
	{
		_ulSrcAddr--;
		((uint8_t *) _ucpDst)[0] = (((uint16_t *) _ulSrcAddr)[0]) >> 8;
		_ucpDst += 1;
		_ulSrcAddr += 2;
		_ulSize--;
	}

	/* (uint16_t *)ֻ����ż����ַ */
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
*	�� �� ��: bsp_WriteCpuFlash
*	����˵��: дFlash,д������ݳ������ΪFLASH_SIZE��֧������������������
*	��    �Σ�
*			_ulWrAddr : Ŀ���ַ,ƫ�Ƶ�ַ
*			_ucpSrc   : ����Դָ��
*			_uiWrLen  : ���ݳ���,��λ:�ֽ�
*	�� �� ֵ: 0-�ɹ���1-���ݳ��Ȼ��ַ�����2-дFlash����(����Flash������)
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
*	�� �� ��: bsp_CmpCpuFlash
*	����˵��: �Ƚ�Flashָ����ַ������
*	��    �Σ�
*			_ulSrcAddr : ��ʼ��ַ,ƫ�Ƶ�ַ
*			*_ucpTar   : ���ݻ�����
*			_ulSize    : ����,��λ:�ֽ�
*	�� �� ֵ: 0 = ���, 1 = ����
*********************************************************************************************************
*/
static uint8_t bsp_CmpCpuFlash(uint32_t _ulSrcAddr, uint8_t *_ucpTar, uint32_t _ulSize)
{
	uint32_t i;
	uint16_t uiTemp;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if (_ulSrcAddr >= FLASH_SIZE)
	{
		return 1;
	}

	/* ����Ϊ0ʱ����������,������ʼ��ַΪ���ַ����� */
	if (_ulSize == 0)
	{
		return 0;
	}

	_ulSrcAddr = _ulSrcAddr + FLASH_BASE_ADDR;


	/* Flash��16bit������,ÿ�οɶ����ֽ� */
	/* �������Ҫ��ȡ��ַΪ����ʱ�Ĵ��� */
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

	/* (uint16_t *)ֻ����ż����ַ */
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
*	�� �� ��: _ProgSector
*	����˵��: ���һ������
*	��    �Σ�
*			_ulSrcAddr : ��ʼ��ַ,ƫ�Ƶ�ַ
*			*_ucpTar   : ���ݻ�����
*			_ulSize    : ����,��λ:�ֽ�
*	�� �� ֵ: 0 = ���, 1 = ����
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
*	�� �� ��: WriteSector
*	����˵��: д1��������У��,�������ȷ������д���Ρ��������Զ���ɲ���������
*	��    �Σ�
*			_ulWrAddr : Ŀ���ַ,ƫ�Ƶ�ַ
*			_ucpSrc   : ����Դָ��
*			_uiWrLen  : ����, ���ܴ��� SECTOR_SIZE (4K)
*	�� �� ֵ: 0 = ���, 1 = ����
*********************************************************************************************************
*/
static uint8_t WriteSector(uint32_t _ulWrAddr, uint8_t *_ucpSrc, uint16_t _uiWrLen)
{
	uint16_t i;
	uint16_t j;				/* ������ʱ */
	uint32_t ulFirstAddr;		/* ������ַ */
	uint8_t ucaFlashBuf[SECTOR_SIZE];
	uint8_t cRet;

	/* ���ƫ�Ƶ�ַ����оƬ���������˳� */
	if (_ulWrAddr >= FLASH_SIZE)
	{
		return 0;
	}

	/* ������ݳ��ȴ����������������˳� */
	if (_uiWrLen > SECTOR_SIZE)
	{
		return 0;
	}

	/* ����Ϊ0ʱ����������,ֱ����Ϊ�ɹ�,������ʼ��ַΪ���ַ����� */
	if (_uiWrLen == 0)
	{
		return 1;
	}

	/* ���FLASH�е�����û�б仯,��дFLASH */
	bsp_ReadCpuFlash(ucaFlashBuf, _ulWrAddr, _uiWrLen);
	if (memcmp(ucaFlashBuf, _ucpSrc, _uiWrLen) == 0)
	{
		return 1;
	}

	ulFirstAddr = _ulWrAddr & SECTOR_MASK;

	if (_uiWrLen == SECTOR_SIZE)		/* ������������д */
	{
		for	(i = 0; i < SECTOR_SIZE; i++)
		{
			ucaFlashBuf[i] = _ucpSrc[i];
		}
	}
	else						/* ��д�������� */
	{
		/* �Ƚ��������������ݶ��� */
		/*
		Ϊ�˺���������ֲ����ִ��Ч�ʣ��������ʹ�� ReadFlash() ������
		MyMemCpy(); ReadFlash �ǰ�16bit���ʣ�Ч��Ҫ��Щ��
		*/
		bsp_ReadCpuFlash(ucaFlashBuf, ulFirstAddr, SECTOR_SIZE);

		/* ���������ݸ��� */
		i = _ulWrAddr & (SECTOR_SIZE - 1);
		memcpy(&ucaFlashBuf[i], _ucpSrc, _uiWrLen);
	}

	/* д��֮�����У�飬�������ȷ����д�����3�� */
	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_Unlock();
	cRet = 1;
	for (i = 0; i < 3; i++)
	{
		FLASH_ErasePage(ulFirstAddr + FLASH_BASE_ADDR);			/* ����1������ */
		_ProgSector(ulFirstAddr, ucaFlashBuf, SECTOR_SIZE);		/* ���һ������ */

		/* ע��:������䲻����ֲ������ Flash,��Ҫ�� ulFirstAddr ��ʼ�����ݶ���RAM */
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

			/* ���ܵ��� OSTimeDly(50), ��Ϊ�жϺͲ���ϵͳ���ȿ��ܹر�.
			ֱ��Ӳ��ʱ�ɿ�Щ */
			for (j = 0; j < 10000; j++);
		}
	}

	FLASH_Lock();
	return cRet;
}

/*
*********************************************************************************************************
*	�� �� ��: _WriteFlash
*	����˵��: дFlash,д������ݳ������ΪSECTOR_SIZE��֧�ֿ�����������
*	��    �Σ�
*			_ulWrAddr : Ŀ���ַ,ƫ�Ƶ�ַ
*			_ucpSrc   : ����Դָ��
*			_uiWrLen  : ���ݳ���,��λ:�ֽ�
*	�� �� ֵ: 0-�ɹ���1-���ݳ��Ȼ��ַ�����2-дFlash����(����Flash������)
*********************************************************************************************************
*/
static int _WriteFlash(uint32_t _ulWrAddr, uint8_t *_ucpSrc, int _uiWrLen)
{
	uint16_t i;
	uint16_t j;
	uint16_t uiTemp;

	/* ���ƫ�Ƶ�ַ����оƬ���������˳� */
	if ((_ulWrAddr + _uiWrLen) >= FLASH_SIZE)
	{
		return 1;
	}

	/* ����Ϊ0ʱ����������,ֱ����Ϊ�ɹ�,������ʼ��ַΪ���ַ����� */
	if (_uiWrLen == 0)
	{
		return 0;
	}

	i = (uint16_t)(_ulWrAddr & (SECTOR_SIZE - 1));
	uiTemp = i + _uiWrLen;
	if (uiTemp > SECTOR_SIZE)	/* ������ */
	{
		j = uiTemp - SECTOR_SIZE;
		uiTemp = SECTOR_SIZE - i;

		/* ��д��1������ */
		if (WriteSector(_ulWrAddr, _ucpSrc, uiTemp) == 0)
		{
			return 2;
		}

		/* ��д��2������ */
		if (WriteSector(_ulWrAddr + uiTemp, _ucpSrc + uiTemp, j) == 0)
		{
			return 2;
		}
	}
	else						/* û�п����� */
	{
		if (WriteSector(_ulWrAddr, _ucpSrc, _uiWrLen) == 0)
		{
			return 2;
		}
	}
	return 0;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
