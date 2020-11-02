//--------------------------Q Controllers---------------------------//
/*
Q-Ctrl��һ�׻����¼��Ŀ�������ܣ������MCV��ܣ�Q-Ctrl����Э��
�洢��Data�������������IO������������Controller�����ߵ��߼�����
���DIC��ܡ�

Q-Ctrl����stm32�д��������������ֱ�ӵ��ã�Ҳ������ֲ��������Ƭ��ƽ̨��
�������ϵͳ��֧�֣������ؿ�������̹��������£��ɴ���������Ҫ����ϵͳ
���ܴ���ĸ���ҵ��

���л��ڿ�������ƽ̨���еĿ�����������Ʒ��������ϵ�����Ŷӣ���ѷ�����
��������Ƶ��q-iot.cn�����д������г���������Ӧ��������۳�˰�Ѽ�ά�����ú�
��ȫ���ṩ�������ߣ��Դ˹������ڿ�Դ��ҵ��

By Karlno ����Ƽ�

���ļ�������һ���洢��оƬflash�ڲ������ݴ洢����
*/
//------------------------------------------------------------------//
#include "SysDefines.h"
#include "Product.h"

static const RFS_BLOCK gDefBlock={//Ĭ������
1,//Ver
0,//Num
0,//DebugFlags
9600,//u32 Com2Baud;//us2������

WRF_RSSI_THRESHOLD,//RFSI_RSSI_THRD
0,//u32 RFSI_FLY_ADDR
0,//u32 RFSI_BROTHER_ADDR;//͸���ֵܵ�ַ

FALSE,//bool SnAuth;//������֤
};

static RFS_BLOCK gDataBlock;//���û���

void RFS_Debug(char *pOutStream)
{
	Debug("  -------------------------------------------------------------------\n\r");

	if(pOutStream!=NULL)
	{
		pOutStream[0]=0;
		sprintf((void *)&pOutStream[strlen(pOutStream)]," HwId:%u \n\r",GetHwID(NULL));
		sprintf((void *)&pOutStream[strlen(pOutStream)]," SoftVer:%u.%u \n\r",__gBinSoftVer,RELEASE_DAY);
		sprintf((void *)&pOutStream[strlen(pOutStream)]," Com2_Baud:%u \n\r",gDataBlock.Com2Baud);
		sprintf((void *)&pOutStream[strlen(pOutStream)]," My WAddr:0x%x \n\r",WNetMyAddr());
		if(RFS_DB()->SnAuth) sprintf((void *)&pOutStream[strlen(pOutStream)]," Fly WAddr:0x%x \n\r",gDataBlock.RFSI_FLY_ADDR);
		sprintf((void *)&pOutStream[strlen(pOutStream)]," Broterh:0x%x \n\r",gDataBlock.RFSI_BROTHER_ADDR);
	}

	//���ڴ�ӡ
	Frame();Debug("  |HwId:%u\n\r",GetHwID(NULL));
	Frame();Debug("  |SoftVer:%u.%u\n\r",__gBinSoftVer,RELEASE_DAY);
	Frame();Debug("  |Com2 Baudrate:%u\n\r",gDataBlock.Com2Baud);
	Frame();Debug("  |MY_WNET_ADDR:0x%x\n\r",WNetMyAddr());
	Frame();Debug("  |RFSI_RSSI_THRD:%u\n\r",gDataBlock.RFSI_RSSI_THRD);
	Frame();Debug("  |RFSI_FLY_ADDR:0x%x\n\r",gDataBlock.RFSI_FLY_ADDR);
	Frame();Debug("  |RFSI_BROTHER_ADDR:0x%x\n\r",gDataBlock.RFSI_BROTHER_ADDR);
	Frame();Debug("  |Debug:",gDataBlock.RFSI_BROTHER_ADDR);
		if(NeedDebug(DFT_WNET))Debug("wnet ");
		if(NeedDebug(DFT_WPKT))Debug("wpkt ");
		if(NeedDebug(DFT_WDEV))Debug("wdev ");
		Debug("\n\r");
	Debug("  -------------------------------------------------------------------\n\r");
}

//���ݴ洢��ʼ��
void RFS_Init(void)
{
	if(sizeof(RFS_BLOCK)>ROM_FLASH_PAGE_SIZE)
	{
		Debug("InfoSaveBlock %u too big!\n\r",sizeof(RFS_BLOCK));
		while(1);
	}

	//��������
	Rom_ReadPage((void *)&gDataBlock,IAP_ROM1_ADDR+ROM_INFO_SAVE_PAGE*ROM_FLASH_PAGE_SIZE,sizeof(gDataBlock));

	if(MakeHash33((void *)&gDataBlock,sizeof(RFS_BLOCK)-4)!=gDataBlock.ChkSum)
	{
		Debug("RFS ChkSum error!\n\r");
		RFS_BurnDefaultToRom();
	}

	if(gDataBlock.Ver!=gDefBlock.Ver)
	{
		Debug("Ver Error,Reset Database!\n\r");
		RFS_BurnDefaultToRom();
	}
}

//�洢Ĭ�����ݵ�rom
void RFS_BurnDefaultToRom(void)
{
	__IO u32 *pChkSum=(void *)(IAP_ROM1_ADDR+ROM_INFO_SAVE_PAGE*ROM_FLASH_PAGE_SIZE+sizeof(RFS_BLOCK)-4);
	
	MemCpy((void *)&gDataBlock,&gDefBlock,sizeof(RFS_BLOCK));
	gDataBlock.ChkSum=MakeHash33((void *)&gDataBlock,sizeof(RFS_BLOCK)-4);

	if(*pChkSum!=gDataBlock.ChkSum)		
		Rom_WritePage((void *)&gDataBlock,IAP_ROM1_ADDR+ROM_INFO_SAVE_PAGE*ROM_FLASH_PAGE_SIZE,sizeof(gDataBlock));
}

//�洢��ǰ���ݵ�rom
void RFS_BurnToRom(void)
{
	__IO u32 *pChkSum=(void *)(IAP_ROM1_ADDR+ROM_INFO_SAVE_PAGE*ROM_FLASH_PAGE_SIZE+sizeof(RFS_BLOCK)-4);

	gDataBlock.ChkSum=MakeHash33((void *)&gDataBlock,sizeof(RFS_BLOCK)-4);

	if(*pChkSum!=gDataBlock.ChkSum)	
		Rom_WritePage((void *)&gDataBlock,IAP_ROM1_ADDR+ROM_INFO_SAVE_PAGE*ROM_FLASH_PAGE_SIZE,sizeof(gDataBlock));
}

const RFS_BLOCK *RFS_DefDB(void)
{
	return &gDefBlock;
}

RFS_BLOCK *RFS_DB(void)
{
	return &gDataBlock;
}

bool RFS_GetDebugBits(DEBUG_FLAG_TYPE Flag)
{
	return ReadBit(gDataBlock.DebugFlags,Flag)?TRUE:FALSE;
}

void RFS_SetDebugBits(DEBUG_FLAG_TYPE Flag)
{
	if(Flag==DFT_MAX) gDataBlock.DebugFlags=0xff;
	else SetBit(gDataBlock.DebugFlags,Flag);
}

void RFS_ClrDebugBits(DEBUG_FLAG_TYPE Flag)
{
	if(Flag==DFT_MAX) gDataBlock.DebugFlags=0;
	else ClrBit(gDataBlock.DebugFlags,Flag);
}

