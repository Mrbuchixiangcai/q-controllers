#ifndef WNET_WAVER_CONFIG_H
#define WNET_WAVER_CONFIG_H

#include "LimitMarco.h"

typedef enum{
	WDT_NULL=0,
	WDT_WDEV_IO,//����ģ��
	WDT_OPEN_DUT,//open dev
	WDT_JMP,//ת����
	WDT_COM,//����͸��
	WDT_WIFI,//wifiģ���豸
	WDT_LOW_POWER,//�͹����豸
	WDT_BLUE_TOOTH,//�����豸

}WAVER_DUT_TYPE;


typedef enum{
	WIM_NULL=0,
	
	WIM_IPD,//�������룬��Ӧ�����غ��½��أ�������Ӧʱ������ƥ�䡢����ƥ�䡢ȡ��������������������Ӧʱ��Ϊ���̣��ʺϽ������źš�
	
	WIM_KEYU,//����ģʽ�����з����Ŵ���ֻ��Ӧ�����أ�������Ӧʱ������ƥ�䡢����ƥ�䡢ȡ��������������������Ӧʱ��Ϊ��ʱ200ms���ʺϽӰ�����
	WIM_KEYU_OPPO,//����ģʽ��ʱ������Ӧ������ʱ������ʱ����ʱ���ž���io�ţ�1-8��
	
	WIM_IPU,//��������

	WIM_OUT_PP,//������������ݰ󶨵ı�����ƥ�������������ĳbit�����仯ʱ������ƥ�䡢����ƥ�䡢ȡ����
	WIM_OUT_PP_KEY,//������������󶨱����������ܵ�keyֵʱ�������øߡ��õ͡�ȡ�������keyֵ����io�ţ�1-8��

	WIM_OUT_OD,//��©���
	WIM_OUT_OD_KEY,//��©���

	WIM_IRQ,//����ͨ���źţ��ߵ�ƽ��Ч	
	WIM_AIN,//ģ�������룬��������ͬ�ϱ�������ͬ�����ϱ����ڵ���ʱ�������δ�䣬���ϱ�
	WIM_PWM,//pwm�����ֻ֧��io7��io8
	
	WIM_MAX
}WAVER_IO_MODE;

typedef enum{
	AIM_NULL=0,
	AIM_MATCH,//ƥ�䡣���ʱ��������ӦbitΪ1�����1��bitΪ0�����0������ʱ��ioΪ1��bit��Ϊ1��ioΪ0��bit��Ϊ0��
	AIM_REVERSE,//����ƥ�䡣���ʱ��bitΪ1�����0��bitΪ0�����1������ʱ��ioΪ1��bit��Ϊ0��ioΪ0��bit��Ϊ1��
	AIM_NEGATION,//ȡ�������ʱ��bit�ı���ȡ�����io״̬������ʱ��io���жϣ���bitȡ��
	AIM_COUNT,//���������ʱ������ָ��key����������������壬���峤��Ϊ100ms����Num�������ƽΪbit������ʱ��io���жϣ��������1

	AIM_MAX,
}VAR_IO_MATCH;

typedef struct{
	WAVER_IO_MODE IoMode:4;//ioģʽ
	VAR_IO_MATCH VarIoMth:3;//io������Ĺ�ϵ
	bool IsRelVar:1;//num��ʾ������ʱ����λΪ0����ʾ�����������λΪ1����ʾ����������
	u8 Num:4;//�����ţ�0��ʾ��ӳ�䣬1-8Ϊ�ڲ�������ţ�1-4Ϊ��ע������š�
	u8 Bit:4;//����λ��0-15
}WAVER_IO_CONFIG;

typedef struct{
	u8 AdcMode;//ʹ��adcʱ����ֵ��1
	u8 Tolerance;//�ݲ1-99%������ֵ����Ľ�������ݲ�����Ÿı�
	u16 Factor;//���ӣ�����ֵ���Դ�ֵ���ټ���ƫ��Ŵ洢����������ֵΪʵ��ֵ��100����������Ϊ0
	s16 Offset;//ƫ��
}WAVER_AIN_CONFIG;

