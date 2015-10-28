/*******************************************Copyright (c)*******************************************
**									ɽ������ռ似����˾(�����ֲ�)                                **                            
**  ��   ��   ��: tasks3.cpp																      **
**  ��   ��   ��: ������																		  **
**	��   ��   ��: 0.1																		      **
**  �� ��  �� ��: 2012��12��07�� 													     	      **
**  ��        ��: GPRS��������        			      						                      **
**	�� ��  �� ¼:   																			  **
****************************************************************************************************/
/********************************************** include *********************************************/
#include <includes.h>
#include "app_flashmem.h"
#include "app_down.h"
#include "tasks3.h"
#include "tasks_up.h"


//#define TEST_SELF_CHECK 2
/********************************************** static **********************************************/
static OS_STK  TaskGprsIpdStk[TASK_STK_SIZE];
static OS_STK  TaskGprsManaStk[TASK_STK_SIZE];
//static char  gGprsRecBuf[MAX_IPSEND_MC52i_BYTENUM*2+100];
static char gRecBuf_xmz[MAX_REC_MC52i_BYTENUM+100];//����+100������

static  uint32 tstn1,tstn2,tstn3,tstn4,tstn5;

/************************************************ gobale ***********************************************/
OS_EVENT* GprsManaMbox;
OS_EVENT* UpLandAnswMbox;
OS_EVENT* HeartFrmMbox;

/************************************************ extern ***********************************************/
extern uint16 gUpdateBegin ;
/********************************************************************************************************
**  �� ��  �� ��: Fun_GprsIpd_xmz       									                           **
**	�� ��  �� ��: ������ģ�� 								                                           **			
**	�� ��  �� ��: pdata												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void Fun_GprsIpd_xmz(void)
{
	//������û������,ԭ��֮һ����������һ��ʱ�䴦�ڹ���״̬
	uint8 err;
	uint16 len;
	uint8 Count = 0;

	for(;;)
	{
        //OSTimeDly(10*OS_TICKS_PER_SEC);
        /*begin:yangfei added 2013-02-28 for 
        ������������ȥ������ֹGPRSģ�黺��������������*/
        if(gUpdateBegin == 1)
        	{
        	err = 0;
        	}
		else
			{
			err=GprsGetIPDATA_xmz(gRecBuf_xmz,0,&len);
			}
		/*end:yangfei added 2013-02-28 */
		if(err==0)
			{
			debug_debug(gDebugModule[GPRS_MODULE],"enter jh");
			do{
	  			err=GprsGetIPDATA_jh(gRecBuf_xmz,0,&len);
	  			if(err)
				{
				//����һ�� 
				//���ܶԵ��Թ����жϵ�(�򱾵ؿ���������)���´���������ж�������ģ�鷢��^SISR: 1, 1(���ݻ�û����ȫ����ʱ)
					debug_err(gDebugModule[GPRS_MODULE],"%s %d err=%d",__FUNCTION__,__LINE__,err);	
					/*begin:yangfei added 2013-02-28 needed deleted*/
					#if  0
					OSTimeDly(OS_TICKS_PER_SEC);
					#endif
					OSTimeDly(OS_TICKS_PER_SEC/8);
					/*end:yangfei added 2013-02-28 needed deleted*/
	  				err=GprsGetIPDATA_jh(gRecBuf_xmz,0,&len);
					if(err)
						{
						debug_err(gDebugModule[GPRS_MODULE],"%s %d err=%d",__FUNCTION__,__LINE__,err);
						}
	  			}
	  		/*begin:yangfei added 2013-02-28 needed deleted*/
				debug_info(gDebugModule[GPRS_MODULE],"GprsIPREC len=%d ok ",len);	
				if(len>400){gUpdateBegin = 1;}/*����������ȡ����ģʽ*/
				if(len == 0)
					{Count++;}
				else
					{Count = 0;}
				if(Count > 10){gUpdateBegin = 0;}/*����10�������ݽ����ѯ��������ģʽ*/
			/*end:yangfei added 2013-02-28 needed deleted*/
	  		UpdGprsRunSta_FeedRecDog();	
	  		UpdGprsRunSta_AddFrmRecTimes(len);	
	  		UpRecQueueWrite(UP_COMMU_DEV_GPRS,(uint8*)gRecBuf_xmz,len);
	  		OSTimeDly(OS_TICKS_PER_SEC/4);
	  		}while(err==0&&len>0);	
			debug_debug(gDebugModule[GPRS_MODULE],"leave jh");
		}
	else
		{
		//debug_err(gDebugModule[GPRS_MODULE],"%s %d err=%d",__FUNCTION__,__LINE__,err);
		}
	} 
}

