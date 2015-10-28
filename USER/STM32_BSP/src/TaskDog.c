#define   TASKDOG_GLOBALS
#include "includes.h"

void InitFeedDogFuns(void)
{
	uint8 i;

	for(i=0;i<OS_LOWEST_PRIO+1;i++){ 
	 	gTasksWatchDog[i].CounterPre=0;
	 	gTasksWatchDog[i].CounterCur=0;
	 	gTasksWatchDog[i].Monitor=FALSE;
	} 
	
	//===ע��Ҫ���ӵ�����
	gTasksWatchDog[PRIO_TASK_I2C_MANAGEMENT].Monitor		=TRUE;
//	gTasksWatchDog[PRIO_TASK_UART5_REC].Monitor				=TRUE;
	gTasksWatchDog[PRIO_TASK_UART4_REC].Monitor				=TRUE;
	gTasksWatchDog[PRIO_TASK_USART1_REC].Monitor			=TRUE;
	gTasksWatchDog[PRIO_TASK_USART2_REC].Monitor			=TRUE;
	gTasksWatchDog[PRIO_TASK_DOWN_UATR_MANAGEMENT].Monitor	=TRUE;
	gTasksWatchDog[PRIO_TASK_UP_SEND].Monitor				=TRUE;
	gTasksWatchDog[PRIO_TASK_UP_ANAL_FR].Monitor			=TRUE;
	gTasksWatchDog[PRIO_TASK_UP_REC_ZIGBEE].Monitor			=TRUE;
	gTasksWatchDog[PRIO_TASK_CLOCK].Monitor					=TRUE;
	gTasksWatchDog[PRIO_TASK_READ_ALL_CUR].Monitor			=TRUE;
	gTasksWatchDog[PRIO_TASK_LOG].Monitor					=TRUE;

	#if   TEST_SELF_CHECK > 0
	//gTasksWatchDog[PRIO_TASKTEST].Monitor=TRUE;
	#endif
}

void SetTaskDogMon(uint8 prio,uint8 Monitor)
{
	CPU_SR		cpu_sr;
	OS_ENTER_CRITICAL();
	if(prio==OS_PRIO_SELF){
		prio=OSPrioCur;
	}
	gTasksWatchDog[prio].Monitor=Monitor;
	gTasksWatchDog[prio].CounterCur=gTasksWatchDog[prio].CounterPre+1;//��Monitor==TRUEʱ���ָ�������ι����Monitor==FLASEʱ������
	OS_EXIT_CRITICAL();
}
void Dog_OSTaskSuspend(uint8 prio)
{
	SetTaskDogMon(prio,FALSE);
	OSTaskSuspend(prio);
	if(prio==OS_PRIO_SELF){//������Լ����Լ������
		SetTaskDogMon(prio,TRUE);
	}
}

//ע�⣺��ռ��MUTEX��ʱ����ñ����������ñ������ĺ�����ι�����ÿ�����Ч����Ϊ��ʱ��������ȼ����ܱ��������ı�
void FeedTaskDog(void)
{
	#if WDT_TASK_FEED_EN == 1
	CPU_SR		cpu_sr;
	
	OS_ENTER_CRITICAL();
	gTasksWatchDog[OSPrioCur].CounterCur++;
	OS_EXIT_CRITICAL();
	
	#endif
}

void FeedTaskDog_Int(void)//˵�����˺���ֻ�����ٽ�����ڵ���
{
	#if WDT_TASK_FEED_EN == 1
	
	//OS_ENTER_CRITICAL();
	gTasksWatchDog[OSPrioCur].CounterCur++;
	//OS_EXIT_CRITICAL();
	
	#endif
}
