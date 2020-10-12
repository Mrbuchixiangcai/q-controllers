#include "Drivers.h"
#include "Si4432.h"
//#include "SecretRun.h"

#define RSSI_DEBUG_MODE 0//�źż��ģʽ���������豸��������������

static u32 gMySiRxAddr=0;

#define Select_Si4432()     IOOUT_SetIoStatus(IOOUT_WRF_DRV_CS, FALSE)
#define NotSelect_Si4432()    IOOUT_SetIoStatus(IOOUT_WRF_DRV_CS, TRUE)
#if PRODUCT_IS_JUMPER || PRODUCT_IS_LIFE1
#define Si4432_Spi_Init() SPI1_Init()
#define Si4432_Spi_Read() SPI_ReadByte(SPI1)
#define	Si4432_Spi_Write(x) SPI_WriteByte(SPI1,x)
#elif PRODUCT_IS_WAVER
#define Si4432_Spi_Init() SPI2_Init()
#define Si4432_Spi_Read() SPI_ReadByte(SPI2)
#define	Si4432_Spi_Write(x) SPI_WriteByte(SPI2,x)
#endif
#define ANT_TX1_RX0()	Si4432_WriteReg(R_IO_CONF,0x01)		// ����״̬�����߿��ض���
#define ANT_TX0_RX1()	Si4432_WriteReg(R_IO_CONF,0x02)		// ����״̬�����߿��ض���
#define ANT_TX0_RX0()	Si4432_WriteReg(R_IO_CONF,0x00)         // �Ƿ��䣬����״̬�����߿��ض���
#define WSDN_0()  IOOUT_SetIoStatus(IOOUT_WRF_DRV_SDN, FALSE)
#define WSDN_1()  IOOUT_SetIoStatus(IOOUT_WRF_DRV_SDN, TRUE)
#define NIRQ() 	IOIN_ReadIoStatus(IOIN_WRF_DRV_INT)
#define READREG        0x7f  	//���Ĵ���ָ��
#define WRITEREG       0x80 	//д�Ĵ���ָ��

//�Ĵ���������λ
#define R_DEVICE_TYPE 0x00

#define R_VERSION 0x01

#define R_STAUS 0x02
#define B_STAUS_FFOVFL 0x80 //fifo����
#define B_STAUS_FFUNFL 0x40 //fifo����
#define B_STAUS_RXFFEM 0x20 //rx fifo��
#define B_STAUS_HEADERR 0x10 //֡ͷ����
#define B_STAUS_FREQERR 0x08 //Ƶ�ʴ���
#define B_STAUS_LOCKDET 0x04 //�ϳ����������
#define B_STAUS_CPS 0x03 //оƬ��Դ״̬
#define V_STAUS_CPS_SUSPEND 0x00
#define V_STAUS_CPS_RECV 0x01
#define V_STATUS_CPS_SEND 0x02

#define R_IT_STAUS_1 0x03
#define B_IT1_IFFERR 0x80	//fifo��������
#define B_IT1_ITXFFAFULL 0x40 //tx fifo ������
#define B_IT1_ITXFFAEM 0x20 //tx fifo ������
#define B_IT1_IRXFFAFULL 0x10 //rx fifo ������
#define B_IT1_IEXT 0x08 //�ⲿ�ж�
#define B_IT1_IPKSENT 0x04 //�������ж�
#define B_IT1_IPKVALID 0x02 //�������ж�
#define B_IT1_ICRCERROR 0x01 //crc����

#define R_IT_STAUS_2 0x04
#define B_IT2_ISWDET 0x80 //��⵽ͬ����
#define B_IT2_IPREAVAL 0x40 //��⵽��Ч������
#define B_IT2_IPREAINVAL 0x20 //��⵽��Ч������
#define B_IT2_IRSSI 0x10 //�źŵȼ������ż�
#define B_IT2_IWUT 0x08 //��ʱ����
#define B_IT2_ILDB 0x04 //���Ƿѹ
#define B_IT2_ICHIPRDY 0x02 //оƬԤ��
#define B_IT2_IPOR 0x01 //�ϵ縴λ

#define R_IT_ENABLE_1 0x05 //�ж�ʹ��1
#define R_IT_ENABLE_2 0x06 //�ж�ʹ��2

#define R_RUN_MODE_1 0x07 //����ģʽ����1
#define B_MODE_SWRET 0x80
#define B_MODE_READY 0x01
#define B_MODE_RECV 0x05
#define B_MODE_SEND 0x09

#define R_RUN_MODE_2 0x08 //����ģʽ����2
#define B_RM2_CLEAN_RX_FIFO 0x02
#define B_RM2_CLENA_TX_FIFO 0x01

