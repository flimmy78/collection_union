/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)                                                          
**  ��   ��   ��: SysLog.c
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��9��10�� 
**  ��        ��: ��־��¼ϵͳ������Ϣ
**	�� ��  �� ¼:   	
*****************************************************************************************************/

/********************************************** include *********************************************/
#include <includes.h>
#include <stdarg.h>

/********************************************** define *********************************************/
#define 	LOG_SD_SAVE_SIZE			512
static uint8 LOG_Flag_Level = LOG_LEVEL_INFO;//muxiaoqing add

/********************************************** global *********************************************/
uint8 gLOG_LogLevelFlag	= LOG_LEVEL_WARN;
extern uint8  EXTI_ERRO_Happen ;

/********************************************** static *********************************************/

/****************************************************************************************************
**	�� ��  �� ��: LOG_QueueCreate
**	�� ��  �� ��: ������־��¼��������ݶ��У���־���ź���
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void LOG_QueueCreate(void)
{
	//�������Ͷ���
    if (QueueCreate((void *)gLOG_SystemLog,
                     sizeof(gLOG_SystemLog),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
    
    //������������־���ź���
	LogQueSem = OSSemCreate(0);
    if (LogQueSem == NULL){
        while(1);
    }
	
	QueueFlush((void*)gLOG_SystemLog); 								//��ս��ն���
    
}

/****************************************************************************************************
**	�� ��  �� ��: LOG_ParaInit
**	�� ��  �� ��: ��־��¼�ļ���ȫ�ֱ�����ʼ��
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void LOG_ParaInit(void)
{
	//uint8 Err 		= 0x00;
	//uint32 AddrTemp = 0x00;
		
	gLOG_LogLevelFlag	= LOG_ALL;  
}

/****************************************************************************************************
**	�� ��  �� ��: TaskLog
**	�� ��  �� ��: ��־��¼���������������־��Ϣ���ɸ�������	
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: ��ѡ��Ӵ���������־��Ϣ��Ҳ���Խ���־��Ϣ�洢��SD����
*****************************************************************************************************/
void TaskLog(void *pdata)
{
	uint8 i,j;
	uint8 err;
	uint16 BufLen = 0x00;
	uint8 LogBuf[1024] = {0x00};
	uint8 *LogTemp = LogBuf;
	uint8 LogData = 0x00;
	uint8 CirTimes, Residual;
	//uint32 SD_SaveAddr = 0x8000000;
	CPU_SR		cpu_sr;
	/*begin:yangfei added 2012-12-24*/
	char    LogPath[]="/2012/12/24/log.txt";
	uint8   HexSystemTime[6] 			= {0x00};
	/*end   :yangfei added 2012-12-24*/
	#if 0      /*yangfei deleted 2012-12-19*/
	LOG_ParaInit();
	#endif
	QueueFlush((void*)gLOG_SystemLog); 								//��ս��ն���
	
	while(1)
	{
		do{
			FeedTaskDog();
			OSSemPend(LogQueSem, OS_TICKS_PER_SEC*6, &err);	
		}while(err != OS_ERR_NONE);

		/*begin:yangfei added 2012-12-24*/
		OS_ENTER_CRITICAL();
		memcpy(HexSystemTime,gSystemTime,6);
		OS_EXIT_CRITICAL();
		GetFilePath(LogPath, HexSystemTime,  ARRAY_DAY)	;
		/*end   :yangfei added 2012-12-24*/
		BufLen = QueueNData(gLOG_SystemLog);
		LogTemp = LogBuf;

		if(BufLen >= LOG_SD_SAVE_SIZE)
			{
				CirTimes = BufLen/4;
				Residual = BufLen%4;
				
				for(i=0; i<CirTimes; i++)
				{
					OS_ENTER_CRITICAL();
					for(j=0; j<4; j++)
					{
						err = QueueRead_Int(&LogData, (void*)gLOG_SystemLog);
						if(err == QUEUE_EMPTY)	break;
						if(err!=QUEUE_OK)		while(1);			//��Ӧ�÷������ִ���
						*LogTemp++ = LogData;
					}
					OS_EXIT_CRITICAL();
				}
				
				OS_ENTER_CRITICAL();
				for(j=0; j<Residual; j++)
				{
					err = QueueRead_Int(&LogData, (void*)gLOG_SystemLog);
					if(err == QUEUE_EMPTY)	break;
					if(err!=QUEUE_OK)		while(1);				//��Ӧ�÷������ִ���
					*LogTemp++ = LogData;
				}
				OS_EXIT_CRITICAL();	
				

				OSMutexPend (FlashMutex,0,&err);

				err = SDSaveLog(LogPath, LogBuf, BufLen,0);

		    	OSMutexPost (FlashMutex);

			}
	}
}
/*add by yangmin 2013-09-17*/
void LOG_HandleLogBuf()
{
      uint8 i,j;
	uint8 err;
	CPU_SR		cpu_sr;
	uint8 LogData = 0x00;
	uint16 BufLen = 0x00;
	uint8 CirTimes, Residual;
      uint8 LogBuf[1024] = {0x00};
	uint8 *LogTemp = LogBuf;
	/*begin:yangfei added 2012-12-24*/
	char    LogPath[]="/2012/12/24/log.txt";
	uint8   HexSystemTime[6] 			= {0x00};
	/*end   :yangfei added 2012-12-24*/
   
/*begin:yangfei added 2012-12-24*/
	OS_ENTER_CRITICAL();
	memcpy(HexSystemTime,gSystemTime,6);
	OS_EXIT_CRITICAL();
	
	BufLen = QueueNData(gLOG_SystemLog);
	GetFilePath(LogPath, HexSystemTime,  ARRAY_DAY);

	CirTimes = BufLen/4;
	Residual = BufLen%4;
				
	for(i=0; i<CirTimes; i++)
	{
		OS_ENTER_CRITICAL();
		for(j=0; j<4; j++)
		{
			err = QueueRead_Int(&LogData, (void*)gLOG_SystemLog);
			if(err == QUEUE_EMPTY)	break;
			if(err!=QUEUE_OK)		while(1);			//��Ӧ�÷������ִ���
			*LogTemp++ = LogData;
		}
		OS_EXIT_CRITICAL();
	}
				
	OS_ENTER_CRITICAL();
	for(j=0; j<Residual; j++)
	{
		err = QueueRead_Int(&LogData, (void*)gLOG_SystemLog);
		if(err == QUEUE_EMPTY)	break;
		if(err!=QUEUE_OK)		while(1);				//��Ӧ�÷������ִ���
		*LogTemp++ = LogData;
	}
	OS_EXIT_CRITICAL();	
				
	OSMutexPend (FlashMutex,0,&err);
	err = SDSaveLog(LogPath, LogBuf, BufLen,0);
     OSMutexPost (FlashMutex);

}
void LOG_HandleEXTIBuf(const uint8 *pInLog,uint8 Index)

