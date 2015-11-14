/*----------------------------------------------------------------------------
 *      
 *----------------------------------------------------------------------------
 *      Name:    app.C
 *      Purpose: 
 *----------------------------------------------------------------------------
 *      
 *      Copyright (c) 2004-2009          All rights reserved.
 *---------------------------------------------------------------------------*/


/**********************************************************************************************************
*                                             INCLUDE FILES
**********************************************************************************************************/
	
#include <includes.h>

#include "tasks.h"
#include "tasks_up.h"
#include "app_up.h"
#include "lcd_menu.h"
/**********************************************************************************************************
*                                      extern task create FUNCTION
**********************************************************************************************************/

#if (OS_VIEW_MODULE == DEF_ENABLED)
extern void  App_OSViewTaskCreate (void);
#endif

/* extern GLOBAL VARIABLES*/
extern uint16 gVersion;
/**********************************************************************************************************
*                                            LOCAL DEFINES
********************************************************************************************************* */



/**********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
**********************************************************************************************************/

static  OS_STK         App_TaskStartStk[APP_TASK_START_STK_SIZE];

uint8 gTASK_DeadTask;
uint8 gDebugLevel = 4;

uint8 gRestartFlag = FALSE;

/**********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
**********************************************************************************************************/

static  void  App_TaskStart			(void		*p_arg);  

/**********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
**********************************************************************************************************/

INT32S main (void)
{
    CPU_INT08U  os_err;
	os_err = os_err; /* prevent warning... */

	/* Note:  ����ʹ��UCOS, ��OS����֮ǰ����,ע���ʹ���κ��ж�. */
	CPU_IntDis();                    /* Disable all ints until we are ready to accept them.  */

    OSInit();                        /* Initialize "uC/OS-II, The Real-Time Kernel".         */


	os_err = OSTaskCreateExt((void (*)(void *)) App_TaskStart,  /* Create the start task.                               */
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO,
                             (INT16U          ) APP_TASK_START_PRIO,
                             (OS_STK        * )&App_TaskStartStk[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	
	BSP_FSMC_IO_Init();
	BSP_SRAM_Init();
	LOG_QueueCreate();
	Createtasks_up();
    CreateMboxs_up();
    
    CreateTasks_Tasks();   //��������Tasks1()��Tasks2()��Tasks4()������
    
    CreateMboxs_Tasks();   //��������Mbox������
	CreateMutexs_Tasks();  //��������Mutexs������

	OSStart();                               	/* Start multitasking (i.e. give control to uC/OS-II).  */

	return (0);
}

/********************************************************************************************************
**  �� ��  �� ��: App_TaskStart  									                                   **
**	�� ��  �� ��: ����ʼ                                                                             **			
**	�� ��  �� ��: void 													                               **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: ���������������ȼ�����Ӳ�����м�ȫ�ֱ����Ƚ��г�ʼ������������Ź�						                                                               **
********************************************************************************************************/	  
static  void  App_TaskStart ( void *p_arg )
{
	uint32 i = 0x00;
	uint8 num = 0x00;
	uint8 rest = 0;
	p_arg = p_arg;  			// ������뾯��

	//uint8 chtest[] = {0x55,0x55,0x55,0x55,0x55,0xaa} ;
	/******************  Init hardware *****************/
	BSP_Init();
	//��ϵͳ�ڣ����ڴ����ȫ�ֱ���������ⲿSRAM�У�û�н��б��������Զ���0x00Ϊ��ʼֵ��
	//�����ڴ˽���ȫ�����������ó�ʼֵ
	READ_ParaInit();
	PRO_InitGlobalVar();
	//PARA_InitGlobalVar();
	InitFeedDogFuns();
	/***************************************************/
	
    OS_CPU_SysTickInit();                                    /* Initialize the SysTick.                              */
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                            /* Determine CPU capacity.                              */
#endif

//	OSTimeDly(OS_TICKS_PER_SEC*2);
//	PARA_InitGlobalVar();
	
	//����ʼǰ��ʼ�����Ź�
	IWDG_Configuration();
	/*begin:yangfei added 2013-01-24 for start log*/
	Uart_Printf("\r\n===============================================");
	Uart_Printf("\r\n=         (C) COPYRIGHT 2013 513��");
	Uart_Printf("\r\n=                                      ");
	Uart_Printf("\r\n=           app (Version %d.%d%d)          ",(gVersion>>8)&0x0f,(gVersion>>4)&0x0f,gVersion&0x0f);
	Uart_Printf("\r\n=                                      ");
	Uart_Printf("\r\n=        By 513 xi'an Application Team ");
	Uart_Printf("\r\n===============================================");
	Uart_Printf("\r\n*****Compile time:("__TIME__","__DATE__")*********\r\n");
	UartCmdtime(0,0);
	Uart_Printf("***********************************************\r\n");

	while(1)
    {
    	OSTimeDly(OS_TICKS_PER_SEC);
		//LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: [App_TaskStart] Per Second Check OK!");
		/*begin:yangfei added test*/
			{
			//uint8 SendBuff[]={0xaa,0x55};
		       //uint8 SendBuff[]={0xfe, 0xfe, 0x68, 0x20, 0x20, 0x58, 0x35, 0x72, 0x69, 0x55, 0x66, 0x04, 0x04, 0xa0, 0x17, 00, 0x99, 0x23, 0x16};
                     //UpDevSend(UP_COMMU_DEV_485,  SendBuff,  sizeof(SendBuff)); 
			}
	
		/*end:yangfei added test*/

		
		ProBltStaDog();
		
        #if WDT_TYPE ==1 
		IWDG_ReloadCounter();
        #endif
			
		
		if(gRestartFlag==TRUE)
		{
			rest++;
			if(rest>4)						//5���λ
			{
				NVIC_SETFAULTMASK();
				NVIC_GenerateSystemReset();
			}
		}
		else
		{
			rest=0;
		}
		

        num=(num+1)%64;  //������ÿ64�����������������Ƿ����� 
        if(num)	continue;      

		for(i=0; i<OS_LOWEST_PRIO+1; i++)
			{
				if(gTasksWatchDog[i].Monitor==FALSE)
					{
						continue;
					}

				if(gTasksWatchDog[i].CounterPre==gTasksWatchDog[i].CounterCur)
					{
						gTASK_DeadTask=i;
						#if WDT_TASK_MONIT_EN == 1
						while(1);
						#endif 
					}
				else{
						gTasksWatchDog[i].CounterPre=gTasksWatchDog[i].CounterCur;
					}
			}//���ڱ��������ȼ���ߣ����������gTasksWatchDog�ķ��ʲ���Ҫ���жϣ�������ι��ʱҪע����ж�
	}
	
}

/*********************************************************************************************************
  												END FILE
*********************************************************************************************************/
