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

//ģʽ���ư������q-ctrl�������и��棬Ϊled1 2��key����Ҫ��main����ص�led1 2���ж�5���趨key�ж�

#include "SysDefines.h"
#include "Product.h"

#define LED8_IO_NUM 7

const IO_IN_HAL_DEFINE gKeyDef={0,GPI_A,  GPin5, GPIO_Mode_IPU,EXTI_Trigger_Rising_Falling,EXTI0_IRQn,EXTI_Pio_Priority};

const IO_OUT_HAL_DEFINE gOutSingle1Def={0,GPI_A,	GPin3,		GPIO_Mode_Out_PP,FALSE};
const IO_OUT_HAL_DEFINE gOutSingle2Def={0,GPI_A,	GPin4,		GPIO_Mode_Out_PP,FALSE};
const IO_OUT_HAL_DEFINE gOutLedDef[LED8_IO_NUM]={
{0,GPI_A,	GPin15,	GPIO_Mode_Out_PP,TRUE},
{1,GPI_B,	GPin3,		GPIO_Mode_Out_PP,TRUE},//����
{2,GPI_B,	GPin5,		GPIO_Mode_Out_PP,TRUE},
{3,GPI_B,	GPin7,		GPIO_Mode_Out_PP,TRUE},
{4,GPI_B,	GPin8,		GPIO_Mode_Out_PP,TRUE},
{5,GPI_B,	GPin4,		GPIO_Mode_Out_PP,TRUE},//����
{6,GPI_B,	GPin6,		GPIO_Mode_Out_PP,TRUE}
};

static u8 gNowMode=1;

static void LED8_Set(u8 Map)
{
	u16 i;
	for(i=0;i<LED8_IO_NUM;i++) IOOUT_SetOne(&gOutLedDef[i],ReadBit(Map,i)?FALSE:TRUE);
}

static void LED8_SetModeNum(u8 Num)
{
	const u8 NumMap[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};//�����ӳ��
	LED8_Set(NumMap[Num]);

	switch(Num)
	{
		case 1:
			IOOUT_SetOne(&gOutSingle1Def,FALSE);//PA3
			IOOUT_SetOne(&gOutSingle2Def,TRUE);//PA4
			break;
		case 2:
			IOOUT_SetOne(&gOutSingle1Def,TRUE);//PA3
			IOOUT_SetOne(&gOutSingle2Def,TRUE);//PA4
			break;		
		case 3:
			IOOUT_SetOne(&gOutSingle1Def,TRUE);//PA3
			IOOUT_SetOne(&gOutSingle2Def,FALSE);//PA4
			break;		
		case 4:
			IOOUT_SetOne(&gOutSingle1Def,FALSE);//PA3
			IOOUT_SetOne(&gOutSingle2Def,FALSE);//PA4
			break;		
	}
	
} //110 1101    

//�������¼�������
static EVENT_HANDLER_RESUTL Init_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	u16 i;
	
	Debug("InitEF\n\r");

	GPIO_ConfigOne(&gKeyDef,NULL);
	GPIO_ConfigOne(NULL,&gOutSingle1Def);
	GPIO_ConfigOne(NULL,&gOutSingle2Def);
	for(i=0;i<LED8_IO_NUM;i++) GPIO_ConfigOne(NULL,&gOutLedDef[i]);

	EXTI_ConfigOne(&gKeyDef);
	NVIC_ConfigOne(&gKeyDef);

	LED8_SetModeNum(gNowDutMode);	
	
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
	
	if(++gNowDutMode>4) gNowDutMode=1;
	LED8_SetModeNum(gNowDutMode);	

	return EFR_OK;
}

//�û��Զ����¼�
static EVENT_HANDLER_RESUTL Evt1_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	Debug("New Event 1\n\r");

	return EFR_OK;
}

//�����������ע��
static const EVENT_FUNC_ITEM gHotoDiodeController[]={
{EBF_INIT,Init_EF},
{EBF_IDLE,Idle_EF},
{EBF_KEY,KeyHandler_EF},
{EBF_USER_EVT1,Evt1_EF},




{EBF_NULL,NULL}//����������һ���Դ˽�β
};

void HotoDiodeControllerReg(void)
{
	ControllerRegister(gHotoDiodeController,"HotoDiode Controller");
}

