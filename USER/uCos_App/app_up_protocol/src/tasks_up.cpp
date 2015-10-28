/*******************************************Copyright (c)*******************************************
         							ɽ������ռ似����˾(�����ֲ�)                                                 
**  ��   ��   ��: tasks_up.cpp
**  ��   ��   ��: ������
**  �� ��  �� ��: 2012.08.07 
**  ��        ��: ����վ��ͨ������
**  �� ��  �� ¼:   	
*****************************************************************************************************/

/********************************************** include *********************************************/
#include <includes.h>
#include "tasks_up.h"
#include "app_up.h"

/********************************************** extern *********************************************/
extern OS_EVENT* GprsManaMbox;
extern GPRS_RUN_STA gGprsRunSta;
extern OS_EVENT* HeartFrmMbox;


/********************************************** global *********************************************/
OS_MEM* CommTxBuffer;
uint32 CommTxPart[BLK_NUM][RECE_BUF_LEN];
OS_EVENT *CommTxPartSem;						//���ڴ����󶨵��ź���

void* MsgGrp[BLK_NUM];               			//������Ϣָ������
OS_EVENT *Str_Q;

void* UpSend_Q_Grp[BLK_NUM];
OS_EVENT *UpSend_Q;

uint32 num1,num2;

/********************************************** static *********************************************/
static uint8 gCommDeviceZigbee;
static uint8 gCommDeviceRS485;
static uint8 gCommDeviceRS232;
static uint8 gCommDeviceGprs;

static OS_STK    TaskUpSendStk[TASK_STK_SIZE];
static OS_STK    TaskUpAnalFrStk[TASK_STK_SIZE*2];
static OS_STK    TaskUpRecZigbeeStk[TASK_STK_SIZE];
static OS_STK    TaskUpRecRS485Stk[TASK_STK_SIZE];
static OS_STK    TaskUpRecRS232Stk[TASK_STK_SIZE];
static OS_STK    TaskUpRecGprsStk[TASK_STK_SIZE];
static OS_STK	 TaskReportUpStk[TASK_STK_SIZE];

//static uint8 testHeartFrm_xmz[]={"����!������CZ513-6�ͼ������Լ����!GPRS����ͨ���Ѿ��ɹ�����!�Ǻ�.����������������һЩ���ֻ�����.�����Լ����������Ƿ��յ�.---����!�����ǳ�̩�������Լ����!GPRS����ͨ���Ѿ��ɹ�����!�Ǻ�.����������������һЩ���ֻ�����.�����Լ����������Ƿ��յ�.---����!�����ǳ�̩�������Լ����!GPRS����ͨ���Ѿ��ɹ�����!�Ǻ�.����������������һЩ���ֻ�����.�����Լ����������Ƿ��յ�.---����!�����ǳ�̩�������Լ����!GPRS����ͨ���Ѿ��ɹ�����!�Ǻ�.����������������һЩ���ֻ�����.�����Լ����������Ƿ��յ�."};

