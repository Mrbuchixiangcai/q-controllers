#include "SysDefines.h"
#include "Product.h"

#define It_Debug(format,...) //Debug

void NMI_Handler(void)
{}

void HardFault_Handler(void)
{
	Debug("HardFault!\r\n");
	while(1);
}

void HardFaultException(unsigned int * hardfault_args,unsigned int *sp) 
{ 
	unsigned int stacked_r0; 
	unsigned int stacked_r1; 
	unsigned int stacked_r2; 
	unsigned int stacked_r3; 
	unsigned int stacked_r12; 
	unsigned int stacked_lr; 
	unsigned int stacked_pc; 
	unsigned int stacked_psr; 

	stacked_r0 = ((unsigned long) hardfault_args[0]); 
	stacked_r1 = ((unsigned long) hardfault_args[1]); 
	stacked_r2 = ((unsigned long) hardfault_args[2]); 
	stacked_r3 = ((unsigned long) hardfault_args[3]); 
	stacked_r12 = ((unsigned long) hardfault_args[4]); 
	stacked_lr = ((unsigned long) hardfault_args[5]); 
	stacked_pc = ((unsigned long) hardfault_args[6]); 
	stacked_psr = ((unsigned long) hardfault_args[7]); 
	Debug ("\n\n\n\n\r################Hard fault handler]################\n\r"); 
	Debug ("R0 = 0x%08x\n\r", stacked_r0); 
	Debug ("R1 = 0x%08x\n\r", stacked_r1); 
	Debug ("R2 = 0x%08x\n\r", stacked_r2); 
	Debug ("R3 = 0x%08x\n\r", stacked_r3); 
	Debug ("R12 = 0x%08x\n\r", stacked_r12); 
	Debug ("SP = 0x%08x\n\r",(unsigned int)sp);
	Debug ("LR = 0x%08x\n\r", stacked_lr); 
	Debug ("PC = 0x%08x\n\r", stacked_pc); 
	Debug ("PSR = 0x%08x\n\r", stacked_psr); 
	Debug ("BFAR = 0x%08x\n\r", (*((volatile unsigned long *)(0xE000ED38)))); 
	Debug ("CFSR = 0x%08x\n\r", (*((volatile unsigned long *)(0xE000ED28)))); 
	Debug ("HFSR = 0x%08x\n\r", (*((volatile unsigned long *)(0xE000ED2C)))); 
	Debug ("DFSR = 0x%08x\n\r", (*((volatile unsigned long *)(0xE000ED30)))); 
	Debug ("AFSR = 0x%08x\n\r", (*((volatile unsigned long *)(0xE000ED3C)))); 

	while(1);
	return; 
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

void DebugMon_Handler(void)
{}

void SVC_Handler(void)
{}

void __inline SecTick_Handler(void)// 1s��һ��
{
	extern volatile u32 gSecFuncRtc;
	static u8 SecCount=30;

    if(gSecFuncRtc)//��ֵ�ͼ�
    {
		if(--gSecFuncRtc==0)
		{
			SetEventFlag(EBF_SEC_FUNC);
	    }	   
    }	

    if(--SecCount==0)
    {    	
		SetEventFlag(EBF_30SEC);
		SecCount=30;
    }
}

//��ʱ����������
#define KEY_MAX_PUSH_DELAY_MS 20000
#define KEY_FILTER_DELAY 50
static volatile u32 gKeyDelay[IOIN_MAX]={0};//��ֹë��
extern volatile u32 gSysTick;//ϵͳ��ʱ��
extern volatile SYS_TIMER_RCD gpSysTimerRcd[SYS_TIMER_MAX_NUM];
void SysTick_Handler(void)
{
	if((gSysTick++&SYS_SCHEDULE_PERIOD_MASK)==0)//��Ҫ���ȣ����еĶ�ʱ����Ӧ�÷ŵ�������ȥ
	{
		volatile SYS_TIMER_RCD *pSysTimer=gpSysTimerRcd;
		u16 i;	

		for(i=0;i<IOIN_MAX;i++){if(gKeyDelay[i]) gKeyDelay[i]--;}
		
#if 1
		{//ģ��rtc�жϣ�����Ӳ��rtc֧�֣���Ǩ��
			static u16 RtcMonitor=0;
			if(RtcMonitor++>=250)
			{
				extern volatile u32 gRtcMonitorCnt;
				RtcMonitor=0;
				gRtcMonitorCnt++;

				SecTick_Handler();
			}
		}
#endif	

		//ϵͳ��ʱ����ѯ
		for(i=0;i<SYS_TIMER_MAX_NUM;i++,pSysTimer++)
		{
			if(pSysTimer->Id==0) break;//Ϊ��ʡʱ�䣬������Ч��Ϣ������ǰ�� 
			if(pSysTimer->ValueTick==0) continue;
			switch(pSysTimer->Type)
			{
				case STT_AUTO:
					if(pSysTimer->Count++ >= pSysTimer->ValueTick)
					{
						if(pSysTimer->Event!=EBF_NULL) SetEventFlag(pSysTimer->Event);//�����¼��ɻ�
						pSysTimer->Count=0;
						if(pSysTimer->pCB!=NULL) 
						{
							if(pSysTimer->ExcInISR) pSysTimer->pCB();
							else AddNextVoidFunc(TRUE,pSysTimer->pCB);
						}
					}
					break;
				case STT_MANUAL:
					{
						if(pSysTimer->Count++ >= pSysTimer->ValueTick)
						{
							if(pSysTimer->Event!=EBF_NULL) SetEventFlag(pSysTimer->Event);//�����¼��ɻ�
							pSysTimer->Count=0;
							pSysTimer->ValueTick=0;
							if(pSysTimer->TaskId==0)
							{
								if(pSysTimer->pCB!=NULL)
								{
									if(pSysTimer->ExcInISR) pSysTimer->pCB();
									else AddNextVoidFunc(TRUE,pSysTimer->pCB);
								}
							}
							else 
							{
								if(pSysTimer->pTaskTimoutCB!=NULL)//taskר��
									AddNextStdFunc(TRUE,pSysTimer->pTaskTimoutCB,pSysTimer->TaskId,NULL);
							}
						}
					}
					break;
			}
		}
	}
}

void WWDG_IRQHandler(void)
{}

void PendSV_Handler(void)
{}

void TAMPER_IRQHandler(void)
{}

void RTC_IRQHandler(void)//���ڲ�һ������˾������Ը���ģ�ⷽʽ���˴����벻һ������
{
#if 0
	It_Debug("--R--%d %d %d\n\r",RTC_GetITStatus(RTC_IT_SEC),RTC_GetITStatus(RTC_IT_ALR),RTC_GetCounter());

	if(RTC_GetITStatus(RTC_IT_ALR) != RESET) //���ӱ���
	{		
	    RTC_ClearITPendingBit(RTC_IT_ALR);
	}

	if(RTC_GetITStatus(RTC_IT_SEC) != RESET)//ÿ��һ��
	{
	    RTC_ClearITPendingBit(RTC_IT_SEC);
		SecTick_Handler();	    
	}	
#endif	
}

void RTCAlarm_IRQHandler(void)
{}

void FLASH_IRQHandler(void)
{}

void RCC_IRQHandler(void)
{}

//����pin io�ж�
static __inline void PIO_EXTI_Handler(IO_IN_DEFS Io,u8 State)
{
	SendEvent(EBF_PIO_IN,(State<<16)|Io,NULL);		
}

//�������жϣ��������Ĭ������������IsPullUp����Ϊtrue
static __inline void Key_EXTI_Handler(IO_IN_DEFS Io,u8 State,bool IsPullUp)
{
	if(State==(IsPullUp?0:1))//press
	{
		gKeyDelay[Io]=Ms2Sch(KEY_MAX_PUSH_DELAY_MS);//��ʱ20��
	}
	else //release 
	{			
		if(gKeyDelay[Io] && gKeyDelay[Io]<Ms2Sch(KEY_MAX_PUSH_DELAY_MS-KEY_FILTER_DELAY))//200�������Ӳ�����
		{
			SendEvent(EBF_KEY,((KEY_MAX_PUSH_DELAY_MS-Sch2Ms(gKeyDelay[Io]))<<16)|Io,NULL);
		}
		gKeyDelay[Io]=0;
	}			
}

void EXTI0_IRQHandler(void)
{
	It_Debug("--E0--\n\r");
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{  
		EXTI_ClearITPendingBit(EXTI_Line0);	
		//PIO_EXTI_Handler(IOIN_PIO0,IOIN_ReadIoStatus(IOIN_PIO0));//���������io���������
		//Key_EXTI_Handler(IOIN_PIO0,IOIN_ReadIoStatus(IOIN_PIO0),TRUE);//�����key���������
	}
}

void EXTI1_IRQHandler(void)
{
	It_Debug("--E1--\n\r");
	
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);
		Key_EXTI_Handler(IOIN_USER_KEY,IOIN_ReadIoStatus(IOIN_USER_KEY),TRUE);//�����key���������
	}
}

