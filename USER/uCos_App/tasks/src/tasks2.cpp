/*******************************************Copyright (c)*******************************************
**									ɽ������ռ似����˾(�����ֲ�)                                **                            
**  ��   ��   ��: tasks2.cpp																      **
**  ��   ��   ��: ������																		  **
**	��   ��   ��: 0.1																		      **
**  �� ��  �� ��: 2012��9��20�� 													     	      **
**  ��        ��: �����趨��ʱ�䣬��ʼ��ȡ�����������			      						      **
**	�� ��  �� ¼:   																			  **
****************************************************************************************************/
/********************************************** include *********************************************/
#include <includes.h>
#include "app_flashmem.h"
#include "app_down.h"
#include "tasks_up.h"
#include "tasks2.h"
#include "app_up.h"
#include "Ul_array.h"
/****************************************************************************************************/
/********************************************** global **********************************************/
OS_EVENT * ReadAllCurMbox;

OS_EVENT* MbusChannleOnMbox;
OS_EVENT* MbusChannleOFFMbox;

uint8 gu8CreateNewDay = 1; //��һ�쿪ʼ��ǣ�1-��һ�졣���տ�������һ�쿪ʼʱ���Ϊ1.

/****************************************************************************************************/
/********************************************** static **********************************************/
//�������ջ����
static OS_STK	TaskClockStk[256];
static OS_STK	TaskReadAllCurStk[TASK_STK_SIZE];
static OS_STK	TaskLogStk[TASK_STK_SIZE];
static OS_STK	TaskMbusShortCStk[TASK_STK_SIZE];
static OS_STK	TaskMbusShortCTimerStk[TASK_STK_SIZE];

static uint8 counter1=0;
static 	 uint8 PreTime[8] = {0x00};
extern uint8  EXTI_ERRO_Happen ;//muxiaoqing
vs32  Timer_counter =0;
vs32  Channel1_Timer_counter =0;
vs32  Channel2_Timer_counter =0;
vs32  Channel3_Timer_counter =0;
vs32  Channel4_Timer_counter =0;
#define	FIRST_COUNTER_TIME 30
#define	SECOND_COUNTER_TIME 35
#define	THIRD_COUNTER_TIME 50



vs32 Channel1_Start_Counter =0;

vs32 Channel2_Start_Counter =0;
vs32 Channel3_Start_Counter =0;
vs32 Channel4_Start_Counter =0;


extern ul_array_t  *gpValve_Array;
uint8  BinaryDA1[32]={0};

void DecimalToBinary1(int num,uint8 *BinaryArry)
	{   
	uint8 *Binary;
	Binary =BinaryArry;
	if(num ==0)    
		{
		return;   
	    }  
	else   
		{    
		if(num/2)
			{			    
        		counter1++;
        		DecimalToBinary1(num/2,Binary);           
        		Binary[31-(--counter1)]= num%2;
			}
		else
			{
                Binary[31-counter1]=num%2;
			
		    }
		}
	}

/****************************************************************************************************/
/********************************************************************************************************
**  �� ��  �� ��: TaskClock     									                                   **
**	�� ��  �� ��: ���ó���ʱ��                                                                         **			
**	�� ��  �� ��: pdata													                               **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void  TaskClock( void *pdata )
{
	uint8 Err = 0;
	uint8 TNum;
	uint8 ReadDate[6];
	uint8 CmdSenddFlag		= 0;					//����ڵ㷢�ͳ����źŵı�־
	uint8 TimeNodeFlag		= 0;					//����ʱ��㵽�����ͳ�������
	uint8 NodeIndex			= 0;
	int8  TimeOffset		= 0;
	uint8 SendMsg[2]		= {0x00};
	uint16 CurrentTime		= 0;
//	uint32 LogicAddr		= 0;	
	/*begin:yangfei added 2012-12-26*/
	uint8  OldMonth          =5;
	/*end   :yangfei added 2012-12-26*/

	uint8 OldDay = 0;
	char lcYesterdayPath[] = "/2015/06/09/1200";
	uint8 SystemTime[6] = {0x00};


//	uint16 ReadingMeterSn	= 0x00;
	/*begin:yangfei added 2012-12-24 find path*/