#define R_XTAL_P 0x09 //30mhz�����ص���
#define R_MCU_OUT_CLOCK 0x0a//���������ʱ��
#define R_GPIO0_CONF 0x0B //gpio����0
#define R_GPIO1_CONF 0x0C //gpio����1
#define R_GPIO2_CONF 0x0D //gpio����2
#define R_IO_CONF 0x0E //io�˿�����
#define R_ADC_CONF 0x0F //adc����
#define R_TEMP_SEN_OFFSET 0x12 //�¶ȴ�����У��
#define R_WUT_1 0x14  //���Ѷ�ʱ��1
#define R_WUT_2 0x15  //���Ѷ�ʱ��2
#define R_WUT_3 0x16  //���Ѷ�ʱ��3
#define R_WUT_VAL_1 0x17  //���Ѷ�ʱ��ֵ1
#define R_WUT_VAL_2 0x18  //���Ѷ�ʱ��ֵ2
#define R_BAT_THRESHOLD 0x1A  //���Ƿѹ����ż�
#define R_BAT_LEVEL 0x1B  //��ص�ѹ����
#define R_FILTER_BAUD_WITDH 0x1C  //�˲�������
#define R_AFC_LOOP_GEAR_OVER 0x1D //afc
#define R_RSSI 0x26  //�ź�ָʾ��
#define R_RSSITH 0x27  //�ź�����

#define R_DATA_RW_CTRL 0x30  //���ݴ�ȡ����
#define B_DRC_ENPACRX  0x80 //ʹ�ܽ������ݰ�����
#define B_DRC_LSBFRST 0x40 //LSB����
#define B_DRC_CRCDONLY 0x20 //���������ݽ���CRCУ��
#define B_DRC_ENPACTX 0x80 //ʹ�����ݰ����䴦��
#define B_DRC_ENCRC 0x40 //CRCʹ��
#define B_DRC_CRC 0x03 //CRC����ѡ��
#define V_DRC_CRC_16 0x01 //CRC16У��

#define R_FHC_1 0x32 //֡ͷ����1
#define B_FHC_BOARDCAST_EN 0xf0 //�㲥����λ
#define V_FHC_NO_BOARDCAST 0x00 //���ֽ����ڹ㲥
#define V_FHC_BYTE_BOARDCAST 0x01 // 1�ֽ����ڹ㲥У��
#define B_FHC_BYTE_NUM 0x0f 	//�ֽ�������λ
#define V_FHC_NO_BYTE_CHK 0x00 //���ֽ�����֡ͷ
#define V_FHC_BYTE_CHK 0x01 // 1�ֽ�����֡ͷ

#define R_FHC_2 0x33 //֡ͷ����2
#define B_FHC_HDLEN 0x70//����֡ͷ����
#define B_FHC_FIXPKLEN 0x08//�̶�֡ͷ�����Ƿ�������ݰ�����
#define B_FHC_SYNCLEN 0x06//ͬ���ֳ���
#define B_FHC_PREALEN 0x01//ǰ����ʹ��

#define R_PREALEN_LEN 0x34 //ǰ���볤��

#define R_PREATH_CTRL1 0x35 //ǰ����������1
#define B_PREATH_BYTH_HALF 0xf8 //������ڼ䴦����ֽ���

#define R_SYNC_CHAR_3 0x36 //ͬ����3
#define R_SYNC_CHAR_2 0x37 //ͬ����2
#define R_SYNC_CHAR_1 0x38 //ͬ����1
#define R_SYNC_CHAR_0 0x39 //ͬ����0
#define R_TX_FH3 0x3a //����֡ͷ3
#define R_TX_FH2 0x3b //����֡ͷ2
#define R_TX_FH1 0x3c //����֡ͷ1
#define R_TX_FH0 0x3d //����֡ͷ0
#define R_TX_PKTLEN 0x3e //�������ݰ�����
#define R_FH_CHK3 0x3f //������ͷ3���
#define R_FH_CHK2 0x40 //������ͷ2���
#define R_FH_CHK1 0x41 //������ͷ1���
#define R_FH_CHK0 0x42 //������ͷ0���
#define R_FH_EN3 0x43 //֡ͷʹ��3
#define R_FH_EN2 0x44 //֡ͷʹ��2
#define R_FH_EN1 0x45 //֡ͷʹ��1
#define R_FH_EN0 0x46 //֡ͷʹ��0
#define R_FH_RX3 0x47 //���յ���֡ͷ3
#define R_FH_RX2 0x48 //���յ���֡ͷ2
#define R_FH_RX1 0x49 //���յ���֡ͷ1
#define R_FH_RX0 0x4a //���յ���֡ͷ0
#define R_RX_PKTLEN 0x4b //���յ������ݰ�����
#define R_TX_POWER 0x6d //���书��
#define R_TX_BAUD1 0x6e//���䲨����1
#define R_TX_BAUD0 0x6f//���䲨����0
#define R_MODULATION_CTRL1 0x70//���ƿ���ģʽ1
#define R_MODULATION_CTRL2 0x71//���ƿ���ģʽ2
#define R_FREQ_DEVIATATION 0x72//Ƶ��ƫ��
#define R_TX_FIFO_CTRL1 0x7c //tx fifo���ƣ�tx����������
#define R_TX_FIFO_CTRL2 0x7d//tx fifo���ƣ�tx����������
#define R_RX_FIFO_CTRL 0x7e//rx fifo ����������
#define R_FIFO 0x7f //fifo


