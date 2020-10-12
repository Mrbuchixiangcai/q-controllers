#include "SysDefines.h"
//#include "SecretRun.h"

#define WNET_SYNC_ACK_DELAY_MSK 0xfff

void AppButtonActive(u8 Button)
{
	if(Button && Button<100) //��ֵ����io
	{
		u8 Str[16];

		UpdateIoStateByButtonMap(Button);

#if !USER_COM_HOOK
		sprintf((void *)Str,"#key %u\r",Button);
		UserComBulkSend(Str);
#endif
	}
	else if(Button>=100 && Button<=(100+RFS_GetConfig()->VarNum)) //��ֵ�洢����
	{
		u8 VarIdx=Button-100;
		
		if(VarIdx)//���浥��
		{
			RFS_SetVarValSave(VarIdx-1,SelfVarRead(VarIdx-1));
		}
		else//��������
		{
			u8 Id;
			for(Id=0;Id<RFS_GetConfig()->VarNum;Id++)
			{
				RFS_SetVarValSave(Id,SelfVarRead(Id));
			}
		}	
		RFS_BurnToRom();
	}	
}


//���úò���
//num��datalenһ�����
bool WNetAttribGenericHandler(WCON_PACKET *pPkt)
{
	u16 DataLen=0;
	u16 i;
	bool BurnDbFlag=FALSE;
	u8 *pBuf=Q_Mallco(64);
	WCON_PACKET *pReply=Q_Mallco(256);

	if(pPkt->Num==0 || pPkt->DataLen==0)
	{
		pPkt->Num=pPkt->DataLen=0;
		return FALSE;
	}

	pReply->Num=pReply->DataLen=0;
	for(i=0;i<pPkt->Num;i++)
	{
		WNET_ATTRIB_BLOCK *pBlock=(void *)&pPkt->Data[DataLen];
		
		DataLen+=(sizeof(WNET_ATTRIB_BLOCK)+pBlock->AddLen);//������һ��λ��
		MemCpy(pBuf,pBlock,sizeof(WNET_ATTRIB_BLOCK)+pBlock->AddLen);//��ֹ�ֽڶ������
		pBlock=(WNET_ATTRIB_BLOCK *)pBuf;
		
		if((pPkt->Res==WPR_MAIN && pBlock->Act==WAA_SET) || (pPkt->Res==WPR_SUCESS && pBlock->Act==WAA_GET)) //����д(����)ģ��
		{
			switch(pBlock->Type)
			{
				case WDA_TRANSER_U32://�ҵİ���˭ת��
					{
						u32 Addr;
						MemCpy(&Addr,pBlock->Val,4);
						Debug("  WNetTransBy %x\n\r",Addr);
						
						if(RFS_GetNum(RFSI_WNET_TRANSER)!=Addr)
						{						
							RFS_SetNum(RFSI_WNET_TRANSER,Addr);
							BurnDbFlag=TRUE;
						}
					}
					break;
				case WDA_KEY:
					{
						if(pBlock->Val[0])
						{
							AppButtonActive(pBlock->Val[0]);
#if OPEN_USER_HOOK
							UserAppButtonHook(pBlock->Val[0]);
#endif
						}
					}
					break;
				case WDA_BEAT://���ֵΪ0��˵�������ر������ֵΪ1�����ʾ������ģ���ط�sync
					{
						if(pBlock->Val[0]==1)
						{
							AddOnceSecFunc(1,SyncToHost,0,NULL);
						}
					}
					break;
				case WDA_PASSWORD:
					{
						if(pBlock->Val[0]==(LBit16(WNetMyAddr())^0xa5a5))//�ָ�Ĭ��
						{
							AddNextVoidFunc(FALSE,DefaultConfig);
						}
					}
					break;
			}
		}
		else if(pPkt->Res==WPR_MAIN && pBlock->Act==WAA_GET) //������ģ��
		{
			switch(pBlock->Type)
			{
				case WDA_VER:
					WPktSetAttrib(pReply,WAA_GET,pBlock->Type,WNET_VERSION);
					break;
				case WDA_HW:
					WPktSetAttrib(pReply,WAA_GET,pBlock->Type,WDT_WDEV_IO);
					break;
					
				default:
					WPktSetAttrib(pReply,WAA_GET,pBlock->Type,0);
			}
		}
	}

	//����ذ�
	pPkt->Num=pReply->Num;
	pPkt->DataLen=pReply->DataLen;
	MemCpy(pPkt->Data,pReply->Data,pReply->DataLen);
	if(BurnDbFlag)
	{
		AddNextVoidFunc(FALSE,RFS_BurnToRom);
	}

	Q_Free(pBuf);
	Q_Free(pReply);
	
	return TRUE;
}

