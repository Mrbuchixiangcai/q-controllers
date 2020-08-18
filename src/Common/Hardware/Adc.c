//--------------------------Q Controllers---------------------------//
/*
Q-Ctrl��һ�׻����¼��Ŀ�������ܣ������MCV��ܣ�Q-Ctrl����Э��
�洢��Data�������������IO������������Controller�����ߵ��߼�����
���DIC��ܡ�
Q-Ctrl����stm32�д��������������ֱ�ӵ��ã�Ҳ������ֲ��������Ƭ��ƽ̨��
�������ϵͳ��֧�֣������ؿ�������̹��������£��ɴ���������Ҫ����ϵͳ
���ܴ���ĸ���ҵ��
By Karlno ����Ƽ�

���ļ���װ��adc�������ɱ���������������stm32��Ŀ�����ٴ��뿪����
*/
//------------------------------------------------------------------//

#include "Drivers.h" 

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define ADC_SAMPLE_NUM (16+2)
#define ADC_SAMPLE_OFFSET 4 //����ADC_SAMPLE_NUM-2����

static volatile u16 gAdcChanVals[6*ADC_SAMPLE_NUM]; //�������ADCת�������Ҳ��DMA��Ŀ���ַ
static u8 gAdcChanNum=0;

#define WAVER_AIO_NUM 4
static u8 gAdcIoChMap[WAVER_AIO_NUM+2]={0,0,0,0};

static void Adc1_DmaConfig(volatile u16 *pBuf,u8 SampleNum,u8 ChanNum)
{
    DMA_InitTypeDef DMA;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    
    DMA_DeInit(DMA1_Channel1);
    
    DMA.DMA_PeripheralBaseAddr  = ADC1_DR_Address;     //DMA��Ӧ���������ַ
    DMA.DMA_MemoryBaseAddr      = (u32)pBuf;       //�ڴ�洢����ַ
    DMA.DMA_DIR                 = DMA_DIR_PeripheralSRC;//DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�
    DMA.DMA_M2M                 = DMA_M2M_Disable;      //M2Mģʽ����
    DMA.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_HalfWord;//�����������ݿ��Ϊ16λ
    DMA.DMA_MemoryDataSize      = DMA_MemoryDataSize_HalfWord;  //DMA�������ݳߴ磬HalfWord����Ϊ16λ    
    DMA.DMA_BufferSize          = SampleNum*ChanNum;                       //DMA�����СCH_NUM��
    DMA.DMA_MemoryInc           = DMA_MemoryInc_Enable;         //����һ�����ݺ�Ŀ���ڴ��ַ����
    DMA.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    //����һ�����ݺ��豸��ַ��ֹ����
    DMA.DMA_Mode                = DMA_Mode_Circular;            //ת��ģʽ��ѭ������ģʽ��
    DMA.DMA_Priority            = DMA_Priority_High;            //DMA���ȼ���
    DMA_Init(DMA1_Channel1,&DMA); 
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

//��bit����adc
//���ظ����ã������һ�ε���Ϊ׼
void Adc1_Rand_Init(u8 UseAio)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;  
	u8 UseAdcIoNum=0;

	if(UseAio)
	{
		Debug("ADC Init:");
		if(ReadBit(UseAio,0)) Debug("0 ");
		if(ReadBit(UseAio,1)) Debug("1 ");
		if(ReadBit(UseAio,2)) Debug("2 ");
		if(ReadBit(UseAio,3)) Debug("3 ");
		Debug("\n\r");
	}
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_ADC1, ENABLE);

	if(ReadBit(UseAio,0))
	{
	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);
	    UseAdcIoNum++;
	}

	if(ReadBit(UseAio,1))
	{
	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);
	    UseAdcIoNum++;
	}

	if(ReadBit(UseAio,2))
	{
	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);
	    UseAdcIoNum++;
	}

	if(ReadBit(UseAio,3))
	{
	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);
	    UseAdcIoNum++;
	}	

	//PB0����Ϊģ��ͨ��8                   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	gAdcChanNum=2;//Ĭ�ϼ���pb0��cpu temp
	
	Adc1_DmaConfig(gAdcChanVals,ADC_SAMPLE_NUM,gAdcChanNum+UseAdcIoNum);//��ʼ��DMA
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	ADC_DeInit(ADC1);
 
    //������ADC1�ļĴ�������
  	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADģʽѡΪ����ģʽ
  	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//�Զ�ɨ��ģʽʹ��
  	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��ģʽʹ��
  	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//û���жϴ���ת��
  	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�������Ҷ���
  	ADC_InitStructure.ADC_NbrOfChannel = gAdcChanNum+UseAdcIoNum;//��ʼ��ADCͨ������1
  	ADC_Init(ADC1, &ADC_InitStructure);//����ADC1�豸

  	ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����

  	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);  	//PB0��ӦADC1ͨ����8
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 2, ADC_SampleTime_239Cycles5);	//Vref 1.2v ��ӦADC1ͨ����17
	if(ReadBit(UseAio,0)) 
	{
	    gAdcChanNum++;
		gAdcIoChMap[0]=gAdcChanNum;
		ADC_RegularChannelConfig(ADC1, ADC_Channel_0,gAdcChanNum, ADC_SampleTime_239Cycles5);  	//PA0��ӦADC1ͨ����0
	}
	if(ReadBit(UseAio,1))
	{
		gAdcChanNum++;
		gAdcIoChMap[1]=gAdcChanNum;
		ADC_RegularChannelConfig(ADC1, ADC_Channel_1,gAdcChanNum, ADC_SampleTime_239Cycles5);  	//PA1��ӦADC1ͨ����1
	}
	if(ReadBit(UseAio,2))
	{
		gAdcChanNum++;
		gAdcIoChMap[2]=gAdcChanNum;
		ADC_RegularChannelConfig(ADC1, ADC_Channel_2,gAdcChanNum, ADC_SampleTime_239Cycles5);  	//PA2��ӦADC1ͨ����2
	}
	if(ReadBit(UseAio,3))
	{
		gAdcChanNum++;
		gAdcIoChMap[3]=gAdcChanNum;
		ADC_RegularChannelConfig(ADC1, ADC_Channel_3,gAdcChanNum, ADC_SampleTime_239Cycles5);  	//PA3��ӦADC1ͨ����3
	}
	
	ADC_DMACmd(ADC1,ENABLE);//����adc��DMA
  	ADC_Cmd(ADC1, ENABLE);//ʹ��ADC1  	
  	
  	ADC_ResetCalibration(ADC1);//��λADC1�ļĴ���      	
  	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ���λ���� 
  	
  	ADC_StartCalibration(ADC1);//��ʼADC1У׼  	
  	while(ADC_GetCalibrationStatus(ADC1));//�ȴ�ADC1У׼���� 	

  	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ADC1ת��  	
}