const u8 GFSKRfSettings_Si443x[][15] ={
//{ IFBW(1C), COSR, CRO2(21), CRO1(22), CRO0(23), CTG1(24), CTG0(25), TDR1(6E), TDR0(6F), MMC1(70), TXFDEV,	RXFDEV 	B_TIME,  AFC(1E), AFC Limiter(2A)	
{0x1D, 0x41, 0x60, 0x27, 0x52, 0x00, 0x04, 0x13, 0xa9, 0x20, 0x3d, 	0x3d,		209,	0x00,	0x21},  	//DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 95.3kHz	
{0x1D, 0xA1, 0x20, 0x4E, 0xA5, 0x00, 0x0C, 0x27, 0x52, 0x20, 0x3D, 	0x3D,		105,	0x00,	0x21},		//DR: 4.8kbps, DEV: +-38.4kHz, BBBW: 95.3kHz 
{0x1E, 0xD0, 0x00, 0x9D, 0x49, 0x00, 0x29, 0x4e, 0xa5, 0x20, 0x3D, 	0x3D,		53,		0x00,	0x21},		//DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 102.2kHz
{0x1E, 0xc8, 0x00, 0xA3, 0xD7, 0x00, 0x2B, 0x51, 0xec, 0x20, 0x40, 	0x40,		50,		0x00,	0x21},		//DR: 10kbps, DEV: +-40kHz, BBBW: 102.2kHz
{0xAC, 0x96, 0x00, 0xDA, 0x74, 0x00, 0xDC, 0xa3, 0xd7, 0x20, 0x20, 	0x20,		25,		0x00,	0x1F},		//DR: 20kbps, DEV: +-20kHz, BBBW: 115.6kHz	
{0x04, 0x64, 0x01, 0x47, 0xae, 0x02, 0x91, 0x0A, 0x3D, 0x00, 0x20, 	0x20,		13,		0x00,	0x22},		//DR: 40kbps, DEV: +-20kHz, BBBW: 95.3kHz
{0x05, 0x50, 0x01, 0x99, 0x9A, 0x03, 0x35, 0x0C, 0xCD, 0x00, 0x28, 	0x28,		10,		0x00,	0x28},		//DR: 50kbps, DEV: +-25kHz, BBBW: 112.8kHz
{0x9A, 0x3C, 0x02, 0x22, 0x22, 0x07, 0xFF, 0x19, 0x9A, 0x00, 0x50, 	0x50,		8, 		0x00,	0x48},  	//DR: 100kbps, DEV: +-50kHz, BBBW: 208 kHz
{0x83, 0x5e, 0x01, 0x5D, 0x86, 0x02, 0xBB, 0x20, 0xc5, 0x00, 0x66, 	0x66,		8,		0x00,	0x50},		//DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
{0x8c, 0x2f, 0x02, 0xbb, 0x0d, 0x05, 0x74, 0x41, 0x89, 0x00, 0xcd, 	0xcd, 		4,		0x00, 	0x50},		//DR: 256kbps, DEV: +-128kHz, BBBW: 518.8kHz
};

volatile u8 gSiRssiThred=WRF_RSSI_THRESHOLD;//rssi��ֵ������������ʱ��ͬ�������ݿ��ֵ

static void Si4432_DelayMs(u32 Ms)
{
	u32 i;
	for(;Ms;Ms--) for(i=10000;i;i--);
}

static void Si4432_WriteReg(u8 Reg, u8 Val)
{	
	Select_Si4432() ;
	Si4432_Spi_Write(WRITEREG|Reg);
	Si4432_Spi_Write(Val);
	NotSelect_Si4432();
}

static u8 Si4432_ReadReg(u8 Reg)
{
	u8 Val;
	
	Select_Si4432() ;
	Si4432_Spi_Write(READREG&Reg);
	Val = Si4432_Spi_Write(0);
	NotSelect_Si4432();
	
	return Val;
}

static void Si4432_BulkWriteReg(u8 Reg, u8 *data, u8 num)
{
	u8 num_ctr;
	
	Select_Si4432();  

	Si4432_Spi_Write(WRITEREG|Reg); 
	for(num_ctr=0; num_ctr<num; num_ctr++) 
	{
		Si4432_Spi_Write(data[num_ctr]);
	}
	NotSelect_Si4432();
}

static void Si4432_BulkReadReg(u8 Reg, u8 *data, u8 num)
{
	u8 num_ctr;
	
	Select_Si4432();
	Si4432_Spi_Write(READREG&Reg);
	for(num_ctr=0;num_ctr<num;num_ctr++)
		data[num_ctr] = Si4432_Spi_Read();
	NotSelect_Si4432();
}

static __inline void Si4432_ClearNIRQ(void)
{
	Si4432_ReadReg(R_IT_STAUS_1);	 //��״̬�Ĵ���������RFģ���ж�	
	Si4432_ReadReg(R_IT_STAUS_2);	//��״̬�Ĵ���2������RFģ���ж�	
}