/********************************************************************************************************
**  �� ��  �� ��: TaskGprsIpd       									                               **
**	�� ��  �� ��: ������ģ�� 								                                           **
**	�� ��  �� ��: pdata												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: Ϊ���ݶ���ģ�����	                                                               **
********************************************************************************************************/
void  TaskGprsIpd(void *pdata)
{
	//������û������,ԭ��֮һ����������һ��ʱ�䴦�ڹ���״̬

	pdata = pdata;  
	OSTaskSuspend(OS_PRIO_SELF);	
	
	if(GetGprsRunSta_ModuId()==MODU_ID_XMZ)
		{
			Fun_GprsIpd_xmz();
		}

}

/********************************************************************************************************
**  �� ��  �� ��: DlyLandFail       									                               **
**	�� ��  �� ��: ��¼ʧ����ʱ 								                                           **			
**	�� ��  �� ��: none												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void DlyLandFail(void)
{
	uint8 i;
	for(i=0;i<6;i++){
		FeedTaskDog();
		OSTimeDly(10*OS_TICKS_PER_SEC);
	}
}

/********************************************************************************************************
**  �� ��  �� ��: TaskGprsIpd       									                               **
**	�� ��  �� ��: ��¼ʧ����ʱ���							                                           **
**	�� ��  �� ��: none												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void DlyGprsCheck(void)
{
	uint8 n,m;
	n=GPRS_CHECK_CYCLE/10;
	m=GPRS_CHECK_CYCLE%10;
	
	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <DlyGprsCheck> Wait for GPRS Check Time Delay!");
	while(n--){
		FeedTaskDog();
		OSTimeDly(10*OS_TICKS_PER_SEC);
	}
	if(m){
		OSTimeDly(m*OS_TICKS_PER_SEC);
		FeedTaskDog();
	}
}

/********************************************************************************************************
**  �� ��  �� ��: ConnectConfirm       									                               **
**	�� ��  �� ��: ��¼ȷ�ϣ�ʧ�ܲ�����						                                           **
**	�� ��  �� ��: none												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע:    					                                                               **
********************************************************************************************************/
uint8 ConnectConfirm(void)
{
	uint8 err,TryT;
  /*	
	uint8 CurSocketType;

	CurSocketType=ReadCurSType();
  
	if(CurSocketType==0){//TCP
		OSMboxPost(HeartFrmMbox,(void*)1);//֪ͨ�����ϱ����񷢵�½֡
		return TRUE;
	}
  */
	//UDP
	TryT=0;
	while(1)
	{
		FeedTaskDog();
		OSTimeDly(2*OS_TICKS_PER_SEC);
		OSMboxPost(HeartFrmMbox,(void*)1);//֪ͨ�����ϱ����񷢵�½֡
		OSMboxAccept(UpLandAnswMbox);//���շ�Ԥ�ڵ���Ϣ
		TryT++;
		OSMboxPend(UpLandAnswMbox,OS_TICKS_PER_SEC*10,&err);
		//err = OS_ERR_NONE;		//for test
		if(err==OS_ERR_NONE)
			{//�յ���Ӧ
				LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <ConnectConfirm> Recvie GPRS UpLand Confirm Frame OK!");
				break;
			}
		if(TryT>=5)
			{
				LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <ConnectConfirm> Retry five Times! GPRS UpLand Failure!");
				return FALSE; //5��û���յ���½֡��Ӧ
			}
		LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <ConnectConfirm> Retry Times is %d", TryT);
	}
	return TRUE;
	
}

