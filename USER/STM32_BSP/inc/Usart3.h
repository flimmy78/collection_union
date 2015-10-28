
#ifndef				_USART3_H
#define				_USART3_H

/*begin:yangfei test needed deleted 2013-02-26*/
//#define TEST_TASK_TEST_EN 1
/*end:yangfei test needed deleted*/
extern uint8 USART3RecQueue_At[];
extern uint8 USART3RecQueue_AtIPD[];
#if TEST_TASK_TEST_EN == 1
extern uint8 USART3RecQueue_Test[];
#endif

extern OS_EVENT *USART3RecQueSem_At; //����ն��а󶨵��ź���
extern OS_EVENT *USART3RecQueSem_AtIPD; //����ն��а󶨵��ź���
#if TEST_TASK_TEST_EN == 1
extern OS_EVENT *USART3RecQueSem_Test;
#endif
//-------------------------------------------------------------------------------------------------------
extern uint8 USART3_QueueCreate(void);

extern void USART3Send(uint8* buf,uint32 n);//���ϲ����

extern void USART3Putch(uint8 Data);//���ϲ����

extern uint8 USART3Getch(uint8* data,uint16 OutTime);//���ϲ����

extern void TaskUSART3Rec(void *pdata);

extern void USART3_IRQHandler(void);

#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