static void Si4432_SetRxMode(u32 RxAddr)
{	
	//Debug("SetRxAddr:0x%x\n\r",RxAddr);
	Si4432_WriteReg(R_RUN_MODE_1, B_MODE_READY);	//���� Ready ģʽ
	ANT_TX0_RX1();		//�������߿���

	Si4432_WriteReg(R_RUN_MODE_2, Si4432_ReadReg(R_RUN_MODE_2)|B_RM2_CLEAN_RX_FIFO); //����FIFO��0
	Si4432_WriteReg(R_RUN_MODE_2, Si4432_ReadReg(R_RUN_MODE_2)&(~B_RM2_CLEAN_RX_FIFO));
	
	Si4432_ClearNIRQ();

	Si4432_WriteReg(R_FH_CHK0,(RxAddr>>24)&0xff);//��ҪУ���ͷ��,���ڵ�ַ��֤
	Si4432_WriteReg(R_FH_CHK1,(RxAddr>>16)&0xff);
	Si4432_WriteReg(R_FH_CHK2,(RxAddr>>8)&0xff);
	Si4432_WriteReg(R_FH_CHK3,RxAddr&0xff);   
		
	Si4432_WriteReg(R_IT_ENABLE_1, B_IT1_IPKVALID);  //RFģ���յ��������ݺ󣬲����ж�
#if RSSI_DEBUG_MODE //�źŵ���ģʽ
	Si4432_WriteReg(R_IT_ENABLE_2, B_IT2_ISWDET|B_IT2_IRSSI);//����ͬ���ּ��
#else
	Si4432_WriteReg(R_IT_ENABLE_2, 0);//����ͬ���ּ��
#endif

	Si4432_WriteReg(R_RUN_MODE_1, B_MODE_RECV);  //RFģ��������ģʽ

#if PRODUCT_IS_WEIBO
	IOIN_OpenExti(IOIN_WEIBO_IN);//��weibo
#endif	
}

static void Si4432_SetTxMode(u32 TxAddr)
{
#if PRODUCT_IS_WEIBO
	IOIN_CloseExti(IOIN_WEIBO_IN);//��weibo
#endif

	//Debug("SetTxAddr:0x%x\n\r",TxAddr);
	Si4432_WriteReg(R_RUN_MODE_1, B_MODE_READY);	//rfģ�����Readyģʽ
	ANT_TX1_RX0();		//�������߿��صķ���

	Si4432_WriteReg(R_RUN_MODE_2, Si4432_ReadReg(R_RUN_MODE_2)|B_RM2_CLENA_TX_FIFO); //����FIFO��0
	Si4432_WriteReg(R_RUN_MODE_2, Si4432_ReadReg(R_RUN_MODE_2)&(~B_RM2_CLENA_TX_FIFO));
	
	Si4432_ClearNIRQ();

	Si4432_WriteReg(R_TX_FH0,(TxAddr>>24)&0xff); //�����ͷ�룬���ڵ�ַУ��
	Si4432_WriteReg(R_TX_FH1,(TxAddr>>16)&0xff);
	Si4432_WriteReg(R_TX_FH2,(TxAddr>>8)&0xff);
	Si4432_WriteReg(R_TX_FH3,TxAddr&0xff);  
		
	Si4432_WriteReg(R_IT_ENABLE_1, B_IT1_IPKSENT);	//�������ݷ�����󣬲����ж�
	Si4432_WriteReg(R_RUN_MODE_1, B_MODE_SEND); //���뷢��ģʽ
}

void Si4432_SetTxPower(u8 Val)
{
	Si4432_WriteReg(R_TX_POWER,Val&0x07);
}

//���ò�ͬƵ��
void Si4432_SetChannel(u16 Channel)
{
	if(Channel>=50) return;//480M����Ҫƫ��

	if(Channel==0)
	{
		Debug("Freq 430.5M\n\r");
		Si4432_WriteReg(0x75, 0x53);  //Ĭ��Ƶ������433.5
		Si4432_WriteReg(0x76, 0x57);  
		Si4432_WriteReg(0x77, 0x80);
	}
	else
	{
		Debug("Freq %uM\n\r",430+Channel);
		Si4432_WriteReg(0x75, 0x53+Channel/10); //��430M��ʼ��ÿ1Mһ��Ƶ��
		Si4432_WriteReg(0x76, 25*(Channel%10));
		Si4432_WriteReg(0x77, 0x00);
		//Debug("75H:0x%x, 76H:%x\n\r",0x53+Channel/10,25*(Channel%10));
	}
}

void Si4432_Deinit(void)
{
	WSDN_1();
}

