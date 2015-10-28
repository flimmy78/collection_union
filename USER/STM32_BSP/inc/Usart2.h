
#ifndef				_USART2_H
#define				_USART2_H

extern uint8 USART2RecQueue[];
extern OS_EVENT *Usart2RecQueSem;
//-------------------------------------------------------------------------------------------------------
extern uint8 USART2_QueueCreate(void);

extern void USART2Send(uint8* buf,uint32 n);//���ϲ����

extern void USART2Putch(uint8 Data);//���ϲ����

extern uint8 USART2Getch(uint8* data,uint16 OutTime);//���ϲ����

extern void TaskUsart2Rec(void *pdata);

extern void USART2_IRQHandler(void);

#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