//	uint8 SystemTime[6] = {0x00};
//	char  DataPath[] = "/2012/12/24/1530";
//	uint8 Res = 0;
//	CPU_SR			cpu_sr;	

	pdata = pdata;

	TNum=0;
	
	memset(PreTime, 0x00, 8);
 	ReadDateTime(ReadDate);
	memcpy(PreTime, ReadDate, 6);

//"����"��ʼ��
	memcpy(SystemTime, gSystemTime, 6);
	if(SystemTime[ARRAY_DAY] == 1)   //���죬���Ͻ���
		SystemTime[ARRAY_DAY] = 30;
	else
		SystemTime[ARRAY_DAY] -= 1;
	
	GetFilePath(lcYesterdayPath, SystemTime,ARRAY_DAY);
	memcpy(gcYesterdayPath,lcYesterdayPath,sizeof(lcYesterdayPath));

	
	/*begin:yangfei added 2012-12-26*/
	//MakeDirInit(1);
	/*end   :yangfei added 2012-12-26*/
//	LogicAddr = 0x4C800000;	
	while(1)
	{
		FeedTaskDog(); 
	 	OSTimeDly(15*OS_TICKS_PER_SEC);

		TNum=(TNum+1)%4; //60 second
	 	if(TNum == 0)											//ÿ���Ӽ��һ��ʱ��
	 	{
	 			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskClock] One Minute Check OK!"); 
	 			//����Ƿ��˳���ʱ���
	 			ReadDateTime(ReadDate);
				if( (ReadDate[ARRAY_HOUR]==0x00)&&(ReadDate[ARRAY_MINUTE]==0x00) )
				{
					gu8CreateNewDay = 1; //��һ�쿪ʼ����Ǵ�����һ��Ļ�����Ϣ��
					TIME_UpdateSystemTime();  					//����ϵͳʱ��
				}
				
				if(gu8CreateNewDay == 1){
					STORE_InitTimeNodePara(ReadDate, 0); //��������Ҫ������ÿ��0��0��׼ʱ������һ��Ļ�����Ϣ��
					gu8CreateNewDay = 0;
				}
				
				CurrentTime = (ReadDate[ARRAY_HOUR]<<8) | ReadDate[ARRAY_MINUTE];
	 			
	 			Err = STORE_GetTimeNodeInfo(CurrentTime, &NodeIndex, &TimeOffset);
				if(Err == NO_ERR)
				{
					if( (0<TimeOffset) && (TimeOffset<20) )		//5������ һ��Ҫ����ʱ���������ȥ
	 					{
	 						TimeNodeFlag = 1;
	 						//LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskClock] Ready To Read Meter!"); 
	 					}
	 				else
	 					{
							TimeNodeFlag = 0;
	 						CmdSenddFlag = 0;
	 						//LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskClock] Not Ready To Read Meter!");
	 					}
				}/*
				LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskClock] Time Check Info: NodeIdex is %2XH, TimeOffset is %2XH,"
									   "The Function <STORE_GetTimeNodeInfo> Return Value is %2XH",
									   NodeIndex, TimeOffset, Err);*/
				/*
				uint8 test = 0xA1;
				USART1Send(&test, 1);
				USART1Send(ReadDate, 6);
				USART1Send(&NodeIndex, 1);
				USART1Send((uint8 *)&TimeOffset, 1);
				USART1Send(&Err, 1);
				*/
				/*  ����ʱ������ʱ������֮��ĳ�ͻ����ν��
				//����ʱ���ж�Ҫ�ϸ�
				testcmp = CmpTime(ReadDate, PreTime);
				if(testcmp > 0)		//ʱ����ȷ
				{
					memcpy(PreTime, ReadDate, 6);
				}
				else				//ʱ�䲻��ȷ
				{
					PreTime[7]++;
					OSMutexPend (FlashMutex,0,&Err);
    				Err =AppSdWrRdBytes(LogicAddr, 8, PreTime, SD_WR);
    				OSMutexPost (FlashMutex);
					LogicAddr += 8;

					AddMin(PreTime, 1);		//��������ʱ��	
					UserSetDateTime(PreTime);

				}
				*/
				if(OldMonth != gSystemTime[ARRAY_MONTH])
	 			{
	 					OldMonth = gSystemTime[ARRAY_MONTH];
				
						MakeDirInit(OldMonth);
	 			}

				if(OldDay != gSystemTime[ARRAY_DAY]){
					StoreYesterday();
					OldDay = gSystemTime[ARRAY_DAY];
				}


			
	 		}
	 	
	 	if( (CmdSenddFlag==0) && (TimeNodeFlag==1) )		//ÿ��ʱ��ڵ� ֻ����һ����������
		{	
	 			STORE_InitTimeNodePara(ReadDate, NodeIndex);
	 			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskClock]Init The Time Node Para!");
	 			CmdSenddFlag = 1;							//��ֹ5������ÿ���Ӷ�����һ����������

				if((gu8ReadAllMeterFlag!=0) && (gu8ReadAllMeterFlag!=1)){  //��ֹ����
					gu8ReadAllMeterFlag = 0;
				}
				
				if(gu8ReadAllMeterFlag == 0){  //ֻ���ڲ�����ʱ�ŷ��͡�
					SendMsg[0] =  TIMING_ALL;
	 				OSQPost(CMD_Q, (void*)SendMsg);
	 				LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskClock] Post Msg Queue For Reading all of HeatMeters!");
				}
				else{
					LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: Ignore this time Reading all of HeatMeters!");
				}
				
			}

   	}
}

