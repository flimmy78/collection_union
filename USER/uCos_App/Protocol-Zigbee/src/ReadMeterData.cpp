/*******************************************Copyright (c)*******************************************
**									ɽ������ռ似����˾(�����ֲ�)                                **                            
**  ��   ��   ��: ReadMeterdata.cpp																  **
**  ��   ��   ��: ������																		  **
**	��   ��   ��: 0.1																		      **
**  �� ��  �� ��: 2012��9��20�� 													     	      **
**  ��        ��: ��ȡ����������									      						  **
**	�� ��  �� ¼:   							  												  **
**  ��		  ע: ���غ��¿ص�Э�鶩�Ĳ��ã���ɳ�����д�߼���Ϊ�鷳!����!
****************************************************************************************************/
/********************************************** include *********************************************/
#include <includes.h>
#include "app_flashmem.h"
#include "app_down.h"
#include "app_up.h"
#include "Valve.h"



extern"C"
{
#include "ModBus.h" //����modbus  ,���������
}
/********************************************** static *********************************************/
static uint8 gDEAL_ReadMailBox_Msg18[20] = {0x00};
static uint8 gDEAL_ReadMailBox_Msg1C[20] = {0x00};
static uint8 gDEAL_ReadDataTemp[10*METER_PER_CHANNEL_NUM] = {0x00};

/********************************************** global **********************************************/
void* CmdMsgQ[10];               	//������Ϣָ������, ���洢10����Ϣ
OS_EVENT *CMD_Q; 

extern uint8 gDownCommDev485;