void EXTI2_IRQHandler(void)
{
	It_Debug("--E2--\n\r");
	
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
		PIO_EXTI_Handler(IOIN_PIO2,IOIN_ReadIoStatus(IOIN_PIO2));
	}
}

void EXTI3_IRQHandler(void)
{
	Debug("--E3--\n\r");
	
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line3);	 		
		//PIO_EXTI_Handler(IOIN_PIO3,IOIN_ReadIoStatus(IOIN_PIO3));

#if ENABLE_IR_FUNC
		IrPulseIn_ISR();		
#endif		
	}
}

void EXTI4_IRQHandler(void)
{
	It_Debug("--E4--\n\r");
	
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{		
		EXTI_ClearITPendingBit(EXTI_Line4);
		PIO_EXTI_Handler(IOIN_PIO4,IOIN_ReadIoStatus(IOIN_PIO4));
	}
}

void DMA1_Channel1_IRQHandler(void)
{}

void DMA1_Channel2_IRQHandler(void)
{}

void DMA1_Channel3_IRQHandler(void)
{}

void DMA1_Channel4_IRQHandler(void)
{}

void DMA1_Channel5_IRQHandler(void)
{}

void DMA1_Channel6_IRQHandler(void)
{}

void DMA1_Channel7_IRQHandler(void)
{}