//��ʼ��ʱ�Ĵ�������
static void Si4432_RegConfig(void)
{
	//�������silabs��excel
	Si4432_WriteReg(R_MODULATION_CTRL1, 0x2c);  //write 0x2C to the Modulation Mode Control 1 register 
	Si4432_WriteReg(R_AFC_LOOP_GEAR_OVER, 0x40);  //ʹ�� afc

	//set the desired TX data rate (9.6kbps)
	Si4432_WriteReg(0x1c, 0x1B);														//write 0x05 to the IF Filter Bandwidth register		
	Si4432_WriteReg(0x20, 0xD0);														//write 0xA1 to the Clock Recovery Oversampling Ratio register		
	Si4432_WriteReg(0x21, 0x00);														//write 0x20 to the Clock Recovery Offset 2 register		
	Si4432_WriteReg(0x22, 0x9d);														//write 0x4E to the Clock Recovery Offset 1 register		
	Si4432_WriteReg(0x23, 0x49);														//write 0xA5 to the Clock Recovery Offset 0 register		
	Si4432_WriteReg(0x24, 0x00);														//write 0x00 to the Clock Recovery Timing Loop Gain 1 register		
	Si4432_WriteReg(0x25, 0x34);														//write 0x13 to the Clock Recovery Timing Loop Gain 0 register		

	Si4432_WriteReg(R_TX_BAUD1, 0x4E);														//write 0x4E to the TXDataRate 1 register
	Si4432_WriteReg(R_TX_BAUD0, 0xA5);														//write 0xA5 to the TXDataRate 0 register

	//�������ݰ���ع���
	//���´���ʵ��:8�ֽ�������+2�ֽ�ͬ��+4�ֽ�֡ͷ+1�ֽڳ���+64�ֽ�����+2�ֽ�CRC
	// 4�ֽ�֡ͷ�У�����Ŀ�ĵ�ַ
	Si4432_WriteReg(R_DATA_RW_CTRL, 0x8D);  //ʹ��PH+FIFOģʽ����λ��ǰ�棬ʹ��CRCУ��
#if RSSI_DEBUG_MODE//һ�������˵���ģʽ���Ͳ��ü��֡ͷ��
	Si4432_WriteReg(R_FHC_1, 0); 
#else
	Si4432_WriteReg(R_FHC_1, 0xff);  //bit[0:3]����������Ҫ����֡ͷ������bit[4:7]�������ÿ����㲥��֡ͷ����
#endif
	Si4432_WriteReg(R_FHC_2, 0x42);  //bit[4:7]����֡ͷ����Ϊ4�ֽڣ�bit[1:2]ȡ����ͬ���֣���ͬ����3,2��Ϊͬ����
	Si4432_WriteReg(R_PREALEN_LEN, 16);    //����16��Nibble��Preamble����8�ֽ�
	Si4432_WriteReg(R_PREATH_CTRL1, 4<<3);  //��Ҫ���4��Nibble��Preamble
	Si4432_WriteReg(R_SYNC_CHAR_3, 'w');  //ͬ����
	Si4432_WriteReg(R_SYNC_CHAR_2, 'n');	//ͬ����
	Si4432_WriteReg(R_SYNC_CHAR_1, 'e');
	Si4432_WriteReg(R_SYNC_CHAR_0, 't');

	Si4432_WriteReg(R_FH_EN3, 0xff);  //ͷ��1,2,3,4 ������λ����ҪУ��
	Si4432_WriteReg(R_FH_EN2, 0xff);  // 
	Si4432_WriteReg(R_FH_EN1, 0xff);  // 
	Si4432_WriteReg(R_FH_EN0, 0xff);  // 

	//����ͨ�Ų���
	Si4432_WriteReg(R_TX_POWER, 0x07);  //���书������  0x00:+0dBM  0x01:+3dBM  0x02:+6dBM  0x03:+9dBM  0x04:+11dBM  0x05:+14dBM  0x06:+17dBM  0x07:20dBM
	Si4432_WriteReg(0x79, 0x0);   //����Ҫ��Ƶ
	Si4432_WriteReg(0x7a, 0x0);   //����Ҫ��Ƶ
	Si4432_WriteReg(R_MODULATION_CTRL2, 0x22);  //���䲻��ҪCLK��FiFo��FSKģʽ
	Si4432_WriteReg(R_FREQ_DEVIATATION, 0x19);  //ƵƫΪ30KHz
	Si4432_WriteReg(0x73, 0x0);   //û��Ƶ��ƫ��
	Si4432_WriteReg(0x74, 0x0);   //û��Ƶ��ƫ��
	
	//����ͨ��Ƶ��
	Si4432_SetChannel(0);
}