/********************************************************************************************************
**  �� ��  �� ��: TaskReadAllCur     									                               **
**	�� ��  �� ��: ��ȡ���������������                                                                 **			
**	�� ��  �� ��: pdata													                               **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void  TaskReadAllCur(void *pdata)
{
	uint8 err;
	uint8 *pMsg = NULL;
	
	
	pdata = pdata;
	gpValve_Array = ul_array_create();
	while(1)
	{
		
	 	do{
	 		FeedTaskDog();
	 		pMsg = (uint8 *)OSQPend(CMD_Q, 10*OS_TICKS_PER_SEC, &err);
	 	}while( (err!=OS_ERR_NONE) || (pMsg == NULL) );
	 	
	 	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskReadAllCur] Recive a Message, The Msg ID is 0x%X", *pMsg);
	 	switch(*pMsg)
	 	{
			case MSG_1D:
			case TIMING_ALL:
	 			{
					gu8ReadAllMeterFlag = 1;//��ǿ�ʼ��ȫ��
	 				READ_ReadAllMeters();
					gu8ReadAllMeterFlag = 0;//��ǳ�ȫ�������
	 				break;
	 			}
	 		case MSG_18:
	 			{
	 				DEAL_MSG18_ForAll(++pMsg);
	 				break;
	 			}
	 		case MSG_1C:
	 			{
	 				DEAL_MSG1C_ForAll(++pMsg);
	 				break;
	 			}
	 		default:
	 			break;
	 	}
	 	
   		OSTimeDly(2*OS_TICKS_PER_SEC); 

   	}
}


/********************************************************************************************************
**  �� ��  �� ��: TaskMbusShortC     									                               **
**	�� ��  �� ��: Mbus ��·����                 **			
**	�� ��  �� ��: pdata													                               **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void  TaskMbusShortC(void *pdata)
{
	uint8 i = 0;
	uint8 lu8MBusState = 0;

	while(1){
				//begin: ������.PB5,�ߵ�ƽ�죬�͵�ƽֹͣ�졣
		
		
				if((gu8ErrFlag==1) && (gu16ErrOverTime<4000)){
					GPIO_SetBits(GPIOB,GPIO_Pin_5);
					OSTimeDlyHMSM(0,0,0,100);
					GPIO_ResetBits(GPIOB,GPIO_Pin_5);
					OSTimeDlyHMSM(0,0,0,200);
					GPIO_SetBits(GPIOB,GPIO_Pin_5);
					OSTimeDlyHMSM(0,0,0,100);
					GPIO_ResetBits(GPIOB,GPIO_Pin_5);
					
					gu16ErrOverTime++;
				}
				else{
					OSTimeDlyHMSM(0,0,0,500);

				}
		
		
		
				//end: ��������

				FeedTaskDog();
		
				//begin:����ж�MBUSͨ���Ƿ��·����ȴ�3�롣
				if(1 == gu8MBusShortRising){
					for(i=0;i<60;i++){
						OSTimeDlyHMSM(0,0,0,50);
						lu8MBusState = STA_MBUS_OFF();
						if(lu8MBusState == 0){	//���MBUS��·�������Ϊ�͵�ƽ����˵��û��������·��
							break;
						}
						else{
							gu8MBusShortTime++;
		
						}
		
					}
		
					if(i >= 59){
						DisableAllMBusChannel();  //����MBUS�жϣ��ж�����ͨ����
						gu8MBusShortFlag[gu8NowMBUSChannel-1] = 1;	//��Ƕ�ӦMBUSͨ��ͨ����·
		
					}
					
					gu8MBusShortRising = 0; //�����Ƿ����·���ж����Ҫ�����ǡ�
		
					
		
					
		
				}
				else{
					OSTimeDlyHMSM(0,0,0,500);

				}
		
				//end:����ж�MBUSͨ���Ƿ��·��


	}

}
/********************************************************************************************************
**  �� ��  �� ��: TaskMbusShortC_Timer     									                               **
**	�� ��  �� ��: Mbus ��·ʱ�����                 **			
**	�� ��  �� ��: pdata													                               **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void  TaskMbusShortC_Timer(void *pdata)
{

}
/********************************************************************************************************
**  �� ��  �� ��: CreateTasks2      									                               **
**	�� ��  �� ��: ����Tasks2                                                                           **			
**	�� ��  �� ��: none												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void CreateTasks2(void)
{
	uint8 err=0;
    
    err=OSTaskCreate(TaskClock, (void *)0,&TaskClockStk[256 - 1], PRIO_TASK_CLOCK);
	if(err!=OS_ERR_NONE){
    	while(1);
    }
    
    err=OSTaskCreate(TaskReadAllCur, (void *)0,&TaskReadAllCurStk[(TASK_STK_SIZE) - 1],PRIO_TASK_READ_ALL_CUR);
	if(err!=OS_ERR_NONE){
    	while(1);
    }
	
	err=OSTaskCreate(TaskLog, (void *)0,&TaskLogStk[(TASK_STK_SIZE) - 1],PRIO_TASK_LOG);
	if(err!=OS_ERR_NONE){
    	while(1);
    } 

	err=OSTaskCreate(TaskMbusShortC, (void *)0,&TaskMbusShortCStk[(TASK_STK_SIZE) - 1],PRIO_TASK_MBUSSHORTC);
	if(err!=OS_ERR_NONE){
    	while(1);
		}

//	err=OSTaskCreate(TaskMbusShortC_Timer, (void *)0,&TaskMbusShortCTimerStk[(TASK_STK_SIZE) - 1],37);
//		if(err!=OS_ERR_NONE){
//			while(1);		
//    } 

}

/********************************************************************************************************
**  �� ��  �� ��: CreateMboxs2       									                               **
**	�� ��  �� ��: ����Mboxs2                                                                           **			
**	�� ��  �� ��: none												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void CreateMboxs2(void)
{
	UpAskMeterSem = OSSemCreate(1);
    if (UpAskMeterSem == NULL)
    {
        while(1);
    }

	
	METERChangeChannelSem = OSSemCreate(1);
    if (METERChangeChannelSem == NULL)
    {
        while(1);
    }
    
    CMD_Q = OSQCreate(&CmdMsgQ[0], 10);
	if (Str_Q == NULL)
    {
        while (1);
    }
  	MbusChannleOnMbox= OSMboxCreate((void*)0);
	if (MbusChannleOnMbox == NULL)
    {
        while (1);
    } 
	MbusChannleOFFMbox= OSMboxCreate((void*)0);
	if (MbusChannleOFFMbox == NULL)
    {
        while (1);
    }  
}
/********************************************************************************************************
**  �� ��  �� ��: CreateMutexs2       									                               **
**	�� ��  �� ��: ����Mutexs2                                                                          **			
**	�� ��  �� ��: none												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void CreateMutexs2(void)
{
    ;
}
/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/
