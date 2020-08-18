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

���ļ�������һ��������������������������qwifiͨ��com3���ص�ָ������
��������һϵ�к���������������������ã��Է������qwifi����ָ��
*/
//------------------------------------------------------------------//

#include "SysDefines.h"
#include "Product.h"

#define QCOM_OPEN_DEBUG 0//�Ƿ���������Ϣ
#if QCOM_OPEN_DEBUG
#define QCC_Debug Debug
#else
#define QCC_Debug (void)
#endif

#define VAR_TAG_LEN 8

#if 1 //���ⲿ�õĽӿ�
static char CmdBuf[32]={0};

//��ȡ����ָ��棬�����һ��ָ��
const char *QCom_GetLastCmd(void)
{
	return CmdBuf;
}

//��qwifi���ͻ�ȡ����ֵ��ָ��
void QCom_GetVarValue(const char *pTag)
{
	if(strlen(pTag)==VAR_TAG_LEN)
	{
		CmdBuf[0]=0;
		sprintf(CmdBuf,"#var %s\r",pTag);
		Com3_Send(strlen(CmdBuf),CmdBuf);
	}
	else
	{
		Debug("Var tag len error!\n\r");
	}
}

//��qwifi�������ñ���ֵ��ָ��
void QCom_SetVarValue(const char *pTag,int Value,bool Signed)
{
	if(strlen(pTag)==VAR_TAG_LEN)
	{
		CmdBuf[0]=0;
		if(Signed) sprintf(CmdBuf,"#var %s %d\r",pTag,Value);
		else sprintf(CmdBuf,"#var %s %u\r",pTag,Value);
		Com3_Send(strlen(CmdBuf),CmdBuf);
	}
	else
	{
		Debug("Var tag len error!\n\r");
	}
}

//��qwifi�����ַ���ָ��
void QCom_SendStr(u32 StrId,const char *pStr)
{
	char *p=Q_Malloc(strlen(pStr)+16);

	sprintf(p,"#str %u %s\r",StrId,pStr);
	StrnCpy(CmdBuf,p,32);
	Com3_Send(strlen(p),p);
	
	Q_Free(p);
}

//��qwifi����ϵͳ��Ϣָ��
void QCom_SendMsg(u8 Flag,const char *pMsg)
{
	char *p=Q_Malloc(strlen(pMsg)+16);

	sprintf(p,"#msg %u %s\r",Flag,pMsg);
	StrnCpy(CmdBuf,p,32);
	Com3_Send(strlen(p),p);
	
	Q_Free(p);
}

//��qwifi����״̬��ȡָ��
void QCom_SendSta(void)
{
	CmdBuf[0]=0;
	sprintf(CmdBuf,"#sta\r");
	Com3_Send(strlen(CmdBuf),CmdBuf);
}

//��qwifi��������ָ��
void QCom_ResetQwifi(void)
{
	CmdBuf[0]=0;
	sprintf(CmdBuf,"#rst\r");
	Com3_Send(strlen(CmdBuf),CmdBuf);
}
#endif

//qwifi�ظ��ַ����Ľ�������
static void QCom_Res_Handler(u16 Num,const char **pCmd,const char *pStr)
{
	if(strcmp((void *)pCmd[0],"#rvar")==0)//�ظ��˱���ָ��
	{
		if(Num==3)//set return
		{
			if(pCmd[1][0]=='0')
			{
				SendEvent(EBF_QWIFI_SET_VAR_RET,0,(void *)pCmd[2]);
			}
			else
			{
				Debug("VAR SET RET ERROR %s\n\r",pCmd[2]);
				SendEvent(EBF_QWIFI_SET_VAR_ERROR,0,(void *)pCmd[2]);
			}
		}
		else if(Num==4)//read return
		{
			if(pCmd[1][0]=='0')
			{
				if(pCmd[3][0]=='-')
				{
					s32 VarValueS32=Str2Sint(pCmd[3]);
					SendEvent(EBF_QWIFI_READ_VAR_RET,VarValueS32,(void *)pCmd[2]);
				}
				else
				{
					u32 VarValueU32=Str2Uint(pCmd[3]);
					SendEvent(EBF_QWIFI_READ_VAR_RET,VarValueU32,(void *)pCmd[2]);
				}
			}
			else
			{
				Debug("VAR READ RET ERROR %s = x\n\r",pCmd[2]);
				SendEvent(EBF_QWIFI_READ_VAR_ERROR,0,(void *)pCmd[2]);
			}
		}
		else
		{
			Debug("VAR RET ERROR NUM = %u\n\r",Num);
		}
	}
	else if(strcmp((void *)pCmd[0],"#rstr")==0)//�ظ����ַ���ָ��
	{
		if(Num==2 && pCmd[1][0]=='0') SendEvent(EBF_QWIFI_STR_RET,0,NULL);
		else if(Num==2 && pCmd[1][0]!='0') SendEvent(EBF_QWIFI_STR_RET,1,NULL);
		else Debug("STR RET ERROR NUM = %u\n\r",Num);
	}
	else if(strcmp((void *)pCmd[0],"#rmsg")==0)//�ظ���ϵͳ��Ϣָ��
	{
		if(Num==2 && pCmd[1][0]=='0') SendEvent(EBF_QWIFI_MSG_RET,0,NULL);
		else if(Num==2 && pCmd[1][0]!='0') SendEvent(EBF_QWIFI_MSG_RET,1,NULL);
		else Debug("MSG RET ERROR NUM = %u\n\r",Num);
	}
	else if(strcmp((void *)pCmd[0],"#rsta")==0)//�ظ���״ָ̬��
	{
		if(Num==3 && pCmd[1][0]=='0')
		{
			if(strcmp((void *)pCmd[2],"rdy")==0) SendEvent(EBF_QWIFI_STATE,QSE_READY,"rdy");
			else if(strcmp((void *)pCmd[2],"con")==0) SendEvent(EBF_QWIFI_STATE,QSE_CONNECTING,"con");
			else if(strcmp((void *)pCmd[2],"dis")==0) SendEvent(EBF_QWIFI_STATE,QSE_DISCONNECT,"dis");
			else Debug("STA RET ERROR\n\r");
		}
		else 
		{
			Debug("STA RET ERROR NUM = %u\n\r",Num);
		}		
	}
	else if(strcmp((void *)pCmd[0],"#rrst")==0)//�ظ�������ָ��
	{
		if(Num==2 && pCmd[1][0]=='0') SendEvent(EBF_QWIFI_STATE,QSE_RESET,"rst");
		else Debug("RST RET ERROR\n\r");
	}
	else
	{
		Debug("QCOM RET ERROR %s\n\r",pCmd[0]);
	}
}

