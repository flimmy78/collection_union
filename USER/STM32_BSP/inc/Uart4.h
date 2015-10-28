
#ifndef				_UART4_H
#define				_UART4_H

extern uint8 UART4RecQueue[];
extern OS_EVENT *UART4RecQueSem;

extern uint16 gIRQCOunter;
extern uint16 gTaskCounter;

//-------------------------------------------------------------------------------------------------------
extern uint8 UART4_QueueCreate(void);

extern void UART4Send(uint8* buf,uint32 n);//���ϲ����

extern void UART4Putch(uint8 Data);//���ϲ����

extern uint8 UART4Getch(uint8* data,uint16 OutTime);//���ϲ����

extern void TaskUART4Rec(void *pdata);

extern void UART4_IRQHandler(void);

#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