{
      uint8 i,j;
	uint8 err;
	CPU_SR		cpu_sr;
	uint8 LogData = 0x00;
	uint16 BufLen = 0x00;
	uint8 CirTimes, Residual;
 //     uint8 LogBuf[1024] = {0x00};
//	uint8 *LogTemp = LogBuf;
	char    LogPath[]="/EXTI_PWR_ERR_REC"; 
    char	 MBUSLogPath[]="/EXTI_MBUS_ERR_REC";
	uint8   HexSystemTime[6] 			= {0x00};
	uint8 Res;
    ERRLogFileType ERRLOGFile; 	
    ERRLogFilePara ERRLOGPara; 
    ERRLogMBUSFileType ERRLOGMBUSFile; 	
    ERRLogMBUSFilePara ERRLOGMBUSPara; 

#if 1	
		
	OSMutexPend (FlashMutex,0,&err);
	if(Index==2)//MBUS��·
		{
		Res = SDReadData((const char*)"/EXTI_MBUS_ERR_PARA", (uint8 *)&ERRLOGMBUSPara, sizeof(ERRLogMBUSFilePara),0); 

	    }
	else
	
    Res = SDReadData((const char*)"/EXTI_PWR_ERR_PARA", (uint8 *)&ERRLOGPara, sizeof(ERRLogFilePara),0); 

	OSMutexPost (FlashMutex);
	if(Res!=NO_ERR) 
	  {
		debug("%s %d read LOG_HandleEXTIBuf err=%d!\r\n",__FUNCTION__,__LINE__,Res);
//		return;
	  }

	if(Index==2)//MBUS��·
	{
		if(ERRLOGMBUSPara.WriteFlag == 0xAA55)
		{	   
		  if(ERRLOGMBUSPara.LOGStoreNums<11)//�洢10��log��Ϣ
			
		  ERRLOGMBUSPara.LOGStoreNums++;
			else
		  ERRLOGMBUSPara.LOGStoreNums=1;		
		}
		else
			{
			  ERRLOGMBUSPara.WriteFlag = 0xAA55;
			  ERRLOGMBUSPara.LOGStoreNums =1;
			}

	}
	else
		{
        	if(ERRLOGPara.WriteFlag == 0xAA55)
        	{      
        	  if(ERRLOGPara.LOGStoreNums<11)//�洢10��log��Ϣ
        	  	
        	  ERRLOGPara.LOGStoreNums++;
        	  	else
        	  ERRLOGPara.LOGStoreNums=1;		
        	}
            else
            	{
                  ERRLOGPara.WriteFlag = 0xAA55;
        		  ERRLOGPara.LOGStoreNums =1;
            	}
		}
	
	OSMutexPend (FlashMutex,0,&err);
	
	if(Index==2)//MBUS��·
	{
	
	err = SDSaveData((const char*)"/EXTI_MBUS_ERR_PARA", (uint8 *)&ERRLOGMBUSPara, sizeof(ERRLogMBUSFilePara),0);
	}
	else
	err = SDSaveData((const char*)"/EXTI_PWR_ERR_PARA", (uint8 *)&ERRLOGPara, sizeof(ERRLogFilePara),0);
     OSMutexPost (FlashMutex);
	#if 0
	BufLen = QueueNData(gLOG_SystemLog);

	CirTimes = BufLen/4;
	Residual = BufLen%4;
				
	for(i=0; i<CirTimes; i++)
	{
		OS_ENTER_CRITICAL();
		for(j=0; j<4; j++)
		{
			err = QueueRead_Int(&LogData, (void*)gLOG_SystemLog);
			if(err == QUEUE_EMPTY)	break;
			if(err!=QUEUE_OK)		while(1);			//��Ӧ�÷������ִ���
			*LogTemp++ = LogData;
		}
		OS_EXIT_CRITICAL();
	}
				
	OS_ENTER_CRITICAL();
	for(j=0; j<Residual; j++)
	{
		err = QueueRead_Int(&LogData, (void*)gLOG_SystemLog);
		if(err == QUEUE_EMPTY)	break;
		if(err!=QUEUE_OK)		while(1);				//��Ӧ�÷������ִ���
		*LogTemp++ = LogData;
	}
	OS_EXIT_CRITICAL();	
	#endif
	
	if(Index==2)//MBUS��·
	    {
	    #if 0
        	memcpy(ERRLOGMBUSFile.LOGsting,pInLog,96);	
        	ERRLOGMBUSFile.LOGLevel = LOG_LEVEL_ERROR;
        	OSMutexPend (FlashMutex,0,&err);
        	err = SDSaveData(MBUSLogPath, (uint8 *)&ERRLOGMBUSFile, sizeof(ERRLogMBUSFileType),(ERRLOGMBUSPara.LOGStoreNums-1)*128);
             OSMutexPost (FlashMutex);
		#endif
	    }
	else
		{
        	memcpy(ERRLOGFile.LOGsting,pInLog,96);	
        	ERRLOGFile.LOGLevel = LOG_LEVEL_ERROR;
        	OSMutexPend (FlashMutex,0,&err);
        	err = SDSaveData(LogPath, (uint8 *)&ERRLOGFile, sizeof(ERRLogFileType),(ERRLOGPara.LOGStoreNums-1)*128);
            OSMutexPost (FlashMutex);
		}
	 if(err!=NO_ERR) 
	   {
		 debug("%s %d read LOG_HandleEXTIBuf err=%d!\r\n",__FUNCTION__,__LINE__,err);
//		 return;
	   }
	 #endif
	if(Index ==1)
		{
            //����
        	//NVIC_SETFAULTMASK();
        	//NVIC_GenerateSystemReset();
		}
	else if(Index ==2)//MBUS��·
		{
       //   DISABLE_MBUS();
	    }
}