/****************************************************************************************************
**	�� ��  �� ��: TaskUpSend
**	�� ��  �� ��: ����λ���������ݣ������κ����ݣ�����ͨ��������������
**	�� ��  �� ��: ��
**	�� ��  �� ��: �� 
**  ��   ��   ֵ: 
**	��		  ע: ��
*****************************************************************************************************/
void  TaskUpSend(void *pdata)
{
	uint8 Err 		= 0x00;
	uint8 csq;  
    uint16 BufNum = 0;
    
	UP_COMM_MSG	*pUpCommMsg	= NULL;
	GPRS_RUN_STA GprsRunSta;
	
    pdata = pdata;                            /* ������뾯��*/
    
    while(1)
    {	
       	do 
      	{	
			FeedTaskDog();
       		pUpCommMsg = (UP_COMM_MSG*)OSQPend(UpSend_Q, OS_TICKS_PER_SEC*6, &Err);
      	}
      	while ((Err!= OS_ERR_NONE)||(pUpCommMsg==NULL));
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] Recived a UpSend_Q, Ready to UpSend Data !");
        
        if(pUpCommMsg->GprsGvc == TRUE)
        {
      		ReadGprsRunSta(&GprsRunSta);
            if(GprsRunSta.Ready == TRUE)
            	{
            		if(CMD_IpClose() == FALSE)
            			{
            				UpdGprsRunSta_IpCloseNum(0);
							LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] UpdGprsRunSta_IpCloseNum(0)!");
							debug_info(gDebugModule[GPRS_MODULE],"UpdGprsRunSta_IpCloseNum(0)!");
            				num1++;
            			}	
            		else
            			{
							/*begin:yangfei needed modified 2013-02-27*/
							#if 1
							UpdGprsRunSta_AddIpCloseNum();
							#endif
							/*end:yangfei needed modified 2013-02-27*/
							LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] UpdGprsRunSta_AddIpCloseNum()!");
							debug_err(gDebugModule[GPRS_MODULE],"UpdGprsRunSta_AddIpCloseNum()");
            				num2++;
            			}
            	
            		OSTimeDly(OS_TICKS_PER_SEC/20);
            		if(CMD_Csq(&csq)==0)
            			{
            				UpdGprsRunSta_Csq(csq);
            			}
            	
            	}
            
            OSMboxPost(GprsManaMbox,(void*)1);
            OSTimeDly(OS_TICKS_PER_SEC/20);
      		continue;//��֤���治���ڴ��ͷţ���Ϊ����ʹ�õĲ��Ƕ�̬������ڴ�
      	}
      	else
      		{ 	
      			switch (pUpCommMsg->Device)
    			{
        		    case UP_COMMU_DEV_GPRS:
        		    	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] Send Data By GPRS!");
        		    	ReadGprsRunSta(&GprsRunSta);
        		    	if(GprsRunSta.Ready==FALSE){
        		    		break;
        		    	}
        		    	Err=GprsIPSEND(pUpCommMsg->buff,pUpCommMsg->ByteNum,&BufNum);
        		    	if(Err==0){
        		    		UpdGprsRunSta_IpSendRemnBufNum((uint8)BufNum);
        		    		UpdGprsRunSta_FeedSndDog();	
        		    		UpdGprsRunSta_AddFrmSndTimes(pUpCommMsg->ByteNum);
							LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] IP Send Successful!");
        		    	}
        		    	else{
							LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: [TaskUpSend] IP Send Failure!");
        		    		UpdGprsRunSta_AddIpSendFailNum();
        		    	}  	
       					break;
       					
					case UP_COMMU_DEV_ZIGBEE:
						
            			UpDevSend(UP_COMMU_DEV_ZIGBEE, pUpCommMsg->buff, pUpCommMsg->ByteNum);
            			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] Send Data By Zigbee!");
            			break;
            			
            		case UP_COMMU_DEV_485:
            			
            			UpDevSend(UP_COMMU_DEV_485, pUpCommMsg->buff, pUpCommMsg->ByteNum);
            			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] Send Data By RS485!");
            			break;
            		case UP_COMMU_DEV_232:
            			UpDevSend(UP_COMMU_DEV_232, pUpCommMsg->buff, pUpCommMsg->ByteNum);
            			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] Send Data By RS232!");
            			break;	
        		    default :
        		    	break;
        		    	
   				}
     		}
   		
      	OSTimeDly(OS_TICKS_PER_SEC/100);
      	
       	OSMemPut(CommTxBuffer,(void*)(pUpCommMsg));
      	OSSemPost(CommTxPartSem);
      	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] Release OSMem!");
      	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpSend] Waiting for UpSend_Q!");
    } 
}

