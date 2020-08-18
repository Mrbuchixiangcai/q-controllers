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

���ļ�����Ŀ�������Ҫ������������ʾ����ʾ�˿�����Ӧ����δ�����qwifi��ͨ�ţ�
������Ҫ�Ӵ��κ��ַ�����������������qcom��api���ɡ�
����û���Ҫ�Լ�дһ��������qwifi�������򽫱��ļ������������ɡ�
*/
//------------------------------------------------------------------//

#include "SysDefines.h"
#include "Product.h"
#include "StrParse.h"

//�յ�qwifi״̬�¼�
static EVENT_HANDLER_RESUTL QWIFI_State_EF(EVENT_BIT_FLAG Event,QWIFI_STATE State,const char *pCmd)
{
	Debug("Call %s [%u]%s\n\r",__FUNCTION__,State,pCmd);
	Debug("LastCmd:%s\n\r",QCom_GetLastCmd());
	return EFR_OK;
}

//�յ�qwifi��app��ť�¼�
static EVENT_HANDLER_RESUTL QWIFI_Key_EF(EVENT_BIT_FLAG Event,u32 Key,u32 DevID)
{
	Debug("Call %s %u Dev%u\n\r",__FUNCTION__,Key,DevID);
	Debug("LastCmd:%s\n\r",QCom_GetLastCmd());
	return EFR_OK;
}

//�յ�qwifiϵͳ��Ϣ�¼�
static EVENT_HANDLER_RESUTL QWIFI_Message_EF(EVENT_BIT_FLAG Event,u32 Len,const char *pMsg)
{
	Debug("Call %s [%u]%s\n\r",__FUNCTION__,Len,pMsg);
	Debug("LastCmd:%s\n\r",QCom_GetLastCmd());
	return EFR_OK;
}

//����ϵͳ��Ϣ���յ��ظ�
static EVENT_HANDLER_RESUTL QWIFI_Message_Res_EF(EVENT_BIT_FLAG Event,u32 Len,const char *pMsg)
{
	Debug("Event %u call %s [%u]%s\n\r",Event,__FUNCTION__,Len,pMsg);
	Debug("LastCmd:%s\n\r",QCom_GetLastCmd());
	return EFR_OK;
}

//�յ�qwifi�����¼�
static EVENT_HANDLER_RESUTL QWIFI_Varible_EF(EVENT_BIT_FLAG Event,int Value,const char *pVarTag)
{
	Debug("Call %s [%s]=%d\n\r",__FUNCTION__,pVarTag,Value);
	Debug("LastCmd:%s\n\r",QCom_GetLastCmd());
	return EFR_OK;
}

//��������ָ��յ��ظ�
static EVENT_HANDLER_RESUTL QWIFI_Varible_Res_EF(EVENT_BIT_FLAG Event,int Value,const char *pVarTag)
{
	Debug("Event %u call %s [%s]=%d\n\r",Event,__FUNCTION__,pVarTag,Value);
	Debug("LastCmd:%s\n\r",QCom_GetLastCmd());

#if 0//�˲������ڽ��������ʾ��
	if(strlen(QCom_GetLastCmd()))
	{
		const char *pLastCmd=QCom_GetLastCmd();
		char *pCmd[6];
		char *pBuf=NULL;
		u16 i,Num=0;
		
		pBuf=Q_Malloc(64);
		Num=StrCmdParse(pLastCmd,pCmd,pBuf,TRUE);//����ָ��Ͳ���

		for(i=0;i<Num;i++)
		{
			Debug("%u:%s\n\r",i,pCmd[i]);
		}

		Q_Free(pBuf);
	}
#endif

	return EFR_OK;
}

//�����������ע��
static const EVENT_FUNC_ITEM gQWifiController[]={
{EBF_QWIFI_STATE,QWIFI_State_EF},
{EBF_QWIFI_KEY,QWIFI_Key_EF},
{EBF_QWIFI_MSG,QWIFI_Message_EF},
{EBF_QWIFI_MSG_RET,QWIFI_Message_Res_EF},
{EBF_QWIFI_STR_RET,QWIFI_Message_Res_EF},
{EBF_QWIFI_VAR,QWIFI_Varible_EF},
{EBF_QWIFI_READ_VAR_RET,QWIFI_Varible_Res_EF},
{EBF_QWIFI_READ_VAR_ERROR,QWIFI_Varible_Res_EF},
{EBF_QWIFI_SET_VAR_RET,QWIFI_Varible_Res_EF},
{EBF_QWIFI_SET_VAR_ERROR,QWIFI_Varible_Res_EF},




{EBF_NULL,NULL}//����������һ���Դ˽�β
};

void QWifiControllerReg(void)
{
	ControllerRegister(gQWifiController,"QWifi Controller");
}

