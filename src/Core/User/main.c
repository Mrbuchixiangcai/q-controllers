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

���ļ�����Ŀ�����c�����ļ�
*/
//------------------------------------------------------------------//

#include "SysDefines.h"
#include "Product.h"
#include "Controllers.h"

u32 gWdgTimer=0;//ϵͳ��ʱ�����
u32 gTimingFuncTimer=0;//ϵͳ��ʱ�����

//Ӳ����ʼ��
void HardwareInit(void)
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//ʹ��swd����
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1,ENABLE);//TIM1_CH1N���õ�PA7

	IoDefinesInit();//io�ڳ�ʼ��

	SpiFlsInit();

	Tim2_Init();
	Tim4_Init();
}

void EventStateHandler(void)
{
	EVENT_BIT_FLAG Event=EBF_NULL;
	s32 S32Param=0;
	void *pParam=NULL;
	
	CleanAllEvent();//��ѭ��֮ǰ��������¼���־λ

	//������init
	ControllerEvtPost(EBF_INIT,0,NULL);
	
	while(1)
	{
		pParam=WaitEvent(&Event,&S32Param);

		if(Event>=EBF_MAX) continue;//����
		
		//ϵͳ�����¼�����
 		if(Event==EBF_NEXT_QUICK_FUNC) NextFuncExcute(TRUE);

		//�������ص�����
		switch(Event)//��ѯ����־λ�������¼�ʱ������ִ�д������
		{
			case EBF_NEXT_QUICK_FUNC:
			case EBF_SEC_FUNC:
			case EBF_SYS_CMD:
			case EBF_NEXT_LOOP_FUNC:
				break;

			default:
				ControllerEvtPost(Event,S32Param,pParam);//�¼��ַ���������
		}

		//ϵͳ�ڲ��¼�����
		switch(Event)//��ѯ����־λ�������¼�ʱ������ִ�д������
		{
			case EBF_SEC_FUNC:
				SecFuncExpired();
				break;
			case EBF_Q_COM_CMD:
				if(IsHeapRam(pParam)) Q_Free(pParam);//�ͷ�������ڴ�				
				break;
			case EBF_SYS_CMD:
				{
					extern bool SysCmdHandler(u16 Len,const char *pStr,char *pOutStream);
					SysCmdHandler(S32Param,pParam,NULL);
				}
				break;				
			case EBF_NEXT_LOOP_FUNC:
				NextFuncExcute(FALSE);
				break;
		}
	}
}

int main(void)
{	
	SysTick_Init();//���Ķ���	
	SystemInit();//ϵͳʱ�ӳ�ʼ��
	COM1_Init(115200);//���Դ���
	COM3_Init(74880);//�û����ڣ���q-wifi

	DebugCol("\n\n\n\n\rQ-Controller");Debug(" ");
	DebugCol("%u\n\r",GetHwID(NULL));
	Debug("Release %u\n\r",RELEASE_DAY);

    RFS_Init();//���ݴ洢��ʼ��
	Adc1_Rand_Init(0);//adc pa0 pa1 pa2 pa3��ÿ��bit����һ·

	NextLoopFuncInit();//sys time ���õ�
	SysTimerInit();//����ŵ��ȽϿ�ǰ��λ�ã���ֹ��Щ��ʼ������Ҫ�õ�Timing����
	gWdgTimer=AddSysTimer(STT_AUTO,WDG_CB_MS,EBF_NULL,IWDG_PeriodCB,TRUE);
	gTimingFuncTimer=AddSysTimer(STT_MANUAL,0,EBF_NULL,MsFuncExpired,FALSE);
	
	MsFuncInit();//����ŵ��ȽϿ�ǰ��λ�ã���ֹ��Щ��ʼ������Ҫ�õ�Timing����
	SecFuncInit();//�뼶��ʱ��
	HardwareInit();//�ײ�Ӳ����ʼ��

	//ָʾ��
	LedSet(IOOUT_LED1,1);//yellow
	LedSet(IOOUT_LED2,1);//blue
	DelayMs(200);
	LedSet(IOOUT_LED1,0);//yellow
	LedSet(IOOUT_LED2,0);//blue
	DelayMs(200);

	//���������ע�ᣬ��ǰ���¼�����ʱ����ִ��
	QComControllerReg();
	//QWifiControllerReg();
	//NewControllerReg();
	//TestControllerReg();
	//CollControllerReg();
	LoraControllerReg();

#if 1//!ADMIN_DEBUG
	IWDG_Configuration();//�������Ź�
#endif

	//���ж�
	//IOIN_OpenExti(IOIN_IR_IN);
	IOIN_OpenExti(IOIN_USER_KEY);//ռ���ⲿ�ж�1
	IOIN_OpenExti(IOIN_PIO0);
	
	EventStateHandler();

	while(1);
}


/*******************END OF FILE****/

