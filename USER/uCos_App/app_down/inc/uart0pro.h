/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)                                                          
**  ��   ��   ��: uart0pro.h
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��8��10�� 
**  ��        ��: ���д��ڳ������ݴ���
**	�� ��  �� ¼:   	
*****************************************************************************************************/
#ifndef				_UART0PRO_H
#define				_UART0PRO_H				

#include "meter.h"

extern OS_EVENT *DownUartRequestQ;//����ʹ��Mbus����Ϣ����
extern OS_EVENT *DownUartAnswerMbox_UAF;
extern OS_EVENT *DownUartAnswerMbox_CLK;
extern OS_EVENT *DownUartAnswerMbox_MBUS;
extern OS_EVENT *DownUartAnswerMbox_LCM;

extern uint8* 	Uart0SendThenReceive_PostPend(uint8* buf,uint8* n);
extern OS_EVENT* GetDownUartAnswMbox(uint8 TaskPrio);

#endif
