
/********************************************************************

********************************************************************/
#include <includes.h>


#define USART3_SEND_QUEUE_LENGTH			2072 			//��UART2�������ݶ��з���Ŀռ��С,��λ���ֽ�

#define USART3_REC_QUEUE_AT_LENGTH   		4000
#define USART3_REC_QUEUE_ATIPD_LENGTH   		10000

//ע�⣺**QUEUE_LENGTH ��������ָ�뼰����ָ�룬ʵ�������������ݵ���**QUEUE_LENGTH-24 ���ֽ�
static 	uint8 USART3SendQueue[USART3_SEND_QUEUE_LENGTH];
/*begin:yangfei modified 2013-03-01*/
#if  1
uint8 USART3RecQueue_At[USART3_REC_QUEUE_AT_LENGTH];
#else
uint8 USART3RecQueue_At[10000];
#endif		
uint8 IRQ3testBuf[32] = {0x00};//muxiaoqing test
uint8 IRQ3testBuf_Counter = 0;

/*end:yangfei modified*/
uint8 USART3RecQueue_AtIPD[USART3_REC_QUEUE_ATIPD_LENGTH];
#if TEST_TASK_TEST_EN == 1
//int USART3RecQueue_Test[(USART3_REC_QUEUE_ATIPD_LENGTH + sizeof(int) - 1) / sizeof(int)];
uint8 USART3RecQueue_Test[USART3_REC_QUEUE_ATIPD_LENGTH];
#endif
/*begin:yangfei modified*/
//static uint8 IRQBuf[64] = {0x00};
static uint8 IRQBuf[128] = {0x00};
/*end:yangfei modified*/
static void *Q_IRQBuf[64];
static uint8 IRQBuf_Counter = 0;

static OS_EVENT *USART3QMsg;									//�жϵ��������Ϣ����
	   OS_EVENT *USART3SendQueSem;								//�������ݵ��ź���
	   
