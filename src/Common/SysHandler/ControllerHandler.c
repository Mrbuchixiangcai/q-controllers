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

���ļ����������������������صĻ���
*/
//------------------------------------------------------------------//

#include "SysDefines.h"

typedef struct{
	const char *pName;
	const EVENT_FUNC_ITEM *ItemArray;
	u16 EvtFuncTotal;
}EVENT_CTRLER_ITME;//��������¼��

#define EVENT_CONTROLLER_MAX 64
static EVENT_CTRLER_ITME gpEvtCtrlers[EVENT_CONTROLLER_MAX]={{NULL,0}};//�������б�
static u16 gEvtCtrlerNum=0;

//��ӡ��Ϣ
void ControllerDebug(void)
{
	u16 i;

	for(i=0;i<gEvtCtrlerNum;i++)
	{
		Debug("[%s] Has %u Evt\n\r",gpEvtCtrlers[i].pName,gpEvtCtrlers[i].EvtFuncTotal);
	}
}

//������ע���Լ���ϵͳ
void ControllerRegister(const EVENT_FUNC_ITEM *pItemArray,const char *pName)
{
	u16 i;
	
	for(i=0;i<EBF_MAX;i++)
	{
		if(pItemArray[i].Event==EBF_NULL || pItemArray[i].Event>=EBF_MAX) break;
		if(pItemArray[i].EvtFunc==NULL) break;
	}

	if(i)
	{
		gpEvtCtrlers[gEvtCtrlerNum].pName=pName;
		gpEvtCtrlers[gEvtCtrlerNum].ItemArray=pItemArray;
		gpEvtCtrlers[gEvtCtrlerNum].EvtFuncTotal=i;
		gEvtCtrlerNum++;
	}
}

//�������¼��ַ�
void ControllerEvtPost(EVENT_BIT_FLAG Event,int Param,void *p)
{
	u16 CtrlerNum=0,EvtNum=0;

	for(CtrlerNum=0;CtrlerNum<gEvtCtrlerNum;CtrlerNum++)//��ѯ������
	{
		const EVENT_FUNC_ITEM *pItemArray=gpEvtCtrlers[CtrlerNum].ItemArray;
		EVENT_HANDLER_RESUTL Res=EFR_OK;
		
		//if(Event!=EBF_IDLE) Debug("Find %s\n\r",gpEvtCtrlers[CtrlerNum].pName);
		for(EvtNum=0;EvtNum<gpEvtCtrlers[CtrlerNum].EvtFuncTotal;EvtNum++)//��ѯ�¼��ص�
		{			
			if(Event==pItemArray[EvtNum].Event && pItemArray[EvtNum].EvtFunc!=NULL)
			{
				//if(Event!=EBF_IDLE) Debug("  Goin %u\n\r",Event);
				Res=pItemArray[EvtNum].EvtFunc(Event,Param,p);//�¼��������ص�
				if(Res==EFR_STOP) goto EvtFinish;
				break;//ͬһ���¼�ÿ��������ֻ�ܶ�Ӧһ���ص�
			}
		}
	}

EvtFinish:
	//EventMemoryFree(Event,Param,p);//�¼��ڴ����
	return;
}


