
/********************************************************************

********************************************************************/
#include <includes.h>


#define USART2_SEND_QUEUE_LENGTH			2072 			//��UART2�������ݶ��з���Ŀռ��С,��λ���ֽ�
#define USART2_REC_QUEUE_LENGTH			    2072 			//��UART2�������ݶ��з���Ŀռ��С,��λ���ֽ�

//ע�⣺**QUEUE_LENGTH ��������ָ�뼰����ָ�룬ʵ�������������ݵ���**QUEUE_LENGTH-24 ���ֽ�
static uint8 USART2SendQueue[USART2_SEND_QUEUE_LENGTH];
	   uint8 USART2RecQueue[USART2_REC_QUEUE_LENGTH];
	   uint8 IRQ2testBuf[32] = {0x00};//muxiaoqing test
	   uint8 IRQ2testBuf_Counter = 0;

static uint8 IRQBuf[32] = {0x00};
static void *Q_IRQBuf[32];
static uint8 IRQBuf_Counter = 0;

static OS_EVENT *Usart2QMsg;									//�жϵ��������Ϣ����
	   OS_EVENT *Usart2RecQueSem; 							//����ն��а󶨵��ź���
	   OS_EVENT *Usart2SendQueSem;							//�������ݵ��ź���

uint8 USART2_QueueCreate(void)
{
	//�������Ͷ���
    if (QueueCreate((void *)USART2SendQueue,
                     sizeof(USART2SendQueue),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
	//�������ն���
	if (QueueCreate((void *)USART2RecQueue,
                     sizeof(USART2RecQueue),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
    
    Usart2QMsg= OSQCreate(&Q_IRQBuf[0], 32);
	if (Usart2QMsg == NULL)
    {
        while (1);
    }
	//��������ն��а��ź���
	Usart2RecQueSem = OSSemCreate(0);
    if (Usart2RecQueSem == NULL){
        while(1);
    }
    
    //�����뷢�Ͷ��а��ź���
	Usart2SendQueSem = OSSemCreate(0);
    if (Usart2SendQueSem == NULL){
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
static void __USART2Putch(uint8 Data)
{
	QueueWrite_Int((void *)USART2SendQueue, Data); 					//���������
	
	/*
	if(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == SET)			//UART0���ͱ��ּĴ�����
	{ 									
		QueueRead_Int(&tmp, UART0SendQueue); 						//���������ӵ�����
		//˵�������������û���жϷ���ֵ������Ϊ��ʱ�ǹ��жϵģ��������д�����ˡ������ڿ��ж�״̬�¿��Բ����Բ��жϷ���ֵ��û����ϸ����
		USART2->DR = tmp;
	}*/
}

/*********************************************************************************************************
** ��������: USART2Send
** ��������: ���Ͷ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: __UART0Putch
** �ر�˵�����������������룬���������ʹ�ñ�������������ڵ���ǰ�����ź��������ң�����û��ʹ��OSSchedLock(������UART0Send_L)��
**           Ҳ��û�н��"��ֹ�����ȼ�������ñ����������б���ʱ�����CPUʹ��Ȩ�������·��������ݳ��ֽϳ�ʱ��ļ��"
**           ������
********************************************************************************************************/
void USART2Send(uint8* buf,uint32 n)
{
	OS_CPU_SR cpu_sr;
	uint32 N=3; //Ϊ�˼���ϵͳ���ж�ʱ��,�����ٿ����жϴ������������дN���ֽڹؿ�һ���ж�
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=10ʱ�����Եó����ж�ʱ��ԼΪ63us��
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=6ʱ�����Եó����ж�ʱ��ԼΪ40us��(���ļ�ĩβ˵��)
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=3ʱ�����Եó����ж�ʱ��ԼΪ23us��
				//Ϊ�˽���ϵͳ���ж��ӳ�ʱ�䣬ԭ����N ��Ӧ����6��
	uint32 i,j,p,x,y;
	
	//======================�ȴ����Ͷ��п��ÿռ�>=n
	if(QueueNotEnoughPend((void*)USART2SendQueue,n)){
		return ;
	}
	//======================
	x=n/N;	y=n%N;	p=0;
	for(i=0;i<x;i++){
		OS_ENTER_CRITICAL();
		for(j=0;j<N;j++){
			__USART2Putch(buf[p++]);
		}
		OS_EXIT_CRITICAL();
	}

	OS_ENTER_CRITICAL();
	for(j=0;j<y;j++){
		__USART2Putch(buf[p++]);
	}
	OS_EXIT_CRITICAL();
	
	//���Ͼ�Ϊ ����Ҫ���͵����ݣ�д����У�д����ɺ������ͻ���Ĵ���Ϊ���жϣ���ʼ�жϷ���
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	
}

/*********************************************************************************************************
** ��������: UART0Putch
** ��������: ����һ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: Uart0Send
********************************************************************************************************/
void USART2Putch(uint8 Data)
{
	USART2Send(&Data,1);
}


/*********************************************************************************************************
** ��������: USART2Getch
** ��������: ��USART2���ն��ж�һ���ֽڣ�����������������OutTime����ʱ�ȴ�. ���ϲ����
** �䡡��: OutTime,�ȴ���ʱʱ��
** �䡡��: data ���յ�������
** ȫ�ֱ���: ��
** ����ģ��:
** �ر�˵���� ���������������������˺����������룡������������������
********************************************************************************************************/

uint8 USART2Getch(uint8* data, uint16 OutTime)
{
	uint8 err;
	
	while (QueueRead(data,(void*)USART2RecQueue)!=QUEUE_OK)
	{
        OSSemPend(Usart2RecQueSem, OutTime, &err); //�ȴ���������
        FeedTaskDog();
    	if(err!=OS_ERR_NONE)
    		{
    			return err;
			}
	}
	return 0;
}

/*********************************************************************************************************
** ��������: TaskUsart2Rec
** ��������: UART2�������񣬽��յ�������д��UART0���ն��У�������Ӧ���нϸߵ����ȼ�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
********************************************************************************************************/

//��Ϣ���з�ʽ
void TaskUsart2Rec(void *pdata)
{
	uint8 err;
	uint8 *pReadData = NULL;	

	pdata = pdata;
	IRQBuf_Counter = 0x00;
	QueueFlush((void*)USART2RecQueue); //��ս��ն���
	memset(IRQBuf, 0x00, 32);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	while(1)
	{
		do{
			FeedTaskDog();
			pReadData = (uint8 *)OSQPend(Usart2QMsg, OS_TICKS_PER_SEC*6, &err);
		}while( (err != OS_ERR_NONE) || (pReadData == NULL) );
		
		QueueWrite((void*)USART2RecQueue, *pReadData); //���յ�������д�����
		OSSemPost(Usart2RecQueSem);	
	}
}

/*********************************************************************************************************
** ��������: USART2_IRQHandler
** ��������: UART2�����ж�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
********************************************************************************************************/

void USART2_IRQHandler(void)
{
    CPU_SR		cpu_sr;
    uint8 Err = 0x00;
	uint8 tmp = 0x00;
	
	
	OS_ENTER_CRITICAL();
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
	//�в��� Tansmit Data Register empty interrupt
	if(USART_GetITStatus(USART2, USART_IT_TXE) == SET)
		{
			Err = QueueRead(&tmp, USART2SendQueue);
			if (Err == QUEUE_OK)
				{
					USART2->DR = tmp;
				}
			else if(Err == QUEUE_EMPTY)
				{
					USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
				}
			
		}
	else if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
			{ //���� Receive Data register not empty interrupt
				IRQBuf[IRQBuf_Counter] = USART2->DR;	
				OSQPost(Usart2QMsg, &IRQBuf[IRQBuf_Counter]);
				if(IRQBuf_Counter>=30)	IRQBuf_Counter = 0;
				IRQBuf_Counter++;
				
				//{
				//	IRQ2testBuf[IRQ2testBuf_Counter]=IRQBuf[IRQBuf_Counter];
				//	IRQ2testBuf_Counter ++;
				//	if(IRQ2testBuf_Counter>=18)	
         		//	{
         		//	IRQ2testBuf_Counter = 0;
         		//	}
				//}

			}
		else if( (USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET) || 
				 (USART_GetFlagStatus(USART2,USART_FLAG_NE)==SET) ||
				 (USART_GetFlagStatus(USART2,USART_FLAG_FE)==SET) ||
				 (USART_GetFlagStatus(USART2,USART_FLAG_PE)==SET) )
    			{//���-������������Ҫ�ȶ�SR,�ٶ�DR�Ĵ��� �������������жϵ�����
        			USART_ClearFlag(USART2, USART_FLAG_ORE);    //��SR
        			tmp = USART2->DR;							//��DR
    			}
			else
				{
					//����жϱ�־
					USART_ClearITPendingBit(USART2, USART_IT_TC);
				}
	OSIntExit();
}