/****************************************************************************************************
**	�� ��  �� ��: TaskUpAnalFr
**	�� ��  �� ��: ���յ���λ�������ݺ󣬶�����֡�Ľ���; ��������ǰ������֡����֡�����ɸ�����������
**	�� ��  �� ��: ��
**	�� ��  �� ��: �� 
**  ��   ��   ֵ: 
**	��		  ע: ��
*****************************************************************************************************/
void  TaskUpAnalFr(void *pdata)
{
 	uint8     err = 0; 
 	uint16 lenBuf = 0; 
   	
	UP_COMM_MSG 	*pUpCommMsg;
	BaseProtocol 	*pBase;
	//int i;
  	_Protocol Pro_Object_Anal;
  	
  	pBase = (BaseProtocol *)&Pro_Object_Anal;
  	pdata = pdata;                            // ������뾯��
        
    while(1)
    {	
       	do 
      	{	
      		FeedTaskDog();  
       		pUpCommMsg = (UP_COMM_MSG*)OSQPend(Str_Q, OS_TICKS_PER_SEC*6, &err); 
      	}
      	while((err!= OS_ERR_NONE)||(pUpCommMsg==NULL));
      	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpAnalFr] Recived a Str_Q, Ready to AnalysisFrame !");
      	
        err = pBase->AnalysisFrame(pUpCommMsg->buff, lenBuf);
		/*begin:yangfei added 2013-02-18 */
		//debugX(LOG_LEVEL_INFO,"Recvie %s \r\n",pUpCommMsg->buff);
		/*end:yangfei added 2013-02-18*/
 		if (err==0)
 		{
    	 	pUpCommMsg->ByteNum=lenBuf;
    	 	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpAnalFr] AnalysisFrame Successful, Ready to OSQPost UpSend_Q!");
   		 	OSQPost(UpSend_Q,(void*)(pUpCommMsg));
 			
 		}
 		else
		{
			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpAnalFr] The Return Err is %d, Release OSMem!", err);
			OSMemPut(CommTxBuffer,(void*)(pUpCommMsg));
  			OSSemPost(CommTxPartSem);
  		}
      	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpAnalFr] Waiting for Str_Q!");
	} 
}

/****************************************************************************************************
**	�� ��  �� ��: TaskUpRecZigbee
**	�� ��  �� ��: �������Zigbee�����ݣ�����Ч����ȷ, ֮���ɷ���֡�������ִ��
**	�� ��  �� ��: ��
**	�� ��  �� ��: �� 
**  ��   ��   ֵ: 
**	��		  ע: ��
*****************************************************************************************************/
void  TaskUpRecZigbee(void *pdata)
{
 	
 	uint8 err,temp;
    uint16 n,len;
	uint8 CommDevice;
	
	_Protocol 		Pro_Object_Rec;
	UP_COMM_MSG 	*pUpCommMsg;
	BaseProtocol 	*pBase;
  	
  	CommDevice = *((uint8*)pdata);
  	pBase = (BaseProtocol *)&Pro_Object_Rec;

 	while(1)
    {
		do 
        {	
        	//���ﲻι��,��Ϊ���������һ�������뵽(�ڱ�֤�㹻����ڴ��������,��BLK_NUMҪ�㹻��),����˵���쳣
        	OSSemPend(CommTxPartSem, 0, &err);
       	}while (err != OS_ERR_NONE);
       	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecZigbee] Recived a CommTxPartSem, Ready to OSMemGet !");
       	
		pUpCommMsg=(UP_COMM_MSG*)OSMemGet(CommTxBuffer, &err);
		if(err != OS_ERR_NONE)
		{
			OSSemPost(CommTxPartSem);
			LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: [TaskUpRecZigbee] OSMemGet Failure!");
			continue;
		}
		LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecZigbee] OSMemGet Successful!");
			
