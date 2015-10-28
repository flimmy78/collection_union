/*******************************************Copyright (c)*******************************************
         							ɽ������ռ似����˾(�����ֲ�)                                                 
**  ��   ��   ��: StoreManage.c
**  ��   ��   ��: ������
**  �� ��  �� ��: 2012.08.07 
**  ��        ��: ϵͳ���ݴ洢����
**  �� ��  �� ¼:   	
*****************************************************************************************************/

/********************************************** include *********************************************/
#include <includes.h>
#include "app_down.h"
#include "HZTable.h"
extern uint8 gRestartFlag ;

/********************************************** extern *********************************************/

/********************************************** global *********************************************/
// 1, ���ڴ���C++ ���� C �������������Щȫ��������������������.C�ļ��С�
// 2, ����ȫ�ֱ������ݷ�ɢ���ص��ⲿSRAM�У�����ͳһ�����ڸ��ļ���.
// 3, �������ⲿSRAM�е�ȫ�ֱ���������Ӧ��ǰ��ϵͳ����ǰ�����ú������г�ʼ�������������������Զ���ʼ��
// 4, ��Щȫ�ֱ�����Ҫ�ڿ�����ʼ������ʼ��ʱ��sd�����Ƿ�д�����ݺ�δд������2�����
/************************************** HeatMeterFile *********************************************/
uint16 gPARA_MeterNum 			= 0;							//��ǰ����������
MeterFileType gPARA_MeterFiles[METER_NUM_MAX];					//�ڴ��д洢��ǰ������Ϣ
uint8 gPARA_SendNum_Prevous		= 0;							//������һ�����͵ı�������
uint8 gPARA_SendIndex_Prevous	= 0;							//������һ�����ݰ������к�
/*begin:yangfei added 2013-03-25 for add leye 485 meter*/
uint16 gPARA_MeterChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];			//6��ͨ���ȼ�������Ϣ, MeterSn
uint8  gPARA_Meter_Failed[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];	/*7��ͨ��ʧ�ܼ�¼*/
uint16 gPARA_MeterChannelNum[METER_CHANNEL_NUM];								//ÿ��ͨ��������
uint16 gPARA_ConPanelChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];			//6��ͨ��ÿ��ͨ�����¿������Ϣ, MeterSn
uint16 gPARA_ConPanelChannelNum[METER_CHANNEL_NUM];								//ÿ��ͨ�����ص��¿��������
uint16 gPARA_ValveConChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];			//6��ͨ��ÿ��ͨ���ķ�����Ϣ, MeterSn
uint16 gPARA_ValveConChannelNum[METER_CHANNEL_NUM];								//ÿ��ͨ�����صķ�������
/*end:yangfei added 2013-03-25 for add leye 485 meter*/
/************************************** ����ʱ��ڵ� **********************************************/
uint16 gPARA_TimeNodes[TIME_NODE_NUM_MAX]	 = {0};				//��ǰ����ʱ��㣬���ڴ��еı���
uint16 gPARA_TimeNodesNum = 0;			//������õĳ���ʱ���������
/************************************** �������������� ********************************************/
TermParaSaveType gPARA_TermPara;								//��������������
UpCommParaSaveType	gPARA_UpPara;								//����ͨѶ��������
CommPara uPCommPara;//muxiaoqing
ReplenishReadMeterSaveType gPARA_ReplenishPara;  //�������������,added by zjjin.


GlobalPara gStore_CommPara;
/************************************** SystemLog *************************************************/
uint8 gLOG_SystemLog[1024] = {0x00};							//��¼ϵͳ������־
OS_EVENT *LogQueSem; 											//��������־���ź���
OS_EVENT *LogWriteSem;											//����д����־���ź���	
LCDContrastPara NVLCDConraston;

/********************������Ϣ�鿴�������õ��ı���****************************/
uint8 gu8ReadAllMeterFlag = 0;   //��ǵ�ǰ�Ƿ����ڳ�ȫ��0-���ڣ�1-����ȫ����
uint8 gu8ReadDataStore[METER_FRAME_LEN_MAX] = {0};  //���ڴ�Ÿոճ����ı����Ϣ���������Ի��ѯ��



/********************MBUS��·�����õ���ȫ�ֱ���*********************************/
uint8 gu8MBusShortRising = 0;  //MBUS��·ָʾ�����Ƿ���������أ�0-û�У�1-���������ء�
uint8 gu8MBusShortFlag[METER_CHANNEL_NUM] = {0};  //��Ƕ�ӦMBUSͨ��ͨ����·,1-��·��0-����·��
uint8 gu8NowMBUSChannel = 0;    //��¼��ǰMBUSͨ����

uint8 gu8MBusShortTime = 0; //���MBUSͨ����·��ʱ���¼��

/********************��������ʧ�ܱ��***********************************************/
uint8 gu8ReadValveFail = 0;  //��ǳ��������Ƿ�ʧ�ܣ�0-ûʧ�ܣ�1-ʧ�ܡ�


uint8 gu8ErrFlag = 0;//����Ƿ��й��Ϸ������й�����������졣1-�й��ϡ�
uint16 gu16ErrOverTime = 0;//��������ʱ�䣬��һ��ʱ�����ֹͣ��





#ifdef DEMO_APP

uint8  DemoDateIsGet=0;
#endif


/********************************************** static *********************************************/

