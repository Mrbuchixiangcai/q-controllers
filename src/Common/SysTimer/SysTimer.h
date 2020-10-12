#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__

#include "FuncType.h"
#include "Product.h"

typedef enum{
	STT_NULL=0,
	STT_AUTO,//ȫ�Զ���ʱ��������󼴿�ʼ��ʱ��Ӧ��һֱ��ϵͳֹͣ
	STT_MANUAL,//�ֶ���ʱ��������󣬲��Զ���ʼ�������StartSysTimer�ſ�ʼ,���ں�ֹͣ�����ٴε���StartSysTimer��ʼ
}SYS_TIMER_TYPE;

typedef struct{
	u32 Id;//���صĶ�ʱ��ʶ��ţ�Ϊ0��ʾδʹ��
	bool ExcInISR;
	SYS_TIMER_TYPE Type;
	EVENT_BIT_FLAG Event;
	u32 Count;//����
	u32 ValueTick;//���ն�ʱֵ
	u32 DefVal;//���Ӷ�ʱ��ʱ����Ķ�ʱֵ
	pVoidFunc pCB;//���ں��callback��auto��manual���͵����ж���ִ��
	pStdFunc pTaskTimoutCB;//����task
	u8 TaskId;//����task
}SYS_TIMER_RCD;

void DebugSysTimer(void);
void SysTimerInit(void);
u32 AddSysTimer(SYS_TIMER_TYPE Type,u32 Value,EVENT_BIT_FLAG Event,pVoidFunc pCallBackFunc,bool ExcInISR);
u32 AddSysTimerForTask(u32 Value,pStdFunc pCallBackFunc,u8 TaskId);
bool ChangeSysTimerVal(u32 Id,u32 Value);
bool StartSysTimer(u32 Id,u32 Value);
bool StopSysTimer(u32 Id);
bool SysTimerWorking(u32 Id);
u32 GetSysTimerRemain(u32 Id);
u32 GetSysTimerCount(u32 Id);
bool DeleteSysTimer(u32 Id);

#define SYS_TIMER_MAX_NUM 20//�������ϵͳ��ʱ����Ŀ


extern u32 gWdgTimer;
extern u32 gTimingFuncTimer;	
extern u32 gWnetTimer;



#endif