//��ʼ��ʱ�Ĵ�������
static void Si4432_RegConfig2(void)
{
	//�������silabs��excel
	Si4432_WriteReg(R_MODULATION_CTRL1, 0x2c);  //write 0x2C to the Modulation Mode Control 1 register 
	Si4432_WriteReg(R_AFC_LOOP_GEAR_OVER, 0x40);  //ʹ�� afc

	//set the desired TX data rate (19.2kbps)
	Si4432_WriteReg(0x1c, 0x05);														//write 0x05 to the IF Filter Bandwidth register		
	Si4432_WriteReg(0x20, 0xD0);														//write 0xA1 to the Clock Recovery Oversampling Ratio register		
	Si4432_WriteReg(0x21, 0x00);														//write 0x20 to the Clock Recovery Offset 2 register		
	Si4432_WriteReg(0x22, 0x9d);														//write 0x4E to the Clock Recovery Offset 1 register		
	Si4432_WriteReg(0x23, 0x49);														//write 0xA5 to the Clock Recovery Offset 0 register		
	Si4432_WriteReg(0x24, 0x02);														//write 0x00 to the Clock Recovery Timing Loop Gain 1 register		
	Si4432_WriteReg(0x25, 0x78);														//write 0x13 to the Clock Recovery Timing Loop Gain 0 register		

	Si4432_WriteReg(R_TX_BAUD1, 0x9d);														//write 0x4E to the TXDataRate 1 register
	Si4432_WriteReg(R_TX_BAUD0, 0x49);														//write 0xA5 to the TXDataRate 0 register

	//�������ݰ���ع���
	//���´���ʵ��:8�ֽ�������+2�ֽ�ͬ��+4�ֽ�֡ͷ+1�ֽڳ���+64�ֽ�����+2�ֽ�CRC
	// 4�ֽ�֡ͷ�У�����Ŀ�ĵ�ַ
	Si4432_WriteReg(R_DATA_RW_CTRL, 0x8D);  //ʹ��PH+FIFOģʽ����λ��ǰ�棬ʹ��CRCУ��
#if RSSI_DEBUG_MODE//һ�������˵���ģʽ���Ͳ��ü��֡ͷ��
	Si4432_WriteReg(R_FHC_1, 0); 
#else
	Si4432_WriteReg(R_FHC_1, 0xff);  //bit[0:3]����������Ҫ����֡ͷ������bit[4:7]�������ÿ����㲥��֡ͷ����
#endif
	Si4432_WriteReg(R_FHC_2, 0x42);  //bit[4:7]����֡ͷ����Ϊ4�ֽڣ�bit[1:2]ȡ����ͬ���֣���ͬ����3,2��Ϊͬ����
	Si4432_WriteReg(R_PREALEN_LEN, 16);    //����16��Nibble��Preamble����8�ֽ�
	Si4432_WriteReg(R_PREATH_CTRL1, 4<<3);  //��Ҫ���4��Nibble��Preamble
	Si4432_WriteReg(R_SYNC_CHAR_3, 'w');  //ͬ����
	Si4432_WriteReg(R_SYNC_CHAR_2, '2');	//ͬ����
	Si4432_WriteReg(R_SYNC_CHAR_1, 0);
	Si4432_WriteReg(R_SYNC_CHAR_0, 0);

	Si4432_WriteReg(R_FH_EN3, 0xff);  //ͷ��1,2,3,4 ������λ����ҪУ��
	Si4432_WriteReg(R_FH_EN2, 0xff);  // 
	Si4432_WriteReg(R_FH_EN1, 0xff);  // 
	Si4432_WriteReg(R_FH_EN0, 0xff);  // 

	//����ͨ�Ų���
	Si4432_WriteReg(R_TX_POWER, 0x07);  //���书������  0x00:+0dBM  0x01:+3dBM  0x02:+6dBM  0x03:+9dBM  0x04:+11dBM  0x05:+14dBM  0x06:+17dBM  0x07:20dBM
	Si4432_WriteReg(0x79, 0x0);   //����Ҫ��Ƶ
	Si4432_WriteReg(0x7a, 0x0);   //����Ҫ��Ƶ
	Si4432_WriteReg(R_MODULATION_CTRL2, 0x22);  //���䲻��ҪCLK��FiFo��FSKģʽ
	Si4432_WriteReg(R_FREQ_DEVIATATION, 0x17);  //ƵƫΪ4.8KHz
	Si4432_WriteReg(0x73, 0x0);   //û��Ƶ��ƫ��
	Si4432_WriteReg(0x74, 0x0);   //û��Ƶ��ƫ��
	
	//����ͨ��Ƶ��
	Si4432_WriteReg(0x75, 0x53);  //Ƶ������868
	Si4432_WriteReg(0x76, 0x4b);  //
	Si4432_WriteReg(0x77, 0x00);
}


//�����λ����ʼ��
void Si4432_Init(u32 MyAddr,u8 RssiThred)
{	
	u8 a,b,c;
	u32 BusyCnt=0;
	
	//���߳�ʼ��
	Si4432_Spi_Init();
	
	if(0)//if(CheckSecretKey(BIF_KEY4)==FALSE)
	{
        //�˴����أ��ǲ����û��汾û��setkey�����
        Debug("AKEY EA\n\r");
		return;
	}

	gMySiRxAddr=MyAddr;

	//Ӳ����λ
	Si4432_DelayMs(100);
	WSDN_1();Si4432_DelayMs(50);
	WSDN_0();Si4432_DelayMs(100);//��ʱ����15ms

	//״̬���
	a=Si4432_ReadReg(0x00);
	b=Si4432_ReadReg(0x01);
	c=Si4432_ReadReg(0x02);
	Debug("Si4432[%02x%02x]:%x\n\r",a,b,c);
	if(a!=0x08||b!=0x06)
	{
		Debug("Si4432 Init Error!\r\n");
		return;
		//while(1);
	}

	Si4432_SetRxMode(gMySiRxAddr);//��������״̬
	if(NIRQ()==0)
	{
		Debug("Si4432 Status Error!\n\r");
		while(1);
	}
	
	Debug("%d",NIRQ());
		
	//�����λ
	Si4432_WriteReg(R_RUN_MODE_1, B_MODE_SWRET); //write 0x80 to the Operating & Function Control1 register 
	Debug("%d",NIRQ());
	while (NIRQ() == 1)
	{
		Debug("."); //read interrupt status registers to clear  the interrupt flags and release NIRQ()pin 
		if(BusyCnt++>1000) RebootBoard();
	}
	Debug("%d",NIRQ());
	Si4432_ClearNIRQ();
	Debug("\n\r");
	
	//��ʼ����оƬ
	Si4432_WriteReg(R_RUN_MODE_1, B_MODE_READY);  //����Readyģʽ
	Si4432_WriteReg(R_IT_ENABLE_2, 0);//�رղ���Ҫ���ж�
	
	Si4432_WriteReg(R_XTAL_P, 0xf7);  //���ص���,0x7f=12P,0x60=6p����ɫģ��7f����ɫģ��f7
	Si4432_WriteReg(R_MCU_OUT_CLOCK, 0x05);  //�رյ�Ƶ���
	Si4432_WriteReg(R_GPIO0_CONF, 0xea);  //GPIO 0 ������ͨ�����
	Si4432_WriteReg(R_GPIO1_CONF, 0xea);  //GPIO 1 ������ͨ�����
	Si4432_WriteReg(R_GPIO2_CONF, 0x1f);  //GPIO 2 

	Si4432_RegConfig();//����оƬͨѶ����
	
	Si4432_SetRssiThred(RssiThred);//���˵�SI4432_RSSI_THRESHOLD���µ��ź�
	
	Si4432_SetRxMode(gMySiRxAddr);	
}