/********************************************** extern *********************************************/
extern uint16 gUpdateBegin ;
#if 0 /*�˺���û��*/
/****************************************************************************************************
**	�� ��  �� ��: STORE_GetAddrOfMeterData
**	�� ��  �� ��: ��ȡĳ���ȼ���������ݻ�����Ĵ洢λ��
**	�� ��  �� ��: uint8 *pTime -- ����ҵ�ʱ��; uint16 MeterSn -- �ȼ��������; 
**				  uint8 Flag -- FIND_HISTORY, FIND_CURRENT, FIND_DATA, FIND_PARA
**	�� ��  �� ��: uint32 *AddrOut -- ���SD���洢��λ�õ�ַ
**  ��   ��   ֵ: NO_ERR;
**	��		  ע: 
*****************************************************************************************************/
uint8 STORE_GetAddrOfMeterData(uint8 *pTime, uint16 MeterSn, uint32 *AddrOut, uint8 Flag)
{
	uint8  AddrErrFlag = 0;							//���ظ��ִ����־
	uint8  ReadTime[6] = {0};						//���ȡ��ʱ�䣬Сʱ���գ��£���Ϊ��Ч����
	uint32 TimeOffSet  = 0;							//�����ȡ��ʱ�����׼ʱ��ƫ����
	uint8  MonthHEX	   = 1;							//�·ݱ����������ж��������Ƿ��ڹ�ů��
	
	LOG_assert_param(pTime == NULL);
	LOG_assert_param(AddrOut == NULL);
	LOG_assert_param(MeterSn > METER_NUM_MAX);
	
	memcpy(ReadTime, pTime, 6);
	if(TimeCheck(ReadTime))			return ERR_1;	//����ʱ����ȷ��
	
	MonthHEX = BcdToHex(ReadTime[ARRAY_MONTH]);	
	
	AddrErrFlag  = STORE_CalcTime(ReadTime, &TimeOffSet, Flag);	//��ȡ���ȡ��ʱ�����׼ʱ���ƫ��������һ��ʱ��ڵ�Ϊ��λ
	if((MonthHEX >= 0x0B) && (MonthHEX <= 0x03))	//�ж��Ƿ��ڲ�ů������ů��ÿ���µ����ݶ��洢
		{
			if(AddrErrFlag == NO_ERR)
				{
					if( (Flag&0xF0) == FIND_DATA )
						{
							*AddrOut    = ( METERS_DATA_STORE_ADDR + \
						   				(TimeOffSet*METERS_DATA_LEN) + ((MeterSn+1)*METER_FRAME_LEN_MAX) );
						}
					if( (Flag&0xF0) == FIND_PARA )
						{
							*AddrOut    = METERS_DATA_STORE_ADDR + (TimeOffSet*METERS_DATA_LEN);
						}
				}
			else
				{
					*AddrOut	= LOGIC_ADDR_NULL;
				}
		}
	else												//�ǲ�ů����ֻ����һ���¹��õĴ洢�ռ�
		{
			if( (Flag&0xF0) == FIND_DATA )
				{
					*AddrOut    = ( METERS_DATA_STORE_PUBLIC + \
				   				(TimeOffSet*METERS_DATA_LEN) + ((MeterSn+1)*METER_FRAME_LEN_MAX) );
				}
			if( (Flag&0xF0) == FIND_PARA )
				{
					*AddrOut    = METERS_DATA_STORE_PUBLIC + (TimeOffSet*METERS_DATA_LEN);
				}
		}
	
	return AddrErrFlag;
}
#endif
/****************************************************************************************************
**	�� ��  �� ��: PARA_ReadMeterInfo
**	�� ��  �� ��: ��ȡĳ���ȼ�����ı�����Ϣ
**	�� ��  �� ��: uint16 MeterSn -- �ȼ��������; 
**	�� ��  �� ��: MeterFileType *pMeterFile -- ���������Ϣ
**  ��   ��   ֵ: NO_ERR;
**	��		  ע: 
*****************************************************************************************************/
uint8 PARA_ReadMeterInfo(uint16 MeterSn, MeterFileType *pMeterFile)
{
	uint8 Err = NO_ERR;
	CPU_SR	cpu_sr;
	LOG_assert_param(MeterSn > METER_NUM_MAX);
	LOG_assert_param(pMeterFile == NULL);

	OS_ENTER_CRITICAL();
	memcpy((uint8 *)&(pMeterFile->MeterID), (uint8 *)&(gPARA_MeterFiles[MeterSn].MeterID), sizeof(MeterFileType));
	OS_EXIT_CRITICAL();
	
	return Err;
}




/****************************************************************************************************
**	�� ��  �� ��: STORE_GetTimeNodeInfo
**	�� ��  �� ��: ָ��ʱ���ڵ�ǰ����ʱ��ڵ������µ�ʱ��ڵ���ţ��������ʱ��ڵ�Ĳ���ٷ���
**	�� ��  �� ��: uint16 InTime 
**      	 	  uint8 *pNodeIndex 
**			      int8 *pNodeOffset
**  �� ��  �� ��: 
**  ��   ��   ֵ: NO_ERR;
**	��		  ע: �������߼���   ������������ܳ��ָ���
*****************************************************************************************************/
uint8 STORE_GetTimeNodeInfo(uint16 InTime, uint8 *pNodeIndex, int8 *pNodeOffset)
{
	uint8 Err				= 0x00;
	uint8 MinuteBinIn 		= 0x00;
	uint8 MinuteBinNode 	= 0x00;
	uint8 NodeIndex			= 0x00;
	uint16 TimeNodeCur[TIME_NODE_NUM_MAX] 	= {0x00};
	CPU_SR		cpu_sr;

	LOG_assert_param( pNodeIndex == NULL );
	LOG_assert_param( pNodeOffset == NULL );

	OS_ENTER_CRITICAL();
    memcpy((uint8 *)TimeNodeCur, (uint8 *)gPARA_TimeNodes, sizeof(gPARA_TimeNodes));
    OS_EXIT_CRITICAL();
    
    Err = STORE_FindTimeNodeIndex(InTime, TimeNodeCur, &NodeIndex);
	*pNodeIndex = NodeIndex;
    
    //��ʱ�����ӵıȽ�
    if(Err == NO_ERR)
    	{
    		if( (InTime&0xFF00) == (TimeNodeCur[NodeIndex]&0xFF00))
				{
					MinuteBinIn	=  BcdToHex(InTime);
					MinuteBinNode =  BcdToHex(TimeNodeCur[NodeIndex]);
					*pNodeOffset = 	MinuteBinIn - MinuteBinNode;
				}
    	}
	
    return Err;
}