//����ǰ������
void WDevConfigHandler(WCON_PACKET *pConPkt,u32 SrcAddr,u32 DstAddr)
{
	//WVAR_PACKET *pVarPkt=(void *)pConPkt;
	WWAV_PACKET *pWavPkt=(void *)pConPkt;

	switch(pConPkt->Type)
	{
		case WPT_ADD_CLIENT:
			if(ClientAddr(DstAddr) == WNET_BROADCAST_ADDR)//�㲥
			{
				HostCmdHandler(pConPkt,SrcAddr,DstAddr);
			}
			break;
		case WPT_TEST:
			{
				IOOUT_SetIoStatus(IOOUT_LED1,IOOUT_ReadIoStatus(IOOUT_LED1)?FALSE:TRUE);
				IOOUT_SetIoStatus(IOOUT_LED2,IOOUT_ReadIoStatus(IOOUT_LED1)?FALSE:TRUE);
#if OPEN_USER_HOOK
				UserTestHook();
#endif
			}
			break;
			
//----------------------------------------------------------waver���ð�-----------------------------------------------------------------			
		case WPT_WAVER_CONFIG:
			{
				if(RFS_GetConfig()->ProdID==0 && CheckWaverParamTab(pWavPkt->Tab)==0)//��ʼ��״̬���ܱ�����
				{
					RFS_SetConfig(pWavPkt->Tab);
					AddNextVoidFunc(FALSE,RFS_BurnToRom);
					AddOnceMsFunc(1000,(pStdFunc)RebootBoard,0,NULL);

					pWavPkt->Res=WPR_SUCESS;
				}
				else
				{
					Debug("WaverParamTab Error %u\n\r",CheckWaverParamTab(pWavPkt->Tab));
					pWavPkt->Res=WPR_PARAM_ERROR;					
				}			
				
				WnetSendToFly(SrcAddr,(void *)pWavPkt);	
			}
			break;		
	}
}