loop:	len = pBase->minFrameLen;
		do 
        {
          	FeedTaskDog();
            err=UpGetStart(CommDevice, OS_TICKS_PER_SEC*6);   
    	}while ( (err!=0) );
    	
    	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecZigbee] OSMemGet Successful!");	
		
		n=0;
		pUpCommMsg->buff[n++]=0x7B;
		for(n=n; n<len; n++)
       	{
      	    err=UpGetch(CommDevice, &temp, OS_TICKS_PER_SEC);
         	if(err)
         	{
         		goto loop;
         	}
       		pUpCommMsg->buff[n]=temp;
		
       	}
		err = pBase->ReceiveFrame(pUpCommMsg->buff,len);
		//LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecZigbee] ReceiveFrame First, Return Value is %d", err);
		if (err == 4) 
       	{	
       		
       	   	if (len>(RECE_BUF_LEN*4)) 
       	   	{
       	     	goto loop;
       	   	}
       	   	//
       	   	for(n=n;n<len;n++)
       	   	{
     	    	err=UpGetch(CommDevice,&temp,OS_TICKS_PER_SEC);
         		if(err)
         		{
         			goto loop;
         		}
         		pUpCommMsg->buff[n]=temp;
       		}
       	   	err = pBase->ReceiveFrame(pUpCommMsg->buff,len);
       	}
		if(err)
		{
			LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: [TaskUpRecZigbee] ReceiveFrame Failure, Return Value is %d", err);
			goto loop;
		}
		else
		{
			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecZigbee] ReceiveFrame Successful, Ready to OSQPost Str_Q");
			pUpCommMsg->GprsGvc = FALSE;
        	pUpCommMsg->Device = CommDevice;
        	OSQPost(Str_Q,(void*)(pUpCommMsg));
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecZigbee] Waiting Recive Frame Data!");
	}
} 

/****************************************************************************************************
**	�� ��  �� ��: TaskUpRecRS485
**	�� ��  �� ��: �������RS485�����ݣ�����Ч����ȷ, ֮���ɷ���֡�������ִ��
**	�� ��  �� ��: ��
**	�� ��  �� ��: �� 
**  ��   ��   ֵ: 
**	��		  ע: ��
*****************************************************************************************************/
void  TaskUpRecRS485(void *pdata)
{
 	
 	uint8 err,temp;
    uint16 n,len;
	uint8 CommDevice;
	
	_Protocol 		Pro_Object_Rec;
	UP_COMM_MSG 	*pUpCommMsg;
	BaseProtocol 	*pBase;
  	
  	CommDevice = *((uint8*)pdata);
  	pBase = (BaseProtocol *)&Pro_Object_Rec;

 	while(1)
    {
		do 
        {	
        	//���ﲻι��,��Ϊ���������һ�������뵽(�ڱ�֤�㹻����ڴ��������,��BLK_NUMҪ�㹻��),����˵���쳣
        	OSSemPend(CommTxPartSem, 0, &err);
       	}while (err != OS_ERR_NONE);
       	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS485] Recived a CommTxPartSem, Ready to OSMemGet !");
       	
		pUpCommMsg=(UP_COMM_MSG*)OSMemGet(CommTxBuffer, &err);
		if(err != OS_ERR_NONE)
		{
			OSSemPost(CommTxPartSem);
			LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: [TaskUpRecRS485] OSMemGet Failure!");
			continue;
		}
		LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS485] OSMemGet Successful!");
			