/****************************************************************************************************
**	�� ��  �� ��: STORE_FindTimeNodeIndex
**	�� ��  �� ��: ʱ��ڵ������
**	�� ��  �� ��: uint16 InTime 
**      	 	  uint16 *pTimeNode 
**			      uint8 *pNodeIndexOut
**  �� ��  �� ��:
**  ��   ��   ֵ: NO_ERR;
**	��		  ע: 
*****************************************************************************************************/
uint8 STORE_FindTimeNodeIndex(uint16 InTime, uint16 *pTimeNode, uint8 *pNodeIndexOut)
{
	uint8 i=0;
	uint8 FindFlag	 = 0x00;
	uint8 CheckBinHour = 0x00;
	uint8 CheckBinMinute = 0x00;
	uint16 TimeNodeCur[TIME_NODE_NUM_MAX] = {0x00};
    
    LOG_assert_param( pTimeNode == NULL );
	LOG_assert_param( pNodeIndexOut == NULL );
	memcpy((uint8 *)TimeNodeCur, (uint8 *)pTimeNode, (TIME_NODE_NUM_MAX)*sizeof(uint16));

    for(i=0; i<TIME_NODE_NUM_MAX; i++)
    {
    	//��ʱ���BCD���
		CheckBinMinute	=  BcdToHex(TimeNodeCur[i]);
		CheckBinHour =  BcdToHex(TimeNodeCur[i]>>8);
		if( (CheckBinHour>0x18) || (CheckBinMinute>=0x3C))
			{
				return 1;							//ʱ��ڵ����
			}

    	if( (TimeNodeCur[i] <= InTime) && (InTime < TimeNodeCur[i+1]) )
    		{
    			FindFlag = i;
    			break;
    		}
		if( (i==(TIME_NODE_NUM_MAX-1)) && (TimeNodeCur[i] <= InTime) ) //���һ��ʱ��ڵ�
			{
			 	FindFlag = i;
				break;
			}

    }
    FindFlag = i;			//��ֹ�������Ż���
    if(FindFlag < TIME_NODE_NUM_MAX)
    	{
    		*pNodeIndexOut = FindFlag;				//�ҵ�ĳ��ʱ��ڵ�
    	}
    else
    	{											//δ�ҵ���Ӧ��ʱ��ڵ�
    		return 3;
    	}
    	
    return NO_ERR;
}


/****************************************************************************************************
**	�� ��  �� ��: FindTimeNodeIndex_Forward
**	�� ��  �� ��: ��ȡʱ��ڵ������
**	�� ��  �� ��: uint16 InTime 
**      	 	  uint16 *pTimeNode 
**			      uint8 *pNodeIndexOut
**  �� ��  �� ��:
**  ��   ��   ֵ: NO_ERR;
**	��		  ע: ����ʱ�䣬��ѯ��ȡ����ʱ��֮ǰ�����ʱ��ڵ�������
**                         ��������ѯ�����������ǰһ�����һ��ʱ��ڵ�������
*****************************************************************************************************/
uint8 FindTimeNodeIndex_Forward(uint16 InTime,DataStoreParaType *datastorePara, uint8 *pNodeIndexOut,uint8 *pYesterday)
{
	uint8 i=0;
	uint8 FindFlag	 = 0x00;
	uint8 CheckBinHour = 0x00;
	uint8 CheckBinMinute = 0x00;
	uint16 TimeNodeCur[TIME_NODE_NUM_MAX] = {0x00};
	uint16 lu16MinNode = 0;  
	uint16 lu16MaxNode = 0;
	uint8 lu8MaxNodeIndex = 0;
    
    LOG_assert_param( datastorePara == NULL );
	LOG_assert_param( pNodeIndexOut == NULL );
	memcpy((uint8 *)TimeNodeCur, (uint8 *)datastorePara->TimeNode, (TIME_NODE_NUM_MAX)*sizeof(uint16));


	lu16MinNode = TimeNodeCur[0];
	lu16MaxNode = TimeNodeCur[0];
	for(i=0; i<TIME_NODE_NUM_MAX; i++){   //�ҵ���С���Ķ�ʱ����ʱ��㡣
		if(TimeNodeCur[i] != 0xffff){
			if(	lu16MinNode > TimeNodeCur[i])
				lu16MinNode = TimeNodeCur[i];

			if(lu16MaxNode < TimeNodeCur[i]){
				lu16MaxNode = TimeNodeCur[i];
				lu8MaxNodeIndex = i;
				}

			}
	}
		

    for(i=0; i<TIME_NODE_NUM_MAX; i++)
    {
    	//��ʱ���BCD���
		CheckBinMinute	=  BcdToHex(TimeNodeCur[i]);
		CheckBinHour =  BcdToHex(TimeNodeCur[i]>>8);
		if( (CheckBinHour>0x18) || (CheckBinMinute>=0x3C))
			{
				return 1;							//ʱ��ڵ����
			}

		if(lu16MinNode >= InTime)
    		{
    			FindFlag = lu8MaxNodeIndex;
				*pYesterday = 1;  //��������һ��ʱ��㡣
    			break;
    		}

    	if( (TimeNodeCur[i] <= InTime) && (InTime < TimeNodeCur[i+1]) )
    		{
    			FindFlag = i;
    			break;
    		}
		if( (i==(TIME_NODE_NUM_MAX-1)) && (TimeNodeCur[i] <= InTime) ) //���һ��ʱ��ڵ�
			{
			 	FindFlag = i;
				break;
			}

    }
    FindFlag = FindFlag;			//��ֹ�������Ż���
    if(FindFlag < TIME_NODE_NUM_MAX)
    	{
    		*pNodeIndexOut = FindFlag;				//�ҵ�ĳ��ʱ��ڵ�
    	}
    else
    	{											//δ�ҵ���Ӧ��ʱ��ڵ�
    		return 3;
    	}
    	
    return NO_ERR;
}


