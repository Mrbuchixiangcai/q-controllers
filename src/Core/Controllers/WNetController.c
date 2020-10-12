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

���ļ�����Ŀ�������Ҫ��������WNET�Ľ���
*/
//------------------------------------------------------------------//

#include "SysDefines.h"
#include "Product.h"
#include "LedsMode.h"
#include "WDevFunc.h"


#define PAIR_LOOP_MS 2000
static void PairLoopTimer(int cnt,void *p) //��������������
{
	if(cnt==0) //������Ѱʱ�䣬�˳�
	{
		SetWorkMode(MNM_IDLE);
		LedIndicate(LMO_IDLE);
		return;
	}
	else if(cnt>=0xffff){}
	else {cnt--;}
	
	if(WorkMode()==MNM_START_PAIR)//ѭ����������
	{
		WDevSearchDev();
		AddOnceMsFunc(PAIR_LOOP_MS,PairLoopTimer,cnt,NULL);
	}
	else if(WorkMode()==MNM_SYNC)
	{
		AddOnceMsFunc(PAIR_LOOP_MS,PairLoopTimer,cnt,NULL);
	}
	else if(WorkMode()==MNM_PRE_WORK)
	{
		WDevSyncToDev(RFS_DB()->RFSI_BROTHER_ADDR);//����ͬ�������ֵ�
		AddOnceMsFunc(PAIR_LOOP_MS,PairLoopTimer,cnt,NULL);
	}
	else //ģʽ�ı䣬ֹͣѭ��
	{

	}
}

//�������¼�������
static EVENT_HANDLER_RESUTL Init_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	IOIN_OpenExti(IOIN_KEY1);
	IOIN_OpenExti(IOIN_KEY2);

	gWnetTimer=AddSysTimer(STT_MANUAL,WNET_TIMER_POLL_MS,EBF_NULL,WNetTimerPoll,FALSE);

	WRF_DRV.pWRF_Init(WNetMyAddr(),RFS_DB()->RFSI_RSSI_THRD);//Ӳ���ĳ�ʼ��
	WNetInit();//wnet�ĳ�ʼ��

	if(RFS_DB()->RFSI_BROTHER_ADDR)
	{
		SetWorkMode(MNM_PRE_WORK);
		LedIndicate(LMO_WAIT_BROTHER);
		AddOnceMsFunc(Rand(0x3ff),PairLoopTimer,60000/PAIR_LOOP_MS,NULL);//����һ���ӵ���Ѱʱ��
	}
	else
	{
		SetWorkMode(MNM_IDLE);
	}

	return EFR_OK;
}

//ϵͳwhile(1)�յ�ʱѭ�����õ��¼�
static EVENT_HANDLER_RESUTL Idle_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	if(IOIN_ReadIoStatus(IOIN_WRF_DRV_INT)==WRF_DRV_INT_LEVEL)
	{
		WRF_DRV.pWRF_ISR();
	}
	
	return EFR_OK;
}

//�����ݽ���
static EVENT_HANDLER_RESUTL WNet_PacketIn_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	WNetPacketInHandler();
	return EFR_OK;
}

//Ӧ�ð�����
static EVENT_HANDLER_RESUTL WNet_DataIn_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	u8 *pData;
	u16 DataLen;
	u32 SrcAddr,DstAddr;

	pData=GetWNetRecvBuf(&DataLen,&SrcAddr,&DstAddr);
	if(pData!=NULL)
	{		
		if(DataLen) WNetDataInHandler(SrcAddr,DstAddr,pData,DataLen);
		WNetAppCopyRecvDataFinish();//������ϣ������ڴ�					
	}

	return EFR_OK;
}

//���������²��ɿ�����¼�
static EVENT_HANDLER_RESUTL KeyHandler_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	u16 KeyIo=a&0xffff;
	u16 Ms=a>>16;

	//Debug("New Key%u %umS\n\r",KeyIo,Ms);

	if(KeyIo==0)
	{
		if(Ms>=5000) //����ֵ�
		{
			if(RFS_DB()->RFSI_BROTHER_ADDR)
			{
				SetWorkMode(MNM_IDLE);
				LedIndicate(LMO_IDLE);
				WDevUnbindDev(RFS_DB()->RFSI_BROTHER_ADDR);
				RFS_DB()->RFSI_BROTHER_ADDR=0;
				AddNextVoidFunc(FALSE,RFS_BurnToRom);
			}
			else //��û�ֵ�
			{
				//LedIndicate(LMO_ERR);//�����
			}
		}
		else if(Ms>=2000)//���ֵ�
		{
			if(RFS_DB()->RFSI_BROTHER_ADDR)//�Ѿ����ֵ���
			{
				//LedIndicate(LMO_ERR);//�����
			}
			else if(WorkMode()==MNM_IDLE)
			{
				//������߼�
				SetWorkMode(MNM_START_PAIR);
				LedIndicate(LMO_WAIT_PAIR);//׼�����
				AddOnceMsFunc(PAIR_LOOP_MS,PairLoopTimer,60000/PAIR_LOOP_MS,NULL);//����һ���ӵ���Ѱʱ��
				WDevSearchDev();
			}
		}
		else //С��2s�İ���
		{
			if(WorkMode()==MNM_START_PAIR) //��ǰ�����״̬����ȡ�����
			{
				SetWorkMode(MNM_IDLE);
				LedIndicate(LMO_CANCLE);
			}
			else if(WorkMode()==MNM_WORK)
			{
				WDevSyncToDev(RFS_DB()->RFSI_BROTHER_ADDR);
			}
		}
	}

	return EFR_OK;
}

//�����������ע��
static const EVENT_FUNC_ITEM gWNetController[]={
{EBF_INIT,Init_EF},
{EBF_IDLE,Idle_EF},
{EBF_WNET_PACKET_IN,WNet_PacketIn_EF},
{EBF_WNET_DATA_IN,WNet_DataIn_EF},
{EBF_KEY,KeyHandler_EF},

{EBF_NULL,NULL}//����������һ���Դ˽�β
};

void WNetControllerReg(void)
{
	ControllerRegister(gWNetController,"WNet Controller");
}

