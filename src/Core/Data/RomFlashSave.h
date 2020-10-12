#ifndef __ROM_FLASH_SAVE_H__
#define __ROM_FLASH_SAVE_H__

#include "Product.h"

typedef struct{//�������ݽṹ
	u16 Ver;//�޸ĺ�ᵼ�»ָ�Ĭ��

	//�û��Լ���������ݴ洢��ע���ܳ��Ȳ�Ҫ����ROM_FLASH_PAGE_SIZE
	u32 Num;//ʾ��
	u32 DebugFlags;//bit flag
	u32 Com2Baud;//us2������
	
	u16 RFSI_RSSI_THRD;//rssi��ֵ
	u32 RFSI_FLY_ADDR;//������ַ
	u32 RFSI_BROTHER_ADDR;//͸���ֵܵ�ַ

	bool SnAuth;//������֤

	u32 ChkSum;//ϵͳ�ڲ�ʹ�ã������޸�
}RFS_BLOCK;//�洢�飬���1k

typedef enum{
	DFT_WNET=0,
	DFT_WPKT,
	DFT_WDEV,

	DFT_MAX	
}DEBUG_FLAG_TYPE;

void RFS_Debug(void);
void RFS_Init(void);
void RFS_BurnDefaultToRom(void);
void RFS_BurnToRom(void);
const RFS_BLOCK *RFS_DefDB(void);
RFS_BLOCK *RFS_DB(void);

bool RFS_GetDebugBits(DEBUG_FLAG_TYPE Flag);
void RFS_SetDebugBits(DEBUG_FLAG_TYPE Flag);
#define NeedDebug(x) RFS_GetDebugBits(x)

#endif