/****************************************************************************************************
**	�� ��  �� ��: StoreYesterday
**	�� ��  �� ��: ��ס���죬����������ڴ�����죬�����ѯʹ�á�
**	�� ��  �� ��: 
**      	 	 
**  �� ��  �� ��:
**  ��   ��   ֵ: 
**	��		  ע: 
*****************************************************************************************************/
/********************���쳭�����ݴ洢·����¼ȫ�ֱ���**********************/
char  gcYesterdayPath[] = "/2015/06/09/1200";

uint8 StoreYesterday(void)
{
	uint8 Err = 0;
	char  YesterdayPath[] = "/2015/06/09/yesterday";
	uint8 SystemTime[6] = {0x00};
	

	memcpy(SystemTime, gSystemTime, 6);
	//�Ƚ�����·���������·���У��Ա���ѯʹ�á�
	GetFilePath(YesterdayPath, SystemTime,ARRAY_DAY);
	OSMutexPend (FlashMutex,0,&Err);
	
	Err = SDSaveData(YesterdayPath, (uint8 *)gcYesterdayPath, sizeof(gcYesterdayPath),0);
	if(Err!=NO_ERR)
	{
		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
	}
	OSMutexPost (FlashMutex);
	GetFilePath(gcYesterdayPath, SystemTime,ARRAY_DAY);  //����Ϊ���죬��������

	if(Err == NO_ERR)
		return 0;
	else
		return 1;


}



/****************************************************************************************************
**	�� ��  �� ��: STORE_InitTimeNodePara
**	�� ��  �� ��: ��ʼ��ʱ��ڵ�Ĳ���
**	�� ��  �� ��: uint8 *pDateTime 
**      	 	  uint8 NodeIndex 
**  �� ��  �� ��:
**  ��   ��   ֵ: NO_ERR;
**	��		  ע: 
*****************************************************************************************************/
uint8 STORE_InitTimeNodePara(uint8 *pDateTime, uint8 NodeIndex)
{
	uint8 Err			= 0x00;
	//uint32 LogicAddr	= 0xFFFFFFFF;
	DataStoreParaType	InitPara;
	CPU_SR		cpu_sr;
	/*begin:yangfei added 2012-12-24 find path*/
	uint8 SystemTime[6] = {0x00};
	char  NodePath[] = "/2012/12/24/timenode";

	memset(&InitPara,0,sizeof(InitPara));
	/*end   :yangfei added 2012-12-24 */
    LOG_assert_param( pDateTime == NULL );	
	memcpy(InitPara.DataRecordTime, pDateTime, 6);
	InitPara.DataRecordTime[ARRAY_SECOND]		= 0x00;	
     //memset(InitPara.ReadCmplFlag, 0x00, 75);
	//READ_ParaInit();

	/*begin:yangfei added 2012-12-24 find path*/
	
	if(!NodeIndex)		//ֻ����ÿ��һ��ʼ��ʱ��ڵ�������Ч
		{
			OS_ENTER_CRITICAL();
			InitPara.MeterNums = gPARA_MeterNum;
			memcpy((uint8 *)InitPara.TimeNode, (uint8 *)gPARA_TimeNodes, sizeof(gPARA_TimeNodes));
			memcpy(SystemTime, gSystemTime, 6);
			OS_EXIT_CRITICAL();


			GetFilePath(NodePath, SystemTime,ARRAY_DAY);
			OSMutexPend (FlashMutex,0,&Err);
			Err =SDSaveData(NodePath, &InitPara, sizeof(DataStoreParaType),0);
			if(Err!=NO_ERR)
	    	{
	    		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
	    	}
		    OSMutexPost (FlashMutex);
		}
	#if 0
	else
		{
			memset((uint8 *)InitPara.TimeNode, 0xFF, sizeof(gPARA_TimeNodes));
		}
	STORE_GetAddrOfMeterData(pDateTime, 0, &LogicAddr, FIND_CURRENT|FIND_PARA);

	OSMutexPend (FlashMutex,0,&Err);
    Err =AppSdWrRdBytes(LogicAddr, sizeof(DataStoreParaType), (uint8 *)&InitPara, SD_WR);
    OSMutexPost (FlashMutex);
	#endif
	/*end   :yangfei added 2012-12-24 */
	return 0;
}
#if 0
/****************************************************************************************************
**	�� ��  �� ��: STORE_FindDayStartAddr
**	�� ��  �� ��: ���ҿ�ʼ��ַ
**	�� ��  �� ��: uint8 *pReadTime 
**      	 	  uint32 *pAddrOut 
**  �� ��  �� ��:
**  ��   ��   ֵ: NO_ERR;
**	��		  ע: 
*****************************************************************************************************/
uint8 STORE_FindDayStartAddr(uint8 *pReadTime, uint32 *pAddrOut)
{
	//���ҵ�����ҵ�ʱ��ڵ�� �꣬�£���
	//�������ڱ��������Ƿ��и�ʱ, �ֵĳ���洢�ڵ�
	uint8 YearHex	= 0x00;
	uint8 MonthHex	= 0x01;
	uint8 DayHex	= 0x01;
	uint8 ReadTime[6] = {0x00};
	uint32 CalcTemp	= 0x00;
	uint16 CalcYearTemp = 0x00;
	uint16 CalcMonthTemp = 0x00;
	uint16 CalcDayTemp = 0x00;
	uint16 TimeTempSource = 0x00;

    LOG_assert_param( pReadTime == NULL );
	LOG_assert_param( pAddrOut == NULL );	
	
	memcpy(ReadTime, pReadTime, 6);
	if(TimeCheck(ReadTime))				return 1;
	
	YearHex 	= BcdToHex(ReadTime[ARRAY_YEAR]);
	MonthHex	= BcdToHex(ReadTime[ARRAY_MONTH]);
	DayHex		= BcdToHex(ReadTime[ARRAY_DAY]);
	TimeTempSource 	= ReadTime[ARRAY_MINUTE] + (ReadTime[ARRAY_HOUR]<<8);
	
	//�ж��Ƿ��ǹ�ů�ڣ�������  ��ֱ�ӷ���
	if((MonthHex >= 0x0B) && (MonthHex <= 0x03))
		{
			CalcYearTemp 	= (YearHex-0x0C)*5*31;
			
			if(MonthHex >= 0x0B)
				{
					CalcMonthTemp = (MonthHex - 0x0B)*31;
				}
			if(MonthHex <= 0x03)
				{
					CalcMonthTemp = (2 + MonthHex - 1)*31;
				}
			
			CalcDayTemp = (DayHex - 0x01)*TIME_NODE_NUM_MAX;
			
			CalcTemp = CalcYearTemp + CalcMonthTemp + CalcDayTemp;
		
		//�Ѿ���λ��ĳһ��Ĵ洢��ʼ��ַ, ��ÿ�����ʼ�洢��ַ��ǰ128�ֽ�Ϊ�洢�ض�����
			*pAddrOut = METERS_DATA_STORE_ADDR + CalcTemp*METERS_DATA_LEN;
		}
		else
		{
			CalcTemp = (DayHex - 0x01)*TIME_NODE_NUM_MAX;
			*pAddrOut = METERS_DATA_STORE_PUBLIC + CalcTemp*METERS_DATA_LEN;
		}
	return NO_ERR;
}
#endif
/****************************************************************************************************
**	�� ��  �� ��: PARA_InitGlobalVar
**	�� ��  �� ��: �Լ������������г�ʼ��
**	�� ��  �� ��: none
**  �� ��  �� ��:
**  ��   ��   ֵ: NO_ERR;
**	��		  ע: 
*****************************************************************************************************/
void PARA_InitGlobalVar(void)
{
	gPARA_MeterNum 			= 0;
	gPARA_SendIndex_Prevous	= 0;
	gSD_Err = 0x00;
	gUpdateBegin = 0;
	gCurrent_Channel = 0;
    /*begin:yangfei added 2013-04-02 for wait sd ok*/
    //delay(0x007fffff);
    /*end:yangfei added 2013-04-02 for wait sd ok*/
	mountSd();                    /* ����SD  ��*/  

	GetVipDataFormSD();
}