loop:	len = pBase->minFrameLen;
		do 
        {
          	FeedTaskDog();
            err=UpGetStart(CommDevice, OS_TICKS_PER_SEC*6);   
    	}while ( (err!=0) );
    	
    	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS485] OSMemGet Successful!");	
		
		n=0;
		pUpCommMsg->buff[n++]=0x7B;
		for(n=n; n<len; n++)
       	{
      	    err=UpGetch(CommDevice, &temp, OS_TICKS_PER_SEC);
         	if(err)
         	{
         		goto loop;
         	}
       		pUpCommMsg->buff[n]=temp;
		
       	}
		err = pBase->ReceiveFrame(pUpCommMsg->buff,len);
		//LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS485] ReceiveFrame First, Return Value is %d", err);
		if (err == 4) 
       	{	
       		
       	   	if (len>(RECE_BUF_LEN*4)) 
       	   	{
       	     	goto loop;
       	   	}
       	   	//
       	   	for(n=n;n<len;n++)
       	   	{
     	    	err=UpGetch(CommDevice,&temp,OS_TICKS_PER_SEC);
         		if(err)
         		{
         			goto loop;
         		}
         		pUpCommMsg->buff[n]=temp;
       		}
       	   	err = pBase->ReceiveFrame(pUpCommMsg->buff,len);
       	}
		if(err)
		{
			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "ERROR: [TaskUpRecRS485] ReceiveFrame Failure, Return Value is %d", err);
			goto loop;
		}
		else
		{
			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS485] ReceiveFrame Successful, Ready to OSQPost Str_Q");
			pUpCommMsg->GprsGvc = FALSE;
        	pUpCommMsg->Device = CommDevice;
			//
        	OSQPost(Str_Q,(void*)(pUpCommMsg));
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS485] Waiting Recive Frame Data!");
	}
}

/****************************************************************************************************
**	�� ��  �� ��: TaskUpRecRS232
**	�� ��  �� ��: �������RS232�����ݣ�����Ч����ȷ, ֮���ɷ���֡�������ִ��
**	�� ��  �� ��: ��
**	�� ��  �� ��: �� 
**    ��   ��   ֵ: 
**	��		  ע: ��
*****************************************************************************************************/
void  TaskUpRecRS232(void *pdata)
{
 	
 	uint8 err,temp;
    uint16 n,len;
	uint8 CommDevice;
	
	_Protocol 		Pro_Object_Rec;
	UP_COMM_MSG 	*pUpCommMsg;
	BaseProtocol 	*pBase;
  	
  	CommDevice = *((uint8*)pdata);
  	pBase = (BaseProtocol *)&Pro_Object_Rec;

 	while(1)
    {
		do 
        {	
        	//���ﲻι��,��Ϊ���������һ�������뵽(�ڱ�֤�㹻����ڴ��������,��BLK_NUMҪ�㹻��),����˵���쳣
        	OSSemPend(CommTxPartSem, 0, &err);
       	}while (err != OS_ERR_NONE);
       	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS232] Recived a CommTxPartSem, Ready to OSMemGet !");
       	
		pUpCommMsg=(UP_COMM_MSG*)OSMemGet(CommTxBuffer, &err);
		if(err != OS_ERR_NONE)
		{
			OSSemPost(CommTxPartSem);
			LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: [TaskUpRecRS232] OSMemGet Failure!");
			continue;
		}
		LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS232] OSMemGet Successful!");
			
loop:	len = pBase->minFrameLen;
		do 
        {
          	FeedTaskDog();
            err=UpGetStart(CommDevice, OS_TICKS_PER_SEC*6);   
    	}while ( (err!=0) );
    	
    	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS232] OSMemGet Successful!");	
		
		n=0;
		pUpCommMsg->buff[n++]=0x7B;
		for(n=n; n<len; n++)
       	{
      	    err=UpGetch(CommDevice, &temp, OS_TICKS_PER_SEC);
         	if(err)
         	{
         		goto loop;
         	}
       		pUpCommMsg->buff[n]=temp;
		
       	}
		err = pBase->ReceiveFrame(pUpCommMsg->buff,len);
		LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS232] ReceiveFrame First, Return Value is %d", err);
		if (err == 4) 
       	{	
       		
       	   	if (len>(RECE_BUF_LEN*4)) 
       	   	{
       	     	goto loop;
       	   	}
       	   	//
       	   	for(n=n;n<len;n++)
       	   	{
     	    	err=UpGetch(CommDevice,&temp,OS_TICKS_PER_SEC);
         		if(err)
         		{
         			goto loop;
         		}
         		pUpCommMsg->buff[n]=temp;
       		}
       	   	err = pBase->ReceiveFrame(pUpCommMsg->buff,len);
       	}
		if(err)
		{
			LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: [TaskUpRecRS232] ReceiveFrame Failure, Return Value is %d", err);
			goto loop;
		}
		else
		{
			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS232] ReceiveFrame Successful, Ready to OSQPost Str_Q");
			pUpCommMsg->GprsGvc = FALSE;
        	pUpCommMsg->Device = CommDevice;
        	OSQPost(Str_Q,(void*)(pUpCommMsg));
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecRS232] Waiting Recive Frame Data!");
	}
}

