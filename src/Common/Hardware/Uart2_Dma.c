#include "Drivers.h"
#include "Q_Heap.h"
#include "EventInHandler.h"

#define US2_TX_LEN 512
#define US2_RX_LEN 512
static u8 US2_TX_BUF[US2_TX_LEN];
static u8 US2_RX_BUF[US2_RX_LEN];

static volatile u16 gSendNumA=0;
static volatile u16 gSendNumB=0;

void Com2_DmaConfig(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /* DMA1 Channel6 (triggered by USART2 Rx event) Config */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

  /* DMA1 Channel6 (triggered by USART2 Rx event) Config */
  DMA_DeInit(DMA1_Channel6);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);// ��ʼ�������ַ���൱�ڡ��ļҿ�ݡ�
  DMA_InitStructure.DMA_MemoryBaseAddr =(u32)US2_RX_BUF;// �ڴ��ַ���൱�ڼ��Ź�
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//������Ϊ������Դ����Ϊ�տ��
  DMA_InitStructure.DMA_BufferSize = US2_RX_LEN ;// ���������������Ӵ�С
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // �����ַ�������������Ӷ�Ӧ�Ŀ�ݲ���
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;// �ڴ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�����ֽڿ�ȣ��������������С�������������㣬���ǰ�����㣩
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;// �ڴ��ֽڿ�ȣ���������װ��ݵĶ���(�����������ǰ����ʽ��з�װ)
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ����ģʽ�������˾Ͳ��ڽ����ˣ�������ѭ���洢
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;// ���ȼ��ܸߣ���Ӧ��ݾ��ǼӼ�
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // �ڴ�������ͨ�ţ������ڴ浽�ڴ�
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);// �Ѳ�����ʼ������������ݹ�˾��Э��
  DMA_Cmd(DMA1_Channel6, ENABLE);// ����DMA�������ݹ�˾ǩ����ͬ����ʽ��Ч

  /* DMA1 Channel7 (triggered by USART2 Tx event) Config */
  DMA_DeInit(DMA1_Channel7);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);  // �����ַ������1�� �������Ŀ��
  DMA_InitStructure.DMA_MemoryBaseAddr =(u32)US2_TX_BUF;// �����ڴ��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;// ����Ϊ��������Ŀ�ĵأ����������ݣ�������Ƿ���
  DMA_InitStructure.DMA_BufferSize = 0;  //���ͳ���Ϊ0����δ�п����Ҫ����
  DMA_Init(DMA1_Channel7, &DMA_InitStructure);//��ʼ��

  USART_DMACmd(USART2, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);// ʹ��DMA���ڷ��ͺͽ�������
}

//���DMA ���棬����ֹDMA
static void Com2_Dma_Clr(void)
{
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel7,0);
    
    DMA_Cmd(DMA1_Channel6, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel6,US2_RX_LEN);
    DMA_Cmd(DMA1_Channel6, ENABLE);
}

//��������
u16 Com2_Send_Dma(u8 *pData, u16 Len)
{
	if(Len==0 || pData==NULL) return 0;//�жϳ����Ƿ���Ч

	while(DMA_GetCurrDataCounter(DMA1_Channel7));// ���DMA����ͨ�����Ƿ�������
	USART_ITConfig(USART2,USART_IT_TC,ENABLE);//ʹ�ܴ��ڷ�������ж�
	MemCpy(US2_TX_BUF, pData,(Len>US2_TX_LEN?US2_TX_LEN:Len));
	DMA_Cmd(DMA1_Channel7,DISABLE);//DMA��������-Ҫ�ȹ� ���÷��ͳ��� ����DMA
	DMA_SetCurrDataCounter(DMA1_Channel7,Len);// ���÷��ͳ���
	DMA_Cmd(DMA1_Channel7, ENABLE);  // ����DMA����
	return Len;
}

//��������ж�
void Com2_Tx_TC_ISR(void)
{
	DMA_Cmd(DMA1_Channel7, DISABLE); // �ر�DMA
	DMA_SetCurrDataCounter(DMA1_Channel7,0);// ������ݳ���
	USART_ITConfig(USART2,USART_IT_TC,DISABLE);//�رմ��ڷ�������ж�
}

//���տ��к���
void Com2_Rx_IDLE_ISR(void)
{
	int Len=0;
	u8 *pBuf=NULL;
	
	Len=US2_RX_LEN-DMA_GetCurrDataCounter(DMA1_Channel6);// �ܵ�buf���ȼ�ȥʣ��buf���ȣ��õ����յ����ݵĳ���
	pBuf=(u8 *)Q_Malloc(Len+2);
	MemCpy(pBuf,US2_RX_BUF,Len);
	
	Com2_Dma_Clr();
	SendEvent(EBF_RS_COM_CMD,Len,pBuf);
}

