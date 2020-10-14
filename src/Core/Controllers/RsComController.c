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

���ļ�����Ŀ�������Ҫ������͸��ͨ�ŵ��߼�����
*/
//------------------------------------------------------------------//
#include "SysDefines.h"
#include "Product.h"
#include "WDevFunc.h"

//�������¼�������
static EVENT_HANDLER_RESUTL RsComInit_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	COM2_Init(RFS_DB()->Com2Baud);
	Com2_DmaConfig();
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);

	return EFR_OK;
}

//����ʧ�ܣ��ı�ģʽ
static void PassToDev_CB(WNET_BASE_RES TransRes, WNET_INFO_BLOCK *pBlock)
{
	if(TransRes!=WBR_OK)
	{
		Debug("Trans to 0x%x Failed[%u]!\n\r",pBlock->DstAddr,TransRes);

		SetWorkMode(MNM_PRE_WORK);
		LedIndicate(LMO_WAIT_BROTHER);
		WDevSyncToDev(RFS_DB()->RFSI_BROTHER_ADDR);//����ͬ�������ֵ�
	}
}

//�յ�rs��������
static EVENT_HANDLER_RESUTL RsComHandler_EF(EVENT_BIT_FLAG Event,int DateLen,u8 *pData)
{
	//Debug("RS COM:\n\r");
	//DisplayBuf(pData,DateLen,16);

	if(DateLen==0 || pData==NULL) return EFR_OK;
	if(WorkMode()!=MNM_WORK) return EFR_OK;

	LedSet(IOOUT_LED2,0);
	WDevPassData(RFS_DB()->RFSI_BROTHER_ADDR,DateLen,pData,PassToDev_CB);
	LedSet(IOOUT_LED2,1);

	if(IsHeapRam(pData)) Q_Free(pData);

	return EFR_OK;
}

//���������²��ɿ�����¼�
static EVENT_HANDLER_RESUTL KeyHandler_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	u16 KeyIo=a&0xffff;
	u16 Ms=a>>16;

	if(KeyIo==1)
	{
		
	}

	return EFR_OK;
}


//�����������ע��
static const EVENT_FUNC_ITEM gRsComController[]={
{EBF_INIT,RsComInit_EF},
{EBF_RS_COM_CMD,RsComHandler_EF},
{EBF_KEY,KeyHandler_EF},

{EBF_NULL,NULL}//����������һ���Դ˽�β
};

void RsComControllerReg(void)
{
	ControllerRegister(gRsComController,"My New Controller");
}