/****************************************************************************************************
**	�� ��  �� ��: LOG_WriteSysLog
**	�� ��  �� ��: �����¼����־�������뵽��־���ݶ�����	
**	�� ��  �� ��: uint8 *pInLog	-- ��־����ָ��; uint16 LogLen -- ��־���ݳ���
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR
**	��		  ע: ���������ù��жϷ�ʽ��������Ϊ�����룬���Զ���������������.
**				  ��־��ϢΪ���ݻ���Ҫ���ø�ʽ����ʱʹ�øú���
*****************************************************************************************************/
uint8 LOG_WriteSysLog(const uint8 *pInLog, uint16 LogLen)
{
	uint32 N=4; //Ϊ�˼���ϵͳ���ж�ʱ��,�����ٿ����жϴ������������дN���ֽڹؿ�һ���ж�
				//Ϊ�˽���ϵͳ���ж��ӳ�ʱ�䣬ԭ����N ��Ӧ����6��
	uint32 i,j,p,x,y;
	OS_CPU_SR cpu_sr;

   	LOG_assert_param( pInLog == NULL );

	if(pInLog==NULL)	while(1);			//���ִ���
	
	//======================�ȴ���־��¼���п��ÿռ�>=n
	if(QueueNotEnoughPend((void*)gLOG_SystemLog, LogLen)){
		return 1;
	}

	//======================
	x=LogLen/N;	y=LogLen%N;	p=0;
	for(i=0;i<x;i++){
		OS_ENTER_CRITICAL();
		for(j=0;j<N;j++){
			QueueWrite_Int((void *)gLOG_SystemLog, pInLog[p++]); 					//���������
		}
		OS_EXIT_CRITICAL();
	}

	OS_ENTER_CRITICAL();
	for(j=0;j<y;j++){
		QueueWrite_Int((void *)gLOG_SystemLog, pInLog[p++]); 					//���������
	}
	OS_EXIT_CRITICAL();
	
	OSSemPost(LogQueSem);

	return NO_ERR;
}