void ADC1_2_IRQHandler(void)
{}

void USB_HP_CAN1_TX_IRQHandler(void)
{}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
}

void CAN1_RX1_IRQHandler(void)
{}

void CAN1_SCE_IRQHandler(void)
{}

void EXTI9_5_IRQHandler(void)
{	
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line5);It_Debug("--E5--\n\r");
		PIO_EXTI_Handler(IOIN_PIO5,IOIN_ReadIoStatus(IOIN_PIO5));
	}

	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line6);It_Debug("--E6--\n\r");
		PIO_EXTI_Handler(IOIN_PIO6,IOIN_ReadIoStatus(IOIN_PIO6));
	}
	
	if(EXTI_GetITStatus(EXTI_Line7) != RESET) 
	{
		EXTI_ClearITPendingBit(EXTI_Line7);	 It_Debug("--E7--\n\r");
		PIO_EXTI_Handler(IOIN_PIO7,IOIN_ReadIoStatus(IOIN_PIO7));
	}

	if(EXTI_GetITStatus(EXTI_Line8) != RESET) 
	{
		EXTI_ClearITPendingBit(EXTI_Line8); It_Debug("--E8--\n\r");
		//if(IOIN_ReadIoStatus(IOIN_WRF_DRV_INT)==WRF_DRV_INT_LEVEL)
		{
			//WRF_DRV.pWRF_ISR();
			Debug("pa8:%u\n\r",IOIN_ReadIoStatus(IOIN_WRF_DRV_INT));
			Sx1276_ISR();
		}
	}

	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line9);It_Debug("--E9--\n\r");
	}
}