/****************************************************************************************************
**	�� ��  �� ��: TaskUpRecGPRS
**	�� ��  �� ��: �������GPRS�����ݣ�����Ч����ȷ, ֮���ɷ���֡�������ִ��
**	�� ��  �� ��: ��
**	�� ��  �� ��: �� 
**  ��   ��   ֵ: 
**	��		  ע: ��
*****************************************************************************************************/
uint32 gGprsErrCounter	= 0x00;
//char gGPRSTestData[1024] = {0x00};
void  TaskUpRecGprs(void *pdata)
{
 	
 	uint8 err,temp;
    uint16 n,len;
	uint8 CommDevice;
	
	_Protocol 		Pro_Object_Rec;
	UP_COMM_MSG 	*pUpCommMsg;
	BaseProtocol 	*pBase;
  	
	gGprsErrCounter	= 0x00;
  	CommDevice = *((uint8*)pdata);
  	pBase = (BaseProtocol *)&Pro_Object_Rec;
  	//memset(gGPRSTestData, 0x00, 1024);

 	while(1)
    {
		do 
        {	
        	//���ﲻι��,��Ϊ���������һ�������뵽(�ڱ�֤�㹻����ڴ��������,��BLK_NUMҪ�㹻��),����˵���쳣
        	OSSemPend(CommTxPartSem, 0, &err);
       	}while (err != OS_ERR_NONE);
       	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] Recived a CommTxPartSem, Ready to OSMemGet !");
       	
		pUpCommMsg=(UP_COMM_MSG*)OSMemGet(CommTxBuffer, &err);
		if(err != OS_ERR_NONE)
		{
			OSSemPost(CommTxPartSem);
			LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: [TaskUpRecGprs] OSMemGet Failure!");
			continue;
		}
		LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] OSMemGet Successful!");
		
		#if TEST_GPRS_EN > 0
		n=TestUpRec(CommDevice, pUpCommMsg->buff, MAX_REC_MC52i_BYTENUM-4);
		pUpCommMsg->GprsGvc  = FALSE;
        pUpCommMsg->Device   = CommDevice;
       	pUpCommMsg->ByteNum  = n;
       	OSQPost(Str_Q,(void*)(pUpCommMsg));
        continue;
		#endif
			
