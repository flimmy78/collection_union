/*******************************************Copyright (c)*******************************************
**									ɽ������ռ似����˾(�����ֲ�)                                **                            
**  ��   ��   ��: tasks1.cpp																      **
**  ��   ��   ��: ������																		  **
**	��   ��   ��: 0.1																		      **
**  �� ��  �� ��: 2012��9��20�� 													     	      **
**  ��        ��: ���ڹ�����Ϣ									      						      **
**	�� ��  �� ¼:   																			  **
****************************************************************************************************/

/********************************************** include *********************************************/
#include <includes.h>
#include "app_flashmem.h"
#include "app_down.h"
#include "tasks1.h"
#include "bps_i2c.h"

/***********************************************function*********************************************/
void CreateMboxs(void);
void CreateMutexs(void);

/********************************************** define ***********************************************/
#define		DOWN_UART_MSGGRP_SIZE	METER_FRAME_LEN_MAX	//Task_DownUartManagement���������񽻻���Ϣ������ֽ���
#define 	DOWN_UART_MSG_SIZE    	5					//���д��ڹ����õ���Ϣ���е�ָ������Ĵ�С

/********************************************** static *********************************************/
static void*   DownUartMsgGrp[DOWN_UART_MSG_SIZE];		//���д��ڹ����õ���Ϣ���е�ָ������
//�������ջ����
static OS_STK  TaskUsart1RecStk[256];
static OS_STK  TaskUsart2RecStk[256];
static OS_STK  TaskUsart3RecStk[256];
static OS_STK  TaskUart4RecStk[256];
static OS_STK  TaskUart5RecStk[256];

static OS_STK  TaskI2cManagementStk[256];
static OS_STK  TaskDownUartManagementStk[TASK_STK_SIZE];
/*begin:yangfei added 2013-01-23 for shell task */
static  OS_STK         Task_UartCmdStk[TASK_STK_SIZE];      /*������Ϣ��������Ķ�ջ*/