/****************************************************************************************************
**	�� ��  �� ��: GetVipDataFormSD
**	�� ��  �� ��: �Լ�������SD���洢�Ĺؼ���Ϣ�������ڴ��������ɣ������Ч��
**	�� ��  �� ��: none
**  �� ��  �� ��:
**  ��   ��   ֵ: NO_ERR;
**	��		  ע: �������ݣ�����Ҫ��SD�����ж��Ƿ�д���־(writeFlag)��
**                ���ò���δд���������øò����ĳ�ʼ������;
**                ��д������������ȡ�������ݣ���ú�������������Ч���жϺ���ȷ��Χ�жϡ�
**                ���ڱ�������Ϣ��CRCУ������֤���ݵ���ȷ�ԡ�
*****************************************************************************************************/
void GetVipDataFormSD(void)
{
	uint16 i	= 0;
	uint8 Err 	= 0;
	uint8 ChannelTemp = 0;
	uint8 mfChannel	  = 0;
	uint8 SystemTime[6] = {0x00};
	uint16 TimeNode[TIME_NODE_NUM_MAX+2] = {0x00};

		
#ifdef DEMO_APP
	
//	uint8 addrtemp_DeLu[7] = {0x89, 0x57, 0x10, 0x21, 0x00, 0x11, 0x11};
//	uint8 addrtemp_LanJE[7] = {0x99, 0x44, 0x88, 0x23, 0x00, 0x00, 0x00};
//	uint8 addrtemp_TG[7] = {0x99, 0x44, 0x88, 0x23, 0x00, 0x00, 0x00};
#endif			

	//uint8 addrtemp[7] = {0x89, 0x57, 0x10, 0x21, 0x00, 0x11, 0x11};
//	uint8 addrtemp[7] = {0x99, 0x44, 0x88, 0x23, 0x00, 0x00, 0x00};
	//uint32 LogicAddr = LOGIC_ADDR_NULL;
	MeterFilePara	para;
	MeterFileType mf;
	TermParaSaveType TermPara;
	UpCommParaSaveType	UpPara;
	ReplenishReadMeterSaveType ReplenishPara;
	//DataStoreParaType	DataPara;
	CPU_SR			cpu_sr;
	uint8  counter=0;
	/*begin:yangfei modified 2012-12-24 find path*/
	char  NodePath[] = "/2012/12/24/timenode";
	/*end   :yangfei modified 2012-12-24*/
	//��ʼ������������

	
	/*begin:yangfei added 2013-4-3 for ��һ�ζ�ȡ��ʧ�ܣ���ֹ�±ߵ�ʧ��*/
     Err=MakeFile("/test");
	if(Err)
		{
		
		}
	else
		{
	
            do{
 //           	OSMutexPend (FlashMutex,0,&Err);
            	Err = SDReadData("/test", &TermPara, sizeof(TermParaSaveType), 0);
 //               OSMutexPost (FlashMutex);
				
            	if(Err!=NO_ERR)	
            	  {
            	   // debug("%s %d read test err=%d!\r\n",__FUNCTION__,__LINE__,Err);
            	   counter++;
            	   
            	  }
				if (counter == 5)
					{
					counter =0;
					break;
					}
            }while(Err!=NO_ERR);
		}
	
	  
    /*end:yangfei modified 2013-4-3 */
	//OSMutexPend (FlashMutex,0,&Err);
	Err = SDReadData("/TERMINAL_PARA", &TermPara, sizeof(TermParaSaveType), 0);
    //OSMutexPost (FlashMutex);
#ifdef DEMO_APP
	
	DemoDateIsGet =0;  
#endif
	if(Err!=NO_ERR)	{
	    debug("%s %d read TERMINAL_PARA err=%d!\r\n",__FUNCTION__,__LINE__,Err);
	  }
	
	if(TermPara.WriteFlag == 0xAA55){
			gPARA_TermPara = TermPara;
			
			//gPARA_TermPara.TermAddr[2]	= 0;
			//gPARA_TermPara.TermAddr[3]	= 0; 
			//gPARA_TermPara.TermAddr[4]	= 0;
			//gPARA_TermPara.TermAddr[5]	= 0; 

			//gPARA_TermPara.HostAddr[2]	= 0;
			//gPARA_TermPara.HostAddr[3]	= 0; 
			//gPARA_TermPara.HostAddr[4]	= 0;
			//gPARA_TermPara.HostAddr[5]	= 0; 
			
			
		}
	else{
			memset(gPARA_TermPara.TermAddr, 0x00, 6);
			memset(gPARA_TermPara.HostAddr, 0x00, 6);
			gPARA_TermPara.TermAddr[0]	= 0x34;
			gPARA_TermPara.TermAddr[1]	= 0x12; 
			//gPARA_TermPara.TermAddr[2]	= 0;
			//gPARA_TermPara.TermAddr[3]	= 0; 
			//gPARA_TermPara.TermAddr[4]	= 0;
			//gPARA_TermPara.TermAddr[5]	= 0; 

			gPARA_TermPara.HostAddr[0]	= 0x01;
			gPARA_TermPara.HostAddr[1]	= 0x00;
			//gPARA_TermPara.HostAddr[2]	= 0;
			//gPARA_TermPara.HostAddr[3]	= 0; 
			//gPARA_TermPara.HostAddr[4]	= 0;
			//gPARA_TermPara.HostAddr[5]	= 0; 
			/*begin:yangfei added 20140224 for add support time_on_off*/
			gPARA_TermPara.DeviceType   = HEAT_METER_TYPE;
			/*end:yangfei added 20140224 for add support time_on_off*/
		}
	/*begin:yangfei modified 2013-10-14 for modified send max number*/
	   gPARA_TermPara.DebugPara.FrameLen_Max = 450;
	   //gPARA_TermPara.DebugPara.FrameLen_Max = PACK_MAX;
	/*end:yangfei modified 2013-10-14 for modified send max number*/


//begin: ��ʼ�����������(����������ʱ����)��
	Err = SDReadData("/REPLENISH_PARA", &ReplenishPara, sizeof(ReplenishPara), 0);
	if(Err!=NO_ERR) {
		  debug("%s %d read REPLENISH_PARA err=%d!\r\n",__FUNCTION__,__LINE__,Err);
	}
	if(ReplenishPara.WriteFlag == 0xAA55){
		gPARA_ReplenishPara = ReplenishPara;

		//�����顣
		if(gPARA_ReplenishPara.MeterReplenishTimes>9)
			gPARA_ReplenishPara.MeterReplenishTimes = 2;
		if((gPARA_ReplenishPara.MeterInterval>9999) || (gPARA_ReplenishPara.MeterInterval<100))
			gPARA_ReplenishPara.MeterInterval = 200;
		if(gPARA_ReplenishPara.ValveReplenishTimes>9)
			gPARA_ReplenishPara.ValveReplenishTimes = 2;
		if((gPARA_ReplenishPara.ValveInterval>9999) || (gPARA_ReplenishPara.ValveInterval<100))
			gPARA_ReplenishPara.ValveInterval = 500;
	}
	else{
		gPARA_ReplenishPara.MeterReplenishTimes = 2;  //Ĭ�ϱ���2�Ρ�
		gPARA_ReplenishPara.MeterInterval = 200;	 //Ĭ�ϳ���ʱ����200ms��
		gPARA_ReplenishPara.ValveReplenishTimes = 2;  //Ĭ�Ϸ�����2�Ρ�
		gPARA_ReplenishPara.ValveInterval = 500;	 //Ĭ�ϳ���ʱ����500ms��


	}

//end: ��ʼ�����������(����������ʱ����)��
	   

	//��ʼ��������Ϣ
	//OSMutexPend (FlashMutex,0,&Err);
	Err = SDReadData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
   // OSMutexPost (FlashMutex);
	if(Err!=NO_ERR)	 {
	    debug("%s %d read METER_FILE_PARA err=%d!\r\n",__FUNCTION__,__LINE__,Err);
	 }
	
	if(para.WriteFlag == 0xAA55){				//�Ѿ�д������Ч��������
			memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
			memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
			memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));/*yangfei added 2013-11-17*/
			memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
			memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
			memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
			memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
			memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
			 
			for(i=0; i<para.MeterStoreNums; i++)
			{
				/*begin:yangfei modified 2012-12-27*/

				//	OSMutexPend (FlashMutex,0,&Err);
				Err = SDReadData("/METER_FILE_ADDR", &mf, sizeof(MeterFileType), sizeof(MeterFileType)*i);
    			//	OSMutexPost (FlashMutex);
				/*end  :yangfei modified 2012-12-27  */
    			gPARA_MeterFiles[i] = mf;
    			
				mfChannel = mf.ChannelIndex-1;				//�������洢ͨ����0��ʼ������վ����ͨ����1��ʼ
				if(mfChannel > (METER_CHANNEL_NUM - 1))  //��ֹ��������Ϣ���󣬵������渳ֵʱ�������鷶Χ��
						mfChannel = METER_CHANNEL_NUM - 1;
				
				//�ȼ�������ͨ������
				/*begin:yangfei deleted 2014-02-27 for ʱ��ͨ����������ȱ��ַΪ0����ַ���±��������*/
				//if(MeterNoBcdCheck(mf.MeterAddr))
				//	{
						ChannelTemp = gPARA_MeterChannelNum[mfChannel]++;
						if(ChannelTemp >= METER_PER_CHANNEL_NUM)  //��ֹ���鳬�ޡ�
							ChannelTemp = METER_PER_CHANNEL_NUM - 1;
						
						gPARA_MeterChannel[mfChannel][ChannelTemp] = i;
				//	}
					
    			//�����¿����ͨ��
				if(MeterNoBcdCheck(mf.ControlPanelAddr))
					{
						ChannelTemp = gPARA_ConPanelChannelNum[mfChannel]++;
						if(ChannelTemp >= METER_PER_CHANNEL_NUM)  //��ֹ���鳬�ޡ�
							ChannelTemp = METER_PER_CHANNEL_NUM - 1;
						
						gPARA_ConPanelChannel[mfChannel][ChannelTemp] = i;
					}
		
				//�����¿����ͨ��
				if(MeterNoBcdCheck(mf.ValveAddr))
					{
						ChannelTemp = gPARA_ValveConChannelNum[mfChannel]++;
						if(ChannelTemp >= METER_PER_CHANNEL_NUM)  //��ֹ���鳬�ޡ�
							ChannelTemp = METER_PER_CHANNEL_NUM - 1;
						
						gPARA_ValveConChannel[mfChannel][ChannelTemp] = i;
					}
			}
			
			gPARA_MeterNum = para.MeterStoreNums;
			
