#ifndef WRF_DRV_H
#define WRF_DRV_H

//rfsk��Ƶģ���Ӳ��������ļ�
#define USE_SI4432 4432
#define USE_LORA 1276
#define WRF_HW_DRV USE_SI4432 //�˴�������ʲôӲ��ģ��

#if WRF_HW_DRV == USE_SI4432
#define WRF_DRV_INT_LEVEL 0
#define WRF_DRV_PKT_LEN 64 //Ӳ��ԭ������
#define WRF_RSSI_THRESHOLD 100 //���õ�Ӳ������ź����޵�Ĭ��ֵ
#define WRF_RSSI_BOTTOM 70 //���ڴ˷�ֵ��rssi busy��⽫�ر�
#elif WRF_HW_DRV == USE_LORA//sx1278
#define WRF_DRV_INT_LEVEL 1
#define WRF_DRV_PKT_LEN 64 //Ӳ��ԭ������
#define WRF_RSSI_THRESHOLD 100 //���õ�Ӳ������ź����޵�Ĭ��ֵ
#define WRF_RSSI_BOTTOM 70 //���ڴ˷�ֵ��rssi busy��⽫�ر�
#else
#error "No define wrf hw drv!"
#endif


typedef struct{
	void (*pWRF_Deinit)(void);
	void (*pWRF_Init)(u32 MyAddr,u8 RssiThred);
	void (*pWRF_SetChannel)(u16 Channel);
	u8 (*pWRF_GetRssi)(void);
	void (*pWRF_SetRssiThred)(u8 RssiThred);
	u8 (*pWRF_GetRssiThred)(void);
	bool (*pWRF_TxPacket)(u32 TxAddr,u8 *Buf,u8 Len);
	void (*pWRF_SetTxPower)(u8 Val);
	void (*pWRF_TestPacket)(void);
	bool (*pWRF_ChannelBusy)(void);
	void (*pWRF_ISR)(void);
	void (*pWRF_Test)(void);

	void (*pWRF_IntoOokMode)(void);
	void (*pWRF_LeaveOokMode)(void);
}WRF_DRV_FUNC;
extern WRF_DRV_FUNC gWRfDrv;

#define WRF_DRV gWRfDrv

#endif