loop:	len = pBase->minFrameLen;
		do 
        {
          	FeedTaskDog();
            err=UpGetStart(CommDevice, OS_TICKS_PER_SEC*6);   
    	}while ( (err!=0) );
    	
    	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] OSMemGet Successful!");	
		//debug_debug(gDebugModule[GPRS_MODULE]," [TaskUpRecGprs] PreSmybol get Successful!");
		
		n=0;
		pUpCommMsg->buff[n++]=0x7B;
		for(n=n; n<len; n++)
       	{
      	    err=UpGetch(CommDevice, &temp, OS_TICKS_PER_SEC);
         	if(err)
         	{
         		goto loop;
         	}
       		pUpCommMsg->buff[n]=temp;
		
       	}
		err = pBase->ReceiveFrame(pUpCommMsg->buff,len);
		//LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] The Receive Data is %s", PUBLIC_HexStreamToString(pUpCommMsg->buff, len, gGPRSTestData));
		if (err == 4) 
       	{	
       		
       	   	if (len>(RECE_BUF_LEN*4)) 
       	   	{
       	   		LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] Receive Frame Length is short, Return Value is %d", err);
       	     	//LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] The Receive Data is %s", PUBLIC_HexStreamToString(pUpCommMsg->buff, len, gGPRSTestData));
       	     	goto loop;
       	   	}
       	   	//
       	   	for(n=n;n<len;n++)
       	   	{
     	    	err=UpGetch(CommDevice,&temp,OS_TICKS_PER_SEC);
         		if(err)
         		{
         			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] Com get data err, Return Value is %d", err);
         		//	LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] The Receive Data is %s", PUBLIC_HexStreamToString(pUpCommMsg->buff, len, gGPRSTestData));
         			goto loop;
         		}
         		pUpCommMsg->buff[n]=temp;
       		}
       	   	err = pBase->ReceiveFrame(pUpCommMsg->buff,len);
       	}
		if(err)
		{
			gGprsErrCounter++;
			LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: [TaskUpRecGprs] ReceiveFrame Failure, Return Value is %d" 
								  "The Error Counter is %2d", err, gGprsErrCounter);
		    //LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] The Receive Data is %s", PUBLIC_HexStreamToString(pUpCommMsg->buff, len, gGPRSTestData));
			goto loop;
		}
		else
		{
			LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] ReceiveFrame Successful, Ready to OSQPost Str_Q");
			pUpCommMsg->GprsGvc = FALSE;
        	pUpCommMsg->Device  = CommDevice;
        	OSQPost(Str_Q, (void*)(pUpCommMsg));
        }
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [TaskUpRecGprs] Waiting Recive Frame Data!");
	}
}

/****************************************************************************************************
**	�� ��  �� ��: TaskReportUp
**	�� ��  �� ��: ����ϵͳ�������ϱ�����
**	�� ��  �� ��: ��
**	�� ��  �� ��: �� 
**  ��   ��   ֵ: 
**	��		  ע: ��
*****************************************************************************************************/
void  TaskReportUp(void *pdata)
{
   	uint8 err;
   	void* cp1;
   
   	_Protocol 		Pro_Object_Rec;
	UP_COMM_MSG 	*pUpCommMsg;
	BaseProtocol 	*pBase;
	//uint8 testHeartFrm[100] = {0x00};

  	pBase = (BaseProtocol *)&Pro_Object_Rec;
 	
	while(1)
    {   	    	
    	do 
        {	
        	//���ﲻι��,��Ϊ���������һ�������뵽(�ڱ�֤�㹻����ڴ��������,��BLK_NUMҪ�㹻��),����˵���쳣
        	OSSemPend(CommTxPartSem, 0, &err);
       	}while (err != OS_ERR_NONE) ;
       	 
		pUpCommMsg=(UP_COMM_MSG*)OSMemGet(CommTxBuffer,&err);
		if(err != OS_ERR_NONE)
			{
				OSSemPost(CommTxPartSem);
				continue; 
			}
			
		do 
        {	
        	FeedTaskDog();
        	OSTimeDly(3*OS_TICKS_PER_SEC); 
        	cp1 = OSMboxAccept(HeartFrmMbox);
        	
       	}while ( (cp1==(void*)0) );
       	
    	if (cp1!=(void*)0)
    		{
    				
     			pUpCommMsg->GprsGvc = FALSE;
				pUpCommMsg->Device = UP_COMMU_DEV_GPRS;
				debug("INFO: [TaskReportUp] Up Report Msg The cp1 = %d \r\n", cp1);
		    	
    			pUpCommMsg->ByteNum = pBase->CreateHeartFrm(pUpCommMsg->buff,(uint32)cp1);//sizeof(testHeartFrm);////ע��,����Ҫ�ĳƵ���������֡����
       			OSQPost(UpSend_Q,(void*)(pUpCommMsg));
            
   			}
   		else
   			{
   				OSMemPut(CommTxBuffer,(void*)(pUpCommMsg));
      			OSSemPost(CommTxPartSem);
   			}
	}
}
   