#ifdef DEMO_APP
        
		 if(para.DEMOWriteFlag== 0xAA55)
         DemoDateIsGet =1;  

#endif
		}
	else{				//δд�����Ч��������
			gPARA_MeterNum 			= 0;
			gPARA_SendIndex_Prevous	= 0;
			memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
			memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
			memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
			memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
			memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
			memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
			memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
		}


	
	//��ʼ������ʱ��ڵ���Ϣ
//	OSMutexPend (FlashMutex,0,&Err);
	Err = SDReadData("/TIME_NODE_ADDR", TimeNode, sizeof(TimeNode), 0);
 //   OSMutexPost (FlashMutex);
	if(Err!=NO_ERR)	 {
	    debug("%s %d read TIME_NODE_ADDR err=%d!\r\n",__FUNCTION__,__LINE__,Err);
	  }
	
	if(Err==NO_ERR)//��ֹ������ʱд��������
	{
	/*begin:yangfei added 2013-5-15*/
//	MakeDirInit(1);
	/*end   :yangfei added 2013-5-15*/
	if(TimeNode[0] == 0xAA55){
		//	OS_ENTER_CRITICAL();
			memcpy((uint8 *)gPARA_TimeNodes, (uint8 *)&TimeNode[1], (TIME_NODE_NUM_MAX)*sizeof(uint16));
			gPARA_TimeNodesNum = TimeNode[TIME_NODE_NUM_MAX+1];
			if(gPARA_TimeNodesNum > 24)
				gPARA_TimeNodesNum = 24;
			
		//	OS_EXIT_CRITICAL();
			
	}
	else{
		//	OS_ENTER_CRITICAL();
			memset((uint8 *)gPARA_TimeNodes, 0xFF, (TIME_NODE_NUM_MAX)*sizeof(uint16));
		//	OS_EXIT_CRITICAL();
			gPARA_TimeNodes[0] = 0x0100;
			gPARA_TimeNodes[1] = 0x0700;
			gPARA_TimeNodes[2] = 0x1100;
			gPARA_TimeNodes[3] = 0x2000;
			gPARA_TimeNodesNum = 4;
	}
	
	/*begin:yangfei modified 2012-12-24 for gPARA_TimeNodes save to timenode */
	//OS_ENTER_CRITICAL();
	memcpy(SystemTime,gSystemTime,sizeof(gSystemTime));
	//OS_EXIT_CRITICAL();
	GetFilePath(NodePath, SystemTime, ARRAY_DAY);
	
	//OSMutexPend (FlashMutex, 0, &Err);
	
	Err = SDSaveData(NodePath, &gPARA_MeterNum, sizeof(gPARA_MeterNum),6); 
	Err = SDSaveData(NodePath, gPARA_TimeNodes, sizeof(gPARA_TimeNodes),8); 
	Err = SDSaveData(NodePath, &gPARA_TimeNodesNum, sizeof(gPARA_TimeNodesNum),8+2*TIME_NODE_NUM_MAX); 

	
 	//OSMutexPost (FlashMutex);
	
	//��ʼ������ͨѶ����

	//OSMutexPend (FlashMutex,0,&Err);
	Err = SDReadData("/UP_COMM_PARA", &UpPara, sizeof(UpCommParaSaveType), 0);
   // OSMutexPost (FlashMutex);
	if(Err!=NO_ERR)	{
	    debug("%s %d read UP_COMM_PARAerr=%d!\r\n",__FUNCTION__,__LINE__,Err);
	}

	if(UpPara.WriteFlag == 0xAA55){
			//OS_ENTER_CRITICAL();
			gPARA_UpPara = UpPara;
			//OS_EXIT_CRITICAL();
	}	
	else{
			//OS_ENTER_CRITICAL();
			gPARA_UpPara.HostIPAddr[3] = 0x7A;
			gPARA_UpPara.HostIPAddr[2] = 0x05;
			gPARA_UpPara.HostIPAddr[1] = 0x12;
			gPARA_UpPara.HostIPAddr[0] = 0x01;
			
			gPARA_UpPara.HostPort	   = 0x475;
			gPARA_UpPara.HeartCycle	   = 0x01;
			gPARA_UpPara.RelandDelay   = 0x02;
			gPARA_UpPara.RelandTimes   = 0x05;
			//OS_EXIT_CRITICAL();
	}

	
	Err = SDReadData((const char*)"/LCD_CONTRAST_PARA", (uint8 *)&NVLCDConraston, sizeof(LCDContrastPara),0); 
	if(Err!=NO_ERR)	{
	    debug("%s %d read LCD_CONTRAST_PARA=%d!\r\n",__FUNCTION__,__LINE__,Err);
	}

	if(NVLCDConraston.WriteFlag == 0xAA55){
		 if(NVLCDConraston.LCDContrast<121)
		 	NVLCDConraston.LCDContrast =LCD_CON_VALUE;
	}	
	else{
			//OS_ENTER_CRITICAL();
			NVLCDConraston.LCDContrast = LCD_CON_VALUE;
			//OS_EXIT_CRITICAL();
	}
	SetContrast(NVLCDConraston.LCDContrast);//���öԱ�ֵ
	
	/*end   :yangfei modified 2012-12-24*/
	MakeDirInit(1);
  }
	else{
     		{
     				//	OS_ENTER_CRITICAL();
     					memset((uint8 *)gPARA_TimeNodes, 0xFF, (TIME_NODE_NUM_MAX)*sizeof(uint16));
     					
     				//	OS_EXIT_CRITICAL();
     					gPARA_TimeNodes[0] = 0x0100;
     					gPARA_TimeNodes[1] = 0x0700;
     					gPARA_TimeNodes[2] = 0x1100;
     					gPARA_TimeNodes[3] = 0x2000;
						gPARA_TimeNodesNum = 4;
     					
     		}

	        
			 //��ʼ������ͨѶ����
			
			 //OSMutexPend (FlashMutex,0,&Err);
			 Err = SDReadData("/UP_COMM_PARA", &UpPara, sizeof(UpCommParaSaveType), 0);
			// OSMutexPost (FlashMutex);
			 if(Err!=NO_ERR) {
				 debug("%s %d read UP_COMM_PARAerr=%d!\r\n",__FUNCTION__,__LINE__,Err);
			 }
			
			 if(UpPara.WriteFlag == 0xAA55){
					 //OS_ENTER_CRITICAL();
					 gPARA_UpPara = UpPara;
					 //OS_EXIT_CRITICAL();
			 }	 
			 else{
					 //OS_ENTER_CRITICAL();
					 gPARA_UpPara.HostIPAddr[3] = 0x7A;
					 gPARA_UpPara.HostIPAddr[2] = 0x05;
					 gPARA_UpPara.HostIPAddr[1] = 0x12;
					 gPARA_UpPara.HostIPAddr[0] = 0x01;
					 
					 gPARA_UpPara.HostPort		= 0x475;
					 gPARA_UpPara.HeartCycle	= 0x01;
					 gPARA_UpPara.RelandDelay	= 0x02;
					 gPARA_UpPara.RelandTimes	= 0x05;
					 //OS_EXIT_CRITICAL();
			 }
	        
		Err = SDReadData((const char*)"/LCD_CONTRAST_PARA", (uint8 *)&NVLCDConraston, sizeof(LCDContrastPara),0); 
		if(Err!=NO_ERR)	{
	    	debug("%s %d read LCD_CONTRAST_PARA=%d!\r\n",__FUNCTION__,__LINE__,Err);
		}

		if(NVLCDConraston.WriteFlag == 0xAA55){
			if(NVLCDConraston.LCDContrast<121)
		  		 NVLCDConraston.LCDContrast =LCD_CON_VALUE;
		}	
		else{
			//OS_ENTER_CRITICAL();
			NVLCDConraston.LCDContrast = LCD_CON_VALUE;
			//OS_EXIT_CRITICAL();
    	}
	
		SetContrast(NVLCDConraston.LCDContrast);//���öԱ�ֵ

	
		memcpy(SystemTime,gSystemTime,sizeof(gSystemTime));
		GetFilePath(NodePath, SystemTime, ARRAY_DAY);
				   
		Err=MakeFile(NodePath);
		 if(Err){
						  
		 }
	 	else {
					  
		    do{
				Err = SDReadData(NodePath, (uint8 *)&TimeNode[1], (TIME_NODE_NUM_MAX)*sizeof(uint16), 8);
								  
				if(Err!=NO_ERR) {
					 counter++; 
				}
				if (counter ==5 ){
                    counter =0;
                    break;
                 }
			}while(Err!=NO_ERR);
		}

		Err = SDSaveData(NodePath, &gPARA_MeterNum, sizeof(gPARA_MeterNum),6); 
		Err = SDSaveData(NodePath, gPARA_TimeNodes, sizeof(gPARA_TimeNodes),8); 
		Err = SDSaveData(NodePath, &gPARA_TimeNodesNum, sizeof(gPARA_TimeNodesNum),8+2*TIME_NODE_NUM_MAX); 
				   
		MakeDirInit(1);//�ٽ���һ��


	}


	
}


/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/