//void TIM1_BRK_IRQHandler(void)
//{}

//void TIM1_UP_IRQHandler(void) //pwmռ�ã��û�������
//{}

//void TIM1_TRG_COM_IRQHandler(void)
//{}

//void TIM1_CC_IRQHandler(void)
//{}

#if !ENABLE_IR_FUNC //���������Ҫ����һ����ʱ��
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		


	}
}
#endif

//void TIM3_IRQHandler(void) //pwmռ�ã��û�������
//{}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);		


	}
}

void I2C1_EV_IRQHandler(void)
{}

void I2C1_ER_IRQHandler(void)
{}

void I2C2_EV_IRQHandler(void)
{}

void I2C2_ER_IRQHandler(void)
{}

void SPI1_IRQHandler(void)
{}

void SPI2_IRQHandler(void)
{}

#define CMD_BUF_LEN 256
void CmdParse(u8 Byte)
{
	static u8 CmdBuf[CMD_BUF_LEN+2];
	static u16 CmdLen=0;

	if(CheckEventFinished(EBF_SYS_CMD)==FALSE) return;//���ǰ��İ���û�д�����
	
	if(Byte== '\r')//�س�
	{
		CmdBuf[CmdLen]=0;
		SendEvent(EBF_SYS_CMD,CmdLen,CmdBuf);
		CmdLen=0;
	}
	else if(Byte=='\b')//��ɾ
	{
		if(CmdLen>0)
		{
			CmdBuf[--CmdLen]=0;
			USART_SendData(USART1,'\b');//����
			while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//�ȴ����ͽ���
			USART_SendData(USART1,' ');//����
			while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//�ȴ����ͽ���
			USART_SendData(USART1,'\b');//����
			while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//�ȴ����ͽ���
		}
	}
	else
	{
		if(CmdLen<CMD_BUF_LEN) CmdBuf[CmdLen++]=Byte;
		USART_SendData(USART1, Byte);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//�ȴ����ͽ���
	}
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//����жϱ�־.
		CmdParse(USART_ReceiveData(USART1));
	}
}

void USART2_IRQHandler(void)
{}


#define US3_BUF_LEN 64
void USART3_IRQHandler(void)
{
	static u8 Us3RecvBuf[US3_BUF_LEN+2];
	static u16 gUs3RecvLen=0;
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{ 
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);//����жϱ�־.

		if(gUs3RecvLen<US3_BUF_LEN) Us3RecvBuf[gUs3RecvLen++]=USART_ReceiveData(USART3);
		if(Us3RecvBuf[gUs3RecvLen-1]=='\r') goto UsRecvFinish;
	}

	if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET)
	{
		USART3->SR;USART3->DR;//��idle��־λ
		
		if(Us3RecvBuf[0]=='#')//ָ��
		{
			if(Us3RecvBuf[gUs3RecvLen-1]!='\r') return;
		}
		else
		{
			gUs3RecvLen=0;
			return;
		}

UsRecvFinish:
		{
			char *pData=Q_Malloc(gUs3RecvLen+2);//�����ʱ����Զ����㣬����ĩβ���ø�0�ˣ���EventMemoryFree���ͷ�
			MemCpy(pData,Us3RecvBuf,gUs3RecvLen);
			if(pData[gUs3RecvLen-1]=='\r') pData[--gUs3RecvLen]=0;
			SendEvent(EBF_Q_COM_CMD,gUs3RecvLen,(void *)pData);
			gUs3RecvLen=0;
		}
	}
}

void EXTI15_10_IRQHandler(void)
{
	It_Debug("--T15_10--\n\r");

	if(EXTI_GetITStatus(EXTI_Line10) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line10);		
	}
	
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line11);
		IrPulseIn_ISR();
	}
	
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
	
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line13);
	}

	if(EXTI_GetITStatus(EXTI_Line14) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line14);
	}

	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line15);		
	}
}

void USBWakeUp_IRQHandler(void)
{}



/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
