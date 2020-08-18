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

���ļ�����Ŀ�������Ҫ������������ʾ����ʾ�˿������԰����¼��ȼ��¼���
����ʾ����
*/
//------------------------------------------------------------------//


#include "SysDefines.h"
#include "Product.h"

static u32 gLight=0;
static u32 gTemp=0;

//��ʱ���ص�
void AdcLoop_CB(int cnt,int a,void *p)
{
	gLight=Adc1_GetValByAio(0)*1000/4096;
	gTemp=Adc1_GetValByAio(1)*3300/4096;

	Debug("ReadAdc %u %u\n\r",gLight,gTemp);

	QCom_SetVarValue("QSXXTEMP",gTemp,FALSE);//�ȷ���һ��ֵ���ذ��ٷ��ڶ���ֵ
}

//�������¼�������
static EVENT_HANDLER_RESUTL Init_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	Debug("Start Data Collection\n\r");
	Adc1_Rand_Init(Bit(0)|Bit(1));//adc pa0 pa1 pa2 pa3��ÿ��bit����һ·
	AddMsFunc((u32)-1,5000,AdcLoop_CB,0,NULL);
	
	return EFR_OK;
}

//ϵͳwhile(1)�յ�ʱѭ�����õ��¼�
static EVENT_HANDLER_RESUTL Idle_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	return EFR_OK;
}

//���������²��ɿ�����¼�
static EVENT_HANDLER_RESUTL KeyHandler_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	u16 KeyIo=a&0xffff;
	u16 Ms=a>>16;

	Debug("New Key%u %umS\n\r",KeyIo,Ms);

	return EFR_OK;
}

//��������ָ��յ��ظ�
static EVENT_HANDLER_RESUTL QWIFI_Varible_Res_EF(EVENT_BIT_FLAG Event,int Value,const char *pVarTag)
{
	//Debug("Event %u call %s [%s]=%d\n\r",Event,__FUNCTION__,pVarTag,Value);
	//Debug("LastCmd:%s\n\r",QCom_GetLastCmd());

	if(Event==EBF_QWIFI_SET_VAR_RET && strcmp(pVarTag,"QSXXTEMP")==0)//���ͳɹ��ٷ��ڶ���ֵ
	{
		QCom_SetVarValue("QSXXLIGH",gLight,FALSE);	
	}
	
	if(Event==EBF_QWIFI_SET_VAR_RET && strcmp(pVarTag,"QSXXLIGH")==0)
	{
		Debug("Var Set Success.\n\r");
	}

	return EFR_OK;
}


//�����������ע��
static const EVENT_FUNC_ITEM gCollController[]={
{EBF_INIT,Init_EF},
{EBF_IDLE,Idle_EF},
{EBF_KEY,KeyHandler_EF},
{EBF_QWIFI_SET_VAR_RET,QWIFI_Varible_Res_EF},



{EBF_NULL,NULL}//����������һ���Դ˽�β
};

void CollControllerReg(void)
{
	ControllerRegister(gCollController,"Data Collection Controller");
}

