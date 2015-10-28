
/********************************************************************

********************************************************************/
#include <includes.h>


#define USART1_SEND_QUEUE_LENGTH			2072 			//��UART1�������ݶ��з���Ŀռ��С,��λ���ֽ�
#define USART1_REC_QUEUE_LENGTH			    2072 			//��UART1�������ݶ��з���Ŀռ��С,��λ���ֽ�

//ע�⣺**QUEUE_LENGTH ��������ָ�뼰����ָ�룬ʵ�������������ݵ���**QUEUE_LENGTH-24 ���ֽ�
static uint8 USART1SendQueue[USART1_SEND_QUEUE_LENGTH];
	   uint8 USART1RecQueue[USART1_REC_QUEUE_LENGTH];
	   uint8 IRQ1testBuf[32] = {0x00};//muxiaoqing test
	   uint8 IRQ1testBuf_Counter = 0;

static uint8 IRQBuf[32] = {0x00};
static void *Q_IRQBuf[32];
static uint8 IRQBuf_Counter = 0;
uint8 forbidsending1 = 0;//muxiaoqing test
static OS_EVENT *Usart1QMsg;								//�жϵ��������Ϣ����
	   OS_EVENT *Usart1RecQueSem; 							//����ն��а󶨵��ź���
	   OS_EVENT *Usart1SendQueSem;							//�������ݵ��ź���
//static uint8 testttttt=0;
extern uint8 autotestmode ;//muxiaoqing test
extern uint8 manualtestmode;

extern OS_EVENT *UartMsgOSQ;	/*������Ϣ����*/

