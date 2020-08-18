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

//�������¼�������
static EVENT_HANDLER_RESUTL Init_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	Debug("InitEF\n\r");
	Sx1276_Init(1234,100);

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
	//SendEvent(EBF_USER_EVT1,0,NULL);

	Sx1276_TestPacket();
	
	return EFR_OK;
}

//�û��Զ����¼�
static EVENT_HANDLER_RESUTL Evt1_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	Debug("New Event 1\n\r");

	return EFR_OK;
}

//�����������ע��
static const EVENT_FUNC_ITEM gLoraController[]={
{EBF_INIT,Init_EF},
{EBF_IDLE,Idle_EF},
{EBF_KEY,KeyHandler_EF},
{EBF_USER_EVT1,Evt1_EF},




{EBF_NULL,NULL}//����������һ���Դ˽�β
};

void LoraControllerReg(void)
{
	ControllerRegister(gLoraController,"Lora Controller");
}