//��ȡ�ŵ�ǿ��
u8 Si4432_GetRssi(void)
{
	return Si4432_ReadReg(R_RSSI);
}

//������һ��Ҫ����һ�Σ�ͬ�����ݿ��ֵ��gRssiThred
void Si4432_SetRssiThred(u8 RssiThred)
{	
	gSiRssiThred=RssiThred;
	Si4432_WriteReg(R_RSSITH,gSiRssiThred);
}

u8 Si4432_GetRssiThred(void)
{
	return Si4432_ReadReg(R_RSSITH);
}

//����ŵ��Ƿ�æ
bool Si4432_ChannelBusy(void)
{
	return Si4432_ReadReg(R_RSSI)>gSiRssiThred?TRUE:FALSE;
}

u8 Si4432_RxPacket(u8 *Buf,u32 *pFH)
{	
	u8 Len = Si4432_ReadReg(R_RX_PKTLEN);
 	if(Len)
	{			
	    Si4432_BulkReadReg(R_FIFO, Buf, Len);
		*pFH=(Si4432_ReadReg(R_FH_RX0)<<24)|(Si4432_ReadReg(R_FH_RX1)<<16)|
			(Si4432_ReadReg(R_FH_RX2)<<8)|(Si4432_ReadReg(R_FH_RX3));
		//Debug("R%08x:%d\n\r",*pFH,Len);Debug("Recvd:\n\r");DisplayBuf(Buf,Len,16);
	}
	
	Si4432_WriteReg(R_RUN_MODE_2, Si4432_ReadReg(R_RUN_MODE_2)|B_RM2_CLEAN_RX_FIFO); //����FIFO��0
	Si4432_WriteReg(R_RUN_MODE_2, Si4432_ReadReg(R_RUN_MODE_2)&(~B_RM2_CLEAN_RX_FIFO));
	Si4432_SetRxMode(gMySiRxAddr);
	  
	return Len;
}

bool Si4432_TxPacket(u32 TxAddr,u8 *Buf,u8 Len)
{
    u32 TX_Timeout;
    u8 ItStatus1,ItStatus2;

	Si4432_SetTxMode(TxAddr);
	
	Si4432_WriteReg(R_TX_PKTLEN,Len); 
	Si4432_BulkWriteReg(R_FIFO,Buf,Len);   
	//Debug("Send:");DisplayBuf(Buf,Len,16);

	IOIN_CloseExti(IOIN_WRF_DRV_INT);//���ж�
	TX_Timeout = 0;
	while(NIRQ())		//�ȴ��ж�
	{
	   TX_Timeout++;
		if(TX_Timeout>=1000)
		{
			IOIN_OpenExti(IOIN_WRF_DRV_INT);//���ж�
		    Debug("Tx FAILED\r\n");
			Si4432_Init(gMySiRxAddr,gSiRssiThred);
			return FALSE;//���ش���
		}
		Si4432_DelayMs(1);
	}
	IOIN_OpenExti(IOIN_WRF_DRV_INT);//���ж�

	ItStatus1 = Si4432_ReadReg(R_IT_STAUS_1);  //���жϼĴ���
	ItStatus2 = Si4432_ReadReg(R_IT_STAUS_2);  //���жϼĴ���
	if(ItStatus1&B_IT1_IPKSENT)
	{
		//Debug("Tx Ok %x\r\n",TxAddr); 	
		Si4432_SetRxMode(gMySiRxAddr);
		return TRUE;
	}

	Si4432_SetRxMode(gMySiRxAddr);
	return FALSE;
}

#if 1
#define Si4432_Sdi_Group GPIOB
#define Si4432_Sdi_Pin	GPIO_Pin_5