//��dma�ж�ȡchannal�Ĳ���ֵ
//channal��1��ʼ��channal��Ÿ��ݳ�ʼ��˳������Ĭ��pb0
//����ֵ0-4095
u16 Adc1_GetVal(u8 Chan)
{
	u32 Sum=0;
	u16 Min=0xffff;
	u16 Max=0;
	u16 Val,i;

	if(Chan==0 || Chan>gAdcChanNum) return 0;

	Chan--;

	for(i=0;i<ADC_SAMPLE_NUM;i++)
	{
		Val=gAdcChanVals[i*gAdcChanNum+Chan];
		
		//Debug("Adc %u\n\r",Val);
		
		Sum+=Val;
		if(Val<Min) Min=Val;
		if(Val>Max) Max=Val;
	}	
	//Debug("\n\r");
	
	Sum-=Min;
	Sum-=Max;

	return Sum>>ADC_SAMPLE_OFFSET;//����ADC_SAMPLE_NUM-2����
}

//AioID 0-3
//����ֵ0-4095
u16 Adc1_GetValByAio(u8 AioID)
{
	if(AioID>=WAVER_AIO_NUM) return 0;
	return Adc1_GetVal(gAdcIoChMap[AioID]);
}

//��channal 1��ȡ���ݣ�channal��Ÿ��ݳ�ʼ��˳������Ĭ��pb0
u32 GetAdcRand(void)
{
	u32 Num=0;
	u8 i;
	
	for(i=0;i<8;i++)
	{
		Num|=((gAdcChanVals[i*gAdcChanNum]&0x0f)<<(i*4));
	}
	
	return Num;
}

//����0-4096�Ĳ���ֵ
//�����0.01���϶�Ϊ��λ���¶�ֵ
static u16 Vat2Temp(u16 Vat)
{
	u32 Temp=Vat;
	
	// (14300-val*33000/4096)/43 *100 +2500
	Temp*=33000;
	Temp/=4096;
	Temp=14300-Temp;
	Temp*=100;
	Temp/=43;
	Temp+=2500;

	return Temp;
}

//�����0.01���϶�Ϊ��λ���¶�ֵ
u16 GetCpuTemp(void)
{
	return Vat2Temp(Adc1_GetVal(ADC_CPU_CH));
}





