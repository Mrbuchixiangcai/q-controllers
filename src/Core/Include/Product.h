#ifndef __PRODUCT_H__
#define __PRODUCT_H__

#define RELEASE_DAY 20200819
#define PRODUCT_IS_CONTROLLER 0
#define PRODUCT_IS_LIFE1 1
#define PRODUCT_C8_SUPPORT 0 //����code����64k���޷�ʹ��c8оƬ

#if PRODUCT_C8_SUPPORT //ʹ��c8оƬ����ʡ�ɱ�
#define ROM_FLASH_SIZE	     0x10000	//64K
#define ROM_FLASH_PAGE_SIZE 1024		 // 1K
#define IAP_BIN_PAGE_NUM 62//ȫƬ64ҳ��bin���62�����һҳ��������iap��Ϣ
#define ROM_INFO_SAVE_PAGE 62 //�����ڶ�ҳ�����������ݿ�

#define IAP_ROM1_ADDR (0x08000000)
#define IAP_ROM_SIZE 0x10000

#define ROM_FLASH_PAGE_U32_NUM (ROM_FLASH_PAGE_SIZE>>2)
#define IAP_ACCESS_INFO_ADDR	(IAP_ROM1_ADDR+IAP_ROM_SIZE-ROM_FLASH_PAGE_SIZE)//rom���һҳ��Ϊ��Ϣ��¼
#elif (PRODUCT_IS_CONTROLLER || PRODUCT_IS_LIFE1)
#define ROM_FLASH_SIZE	     0x20000	//128K
#define ROM_FLASH_PAGE_SIZE 1024		 // 1K
#define IAP_BIN_PAGE_NUM 126//ȫƬ128ҳ��bin���126�����һҳ��������iap��Ϣ
#define ROM_INFO_SAVE_PAGE 126 //�����ڶ�ҳ�����������ݿ�

#define IAP_ROM1_ADDR (0x08000000)
#define IAP_ROM_SIZE 0x20000

#define ROM_FLASH_PAGE_U32_NUM (ROM_FLASH_PAGE_SIZE>>2)
#define IAP_ACCESS_INFO_ADDR	(IAP_ROM1_ADDR+IAP_ROM_SIZE-ROM_FLASH_PAGE_SIZE)//rom���һҳ��Ϊ��Ϣ��¼
#endif

#define BIN_NOVAILD_FLAG 0x01020304
#define BIN_VAILD_FLAG 0xa5a51234
#define BIN_NEED_BURN_FLAG 0x19850508

//ָ������
#define BB_UPFUNC 7
#define BB_SOFTV 8
#define BB_SIZE 9
#define BB_SUM 10


#endif