/********************************************************************************************************
**  �� ��  �� ��: test_proMsg_04				    							                       
**	�� ��  �� ��: ����Э����Ϣ                                                                         			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 test_proMsg_04( _ProtocolType &_ProData)
{
	uint8 i=0, Err=0;
	uint8 Channel = 1;
	uint8 RetryTimes = 2;
	uint8 TempLen	= 0x00;
	uint8 *pTemp = _ProData.Data.pData;
	uint8 ProtocolVer	= 0x00;
	uint8 TempData[128] = {0x00};
	uint8 AddrTemp[7] = {0x00};
	uint8 err=0;
	//uint8 addr[7] = {0xFF, 0xFF, 0xF1, 0x22, 0x65, 0x48, 0x25};
	DELU_Protocol	ProtocoalInfo;
	
	ProtocolVer = *pTemp++;						//Э������
	Channel = *pTemp++;
	
	if(ProtocolVer>PROTOCOL_MAX)	  ProtocolVer = 0;
	/*begin:yangfei added 2013-11-2 for �ж�485 �ȱ����Ϊ7ͨ��*/
	if(ProtocolVer==4&&Channel!=7)
		{
		debug("%s:485 meter channel!=7 \r\n",__FUNCTION__);
		goto ERROR;
		}
	/*end:yangfei added 2013-11-2 for �ж�485 �ȱ����Ϊ7ͨ��*/
	/*begin:yangfei added 2013-11-17 for ���㳭���־�����㳭���ʱ������ʵʱ������ֹͨ���л�*/
	OSSemPend(METERChangeChannelSem, OS_TICKS_PER_SEC, &err); //����MBUSͨ���л�Ȩ��
	if(err!=OS_ERR_NONE)
		{
		debug("%s:nwo timing  read meter!\r\n",__FUNCTION__);
		goto ERROR;
		}
	/*end:yangfei added 2013-11-17 for ���㳭���־�����㳭���ʱ������ʵʱ������ֹͨ���л�*/
	(*METER_ComParaSetArray[gMETER_Table[ProtocolVer][0]])();
	/*begin:yangfei added for support 485 meter*/
      gDownCommDev485 = gMETER_Table[ProtocolVer][3];
       /*end:yangfei added for support 485 meter*/
	METER_ChangeChannel(Channel);

	//��д��Ӧ�� �ȼ�����ͨ��Э�� �ṹ��
	ProtocoalInfo.PreSmybolNum  = gMETER_Table[ProtocolVer][2];
	ProtocoalInfo.MeterType 	= 0x20;

	memcpy(AddrTemp, pTemp, 7);
	if(ProtocolVer == 2)
	{							//��Ϊ����˹���򽫱��ַǰ5λΪFFFFF
		for(i=0; i<7; i++)
		{
			ProtocoalInfo.MeterAddr[i] = *(pTemp+6-i);
		}
		ProtocoalInfo.MeterAddr[0] = 0xFF;
		ProtocoalInfo.MeterAddr[1] = 0xFF;
		ProtocoalInfo.MeterAddr[2] |= 0xF0;
	}
	/*begin:yangfei added for support wanhua big meter 2013-08-12*/
	else if(ProtocolVer == 5)
	{
	memcpy(ProtocoalInfo.MeterAddr, pTemp, 7);
	ProtocoalInfo.MeterAddr[5] = 0x01;
	}
	/*end:yangfei added for support wanhua big meter 2013-08-12*/
	else
	{
		memcpy(ProtocoalInfo.MeterAddr, pTemp, 7);
	}
	//memcpy(ProtocoalInfo.MeterAddr, addr, 7);
	ProtocoalInfo.ControlCode 	= 0x01;
	ProtocoalInfo.Length		= 0x03;
	ProtocoalInfo.DataIdentifier= gMETER_Table[ProtocolVer][1];
	memset(ProtocoalInfo.DataBuf, 0x00, sizeof(ProtocoalInfo.DataBuf));


	/*begin:yangfei added 2013-08-05 for add HYDROMETER*/
	if(ProtocolVer == HYDROMETER775_VER||ProtocolVer == ZENNER_VER||ProtocolVer == LANDISGYR_VER||ProtocolVer == ENGELMANN_VER)
	{	
	Err = HYDROMETER(&ProtocoalInfo,ProtocolVer);
	if(Err == NO_ERR)
		{
		CJ188_Format CJ188_Data;
		
		CJ188_Data=METER_Data_To_CJ188Format(ProtocolVer,ProtocoalInfo.DataBuf,ProtocoalInfo.Length-3,&err);
		if(err==0)
			{
			 memcpy(&TempData[8], &CJ188_Data, sizeof(CJ188_Data));
			 ProtocoalInfo.Length = sizeof(CJ188_Data) + 3;
			}
		else
			{
			debug("%s %d METER_Data_To_CJ188Format err=%d\r\n",__FUNCTION__,__LINE__,err );
			 memcpy(&TempData[8], ProtocoalInfo.DataBuf, ProtocoalInfo.Length-3);/*�ϱ�ԭʼ����*/
			}
       
		}
	else
		{
		debug("%s %d  err=%d\r\n",__FUNCTION__,__LINE__,err );
		}
	memcpy(TempData, AddrTemp, 7);
 	TempData[7] = ProtocoalInfo.Length-3;
	_ProData.MsgLength		= ProtocoalInfo.Length-3+8;  
	}
	else
    /*end:yangfei added 2013-08-05 for add HYDROMETER*/
	{
		for(i=0; i<RetryTimes; i++)
 		{
 		Err = METER_MeterCommunicate(&ProtocoalInfo, &TempData[8], &TempLen);
 		if(Err == NO_ERR)
 			{
 				Err = METER_DELU_AnalDataFrame(&ProtocoalInfo, &TempData[8]);
 				if(Err == NO_ERR)
 					{
 					       CJ188_Format CJ188_Data;
 						memcpy(TempData, AddrTemp, 7);
 						TempData[7] = ProtocoalInfo.Length-3;
						/*begin:yangfei added 2013-03-27 for CJ188 format*/
						#if 0
 						memcpy(&TempData[8], ProtocoalInfo.DataBuf, ProtocoalInfo.Length-3);
						#else
						CJ188_Data = METER_Data_To_CJ188Format(ProtocolVer,ProtocoalInfo.DataBuf,ProtocoalInfo.Length-3,&err);
						memcpy(&TempData[8], &CJ188_Data, ProtocoalInfo.Length-3);
						#endif
						/*end:yangfei added 2013-03-27 for CJ188 format*/
 						break;
 					}
 			}
 		}
		_ProData.MsgLength		= ProtocoalInfo.Length-3+8;
	}
	
	OSSemPost(METERChangeChannelSem);
	if(Err != NO_ERR)
		{
ERROR:	    debug("%s:read one meter fail\r\n",__FUNCTION__);
	           _ProData.MsgLength		= 1;
			*_ProData.Data.pDataBack= 0x10;					//�쳣��Ӧ
			Err = 0;
		}
	else
		{
			memcpy(_ProData.Data.pDataBack, TempData, ProtocoalInfo.Length-3+8);
		}
	_ProData.MsgType		= 0x05;
	ReadDateTime(_ProData.TimeSmybol);
	//DISABLE_MBUS();   ʵʱ����ʱΪ������ٶȣ�����1ֻ���ſ�ͨ����
	
	return Err;
	
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_06				    							                       
**	�� ��  �� ��: ���ݼ����㳭������Ϣ�����ء�                                                                    			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 DEAL_ProcessMsg_06( _ProtocolType &_ProData)
{
	uint8 i=0, Err=0;
	uint8 err_valve = 0;
//	uint8 Channel = 1;
	uint8 RetryTimes = 2;
	uint8 TempLen	= 0x00;
	uint8 *pTemp = _ProData.Data.pData;
	uint16 lu16MeterID	= 0x00;
	uint16 lu16MeterSn = 0;
	uint8 TempData[128] = {0x00};
//	uint8 AddrTemp[7] = {0x00};
	uint8 err=0;
	uint8 lu8DataIn[20];  //���س���ʹ�ã����ݲ�����
	uint8 DataFrame[50];  //���س����á�
	uint8 DataLen_Vave;   //���س����á�
	//uint8 addr[7] = {0xFF, 0xFF, 0xF1, 0x22, 0x65, 0x48, 0x25};
	uint8 lu8MBusChannelBusy = 0;//MBUSͨ���Ƿ�æ��־��0-��æ��1-æ��
	DELU_Protocol	ProtocoalInfo;

	MeterFileType	mf;
	
	lu16MeterID = *((uint16 *)pTemp);			//��ȡMeterID.
	pTemp += 2;

	//����lu16MeterID ��ѯ����Ϣ��
	Err = PARA_GetMeterSn_ByMeterID(&lu16MeterID,&lu16MeterSn);

	if(Err == NO_ERR){
		PARA_ReadMeterInfo(lu16MeterSn, &mf);
		
		/*begin:yangfei added 2013-11-17 for ���㳭���־�����㳭���ʱ������ʵʱ������ֹͨ���л�*/
		OSSemPend(METERChangeChannelSem, OS_TICKS_PER_SEC, &err); //����MBUSͨ���л�Ȩ��
		if(err!=OS_ERR_NONE){
			lu8MBusChannelBusy = 1;//���MBUSͨ����ռ�ã�æ��
			debug("%s:nwo timing  read meter!\r\n",__FUNCTION__);
			goto ERROR;
		}
		/*end:yangfei added 2013-11-17 for ���㳭���־�����㳭���ʱ������ʵʱ������ֹͨ���л�*/

		METER_ChangeChannel(mf.ChannelIndex);  //�л�����Ӧͨ��
		(*METER_ComParaSetArray[gMETER_Table[mf.ProtocolVer][0]])();  //���ó��ȱ��ڲ���


		/*begin:yangfei added for support 485 meter*/
		 gDownCommDev485 = gMETER_Table[mf.ProtocolVer][3];
	    /*end:yangfei added for support 485 meter*/
		
		
		//��д��Ӧ�� �ȼ�����ͨ��Э�� �ṹ��
		ProtocoalInfo.PreSmybolNum	= gMETER_Table[mf.ProtocolVer][2];
		ProtocoalInfo.MeterType 	= 0x20;
	
		if(mf.ProtocolVer == 2){
								//��Ϊ����˹���򽫱��ַǰ5λΪFFFFF
			for(i=0; i<7; i++){
				memcpy(ProtocoalInfo.MeterAddr,mf.MeterAddr,7);
			}
			ProtocoalInfo.MeterAddr[0] = 0xFF;
			ProtocoalInfo.MeterAddr[1] = 0xFF;
			ProtocoalInfo.MeterAddr[2] |= 0xF0;
		}
		/*begin:yangfei added for support wanhua big meter 2013-08-12*/
		else if(mf.ProtocolVer == 5){
			memcpy(ProtocoalInfo.MeterAddr,mf.MeterAddr,7);
			ProtocoalInfo.MeterAddr[5] = 0x01;
		}
		/*end:yangfei added for support wanhua big meter 2013-08-12*/
		else{
			memcpy(ProtocoalInfo.MeterAddr,mf.MeterAddr,7);
		}
		ProtocoalInfo.ControlCode	= 0x01;
		ProtocoalInfo.Length		= 0x03;
		ProtocoalInfo.DataIdentifier= gMETER_Table[mf.ProtocolVer][1];
		memset(ProtocoalInfo.DataBuf, 0x00, sizeof(ProtocoalInfo.DataBuf));

		memcpy(TempData,(uint8 *)&mf.MeterID,2);
		TempData[2] = mf.EquipmentType;
		memcpy(&TempData[3],mf.MeterAddr,7);
		TempData[10] = mf.BulidID;
		TempData[11] = mf.UnitID;
		memcpy(&TempData[12],(uint8 *)&mf.RoomID,2);  //������TempData[0-13]�Ѿ����ꡣ
		TempData[14] = 0;  //TempData[14]Ϊ�������ݳ���Ԥ�������渳ֵ��

		/*begin:yangfei added 2013-08-05 for add HYDROMETER*/
		if(mf.ProtocolVer == HYDROMETER775_VER||mf.ProtocolVer == ZENNER_VER||mf.ProtocolVer == LANDISGYR_VER||mf.ProtocolVer == ENGELMANN_VER)
		{	
			Err = HYDROMETER(&ProtocoalInfo,mf.ProtocolVer);
			if(Err == NO_ERR){
				CJ188_Format CJ188_Data;
			
				CJ188_Data=METER_Data_To_CJ188Format(mf.ProtocolVer,ProtocoalInfo.DataBuf,ProtocoalInfo.Length-3,&err);
				if(err==0){
				 	memcpy(&TempData[15], &CJ188_Data, sizeof(CJ188_Data));
					 ProtocoalInfo.Length = sizeof(CJ188_Data) + 3;
				}
				else{
					debug("%s %d METER_Data_To_CJ188Format err=%d\r\n",__FUNCTION__,__LINE__,err );
				 	memcpy(&TempData[15], ProtocoalInfo.DataBuf, ProtocoalInfo.Length-3);/*�ϱ�ԭʼ����*/
				}
		   
			}
			else{
				debug("%s %d  err=%d\r\n",__FUNCTION__,__LINE__,err );
			}
			
			TempData[14] = ProtocoalInfo.Length - 3;
			_ProData.MsgLength = ProtocoalInfo.Length-3 + 15;  
		}
		else{		/*end:yangfei added 2013-08-05 for add HYDROMETER*/
			for(i=0; i<RetryTimes; i++)	{
				Err = METER_MeterCommunicate(&ProtocoalInfo, &TempData[15], &TempLen);
				if(Err == NO_ERR){
					Err = METER_DELU_AnalDataFrame(&ProtocoalInfo, &TempData[15]);
					if(Err == NO_ERR){
						CJ188_Format CJ188_Data;
						
						TempData[14] = ProtocoalInfo.Length - 3;
						
						/*begin:yangfei added 2013-03-27 for CJ188 format*/
						CJ188_Data = METER_Data_To_CJ188Format(mf.ProtocolVer,ProtocoalInfo.DataBuf,ProtocoalInfo.Length-3,&err);
						memcpy(&TempData[15], &CJ188_Data, ProtocoalInfo.Length-3);
						/*end:yangfei added 2013-03-27 for CJ188 format*/
						break;
					}
				}
			}
			_ProData.MsgLength		= ProtocoalInfo.Length - 3 + 15;
		}


		//���³���
		//begin: �������¶Ⱥͷ�״̬��
		if((MeterNoBcdCheck(mf.ValveAddr) == TRUE)&&(Err == NO_ERR)){
			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Valve Addr is Valid!");
		
			err_valve = ValveContron(&mf,ReadVALVE_All,lu8DataIn,DataFrame,&DataLen_Vave);

			if(err_valve == NO_ERR){
					memcpy(&TempData[15+TempData[14]], DataFrame, DataLen_Vave);
					_ProData.MsgLength += DataLen_Vave;
			}
			else{
				memset(&TempData[15+TempData[14]], 0xee, 6);
				_ProData.MsgLength += 6;	

			}
		
		}
			else{
					LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Valve Addr is Not Valid!");
							 debug_debug(gDebugModule[TASKDOWN_MODULE],"INFO: <METER_ReadMeterDataTiming> Valve Addr is Invalid!");

					memset(&TempData[15+TempData[14]], 0xee, 6);
					_ProData.MsgLength += 6;						
		}
		
		//end: �������¶Ⱥͷ�״̬.

		
			
		OSSemPost(METERChangeChannelSem);

	}
	else{  //��ѯ�����������Ϣ��
		debug("%s:get one meter infomation fail\r\n",__FUNCTION__);
		_ProData.MsgLength		= 1;
		*_ProData.Data.pDataBack= 0x11; 				//�쳣��Ӧ,��ѯ��������Ϣ��
		Err = 0;

		_ProData.MsgType		= 0x07;
		ReadDateTime(_ProData.TimeSmybol);
		
		return Err;
	}


	if(Err != NO_ERR)		{
ERROR:		debug("%s:read one meter fail\r\n",__FUNCTION__);
			
			if(lu8MBusChannelBusy == 1){
				_ProData.MsgLength		= 1;
				*_ProData.Data.pDataBack = 0x12; //�쳣��Ӧ��MBUSͨ����æ��
			}
			else
				memcpy(_ProData.Data.pDataBack, TempData, ProtocoalInfo.Length-3+15+6);  //���س���ʧ����Ϣ��
				
			Err = 0;
	}
	else{
		memcpy(_ProData.Data.pDataBack, TempData, ProtocoalInfo.Length-3+15+6);
	}
	
	_ProData.MsgType		= 0x07;
	ReadDateTime(_ProData.TimeSmybol);
	//DISABLE_MBUS();  ʵʱ����ʱΪ������ٶȣ�����1ֻ���ſ�ͨ����
		
	return Err;

	
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_08				    							                   
**	�� ��  �� ��: ת��Э����Ϣ                                                                         			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                      
**  ��   ��   ֵ: none													                               
**	��		  ע: �˴�͸��ת�����ݣ�������Ӧ���Ƿ������ݣ���ʱ�ȴ���								   
**                ���յ����ݺ�ֱ�����ϴ���ȥ���ɣ����Խ��յ����ݽ����ж�							   
**                �����������ԶԽ��յ������ݽ����ȼ�����Э�����ݵ��ж�	                               																									
*********************************************************************************************************/
//uint8 DEAL_ProcessMsg_08(_ProtocolType &_ProData)
uint8 DEAL_ProcessMsg_31(_ProtocolType &_ProData)
{
	uint8 Err = 0x00;
	uint8 TempData[128]	= {0x00};
	uint8 TempLen		= 0x00;
	
	TempLen = _ProData.MsgLength;
	memcpy(TempData, _ProData.Data.pData, TempLen);
	
	Err = METER_MeterCommunicate_Direct(TempData, TempLen, TempData, &TempLen);
	
	_ProData.MsgLength		= TempLen;
	_ProData.MsgType		= 0x09;
	ReadDateTime(_ProData.TimeSmybol);
	
	if(Err != NO_ERR)
		{
			_ProData.MsgLength		= 1;
			*_ProData.Data.pDataBack= 0x10;					//�쳣��Ӧ
			Err = 0;
		}
	else
		{
			memcpy(_ProData.Data.pDataBack, TempData, TempLen);
		}
	
	return Err;	
}


/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_0E				    							                   
**	�� ��  �� ��: ��վ���������������Ǳ����ݣ����һ�����ݣ�                                                                      			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: SD����ÿ���������ռ��128�ֽڵĿռ�												   
**                ���е�һ���ֽ�Ϊ���ݳ���(1�ֽ�) + ����(N+11) + CS�ۼӺ�У��(1�ֽ�)
**		         �������ݸ�ʽΪ: �ȼ������ַ(7�ֽ�) + �������ݳ���(1�ֽ�) + ��������(N) + �¶�����(3�ֽ�)
**		         ����ȡ�������ݲ���ȷ����û�ж�ȡ�����ݣ���涨�������ݳ���Ϊ0�����������ݡ� 	                                                                                   **																									
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_0E(_ProtocolType &_ProData, uint8 *ControlSave)
{
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 Cs					= 0x00;
	uint8 *pTemp = _ProData.Data.pDataBack;
	uint8 *pFirst				= pTemp;
	uint8 ReadTime[6] = {0x00};
	uint8 MeterDataTemp[1024] 	= {0x00};					//��ʱ�洢SD��������
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 MeterFrameLen			= 0x00;						//�����ȡ��������֡��ȷ�ԣ�����������֡�ֽ���
	uint8 DataCounter			= 0x00;
	uint8 DataErrCounter		= 0x00;
	uint8 SendReadyFalg 		= 0x00;
	uint16 StartMeterSn			= 0x00;
	uint16 MeterSnMax			= 0x00;
	uint16 FrameDataFileLenMax	= 0x00;						//��ȡ��ǰ���õ�������ݰ�������������ֽ���
	uint16 DataBackLen			= 0x00;
	//uint32 LogicAddr			= LOGIC_ADDR_NULL;
	/*begin:yangfei modified 2012-12-24 find path*/
	char  path[] = "/2012/12/24/1530";
	/*end   :yangfei modified 2012-12-24*/
	MultiFrameMsg_0E *pMultiFrame = NULL;
	pMultiFrame = (MultiFrameMsg_0E *)ControlSave;
	CPU_SR		cpu_sr;
	
	LOG_assert_param( ControlSave == NULL );

	if(ControlSave == NULL)		while(1);
	/*begin:yangfei modified 2012-12-24 for get time*/
	OS_ENTER_CRITICAL();
	memcpy(ReadTime,gSystemTime,sizeof(gSystemTime));
	OS_EXIT_CRITICAL();
	/*end   :yangfei modified 2012-12-24*/
	//���ҵ�ǰʱ�������һ�γ�����ɵ�ʱ�䣬������
	
	//��ȡ��ǰ����������ֽ���
	//��ȡ�����б���ŵ������, ������ʵʱ���ݣ�ֱ�Ӷ�ȡ�ڴ汸�ݼ���
	OS_ENTER_CRITICAL();
	FrameDataFileLenMax = gPARA_TermPara.DebugPara.FrameLen_Max;
	MeterSnMax = gPARA_MeterNum;
	OS_EXIT_CRITICAL();
	
	//��ȡ�ϴ��ϴ����ĸ������
	StartMeterSn = pMultiFrame->StartMeterSn;

    pFirst = pTemp++;												//�Ƿ��к���֡��־����֡���ſ�����д
    DataBackLen++;
    *pTemp++ = pMultiFrame->SendPackIndex;					//��ǰ���͵İ����
    DataBackLen++;
	/*begin:yangfei modified 2012-12-24 find crruent file path*/
	GetTimeNodeFilePath(path,ReadTime,gPARA_TimeNodes);
	/*end   :yangfei modified 2012-12-24*/
    while(!SendReadyFalg)
    {
    	//ReadTime[1] = 0x30;
		//ReadTime[2] = 0x04;
		/*begin:yangfei modified 2012-12-24 */
		OSMutexPend (FlashMutex, 0, &Err);
		Res = SDReadData(path, MeterDataTemp, sizeof(MeterDataTemp),StartMeterSn*128); 
		debug_err(gDebugModule[METER_DATA],"%s %d StartMeterSn =%d MeterSnMax=%d!\r\n",__FUNCTION__,__LINE__,StartMeterSn,MeterSnMax);
    	       OSMutexPost (FlashMutex);
		/*end   :yangfei modified 2012-12-24*/
    	if(Res!=NO_ERR)				
		{
		debug("%s %d SDReadData err=%d!\r\n",__FUNCTION__,__LINE__,Err);
		goto ERROR;
		}
		DataCounter = 0;
		pDataTemp =  MeterDataTemp;

    	while(DataCounter< (1024/METER_FRAME_LEN_MAX))
    	{
			MeterFrameLen = *pDataTemp++;
			//�˴�Ҫ�жϳ����Ƿ�Ϊ��Ч
        	
    		if( (DataBackLen+MeterFrameLen) > FrameDataFileLenMax )
    			{						
    				SendReadyFalg = 1;
    				break;										//Ԥ��������жϳ����Ƿ񳬹�����
    			}
    		else
    			{
    				if(StartMeterSn < MeterSnMax)				//�ж��Ƿ��б�û����
    					{
    						/*begin :yangfei modified 2013-08-29 for �����һ����ʱ����һ������,��ֹ������ͣ��Ҫ����*/
                             if(StartMeterSn+1==MeterSnMax)
                             	{
                             	debug_info(gDebugModule[METER_DATA],"StartMeterSn+1==MeterSnMax\r\n");
							    *pFirst = 0x00;	
                             	}
					         else
					         	{
					         	*pFirst = 0x01;						//����������Ҫ����
					         	}
					       /*end :yangfei modified 2013-08-29 for �����һ����ʱ����һ������*/	
    						
    						Cs = *(pDataTemp+MeterFrameLen);
    						if( Cs == PUBLIC_CountCS(pDataTemp, MeterFrameLen) )
    							{
    								if( (MeterFrameLen>0x00) && (MeterFrameLen<0x80) )
    									{
    										memcpy(pTemp, pDataTemp, MeterFrameLen);
    										pTemp 		+= MeterFrameLen;
    										DataBackLen += MeterFrameLen;
    									}
								else
    									{
    										DataErrCounter++;
    									}
    							}
    						else
    							{
    								DataErrCounter++;
    							}
							DataCounter++;
    						pDataTemp 	= &MeterDataTemp[DataCounter*METER_FRAME_LEN_MAX];
    						StartMeterSn++;
    					}
    				else
    					{
    						*pFirst = 0x00;
    						SendReadyFalg = 1;
    						break;
    					}
    			}
    	}
    }
    pMultiFrame->MultiFlag 		= *pFirst;
    pMultiFrame->SendMeterNums 	= StartMeterSn - (pMultiFrame->StartMeterSn);
    
    //����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x10;
	ReadDateTime(_ProData.TimeSmybol);
	
	if((DataErrCounter == pMultiFrame->SendMeterNums)&&(DataErrCounter!=0))							//�������ȫ������ȷ
		{
			*_ProData.Data.pDataBack = 0x10;					//�쳣��Ӧ
			goto ERROR;
		}
	
	return NO_ERR;
	
ERROR:
	_ProData.MsgLength		= 1;
	_ProData.MsgType		= 0x10;
	*_ProData.Data.pDataBack = 0x10;					//�쳣��Ӧ

	return ERR_1;
}


/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_18				    							                   
**	�� ��  �� ��: ת��Э����Ϣ                                                                         			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: �Է��ź��¿����Ĳ��������д�Э�鶩��
** 	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_18(_ProtocolType &_ProData)
{
	uint8 Err			= 0x00;
	uint8 ForAllOrOne	= 0x00;
	uint8 *pTemp = _ProData.Data.pData;
	
	ForAllOrOne	= *pTemp++;
	
	
	if(0x0A == ForAllOrOne)
		{
			Err = DEAL_ProcessMsg_18_ForAll(_ProData);
		}
	else if(0x0B == ForAllOrOne)
		{
			Err = DEAL_ProcessMsg_18_ForOne(_ProData);
		}
	else
		{
			//Ⱥ�� ���� ��־����
		}
	
	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_18_ForOne				    							                   
**	�� ��  �� ��: ��Ե�����ת��Э����Ϣ�������������µ��¿ؿ���                                                                         			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:  	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_18_ForOne(_ProtocolType &_ProData)
{
	uint8 Err			= 0x00;
	uint8 MsgType		= 0x00;
	uint8 DataBackLen	= 0x00;
	uint8 HeatMeterAddr[7] = {0x00};
	uint8 *pTemp = _ProData.Data.pData;
	uint8 *pTempBack = _ProData.Data.pDataBack;
	uint8 *pTempAddr = pTemp;
	uint16 MeterSn		= 0x00;
	DELU_Protocol	ReadHeatMeter;
	MeterFileType   mf;
	uint8 DataFrame[METER_FRAME_LEN_MAX];
	uint8 DataLen_Vave = 0x00;
	uint8 lu8DataIn[20] = {0};  //���ڴ洢�������ݲ�����

	memset((uint8 *)&ReadHeatMeter, 0x00, sizeof(DELU_Protocol));
	*pTempBack++ = *pTemp++;
	DataBackLen++;

	MsgType		= *pTemp++;
	
	//�˴����жϵ�����Ⱥ����־ �Ƿ���ȷ
	
	*pTempBack++	= MsgType;
	DataBackLen++;

	OSSemPend(METERChangeChannelSem, OS_TICKS_PER_SEC, &Err); //����MBUSͨ���л�Ȩ��
	if(Err != OS_ERR_NONE)
	 	goto MBUS_Busy;
	
	
	switch(MsgType)
	{
		case 0x09:
		case 0x0A:
		case 0x0B:
		case 0x0C:
		case 0x0D:{
				pTempAddr = pTemp;
				break;
		}
		case 0x0E:{
				pTempAddr = pTemp + 3;
				break;
		}
		case 0x0F:{
				pTempAddr = pTemp + 6;
				break;
		}
		case 0x10:{
			pTempAddr = pTemp + 9;
			break;
		}
		case 0x11:{
			pTempAddr = pTemp + 3;
			break;
		}
		case 0x12:
		case 0x13:
		case 0x14:{
			pTempAddr = pTemp + 1;
			break;
		}
		
		default:
			break;
	}
	memcpy(HeatMeterAddr, pTempAddr, 7);
	Err = PARA_GetMeterSn(HeatMeterAddr, &MeterSn);
	if(Err!=NO_ERR)			
		{
		*pTempBack = 0x10;
		DataBackLen++;
		//return ERR_1;			//���ַû�в��ҵ����ش���
		goto READ_FAIL;
		}
	PARA_ReadMeterInfo(MeterSn, &mf);
	//���жϿ�������ַ�Ƿ���ȷ��Ч
	METER_ChangeChannel(mf.ChannelIndex);
	
	switch(MsgType)
	{
			case 0x09:									//�����¿ؿ�������
			case 0x0B:
			case 0x0C:
			case 0x0D:{
				lu8DataIn[0] = MsgType;
				Err = ValveContron(&mf,SETVALVE_CONTROLTYPE,lu8DataIn,DataFrame,&DataLen_Vave);
				
				if(Err == NO_ERR){
					*pTempBack = 0x01;
					DataBackLen++;
				}
				else{
					*pTempBack = 0x10;
					DataBackLen++;
						
				}

			}

			break;
			
		case 0x0A:									//������ʵ���¶�
			{
				Err = ValveContron(&mf,ReadVALVE_All,lu8DataIn,DataFrame,&DataLen_Vave);
				
				if(Err == NO_ERR){
					*pTempBack++ = DataFrame[0];  //����λ
					DataBackLen++;
					*pTempBack++ = DataFrame[1];
					DataBackLen++;
					*pTempBack++ = DataFrame[2];  //С��λ�̶�Ϊ0.
					DataBackLen++;
					
					debug_info(gDebugModule[TASKDOWN_MODULE],"%s Read indoor temperature ok ",__FUNCTION__);
				}
				else{
					*pTempBack = 0x10;
					DataBackLen++;
					debug_err(gDebugModule[TASKDOWN_MODULE],"%s Read indoor temperature failed ",__FUNCTION__);
				}

			}

			break;
		
		case 0x0E:							 /*���������趨�¶�*/
				{
					memcpy(lu8DataIn, pTemp, 3);
					Err = ValveContron(&mf,SETROOM_TEMP,lu8DataIn,DataFrame,&DataLen_Vave);

					if(Err == NO_ERR){
						*pTempBack = 0x01;
						DataBackLen++;
						debug_info(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature ok ",__FUNCTION__);
					}
					else{
						*pTempBack = 0x10;
						DataBackLen++;
						debug_err(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature failed ",__FUNCTION__);
					}
				}
				break;

		case 0x0F:                          /*���������������¶�*/
			{
				memcpy(lu8DataIn, pTemp, 6);
				Err = ValveContron(&mf,SETTEMP_RANGE,lu8DataIn,DataFrame,&DataLen_Vave);
				
				if(Err == NO_ERR){
					*pTempBack = 0x01;
					DataBackLen++;
					debug_info(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature ok ",__FUNCTION__);
				}
				else{
					*pTempBack = 0x10;
					DataBackLen++;
					debug_err(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature failed ",__FUNCTION__);
				}
			}

			break;

		case 0x10:{
				memcpy(lu8DataIn, pTemp, 9);
				Err = ValveContron(&mf,SETTEMP_ANDRANGE,lu8DataIn,DataFrame,&DataLen_Vave);
				
				if(Err == NO_ERR){
					*pTempBack = 0x01;
					DataBackLen++;
					debug_info(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature ok ",__FUNCTION__);
				}
				else{
					*pTempBack = 0x10;
					DataBackLen++;
					debug_err(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature failed ",__FUNCTION__);
				}

				break;
			}

		case 0x11:{
			memcpy(lu8DataIn, pTemp, 3);
			Err = ValveContron(&mf,SETVALVE_OFFSETTEMP,lu8DataIn,DataFrame,&DataLen_Vave);
			
			if(Err == NO_ERR){
				*pTempBack = 0x01;
				DataBackLen++;
			}
			else{
				*pTempBack = 0x10;
				DataBackLen++;
			}

			break;
		}
		
		case 0x12:{
			memcpy(lu8DataIn, pTemp, 1);
			Err = ValveContron(&mf,SETVALVE_HYSTEMP,lu8DataIn,DataFrame,&DataLen_Vave);
			
			if(Err == NO_ERR){
				*pTempBack = 0x01;
				DataBackLen++;
			}
			else{
				*pTempBack = 0x10;
				DataBackLen++;
			}

			break;
		}
		
		case 0x13:{
			memcpy(lu8DataIn, pTemp, 1);
			Err = ValveContron(&mf,SETTEMP_GATHERPERIOD,lu8DataIn,DataFrame,&DataLen_Vave);
			
			if(Err == NO_ERR){
				*pTempBack = 0x01;
				DataBackLen++;
			}
			else{
				*pTempBack = 0x10;
				DataBackLen++;
			}

			break;
		}
		
		case 0x14:{
			memcpy(lu8DataIn, pTemp, 1);
			Err = ValveContron(&mf,SETTEMP_UPLOADPERIOD,lu8DataIn,DataFrame,&DataLen_Vave);
			
			if(Err == NO_ERR){
				*pTempBack = 0x01;
				DataBackLen++;
			}
			else{
				*pTempBack = 0x10;
				DataBackLen++;
			}

			break;
		}
		
		default:
			break;
	}
	OSSemPost(METERChangeChannelSem);
	
READ_FAIL:
	//�ظ�����֡
	_ProData.MsgLength	= DataBackLen;
	_ProData.MsgType	= 0x19;
	ReadDateTime(_ProData.TimeSmybol);
	//DISABLE_MBUS();  ʵʱ����ʱΪ������ٶȣ�����1ֻ���ſ�ͨ����
	
	return Err;

MBUS_Busy:
	*pTempBack = 0x12;  //MBUSͨ����æ��
	DataBackLen++;
	_ProData.MsgLength	= DataBackLen;
	_ProData.MsgType	= 0x19;
	ReadDateTime(_ProData.TimeSmybol);
		
	return Err;

}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_18_ForAll				    							                   
**	�� ��  �� ��: ���Ⱥ����ת��Э����Ϣ                                                                         			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:  	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_18_ForAll(_ProtocolType &_ProData)
{
	uint8 Err			= 0x00;
	uint8 ForAllOrOne	= 0x00;
	uint8 MsgType		= 0x00;
	uint8 DataBackLen	= 0x00;
	uint8 *pTemp = _ProData.Data.pData;
	uint8 *pTempBack = _ProData.Data.pDataBack;
	
	ForAllOrOne	= *pTemp++;
	MsgType		= *pTemp++;
	
	//�˴����жϵ�����Ⱥ����־ �Ƿ���ȷ
	
	*pTempBack++	= ForAllOrOne;
	*pTempBack++	= MsgType;
	DataBackLen 	+= 2;
	
	*pTempBack++	= 0x01;						//Ⱥ��ֱ�Ӹ���ȷ��Ӧ ����
	//������Ϣ��ִ������������ʼ���������ȫ���������¿���壬���ŵȣ�
	memset(gDEAL_ReadMailBox_Msg18, 0x00, sizeof(gDEAL_ReadMailBox_Msg18));
	gDEAL_ReadMailBox_Msg18[0] = MSG_18;
	gDEAL_ReadMailBox_Msg18[1] = MsgType;
	memcpy(&gDEAL_ReadMailBox_Msg18[2], pTemp, _ProData.MsgLength-2);
	
	OSQPost(CMD_Q, gDEAL_ReadMailBox_Msg18);
	
	//�ظ�����֡
	_ProData.MsgLength	= DataBackLen+1;
	_ProData.MsgType	= 0x19;
	ReadDateTime(_ProData.TimeSmybol);
	
	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_MSG18_ForAll				    							                   
**	�� ��  �� ��: ��ȡ���м��������Ϣ                                                                         			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:  	                                                                                   																								
*********************************************************************************************************/
void DEAL_MSG18_ForAll(uint8 *pData)
{
	uint8 i,j,Err;
	uint8 MsgType	= 0x00;
	uint8 *pTemp = pData;
	uint8 Res		= 0x00;
	uint16 TempArray[METER_PER_CHANNEL_NUM] = {0x00};
	uint8 TempNums	= 0x00;
	uint16 MeterNums= 0x00;
	uint16 MeterCmplete	= 0x00;
	uint16 MeterFailure = 0x00;
	uint16 SaveCounter  = 0x00;
	uint16 DataTempLen	= 0x00;
	uint8 paratemp[20]  = {0x00};
	uint8 paralen		= 0x00;
	uint32 LogicAddr	= METER_DATA_TEMP;
	DELU_Protocol	ReadData;
	MeterFileType   mf;
	CPU_SR			cpu_sr;
	
	MsgType = *pTemp++;
	LogicAddr += 4;							//4���ֽ�Ԥ����  ��Ϣ����(1�ֽ�) + ָ������(1�ֽ�) + ���ֽ���(2�ֽ�)
	if(MsgType!=0x0A)	LogicAddr += 6;		//6���ֽ�Ԥ����  �����豸����(2Byte)+�ɹ��豸��(2Byte)+ʧ���豸��(2Byte)
	//���¿����Ĳ���
	for(i=0; i<6; i++)
	{
		//�л�Mbusͨ��
		METER_ChangeChannel(i+1);		//ͨ���� �� 1��ʼ
		
		OS_ENTER_CRITICAL();
		memcpy((uint8 *)TempArray, (uint8 *)gPARA_ConPanelChannel, gPARA_ConPanelChannelNum[i]*sizeof(uint16));
		TempNums = gPARA_ConPanelChannelNum[i];
		OS_EXIT_CRITICAL();
		MeterNums += TempNums;
		
		for(j=0; j<TempNums; j++)
		{
			PARA_ReadMeterInfo(TempArray[j], &mf);
			ReadData.MeterType 		= 0x20;
			ReadData.ControlCode 	= 0x04;
			ReadData.Length 		= 0x0B;
			ReadData.DataIdentifier = 0x22A0;
			memcpy(ReadData.DataBuf, mf.ControlPanelAddr, 7);
			switch(MsgType)
				{
					case 0x0A:
						{
							//�齨�Խ�Э�飬���¿�����¶�
							ReadData.DataIdentifier = 0x20A0;
							ReadData.DataBuf[7]		= 0x11;
							break;
						}
					case 0x09:
						{
							//�齨�Խ�Э�飬����ʹ��״̬
							ReadData.DataBuf[7]		= 0x55;
							break;
						}
					case 0x0B:
						{
							//�齨�Խ�Э�飬���ý���״̬
							ReadData.DataBuf[7]		= 0x66;
							break;
						}
					case 0x0C:
						{
							//�齨�Խ�Э�飬�����Զ�״̬
							ReadData.DataBuf[7]		= 0x77;
							break;
						}
					case 0x0D:
						{
							//�齨�Խ�Э�飬���ö�ʱ״̬
							ReadData.DataBuf[7]		= 0x88;
							break;
						}
					case 0x0E:
						{
							//�齨�Խ�Э�飬���������趨�¶�
							ReadData.DataIdentifier = 0x23A0;
							memcpy(&ReadData.DataBuf[7], pTemp, 3);
							break;
						}
					case 0x0F:
						{
							//�齨�Խ�Э�飬���������趨�¶�
							ReadData.DataIdentifier = 0x21A0;
							memcpy(&ReadData.DataBuf[7], pTemp, 6);
							break;
						}
					default:
						break;
				}
			
			Res = METER_ReadMeterDataCur(&ReadData, TempArray[j]);
//
			if(MsgType == 0x0A)
				{
					if(Res)
						{
							memcpy(&gDEAL_ReadDataTemp[10*j], mf.MeterAddr, 7);
							memset(&gDEAL_ReadDataTemp[10*j+7], 0xEE, 3);
							MeterFailure++;
						}
					else
						{
							memcpy(&gDEAL_ReadDataTemp[10*j], mf.MeterAddr, 7);
							memcpy(&gDEAL_ReadDataTemp[10*j+7], ReadData.DataBuf, 3);
							MeterCmplete++;
						}
					SaveCounter += 10;
				}
			else			//���ÿ�����ָ� ֻ���м�¼ʧ�ܱ��ַ
				{
					if(Res)
						{
							memcpy(&gDEAL_ReadDataTemp[7*j], mf.MeterAddr, 7);
							MeterFailure++;
							SaveCounter += 7;
						}
					else
						{
							MeterCmplete++;
						}
				}
		}
		OSMutexPend (FlashMutex,0,&Err);
		/*need modified*/
    	Err =AppSdWrRdBytes(LogicAddr, DataTempLen, gDEAL_ReadDataTemp, SD_WR);
    	OSMutexPost (FlashMutex);
		if(Err == true)		Err = NO_ERR;
			else
				{
					
				}
		LogicAddr += DataTempLen;
		
	}
	//�������豸������ɺ󣬸��ݳɹ���ʧ�ܼ������ܽ��豸�������
	LogicAddr	= METER_DATA_TEMP;
	paratemp[paralen++] = 0x18;
	paratemp[paralen++] = MsgType;
	if(MsgType == 0x0A)
		{
			paratemp[paralen++] = SaveCounter;
			paratemp[paralen++] = SaveCounter>>8;
		}
	else if( (MeterFailure==0x00) && (MeterCmplete!=0x00) )
		{														//ʧ����Ϊ0�����ʾ���ж��ɹ�
			paratemp[paralen++] = 0x01;
			paratemp[paralen++] = 0x00;
			paratemp[paralen++] = 0x01;
		}
	else if((MeterCmplete==0x00) && (MeterFailure!=0x00) )
		{														//�ɹ���Ϊ0�����ʾ���ж�ʧ��
			paratemp[paralen++] = 0x01;
			paratemp[paralen++] = 0x00;
			paratemp[paralen++] = 0x10;
		}
	else
		{
			paratemp[paralen++] = SaveCounter;
			paratemp[paralen++] = SaveCounter>>8;
			paratemp[paralen++] = (MeterFailure+MeterCmplete);
			paratemp[paralen++] = (MeterFailure+MeterCmplete)>>8;
			paratemp[paralen++] = MeterCmplete;
			paratemp[paralen++] = MeterCmplete>>8;
			paratemp[paralen++] = MeterFailure;
			paratemp[paralen++] = MeterFailure>>8;
		}
	OSMutexPend (FlashMutex,0,&Err);
	/*need modified*/
    Err =AppSdWrRdBytes(LogicAddr, paralen, paratemp, SD_WR);
    OSMutexPost (FlashMutex);
	if(Err)					;
	
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_1A				    							                   
**	�� ��  �� ��: ��վ��ȡȺ���¶ȿ�����Ϣ                                                                  			
**	�� ��  �� ��: uint8 *ControlSave 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:  	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_1A(_ProtocolType &_ProData, uint8 *ControlSave)
{
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 ComType				= 0x00;						//ָ������
	//uint8 *pData = _ProData.Data.pData;						//��վ�·�����Ϣ������
	uint8 *pTemp = _ProData.Data.pDataBack;					//�ϴ�����վ����Ϣ������
	uint8 *pFirst				= pTemp;
	uint8 MeterDataTemp[1024] 	= {0x00};					//��ʱ�洢SD��������
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 SendReadyFlag			= 0x00;
	uint8 LoopCounter			= 0x00;
	uint8 LoopMax				= 0x00;
	uint16 LoopLen				= 0x00;						//��Ϣ��������С���ȣ����ڶ�֡���䣬�������ϻ������ݳ���
	uint16 FrameDataFileLenMax	= 0x00;						//��ȡ��ǰ���õ�������ݰ�������������ֽ���
	uint16 DataBackLen			= 0x00;
	uint16 DataStoreLen			= 0x00;
	uint32 StartLogicAddr		= 0x00;
	//uint32 EndLogicAddr			= 0x00;

	LOG_assert_param( ControlSave == NULL );

	MultiFrameMsg_1A *pMultiFrame = NULL;
	pMultiFrame = (MultiFrameMsg_1A *)ControlSave;
	
	FrameDataFileLenMax = 450;								//��ȡ��ǰ����������ֽ���
	
	*pTemp++ = 0;											//�Ƿ��к���֡��־����֡���ſ�����д
	*pTemp++ = pMultiFrame->SendPackIndex;
    ComType  = *_ProData.Data.pData;						//ָ������
    DataBackLen += 2;
	
	//��ȡ�ϴ���ʼ��ַ
	if( (pMultiFrame->MultiFlag) == 0x00)
		{													//����ĵ�һ֡���в�����Ƿ���Ҫ��֡����
			StartLogicAddr = METER_DATA_TEMP;
			OSMutexPend (FlashMutex, 0, &Err);
			/*need modified*/
    		Res = AppSdWrRdBytes(StartLogicAddr, 512, MeterDataTemp, SD_RD);
    		OSMutexPost (FlashMutex);
    		//���Res�����ж�
			if(Res)				;

    		//��Զ�ȡ���ݵ�ǰN�����������ж�
    		if(MeterDataTemp[0] != 0x1A)		;			//��Ϣ���Ͳ���ȷ
    		if(MeterDataTemp[1] != ComType)		
			{		 //ָ�����Ͳ���ȷ
				*pTemp++ = 	MeterDataTemp[1];
				DataBackLen += 1;	
			}			
			else
			{
				*pTemp++ = 	ComType;
				DataBackLen += 1;
			}
    		
    		DataStoreLen = MeterDataTemp[2] | (MeterDataTemp[3]<<8);
    		pMultiFrame->EndReadAddr = pMultiFrame->StartReadAddr + DataStoreLen;
    		
    		if(DataStoreLen > 1)
    			{
    				if( ComType==0x0A )
    					{
    						StartLogicAddr = METER_DATA_TEMP;
    					}
    				else
    					{
    						StartLogicAddr = METER_DATA_TEMP+10;
    					}
    					
    				if( (ComType==0x0B) && (ComType==0x0C) && (ComType==0x0D) && (ComType==0x0E) && (ComType==0x0F) )
    					{												//ָ������ΪBCDEF, ��Ϊ������������
    						memcpy(pTemp, &MeterDataTemp[4], 6);		//��һ����������Ҫ��Ϣ�ϴ�
    						pTemp += 6;
    					}		
    			}
    		else
    			{	
    				//ȫ���ɹ� �� ȫ��ʧ�ܵ�����£�ֱ�ӷ���
    				*pTemp++ = MeterDataTemp[4];
					DataBackLen++;
    				//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
					_ProData.MsgLength		= DataBackLen;
					_ProData.MsgType		= 0x1F;
					ReadDateTime(_ProData.TimeSmybol);
	
					return NO_ERR;
    			}
    		
		}
	else
		{
			if( ComType==0x0A )
    			{
    				StartLogicAddr = pMultiFrame->StartReadAddr + METER_DATA_TEMP+10;
    			}
    		else
    			{
    				StartLogicAddr = pMultiFrame->StartReadAddr + METER_DATA_TEMP;
    			}
		}
    
    if( (ComType==0x0B) && (ComType==0x0C) && (ComType==0x0D) && (ComType==0x0E) && (ComType==0x0F) )
    		{												//ָ������ΪBCDEF, ��Ϊ������������
    			LoopLen = 0x07;
    		}
    if(ComType==0x0A)
    	{
    		LoopLen	= 0x0A;
    	}

    while(!SendReadyFlag)
    {
    	OSMutexPend (FlashMutex, 0, &Err);
			/*need modified*/
    	Res = AppSdWrRdBytes(StartLogicAddr, 512, MeterDataTemp, SD_RD);
    	OSMutexPost (FlashMutex);
    	//���Res�����ж�
		if(Res)				;
		LoopCounter = 0x00;
    	LoopMax		= 512 / LoopLen;
    	while(LoopCounter<LoopMax)
    	{
    		if( (DataBackLen+LoopLen) > FrameDataFileLenMax )
    			{	
    				SendReadyFlag = 1;					
    				break;										//Ԥ��������жϳ����Ƿ񳬹�����
    			}
    		else
    			{
    				if( (StartLogicAddr+DataBackLen-3) < (pMultiFrame->EndReadAddr) )				//�ж��Ƿ�������û����
    					{
    						*pFirst = 0x01;						//����������Ҫ����
    						
    						memcpy(pTemp, pDataTemp, LoopLen);
    						pTemp 		+= LoopLen;
    						DataBackLen += LoopLen;
    						pDataTemp 	+= LoopLen;
        	
    					}
    				else
    					{
    						*pFirst = 0x00;
    						SendReadyFlag = 1;		
    						break;
    					}
    			}
    		LoopCounter++;
    	}
    }
    pMultiFrame->MultiFlag 		= *pFirst;
    pMultiFrame->SendByteNums 	= DataBackLen - 3;
    
    //����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
    if( ComType==0x0A )
    	{
    		_ProData.MsgLength		= DataBackLen+10;
    	}
    else
    	{
    		_ProData.MsgLength		= DataBackLen;
    	}
	_ProData.MsgType		= 0x1B;
	ReadDateTime(_ProData.TimeSmybol);
	
	return NO_ERR;
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_1C				    							                   
**	�� ��  �� ��: �����������µķ��ſ���                                                                 			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:  	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_1C(_ProtocolType &_ProData)
{
	uint8 Err			= 0x00;
	uint8 ForAllOrOne	= 0x00;
	uint8 *pTemp = _ProData.Data.pData;
	
	ForAllOrOne	= *pTemp++;
	
	if(0x0A == ForAllOrOne)
		{
			Err = DEAL_ProcessMsg_1C_ForAll(_ProData);
		}
	else if(0x0B == ForAllOrOne)
		{
			Err = DEAL_ProcessMsg_1C_ForOne(_ProData);
		}
	else
		{
			//Ⱥ�� ���� ��־����
		}
	
	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_1C_ForOne				    							                   
**	�� ��  �� ��: ���ڵ��������ݴ���                                                                      			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:  	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_1C_ForOne(_ProtocolType &_ProData)
{
	uint8 Err			= 0x00;
	uint8 MsgType		= 0x00;
	uint8 DataBackLen	= 0x00;
	uint8 HeatMeterAddr[7] = {0x00};
	uint8 *pTemp = _ProData.Data.pData;
	uint8 *pTempBack = _ProData.Data.pDataBack;
	uint16 MeterSn		= 0x00;
	DELU_Protocol	ReadHeatMeter;
	MeterFileType	mf;
		
	uint8 DataFrame[METER_FRAME_LEN_MAX];
	uint8 DataLen_Vave = 0x00;
	uint8 lu8DataIn[20] = {0};  //���ڴ洢�������ݲ�����
		
//	uint16 ValveState		= 0x00;
	uint8 lu8workmode = 0;
	
		
	memset((uint8 *)&ReadHeatMeter, 0x00, sizeof(DELU_Protocol));
	*pTempBack++	= *pTemp++;
	DataBackLen++;
	
	MsgType 	= *pTemp++;
	
		//�˴����жϵ�����Ⱥ����־ �Ƿ���ȷ
		
	*pTempBack++	= MsgType;
	DataBackLen++;

	OSSemPend(METERChangeChannelSem, OS_TICKS_PER_SEC, &Err); //����MBUSͨ���л�Ȩ��
	if(Err != OS_ERR_NONE)
	 	goto MBUS_Busy;
	 	
		
	memcpy(HeatMeterAddr, pTemp, 7);
	Err = PARA_GetMeterSn(HeatMeterAddr, &MeterSn);
	if(Err!=NO_ERR) 		
	{
		debugX(LOG_LEVEL_ERROR,"%s PARA_GetMeterSn err!\r\n",__FUNCTION__);
		//return ERR_1; 		//���ַû�в��ҵ����ش�
		goto READ_FAIL;
	}
	PARA_ReadMeterInfo(MeterSn, &mf);
	METER_ChangeChannel(mf.ChannelIndex);
	
	memcpy(ReadHeatMeter.MeterAddr, mf.ValveAddr, 7);
	
	if(MsgType == 0xff){  /*��ȡ����״̬*/
		Err = ValveContron(&mf,ReadVALVE_All,lu8DataIn,DataFrame,&DataLen_Vave);
			
	}
	else{
		lu8DataIn[0] = MsgType;
		Err = ValveContron(&mf,SETVALVE_STATUS,lu8DataIn,DataFrame,&DataLen_Vave);
	}
		
	if(Err == NO_ERR){
		if(MsgType == 0xff){			/*��ȡ����״̬*/
			*pTempBack = DataFrame[3];
			debug_info(gDebugModule[TASKDOWN_MODULE],"ValveState = %x",lu8workmode);
				
		}
		else{
			*pTempBack = 0x01;
		}
	
		DataBackLen++;
		debug_info(gDebugModule[TASKDOWN_MODULE],"%s Send Vavle state ok ",__FUNCTION__);
	}
	else{
		
READ_FAIL:			
				*pTempBack++ = 0x10;
				DataBackLen++;
				debug_err(gDebugModule[TASKDOWN_MODULE],"%s Send Vavle state failed",__FUNCTION__);
	}

	OSSemPost(METERChangeChannelSem);
		
		//�ظ�����֡
	_ProData.MsgLength	= DataBackLen;
	_ProData.MsgType	= 0x1D;
	ReadDateTime(_ProData.TimeSmybol);
		
	//DISABLE_MBUS(); ʵʱ����ʱΪ������ٶȣ�����1ֻ���ſ�ͨ����

	return Err;


MBUS_Busy:
	*pTempBack++ = 0x12;  //MBUSͨ����æ��
	DataBackLen++;
	_ProData.MsgLength	= DataBackLen;
	_ProData.MsgType	= 0x1D;
	ReadDateTime(_ProData.TimeSmybol);
		
	return Err;
		
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_1C_ForAll			    							                   
**	�� ��  �� ��: ����Ⱥ�������ݴ���                                                                      			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:  	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_1C_ForAll(_ProtocolType &_ProData)
{
	uint8 Err			= 0x00;
	uint8 DataBackLen	= 0x00;
	uint8 MsgType		= 0x00;
	uint8 *pTemp = _ProData.Data.pData;
	uint8 *pTempBack = _ProData.Data.pDataBack;
	
	*pTempBack++	= *pTemp++;
	MsgType 		= *pTemp;
	*pTempBack++	= *pTemp++;
	DataBackLen 	+= 2;
	
	*pTempBack++	= 0x01;						//Ⱥ��ֱ�Ӹ���ȷ��Ӧ ����
	//������Ϣ��ִ������������ʼ���������ȫ���������¿���壬���ŵȣ�
	memset(gDEAL_ReadMailBox_Msg1C, 0x00, sizeof(gDEAL_ReadMailBox_Msg1C));
	gDEAL_ReadMailBox_Msg1C[0] = MSG_1C;
	gDEAL_ReadMailBox_Msg1C[1] = MsgType;
	memcpy(&gDEAL_ReadMailBox_Msg1C[2], _ProData.Data.pData, _ProData.MsgLength);
	
	OSQPost(CMD_Q, gDEAL_ReadMailBox_Msg1C);
	
	//�ظ�����֡
	_ProData.MsgLength	= DataBackLen+1;
	_ProData.MsgType	= 0x1D;
	ReadDateTime(_ProData.TimeSmybol);

	*pTempBack++ = 0x01;					//������Ӧ

	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_MSG1C_ForAll				    							                   
**	�� ��  �� ��:                                                                       			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:  	                                                                                   																								
*********************************************************************************************************/
void DEAL_MSG1C_ForAll(uint8 *pData)
{
	uint8 i,j,Err;
	uint8 *pTemp = pData;
	uint8 Res		= 0x00;
	uint8 TempNums	= 0x00;
	uint8 MsgTemp	= 0x00;
	uint8 paratemp[20] = {0x00};
	uint8 paralen	= 0x00;
	uint16 TempArray[METER_PER_CHANNEL_NUM] = {0x00};
	uint16 MeterCmplete	= 0x00;
	uint16 MeterFailure = 0x00;
	uint16 DataTempLen	= 0x00;
	uint16 SaveCounter  = 0x00;
	uint32 LogicAddr	= METER_DATA_TEMP;
	DELU_Protocol	ReadData;
	MeterFileType   mf;
	CPU_SR		cpu_sr;
	
	MsgTemp					= *pTemp;
	pTemp++;

	//�齨��³Э��  ���Ʒ���
	ReadData.MeterType 		= 0x20;
	ReadData.ControlCode 	= 0x04;
	ReadData.Length 		= 0x04;
	ReadData.DataIdentifier = 0x17A0;
	ReadData.DataBuf[0] 	= *pTemp;
	
	LogicAddr += 4;							//4���ֽ�Ԥ����  ��Ϣ����(1�ֽ�) + ָ������(1�ֽ�) + ���ֽ���(2�ֽ�)
	LogicAddr += 6;							//6���ֽ�Ԥ����  �����豸����(2Byte)+�ɹ��豸��(2Byte)+ʧ���豸��(2Byte)
	//�Է��ŵĲ���
	for(i=0; i<6; i++)
	{
		DataTempLen = 0x00;
		METER_ChangeChannel(i+1);		//ͨ���� �� 1��ʼ
		
		//�л�Mbusͨ��
		OS_ENTER_CRITICAL();
		memset(gDEAL_ReadDataTemp, 0x00, sizeof(gDEAL_ReadDataTemp));
		memcpy((uint8 *)TempArray, (uint8 *)&gPARA_ValveConChannel[i], gPARA_ValveConChannelNum[i]*sizeof(uint16));
		TempNums = gPARA_ValveConChannelNum[i];
		OS_EXIT_CRITICAL();
		
		for(j=0; j<TempNums; j++)
		{
			PARA_ReadMeterInfo(TempArray[j], &mf);
			
			//�жϷ��ŵ�ַ�Ƿ���Ч
			if(MeterNoBcdCheck(mf.ValveAddr) == TRUE)
				{
					memcpy(ReadData.MeterAddr, mf.ValveAddr, 7);
			
					Res = METER_ReadMeterDataCur(&ReadData, TempArray[j]);
					//Res = 1;
					if(Res)	//���ÿ�����ָ� ֻ���м�¼ʧ�ܱ��ַ
						{
							memcpy(&gDEAL_ReadDataTemp[7*j], mf.MeterAddr, 7);
							MeterFailure++;
							DataTempLen += 7;
							SaveCounter += 7;
						}
					else
						{
							MeterCmplete++;
						}
				}
		}
		OSMutexPend (FlashMutex,0,&Err);
		/*need modified*/
    	Err =AppSdWrRdBytes(LogicAddr, DataTempLen, gDEAL_ReadDataTemp, SD_WR);
    	OSMutexPost (FlashMutex);
		if(Err == true)		Err = NO_ERR;
			else
				{
					
				}
		LogicAddr += DataTempLen;
	}
	//�������豸������ɺ󣬸��ݳɹ���ʧ�ܼ������ܽ��豸�������
	LogicAddr	= METER_DATA_TEMP;
	paratemp[paralen++] = 0x1C;
	paratemp[paralen++] = MsgTemp;
	if( (MeterFailure==0x00) && (MeterCmplete!=0x00) )
		{														//ʧ����Ϊ0�����ʾ���ж��ɹ�
			paratemp[paralen++] = 0x01;
			paratemp[paralen++] = 0x00;
			paratemp[paralen++] = 0x01;
		}
	
	else if((MeterCmplete==0x00) && (MeterFailure!=0x00) )
		{														//�ɹ���Ϊ0�����ʾ���ж�ʧ��
			paratemp[paralen++] = 0x01;
			paratemp[paralen++] = 0x00;
			paratemp[paralen++] = 0x10;
		}
	else
		{
			paratemp[paralen++] = SaveCounter+6;
			paratemp[paralen++] = (SaveCounter+6)>>8;
			paratemp[paralen++] = (MeterFailure+MeterCmplete);
			paratemp[paralen++] = (MeterFailure+MeterCmplete)>>8;
			paratemp[paralen++] = MeterCmplete;
			paratemp[paralen++] = MeterCmplete>>8;
			paratemp[paralen++] = MeterFailure;
			paratemp[paralen++] = MeterFailure>>8;
		}
	OSMutexPend (FlashMutex,0,&Err);
	/*need modified*/
    Err =AppSdWrRdBytes(LogicAddr, paralen, paratemp, SD_WR);
    OSMutexPost (FlashMutex);
	if(Err)					;
}

/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_1E				    							                   
**	�� ��  �� ��:                                                                       			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:  	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_1E(_ProtocolType &_ProData, uint8 *ControlSave)
{
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 ComType				= 0x00;						//ָ������
	uint8 *pTemp = _ProData.Data.pDataBack;					//�ϴ�����վ����Ϣ������
	uint8 *pFirst				= pTemp;
	uint8 MeterDataTemp[600] 	= {0x00};					//��ʱ�洢SD��������
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 SendReadyFlag			= 0x00;
	uint16 LoopLen				= 0x00;						//��Ϣ��������С���ȣ����ڶ�֡���䣬�������ϻ������ݳ���
	uint16 LoopCounter			= 0x00;
	uint16 FrameDataFileLenMax	= 0x00;						//��ȡ��ǰ���õ�������ݰ�������������ֽ���
	uint16 DataBackLen			= 0x00;
	uint16 DataStoreLen			= 0x00;
	uint32 StartLogicAddr		= 0x00;
	//uint32 EndLogicAddr			= 0x00;

	LOG_assert_param( ControlSave == NULL );

	MultiFrameMsg_1E *pMultiFrame = NULL;
	pMultiFrame = (MultiFrameMsg_1E *)ControlSave;
	
	FrameDataFileLenMax = 450;								//��ȡ��ǰ����������ֽ���
	
	*pTemp++ = 0;											//�Ƿ��к���֡��־����֡���ſ�����д
	*pTemp++ = pMultiFrame->SendPackIndex;
    ComType  = *_ProData.Data.pData;						//ָ������
    DataBackLen += 2;
	
	//��ȡ�ϴ���ʼ��ַ
	if( (pMultiFrame->MultiFlag) == 0x00)
		{													//����ĵ�һ֡���в�����Ƿ���Ҫ��֡����
															//��ȡ�洢������������Ϣ
			StartLogicAddr = METER_DATA_TEMP;
			OSMutexPend (FlashMutex, 0, &Err);
			/*need modified*/
    		Res = AppSdWrRdBytes(StartLogicAddr, 512, MeterDataTemp, SD_RD);
    		OSMutexPost (FlashMutex);
    		//���Res�����ж�
			if(Res)				;

    		//��Զ�ȡ���ݵ�ǰN�����������ж�
    		if(MeterDataTemp[0] != 0x1C)		;			//��Ϣ���Ͳ���ȷ
    		if(MeterDataTemp[1] != ComType)		
			{		 //ָ�����Ͳ���ȷ
				*pTemp++ = 	MeterDataTemp[1];
				DataBackLen += 1;	
			}			
			else
			{
				*pTemp++ = 	ComType;
				DataBackLen += 1;
			}
    		
    		DataStoreLen = MeterDataTemp[2] | (MeterDataTemp[3]<<8);
    		pMultiFrame->EndReadAddr = pMultiFrame->StartReadAddr + DataStoreLen + METER_DATA_TEMP;
    		
    		if(DataStoreLen > 1)
    			{
    				StartLogicAddr = METER_DATA_TEMP+10;
    				/*if( (DataStoreLen - 6)%7 )
    					{
    						//�ֽڳ��ȿ���������
    					}
    				*/
    				
    				
    				
    			}
    		else
    			{	
    				//ȫ���ɹ� �� ȫ��ʧ�ܵ�����£�ֱ�ӷ���
    				*pTemp++ = MeterDataTemp[4];
					DataBackLen++;
    				//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
					_ProData.MsgLength		= DataBackLen;
					_ProData.MsgType		= 0x1F;
					ReadDateTime(_ProData.TimeSmybol);
	
					return NO_ERR;
    			}
		}
	else
		{
			StartLogicAddr = pMultiFrame->StartReadAddr + METER_DATA_TEMP+10;
		}
    LoopLen	= 7;

    while(!SendReadyFlag)
    {
    	OSMutexPend (FlashMutex, 0, &Err);
		/*need modified*/
    	Res = AppSdWrRdBytes(StartLogicAddr, 518, MeterDataTemp, SD_RD);
    	OSMutexPost (FlashMutex);
    	//���Res�����ж�
		if(Res)				;
		LoopCounter = 0x00;
    	
    	while(LoopCounter<74)
    	{
    		if( (DataBackLen+LoopLen) > FrameDataFileLenMax )
    			{	
    				SendReadyFlag = 1;					
    				break;										//Ԥ��������жϳ����Ƿ񳬹�����
    			}
    		else
    			{
    				if( (StartLogicAddr+DataBackLen-3) < (pMultiFrame->EndReadAddr) )				//�ж��Ƿ�������û����
    					{
    						*pFirst = 0x01;						//����������Ҫ����
    						
    						memcpy(pTemp, pDataTemp, LoopLen);
    						pTemp 		+= LoopLen;
    						DataBackLen += LoopLen;
    						pDataTemp 	+= LoopLen;
        	
    					}
    				else
    					{
    						*pFirst = 0x00;
    						SendReadyFlag = 1;		
    						break;
    					}
    			}
    		LoopCounter++;
    	}
    }
    pMultiFrame->MultiFlag 		= *pFirst;
    pMultiFrame->SendByteNums 	= DataBackLen - 3;
    
    //����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x1F;
	ReadDateTime(_ProData.TimeSmybol);
	
	return NO_ERR;
}


/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_22				    							                   
**	�� ��  �� ��: ��վ��������Ҫ�̶�ʱ�������                                                                      			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: SD����ÿ���������ռ��128�ֽڵĿռ䣬���е�һ���ֽ�Ϊ���ݳ���(1�ֽ�) + ����(N+11) + CS�ۼӺ�У��(1�ֽ�)
** 	              �������ݸ�ʽΪ: �ȼ������ַ(7�ֽ�) + �������ݳ���(1�ֽ�) + ��������(N) + �¶�����(3�ֽ�)
**				  ����ȡ�������ݲ���ȷ����û�ж�ȡ�����ݣ���涨�������ݳ���Ϊ0�����������ݡ�                                                                    																								
 **   �޸ļ�¼   :1. add input para char*path 2012-12-26  
                                       
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_22(_ProtocolType &_ProData, uint8 *ControlSave)
{

	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 Cs					= 0x00;
	uint8 *pTemp = _ProData.Data.pDataBack;
	uint8 *pFirst				= pTemp;
	uint8 ReadTime[6] 			= {0x00};
	uint8 MeterDataTemp[1024] 	= {0x00};					//��ʱ�洢SD��������
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 MeterFrameLen			= 0x00;						//�����ȡ��������֡��ȷ�ԣ�����������֡�ֽ���
	uint8 DataCounter			= 0x00;
	uint8 DataErrCounter		= 0x00;
	uint8 SendReadyFalg 		= 0x00;
	uint16 StartMeterSn			= 0x00;
	uint16 MeterSnMax			= 0x00;
	uint16 FrameDataFileLenMax	= 0x00;						//��ȡ��ǰ���õ�������ݰ�������������ֽ���
	uint16 DataBackLen			= 0x00;
	//uint32 LogicAddr			= LOGIC_ADDR_NULL;
	
	/*begin:yangfei modified 2012-12-24 find path*/
	char  DataPath[]="/2012/12/24/1530";
	char  NodePath[]="/2012/12/24/timenode";
	uint8   HexTime[6] 			= {0x00};
	/*end   :yangfei modified 2012-12-24*/
	
	LOG_assert_param( ControlSave == NULL );

	MultiFrameMsg_22 *pMultiFrame = NULL;
	pMultiFrame = (MultiFrameMsg_22 *)ControlSave;
	CPU_SR		cpu_sr;
	DataStoreParaType	History_Para;
	
	if(ControlSave == NULL)		while(1);
	
	//��ȡʵʱ�Ŀ��Ʊ�������
	memcpy(ReadTime, _ProData.Data.pData, 6);
	//��ȡ��ǰ����������ֽ���
	//��ȡ�����б���ŵ������, ������ʵʱ���ݣ�ֱ�Ӷ�ȡ�ڴ汸�ݼ���
	OS_ENTER_CRITICAL();
	FrameDataFileLenMax = gPARA_TermPara.DebugPara.FrameLen_Max;
	OS_EXIT_CRITICAL();
	//��ȡ�ϴ��ϴ����ĸ������
	StartMeterSn = pMultiFrame->StartMeterSn;
	/*begin:yangfei modified 2012-12-24 find path*/
	BcdTimeToHexTime(ReadTime,HexTime);
	//GetTimeNode(HexTime,TimeNodes);
	//GetTimeNodeFilePath(DataPath,HexTime,TimeNodes);
	GetFilePath(NodePath, HexTime, ARRAY_DAY);  /*��ȡ�̶���timenode path*/
	OSMutexPend (FlashMutex, 0, &Err);
	Res = SDReadData(NodePath, (uint8 *)&History_Para, sizeof(History_Para),0);
	OSMutexPost (FlashMutex);
	//GetTimeNodeFilePath(DataPath,HexTime,History_Para.TimeNode);
	GetTimeNodeFilePath_Forward(DataPath,HexTime,&History_Para);  //���Ҹ���ʱ���ǰһ����ʱ�����ݴ��·����
	/*end   :yangfei modified 2012-12-24*/
    //���Res�����ж�
	if(Res!=NO_ERR)	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
    	}
	
	MeterSnMax = History_Para.MeterNums;
	
    pFirst = pTemp++;										//�Ƿ��к���֡��־����֡���ſ�����д
    DataBackLen++;
    *pTemp++ = pMultiFrame->SendPackIndex;					//��ǰ���͵İ����
    DataBackLen++;
	/*begin:yangfei modified 2013-1-4 for add time*/
	memcpy(pTemp, ReadTime, 6);
	pTemp += 6;
	DataBackLen += 6;
	/*end:yangfei modified 2013-1-4 */
	
    while(!SendReadyFalg)
    {
	/*begin:yangfei modified 2012-12-24 find path*/
	OSMutexPend (FlashMutex, 0, &Err);
	Res = SDReadData(DataPath, MeterDataTemp, sizeof(MeterDataTemp),StartMeterSn*128); 
	debug_info(gDebugModule[METER_DATA],"%s %d StartMeterSn =%d MeterSnMax=%d!\r\n",__FUNCTION__,__LINE__,StartMeterSn,MeterSnMax);
	OSMutexPost (FlashMutex);
	if(Res!=NO_ERR)
    	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
		    debug_err(gDebugModule[METER_DATA],"ERROR: %s %d SDSaveData Error Res = %d !",__FUNCTION__,__LINE__,Res );
			goto ERROR;
    	}
	/*end  :yangfei modified 2012-12-24 find path*/
    	//���Res�����ж�
		DataCounter = 0;
		pDataTemp =  MeterDataTemp;

    	while(DataCounter< (1024/METER_FRAME_LEN_MAX))
    	{
			MeterFrameLen = *pDataTemp++;
			//�˴�Ҫ�жϳ����Ƿ�Ϊ��Ч
        	
    		if( (DataBackLen+MeterFrameLen) > FrameDataFileLenMax )
    			{						
    				SendReadyFalg = 1;
    				break;										//Ԥ��������жϳ����Ƿ񳬹�����
    			}
    		else
    			{
					/*begin :yangfei modified 2012-01-14 for StartMeterSn��0��ʼ�������ഫ����һ��*/
						#if 0
						if(StartMeterSn <= MeterSnMax)				//�ж��Ƿ��б�û����
						#endif
						if(StartMeterSn < MeterSnMax)
    				/*end  :yangfei modified 2012-01-14*/
    					{
    					/*begin :yangfei modified 2013-08-29 for �����һ����ʱ����һ������,��ֹ������ͣ��Ҫ����*/
                                            if(StartMeterSn+1==MeterSnMax)
                             	             {
                             	                 debug_info(gDebugModule[METER_DATA],"StartMeterSn+1==MeterSnMax\r\n");
							    *pFirst = 0x00;	
                             	             }
					         else
					         	{
					         	*pFirst = 0x01;						//����������Ҫ����
					         	}
					  /*end :yangfei modified 2013-08-29 for �����һ����ʱ����һ������*/	
    						
    						Cs = *(pDataTemp+MeterFrameLen);
    						if( Cs == PUBLIC_CountCS(pDataTemp, MeterFrameLen) )
    							{
    								if( (MeterFrameLen>0x00) && (MeterFrameLen<0x80) )
    									{
    										memcpy(pTemp, pDataTemp, MeterFrameLen);
    										pTemp 		+= MeterFrameLen;
    										DataBackLen += MeterFrameLen;
    									}
							      else
    									{
    										DataErrCounter++;
    									}
    							}
    						else
    							{
    								DataErrCounter++;
    							}
							DataCounter++;
    						pDataTemp 	= &MeterDataTemp[DataCounter*METER_FRAME_LEN_MAX];
    						StartMeterSn++;
    					}
    				else
    					{
    						*pFirst = 0x00;
    						SendReadyFalg = 1;
    						break;
    					}
    			}
    	}
    }
    pMultiFrame->MultiFlag 		= *pFirst;
    pMultiFrame->SendMeterNums 	= StartMeterSn - (pMultiFrame->StartMeterSn);
    
    debug("StartMeterSn =%d  pMultiFrame->StartMeterSn=%d\r\n ",StartMeterSn,pMultiFrame->StartMeterSn);
    
    //����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x23;
	ReadDateTime(_ProData.TimeSmybol);
	
	if((DataErrCounter == pMultiFrame->SendMeterNums)&&(DataErrCounter!=0))			//�������ȫ������ȷ
		{
			_ProData.MsgLength		= 1;
			*_ProData.Data.pDataBack = 0x10;					//�쳣��Ӧ
			LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d  Msg_22 Error!",__FUNCTION__,__LINE__ );
			debug_info(gDebugModule[METER_DATA],"WARNING: %s %d  Msg_22 Error DataErrCounter =%d!",__FUNCTION__,__LINE__ ,DataErrCounter);
			goto ERROR;
		}
	
	return NO_ERR;

ERROR:
	_ProData.MsgType		= 0x23;
	_ProData.MsgLength		= 1;
	*_ProData.Data.pDataBack = 0x10;					//�쳣��Ӧ

	return ERR_1;
}



/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_3C				    							                   
**	�� ��  �� ��: ��վ��������Ҫָ��ʱ���ָ����ŵ���ʷ���ݡ�                                                                   			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: SD����ÿ���������ռ��128�ֽڵĿռ䣬���е�һ���ֽ�Ϊ���ݳ���(1�ֽ�) + ����(N+11) + CS�ۼӺ�У��(1�ֽ�)
** 	              �������ݸ�ʽΪ: �ȼ������ַ(7�ֽ�) + �������ݳ���(1�ֽ�) + ��������(N) + �¶�����(3�ֽ�)
**				  ����ȡ�������ݲ���ȷ����û�ж�ȡ�����ݣ���涨�������ݳ���Ϊ0�����������ݡ�                                                                    																								
 **   �޸ļ�¼   :
                                       
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_3C(_ProtocolType &_ProData)
{
	uint16 i = 0;
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 Cs					= 0x00;
	uint8 *pTemp = _ProData.Data.pDataBack;
	uint8 ReadTime[6] 			= {0x00};
	uint8 MeterDataTemp[128] 	= {0x00};					//��ʱ�洢SD��������
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 MeterFrameLen			= 0x00;						//�����ȡ��������֡��ȷ�ԣ�����������֡�ֽ���
	uint16 MeterSnMax			= 0x00;
	uint16 DataBackLen			= 0x00;
	//uint32 LogicAddr			= LOGIC_ADDR_NULL;
	
	/*begin:yangfei modified 2012-12-24 find path*/
	char  DataPath[]="/2012/12/24/1530";
	char  NodePath[]="/2012/12/24/timenode";
	uint8   HexTime[6] 			= {0x00};
	uint8 lu8MeterNum[7] = {0};
	uint8 lu8RecMeterNum[7] = {0};  //���ڴ洢����������Ŀ�ı�š�
	/*end   :yangfei modified 2012-12-24*/
	
	CPU_SR		cpu_sr;
	DataStoreParaType	History_Para;
	
	
	//��ȡʵʱ�Ŀ��Ʊ�������
	memcpy(ReadTime, _ProData.Data.pData, 6);  //ȡ��Ŀ��ʱ�䡣
	memcpy(lu8RecMeterNum, _ProData.Data.pData+6, 7);  //ȡ��Ŀ����ַ��


	/*begin:yangfei modified 2012-12-24 find path*/
	BcdTimeToHexTime(ReadTime,HexTime);
	GetFilePath(NodePath, HexTime, ARRAY_DAY);  /*��ȡ�̶���timenode path*/
	OSMutexPend (FlashMutex, 0, &Err);
	Res = SDReadData(NodePath, (uint8 *)&History_Para, sizeof(History_Para),0);
	OSMutexPost (FlashMutex);
	//���Res�����ж�
	if(Res != NO_ERR)	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
			goto ERROR;
	}
	
	//GetTimeNodeFilePath(DataPath,HexTime,History_Para.TimeNode);
	GetTimeNodeFilePath_Forward(DataPath,HexTime,&History_Para);  //���Ҹ���ʱ���ǰһ����ʱ�����ݴ��·����
	/*end   :yangfei modified 2012-12-24*/

	MeterSnMax = History_Para.MeterNums;
	
    *pTemp++ = 0;				//�Ƿ��к���֡��־���޺������̶�Ϊ0.
    DataBackLen++;
    *pTemp++ = 0;					//��ǰ���͵İ����,�̶�Ϊ0 ��
    DataBackLen++;
	/*begin:yangfei modified 2013-1-4 for add time*/
	memcpy(pTemp, ReadTime, 6);
	pTemp += 6;
	DataBackLen += 6;
	/*end:yangfei modified 2013-1-4 */

	for(i=0;i<MeterSnMax;i++){
		OSMutexPend (FlashMutex, 0, &Err);
		Res = SDReadData(DataPath, MeterDataTemp, sizeof(MeterDataTemp),i*128); 
		OSMutexPost (FlashMutex);
		if(Res != NO_ERR)	{
				LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
		}
		
		memcpy(lu8MeterNum, &MeterDataTemp[4], 7);  //ȡ�����ַ��
		if(strcmp((char *)lu8MeterNum,(char *)lu8RecMeterNum) == 0){
			MeterFrameLen = *pDataTemp++;
			Cs = *(pDataTemp+MeterFrameLen);
			if( Cs == PUBLIC_CountCS(pDataTemp, MeterFrameLen) )  {
				if( (MeterFrameLen>0x00) && (MeterFrameLen<0x80) )	{
						memcpy(pTemp, pDataTemp, MeterFrameLen);
						pTemp		+= MeterFrameLen;
						DataBackLen += MeterFrameLen;

						break;
				}
				else{
					goto ERROR;	
				}
			}

		}

		if(i == (MeterSnMax-1)){  //û�в�ѯ����
			goto ERROR;

		}

		
	}
	
    
    //����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x3D;
	ReadDateTime(_ProData.TimeSmybol);
	
	return NO_ERR;

ERROR:
	_ProData.MsgType		= 0x3D;
	_ProData.MsgLength		= 1;
	*_ProData.Data.pDataBack = 0x10;					//�쳣��Ӧ

	return ERR_1;
}





/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_3E				    							                   
**	�� ��  �� ��: ��վ������������������ָ�                                                                   			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**   �޸ļ�¼   :
                                       
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_3E(_ProtocolType &_ProData)
{
	uint8 lu8SendMsg[2]		= {0x00};
	
	if((gu8ReadAllMeterFlag!=0) && (gu8ReadAllMeterFlag!=1)){  //��ֹ����
		gu8ReadAllMeterFlag = 0;
	}
	

	ReadDateTime(_ProData.TimeSmybol);

	
	if(gu8ReadAllMeterFlag == 1){//����ȫ����
		_ProData.MsgType		= 0x3F;
		_ProData.MsgLength		= 1;
		*_ProData.Data.pDataBack = 0x12;	//�쳣��Ӧ,����ȫ����
		return ERR_1;

	}
	else{
		lu8SendMsg[0] =  TIMING_ALL;
		OSQPost(CMD_Q, (void*)lu8SendMsg);

		OSTimeDlyHMSM(0,0,1,0);
		
		_ProData.MsgType		= 0x3F;
		_ProData.MsgLength		= 1;
		*_ProData.Data.pDataBack = 0x01;	//

		return NO_ERR;


	}
	

}



/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_40				    							                   
**	�� ��  �� ��: ��վ��������Ҫ�̶�ʱ�������                                                                      			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע:                                                                																								
**   �޸ļ�¼   :
1. add input para char*path 2012-12-26  
2.  yangfei added 20140219  for ֧���ȷ����ʱ��ͨ�������

    sd�������ݱ����ʽ:(ÿ��Ϊ128�ֽ�)
    0:�ȱ�����
    1:�ȷ����򷧿�����
    2:�ȷ����򷧿�����
    3:�ȷ����򷧿�����
    ...

    �ӵ�1�鿪ʼ��ȡ
	�����豸����
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_40(_ProtocolType &_ProData, uint8 *ControlSave)
{
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 Cs					= 0x00;
	uint8 *pTemp = _ProData.Data.pDataBack;
	uint8 *pFirst				= pTemp;
	uint8 ReadTime[6] 			= {0x00};
	uint8 MeterDataTemp[1024] 	= {0x00};					//��ʱ�洢SD��������
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 MeterFrameLen			= 0x00;						//�����ȡ��������֡��ȷ�ԣ�����������֡�ֽ���
	uint8 DataCounter			= 0x00;
	uint8 DataErrCounter		= 0x00;
	uint8 SendReadyFalg 		= 0x00;
	uint16 StartMeterSn			= 0x00;
	uint16 MeterSnMax			= 0x00;
	uint16 FrameDataFileLenMax	= 0x00;						//��ȡ��ǰ���õ�������ݰ�������������ֽ���
	uint16 DataBackLen			= 0x00;

	char  DataPath[]="/2012/12/24/1530";
	char  NodePath[]="/2012/12/24/timenode";
	uint8   HexTime[6] 			= {0x00};
	
	LOG_assert_param( ControlSave == NULL );

	MultiFrameMsg_40 *pMultiFrame = NULL;
	pMultiFrame = (MultiFrameMsg_40 *)ControlSave;
	CPU_SR		cpu_sr;
	DataStoreParaType	History_Para;
	
	if(ControlSave == NULL)		while(1);
	
	//��ȡʵʱ�Ŀ��Ʊ�������
	memcpy(ReadTime, _ProData.Data.pData, 6);
	//��ȡ��ǰ����������ֽ���
	//��ȡ�����б���ŵ������, ������ʵʱ���ݣ�ֱ�Ӷ�ȡ�ڴ汸�ݼ���
	OS_ENTER_CRITICAL();
	FrameDataFileLenMax = gPARA_TermPara.DebugPara.FrameLen_Max;
	OS_EXIT_CRITICAL();
	//��ȡ�ϴ��ϴ����ĸ������
	StartMeterSn = pMultiFrame->StartMeterSn;
	/*begin:yangfei modified 2012-12-24 find path*/
	BcdTimeToHexTime(ReadTime,HexTime);
	
	GetFilePath(NodePath, HexTime, ARRAY_DAY);  /*��ȡ�̶���timenode path*/
	OSMutexPend (FlashMutex, 0, &Err);
	Res = SDReadData(NodePath, (uint8 *)&History_Para, sizeof(History_Para),0);
	OSMutexPost (FlashMutex);
	if(Res!=NO_ERR)
	{
		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
	}
	GetTimeNodeFilePath(DataPath,HexTime,History_Para.TimeNode);
	
	MeterSnMax = History_Para.MeterNums;
	
    pFirst = pTemp;										//�Ƿ��к���֡��־����֡���ſ�����д

	pTemp += sizeof(MultiFrameMsg_40)-2;/*Ԥ������ͷλ��*/
		
	DataBackLen += sizeof(MultiFrameMsg_40)-2;
	
    while(!SendReadyFalg)
    {
	OSMutexPend (FlashMutex, 0, &Err);
	Res = SDReadData(DataPath, MeterDataTemp, sizeof(MeterDataTemp),StartMeterSn*128); 
	debug_info(gDebugModule[METER_DATA],"%s %d StartMeterSn =%d MeterSnMax=%d!\r\n",__FUNCTION__,__LINE__,StartMeterSn,MeterSnMax);
	OSMutexPost (FlashMutex);
	if(Res!=NO_ERR)
    	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
		    debug_err(gDebugModule[METER_DATA],"ERROR: %s %d SDSaveData Error Res = %d !",__FUNCTION__,__LINE__,Res );
			goto ERROR;
    	}
    	//���Res�����ж�
		DataCounter = 0;
		pDataTemp =  MeterDataTemp;

    	while(DataCounter< (1024/METER_FRAME_LEN_MAX))
    	{
			MeterFrameLen = *pDataTemp++;
    		if( (DataBackLen+MeterFrameLen) > FrameDataFileLenMax )/*�˴�Ҫ�жϳ����Ƿ�Ϊ��Ч*/
    			{						
    				SendReadyFalg = 1;
    				break;										//Ԥ��������жϳ����Ƿ񳬹�����
    			}
    		else
    			{
						if(StartMeterSn < MeterSnMax)
    					{
    					/*begin :yangfei modified 2013-08-29 for �����һ����ʱ����һ������,��ֹ������ͣ��Ҫ����*/
                              if(StartMeterSn+1==MeterSnMax)
                             	{
                             	debug_info(gDebugModule[METER_DATA],"StartMeterSn+1==MeterSnMax\r\n");
							    *pFirst = 0x00;	
                             	}
					         else
					         	{
					         	*pFirst = 0x01;						//����������Ҫ����
					         	}
					  /*end :yangfei modified 2013-08-29 for �����һ����ʱ����һ������*/	
    						
    						Cs = *(pDataTemp+MeterFrameLen);
    						if( Cs == PUBLIC_CountCS(pDataTemp, MeterFrameLen) )
    							{
    								if( (MeterFrameLen>0x00) && (MeterFrameLen<0x80) )
    									{
    										memcpy(pTemp, pDataTemp, MeterFrameLen);
    										pTemp 		+= MeterFrameLen;
    										DataBackLen += MeterFrameLen;
    									}
							      else
    									{
    										DataErrCounter++;
    									}
    							}
    						else
    							{
    								DataErrCounter++;
    							}
							DataCounter++;
    						pDataTemp 	= &MeterDataTemp[DataCounter*METER_FRAME_LEN_MAX];
    						StartMeterSn++;
    					}
    				else
    					{
    						*pFirst = 0x00;
    						SendReadyFalg = 1;
    						break;
    					}
    			}
    	}
    }
	
	memcpy(pMultiFrame->time, ReadTime, 6);
	pMultiFrame->device_type = gPARA_TermPara.DeviceType;
	pMultiFrame->MultiFlag 		= *pFirst;
    pMultiFrame->SendMeterNums 	= StartMeterSn - (pMultiFrame->StartMeterSn);

	memcpy(pFirst, pMultiFrame, sizeof(MultiFrameMsg_40)-2);/*��������ͷ*/
    
    debug("StartMeterSn =%d  pMultiFrame->StartMeterSn=%d\r\n ",StartMeterSn,pMultiFrame->StartMeterSn);
    
    //����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x41;
	ReadDateTime(_ProData.TimeSmybol);
	
	if((DataErrCounter == pMultiFrame->SendMeterNums)&&(DataErrCounter!=0))			//�������ȫ������ȷ
		{
			_ProData.MsgLength		= 1;
			*_ProData.Data.pDataBack = 0x10;					//�쳣��Ӧ
			LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d  Msg_22 Error!",__FUNCTION__,__LINE__ );
			debug_info(gDebugModule[METER_DATA],"WARNING: %s %d  Msg_22 Error DataErrCounter =%d!",__FUNCTION__,__LINE__ ,DataErrCounter);
			goto ERROR;
		}
	
	return NO_ERR;

ERROR:
	_ProData.MsgType		= 0x41;
	_ProData.MsgLength		= 1;
	*_ProData.Data.pDataBack = 0x10;					//�쳣��Ӧ

	return ERR_1;
}







/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_42			    							                   
**	�� ��  �� ��:������ַ�����Э��� ͸������ָ�                                                                     			
**	�� ��  �� ��: 
**	�� ��  �� ��: none											                                      
**  ��   ��   ֵ: none													                               
**	��		  ע:                              																									
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_42(_ProtocolType &_ProData)	
{
	uint8 Err = 0x01;
	uint8 lu8OvertimeCnt = 0;
	uint8 DataFrame[256] = {0};
	uint8 DataLen_Vave = 0;
	uint8 lu8DevType = 0;
	uint8 lu8channel = 0;
	uint8 lu8ProtocolType = 0;
	uint8 lu8ForAllorOne = 0;
		
	FeedTaskDog();

	gu8MBusShortTime = 0;

	if(gu8ReadAllMeterFlag == 0){//ֻ���ڷ�ȫ��״̬��͸����������ʾ��æ��
		lu8ForAllorOne = _ProData.Data.pData[0]; 
		if(lu8ForAllorOne == FORALL){  //Ⱥ��
			Err = Communicate_ForAll(_ProData.Data.pData);

			_ProData.MsgLength		= 1; 
			_ProData.MsgType		= 0x43;
			*_ProData.Data.pDataBack = 0x01;  //Ⱥ�������豸�޷��أ��������̶�����λ�����سɹ���
			Err = ReadDateTime(_ProData.TimeSmybol);
			
		}
		else{	//����

		//�����л�����Ӧͨ����
			lu8channel = _ProData.Data.pData[2];
			METER_ChangeChannel(lu8channel);


		//��θ����豸���ͣ��趨��ȷ�Ĵ��ڲ�����
			lu8DevType =  _ProData.Data.pData[1];
			lu8ProtocolType = _ProData.Data.pData[3];


			if(lu8DevType == 0x20){  //�ȱ�

				(*METER_ComParaSetArray[gMETER_Table[lu8ProtocolType][0]])();
			}
			else if(lu8DevType == 0xb0){  //��������
				(*METER_ComParaSetArray[gVALVE_Table[lu8ProtocolType][0]])();  //���÷��Ŷ�Ӧ���ڲ�����
			}
			else{
				//����Ҫʱ���䡣
			}

	loop:	DataLen_Vave =  _ProData.Data.pData[5];  
			memcpy(DataFrame, &(_ProData.Data.pData[6]),DataLen_Vave);
					
			//Err =  ReadData_Communicate(DataFrame, &DataLen_Vave);  //�����ȱ���͸������������������·����ա�

			if(lu8DevType == HEAT_METER_TYPE)
					Err =  ReadData_Communicate(DataFrame, &DataLen_Vave); 
			else if(lu8DevType == TIME_ON_OFF_AREA_TYPE){
					if(lu8ProtocolType == 0x01)   //���ַ���
						Err =  Yilin_Valve_ReceiveFrame(DataFrame, &DataLen_Vave);
							
					else
						Err = ReadData_Communicate(DataFrame, &DataLen_Vave);
			
			}
			else{
			
			}
			
				if(Err != OS_ERR_NONE){
					if(lu8OvertimeCnt < 2){ 
							lu8OvertimeCnt++;
							goto loop;
					}
					else{
						lu8OvertimeCnt = 0;
												
						_ProData.MsgLength		= 2; 
						_ProData.MsgType		= 0x43;
						*_ProData.Data.pDataBack = 0x10;
						*(_ProData.Data.pDataBack+1) = gu8MBusShortTime;
						Err = ReadDateTime(_ProData.TimeSmybol);
											
					}
				}
				else{  //if(err != OS_ERR_NONE)
					lu8OvertimeCnt = 0;
										
					_ProData.MsgLength		= DataLen_Vave + 1; 
					DataFrame[DataLen_Vave] = gu8MBusShortTime;
					memcpy(_ProData.Data.pDataBack,DataFrame,DataLen_Vave);
					_ProData.MsgType		= 0x43;
					Err = ReadDateTime(_ProData.TimeSmybol);
																
				}
				
				//METER_ChangeChannel(7);//�Ͽ�MBUSͨ����
			}
		}
		else{
			_ProData.MsgLength		= 1; 
			_ProData.MsgType		= 0x43;
			*_ProData.Data.pDataBack = 0x12;  //��ʾ��æ��
			Err = ReadDateTime(_ProData.TimeSmybol);

		}

		gu8MBusShortTime = 0; //�ڴ˴����˱��������
	
		return Err; 
}


/*****************************************************************************************
**								End of File
*****************************************************************************************/

