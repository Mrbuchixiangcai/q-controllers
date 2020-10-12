#include "SysDefines.h"

static volatile u32 gHostLastActionTim=0;//������ʱ��rtc
static volatile bool HostSyncFinishFlag=FALSE;//�Ƿ���������

//�豸ͬ�����
void HostSyncFinishHook(bool Online)
{
	Debug("Host Sync Finish\n\r");

	HostSyncFinishFlag=Online;
	
	//if(Online) VarUpdInit();//�趨�����ϱ�

#if PRODUCT_IS_WAVER
#if OPEN_USER_HOOK
	UserConnHook(Online);
#endif

#if !USER_COM_HOOK			
	if(Online) UserComBulkSend("#con\r");
	else UserComBulkSend("#off\r");
#endif

#endif
}

bool HostIsOnline(void)
{
	return HostSyncFinishFlag;
}

void HostActive(u32 HostWAddr)
{
	u32 Fly=RFS_DB()->RFSI_FLY_ADDR;
	
	if(GroupAddr(Fly)==WDG_FLY && Fly==HostWAddr) gHostLastActionTim=GetRtcCount();
}

static void DevOnlinePollRes(WNET_BASE_RES TransRes, WNET_INFO_BLOCK *pBlock)
{
	if(TransRes==WBR_OK)
	{
		//Debug("Beat Ok\n\r");
	}
	else
	{
		Debug("Beat Faild\n\r");
	}
}

//�豸������ѵ֪̽����
void DevOnlinePoll(int a,void *p)
{
	u32 Now=GetRtcCount();
	DeleteSecFuncByCB(DevOnlinePoll);

	if(Now-gHostLastActionTim < WDEV_LOST_SEC)
	{
		AddOnceSecFunc(Now-gHostLastActionTim,DevOnlinePoll,0,NULL);
	}
	else
	{
		if(RFS_DB()->RFSI_FLY_ADDR)//�Ѿ����
		{
			WCON_PACKET *pConPkt=(void *)Q_Malloc(64);
			u16 PktSn;
				
			pConPkt->Type=WPT_CONFIG;
			pConPkt->Res=WPR_MAIN;
			pConPkt->PktCnt=GetWNetPktCnt();

			//��֪�汾�ţ����ò���
			pConPkt->Num=pConPkt->DataLen=0;
			WPktSetAttrib(pConPkt,WAA_GET,WDA_BEAT,0);
			
			PktSn=WNetSendPkt(RFS_DB()->RFSI_FLY_ADDR,(void *)pConPkt,(pStdFunc)DevOnlinePollRes);
			if(PktSn==0)
			{
				Debug("Send Temp Faild!\n\r");
			}

			Q_Free(pConPkt);
		}
		
		AddOnceSecFunc(WDEV_LOST_SEC,DevOnlinePoll,0,NULL);
	}
}



