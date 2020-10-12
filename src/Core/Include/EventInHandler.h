#ifndef __EVENT_IN_HANDLER_H__
#define __EVENT_IN_HANDLER_H__


typedef enum{
	EBF_NULL=0,

	//�����¼����û�������������
	EBF_NEXT_QUICK_FUNC,
	
	//Ӳ���ж��¼�
	EBF_KEY,//�����¼�
	EBF_PIO_IN,//io�¼�
	EBF_SEC_FUNC,//ÿ���¼�
	EBF_30SEC,//30���¼�
	EBF_TIM2,//��ʱ��2�����¼�
	EBF_TIM4,//��ʱ��2�����¼�
	EBF_IR,//��������¼�

	//ϵͳ�¼�
	EBF_IDLE,//ϵͳ��ѭ��
	EBF_INIT,//ϵͳ��ʼ�����
	
	//���ݴ���
	EBF_Q_COM_CMD,//��qwifi���ӵĴ������ַ���ָ�����
	EBF_QWIFI_STATE,//qwifi��״̬����
	EBF_QWIFI_KEY,//qwifi��app�ϵİ�ť������
	EBF_QWIFI_VAR,//qwifi�ı����ı���
	EBF_QWIFI_MSG,//qwifi�յ�ϵͳ��Ϣ
	EBF_QWIFI_READ_VAR_RET,//��ȡqwifi�ı������ؽ��
	EBF_QWIFI_READ_VAR_ERROR,//��ȡqwifi�ı������ش���
	EBF_QWIFI_SET_VAR_RET,//����qwifi�ı������ؽ��
	EBF_QWIFI_SET_VAR_ERROR,//����qwifi�ı������ؽ��
	EBF_QWIFI_MSG_RET,//����ϵͳ��Ϣ��qwifi���ؽ��
	EBF_QWIFI_STR_RET,//�����ַ�����qwifi���ؽ��

	//WNET��������
	EBF_WNET_PACKET_IN,//�ײ������ݽ���
	EBF_WNET_DATA_IN,//Ӧ�ò����ݽ���
	
	//�ڲ�����
	EBF_SYS_CMD,//ϵͳ��������
	EBF_LED_MODE=0x30,//ledģʽ
	EBF_RS_COM_CMD,
	
	//�û��Զ����¼�
	EBF_USER_EVT1,
	EBF_USER_EVT2,
	EBF_USER_EVT3,
	EBF_USER_EVT4,









	
	//ϵͳ����
	EBF_NEXT_LOOP_FUNC,//�¸�ѭ������
	
	EBF_MAX
}EVENT_BIT_FLAG;

typedef enum{
	EFR_OK=0,
	EFR_STOP,//�ص�������ش˽�������¼����ٷ��������������

	EFR_MAX
}EVENT_HANDLER_RESUTL;

typedef EVENT_HANDLER_RESUTL (*pEvtFunc)(EVENT_BIT_FLAG,int,void *);
typedef struct{
	EVENT_BIT_FLAG Event;
	pEvtFunc EvtFunc;
}EVENT_FUNC_ITEM;


void EventDebug(void);
void SetEventFlag(EVENT_BIT_FLAG BitFlag);
void SendEvent(EVENT_BIT_FLAG BitFlag,s32 S32Param,void *pParam);
void CleanAllEvent(void);
void *WaitEvent(EVENT_BIT_FLAG *pEvent,s32 *pS32);
bool CheckEventFinished(EVENT_BIT_FLAG Event);


#endif