typedef enum{
	WPW_NULL=0,//��ʹ��
	WPW_FIXED,//�̶�ʽ��������bit���ƿ���
	WPW_PERIOD,//����ɵ�ʽ����������Cntֵ
	WPW_FACTOR,//ռ�ձȿɵ�ʽ����������PluseCntֵ
}WAVER_PWM_TYPE;

typedef struct{
	WAVER_PWM_TYPE PwmMode;
	u8 a;
	u16 Cnt;//��������
	u16 PluseCnt;//����ռ�ձ�
	u16 uS_Base;
}WAVER_PWM_CONFIG;

typedef enum{
	AUM_NULL=0,//��Ч�ı���
	AUM_CMD,//�������ϱ������ڷ���ʱ�ϱ�
	AUM_CHANGE,//�����ı�ͷ���
	AUM_IO_KEY,//ͨ��io key�����ͣ�param�洢io�ţ�1-8
	AUM_PERIOD,//�����Է��ͣ�param�洢���� TIME_PERIOD_TYPE��adc����ѡ������򲻻�����������

	AUM_MAX
}VAR_UPD_MODE;

typedef enum{
	APP_1S=0,
	APP_5S,
	APP_10S,
	APP_30S,
	APP_1M,
	APP_5M,
	APP_10M,
	APP_30M,
	APP_1H,
	APP_3H,
	APP_6H,
	APP_10H,
	APP_24H,
	APP_72H,
	APP_1W,	

	APP_MAX,
}TIME_PERIOD_TYPE;//��ӦgTimeMap����

typedef struct{
	VAR_UPD_MODE Mode:3;//�����ϱ���ģʽ
	bool SaveBit:1;//���ú󣬱�������洢
	u8 UpdParam:4;//��ʱ�ϴ�ʱ���洢TIME_PERIOD_TYPE��ͨ��io���ͱ���ʱ���洢io�ţ�1-8��
}WAVER_VAR_CONFIG;//��������

typedef struct{
	TIME_PERIOD_TYPE AwakenPeriod:4;//��ʱ���ڻ���ʱ�䣬Ϊ0ʱ����ʾ������
	u8 a:4;
	u8 b;
	
	u8 LifeTime:7;//��ʱ���Ѻ�Ĵ��ʱ�䣬Ϊ0ʱ����ʾ��Զ���
	bool LifeTimeIsMin:1;	//���ʱ���Ƿ��Է���Ϊ��λ

	u8 RfLifeTime:7;//��Ƶ���Ѻ�Ĵ��ʱ�䣬ֻ���ض��ͺž߱��˹���
	bool RfLifeIsMin:1;	//���ʱ���Ƿ��Է���Ϊ��λ
}WAVER_LOW_POWER_CONFIG;//�͹�������

typedef struct{
	u32 ProdID;//����id���û����룬������˷���
	u8 TabVer;//���ñ�İ汾�ţ���1
	WAVER_DUT_TYPE DutType;//u8 �忨���ͣ�������忨���ף��忨�ϻ�ӡ��
	u16 WNetGroup;//�������ַ

	WAVER_IO_CONFIG IoConfig[WAVER_IO_NUM];//ÿ��io������ģʽ
	WAVER_AIN_CONFIG AioConfig[WAVER_AIO_NUM];//Ŀǰֻ�ṩ4·adc
	WAVER_PWM_CONFIG PwmConfig[WAVER_PWM_NUM];//Ŀǰֻ�ṩ2·pwm

	u8 VarNum;//��������ĸ���
	u8 a;
	u16 b;
	WAVER_VAR_CONFIG VarConfig[SELF_VAR_MAX];//�����ϱ�ģʽ
	s16 VarInitVal[SELF_VAR_TOTAL];//���������ʼֵ	

	u32 resv[8];
	
	WAVER_LOW_POWER_CONFIG LowPowerConfig;//�͹�������

	u32 resvv[8];	
}WAVER_PARAM_TAB;//�����������·�����Ϣ


















#endif

