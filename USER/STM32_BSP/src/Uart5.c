
/********************************************************************

********************************************************************/
#include <includes.h>


#define UART5_SEND_QUEUE_LENGTH				2072 			//��UART5�������ݶ��з���Ŀռ��С,��λ���ֽ�
#define UART5_REC_QUEUE_LENGTH			    2072 			//��UART5�������ݶ��з���Ŀռ��С,��λ���ֽ�

//ע�⣺**QUEUE_LENGTH ��������ָ�뼰����ָ�룬ʵ�������������ݵ���**QUEUE_LENGTH-24 ���ֽ�
static uint8 UART5SendQueue[UART5_SEND_QUEUE_LENGTH];
	   uint8 UART5RecQueue[UART5_REC_QUEUE_LENGTH];

static uint8 IRQBuf[32] = {0x00};
static void *Q_IRQBuf[32];
static uint8 IRQBuf_Counter = 0;
uint8 IRQ5testBuf[32] = {0x00};//muxiaoqing test
uint16 IRQ5testBuf_Counter = 0;

extern uint8 testmode ;//muxiaoqing test
static OS_EVENT *UART5QMsg;									//�жϵ��������Ϣ����
	   OS_EVENT *UART5RecQueSem; 							//����ն��а󶨵��ź���
	   OS_EVENT *UART5SendQueSem;							//�������ݵ��ź���
	   
	   OS_EVENT *UART5SendCompleteSem;            //485��������ź���
	   