uint8 USART1_QueueCreate(void)
{
	//�������Ͷ���
    if (QueueCreate((void *)USART1SendQueue,
                     sizeof(USART1SendQueue),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
	//�������ն���
	if (QueueCreate((void *)USART1RecQueue,
                     sizeof(USART1RecQueue),
                     NULL,
                     NULL)
                     == NOT_OK){
		while(1);
    }
	Usart1QMsg= OSQCreate(&Q_IRQBuf[0], 32);
	if (Usart1QMsg == NULL)
    {
        while (1);
    }
	//��������ն��а��ź���
	Usart1RecQueSem = OSSemCreate(0);
    if (Usart1RecQueSem == NULL){
        while(1);
    }
    
    //�����뷢�Ͷ��а��ź���
	Usart1SendQueSem = OSSemCreate(0);
    if (Usart1SendQueSem == NULL){
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
static void __USART1Putch(uint8 Data)
{
	QueueWrite_Int((void *)USART1SendQueue, Data); 					//���������
	
	/*
	if(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET)			//UART0���ͱ��ּĴ�����
	{ 									
		QueueRead_Int(&tmp, UART0SendQueue); 						//���������ӵ�����
		//˵�������������û���жϷ���ֵ������Ϊ��ʱ�ǹ��жϵģ��������д�����ˡ������ڿ��ж�״̬�¿��Բ����Բ��жϷ���ֵ��û����ϸ����
		USART1->DR = tmp;
	}*/
}

/*********************************************************************************************************
** ��������: USART1Send
** ��������: ���Ͷ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: __UART0Putch
** �ر�˵�����������������룬���������ʹ�ñ�������������ڵ���ǰ�����ź��������ң�����û��ʹ��OSSchedLock(������UART0Send_L)��
**           Ҳ��û�н��"��ֹ�����ȼ�������ñ����������б���ʱ�����CPUʹ��Ȩ�������·��������ݳ��ֽϳ�ʱ��ļ��"
**           ������
********************************************************************************************************/
void USART1Send(uint8* buf,uint32 n)
{
	OS_CPU_SR cpu_sr;
	uint32 N=3; //Ϊ�˼���ϵͳ���ж�ʱ��,�����ٿ����жϴ������������дN���ֽڹؿ�һ���ж�
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=10ʱ�����Եó����ж�ʱ��ԼΪ63us��
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=6ʱ�����Եó����ж�ʱ��ԼΪ40us��(���ļ�ĩβ˵��)
				//FCCLK=57.6MHZ��FPCLK=14.4MHZʱ ��N=3ʱ�����Եó����ж�ʱ��ԼΪ23us��
				//Ϊ�˽���ϵͳ���ж��ӳ�ʱ�䣬ԭ����N ��Ӧ����6��
	uint32 i,j,p,x,y;
	
				
					if(!autotestmode)
						forbidsending1 = 0;
				if(forbidsending1)
				{
				}
				else
					{
	//======================�ȴ����Ͷ��п��ÿռ�>=n
	if(QueueNotEnoughPend((void*)USART1SendQueue,n)){
		return ;
	}
	//======================
	x=n/N;	y=n%N;	p=0;
	for(i=0;i<x;i++){
		OS_ENTER_CRITICAL();
		for(j=0;j<N;j++){
			__USART1Putch(buf[p++]);
		}
		OS_EXIT_CRITICAL();
	}

	OS_ENTER_CRITICAL();
	for(j=0;j<y;j++){
		__USART1Putch(buf[p++]);
	}
	OS_EXIT_CRITICAL();
	
	//���Ͼ�Ϊ ����Ҫ���͵����ݣ�д����У�д����ɺ������ͻ���Ĵ���Ϊ���жϣ���ʼ�жϷ���
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
					}
}

/*********************************************************************************************************
** ��������: UART0Putch
** ��������: ����һ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: Uart0Send
********************************************************************************************************/
void USART1Putch(uint8 Data)
{
	USART1Send(&Data,1);
}

/*********************************************************************************************************
** ��������: USART1Getch
** ��������: ��USART1���ն��ж�һ���ֽڣ�����������������OutTime����ʱ�ȴ�. ���ϲ����
** �䡡��: OutTime,�ȴ���ʱʱ��
** �䡡��: data ���յ�������
** ȫ�ֱ���: ��
** ����ģ��:
** �ر�˵���� ���������������������˺����������룡������������������
********************************************************************************************************/

uint8 USART1Getch(uint8* data, uint16 OutTime)
{
	uint8 err;
	
	while (QueueRead(data,(void*)USART1RecQueue)!=QUEUE_OK)
	{
        OSSemPend(Usart1RecQueSem, OutTime, &err); //�ȴ���������
        FeedTaskDog();
    	if(err!=OS_ERR_NONE)
    		{
    			return err;
			}
	}
	return 0;
}

/*********************************************************************************************************
** ��������: TaskUsart1Rec
** ��������: UART1�������񣬽��յ�������д��UART0���ն��У�������Ӧ���нϸߵ����ȼ�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
********************************************************************************************************/

void TaskUsart1Rec(void *pdata)
{
	uint8 err;
	uint8 *pReadData = NULL;
	
	pdata = pdata;
	IRQBuf_Counter = 0x00;
	QueueFlush((void*)USART1RecQueue); //��ս��ն���
	memset(IRQBuf, 0x00, 32);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	while(1)
	{
		do{
			FeedTaskDog();
			//pReadData = (uint8 *)OSMboxPend(Usart1Mbox, OS_TICKS_PER_SEC*6, &err);
			pReadData = (uint8 *)OSQPend(Usart1QMsg, OS_TICKS_PER_SEC*6, &err);
		}while( (err != OS_ERR_NONE) || (pReadData == NULL) );
		
		QueueWrite((void*)USART1RecQueue, *pReadData); //���յ�������д�����
		OSSemPost(Usart1RecQueSem);
		
		//OSTimeDlyHMSM(0, 0, 1, 0);	
	}
}

/****************************************************************************
** ��������: USART1_IRQHandler
** ��������: UART1�����ж�
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
*****************************************************************************/
void USART1_IRQHandler(void)
{
    CPU_SR		cpu_sr;
    uint8 Err = 0x00;
	uint8 tmp = 0x00;
	
	
	OS_ENTER_CRITICAL();
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
	//�в��� Tansmit Data Register empty interrupt
	if(USART_GetITStatus(USART1, USART_IT_TXE) == SET)
		{
			Err = QueueRead(&tmp, USART1SendQueue);
			if (Err == QUEUE_OK)
				{
					USART1->DR = tmp;
				}
			else if(Err == QUEUE_EMPTY)
				{
					USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
					if(autotestmode)
						{
                          forbidsending1 = 1;
					    }
				}
			
		}
	else if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
			{ //���� Receive Data register not empty interrupt
				IRQBuf[IRQBuf_Counter] = USART1->DR;
				OSQPost(Usart1QMsg, &IRQBuf[IRQBuf_Counter]);
				if(IRQBuf_Counter>=30)	IRQBuf_Counter = 0;
				IRQBuf_Counter++;
				
                //#ifdef DEBUG_SHELL
               // #if 0
               // Uart_ReceiveChar(); 
               // #else	
	           // {
				//	IRQ1testBuf[IRQ1testBuf_Counter]=IRQBuf[IRQBuf_Counter];
				//	IRQ1testBuf_Counter ++;
				//	if(IRQ1testBuf_Counter>=19)	
         		//	{
         		//	IRQ1testBuf_Counter = 0;
         		//	}
				//}		

               	//#endif
			}
		else if( (USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET) || 
				 (USART_GetFlagStatus(USART1,USART_FLAG_NE)==SET) ||
				 (USART_GetFlagStatus(USART1,USART_FLAG_FE)==SET) ||
				 (USART_GetFlagStatus(USART1,USART_FLAG_PE)==SET) )
    			{//���-������������Ҫ�ȶ�SR,�ٶ�DR�Ĵ��� �������������жϵ�����
        			USART_ClearFlag(USART1, USART_FLAG_ORE);    //��SR
        			tmp = USART1->DR;							//��DR
    			}
			else
				{
					//����жϱ�־
					USART_ClearITPendingBit(USART1, USART_IT_TC);
				}
	OSIntExit();
}


//if you don't use vsprintf(), the code size is reduced very much.
void Uart_Printf(char *fmt,...)
{
	va_list ap;
	char string[256] = {0};
  
	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
    USART1Send((uint8*) string,sizeof(string));
	//Uart_SendString(string);
	va_end(ap);
}
/****************************************************************************
** ��������: Uart_Printf_Time
** ��������: ��ʱ��Ĵ�ӡ����
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
*****************************************************************************/
void Uart_Printf_Time(char *fmt,...)
{
    char StringBuf[356] 	   	= {0x00};
	uint8 lTimeDate[6]			= {0x00};
	int16 StringLen				= 0x00;
	va_list argptr;
	
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
	
	va_start(argptr, fmt);
	StringLen = vsnprintf(&StringBuf[23], sizeof(StringBuf), fmt, argptr);
	if(StringLen)			;						//�˴���Ը��Ƶĳ��Ƚ����ж�
	USART1Send((uint8*) StringBuf,sizeof(StringBuf));
	va_end(argptr);
}

void Uart_ReceiveChar (void) 
{
	u32 RecChar;
	RecChar = (u32)(USART1->DR & 0xFF);
	OSQPost ( UartMsgOSQ, ( void* )RecChar );  //���ַ�ָ�뻯������Ϣ����
}

u8 Uart_GetChar(void)
{
	u8 Err;
	u8 TempChar;
	TempChar =(u8) ( (u32) (u32*) OSQPend ( UartMsgOSQ, 0, &Err )  &0xFF );
	return TempChar;
}



