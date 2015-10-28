/*******************************************Copyright (c)*******************************************
**									山东华宇空间技术公司(西安分部)                                **                            
**  文   件   名: ReadMeterdata.cpp																  **
**  创   建   人: 勾江涛																		  **
**	版   本   号: 0.1																		      **
**  创 建  日 期: 2012年9月20日 													     	      **
**  描        述: 读取计量器数据									      						  **
**	修 改  记 录:   							  												  **
**  备		  注: 阀控和温控的协议订的不好，造成程序书写逻辑较为麻烦!费神!
****************************************************************************************************/
/********************************************** include *********************************************/
#include <includes.h>
#include "app_flashmem.h"
#include "app_down.h"
#include "app_up.h"
#include "Valve.h"



extern"C"
{
#include "ModBus.h" //测试modbus  ,林晓彬添加
}
/********************************************** static *********************************************/
static uint8 gDEAL_ReadMailBox_Msg18[20] = {0x00};
static uint8 gDEAL_ReadMailBox_Msg1C[20] = {0x00};
static uint8 gDEAL_ReadDataTemp[10*METER_PER_CHANNEL_NUM] = {0x00};

/********************************************** global **********************************************/
void* CmdMsgQ[10];               	//定义消息指针数组, 最多存储10个消息
OS_EVENT *CMD_Q; 

extern uint8 gDownCommDev485;



/********************************************************************************************************
**  函 数  名 称: test_proMsg_04				    							                       
**	函 数  功 能: 测试协议信息                                                                         			
**	输 入  参 数: _ProtocolType &_ProData   							         			 	       
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注: 	                                                                                   																									
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
	
	ProtocolVer = *pTemp++;						//协议类型
	Channel = *pTemp++;
	
	if(ProtocolVer>PROTOCOL_MAX)	  ProtocolVer = 0;
	/*begin:yangfei added 2013-11-2 for 判断485 热表必须为7通道*/
	if(ProtocolVer==4&&Channel!=7)
		{
		debug("%s:485 meter channel!=7 \r\n",__FUNCTION__);
		goto ERROR;
		}
	/*end:yangfei added 2013-11-2 for 判断485 热表必须为7通道*/
	/*begin:yangfei added 2013-11-17 for 定点抄表标志，定点抄表的时候不允许实时抄表，防止通道切换*/
	OSSemPend(METERChangeChannelSem, OS_TICKS_PER_SEC, &err); //申请MBUS通道切换权利
	if(err!=OS_ERR_NONE)
		{
		debug("%s:nwo timing  read meter!\r\n",__FUNCTION__);
		goto ERROR;
		}
	/*end:yangfei added 2013-11-17 for 定点抄表标志，定点抄表的时候不允许实时抄表，防止通道切换*/
	(*METER_ComParaSetArray[gMETER_Table[ProtocolVer][0]])();
	/*begin:yangfei added for support 485 meter*/
      gDownCommDev485 = gMETER_Table[ProtocolVer][3];
       /*end:yangfei added for support 485 meter*/
	METER_ChangeChannel(Channel);

	//填写相应的 热计量表通信协议 结构体
	ProtocoalInfo.PreSmybolNum  = gMETER_Table[ProtocolVer][2];
	ProtocoalInfo.MeterType 	= 0x20;

	memcpy(AddrTemp, pTemp, 7);
	if(ProtocolVer == 2)
	{							//若为丹佛斯表，则将表地址前5位为FFFFF
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
			 memcpy(&TempData[8], ProtocoalInfo.DataBuf, ProtocoalInfo.Length-3);/*上报原始数据*/
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
			*_ProData.Data.pDataBack= 0x10;					//异常回应
			Err = 0;
		}
	else
		{
			memcpy(_ProData.Data.pDataBack, TempData, ProtocoalInfo.Length-3+8);
		}
	_ProData.MsgType		= 0x05;
	ReadDateTime(_ProData.TimeSmybol);
	//DISABLE_MBUS();   实时抄表时为了提高速度，抄完1只不放开通道。
	
	return Err;
	
}

/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_06				    							                       
**	函 数  功 能: 根据计量点抄表、阀信息并返回。                                                                    			
**	输 入  参 数: _ProtocolType &_ProData   							         			 	       
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注: 	                                                                                   																									
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
	uint8 lu8DataIn[20];  //阀控抄读使用，传递参数。
	uint8 DataFrame[50];  //阀控抄读用。
	uint8 DataLen_Vave;   //阀控抄读用。
	//uint8 addr[7] = {0xFF, 0xFF, 0xF1, 0x22, 0x65, 0x48, 0x25};
	uint8 lu8MBusChannelBusy = 0;//MBUS通道是否忙标志，0-不忙，1-忙。
	DELU_Protocol	ProtocoalInfo;

	MeterFileType	mf;
	
	lu16MeterID = *((uint16 *)pTemp);			//获取MeterID.
	pTemp += 2;

	//根据lu16MeterID 查询表信息。
	Err = PARA_GetMeterSn_ByMeterID(&lu16MeterID,&lu16MeterSn);

	if(Err == NO_ERR){
		PARA_ReadMeterInfo(lu16MeterSn, &mf);
		
		/*begin:yangfei added 2013-11-17 for 定点抄表标志，定点抄表的时候不允许实时抄表，防止通道切换*/
		OSSemPend(METERChangeChannelSem, OS_TICKS_PER_SEC, &err); //申请MBUS通道切换权利
		if(err!=OS_ERR_NONE){
			lu8MBusChannelBusy = 1;//标记MBUS通道被占用，忙。
			debug("%s:nwo timing  read meter!\r\n",__FUNCTION__);
			goto ERROR;
		}
		/*end:yangfei added 2013-11-17 for 定点抄表标志，定点抄表的时候不允许实时抄表，防止通道切换*/

		METER_ChangeChannel(mf.ChannelIndex);  //切换到对应通道
		(*METER_ComParaSetArray[gMETER_Table[mf.ProtocolVer][0]])();  //设置抄热表串口参数


		/*begin:yangfei added for support 485 meter*/
		 gDownCommDev485 = gMETER_Table[mf.ProtocolVer][3];
	    /*end:yangfei added for support 485 meter*/
		
		
		//填写相应的 热计量表通信协议 结构体
		ProtocoalInfo.PreSmybolNum	= gMETER_Table[mf.ProtocolVer][2];
		ProtocoalInfo.MeterType 	= 0x20;
	
		if(mf.ProtocolVer == 2){
								//若为丹佛斯表，则将表地址前5位为FFFFF
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
		memcpy(&TempData[12],(uint8 *)&mf.RoomID,2);  //到这里TempData[0-13]已经用完。
		TempData[14] = 0;  //TempData[14]为热量数据长度预留，后面赋值。

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
				 	memcpy(&TempData[15], ProtocoalInfo.DataBuf, ProtocoalInfo.Length-3);/*上报原始数据*/
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


		//以下抄阀
		//begin: 读室内温度和阀状态。
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
		
		//end: 读室内温度和阀状态.

		
			
		OSSemPost(METERChangeChannelSem);

	}
	else{  //查询不到表基础信息。
		debug("%s:get one meter infomation fail\r\n",__FUNCTION__);
		_ProData.MsgLength		= 1;
		*_ProData.Data.pDataBack= 0x11; 				//异常回应,查询不到表信息。
		Err = 0;

		_ProData.MsgType		= 0x07;
		ReadDateTime(_ProData.TimeSmybol);
		
		return Err;
	}


	if(Err != NO_ERR)		{
ERROR:		debug("%s:read one meter fail\r\n",__FUNCTION__);
			
			if(lu8MBusChannelBusy == 1){
				_ProData.MsgLength		= 1;
				*_ProData.Data.pDataBack = 0x12; //异常回应，MBUS通道正忙。
			}
			else
				memcpy(_ProData.Data.pDataBack, TempData, ProtocoalInfo.Length-3+15+6);  //返回抄表失败信息。
				
			Err = 0;
	}
	else{
		memcpy(_ProData.Data.pDataBack, TempData, ProtocoalInfo.Length-3+15+6);
	}
	
	_ProData.MsgType		= 0x07;
	ReadDateTime(_ProData.TimeSmybol);
	//DISABLE_MBUS();  实时抄表时为了提高速度，抄完1只不放开通道。
		
	return Err;

	
}