//������ɺ���հ�
void HostCmdHandler(WCON_PACKET *pConPkt,u32 SrcAddr,u32 DstAddr)
{
	WVAR_PACKET *pVarPkt=(void *)pConPkt;
	WSTR_PACKET *pStrPkt=(void *)pConPkt;

	switch(pConPkt->Type)//�������޶�
	{	
		case WPT_ADD_CLIENT:
		case WPT_SYNC:
		case WPT_UNBIND:
		case WPT_CONFIG:
		case WPT_TEST:	
		case WPT_VAR_SYNC:		
		case WPT_DEV_STR:
			break;
		
		default:
			Debug("WPKT DIR ERROR! 0x%x\n\r",pConPkt->Type);
			return;
	}

	switch(pConPkt->Type)
	{
		case WPT_ADD_CLIENT:
			{
				if(GroupAddr(SrcAddr)!=WDG_FLY) break;

				if(ClientAddr(DstAddr) == WNET_BROADCAST_ADDR)//�㲥
				{
					u16 WNetVer=0;

					WPktFindAttrib(pConPkt,WAA_SET,WDA_VER,1,&WNetVer);			
					if(WNetVer==WNET_VERSION && MyAddrInclude(pConPkt)==FALSE)//��ַ�б�δ�����Լ�
					{
						u32 t;
						t=Rand(WNET_SYNC_ACK_DELAY_MSK);//�����ʱ����
						Debug("DelayMs%d\n\r",t);
						AddOnceMsFunc(t,(pStdFunc)RespAddClientToHost,pConPkt->PktCnt,(void *)SrcAddr);
					}
				}
				else if(DstAddr==WNetMyAddr())//�������������豸
				{
					if(RFS_GetNum(RFSI_FLY_ADDR)==0 || RFS_GetNum(RFSI_FLY_ADDR)==SrcAddr)//ֻ�е�������״̬���ſ������°�����
					{
						u16 WNetVer=0;
						u32 ProdID=0;
						u16 SyncDelay=0;

						WPktFindAttrib(pConPkt,WAA_SET,WDA_VER,1,&WNetVer);
						WPktFindAttrib_Array(pConPkt,WAA_SET,WDA_PRODUCT_ID_U32,1,(void *)&ProdID);
						WPktFindAttrib_Array(pConPkt,WAA_SET,WDA_SYNC_DELAY,1,&SyncDelay);
						
						if(WNetVer!=WNET_VERSION)
						{
							Debug("WNet Ver Error!%u %u\n\r",WNetVer,WNET_VERSION);
							pConPkt->Res=WPR_AUTH_ERROR;
							WnetSendToFly(SrcAddr,(void *)pConPkt);
						}
#if 0						
						else if(ProdID!=RFS_GetConfig()->ProdID)//prod id ����
						{
							Debug("ProdID Error!%u %u\n\r",ProdID,RFS_GetConfig()->ProdID);
							pConPkt->Res=WPR_PARAM_ERROR;
							WnetSendToFly(SrcAddr,(void *)pConPkt);
						}
#endif
						else //��Ҫ����
						{
							WNetAttribGenericHandler(pConPkt);
					
							pConPkt->Num=pConPkt->DataLen=0;
							pConPkt->Res=WPR_SUCESS;
							WnetSendToFly(SrcAddr,(void *)pConPkt);

							RFS_SetNum(RFSI_FLY_ADDR,SrcAddr);
							AddNextVoidFunc(FALSE,RFS_BurnToRom);		

							//��ʱ5�룬����ͬ�����������������±���
							AddOnceSecFunc(SyncDelay,SyncToHost,0,NULL);
						}
					}
					else //�Ѿ��������ˣ��޷����յڶ�������
					{
						pConPkt->Res=WPR_RES_UNENOUGH;
						WnetSendToFly(SrcAddr,(void *)pConPkt);
					}
				}
			}
			break;
		case WPT_SYNC:
			if(RFS_GetNum(RFSI_FLY_ADDR)==SrcAddr) //�󶨵�����������Ϣ
			{
				u16 WNetVer=0;

				WPktFindAttrib(pConPkt,WAA_SET,WDA_VER,1,&WNetVer);			
				if(WNetVer==WNET_VERSION)
				{
					u32 t=0;
					
					DeleteMsFuncByCB(SyncToHost);//һ��fly������waver�Ͳ���������ȡ��Ϣ��

					if(ClientAddr(DstAddr) == WNET_BROADCAST_ADDR)//����ǹ㲥����ʱ�ظ�
						t=500+Rand(WNET_SYNC_ACK_DELAY_MSK);

					Debug("DelayMs%d\n\r",t);
					AddOnceMsFunc(t,(pStdFunc)ReplySyncToHost,pConPkt->PktCnt,(void *)SrcAddr);//���ٵȴ�2��֮��
				}
			}
			else //�ǰ�����������Ϣ
			{
				u16 WNetVer=0;

				WPktFindAttrib(pConPkt,WAA_SET,WDA_VER,1,&WNetVer);
				if(WNetVer==WNET_VERSION)
				{						
					u32 t=0;

					if(ClientAddr(DstAddr) == WNET_BROADCAST_ADDR)//����ǹ㲥����ʱ�ظ�
						t=500+Rand(WNET_SYNC_ACK_DELAY_MSK);//�����ʱ����

					Debug("DelayMs%d\n\r",t);
					AddOnceMsFunc(t,(pStdFunc)ReplySyncToHost,pConPkt->PktCnt,(void *)SrcAddr);//���ٵȴ�2��֮��			
				}
			}
			break;

		case WPT_TEST:
			if((RFS_GetNum(RFSI_FLY_ADDR)==0 || RFS_GetNum(RFSI_FLY_ADDR)==SrcAddr))//ֻ���ܰ󶨵�����
			{
				IOOUT_SetIoStatus(IOOUT_LED1,IOOUT_ReadIoStatus(IOOUT_LED1)?FALSE:TRUE);
				IOOUT_SetIoStatus(IOOUT_LED2,IOOUT_ReadIoStatus(IOOUT_LED2)?FALSE:TRUE);
#if OPEN_USER_HOOK
				UserTestHook();
#endif
			}
			break;
		case WPT_UNBIND:
			{
				if(DstAddr==WNetMyAddr())//����󶨱����ǵ���
				{
					if(RFS_GetNum(RFSI_FLY_ADDR)==SrcAddr)//���Լ���������������
					{
						UnbindToHost(FALSE,NULL);						
						pConPkt->Res=WPR_SUCESS;
					}
					else if(RFS_GetNum(RFSI_FLY_ADDR)==0)//������δ��
					{
						pConPkt->Res=WPR_SUCESS;
					}
					else//ǿ�ƽ��
					{	
						u16 Pw=0;//����������ǿ�ƽ��
						WPktFindAttrib(pConPkt,WAA_SET,WDA_PASSWORD,1,&Pw);		
						if(Pw==LBit16(WNetMyAddr()))//��������Լ��Ĵӵ�ַ
						{
							AddNextStdFunc(FALSE,(pStdFunc)UnbindToHost,TRUE,NULL);
							pConPkt->Res=WPR_SUCESS;
						}				
					}
					WnetSendToFly(SrcAddr,(void *)pConPkt);	
				}
			}
			break;
		case WPT_CONFIG:
			if(RFS_GetNum(RFSI_FLY_ADDR)==SrcAddr)//ֻ���ܰ󶨵�����
			{
				WNetAttribGenericHandler(pConPkt);		

				if(pConPkt->Num)
				{
					pConPkt->Res=WPR_SUCESS;
					WnetSendToFly(SrcAddr,(void *)pConPkt);	
				}					
			}
			else //��������ԣ�����ǿ�ƻָ�Ĭ��
			{
				u16 Pw=0;//����������ǿ�ƽ��
				WPktFindAttrib(pConPkt,WAA_SET,WDA_PASSWORD,1,&Pw);

				if(Pw==(LBit16(WNetMyAddr())^0xa5a5))//������ڵ�ַ��16λ��a5a5�����ֵ
				{
					AddNextVoidFunc(FALSE,DefaultConfig);//�ָ�Ĭ������
					pConPkt->Res=WPR_SUCESS;
				}
				else
				{
					pConPkt->Res=WPR_PARAM_ERROR;
				}
				WnetSendToFly(SrcAddr,(void *)pConPkt);	
			}
			break;
		
//----------------------------------------------------------������-----------------------------------------------------------------			
		case WPT_VAR_SYNC:
			if(RFS_GetNum(RFSI_FLY_ADDR)==SrcAddr)//ֻ���ܰ󶨵�����
			{
				if(pVarPkt->Res == WPR_MAIN)//fly�ı���ֵ
				{
					VAR_OBJ *pVarObj=pVarPkt->Vars;
					VAR_OBJ *pOut=pVarPkt->Vars;
					u8 i,ReplyNum=0;

					for(i=0;i<pVarPkt->Num;i++,pVarObj++)
					{
						switch(pVarObj->VarOpt)
						{
							case VOT_SET:
								if(pVarObj->VarIdxType==VIT_RELATE)
								{
									RelVarChange(pVarObj->VarIdx-1,pVarObj->VarValue,pVarObj->VarState);
								}
								else if(pVarObj->VarIdxType==VIT_SELF)
								{
									SelfVarSet(pVarObj->VarIdx-1,pVarObj->VarValue,VSR_HOST);
								}
								break;
							case VOT_GET:
								if(pVarObj->VarIdxType==VIT_RELATE)
								{
									pOut[ReplyNum].VarOpt=pVarObj->VarOpt;
									pOut[ReplyNum].VarState=RelVarGetState(pVarObj->VarIdx-1);
									pOut[ReplyNum].VarIdx=pVarObj->VarIdx;
									pOut[ReplyNum].VarIdxType=pVarObj->VarIdxType;
									pOut[ReplyNum].VarValue=RelVarRead(pVarObj->VarIdx-1);
									ReplyNum++;
								}															
								else if(pVarObj->VarIdxType==VIT_SELF)
								{
									pOut[ReplyNum].VarOpt=pVarObj->VarOpt;
									pOut[ReplyNum].VarState=SelfVarGetState(pVarObj->VarIdx-1);
									pOut[ReplyNum].VarIdx=pVarObj->VarIdx;
									pOut[ReplyNum].VarIdxType=pVarObj->VarIdxType;
									pOut[ReplyNum].VarValue=SelfVarRead(pVarObj->VarIdx-1);
									ReplyNum++;
								}
								break;
						}
					}

					if(ReplyNum)
					{
						pVarPkt->Num=ReplyNum;
						pVarPkt->DataLen=sizeof(VAR_OBJ)*ReplyNum;
						pConPkt->Res=WPR_SUCESS;					
						WnetSendToFly(SrcAddr,(void *)pVarPkt);	
					}
				}		
				else if(pVarPkt->Res == WPR_SUCESS) //�ذ�
				{
					VAR_OBJ *pVarObj=pVarPkt->Vars;
					u8 i;

					for(i=0;i<pVarPkt->Num;i++,pVarObj++)
					{
						if(pVarObj->VarOpt==VOT_GET)//ֻ��Get�лذ�
						{
							if(pVarObj->VarIdxType==VIT_RELATE)
							{
								RelVarChange(pVarObj->VarIdx-1,pVarObj->VarValue,pVarObj->VarState);
							}															
							else if(pVarObj->VarIdxType==VIT_SELF)
							{
								SelfVarSet(pVarObj->VarIdx-1,pVarObj->VarValue,VSR_HOST);
							}
						}
					}
				}
			}
			break;

//----------------------------------------------------------�ַ�����-----------------------------------------------------------------			
		case WPT_DEV_STR:
			if(RFS_GetNum(RFSI_FLY_ADDR)==SrcAddr)//ֻ���ܰ󶨵�����
			{
				if(pStrPkt->Res == WPR_MAIN)//fly�����ַ�������
				{
					u8 *p=Q_Mallco(pStrPkt->DataLen+24);
					u8 Num=pStrPkt->Num;
					
					sprintf((void *)p,"#str %u %s\r",*(u32 *)pStrPkt->Data,&pStrPkt->Data[Num]);
					UserComBulkSend(p);
					Q_Free(p);
				}
			}
			break;			
	}
}