/****************************************************************************************************
**	�� ��  �� ��: Createtasks_up
**	�� ��  �� ��: ��������ͨ������
**	�� ��  �� ��: ��
**	�� ��  �� ��: �� 
**  ��   ��   ֵ: 
**	��		  ע: ��
*****************************************************************************************************/
void Createtasks_up(void)
{
	uint8 err;
	err=OSTaskCreate(TaskUpSend, (void *)0, &TaskUpSendStk[TASK_STK_SIZE - 1], PRIO_TASK_UP_SEND);
    if(err!=OS_ERR_NONE){
    	while(1);
    }
	err=OSTaskCreate(TaskUpAnalFr, (void *)0, &TaskUpAnalFrStk[TASK_STK_SIZE*2 - 1], PRIO_TASK_UP_ANAL_FR);
    if(err!=OS_ERR_NONE){
    	while(1);
    }
    
	gCommDeviceZigbee = UP_COMMU_DEV_ZIGBEE;
    gCommDeviceRS485  = UP_COMMU_DEV_485;
    gCommDeviceRS232  = UP_COMMU_DEV_232;
	gCommDeviceGprs   = UP_COMMU_DEV_GPRS;

    err=OSTaskCreate(TaskUpRecZigbee, (void *)(&gCommDeviceZigbee), &TaskUpRecZigbeeStk[TASK_STK_SIZE - 1], PRIO_TASK_UP_REC_ZIGBEE);
    if(err!=OS_ERR_NONE){
    	while(1);
    }
    err=OSTaskCreate(TaskUpRecRS485, (void *)(&gCommDeviceRS485), &TaskUpRecRS485Stk[TASK_STK_SIZE - 1], PRIO_TASK_UP_REC_RS485);
    if(err!=OS_ERR_NONE){
    	while(1);
    }
    err=OSTaskCreate(TaskUpRecRS232, (void *)(&gCommDeviceRS232), &TaskUpRecRS232Stk[TASK_STK_SIZE - 1], PRIO_TASK_UP_REC_RS232);
    if(err!=OS_ERR_NONE){
    	while(1);
    }
    err=OSTaskCreate(TaskUpRecGprs, (void *)(&gCommDeviceGprs), &TaskUpRecGprsStk[TASK_STK_SIZE - 1], PRIO_TASK_UP_REC_GPRS);     
    if(err!=OS_ERR_NONE){
    	while(1);
    }
	err=OSTaskCreate(TaskReportUp, (void *)(0), &TaskReportUpStk[TASK_STK_SIZE - 1], PRIO_TASK_REPORT_UP);     
    if(err!=OS_ERR_NONE){
    	while(1);
    }
	
}

/****************************************************************************************************
**	�� ��  �� ��: Createtasks_up
**	�� ��  �� ��: ��������ͨ������������ź�������Ϣ���䣬��Ϣ���е�
**	�� ��  �� ��: ��
**	�� ��  �� ��: �� 
**  ��   ��   ֵ: 
**	��		  ע: ��
*****************************************************************************************************/  
void CreateMboxs_up(void)
{
	uint8 err;
	

	UpSend_Q = OSQCreate(&UpSend_Q_Grp[0],BLK_NUM);
	if (UpSend_Q == NULL)
    {
        while (1);
    }
	Str_Q = OSQCreate(&MsgGrp[0],BLK_NUM);
	if (Str_Q == NULL)
    {
        while (1);
    }
	/*begin:yangfei added 2013-03-15  for ����buff �����С*/
    CommTxBuffer = OSMemCreate(CommTxPart,BLK_NUM,(RECE_BUF_LEN*4),&err);//�ڴ����
    
  	if (CommTxBuffer == NULL)
    {
        while (1);
    }
 	CommTxPartSem = OSSemCreate(BLK_NUM);//��ʼ��Ϊ�ڴ����������ڴ����ͷź�ʹ�ö�����
    if (CommTxPartSem == NULL)
    {
        while(1);
	}

}
    
/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/
