
/********************************************************************

********************************************************************/
#include <includes.h>


#define UART4_SEND_QUEUE_LENGTH				2072 			//��UART4�������ݶ��з���Ŀռ��С,��λ���ֽ�
#define UART4_REC_QUEUE_LENGTH			    2072 			//��UART4�������ݶ��з���Ŀռ��С,��λ���ֽ�

//ע�⣺**QUEUE_LENGTH ��������ָ�뼰����ָ�룬ʵ�������������ݵ���**QUEUE_LENGTH-24 ���ֽ�
static uint8 UART4SendQueue[UART4_SEND_QUEUE_LENGTH];
	   uint8 UART4RecQueue[UART4_REC_QUEUE_LENGTH];

static uint8 IRQBuf[32] = {0x00};
static void *Q_IRQBuf[32];
static uint8 IRQBuf_Counter = 0;

static OS_EVENT *UART4QMsg;							//�жϵ��������Ϣ����
	   OS_EVENT *UART4RecQueSem; 							//����ն��а󶨵��ź���
	   OS_EVENT *UART4SendQueSem;							//�������ݵ��ź���
	   #ifdef   _MBUS_DETACH_
	   OS_EVENT *UART4SendCompleteSem;            //485��������ź���
	   #endif
	   uint8 IRQ4testBuf[32] = {0x00};//muxiaoqing test
	   uint8 IRQ4testBuf_Counter = 0;