/********************************************************************************************************
**  �� ��  �� ��: SuspendGprsRecTask       									                           **
**	�� ��  �� ��: ����GPRS��������							                                           **
**	�� ��  �� ��: none												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע:                                                                                      **
********************************************************************************************************/
void SuspendGprsRecTask(void)
{
	uint8 err;
	//�ڹ���GPRS��������ǰ��ȡ����������ռ�õ��ź���GprsXmzSem,���ź������з�������Ҳ��������,
	//��������˴���,���ܵ������з�����������
	OSSemPend(GprsXmzSem, 0, &err);
	OSTaskSuspend(PRIO_TASK_GPRS_IPD);//����GPRS��������
	OSSemPost(GprsXmzSem);
}

/********************************************************************************************************
**  �� ��  �� ��: SuspendGprsRecTask       									                           **
**	�� ��  �� ��: ����GPRS��������							                                           **
**	�� ��  �� ��: none												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע:                                                                                      **
********************************************************************************************************/
uint8 gGprsFirst;
void  TaskGprsMana(void *pdata)
{
	uint8 tmpmid;
	uint8 err;
	uint32 ReStartCounter = 0x00;
	uint32 HeartFrmSndCycles;
	uint32 Cycles;
	uint32 selfchecknum;
	uint8 lu8csq = 0;
	
	GPRS_RUN_STA GprsRunSta;
	UP_COMM_MSG UpCommMsg;
	CPU_SR	cpu_sr;
	
	pdata = pdata;  
	#if TEST_SELF_CHECK ==0
	uint32 heartcy_sec;//�������ڣ���λ��
	#endif
	#if TEST_SELF_CHECK ==1
	Dog_OSTaskSuspend(OS_PRIO_SELF);
	#endif
	
	gGprsFirst=TRUE;

/*	#if TEST_SELF_CHECK ==1
	tmpmid=GetGprsRunSta_ModuId();
	if(tmpmid==MODU_ID_XMZ){
		err=SetXmzBsp();
		if(err){
			Dog_OSTaskSuspend(OS_PRIO_SELF);
		}
	}
	#endif*/
	
	#if GPRS_HEART_FRM_TIME%GPRS_CHECK_CYCLE == 0
		HeartFrmSndCycles=GPRS_HEART_FRM_TIME/GPRS_CHECK_CYCLE;
	#else
		HeartFrmSndCycles=GPRS_HEART_FRM_TIME/GPRS_CHECK_CYCLE+1;
	#endif
	
	UpdGprsRunSta_ModuId(MODU_ID_UNKNOWN);
	InitGprsRunSta();//GPRSû�о���
	tmpmid=ModelIdentify();
	if(tmpmid == MODU_ID_UNKNOWN)
	{
		SuspendGprsRecTask();
		OSTaskSuspend(OS_PRIO_SELF);
	}
	UpdGprsRunSta_ModuId(tmpmid);
	for(;;)
	{
	  	//OSTaskSuspend(OS_PRIO_SELF);	//GPRS�������
	    //OSTimeDly(10*OS_TICKS_PER_SEC);
		ReStartCounter++;
		if(ReStartCounter > 1)
			{
			LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: [TaskGprsMana] GPRS Modul Restart, Retry Times is %2d", ReStartCounter);
			debug_info(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] GPRS Modul Restart, Retry Times is %2d", ReStartCounter);
			}
		else
			{
			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskGprsMana] GPRS Modul Start!");
			debug_info(gDebugModule[GPRS_MODULE],"INFO: [TaskGprsMana] GPRS Modul Start!");
			}
		/*begin:yangfei 2013-01-14 modified for uart1*/
		#if 1
		if(ReStartCounter >= 5)
		{
			 //SuspendGprsRecTask();//����GPRS��������
			 //�˴����л�������ģʽ
			 //OSTaskSuspend(OS_PRIO_SELF);	//GPRS�������
		}
		#else
		if(ReStartCounter >= 1)
		{
			 SuspendGprsRecTask();//����GPRS��������
			 //�˴����л�������ģʽ
			 OSTaskSuspend(OS_PRIO_SELF);	//GPRS�������
		}
		#endif
		/*end:yangfei 2013-01-14 modified for uart1*/
	    FeedTaskDog();		
		InitGprsRunSta();//GPRSû�о���ʱ ��TaskUpSend����ʹ���豸 UP_COMMU_DEV_AT
		
		SuspendGprsRecTask();//����GPRS�������� 
		
		tmpmid=GetGprsRunSta_ModuId();
		
		if(tmpmid==MODU_ID_XMZ)
			{
				GprsInit_xmz(); //ʹ���豸 UP_COMMU_DEV_AT
			}
	    
	  	UpdGprsRunSta_Ready(TRUE);
		//gprs ready���豸 UP_COMMU_DEV_AT��ʹ��Ȩ����TaskUpSend�����汾����
		//��ʱ���GPRS״̬��ȷ���Ƿ���Ҫ������������ѯGPRS״̬ʹ�õ����豸UP_COMMU_DEV_AT���ʱ�����ֱ��ʹ�ã�
	  	//������TaskUpSend��������TaskUpSend�����д��ȫ��״̬��Ϣ
	  	//�Ƿ񱾵ؿ�Ҳ���ƣ�Ҳ������ʾд����1
	  	
	  	//OSTimeDly(2*OS_TICKS_PER_SEC);
	  	
	  	OSTaskResume(PRIO_TASK_GPRS_IPD);//�ָ�GPRS��������

	  	if(ConnectConfirm()==FALSE){
	  		DlyLandFail();
	  		continue;
	  	}
	
	  	//OSMboxPost(HeartFrmMbox,(void*)1);//֪ͨ�����ϱ���������֡,��1֡����½֡��
	  	//==========��������½֡�����ϻ�Ҫ��1������֡,��Ӧ����������,����,��1���������ں�ſ�ʼ������֡
	  	//OSTimeDly(OS_TICKS_PER_SEC);
	  	//OSMboxPost(HeartFrmMbox,(void*)3);////֪ͨ�����ϱ���������֡
	  	//==========
	  	////
	  	UpdGprsRunSta_Cont(TRUE);
	  	selfchecknum=0;
	  	FeedTaskDog();
	  	while(1){
	  		//OSTimeDly(GPRS_CHECK_CYCLE*OS_TICKS_PER_SEC);//����һ��GPRS�������
	  		DlyGprsCheck();

			if(CMD_Csq(&lu8csq)==0)  //��ȡ�ź�ǿ�ȡ�
           	{
            	UpdGprsRunSta_Csq(lu8csq);
            }

			/*begin:yangfei added 2013-02-28 for test needed modified*/	
			#if 1
	  		UpdGprsRunSta_AddSndDog();
	  		UpdGprsRunSta_AddRecDog();
			#endif
	  		/*end:yangfei added 2013-02-28 */	  			  		
	  		selfchecknum=(selfchecknum+1)%(GPRS_SELF_CHECK_CYCLE/GPRS_CHECK_CYCLE);
	  		if(selfchecknum==0){
	  			//========������Ϣ��֪ͨ����������GPRS״̬���������gGprsRunSta
	  			UpCommMsg.GprsGvc=TRUE;
	  			OSQPost(UpSend_Q,(void*)(&UpCommMsg));
	  			OSMboxPend(GprsManaMbox, 0, &err);
	  			//==========
	  		}
	  		
	  		ReadGprsRunSta(&GprsRunSta);
	  		
	  		if(GprsRunSta.IpCloseNum>=2){
	  			tstn1++;
	  			
	  			LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: [TaskGprsMana] GprsRunSta.IpCloseNum>=2! Modul Restart!");
				debug_info(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] GprsRunSta.IpCloseNum>=2! Modul Restart!");
	  			#if   TEST_SELF_CHECK == 0
	  			break;//��������GPRS
	  			#endif
	  		}

	  		if(GprsRunSta.IpSendFailNum>=10){
	  			tstn2++;
	  			;
	  		}
	  		
	  		if(GprsRunSta.IpSendRemnBufNum<=3){
	  			tstn3++;
	  			;
	  		}
	  		
	  		#if   TEST_SELF_CHECK == 0
	  			OS_ENTER_CRITICAL();
	  			//heartcy_sec=GlobalVariable.HeartCycle;
				heartcy_sec = 60;
	  			OS_EXIT_CRITICAL();
	  			if(heartcy_sec!=0&&heartcy_sec<60){
	  				HeartFrmSndCycles=(heartcy_sec*60)/GPRS_CHECK_CYCLE;
					
				}
	  			else{
	  				HeartFrmSndCycles=GPRS_HEART_FRM_TIME/GPRS_CHECK_CYCLE;
	  			}
	  		
	  		#endif
	  		
	  		
	  		Cycles=GPRS_HEART_FRM_REC_OUTTIME/GPRS_CHECK_CYCLE;
	  		Cycles+=HeartFrmSndCycles;
	  		if(GprsRunSta.RecDog>Cycles){
	  			tstn4++;
	  			
	  			LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: [TaskGprsMana] Heart Frame Recive Out Time! Module will Restart!");
				debug_info(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] Heart Frame Recive Out Time! Module will Restart!");
	  			break;//��������GPRS
	  		}
	  		
	  		//��һ����Ҫ ��ģ�鷵�ط���ʧ��ʱ������ι�������ֵ�ͻᳬ��HeartFrmSndCycles��Խ��Խ��
	  		if(GprsRunSta.SndDog>HeartFrmSndCycles+5){ 
	  			tstn5++;
	  			
	  			LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: [TaskGprsMana] Heart Frame Send Failure! Modul Restart!");
				debug_info(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] Heart Frame Send Failure! Modul Restart!");
	  			break;//��������GPRS
	  		
	  		}
	  		//��һ����Ҫ
	  		
	  		if(GprsRunSta.SndDog>=HeartFrmSndCycles){
	  			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskGprsMana] Post HeartFrmMbox to [TaskReportUp]!");
	  			OSMboxPost(HeartFrmMbox,(void*)3);//֪ͨ�����ϱ���������֡
				ClearCommunication();

	  		}

	  	} 
	} 	
}

void CreateTasks3(void)
{
	uint8 err;
	
    err=OSTaskCreate(TaskGprsIpd, (void *)0, &TaskGprsIpdStk[TASK_STK_SIZE - 1],PRIO_TASK_GPRS_IPD);
	if(err!=OS_ERR_NONE){
    	while(1);
    }
    err=OSTaskCreate(TaskGprsMana, (void *)0, &TaskGprsManaStk[TASK_STK_SIZE - 1],PRIO_TASK_GPRS_MANA);
	if(err!=OS_ERR_NONE){
    	while(1);
    }
}
void CreateMboxs3(void)
{
	GprsManaMbox= OSMboxCreate((void*)0);
	if (GprsManaMbox == NULL)
    {
        while (1);
    }
    UpLandAnswMbox= OSMboxCreate((void*)0);
	if (UpLandAnswMbox == NULL)
    {
        while (1);
    }
    
	HeartFrmMbox = OSMboxCreate(NULL);
    if (HeartFrmMbox == NULL){
		while(1);
    }

	GprsXmzSem = OSSemCreate(1);
    if (GprsXmzSem == NULL)
    {
        while(1);
	}

}
void CreateMutexs3(void)
{
	
}		
