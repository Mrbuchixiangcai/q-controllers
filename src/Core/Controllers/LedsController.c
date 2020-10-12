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

���ļ�����Ŀ�������Ҫ��������led��ģʽ
*/
//------------------------------------------------------------------//


#include "SysDefines.h"
#include "Product.h"
#include "LedsMode.h"

//�������¼�������
static EVENT_HANDLER_RESUTL Init_EF(EVENT_BIT_FLAG Event,int a,void *p)
{
	//ָʾ��
	LedSet(IOOUT_LED1,0);
	LedSet(IOOUT_LED2,0);
	
	return EFR_OK;
}

static LEDS_MODE gLedsMode[2]={LM_OFF,LM_OFF};
static u32 gLedTimCnt[2]={0,0};//��ʱ��������ص�Ƶ������

//��ʱ�����ڹص�
static void TimerExpLed(u32 Io,void *p)
{
	u32 TimDly=0;
	u16 Cnt=HBit16(Io);
	Io=LBit16(Io);
	
	//Debug("LedTimer Led%u Mode%u State %s %u\n\r",Io,gLedsMode[Io],p==NULL?"NULL":"TRUE",GetSysStartMs());
	
	switch(gLedsMode[Io])
	{
		case LM_OFF:	LedSet(IOOUT_LED1+Io,0);break;
		case LM_ON:	LedSet(IOOUT_LED1+Io,1);break; 

		case LM_ON_500MS:
		case LM_ON_1S:
		case LM_ON_2S:
			gLedsMode[Io]=LM_OFF;LedSet(IOOUT_LED1+Io,0);
			break;

		case LM_FLASH_200MS_L2S:
		case LM_FLASH_200MS:	TimDly=200;goto SetLoop;
		case LM_FLASH_500MS:	TimDly=500;goto SetLoop;
		case LM_FLASH_2S:			TimDly=2000;goto SetLoop;
	}	

	if(0)
	{
SetLoop:
		LedSet(IOOUT_LED1+Io,p==NULL?0:1);
		if(GetSysStartMs()-gLedTimCnt[Io] >= TimDly)
		{
			if(Cnt==0) return;
			else if(Cnt==0xffff);
			else Cnt--;

			gLedTimCnt[Io]=GetSysStartMs();
			AddOnceMsFunc(TimDly,TimerExpLed,(Cnt<<16)|Io,p==NULL?(void *)-1:NULL);
		}
	}
}

//�û��Զ����¼�
static EVENT_HANDLER_RESUTL Led_ChangeMode_EF(EVENT_BIT_FLAG Event,LEDS_MODE Mode,void *p)
{
	u32 TimDly=0;
	u16 Cnt=0xffff;
	u8 Io=HBit4(Mode);
	Mode=LBit4(Mode);

	//Debug("Set Led %u Mode %u\n\r",Io,Mode);

	switch(Mode)
	{
		case LM_OFF:	gLedsMode[Io]=LM_OFF;LedSet(IOOUT_LED1+Io,0);break;
		case LM_ON:	gLedsMode[Io]=LM_ON;LedSet(IOOUT_LED1+Io,1);break;
		
		case LM_ON_500MS:	TimDly=500;goto SetLoop1;
		case LM_ON_1S:		TimDly=1000;goto SetLoop1;
		case LM_ON_2S:		TimDly=2000;goto SetLoop1;

		case LM_FLASH_200MS:	TimDly=200;goto SetLoop2;
		case LM_FLASH_500MS:	TimDly=500;goto SetLoop2;
		case LM_FLASH_2S:			TimDly=2000;goto SetLoop2;	
		case LM_FLASH_200MS_L2S: Cnt=10;TimDly=200;goto SetLoop2;
	}

	if(0)
	{
SetLoop1:
		gLedsMode[Io]=LM_OFF;
		LedSet(IOOUT_LED1+Io,1);
		AddOnceMsFunc(TimDly,TimerExpLed,Io,NULL);
	}

	if(0)
	{
SetLoop2:
		gLedsMode[Io]=Mode;
		gLedTimCnt[Io]=GetSysStartMs();			
		LedSet(IOOUT_LED1+Io,1);
		AddOnceMsFunc(TimDly,TimerExpLed,(Cnt<<16)|Io,NULL);
	}	

	return EFR_OK;
}

//�����������ע��
static const EVENT_FUNC_ITEM gLedsController[]={
{EBF_INIT,Init_EF},
{EBF_LED_MODE,Led_ChangeMode_EF},


{EBF_NULL,NULL}//����������һ���Դ˽�β
};

void LedsControllerReg(void)
{
	ControllerRegister(gLedsController,"Leds Controller");
}