uint8 UART4_QueueCreate(void)
{
	//�������Ͷ���
    if (QueueCreate((void *)UART4SendQueue,
                     sizeof(UART4SendQueue),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
	//�������ն���
	if (QueueCreate((void *)UART4RecQueue,
                     sizeof(UART4RecQueue),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
    UART4QMsg= OSQCreate(&Q_IRQBuf[0], 32);
	if (UART4QMsg == NULL)
    {
        while (1);
    }
	//��������ն��а��ź���
	UART4RecQueSem = OSSemCreate(0);
    if (UART4RecQueSem == NULL){
        while(1);
    }
    
    //�����뷢�Ͷ��а��ź���
	UART4SendQueSem = OSSemCreate(0);
    if (UART4SendQueSem == NULL){
        while(1);
    }
	#ifdef   _MBUS_DETACH_
	UART4SendCompleteSem = OSSemCreate(0);
    if (UART4SendCompleteSem == NULL){
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
static void __UART4Putch(uint8 Data)
{
	QueueWrite_Int((void *)UART4SendQueue, Data); 					//���������
	
	/*
	if(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == SET)			//UART0���ͱ��ּĴ�����
	{ 									
		QueueRead_Int(&tmp, UART0SendQueue); 						//���������ӵ�����
		//˵�������������û���жϷ���ֵ������Ϊ��ʱ�ǹ��жϵģ��������д�����ˡ������ڿ��ж�״̬�¿��Բ����Բ��жϷ���ֵ��û����ϸ����
		UART4->DR = tmp;
	}*/
}

/*********************************************************************************************************
** ��������: UART4Send
** ��������: ���Ͷ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: __UART0Putch
** �ر�˵�����������������룬���������ʹ�ñ�������������ڵ���ǰ�����ź��������ң�����û��ʹ��OSSchedLock(������UART0Send_L)��
**           Ҳ��û�н��"��ֹ�����ȼ�������ñ����������б���ʱ�����CPUʹ��Ȩ�������·��������ݳ��ֽϳ�ʱ��ļ��"
**           ������
********************************************************************************************************/
void UART4Send(uint8* buf,uint32 n)
{
	OS_CPU_SR cpu_sr;
	uint32 N=3; //Ϊ�˼���ϵͳ���ж�ʱ��,�����ٿ����жϴ������������дN���ֽڹؿ�һ���ж�
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=10ʱ�����Եó����ж�ʱ��ԼΪ63us��
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=6ʱ�����Եó����ж�ʱ��ԼΪ40us��(���ļ�ĩβ˵��)
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=3ʱ�����Եó����ж�ʱ��ԼΪ23us��
				//Ϊ�˽���ϵͳ���ж��ӳ�ʱ�䣬ԭ����N ��Ӧ����6��
	uint32 i,j,p,x,y;
	#ifdef   _MBUS_DETACH_
	uint8 err;
	#endif
	
	//======================�ȴ����Ͷ��п��ÿռ�>=n
	if(QueueNotEnoughPend((void*)UART4SendQueue,n)){
		return ;
	}
	//======================
	x=n/N;	y=n%N;	p=0;
	for(i=0;i<x;i++){
		OS_ENTER_CRITICAL();
		for(j=0;j<N;j++){
			__UART4Putch(buf[p++]);
		}
		OS_EXIT_CRITICAL();
	}

	OS_ENTER_CRITICAL();
	for(j=0;j<y;j++){
		__UART4Putch(buf[p++]);
	}
	OS_EXIT_CRITICAL();
#ifdef   _MBUS_DETACH_
    OSTimeDly(5);
    RS485_USART4_SEND_DIR(); 
	OSTimeDly(5);
#endif
	
	//���Ͼ�Ϊ ����Ҫ���͵����ݣ�д����У�д����ɺ������ͻ���Ĵ���Ϊ���жϣ���ʼ�жϷ���
	USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
#ifdef   _MBUS_DETACH_	
	OSSemPend(UART4SendCompleteSem, 0, &err); //�ȴ���������
    OSTimeDly(5);
    RS485_USART4_RCV_DIR();
#endif
}

/*********************************************************************************************************
** ��������: UART0Putch
** ��������: ����һ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: Uart0Send
********************************************************************************************************/
void UART4Putch(uint8 Data)
{
	UART4Send(&Data,1);
}

/*********************************************************************************************************
** ��������: UART4Getch
** ��������: ��UART4���ն��ж�һ���ֽڣ�����������������OutTime����ʱ�ȴ�. ���ϲ����
** �䡡��: OutTime,�ȴ���ʱʱ��
** �䡡��: data ���յ�������
** ȫ�ֱ���: ��
** ����ģ��:
** �ر�˵���� ���������������������˺����������룡������������������
********************************************************************************************************/

uint8 UART4Getch(uint8* data, uint16 OutTime)
{
	uint8 err;
	
	while (QueueRead(data,(void*)UART4RecQueue)!=QUEUE_OK)
	{
        OSSemPend(UART4RecQueSem, OutTime, &err); //�ȴ���������
        FeedTaskDog();
    	if(err!=OS_ERR_NONE)
    		{
    			return err;
			}
	}
	return 0;
}

/*********************************************************************************************************
** ��������: TaskUART4Rec
** ��������: UART4�������񣬽��յ�������д��UART0���ն��У�������Ӧ���нϸߵ����ȼ�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
********************************************************************************************************/
//��Ϣ���з�ʽ
void TaskUART4Rec(void *pdata)
{
	uint8 err;
	uint8 *pReadData = NULL;	

	pdata = pdata;
	IRQBuf_Counter = 0x00;
	QueueFlush((void*)UART4RecQueue); //��ս��ն���
	memset(IRQBuf, 0x00, 32);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	
	while(1)
	{
		do{
			FeedTaskDog();
			pReadData = (uint8 *)OSQPend(UART4QMsg, OS_TICKS_PER_SEC*6, &err);
		}while( (err != OS_ERR_NONE) || (pReadData == NULL) );
		
		QueueWrite((void*)UART4RecQueue, *pReadData); //���յ�������д�����
		OSSemPost(UART4RecQueSem);	
	}
}

/*********************************************************************************************************
** ��������: UART4_IRQHandler
** ��������: UART4�����ж�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
********************************************************************************************************/
void UART4_IRQHandler(void)
{
    CPU_SR		cpu_sr;
    uint8 Err = 0x00;
	uint8 tmp = 0x00;
	
	
	OS_ENTER_CRITICAL();
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
	//�в��� Tansmit Data Register empty interrupt
	if(USART_GetITStatus(UART4, USART_IT_TXE) == SET)
		{
			Err = QueueRead(&tmp, UART4SendQueue);
			if (Err == QUEUE_OK)
				{
					UART4->DR = tmp;
				}
			else if(Err == QUEUE_EMPTY)
				{
					USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
					#ifdef   _MBUS_DETACH_
				    OSSemPost(UART4SendCompleteSem);
					#endif
				}
			
		}
	else if(USART_GetITStatus(UART4, USART_IT_RXNE) == SET)
			{ //���� Receive Data register not empty interrupt
				IRQBuf[IRQBuf_Counter] = UART4->DR;	
				OSQPost(UART4QMsg, &IRQBuf[IRQBuf_Counter]);
				if(IRQBuf_Counter>=30)	IRQBuf_Counter = 0;
				IRQBuf_Counter++;
				
	           // {
				//	IRQ4testBuf[IRQ4testBuf_Counter]=IRQBuf[IRQBuf_Counter];
				//	IRQ4testBuf_Counter ++;
				//	if(IRQ4testBuf_Counter>=30)	
         		//	{
         		//	IRQ4testBuf_Counter = 0;
         		//	}
				//}

				
			}
		else if( (USART_GetFlagStatus(UART4,USART_FLAG_ORE)==SET) || 
				 (USART_GetFlagStatus(UART4,USART_FLAG_NE)==SET) ||
				 (USART_GetFlagStatus(UART4,USART_FLAG_FE)==SET) ||
				 (USART_GetFlagStatus(UART4,USART_FLAG_PE)==SET) )
    			{//���-������������Ҫ�ȶ�SR,�ٶ�DR�Ĵ��� �������������жϵ�����
        			USART_ClearFlag(UART4, USART_FLAG_ORE);    	//��SR
        			tmp = UART4->DR;							//��DR
    			}
			else
				{
					//����жϱ�־
					USART_ClearITPendingBit(UART4, USART_IT_TC);
				}
	OSIntExit();
}