//qwifi�����ַ�����������
static EVENT_HANDLER_RESUTL QCom_Cmd_EF(EVENT_BIT_FLAG Event,int Len,const char *pStr)
{
	char *pCmd[6];
	char *pBuf=NULL;
	u16 i,Num=0;
	
	pBuf=Q_Malloc(Len+2);
	Num=StrCmdParse(pStr,pCmd,pBuf,TRUE);//����ָ��Ͳ���

	for(i=0;i<Num;i++)//��ӡ��ȡ���Ĳ���
	{
		QCC_Debug("[%u]%s\n\r",i,pCmd[i]);
	}

	if(pCmd[0][0]!='#') goto Finish;

	if(strlen(pCmd[0])==5)//�ذ��������5���ֽڣ���������4���ֽ�
	{
		QCom_Res_Handler(Num,pCmd,pStr);
		goto Finish;
	}
	
	if(strlen(pCmd[0])!=4) goto Finish;
	
	if(strcmp((void *)pCmd[0],"####")==0)
	{
		//do nothing
	}
	else if(strcmp((void *)pCmd[0],"#rdy")==0)//qwifi����
	{
		SendEvent(EBF_QWIFI_STATE,QSE_READY,"rdy");
	}
	else if(strcmp((void *)pCmd[0],"#con")==0)//qwifi����ok
	{
		SendEvent(EBF_QWIFI_STATE,QSE_CONNECTING,"con");
	}
	else if(strcmp((void *)pCmd[0],"#dis")==0)//qwifi�Ͽ�����
	{
		SendEvent(EBF_QWIFI_STATE,QSE_DISCONNECT,"dis");
	}
	else if(strcmp((void *)pCmd[0],"#key")==0)//qwifi��app�İ�ť������
	{
		if(Num==3) 
		{
			SendEvent(EBF_QWIFI_KEY,Str2Uint(pCmd[2]),(void *)Str2Uint(pCmd[1]));//ǿ��ת������
		}
		else
		{
			Debug("QCOM KEY CMD ERROR %s\n\r",pCmd[0]);
		}
	}
	else if(strcmp((void *)pCmd[0],"#msg")==0)//qwifi�յ�ϵͳ��Ϣ��ת����qcom
	{
		const char *pMsg=&pStr[5];
		SendEvent(EBF_QWIFI_MSG,strlen(pMsg),(void *)pMsg);
	}
	else if(strcmp((void *)pCmd[0],"#var")==0)//qwifi�ı��˱���������qcom
	{
		if(Num==3 && strlen(pCmd[1])==VAR_TAG_LEN)
		{
			if(pCmd[2][0]=='-')
			{
				s32 VarValueS32=Str2Sint(pCmd[2]);
				SendEvent(EBF_QWIFI_VAR,VarValueS32,pCmd[1]);
			}
			else
			{
				u32 VarValueU32=Str2Uint(pCmd[2]);
				SendEvent(EBF_QWIFI_VAR,VarValueU32,pCmd[1]);
			}
		}
		else
		{
			Debug("QCOM VAR CMD ERROR %s\n\r",pCmd[0]);
		}
	}
	else
	{
		Debug("QCOM CMD ERROR %s\n\r",pCmd[0]);
	}

Finish:	
	Q_Free(pBuf);
	return EFR_STOP;
}

//�����������ע��
static const EVENT_FUNC_ITEM gQComController[]={
{EBF_Q_COM_CMD,QCom_Cmd_EF},


{EBF_NULL,NULL}//����������һ���Դ˽�β
};

void QComControllerReg(void)
{
	ControllerRegister(gQComController,"QCom Controller");
}

