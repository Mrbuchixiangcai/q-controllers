#include "SysDefines.h"
#include "WorkMode.h"

//����ģʽ
NOW_WORK_MODE gWDevWorkMode=MNM_IDLE;//��ǰģʽ

NOW_WORK_MODE WorkMode(void)
{
	return gWDevWorkMode;
}

void SetWorkMode(NOW_WORK_MODE Mode)
{
	Debug("Mode [%s]\n\r",gNameWDevWorkMode[Mode]);
	gWDevWorkMode=Mode;
}

//��ʱ��ָ���start pairģʽ
static void RevertWorkMode_CB(int a,void *p)
{
	if(gWDevWorkMode==MNM_SYNC) gWDevWorkMode=MNM_START_PAIR;
}

void RevertWorkMode(u32 DelayMs)
{
	AddOnceMsFunc(DelayMs,RevertWorkMode_CB,0,NULL);
}