/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_08				    							                   
**	函 数  功 能: 转发协议信息                                                                         			
**	输 入  参 数: _ProtocolType &_ProData   							         			 	       
**	输 出  参 数: none											                                      
**  返   回   值: none													                               
**	备		  注: 此处透明转发数据，理论上应该是发出数据，超时等待，								   
**                接收到数据后，直接再上传上去即可，不对接收的数据进行判断							   
**                现有做法，仍对接收到的数据进行热计量表协议数据的判断	                               																									
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
			*_ProData.Data.pDataBack= 0x10;					//异常回应
			Err = 0;
		}
	else
		{
			memcpy(_ProData.Data.pDataBack, TempData, TempLen);
		}
	
	return Err;	
}


/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_0E				    							                   
**	函 数  功 能: 主站读集中器下所有仪表数据（最近一次数据）                                                                      			
**	输 入  参 数: _ProtocolType &_ProData   							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注: SD卡中每个表的数据占用128字节的空间												   
**                其中第一个字节为数据长度(1字节) + 数据(N+11) + CS累加和校验(1字节)
**		         其中数据格式为: 热计量表地址(7字节) + 热量数据长度(1字节) + 热量数据(N) + 温度数据(3字节)
**		         若读取到得数据不正确，或没有读取到数据，则规定热量数据长度为0，无热量数据。 	                                                                                   **																									
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_0E(_ProtocolType &_ProData, uint8 *ControlSave)
{
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 Cs					= 0x00;
	uint8 *pTemp = _ProData.Data.pDataBack;
	uint8 *pFirst				= pTemp;
	uint8 ReadTime[6] = {0x00};
	uint8 MeterDataTemp[1024] 	= {0x00};					//暂时存储SD卡中数据
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 MeterFrameLen			= 0x00;						//检验读取到的数据帧正确性，并返回数据帧字节数
	uint8 DataCounter			= 0x00;
	uint8 DataErrCounter		= 0x00;
	uint8 SendReadyFalg 		= 0x00;
	uint16 StartMeterSn			= 0x00;
	uint16 MeterSnMax			= 0x00;
	uint16 FrameDataFileLenMax	= 0x00;						//获取当前设置的最大数据包的数据域最大字节数
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
	//查找当前时间下最近一次抄表完成的时间，不跨天
	
	//获取当前数据域最大字节数
	//获取表档案中表序号的最大数, 由于是实时数据，直接读取内存备份即可
	OS_ENTER_CRITICAL();
	FrameDataFileLenMax = gPARA_TermPara.DebugPara.FrameLen_Max;
	MeterSnMax = gPARA_MeterNum;
	OS_EXIT_CRITICAL();
	
	//获取上次上传到哪个表序号
	StartMeterSn = pMultiFrame->StartMeterSn;

    pFirst = pTemp++;												//是否有后续帧标志，组帧完后才可以填写
    DataBackLen++;
    *pTemp++ = pMultiFrame->SendPackIndex;					//当前发送的包序号
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
			//此次要判断长度是否为有效
        	
    		if( (DataBackLen+MeterFrameLen) > FrameDataFileLenMax )
    			{						
    				SendReadyFalg = 1;
    				break;										//预处理进行判断长度是否超过限制
    			}
    		else
    			{
    				if(StartMeterSn < MeterSnMax)				//判断是否还有表没传完
    					{
    						/*begin :yangfei modified 2013-08-29 for 当最后一包到时无下一包数据,防止主机不停的要数据*/
                             if(StartMeterSn+1==MeterSnMax)
                             	{
                             	debug_info(gDebugModule[METER_DATA],"StartMeterSn+1==MeterSnMax\r\n");
							    *pFirst = 0x00;	
                             	}
					         else
					         	{
					         	*pFirst = 0x01;						//仍有数据需要传输
					         	}
					       /*end :yangfei modified 2013-08-29 for 当最后一包到时无下一包数据*/	
    						
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
    
    //给主站的响应帧 填写数据结构，只需填写下面信息，其余信息在Create函数中操作
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x10;
	ReadDateTime(_ProData.TimeSmybol);
	
	if((DataErrCounter == pMultiFrame->SendMeterNums)&&(DataErrCounter!=0))							//如果数据全部不正确
		{
			*_ProData.Data.pDataBack = 0x10;					//异常回应
			goto ERROR;
		}
	
	return NO_ERR;
	
ERROR:
	_ProData.MsgLength		= 1;
	_ProData.MsgType		= 0x10;
	*_ProData.Data.pDataBack = 0x10;					//异常回应

	return ERR_1;
}


/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_18				    							                   
**	函 数  功 能: 转发协议信息                                                                         			
**	输 入  参 数: _ProtocolType &_ProData   							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注: 对阀门和温控面板的操作，还有待协议订型
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
			//群发 单发 标志错误
		}
	
	return Err;
}