uint8 UART5_QueueCreate(void)
{
	//�������Ͷ���
    if (QueueCreate((void *)UART5SendQueue,
                     sizeof(UART5SendQueue),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
	//�������ն���
	if (QueueCreate((void *)UART5RecQueue,
                     sizeof(UART5RecQueue),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
    UART5QMsg= OSQCreate(&Q_IRQBuf[0], 32);
	if (UART5QMsg == NULL)
    {
        while (1);
    }
    
	//��������ն��а��ź���
	UART5RecQueSem = OSSemCreate(0);
    if (UART5RecQueSem == NULL){
        while(1);
    }
    
    //�����뷢�Ͷ��а��ź���
	UART5SendQueSem = OSSemCreate(0);
    if (UART5SendQueSem == NULL){
        while(1);
    }

    UART5SendCompleteSem = OSSemCreate(0);
    if (UART5SendCompleteSem == NULL){
        while(1);
    }
    
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
static void __UART5Putch(uint8 Data)
{
	QueueWrite_Int((void *)UART5SendQueue, Data); 					//���������
	
	/*
	if(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == SET)			//UART0���ͱ��ּĴ�����
	{ 									
		QueueRead_Int(&tmp, UART0SendQueue); 						//���������ӵ�����
		//˵�������������û���жϷ���ֵ������Ϊ��ʱ�ǹ��жϵģ��������д�����ˡ������ڿ��ж�״̬�¿��Բ����Բ��жϷ���ֵ��û����ϸ����
		UART5->DR = tmp;
	}*/
}

/*********************************************************************************************************
** ��������: UART5Send
** ��������: ���Ͷ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: __UART0Putch
** �ر�˵�����������������룬���������ʹ�ñ�������������ڵ���ǰ�����ź��������ң�����û��ʹ��OSSchedLock(������UART0Send_L)��
**           Ҳ��û�н��"��ֹ�����ȼ�������ñ����������б���ʱ�����CPUʹ��Ȩ�������·��������ݳ��ֽϳ�ʱ��ļ��"
**           ������
********************************************************************************************************/
void UART5Send(uint8* buf,uint32 n)
{
	OS_CPU_SR cpu_sr;
    uint8 err;
	uint32 N=3; //Ϊ�˼���ϵͳ���ж�ʱ��,�����ٿ����жϴ������������дN���ֽڹؿ�һ���ж�
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=10ʱ�����Եó����ж�ʱ��ԼΪ63us��
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=6ʱ�����Եó����ж�ʱ��ԼΪ40us��(���ļ�ĩβ˵��)
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=3ʱ�����Եó����ж�ʱ��ԼΪ23us��
				//Ϊ�˽���ϵͳ���ж��ӳ�ʱ�䣬ԭ����N ��Ӧ����6��
	uint32 i,j,p,x,y;
	
	//======================�ȴ����Ͷ��п��ÿռ�>=n
	if(QueueNotEnoughPend((void*)UART5SendQueue,n)){
		return ;
	}
	//======================
	x=n/N;	y=n%N;	p=0;
	for(i=0;i<x;i++){
		OS_ENTER_CRITICAL();
		for(j=0;j<N;j++){
			__UART5Putch(buf[p++]);
		}
		OS_EXIT_CRITICAL();
	}

	OS_ENTER_CRITICAL();
	for(j=0;j<y;j++){
		__UART5Putch(buf[p++]);
	}
	OS_EXIT_CRITICAL();
	
	//���Ͼ�Ϊ ����Ҫ���͵����ݣ�д����У�д����ɺ������ͻ���Ĵ���Ϊ���жϣ���ʼ�жϷ���
	//����RS485��˫���������л�ʱ��Ҫ�������л�����ǰ��ʱ���л������Ҳ��ʱ��
	OSTimeDly(5);
	RS485_SEND_DIR();
       OSTimeDly(5);
	
	USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
       /*yangfei deleted 2013-01-30*/
    OSSemPend(UART5SendCompleteSem, 0, &err); //�ȴ���������
    OSTimeDly(5);
    RS485_RCV_DIR();
	//USART_ITConfig(UART5, USART_IT_TC, ENABLE);
}

/*********************************************************************************************************
** ��������: UART0Putch
** ��������: ����һ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: Uart0Send
********************************************************************************************************/
void UART5Putch(uint8 Data)
{
	UART5Send(&Data,1);
}

/*********************************************************************************************************
** ��������: UART5Getch
** ��������: ��UART5���ն��ж�һ���ֽڣ�����������������OutTime����ʱ�ȴ�. ���ϲ����
** �䡡��: OutTime,�ȴ���ʱʱ��
** �䡡��: data ���յ�������
** ȫ�ֱ���: ��
** ����ģ��:
** �ر�˵���� ���������������������˺����������룡������������������
********************************************************************************************************/

uint8 UART5Getch(uint8* data, uint16 OutTime)
{
	uint8 err;
	
	while (QueueRead(data,(void*)UART5RecQueue)!=QUEUE_OK)
	{
        OSSemPend(UART5RecQueSem, OutTime, &err); //�ȴ���������
        FeedTaskDog();
    	if(err!=OS_ERR_NONE)
    		{
    			return err;
			}
	}
	return 0;
}

/*********************************************************************************************************
** ��������: TaskUART5Rec
** ��������: UART5�������񣬽��յ�������д��UART0���ն��У�������Ӧ���нϸߵ����ȼ�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
********************************************************************************************************/
//��Ϣ���з�ʽ
void TaskUART5Rec(void *pdata)
{
	uint8 err;
	uint8 *pReadData = NULL;	

	pdata = pdata;
	IRQBuf_Counter = 0x00;
	QueueFlush((void*)UART5RecQueue); //��ս��ն���
	memset(IRQBuf, 0x00, 32);
  
	//OSTaskSuspend(OS_PRIO_SELF);

	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	
	while(1)
	{
		do{
			FeedTaskDog();
			pReadData = (uint8 *)OSQPend(UART5QMsg, OS_TICKS_PER_SEC*6, &err);
		}while( (err != OS_ERR_NONE) || (pReadData == NULL) );
		
		QueueWrite((void*)UART5RecQueue, *pReadData); //���յ�������д�����
		OSSemPost(UART5RecQueSem);	
	}
}
/*********************************************************************************************************
** ��������: UART5_IRQHandler
** ��������: UART5�����ж�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
********************************************************************************************************/
void UART5_IRQHandler(void)
{
    CPU_SR		cpu_sr;
    uint8 Err = 0x00;
	uint8 tmp = 0x00;
	
	
	OS_ENTER_CRITICAL();
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
	//�в��� Tansmit Data Register empty interrupt
	if(USART_GetITStatus(UART5, USART_IT_TXE) == SET)
		{
			Err = QueueRead(&tmp, UART5SendQueue);
			if (Err == QUEUE_OK)
				{
					UART5->DR = tmp;
				}
			else if(Err == QUEUE_EMPTY)
				{
					USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
                     /*begin:yangfei added 2013-01-30 */
                    OSSemPost(UART5SendCompleteSem);
                   // RS485_RCV_DIR();
                     /*end:yangfei added 2013-01-30*/
				}
		}
  else if(USART_GetITStatus(UART5, USART_IT_RXNE) == SET)
		{ //���� Receive Data register not empty interrupt
			IRQBuf[IRQBuf_Counter] = UART5->DR;	
			OSQPost(UART5QMsg, &IRQBuf[IRQBuf_Counter]);
			if(IRQBuf_Counter>=30)	{
				IRQBuf_Counter = 0;
			}
			IRQBuf_Counter++;
			
						//if(testmode)
						//	{
        				//		IRQ5testBuf[IRQ5testBuf_Counter]=IRQBuf[IRQBuf_Counter];
        				//		IRQ5testBuf_Counter ++;
        				//		if(IRQ5testBuf_Counter>=14)	
                     	//		{
                     	//		IRQ5testBuf_Counter = 0;
                     	//		}
						//	}
						

			
		}
  else if( (USART_GetFlagStatus(UART5,USART_FLAG_ORE)==SET) || 
	 	(USART_GetFlagStatus(UART5,USART_FLAG_NE)==SET) ||
	 	(USART_GetFlagStatus(UART5,USART_FLAG_FE)==SET) ||
	 	(USART_GetFlagStatus(UART5,USART_FLAG_PE)==SET) )
		{//���-������������Ҫ�ȶ�SR,�ٶ�DR�Ĵ��� �������������жϵ�����
				USART_ClearFlag(UART5, USART_FLAG_ORE);    	//��SR
				tmp = UART5->DR;							//��DR
		}
	else
		{
			//����жϱ�־
			USART_ClearITPendingBit(UART5, USART_IT_TC);
		}
	OSIntExit();
}

