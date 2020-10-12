#ifndef _LEDS_MODE_H_
#define _LEDS_MODE_H_


typedef enum{
	LM_OFF=0,
	LM_ON,
	LM_ON_500MS,
	LM_ON_1S,
	LM_ON_2S,
	LM_FLASH_200MS,
	LM_FLASH_500MS,
	LM_FLASH_2S,
	LM_FLASH_200MS_L2S,

	LM2_OFF=0x10,
	LM2_ON,
	LM2_ON_500MS,
	LM2_ON_1S,
	LM2_ON_2S,	
	LM2_FLASH_200MS,
	LM2_FLASH_500MS,
	LM2_FLASH_2S,
	LM2_FLASH_200MS_L2S,
	
}LEDS_MODE;

#define LMO_IDLE LM2_OFF //����״̬
#define LMO_CANCLE LM2_OFF //����ȡ��
#define LMO_WAIT_PAIR LM2_FLASH_200MS //׼�����
#define LMO_WAIT_BROTHER LM2_FLASH_2S //�ȴ��ֵ�����
#define LMO_WORK LM2_ON //���빤��״̬

#define LMO_ERR LM_FLASH_200MS_L2S //��������
#define LMO_KEY_INDICATE LM_ON_500MS //��������ָʾ

#define LedIndicate(x) SendEvent(EBF_LED_MODE,x,NULL)//�����


#endif

