#ifndef _WORK_MODE_H_
#define _WORK_MODE_H_



typedef enum{
	MNM_IDLE=0,//���ã�δ��
	MNM_START_PAIR,//��ʼ���
	MNM_SYNC,//ȷ����Թ�ϵ
	MNM_PRE_WORK,//����Թ�ϵ�����Է�û����
	MNM_WORK,//�Ѿ���ӣ���������

	MNM_MAX
}NOW_WORK_MODE;
NOW_WORK_MODE WorkMode(void);
void SetWorkMode(NOW_WORK_MODE Mode);
void RevertWorkMode(u32 DelayMs);


#endif