OS_EVENT *USART3RecQueSem_At; //����ն��а󶨵��ź���
OS_EVENT *USART3RecQueSem_AtIPD; //����ն��а󶨵��ź���
#if TEST_TASK_TEST_EN == 1
OS_EVENT *USART3RecQueSem_Test;
#endif
/*begin:yangfei added 2013-02-27*/
uint8 write_USART3RecQueue_Test(DataQueue *Queue,QUEUE_DATA_TYPE Data,uint8 mode)
{
   Data = Data;
   mode = mode;
   QueueFlush(Queue);
   return 0;
}
/*end:yangfei added 2013-02-27*/
uint8 USART3_QueueCreate(void)
{
	//�������Ͷ���
    if (QueueCreate((void *)USART3SendQueue,
                     sizeof(USART3SendQueue),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
    
	if (QueueCreate((void *)USART3RecQueue_At,
                     sizeof(USART3RecQueue_At),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
    
	if (QueueCreate((void *)USART3RecQueue_AtIPD,
                     sizeof(USART3RecQueue_AtIPD),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
    
    #if TEST_TASK_TEST_EN == 1
	if (QueueCreate((void *)USART3RecQueue_Test,
                     sizeof(USART3RecQueue_Test),
                     NULL,
                     /*NULL*/write_USART3RecQueue_Test)
                     == NOT_OK){
		while(1);
    }
	#endif
    
    USART3QMsg= OSQCreate(&Q_IRQBuf[0], 32);
	if (USART3QMsg == NULL)
    {
        while (1);
    }
    
    //�����뷢�Ͷ��а��ź���
	USART3SendQueSem = OSSemCreate(0);
    if (USART3SendQueSem == NULL){
        while(1);
    }
    
    //��������ն��а��ź���
	USART3RecQueSem_At = OSSemCreate(0);
    if (USART3RecQueSem_At == NULL){
        while(1);
    }
    
	USART3RecQueSem_AtIPD = OSSemCreate(0);
    if (USART3RecQueSem_AtIPD == NULL){
        while(1);
    }
    
	#if TEST_TASK_TEST_EN == 1
    USART3RecQueSem_Test = OSSemCreate(0);
    if (USART3RecQueSem_Test == NULL){
        while(1);
    }
	#endif
    
	return 0;
}

/*********************************************************************************************************
** ��������: __UART0Putch
** ��������: ����һ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: ��
** ˵���� ���ñ�����ǰ������жϣ��ڲ�ʹ��
********************************************************************************************************/
static void __USART3Putch(uint8 Data)
{
	QueueWrite_Int((void *)USART3SendQueue, Data); 					//���������
	
	/*
	if(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == SET)			//UART0���ͱ��ּĴ�����
	{ 									
		QueueRead_Int(&tmp, UART0SendQueue); 						//���������ӵ�����
		//˵�������������û���жϷ���ֵ������Ϊ��ʱ�ǹ��жϵģ��������д�����ˡ������ڿ��ж�״̬�¿��Բ����Բ��жϷ���ֵ��û����ϸ����
		USART3->DR = tmp;
	}*/
}

/*********************************************************************************************************
** ��������: USART3Send
** ��������: ���Ͷ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: __UART0Putch
** �ر�˵�����������������룬���������ʹ�ñ�������������ڵ���ǰ�����ź��������ң�����û��ʹ��OSSchedLock(������UART0Send_L)��
**           Ҳ��û�н��"��ֹ�����ȼ�������ñ����������б���ʱ�����CPUʹ��Ȩ�������·��������ݳ��ֽϳ�ʱ��ļ��"
**           ������
********************************************************************************************************/
void USART3Send(uint8* buf,uint32 n)
{
	OS_CPU_SR cpu_sr;
	uint32 N=3; //Ϊ�˼���ϵͳ���ж�ʱ��,�����ٿ����жϴ������������дN���ֽڹؿ�һ���ж�
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=10ʱ�����Եó����ж�ʱ��ԼΪ63us��
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=6ʱ�����Եó����ж�ʱ��ԼΪ40us��(���ļ�ĩβ˵��)
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=3ʱ�����Եó����ж�ʱ��ԼΪ23us��
				//Ϊ�˽���ϵͳ���ж��ӳ�ʱ�䣬ԭ����N ��Ӧ����6��
	uint32 i,j,p,x,y;
	
	//======================�ȴ����Ͷ��п��ÿռ�>=n
	if(QueueNotEnoughPend((void*)USART3SendQueue,n)){
		return ;
	}
	//======================
	x=n/N;	y=n%N;	p=0;
	for(i=0;i<x;i++){
		OS_ENTER_CRITICAL();
		for(j=0;j<N;j++){
			__USART3Putch(buf[p++]);
		}
		OS_EXIT_CRITICAL();
	}

	OS_ENTER_CRITICAL();
	for(j=0;j<y;j++){
		__USART3Putch(buf[p++]);
	}
	OS_EXIT_CRITICAL();
	
	//���Ͼ�Ϊ ����Ҫ���͵����ݣ�д����У�д����ɺ������ͻ���Ĵ���Ϊ���жϣ���ʼ�жϷ���
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	
}

/*********************************************************************************************************
** ��������: UART0Putch
** ��������: ����һ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: Uart0Send
********************************************************************************************************/
void USART3Putch(uint8 Data)
{
	USART3Send(&Data,1);
}


/*********************************************************************************************************
** ��������: USART3Getch
** ��������: ��USART3���ն��ж�һ���ֽڣ�����������������OutTime����ʱ�ȴ�. ���ϲ����
** �䡡��: OutTime,�ȴ���ʱʱ��
** �䡡��: data ���յ�������
** ȫ�ֱ���: ��
** ����ģ��:
** �ر�˵���� ���������������������˺����������룡������������������
********************************************************************************************************/

uint8 USART3Getch(uint8* data, uint16 OutTime)
{
	uint8 err;
	
	while (QueueRead(data,(void*)USART3RecQueue_At)!=QUEUE_OK)
	{
        OSSemPend(USART3RecQueSem_At, OutTime, &err); //�ȴ���������
        FeedTaskDog();
    	if(err!=OS_ERR_NONE)
    		{
    			return err;
			}
	}
	return 0;
}

/*********************************************************************************************************
** ��������: TaskUSART3Rec
** ��������: UART2�������񣬽��յ�������д��UART0���ն��У�������Ӧ���нϸߵ����ȼ�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
********************************************************************************************************/

//��Ϣ���з�ʽ
void TaskUSART3Rec(void *pdata)
{
	uint8 err;
	uint8 *pReadData = NULL;	

	pdata = pdata;
	IRQBuf_Counter = 0x00;
	QueueFlush((void*)USART3RecQueue_At); 				//��ս��ն���
	QueueFlush((void*)USART3RecQueue_AtIPD); 			//��ս��ն���
	#if TEST_TASK_TEST_EN == 1
	QueueFlush((void*)USART3RecQueue_Test); 				//��ս��ն���
	#endif
	memset(IRQBuf, 0x00, 128);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	while(1)
	{
		do{
			FeedTaskDog();
			pReadData = (uint8 *)OSQPend(USART3QMsg, OS_TICKS_PER_SEC*6, &err);
		}while( (err != OS_ERR_NONE) || (pReadData == NULL) );
		/*begin:yangfei added 2013-02-22*/
        //debug("%c",*pReadData);//Undervoltage
        /*end:yangfei added */
		QueueWrite((void*)USART3RecQueue_At, *pReadData); 		//���յ�������д�����
		QueueWrite((void*)USART3RecQueue_AtIPD, *pReadData); 	//���յ�������д�����
		#if TEST_TASK_TEST_EN == 1
		QueueWrite((void*)USART3RecQueue_Test, *pReadData); 	//���յ�������д�����
		#endif
		
		OSSemPost(USART3RecQueSem_At);
		OSSemPost(USART3RecQueSem_AtIPD);
		#if TEST_TASK_TEST_EN == 1
		OSSemPost(USART3RecQueSem_Test);
		#endif
	}
}


/*********************************************************************************************************
** ��������: USART3_IRQHandler
** ��������: UART2�����ж�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
********************************************************************************************************/

void USART3_IRQHandler(void)
{
    CPU_SR		cpu_sr;
    uint8 Err = 0x00;
	uint8 tmp = 0x00;
	
	
	OS_ENTER_CRITICAL();
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
	//�в��� Tansmit Data Register empty interrupt
	if(USART_GetITStatus(USART3, USART_IT_TXE) == SET)
		{
 			Err = QueueRead(&tmp, USART3SendQueue);
			if (Err == QUEUE_OK)
				{
					USART3->DR = tmp;
				}
			else if(Err == QUEUE_EMPTY)
				{
					USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
				}
			
		}
	else if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
			{ //���� Receive Data register not empty interrupt
				IRQBuf[IRQBuf_Counter] = USART3->DR;
				OSQPost(USART3QMsg, &IRQBuf[IRQBuf_Counter]);
				if(IRQBuf_Counter>=126)	IRQBuf_Counter = 0;
				IRQBuf_Counter++;
				
				//debug("%c",IRQBuf[IRQBuf_Counter]);
			       //debug_debug(gDebugModule[GPRS_MODULE],"%c",IRQBuf[IRQBuf_Counter]);
			    //{
				//	IRQ3testBuf[IRQ3testBuf_Counter]=IRQBuf[IRQBuf_Counter];
				//	IRQ3testBuf_Counter ++;
				//	if(IRQ3testBuf_Counter>=18)	
         		//	{
         		//	IRQ3testBuf_Counter = 0;
         		//	}
				//}   

			}
		else if( (USART_GetFlagStatus(USART3,USART_FLAG_ORE)==SET) || 
				 (USART_GetFlagStatus(USART3,USART_FLAG_NE)==SET) ||
				 (USART_GetFlagStatus(USART3,USART_FLAG_FE)==SET) ||
				 (USART_GetFlagStatus(USART3,USART_FLAG_PE)==SET) )
    			{//���-������������Ҫ�ȶ�SR,�ٶ�DR�Ĵ��� �������������жϵ�����
        			USART_ClearFlag(USART3, USART_FLAG_ORE);    //��SR
        			tmp = USART3->DR;							//��DR
    			}
			else
				{
					//����жϱ�־
					USART_ClearITPendingBit(USART3, USART_IT_TC);
				}
	OSIntExit();
}