/****************************************************************************************************
**	�� ��  �� ��: LOG_WriteSysLog_Format
**	�� ��  �� ��: ��ʽ��������־��¼	
**	�� ��  �� ��: uint8 Flag -- ��־�ȼ���־;	const char *format -- ��ʽ���������
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR
**	��		  ע: ��ʽ���������룬������ݳ���Ϊ256�ֽ�, ������256�������Ļᱻ�ض�
*****************************************************************************************************/
uint8 LOG_WriteSysLog_Format(uint8 Flag, const char *format, ...)
{
//	uint8 err;
	char StringBuf[256] 	   	= {0x00};
//	uint8 SecondTemp		   	= 0x00;
	uint8 LevelTemp				= 0x00;
	uint8 lTimeDate[6]			= {0x00};
	int16 StringLen				= 0x00;
	//uint32 TimeCounter		   	= 0x00;
	va_list argptr;
	CPU_SR		cpu_sr;
    uint8  EXTIhappenIndex =0;
	LOG_assert_param( format == NULL );
		

	if(format==NULL)		while(1);
	
	//��ֹ2��ָ��ʱ������С��Ƶ����ȡʱ��
	OS_ENTER_CRITICAL();
	LevelTemp = gLOG_LogLevelFlag;
	OS_EXIT_CRITICAL();
	/*begin:yangfei modifed 2013-04-04*/
    #if  0
	//������־�ȼ����й��˼�¼
	if( !(LevelTemp&Flag) )		{return NO_ERR;}
    #else
    if( LevelTemp<Flag )		{return NO_ERR;}
    #endif
    /*end:yangfei modifed 2013-04-04*/
	ReadDateTime(lTimeDate);
	
	// \r	
	StringBuf[0] = '\r';
	// \n
	StringBuf[1] = '\n';
	//��	
	StringBuf[2] = UINT8_TO_ASCII_H((lTimeDate[ARRAY_YEAR]>>4)&0x0F);
	StringBuf[3] = UINT8_TO_ASCII_L(lTimeDate[ARRAY_YEAR]&0x0F);
	// - 
	StringBuf[4] = '-';
	//��
	StringBuf[5] = UINT8_TO_ASCII_H((lTimeDate[ARRAY_MONTH]>>4)&0x0F);
	StringBuf[6] = UINT8_TO_ASCII_L(lTimeDate[ARRAY_MONTH]&0x0F);
	// - 
	StringBuf[7] = '-';
	//��
	StringBuf[8] = UINT8_TO_ASCII_H((lTimeDate[ARRAY_DAY]>>4)&0x0F);
	StringBuf[9] = UINT8_TO_ASCII_L(lTimeDate[ARRAY_DAY]&0x0F);
	// �ո� 
	StringBuf[10] = ' ';
	//ʱ	
	StringBuf[11] = UINT8_TO_ASCII_H((lTimeDate[ARRAY_HOUR]>>4)&0x0F);
	StringBuf[12] = UINT8_TO_ASCII_L(lTimeDate[ARRAY_HOUR]&0x0F);
	// : 
	StringBuf[13] = ':';
	//��
	StringBuf[14] = UINT8_TO_ASCII_H((lTimeDate[ARRAY_MINUTE]>>4)&0x0F);
	StringBuf[15] = UINT8_TO_ASCII_L(lTimeDate[ARRAY_MINUTE]&0x0F);
	// : 
	StringBuf[16] = ':';
	//��
	StringBuf[17] = UINT8_TO_ASCII_H((lTimeDate[ARRAY_SECOND]>>4)&0x0F);
	StringBuf[18] = UINT8_TO_ASCII_L(lTimeDate[ARRAY_SECOND]&0x0F);
	// -->�ո�
	StringBuf[19] = '-';
	StringBuf[20] = '-';
	StringBuf[21] = '>';
	StringBuf[22] = ' ';
	
	va_start(argptr, format);
	StringLen = vsnprintf(&StringBuf[23], sizeof(StringBuf), format, argptr);
	if(StringLen)			;						//�˴���Ը��Ƶĳ��Ƚ����ж�
	va_end(argptr);
	LOG_Flag_Level = Flag;
	if(EXTI_ERRO_Happen)
		{
		EXTIhappenIndex = EXTI_ERRO_Happen;
		EXTI_ERRO_Happen =0;
		LOG_HandleEXTIBuf((uint8 *)StringBuf,EXTIhappenIndex);

	    }
	else
		{
         /*begin:yangfei added 2013-02-20*/
         if(LevelTemp>LOG_LEVEL_INFO)
           {
           debug("%s\r\n",StringBuf);
           }
         else
           {
           LOG_WriteSysLog((uint8 *)StringBuf, strlen(StringBuf));
           }
		}
   /*end:yangfei added 2013-02-20*/
	
	return NO_ERR;
}

/****************************************************************************************************
**	�� ��  �� ��: Log_assert_failed
**	�� ��  �� ��: ���������жϴ�����	
**	�� ��  �� ��: const char *format -- ��ʽ���������
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void LOG_assert_failed(uint8* file, uint32 line)
{
	LOG_WriteSysLog_Format(LOG_LEVEL_DEBUG, "ERROR: <<%s.c>> Line %5d", file, line);
}

/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/