/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_18_ForOne				    							                   
**	函 数  功 能: 针对单发的转发协议信息，操作集中器下的温控控制                                                                         			
**	输 入  参 数: _ProtocolType &_ProData   							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:  	                                                                                   																								
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
	uint8 lu8DataIn[20] = {0};  //用于存储函数传递参数。

	memset((uint8 *)&ReadHeatMeter, 0x00, sizeof(DELU_Protocol));
	*pTempBack++ = *pTemp++;
	DataBackLen++;

	MsgType		= *pTemp++;
	
	//此处需判断单发和群发标志 是否正确
	
	*pTempBack++	= MsgType;
	DataBackLen++;

	OSSemPend(METERChangeChannelSem, OS_TICKS_PER_SEC, &Err); //申请MBUS通道切换权利
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
		//return ERR_1;			//表地址没有查找到返回错误
		goto READ_FAIL;
		}
	PARA_ReadMeterInfo(MeterSn, &mf);
	//需判断控制面板地址是否正确有效
	METER_ChangeChannel(mf.ChannelIndex);
	
	switch(MsgType)
	{
			case 0x09:									//设置温控控制类型
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
			
		case 0x0A:									//抄室内实际温度
			{
				Err = ValveContron(&mf,ReadVALVE_All,lu8DataIn,DataFrame,&DataLen_Vave);
				
				if(Err == NO_ERR){
					*pTempBack++ = DataFrame[0];  //符号位
					DataBackLen++;
					*pTempBack++ = DataFrame[1];
					DataBackLen++;
					*pTempBack++ = DataFrame[2];  //小数位固定为0.
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
		
		case 0x0E:							 /*设置室内设定温度*/
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

		case 0x0F:                          /*设置室内上下限温度*/
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
	//回复数据帧
	_ProData.MsgLength	= DataBackLen;
	_ProData.MsgType	= 0x19;
	ReadDateTime(_ProData.TimeSmybol);
	//DISABLE_MBUS();  实时抄表时为了提高速度，抄完1只不放开通道。
	
	return Err;

MBUS_Busy:
	*pTempBack = 0x12;  //MBUS通道正忙。
	DataBackLen++;
	_ProData.MsgLength	= DataBackLen;
	_ProData.MsgType	= 0x19;
	ReadDateTime(_ProData.TimeSmybol);
		
	return Err;

}

/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_18_ForAll				    							                   
**	函 数  功 能: 针对群发的转发协议信息                                                                         			
**	输 入  参 数: _ProtocolType &_ProData   							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:  	                                                                                   																								
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
	
	//此处需判断单发和群发标志 是否正确
	
	*pTempBack++	= ForAllOrOne;
	*pTempBack++	= MsgType;
	DataBackLen 	+= 2;
	
	*pTempBack++	= 0x01;						//群发直接给正确响应 即可
	//发送信息给执行任务来，开始抄读或控制全体热力表，温控面板，阀门等；
	memset(gDEAL_ReadMailBox_Msg18, 0x00, sizeof(gDEAL_ReadMailBox_Msg18));
	gDEAL_ReadMailBox_Msg18[0] = MSG_18;
	gDEAL_ReadMailBox_Msg18[1] = MsgType;
	memcpy(&gDEAL_ReadMailBox_Msg18[2], pTemp, _ProData.MsgLength-2);
	
	OSQPost(CMD_Q, gDEAL_ReadMailBox_Msg18);
	
	//回复数据帧
	_ProData.MsgLength	= DataBackLen+1;
	_ProData.MsgType	= 0x19;
	ReadDateTime(_ProData.TimeSmybol);
	
	return Err;
}

/********************************************************************************************************
**  函 数  名 称: DEAL_MSG18_ForAll				    							                   
**	函 数  功 能: 读取所有计量表的信息                                                                         			
**	输 入  参 数: _ProtocolType &_ProData   							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:  	                                                                                   																								
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
	LogicAddr += 4;							//4个字节预留给  消息类型(1字节) + 指令类型(1字节) + 总字节数(2字节)
	if(MsgType!=0x0A)	LogicAddr += 6;		//6个字节预留给  操作设备总数(2Byte)+成功设备数(2Byte)+失败设备数(2Byte)
	//对温控面板的操作
	for(i=0; i<6; i++)
	{
		//切换Mbus通道
		METER_ChangeChannel(i+1);		//通道号 从 1开始
		
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
							//组建自建协议，读温控面板温度
							ReadData.DataIdentifier = 0x20A0;
							ReadData.DataBuf[7]		= 0x11;
							break;
						}
					case 0x09:
						{
							//组建自建协议，设置使能状态
							ReadData.DataBuf[7]		= 0x55;
							break;
						}
					case 0x0B:
						{
							//组建自建协议，设置禁用状态
							ReadData.DataBuf[7]		= 0x66;
							break;
						}
					case 0x0C:
						{
							//组建自建协议，设置自动状态
							ReadData.DataBuf[7]		= 0x77;
							break;
						}
					case 0x0D:
						{
							//组建自建协议，设置定时状态
							ReadData.DataBuf[7]		= 0x88;
							break;
						}
					case 0x0E:
						{
							//组建自建协议，设置室内设定温度
							ReadData.DataIdentifier = 0x23A0;
							memcpy(&ReadData.DataBuf[7], pTemp, 3);
							break;
						}
					case 0x0F:
						{
							//组建自建协议，设置室内设定温度
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
			else			//设置控制类指令， 只进行记录失败表地址
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
	//对所有设备操作完成后，根据成功和失败计数来总结设备操作情况
	LogicAddr	= METER_DATA_TEMP;
	paratemp[paralen++] = 0x18;
	paratemp[paralen++] = MsgType;
	if(MsgType == 0x0A)
		{
			paratemp[paralen++] = SaveCounter;
			paratemp[paralen++] = SaveCounter>>8;
		}
	else if( (MeterFailure==0x00) && (MeterCmplete!=0x00) )
		{														//失败数为0，则表示所有都成功
			paratemp[paralen++] = 0x01;
			paratemp[paralen++] = 0x00;
			paratemp[paralen++] = 0x01;
		}
	else if((MeterCmplete==0x00) && (MeterFailure!=0x00) )
		{														//成功数为0，则表示所有都失败
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
**  函 数  名 称: DEAL_ProcessMsg_1A				    							                   
**	函 数  功 能: 主站读取群发温度控制信息                                                                  			
**	输 入  参 数: uint8 *ControlSave 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:  	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_1A(_ProtocolType &_ProData, uint8 *ControlSave)
{
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 ComType				= 0x00;						//指令类型
	//uint8 *pData = _ProData.Data.pData;						//主站下发的消息体内容
	uint8 *pTemp = _ProData.Data.pDataBack;					//上传给主站的消息体内容
	uint8 *pFirst				= pTemp;
	uint8 MeterDataTemp[1024] 	= {0x00};					//暂时存储SD卡中数据
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 SendReadyFlag			= 0x00;
	uint8 LoopCounter			= 0x00;
	uint8 LoopMax				= 0x00;
	uint16 LoopLen				= 0x00;						//消息体数据最小长度，用于多帧传输，拆包不打断基本数据长度
	uint16 FrameDataFileLenMax	= 0x00;						//获取当前设置的最大数据包的数据域最大字节数
	uint16 DataBackLen			= 0x00;
	uint16 DataStoreLen			= 0x00;
	uint32 StartLogicAddr		= 0x00;
	//uint32 EndLogicAddr			= 0x00;

	LOG_assert_param( ControlSave == NULL );

	MultiFrameMsg_1A *pMultiFrame = NULL;
	pMultiFrame = (MultiFrameMsg_1A *)ControlSave;
	
	FrameDataFileLenMax = 450;								//获取当前数据域最大字节数
	
	*pTemp++ = 0;											//是否有后续帧标志，组帧完后才可以填写
	*pTemp++ = pMultiFrame->SendPackIndex;
    ComType  = *_ProData.Data.pData;						//指令类型
    DataBackLen += 2;
	
	//获取上传开始地址
	if( (pMultiFrame->MultiFlag) == 0x00)
		{													//传输的第一帧，尚不清楚是否需要多帧传输
			StartLogicAddr = METER_DATA_TEMP;
			OSMutexPend (FlashMutex, 0, &Err);
			/*need modified*/
    		Res = AppSdWrRdBytes(StartLogicAddr, 512, MeterDataTemp, SD_RD);
    		OSMutexPost (FlashMutex);
    		//需对Res进行判断
			if(Res)				;

    		//需对读取数据的前N个参数进行判断
    		if(MeterDataTemp[0] != 0x1A)		;			//消息类型不正确
    		if(MeterDataTemp[1] != ComType)		
			{		 //指令类型不正确
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
    					{												//指令类型为BCDEF, 均为控制设置命令
    						memcpy(pTemp, &MeterDataTemp[4], 6);		//第一包将操作概要信息上传
    						pTemp += 6;
    					}		
    			}
    		else
    			{	
    				//全部成功 和 全部失败的情况下，直接返回
    				*pTemp++ = MeterDataTemp[4];
					DataBackLen++;
    				//给主站的响应帧 填写数据结构，只需填写下面信息，其余信息在Create函数中操作
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
    		{												//指令类型为BCDEF, 均为控制设置命令
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
    	//需对Res进行判断
		if(Res)				;
		LoopCounter = 0x00;
    	LoopMax		= 512 / LoopLen;
    	while(LoopCounter<LoopMax)
    	{
    		if( (DataBackLen+LoopLen) > FrameDataFileLenMax )
    			{	
    				SendReadyFlag = 1;					
    				break;										//预处理进行判断长度是否超过限制
    			}
    		else
    			{
    				if( (StartLogicAddr+DataBackLen-3) < (pMultiFrame->EndReadAddr) )				//判断是否还有数据没传完
    					{
    						*pFirst = 0x01;						//仍有数据需要传输
    						
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
    
    //给主站的响应帧 填写数据结构，只需填写下面信息，其余信息在Create函数中操作
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
**  函 数  名 称: DEAL_ProcessMsg_1C				    							                   
**	函 数  功 能: 操作集中器下的阀门控制                                                                 			
**	输 入  参 数: 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:  	                                                                                   																								
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
			//群发 单发 标志错误
		}
	
	return Err;
}

/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_1C_ForOne				    							                   
**	函 数  功 能: 对于单发的数据处理                                                                      			
**	输 入  参 数: 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:  	                                                                                   																								
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
	uint8 lu8DataIn[20] = {0};  //用于存储函数传递参数。
		
//	uint16 ValveState		= 0x00;
	uint8 lu8workmode = 0;
	
		
	memset((uint8 *)&ReadHeatMeter, 0x00, sizeof(DELU_Protocol));
	*pTempBack++	= *pTemp++;
	DataBackLen++;
	
	MsgType 	= *pTemp++;
	
		//此处需判断单发和群发标志 是否正确
		
	*pTempBack++	= MsgType;
	DataBackLen++;

	OSSemPend(METERChangeChannelSem, OS_TICKS_PER_SEC, &Err); //申请MBUS通道切换权利
	if(Err != OS_ERR_NONE)
	 	goto MBUS_Busy;
	 	
		
	memcpy(HeatMeterAddr, pTemp, 7);
	Err = PARA_GetMeterSn(HeatMeterAddr, &MeterSn);
	if(Err!=NO_ERR) 		
	{
		debugX(LOG_LEVEL_ERROR,"%s PARA_GetMeterSn err!\r\n",__FUNCTION__);
		//return ERR_1; 		//表地址没有查找到返回错
		goto READ_FAIL;
	}
	PARA_ReadMeterInfo(MeterSn, &mf);
	METER_ChangeChannel(mf.ChannelIndex);
	
	memcpy(ReadHeatMeter.MeterAddr, mf.ValveAddr, 7);
	
	if(MsgType == 0xff){  /*读取阀门状态*/
		Err = ValveContron(&mf,ReadVALVE_All,lu8DataIn,DataFrame,&DataLen_Vave);
			
	}
	else{
		lu8DataIn[0] = MsgType;
		Err = ValveContron(&mf,SETVALVE_STATUS,lu8DataIn,DataFrame,&DataLen_Vave);
	}
		
	if(Err == NO_ERR){
		if(MsgType == 0xff){			/*读取阀门状态*/
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
		
		//回复数据帧
	_ProData.MsgLength	= DataBackLen;
	_ProData.MsgType	= 0x1D;
	ReadDateTime(_ProData.TimeSmybol);
		
	//DISABLE_MBUS(); 实时抄表时为了提高速度，抄完1只不放开通道。

	return Err;


MBUS_Busy:
	*pTempBack++ = 0x12;  //MBUS通道正忙。
	DataBackLen++;
	_ProData.MsgLength	= DataBackLen;
	_ProData.MsgType	= 0x1D;
	ReadDateTime(_ProData.TimeSmybol);
		
	return Err;
		
}

/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_1C_ForAll			    							                   
**	函 数  功 能: 对于群发的数据处理                                                                      			
**	输 入  参 数: 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:  	                                                                                   																								
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
	
	*pTempBack++	= 0x01;						//群发直接给正确响应 即可
	//发送信息给执行任务来，开始抄读或控制全体热力表，温控面板，阀门等；
	memset(gDEAL_ReadMailBox_Msg1C, 0x00, sizeof(gDEAL_ReadMailBox_Msg1C));
	gDEAL_ReadMailBox_Msg1C[0] = MSG_1C;
	gDEAL_ReadMailBox_Msg1C[1] = MsgType;
	memcpy(&gDEAL_ReadMailBox_Msg1C[2], _ProData.Data.pData, _ProData.MsgLength);
	
	OSQPost(CMD_Q, gDEAL_ReadMailBox_Msg1C);
	
	//回复数据帧
	_ProData.MsgLength	= DataBackLen+1;
	_ProData.MsgType	= 0x1D;
	ReadDateTime(_ProData.TimeSmybol);

	*pTempBack++ = 0x01;					//正常回应

	return Err;
}

/********************************************************************************************************
**  函 数  名 称: DEAL_MSG1C_ForAll				    							                   
**	函 数  功 能:                                                                       			
**	输 入  参 数: 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:  	                                                                                   																								
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

	//组建德鲁协议  控制阀门
	ReadData.MeterType 		= 0x20;
	ReadData.ControlCode 	= 0x04;
	ReadData.Length 		= 0x04;
	ReadData.DataIdentifier = 0x17A0;
	ReadData.DataBuf[0] 	= *pTemp;
	
	LogicAddr += 4;							//4个字节预留给  消息类型(1字节) + 指令类型(1字节) + 总字节数(2字节)
	LogicAddr += 6;							//6个字节预留给  操作设备总数(2Byte)+成功设备数(2Byte)+失败设备数(2Byte)
	//对阀门的操作
	for(i=0; i<6; i++)
	{
		DataTempLen = 0x00;
		METER_ChangeChannel(i+1);		//通道号 从 1开始
		
		//切换Mbus通道
		OS_ENTER_CRITICAL();
		memset(gDEAL_ReadDataTemp, 0x00, sizeof(gDEAL_ReadDataTemp));
		memcpy((uint8 *)TempArray, (uint8 *)&gPARA_ValveConChannel[i], gPARA_ValveConChannelNum[i]*sizeof(uint16));
		TempNums = gPARA_ValveConChannelNum[i];
		OS_EXIT_CRITICAL();
		
		for(j=0; j<TempNums; j++)
		{
			PARA_ReadMeterInfo(TempArray[j], &mf);
			
			//判断阀门地址是否有效
			if(MeterNoBcdCheck(mf.ValveAddr) == TRUE)
				{
					memcpy(ReadData.MeterAddr, mf.ValveAddr, 7);
			
					Res = METER_ReadMeterDataCur(&ReadData, TempArray[j]);
					//Res = 1;
					if(Res)	//设置控制类指令， 只进行记录失败表地址
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
	//对所有设备操作完成后，根据成功和失败计数来总结设备操作情况
	LogicAddr	= METER_DATA_TEMP;
	paratemp[paralen++] = 0x1C;
	paratemp[paralen++] = MsgTemp;
	if( (MeterFailure==0x00) && (MeterCmplete!=0x00) )
		{														//失败数为0，则表示所有都成功
			paratemp[paralen++] = 0x01;
			paratemp[paralen++] = 0x00;
			paratemp[paralen++] = 0x01;
		}
	
	else if((MeterCmplete==0x00) && (MeterFailure!=0x00) )
		{														//成功数为0，则表示所有都失败
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
**  函 数  名 称: DEAL_ProcessMsg_1E				    							                   
**	函 数  功 能:                                                                       			
**	输 入  参 数: 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:  	                                                                                   																								
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_1E(_ProtocolType &_ProData, uint8 *ControlSave)
{
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 ComType				= 0x00;						//指令类型
	uint8 *pTemp = _ProData.Data.pDataBack;					//上传给主站的消息体内容
	uint8 *pFirst				= pTemp;
	uint8 MeterDataTemp[600] 	= {0x00};					//暂时存储SD卡中数据
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 SendReadyFlag			= 0x00;
	uint16 LoopLen				= 0x00;						//消息体数据最小长度，用于多帧传输，拆包不打断基本数据长度
	uint16 LoopCounter			= 0x00;
	uint16 FrameDataFileLenMax	= 0x00;						//获取当前设置的最大数据包的数据域最大字节数
	uint16 DataBackLen			= 0x00;
	uint16 DataStoreLen			= 0x00;
	uint32 StartLogicAddr		= 0x00;
	//uint32 EndLogicAddr			= 0x00;

	LOG_assert_param( ControlSave == NULL );

	MultiFrameMsg_1E *pMultiFrame = NULL;
	pMultiFrame = (MultiFrameMsg_1E *)ControlSave;
	
	FrameDataFileLenMax = 450;								//获取当前数据域最大字节数
	
	*pTemp++ = 0;											//是否有后续帧标志，组帧完后才可以填写
	*pTemp++ = pMultiFrame->SendPackIndex;
    ComType  = *_ProData.Data.pData;						//指令类型
    DataBackLen += 2;
	
	//获取上传开始地址
	if( (pMultiFrame->MultiFlag) == 0x00)
		{													//传输的第一帧，尚不清楚是否需要多帧传输
															//读取存储参数区数据信息
			StartLogicAddr = METER_DATA_TEMP;
			OSMutexPend (FlashMutex, 0, &Err);
			/*need modified*/
    		Res = AppSdWrRdBytes(StartLogicAddr, 512, MeterDataTemp, SD_RD);
    		OSMutexPost (FlashMutex);
    		//需对Res进行判断
			if(Res)				;

    		//需对读取数据的前N个参数进行判断
    		if(MeterDataTemp[0] != 0x1C)		;			//消息类型不正确
    		if(MeterDataTemp[1] != ComType)		
			{		 //指令类型不正确
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
    						//字节长度控制有问题
    					}
    				*/
    				
    				
    				
    			}
    		else
    			{	
    				//全部成功 和 全部失败的情况下，直接返回
    				*pTemp++ = MeterDataTemp[4];
					DataBackLen++;
    				//给主站的响应帧 填写数据结构，只需填写下面信息，其余信息在Create函数中操作
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
    	//需对Res进行判断
		if(Res)				;
		LoopCounter = 0x00;
    	
    	while(LoopCounter<74)
    	{
    		if( (DataBackLen+LoopLen) > FrameDataFileLenMax )
    			{	
    				SendReadyFlag = 1;					
    				break;										//预处理进行判断长度是否超过限制
    			}
    		else
    			{
    				if( (StartLogicAddr+DataBackLen-3) < (pMultiFrame->EndReadAddr) )				//判断是否还有数据没传完
    					{
    						*pFirst = 0x01;						//仍有数据需要传输
    						
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
    
    //给主站的响应帧 填写数据结构，只需填写下面信息，其余信息在Create函数中操作
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x1F;
	ReadDateTime(_ProData.TimeSmybol);
	
	return NO_ERR;
}


/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_22				    							                   
**	函 数  功 能: 主站向集中器索要固定时间点数据                                                                      			
**	输 入  参 数: 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注: SD卡中每个表的数据占用128字节的空间，其中第一个字节为数据长度(1字节) + 数据(N+11) + CS累加和校验(1字节)
** 	              其中数据格式为: 热计量表地址(7字节) + 热量数据长度(1字节) + 热量数据(N) + 温度数据(3字节)
**				  若读取到得数据不正确，或没有读取到数据，则规定热量数据长度为0，无热量数据。                                                                    																								
 **   修改记录   :1. add input para char*path 2012-12-26  
                                       
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_22(_ProtocolType &_ProData, uint8 *ControlSave)
{

	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 Cs					= 0x00;
	uint8 *pTemp = _ProData.Data.pDataBack;
	uint8 *pFirst				= pTemp;
	uint8 ReadTime[6] 			= {0x00};
	uint8 MeterDataTemp[1024] 	= {0x00};					//暂时存储SD卡中数据
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 MeterFrameLen			= 0x00;						//检验读取到的数据帧正确性，并返回数据帧字节数
	uint8 DataCounter			= 0x00;
	uint8 DataErrCounter		= 0x00;
	uint8 SendReadyFalg 		= 0x00;
	uint16 StartMeterSn			= 0x00;
	uint16 MeterSnMax			= 0x00;
	uint16 FrameDataFileLenMax	= 0x00;						//获取当前设置的最大数据包的数据域最大字节数
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
	
	//获取实时的控制变量数据
	memcpy(ReadTime, _ProData.Data.pData, 6);
	//获取当前数据域最大字节数
	//获取表档案中表序号的最大数, 由于是实时数据，直接读取内存备份即可
	OS_ENTER_CRITICAL();
	FrameDataFileLenMax = gPARA_TermPara.DebugPara.FrameLen_Max;
	OS_EXIT_CRITICAL();
	//获取上次上传到哪个表序号
	StartMeterSn = pMultiFrame->StartMeterSn;
	/*begin:yangfei modified 2012-12-24 find path*/
	BcdTimeToHexTime(ReadTime,HexTime);
	//GetTimeNode(HexTime,TimeNodes);
	//GetTimeNodeFilePath(DataPath,HexTime,TimeNodes);
	GetFilePath(NodePath, HexTime, ARRAY_DAY);  /*获取固定点timenode path*/
	OSMutexPend (FlashMutex, 0, &Err);
	Res = SDReadData(NodePath, (uint8 *)&History_Para, sizeof(History_Para),0);
	OSMutexPost (FlashMutex);
	//GetTimeNodeFilePath(DataPath,HexTime,History_Para.TimeNode);
	GetTimeNodeFilePath_Forward(DataPath,HexTime,&History_Para);  //查找给定时间点前一个定时抄数据存放路径。
	/*end   :yangfei modified 2012-12-24*/
    //需对Res进行判断
	if(Res!=NO_ERR)	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
    	}
	
	MeterSnMax = History_Para.MeterNums;
	
    pFirst = pTemp++;										//是否有后续帧标志，组帧完后才可以填写
    DataBackLen++;
    *pTemp++ = pMultiFrame->SendPackIndex;					//当前发送的包序号
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
    	//需对Res进行判断
		DataCounter = 0;
		pDataTemp =  MeterDataTemp;

    	while(DataCounter< (1024/METER_FRAME_LEN_MAX))
    	{
			MeterFrameLen = *pDataTemp++;
			//此次要判断长度是否为有效
        	
    		if( (DataBackLen+MeterFrameLen) > FrameDataFileLenMax )
    			{						
    				SendReadyFalg = 1;
    				break;										//预处理进行判断长度是否超过限制
    			}
    		else
    			{
					/*begin :yangfei modified 2012-01-14 for StartMeterSn从0开始计数，多传输了一次*/
						#if 0
						if(StartMeterSn <= MeterSnMax)				//判断是否还有表没传完
						#endif
						if(StartMeterSn < MeterSnMax)
    				/*end  :yangfei modified 2012-01-14*/
    					{
    					/*begin :yangfei modified 2013-08-29 for 当最后一包到时无下一包数据,防止主机不停的要数据*/
                                            if(StartMeterSn+1==MeterSnMax)
                             	             {
                             	                 debug_info(gDebugModule[METER_DATA],"StartMeterSn+1==MeterSnMax\r\n");
							    *pFirst = 0x00;	
                             	             }
					         else
					         	{
					         	*pFirst = 0x01;						//仍有数据需要传输
					         	}
					  /*end :yangfei modified 2013-08-29 for 当最后一包到时无下一包数据*/	
    						
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
    
    //给主站的响应帧 填写数据结构，只需填写下面信息，其余信息在Create函数中操作
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x23;
	ReadDateTime(_ProData.TimeSmybol);
	
	if((DataErrCounter == pMultiFrame->SendMeterNums)&&(DataErrCounter!=0))			//如果数据全部不正确
		{
			_ProData.MsgLength		= 1;
			*_ProData.Data.pDataBack = 0x10;					//异常回应
			LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d  Msg_22 Error!",__FUNCTION__,__LINE__ );
			debug_info(gDebugModule[METER_DATA],"WARNING: %s %d  Msg_22 Error DataErrCounter =%d!",__FUNCTION__,__LINE__ ,DataErrCounter);
			goto ERROR;
		}
	
	return NO_ERR;

ERROR:
	_ProData.MsgType		= 0x23;
	_ProData.MsgLength		= 1;
	*_ProData.Data.pDataBack = 0x10;					//异常回应

	return ERR_1;
}



/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_3C				    							                   
**	函 数  功 能: 主站向集中器索要指定时间点指定表号的历史数据。                                                                   			
**	输 入  参 数: 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注: SD卡中每个表的数据占用128字节的空间，其中第一个字节为数据长度(1字节) + 数据(N+11) + CS累加和校验(1字节)
** 	              其中数据格式为: 热计量表地址(7字节) + 热量数据长度(1字节) + 热量数据(N) + 温度数据(3字节)
**				  若读取到得数据不正确，或没有读取到数据，则规定热量数据长度为0，无热量数据。                                                                    																								
 **   修改记录   :
                                       
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_3C(_ProtocolType &_ProData)
{
	uint16 i = 0;
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 Cs					= 0x00;
	uint8 *pTemp = _ProData.Data.pDataBack;
	uint8 ReadTime[6] 			= {0x00};
	uint8 MeterDataTemp[128] 	= {0x00};					//暂时存储SD卡中数据
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 MeterFrameLen			= 0x00;						//检验读取到的数据帧正确性，并返回数据帧字节数
	uint16 MeterSnMax			= 0x00;
	uint16 DataBackLen			= 0x00;
	//uint32 LogicAddr			= LOGIC_ADDR_NULL;
	
	/*begin:yangfei modified 2012-12-24 find path*/
	char  DataPath[]="/2012/12/24/1530";
	char  NodePath[]="/2012/12/24/timenode";
	uint8   HexTime[6] 			= {0x00};
	uint8 lu8MeterNum[7] = {0};
	uint8 lu8RecMeterNum[7] = {0};  //用于存储主机发来的目的表号。
	/*end   :yangfei modified 2012-12-24*/
	
	CPU_SR		cpu_sr;
	DataStoreParaType	History_Para;
	
	
	//获取实时的控制变量数据
	memcpy(ReadTime, _ProData.Data.pData, 6);  //取出目标时间。
	memcpy(lu8RecMeterNum, _ProData.Data.pData+6, 7);  //取出目标表地址。


	/*begin:yangfei modified 2012-12-24 find path*/
	BcdTimeToHexTime(ReadTime,HexTime);
	GetFilePath(NodePath, HexTime, ARRAY_DAY);  /*获取固定点timenode path*/
	OSMutexPend (FlashMutex, 0, &Err);
	Res = SDReadData(NodePath, (uint8 *)&History_Para, sizeof(History_Para),0);
	OSMutexPost (FlashMutex);
	//需对Res进行判断
	if(Res != NO_ERR)	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
			goto ERROR;
	}
	
	//GetTimeNodeFilePath(DataPath,HexTime,History_Para.TimeNode);
	GetTimeNodeFilePath_Forward(DataPath,HexTime,&History_Para);  //查找给定时间点前一个定时抄数据存放路径。
	/*end   :yangfei modified 2012-12-24*/

	MeterSnMax = History_Para.MeterNums;
	
    *pTemp++ = 0;				//是否有后续帧标志，无后续，固定为0.
    DataBackLen++;
    *pTemp++ = 0;					//当前发送的包序号,固定为0 。
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
		
		memcpy(lu8MeterNum, &MeterDataTemp[4], 7);  //取出表地址。
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

		if(i == (MeterSnMax-1)){  //没有查询到。
			goto ERROR;

		}

		
	}
	
    
    //给主站的响应帧 填写数据结构，只需填写下面信息，其余信息在Create函数中操作
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x3D;
	ReadDateTime(_ProData.TimeSmybol);
	
	return NO_ERR;

ERROR:
	_ProData.MsgType		= 0x3D;
	_ProData.MsgLength		= 1;
	*_ProData.Data.pDataBack = 0x10;					//异常回应

	return ERR_1;
}





/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_3E				    							                   
**	函 数  功 能: 主站向集中器发送立即抄表指令。                                                                   			
**	输 入  参 数: 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**   修改记录   :
                                       
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_3E(_ProtocolType &_ProData)
{
	uint8 lu8SendMsg[2]		= {0x00};
	
	if((gu8ReadAllMeterFlag!=0) && (gu8ReadAllMeterFlag!=1)){  //防止出错。
		gu8ReadAllMeterFlag = 0;
	}
	

	ReadDateTime(_ProData.TimeSmybol);

	
	if(gu8ReadAllMeterFlag == 1){//正在全抄表。
		_ProData.MsgType		= 0x3F;
		_ProData.MsgLength		= 1;
		*_ProData.Data.pDataBack = 0x12;	//异常回应,正在全抄。
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
**  函 数  名 称: DEAL_ProcessMsg_40				    							                   
**	函 数  功 能: 主站向集中器索要固定时间点数据                                                                      			
**	输 入  参 数: 							         			 	      
**	输 出  参 数: none											                                       
**  返   回   值: none													                               
**	备		  注:                                                                																								
**   修改记录   :
1. add input para char*path 2012-12-26  
2.  yangfei added 20140219  for 支持热分配表、时间通断面积法

    sd卡中数据保存格式:(每块为128字节)
    0:热表数据
    1:热分配表或阀控数据
    2:热分配表或阀控数据
    3:热分配表或阀控数据
    ...

    从第1块开始读取
	根据设备类型
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_40(_ProtocolType &_ProData, uint8 *ControlSave)
{
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 Cs					= 0x00;
	uint8 *pTemp = _ProData.Data.pDataBack;
	uint8 *pFirst				= pTemp;
	uint8 ReadTime[6] 			= {0x00};
	uint8 MeterDataTemp[1024] 	= {0x00};					//暂时存储SD卡中数据
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 MeterFrameLen			= 0x00;						//检验读取到的数据帧正确性，并返回数据帧字节数
	uint8 DataCounter			= 0x00;
	uint8 DataErrCounter		= 0x00;
	uint8 SendReadyFalg 		= 0x00;
	uint16 StartMeterSn			= 0x00;
	uint16 MeterSnMax			= 0x00;
	uint16 FrameDataFileLenMax	= 0x00;						//获取当前设置的最大数据包的数据域最大字节数
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
	
	//获取实时的控制变量数据
	memcpy(ReadTime, _ProData.Data.pData, 6);
	//获取当前数据域最大字节数
	//获取表档案中表序号的最大数, 由于是实时数据，直接读取内存备份即可
	OS_ENTER_CRITICAL();
	FrameDataFileLenMax = gPARA_TermPara.DebugPara.FrameLen_Max;
	OS_EXIT_CRITICAL();
	//获取上次上传到哪个表序号
	StartMeterSn = pMultiFrame->StartMeterSn;
	/*begin:yangfei modified 2012-12-24 find path*/
	BcdTimeToHexTime(ReadTime,HexTime);
	
	GetFilePath(NodePath, HexTime, ARRAY_DAY);  /*获取固定点timenode path*/
	OSMutexPend (FlashMutex, 0, &Err);
	Res = SDReadData(NodePath, (uint8 *)&History_Para, sizeof(History_Para),0);
	OSMutexPost (FlashMutex);
	if(Res!=NO_ERR)
	{
		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
	}
	GetTimeNodeFilePath(DataPath,HexTime,History_Para.TimeNode);
	
	MeterSnMax = History_Para.MeterNums;
	
    pFirst = pTemp;										//是否有后续帧标志，组帧完后才可以填写

	pTemp += sizeof(MultiFrameMsg_40)-2;/*预留数据头位置*/
		
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
    	//需对Res进行判断
		DataCounter = 0;
		pDataTemp =  MeterDataTemp;

    	while(DataCounter< (1024/METER_FRAME_LEN_MAX))
    	{
			MeterFrameLen = *pDataTemp++;
    		if( (DataBackLen+MeterFrameLen) > FrameDataFileLenMax )/*此次要判断长度是否为有效*/
    			{						
    				SendReadyFalg = 1;
    				break;										//预处理进行判断长度是否超过限制
    			}
    		else
    			{
						if(StartMeterSn < MeterSnMax)
    					{
    					/*begin :yangfei modified 2013-08-29 for 当最后一包到时无下一包数据,防止主机不停的要数据*/
                              if(StartMeterSn+1==MeterSnMax)
                             	{
                             	debug_info(gDebugModule[METER_DATA],"StartMeterSn+1==MeterSnMax\r\n");
							    *pFirst = 0x00;	
                             	}
					         else
					         	{
					         	*pFirst = 0x01;						//仍有数据需要传输
					         	}
					  /*end :yangfei modified 2013-08-29 for 当最后一包到时无下一包数据*/	
    						
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

	memcpy(pFirst, pMultiFrame, sizeof(MultiFrameMsg_40)-2);/*保存数据头*/
    
    debug("StartMeterSn =%d  pMultiFrame->StartMeterSn=%d\r\n ",StartMeterSn,pMultiFrame->StartMeterSn);
    
    //给主站的响应帧 填写数据结构，只需填写下面信息，其余信息在Create函数中操作
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x41;
	ReadDateTime(_ProData.TimeSmybol);
	
	if((DataErrCounter == pMultiFrame->SendMeterNums)&&(DataErrCounter!=0))			//如果数据全部不正确
		{
			_ProData.MsgLength		= 1;
			*_ProData.Data.pDataBack = 0x10;					//异常回应
			LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d  Msg_22 Error!",__FUNCTION__,__LINE__ );
			debug_info(gDebugModule[METER_DATA],"WARNING: %s %d  Msg_22 Error DataErrCounter =%d!",__FUNCTION__,__LINE__ ,DataErrCounter);
			goto ERROR;
		}
	
	return NO_ERR;

ERROR:
	_ProData.MsgType		= 0x41;
	_ProData.MsgLength		= 1;
	*_ProData.Data.pDataBack = 0x10;					//异常回应

	return ERR_1;
}







/********************************************************************************************************
**  函 数  名 称: DEAL_ProcessMsg_42			    							                   
**	函 数  功 能:针对亿林阀控器协议的 透明传输指令。                                                                     			
**	输 入  参 数: 
**	输 出  参 数: none											                                      
**  返   回   值: none													                               
**	备		  注:                              																									
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

	if(gu8ReadAllMeterFlag == 0){//只有在非全抄状态才透传，否则提示正忙。
		lu8ForAllorOne = _ProData.Data.pData[0]; 
		if(lu8ForAllorOne == FORALL){  //群发
			Err = Communicate_ForAll(_ProData.Data.pData);

			_ProData.MsgLength		= 1; 
			_ProData.MsgType		= 0x43;
			*_ProData.Data.pDataBack = 0x01;  //群发下行设备无返回，集中器固定向上位机返回成功。
			Err = ReadDateTime(_ProData.TimeSmybol);
			
		}
		else{	//单发

		//首先切换到对应通道。
			lu8channel = _ProData.Data.pData[2];
			METER_ChangeChannel(lu8channel);


		//其次根据设备类型，设定正确的串口参数。
			lu8DevType =  _ProData.Data.pData[1];
			lu8ProtocolType = _ProData.Data.pData[3];


			if(lu8DevType == 0x20){  //热表

				(*METER_ComParaSetArray[gMETER_Table[lu8ProtocolType][0]])();
			}
			else if(lu8DevType == 0xb0){  //阀控器。
				(*METER_ComParaSetArray[gVALVE_Table[lu8ProtocolType][0]])();  //设置阀门对应串口参数。
			}
			else{
				//待需要时补充。
			}

	loop:	DataLen_Vave =  _ProData.Data.pData[5];  
			memcpy(DataFrame, &(_ProData.Data.pData[6]),DataLen_Vave);
					
			//Err =  ReadData_Communicate(DataFrame, &DataLen_Vave);  //所有热表、阀透传都可以用这个函数下发接收。

			if(lu8DevType == HEAT_METER_TYPE)
					Err =  ReadData_Communicate(DataFrame, &DataLen_Vave); 
			else if(lu8DevType == TIME_ON_OFF_AREA_TYPE){
					if(lu8ProtocolType == 0x01)   //亿林阀门
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
				
				//METER_ChangeChannel(7);//断开MBUS通道。
			}
		}
		else{
			_ProData.MsgLength		= 1; 
			_ProData.MsgType		= 0x43;
			*_ProData.Data.pDataBack = 0x12;  //提示正忙。
			Err = ReadDateTime(_ProData.TimeSmybol);

		}

		gu8MBusShortTime = 0; //在此处将此变量清除。
	
		return Err; 
}


/*****************************************************************************************
**								End of File
*****************************************************************************************/