/*end :yangfei added 2013-01-23 for shell task */
/********************************************** global **********************************************/
uint32 gtmpiin1=0, gtmpiin2=0, gtmpiin3=0;
uint8  gDownCommDev485 = 0;
/********************************************************************************************************
**  �� ��  �� ��: TaskI2cManagement												                       **
**	�� ��  �� ��: ʵ�ֶ�I2C�Ľӿڹ���,���ж�I2c�Ĳ������뷢��Ϣ����������������ֱ�Ӳ���I2c�ӿ�     **			
**	�� ��  �� ��: pdata													         			 	       **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none													                               **
**	��		  ע: �����������ȼ��ܸߣ�ע������ռ��CPU��ʱ��Ҫ�㹻��		                               **																									
*********************************************************************************************************/
void  TaskI2cManagement( void * pdata ) 
{
	uint8 err;
	I2C_MSG* cp;
	
	pdata = pdata;   
	   
	OSSemPost(I2cReqSem);
	for(;;)
	{
		do 
      	{	
      		FeedTaskDog(); 
       		cp = (I2C_MSG *)OSMboxPend(I2cReqMbox, OS_TICKS_PER_SEC*6, &err); //���� 
      	}
      	while((err!= OS_ERR_NONE)||(cp==NULL));
		   
		gtmpiin1++;
		switch (cp->cmd)
        {
			case I2C_MSG_CMD_RD: //��ʱ��
				Rd8025(cp->datbuf);
				break;
			case I2C_MSG_CMD_SD: //����ʱ��
				WrDt8025(cp->datbuf);
				break;
			case I2C_MSG_CMD_INIT_SD: //����ʱ��
				WrDt8025(cp->datbuf);
				Init8025();
				break;
			default :
				while(1);
        }
		
		gtmpiin3++;
	 	OSMboxPost(I2cAnswMbox, (void *)cp);
		
	 	OSTimeDly(6);
	}
}
/********************************************************************************************************
**  �� ��  �� ��: Task_DownUartManagement   									                       **
**	�� ��  �� ��: ���д��ڹ���������������ͨ����������������                                       **			
**	�� ��  �� ��: pdata													                               **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: �������ȼ���ñȻᷢ�����������������ȼ���										   **
**                ��ռ��MUTEX��ʱ������������Ϣ����Ϊ��ʱ��������ȼ����ܱ��������ı�			   **								 **																									
********************************************************************************************************/
void  Task_DownUartManagement( void * pdata )
{
	uint8 	err = 0;
	uint8 	tmp = 0;
	uint8 	dev = 0 ;
	uint8  Data_Recv_Len = 0;
	/*begin:yangfei modified 2013-10-14 for ����outtime*/
	uint16 	outtime = OS_TICKS_PER_SEC*4;  //for ���������*2��Ϊ*4.
	//uint16 	outtime = OS_TICKS_PER_SEC*1.2;
	/*end:yangfei modified 2013-10-14 for ����outtime*/
	uint32  i = 0;
	
	uint8 MsgBufOfRequest[DOWN_UART_MSGGRP_SIZE];					//������������͵���Ϣ������
	uint8 MsgBuf_UAF[DOWN_UART_MSGGRP_SIZE];						//���ظ��ϲ�����PRIO_TASKUPANALFR ����Ϣ����
	uint8 MsgBuf_CLK[DOWN_UART_MSGGRP_SIZE];
	uint8 MsgBuf_F85[DOWN_UART_MSGGRP_SIZE];
	uint8 MsgBuf_LCM[DOWN_UART_MSGGRP_SIZE];
	
	uint8 *cp;
	uint8 *Buf_Answ;												//ѡ��ָ��������������е�һ��
	OS_EVENT *Mbox_Answ;
	CPU_SR	cpu_sr;
	
  	pdata = pdata;    

	while(1)
	{
		do 
      	{	
      		FeedTaskDog();
			cp = (uint8 *)OSQPend(DownUartRequestQ, OS_TICKS_PER_SEC*6, &err);  			//�ȴ��������������,���� 
      	}
      	while((err!= OS_ERR_NONE)||(cp==NULL));
				
		OS_ENTER_CRITICAL();
		for(i=0; i<DOWN_UART_MSGGRP_SIZE; i++)							//����û����ȫ�������
		{					
		 	MsgBufOfRequest[i] = cp[i];
		}
		OS_EXIT_CRITICAL();
		
	 	//FeedTaskDog();
		
		switch (MsgBufOfRequest[0])
        {
            case PRIO_TASK_UP_ANAL_FR: 
        	 	Buf_Answ=MsgBuf_UAF;
		 		Mbox_Answ=DownUartAnswerMbox_UAF;
            	break;
            case PRIO_TASK_CLOCK: 
        	 	Buf_Answ=MsgBuf_CLK;
		 		Mbox_Answ=DownUartAnswerMbox_CLK;
            	break;
            case PRIO_TASK_READ_ALL_CUR: 
        	 	Buf_Answ=MsgBuf_F85;
		 		Mbox_Answ=DownUartAnswerMbox_MBUS;
            	break;
            case PRIO_TASK_LCM: 
        	 	Buf_Answ=MsgBuf_LCM;
		 		Mbox_Answ=DownUartAnswerMbox_LCM;
            	break;                               
            default :
            	Mbox_Answ=(OS_EVENT*)0;
                break;
        }
        if(Mbox_Answ==(OS_EVENT*)0)
        	{
        		continue;
        	}
	 	OSTimeDly(2); 
		
												
		if(gDownCommDev485 == DOWN_COMM_DEV_485 ){    /*485���͵��ȱ�*/
			dev = DOWN_COMM_DEV_485;
		}
		else{
			dev = DOWN_COMM_DEV_MBUS;				//MBUS���͵��ȱ�
		}

	 	DuQueueFlush(dev);   										//��ջ�����
	 	
		DuSend(dev, &MsgBufOfRequest[2], MsgBufOfRequest[1]);
	 	
	 	memset(Buf_Answ, 0x03, DOWN_UART_MSGGRP_SIZE);
	 	Data_Recv_Len = 0;
	 	
		if(gDownCommDev485 == 5){
			//tmp =  Yilin_Valve_ReceiveFrame(dev, &Buf_Answ[2], outtime, &Data_Recv_Len);
		}
		else
	 		{tmp =  METER_ReceiveFrame(dev, &Buf_Answ[2], outtime, &Data_Recv_Len);}
		
		Buf_Answ[0] = tmp;
	 	Buf_Answ[1] = Data_Recv_Len;
	 		
	 	OSTimeDly(OS_TICKS_PER_SEC/5);	 	

	 	OSMboxPost(Mbox_Answ, (void*)Buf_Answ);  
	}
}
 