//ͨ��ook��������
//��ʱmosi���������
void Si4432_IntoOokMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//�Ĵ�������
	//Si4432_WriteReg(0x1c, 0xba);														//write 0x05 to the IF Filter Bandwidth register		
	//Si4432_WriteReg(0x20, 0x9c);														//write 0xA1 to the Clock Recovery Oversampling Ratio register		
	//Si4432_WriteReg(0x21, 0x00);														//write 0x20 to the Clock Recovery Offset 2 register		
	//Si4432_WriteReg(0x22, 0xd1);														//write 0x4E to the Clock Recovery Offset 1 register		
	//Si4432_WriteReg(0x23, 0xb7);														//write 0xA5 to the Clock Recovery Offset 0 register		
	//Si4432_WriteReg(0x24, 0x00);														//write 0x00 to the Clock Recovery Timing Loop Gain 1 register		
	//Si4432_WriteReg(0x25, 0xd4);														//write 0x13 to the Clock Recovery Timing Loop Gain 0 register		
	Si4432_WriteReg(R_MODULATION_CTRL2, 0x91);  //ookģʽ

	//Ƶ������
#if 1	 //433.92
	Si4432_WriteReg(0x75, 0x53);  
	Si4432_WriteReg(0x76, 0x62);  
	Si4432_WriteReg(0x77, 0x00);
#else //315
	Si4432_WriteReg(0x75, 0x47);  
	Si4432_WriteReg(0x76, 0x7d);  
	Si4432_WriteReg(0x77, 0x00);
#endif
	
	Si4432_SetTxMode(gMySiRxAddr);

	GPIO_InitStructure.GPIO_Pin = Si4432_Sdi_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(Si4432_Sdi_Group, &GPIO_InitStructure);
	NotSelect_Si4432();//nSel need high
	
	//SysVars()->RfSendBySi=TRUE;
}

void Si4432_LeaveOokMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = Si4432_Sdi_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(Si4432_Sdi_Group, &GPIO_InitStructure);

	Si4432_RegConfig();
	
	Si4432_SetRxMode(gMySiRxAddr);
	//SysVars()->RfSendBySi=FALSE;
}
#endif

#include "WNetRecvQ.h"
#include "EventInHandler.h"
void Si4432_ISR(void)
{
	u8 ItStatus1,ItStatus2;

	ItStatus1 = Si4432_ReadReg(R_IT_STAUS_1);  //���жϼĴ���
	ItStatus2 = Si4432_ReadReg(R_IT_STAUS_2);

	//Debug("It %x %x %d\n\r",ItStatus1,ItStatus2,Si4432_ReadReg(R_RX_PKTLEN));

#if RSSI_DEBUG_MODE //�źŵ���ģʽ
	//���������ޣ�������irssi�жϺ󣬵�����rssi����ǿ�ȣ���λ����λ
	if(ItStatus2&B_IT2_IRSSI)//�ź�ǿ�ȳ�������
	{
		//Debug("IR%d\n\r",Si4432_ReadReg(R_IT_STAUS_2)&B_IT2_IRSSI);
	}
	
	if(ItStatus2&B_IT2_ISWDET)//��⵽ͬ����
	{
		Debug("%5d  | ",WRF_DRV.pWRF_GetRssi());
	}
#endif

	if(ItStatus1&B_IT1_IPKVALID)
	{
		WNET_RECV_BLOCK *pRecv=GetWNetIdleRecvQ();

		if(pRecv!=NULL)
		{
			pRecv->Len=Si4432_RxPacket(pRecv->Data,&pRecv->DstAddr);//��ȡ����
			SetEventFlag(EBF_WNET_PACKET_IN);
		}
		else
		{
			Debug("Full!\n\r");
		}
	}
}

void Si4432_TestPacket(void)
{

#if 1//send	
	while(1)
	{
		Si4432_TxPacket(0x200027f2,"2234567890",10);
		Si4432_DelayMs(1000);
	}
#else
	u8 RxBuf[WRF_DRV_PKT_LEN+1];
	u32 DstAddr;

	Si4432_SetRxMode(gMySiRxAddr);
	while(1)
	{	
		u8 Len=0;
		u8 Rssi;
		
		//if(gSiIntFlag == FALSE) continue;
		//gSiIntFlag=FALSE;
		while (NIRQ() == 1);
		Len=Si4432_RxPacket(RxBuf,&Rssi,&DstAddr);
		if(Len)
		{
			//IOOUT_SetIoStatus(IOOUT_BEEP,TRUE);
			RxBuf[Len]=0;
			Debug("%d[%d]%s\n\r",Rssi,Len,RxBuf);
			//DisplayBuf(RxBuf,16,16);
			//Si4432_DelayMs(Rssi*5);
			
			Si4432_WriteReg(R_RUN_MODE_2, Si4432_ReadReg(R_RUN_MODE_2)|B_RM2_CLEAN_RX_FIFO); //����FIFO��0
			Si4432_WriteReg(R_RUN_MODE_2, Si4432_ReadReg(R_RUN_MODE_2)&(~B_RM2_CLEAN_RX_FIFO));
			MemSet(RxBuf,0,Len);
			//IOOUT_SetIoStatus(IOOUT_BEEP,FALSE);
		}
		Si4432_SetRxMode(gMySiRxAddr);
	}
#endif
}


