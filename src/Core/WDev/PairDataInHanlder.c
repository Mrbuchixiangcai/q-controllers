#include "SysDefines.h"
#include "WDevFunc.h"

//������̣������a�����󶨶�b
//a b�Ⱥ�������ģʽ
//a[start] �����㲥��add client  >> b[start]
//a[start] << b[sync] ����ͬ����sync
//a[sync] ����������add client >> b[sync]
//b�̻��Է���ַ
//a�յ��������ĵײ�ack�� �̻��Է���ַ
//�����
//[sync]״̬�£������ᷢ�͹㲥�� Ҳ������������豸�İ�


void PairSyncFinishHook(bool IsSucess)
{
	if(IsSucess) 
	{
		Debug("Brother Sync Sucess!\n\r");
		SetWorkMode(MNM_WORK);
		LedIndicate(LMO_WORK);
	}
	else
	{

	}
}

//��԰����ͳɹ�
static void PairToDev_CB(WNET_BASE_RES TransRes, WNET_INFO_BLOCK *pBlock)
{
	if(TransRes==WBR_OK)
	{
		if(ClientAddr(pBlock->DstAddr)!=0xffff) //��������ȷ���󶨹�ϵ
		{
			Debug("Pair To Dev 0x%x\n\r",pBlock->DstAddr);
			RFS_DB()->RFSI_BROTHER_ADDR=pBlock->DstAddr;
			AddNextVoidFunc(FALSE,RFS_BurnToRom);
			SetWorkMode(MNM_WORK);
			LedIndicate(LMO_WORK);
		}
	}
	else
	{
		Debug("Pair To Dev 0x%x Failed!\n\r",pBlock->DstAddr);
		SetWorkMode(MNM_START_PAIR);
	}
}

//͸��������
static void PairPassThrough(WSTR_PACKET *pDataPkt,u32 SrcAddr,u32 DstAddr)
{
	if(pDataPkt->Type!=WPT_PASS_THROUGH) return;
	if(SrcAddr!=RFS_DB()->RFSI_BROTHER_ADDR) return;

	Com2_Send_Dma(pDataPkt->Data,pDataPkt->DataLen);
}

//�ֵܰ�����
void PairPacketIn(WCON_PACKET *pConPkt,u32 SrcAddr,u32 DstAddr)
{
	switch(pConPkt->Type)
	{
		case WPT_PASS_THROUGH:
			if(ClientAddr(DstAddr) != WNET_BROADCAST_ADDR)//�ǹ㲥
			{
				PairPassThrough((void *)pConPkt,SrcAddr,DstAddr);
			}
			break;
		case WPT_ADD_CLIENT:
			if(GroupAddr(SrcAddr)==GroupAddr(WNetMyAddr()))//ͬ���豸
			{
				if(ClientAddr(DstAddr) == WNET_BROADCAST_ADDR)//�㲥
				{
					if(WorkMode()!=MNM_START_PAIR) break;
					SetWorkMode(MNM_SYNC);
					WDevSyncToDev(SrcAddr);//�����̽
					RevertWorkMode(1000);
				}
				else//����
				{
					if(WorkMode()!=MNM_SYNC) break;
					if(RFS_DB()->RFSI_BROTHER_ADDR==0)
					{
						RFS_DB()->RFSI_BROTHER_ADDR=SrcAddr;
						AddNextVoidFunc(FALSE,RFS_BurnToRom);
						SetWorkMode(MNM_WORK);
						LedIndicate(LMO_WORK);
					}
				}
			}
			break;
		case WPT_SYNC:
			if(ClientAddr(DstAddr) == WNET_BROADCAST_ADDR) break;//������㲥
			if(WorkMode()==MNM_START_PAIR)
			{
				if(GroupAddr(SrcAddr)==GroupAddr(WNetMyAddr()))//ͬ���豸
				{
					SetWorkMode(MNM_SYNC);
					WDevPairDev(SrcAddr,PairToDev_CB);//���͵�����԰������̻���ַ
					RevertWorkMode(500);
				}
			}
			else if(WorkMode()==MNM_SYNC) //�Ѿ���������Ԥ����ˣ��������Է���ͬ����
			{

			}
			else 
			{
				if(SrcAddr == RFS_DB()->RFSI_BROTHER_ADDR) 
				{
					if(WorkMode()==MNM_WORK) 
					{
						Debug("Brother Key Come on\n\r");
						LedIndicate(LMO_KEY_INDICATE);//��Ӧ�Է��İ�������
					}
					else if(WorkMode()==MNM_PRE_WORK) 
					{
						PairSyncFinishHook(TRUE);//���߳ɹ�
					}
				}
				else //���ֵܷ����İ�
				{
					WDevUnbindDev(SrcAddr);
				}
			}
			break;
		case WPT_UNBIND:
			if(ClientAddr(DstAddr) == WNET_BROADCAST_ADDR) break;//������㲥
			if(SrcAddr != RFS_DB()->RFSI_BROTHER_ADDR) break;

			Debug("Unbind By 0x%x\n\r",SrcAddr);
			SetWorkMode(MNM_IDLE);
			LedIndicate(LMO_IDLE);
			RFS_DB()->RFSI_BROTHER_ADDR=0;
			AddNextVoidFunc(FALSE,RFS_BurnToRom);
			break;
	}
}