/********************************************************************************************************
**  �� ��  �� ��: CreateTasks1  									                                   **
**	�� ��  �� ��: ����Tasks1����                                                                       **			
**	�� ��  �� ��: void 													                               **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void CreateTasks1(void)
{
	uint8 err;
	
    err=OSTaskCreate(TaskUsart2Rec, (void *)0, &TaskUsart2RecStk[256 - 1],PRIO_TASK_USART2_REC);
    if(err!=OS_ERR_NONE){
    	while(1);
    }
    err=OSTaskCreate(TaskUsart1Rec, (void *)0, &TaskUsart1RecStk[256 - 1],PRIO_TASK_USART1_REC);
    if(err!=OS_ERR_NONE){
    	while(1);
    }
	err=OSTaskCreate(TaskUSART3Rec, (void *)0, &TaskUsart3RecStk[256 - 1],PRIO_TASK_USART3_REC);
    if(err!=OS_ERR_NONE){
    	while(1);
    }
	err=OSTaskCreate(TaskUART4Rec, (void *)0, &TaskUart4RecStk[256 - 1],PRIO_TASK_UART4_REC);
    if(err!=OS_ERR_NONE){
    	while(1);
    }
    err=OSTaskCreate(TaskUART5Rec, (void *)0, &TaskUart5RecStk[256 - 1],PRIO_TASK_UART5_REC);
    if(err!=OS_ERR_NONE){
    	while(1);
    }

	err=OSTaskCreate(TaskI2cManagement, (void *)0, &TaskI2cManagementStk[256 - 1],PRIO_TASK_I2C_MANAGEMENT);
	if(err!=OS_ERR_NONE){
    	while(1);

    }
	
	err=OSTaskCreate(Task_DownUartManagement, (void *)0, &TaskDownUartManagementStk[TASK_STK_SIZE - 1],PRIO_TASK_DOWN_UATR_MANAGEMENT);
	if(err!=OS_ERR_NONE){
    	while(1);
    } 
	
	/*begin:yangfei added 2013-01-23 for shell task */
	#ifdef DEBUG_SHELL
	err=OSTaskCreate(Task_UartCmd, (void *)0, & Task_UartCmdStk[TASK_STK_SIZE - 1],PRIO_TASK_UART_SHELL);
	if(err!=OS_ERR_NONE){
    	while(1);
    }  
	#endif
	/*end:yangfei added 2013-01-23  */
}

/********************************************************************************************************
**  �� ��  �� ��: CreateMboxs1  									                                   **
**	�� ��  �� ��: ����Mboxs1����                                                                       **			
**	�� ��  �� ��: void 													                               **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void CreateMboxs1(void)
{
	I2cReqSem = OSSemCreate(NULL);
    if (I2cReqSem == NULL){
		while(1);
    }
	I2cReqMbox = OSMboxCreate(NULL);
    if (I2cReqMbox == NULL){
		while(1);
    }
	I2cAnswMbox = OSMboxCreate(NULL);
    if (I2cAnswMbox == NULL){
		while(1);
    }
#ifdef   _MBUS_DETACH_
    I2cMsgSem = OSSemCreate(1);
    if (I2cMsgSem == NULL){
		while(1);
    }
       I2cMsgRetSem = OSSemCreate(NULL);
    if (I2cMsgRetSem == NULL){
		while(1);
    }
#endif	
  	DownUartRequestQ = OSQCreate(DownUartMsgGrp, DOWN_UART_MSG_SIZE);  //���� ����ʹ��DownUart����Ϣ����
	DownUartAnswerMbox_UAF = OSMboxCreate(NULL);  
	DownUartAnswerMbox_CLK = OSMboxCreate(NULL);
	DownUartAnswerMbox_MBUS = OSMboxCreate(NULL);
	DownUartAnswerMbox_LCM = OSMboxCreate(NULL);

	
	if (DownUartRequestQ == NULL)
    {
        while (1);
    }
    if (DownUartAnswerMbox_UAF == NULL)
    {
        while (1);
    }
    if (DownUartAnswerMbox_CLK == NULL)
    {
        while (1);
    }
    if (DownUartAnswerMbox_MBUS == NULL)
    {
        while (1);
    } 
	if (DownUartAnswerMbox_LCM == NULL)
    {
        while (1);
    }  
}

/********************************************************************************************************
**  �� ��  �� ��: CreateMutexs1 									                                   **
**	�� ��  �� ��: ����Mutexs1����                                                                      **			
**	�� ��  �� ��: void 													                               **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void CreateMutexs1(void)
{
	uint8 err;
	FlashMutex=OSMutexCreate(PRIO_FLASH_MUTEX_PIP, &err);
	if (FlashMutex == NULL)
    {
        while (1);
    }
}

