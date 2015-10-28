/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)                                                          
**  ��   ��   ��: uart0pro.c
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��8��10�� 
**  ��        ��: ���д��ڳ������ݴ���
**	�� ��  �� ¼:   	
*****************************************************************************************************/

/********************************************** include *********************************************/
#include <includes.h>
#include "app_down.h"
#include "tasks.h"

/********************************************** define *********************************************/

/********************************************** global *********************************************/
OS_EVENT *DownUartRequestQ;												//����ʹ��DownUart����Ϣ����
OS_EVENT *DownUartAnswerMbox_UAF;
OS_EVENT *DownUartAnswerMbox_CLK;
OS_EVENT *DownUartAnswerMbox_MBUS;
OS_EVENT *DownUartAnswerMbox_LCM;

/********************************************** static *********************************************/

/****************************************************************************************************
**	�� ��  �� ��: GetDownUartAnswMbox
**	�� ��  �� ��: �������ĸ��������Ϣ������Ϣ
**	�� ��  �� ��: uint8 TaskPrio -- �������ȼ�
**	�� ��  �� ��:  
**  ��   ��   ֵ: OS_EVENT* -- ������Ϣ����
**	��		  ע: Ϊ��ͨ��һ������������г����������������ظ�������ͬ������
*****************************************************************************************************/
OS_EVENT* GetDownUartAnswMbox(uint8 TaskPrio)
{
	LOG_assert_param(TaskPrio > OS_LOWEST_PRIO);
	
	switch (TaskPrio)
    {
        case PRIO_TASK_UP_ANAL_FR:
              return (DownUartAnswerMbox_UAF);
        
        case PRIO_TASK_CLOCK:             
            return (DownUartAnswerMbox_CLK);

        case PRIO_TASK_READ_ALL_CUR:
              return (DownUartAnswerMbox_MBUS);

        case PRIO_TASK_LCM:
              return (DownUartAnswerMbox_LCM);
          
        default :
            return ((OS_EVENT*)0);
    }	
}

/****************************************************************************************************
**	�� ��  �� ��: Uart0SendThenReceive_PostPend
**	�� ��  �� ��: ���г�������֮��ȴ���������
**	�� ��  �� ��: uint8 *buf -- �跢�͵�����;	uint8 *n -- �跢�͵��ֽ���
**	�� ��  �� ��: uint8 *buf -- �ÿռ临�ã�������յ�������
**  ��   ��   ֵ: uint8* -- ���ؽ��յ�����Ϣ�������ݵ�ַ
**	��		  ע: �������ȷ������ݣ�֮��ȴ��������ݣ������ؽ��յ�������
*****************************************************************************************************/
uint8* Uart0SendThenReceive_PostPend(uint8 *buf, uint8 *n)
{
	
	uint8 Err			= 0x00;
	uint8 TaskPrio;
	uint8 *cp 			= NULL;
	uint8 MsgBuf[METER_FRAME_LEN_MAX] = {0x00};
	OS_EVENT *Mbox_Answ	= NULL;
	CPU_SR	cpu_sr;
	
	LOG_assert_param(buf == NULL);
	LOG_assert_param(n == NULL);
	LOG_assert_param(*n >= METER_FRAME_LEN_MAX);

	OS_ENTER_CRITICAL();
	TaskPrio = OSPrioCur;
	OS_EXIT_CRITICAL();
	
   	Mbox_Answ = GetDownUartAnswMbox(TaskPrio);
	if(Mbox_Answ == (OS_EVENT*)0)
		{
			//LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <Uart0SendThenReceive_PostPend> GetDownUartAnswMbox Error!");
			while(1);
		}
	
	MsgBuf[0] = TaskPrio;								//����Ϣ������
	MsgBuf[1] = *n;										//���͵��ֽ���
	
	memcpy(&MsgBuf[2], buf, MsgBuf[1]);					//������Ϣ��������

    OSQPost(DownUartRequestQ, (void *)MsgBuf);
    //LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <Uart0SendThenReceive_PostPend> Send Data From DownUart!");
    
	cp = (uint8 *)OSMboxPend(Mbox_Answ, 0, &Err);
	if(cp[0])
		{												//����
			//LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <Uart0SendThenReceive_PostPend> Recive Data Time Out, The Err is %d!", cp[0]);
			return ((uint8*)cp);
		} 
	else
	{
		//LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <Uart0SendThenReceive_PostPend> Recive Data Successful!"); 
	    memcpy(&buf[0], &cp[2], cp[1]);
	    *n = cp[1];
		return ((uint8*)cp);
	}
}

/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/
