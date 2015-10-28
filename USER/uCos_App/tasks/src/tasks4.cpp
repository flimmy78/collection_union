/*******************************************Copyright (c)*******************************************
**									ɽ������ռ似����˾(�����ֲ�)                                **                            
**  ��   ��   ��: tasks4.cpp																      **
**  ��   ��   ��: ������																		  **
**	��   ��   ��: 0.1																		      **
**  �� ��  �� ��: 2012��9��20�� 													     	      **
**  ��        ��: �趨���̲���        			      						                      **
**	�� ��  �� ¼:   																			  **
****************************************************************************************************/
/********************************************** include *********************************************/
#include <includes.h>
#include "app_flashmem.h"
#include "app_down.h"
#include "lcd_menu.h"
#include "tasks4.h"

/********************************************** static **********************************************/
static OS_STK  TaskKeyStk[256];       //�޷�������
static OS_STK  TaskLcmStk[TASK_STK_SIZE];	//TASK_STK_SIZE = 1024
static OS_STK  TaskLcmTopStk[TASK_STK_SIZE];
#ifdef   _MBUS_DETACH_
static OS_STK  TaskPwrStk[TASK_STK_SIZE];
#endif
static void* KeyMessageGrp[KEY_MSG_SIZE]; //  KEY_MSG_SIZE = 6 ������Ϣ���е�ָ������Ĵ�С
uint8  EXTI_ERRO_Happen =0;
uint8  Powerdown =0;

extern uint8 gRestartFlag ;

/************************************************extern***********************************************/
extern uint8 KeyMessageFlag;

extern uint8 autotestIsfromExitKey;//muxiaoqing test

/********************************************************************************************************
**  �� ��  �� ��: TaskKey       									                                   **
**	�� ��  �� ��: �������ã���Ϊ�������ҡ�ȡ���ͽ������                                               **			
**	�� ��  �� ��: pdata												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void  TaskKey ( void * pdata )
{
	uint8 err,i;
	uint8 KeyVal[2];
	pdata = pdata;        
//    uint8 test =0;
	for(;;)
	{
	 	
	 	OSMboxPend(GpioIntMbox,0,&err);
		
		for(i=0;i<2;i++)
		{
			OSTimeDly(OS_TICKS_PER_SEC/20);
			if(STA_KEY_CANCEL()==0){
				if(autotestIsfromExitKey ==1)
				{
				autotestIsfromExitKey=2;
			
				KeyVal[i]=KEY_EXIT;
				}
				else if((autotestIsfromExitKey ==2 )||(autotestIsfromExitKey ==0))
				KeyVal[i]=KEY_EXIT;
			}
			else if(STA_KEY_UP()==0){
				if(autotestIsfromExitKey ==1)//��ֹ�Զ����Է��ؼ��˳����ٰ��������Ҽ��ظ����к���
					{
					
					LCM_LIGHT_ENABLE; //��Һ������
					}
				else
				KeyVal[i]=KEY_UP;
			}
			else if(STA_KEY_DOWN()==0){
				
				if(autotestIsfromExitKey ==1)//��ֹ�Զ����Է��ؼ��˳����ٰ��������Ҽ��ظ����к���
					{
					LCM_LIGHT_ENABLE; //��Һ������

				}
				else
				KeyVal[i]=KEY_DOWN;
			}
			else if(STA_KEY_LEFT()==0){
				
				if(autotestIsfromExitKey ==1)//��ֹ�Զ����Է��ؼ��˳����ٰ��������Ҽ��ظ����к���
					{
					LCM_LIGHT_ENABLE; //��Һ������

				}
				else
				KeyVal[i]=KEY_LEFT;
			}
			
			else if(STA_KEY_RIGHT()==0){
				
				if(autotestIsfromExitKey ==1)//��ֹ�Զ����Է��ؼ��˳����ٰ��������Ҽ��ظ����к���
					{
					LCM_LIGHT_ENABLE; //��Һ������

				}
				else
				KeyVal[i]=KEY_RIGHT;
			} 
			else if(STA_KEY_OK()==0){
				KeyVal[i]=KEY_ENTER;
			}
			else{
				KeyVal[i]=NO_KEY;
				break;
			}
		}

		if(KeyVal[0]==NO_KEY)
			{
				OSTimeDly(OS_TICKS_PER_SEC/10);
			}
		else if(KeyVal[0]==KeyVal[1])
			{
				//OpenBell();
				//OSTimeDly(OS_TICKS_PER_SEC/10);
				//CloseBell();
				//�˴���ʱ�Ȳ��ж�Һ��������
				if(ProBltSta()==TRUE)	//�˴��ж�Һ��������״̬����Ϊ�������Һ�������������򲻲���
					{
						OSQPost(KeyMessageQ,(void *)(KeyVal[0]));
					}
			}
			else
				{
					OSTimeDly(OS_TICKS_PER_SEC/10);
				} 			
   	}
}


/********************************************************************************************************
**  �� ��  �� ��: TaskPwr       									                                   **
**	�� ��  �� ��: ��Դ������񣬽��жϵ籣��
                                1.����һЩ����
                                2.��ӡpower off��Ϣ
                                2.ˢ��log���У�����Ϣ���浽��־�����ϱ�����λ����**			
**	�� ��  �� ��: pdata												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
#ifdef HW_VERSION_1_01
#ifdef   _MBUS_DETACH_

void  TaskGpioPolling(void *pdata)   
{
	uint8 err;	
	uint8 tmpmid;
	uint8 i = 0;
	pdata = pdata; 

	SDCardTest(); //���������SD��д������Ƿ�һ��,����ѭ���⣬ֻ����һ�Ρ�
	
//   GPIOPwrInit();
	for(;;)
	{ 	
	 	OSMboxPend(GpioPwrMbox,0,&err);
		OSTimeDly(OS_TICKS_PER_SEC/20);
		#if 1
		if(EXTI_ERRO_Happen ==3)
			{
               
			   	//�˴���ʾ�ϵ��ϵͳ���»���ϵ�
			   Powerdown =0;
			   LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <TaskGpioPolling> Power on! ");
		    }
		else if(STA_PWR_OFF())
		{
		   // SuspendTask();
		   if(!Powerdown)//��дһ����־���ϵ��ùܽŲ��ȶ���ʱ����ڽ�ѹ�����в�������ж�
		   	{
    		   EXTI_ERRO_Happen =1;
    		   Powerdown =1;
    		   LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <TaskGpioPolling> Power Off! ");
		   	}
		}
		else if(STA_MBUS_OFF())
		{
			// SuspendTask();
			
		//	EXTI_ERRO_Happen =2;
		//    MBUSOff = 1;
		
		//	LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <TaskGpioPolling> MBUS Off! ");
            
		//    DISABLE_MBUS();
		}
		else if (GPIO_GPRS_CHECK_STATUS())
			{
			// SuspendTask();
			
			//EXTI_ERRO_Happen =1;
			//LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <TaskGpioPolling> Power Off! ");
			
			BSP_Gprs_CtrInit_Xmz();
		//	OSTaskResume(PRIO_TASK_GPRS_IPD);//�ָ�GPRS��������
		UpdGprsRunSta_ModuId(MODU_ID_UNKNOWN);
		InitGprsRunSta();//GPRSû�о���
		tmpmid=ModelIdentify();
		if(tmpmid == MODU_ID_UNKNOWN)
		{
		//	SuspendGprsRecTask();
		//	OSTaskSuspend(OS_PRIO_SELF);
		}
		else
			{
        		    UpdGprsRunSta_ModuId(tmpmid);
        			
        			OSTaskResume(PRIO_TASK_GPRS_MANA);//�ָ�GPRS ��������
			}

		    }
		
		else if (GPIO_SD_CHECK_STATUS())
			{
			// SuspendTask();
			
			//EXTI_ERRO_Happen =1;
			//LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: <TaskGpioPolling> Power Off! ");
			//MSD_Init();
			//mountSd();
			
			//PARA_InitGlobalVar();

		    }
		#endif


		


		
	}
}

//#undef STA_PWR_OFF()
#endif
#endif
/********************************************************************************************************
**  �� ��  �� ��: TaskLcm       									                                   **
**	�� ��  �� ��: Һ����ʾ����                                                                         **			
**	�� ��  �� ��: pdata												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void  TaskLcm(void *pdata)
{
	pdata = pdata;        
	
	OSTimeDly(OS_TICKS_PER_SEC);
	
	RegisterMenu();
	
	PARA_InitGlobalVar();
//	gRestartFlag = FALSE;
    OSTimeDly(OS_TICKS_PER_SEC*1);
	//OSTaskSuspend(OS_PRIO_SELF);
	for(;;)
	{
	 	//OSTimeDly(OS_TICKS_PER_SEC);

	 	MenuTest();
	 	/*
	 	KeyVal = (uint32)OSQPend(KeyMessageQ, 0, &err);
	 	
	 	
	 	OSTimeDly(OS_TICKS_PER_SEC/5);
	 	//��������Ӧ����
	 	OpenTestLed2();
		OSTimeDly(OS_TICKS_PER_SEC/5);
		CloseTestLed2();*/
		
   	}
}
/********************************************************************************************************
**  �� ��  �� ��: TaskLcmTop       									                                   **
**	�� ��  �� ��: ѭ������                                                                             **			
**	�� ��  �� ��: pdata												                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void  TaskLcmTop(void *pdata)   
{
	uint8 err;
	pdata = pdata;   
	
	//#if TEST_SELF_CHECK ==1
	//OSTaskSuspend(OS_PRIO_SELF);
	//#endif
		
	for(;;)
	{
 		OSTimeDly(OS_TICKS_PER_SEC*3);
	 	OSSemPend(LcmSem, 0, &err);
	 	ShowMenuTop();

		ShowCommunication();//��ʾ "�ѵ�¼"��"δ��¼"
		
	 	OSSemPost(LcmSem);

	 	KeyMessageFlag = 0;	

		  
   	}
}
/********************************************************************************************************
**  �� ��  �� ��: CreateTasks4     									                                   **
**	�� ��  �� ��: ����TASKS4                                                                           **			
**	�� ��  �� ��: none								    			                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void CreateTasks4(void)
{
	uint8 err;
	
    err=OSTaskCreate(TaskKey, (void *)0, &TaskKeyStk[256 - 1],PRIO_TASK_KEY);
	if(err!=OS_ERR_NONE){
    	while(1);
    }
    
    err=OSTaskCreate(TaskLcm, (void *)0, &TaskLcmStk[TASK_STK_SIZE - 1],PRIO_TASK_LCM);
	if(err!=OS_ERR_NONE){
    	while(1);
    }
    
    err=OSTaskCreate(TaskLcmTop, (void *)0, &TaskLcmTopStk[TASK_STK_SIZE - 1],PRIO_TASK_LCM_TOP);
	if(err!=OS_ERR_NONE){
    	while(1);
    }
#if 1//def HW_VERSION_1_01
	
       err=OSTaskCreate(TaskGpioPolling, (void *)0, &TaskPwrStk[256 - 1],35);
	if(err!=OS_ERR_NONE){
    	while(1);
    }
#endif   
}
/********************************************************************************************************
**  �� ��  �� ��: CreateMboxs4     									                                   **
**	�� ��  �� ��: ����mboxs4                                                                          **			
**	�� ��  �� ��: none								    			                                   **
**	�� ��  �� ��: none											                                       **
**  ��   ��   ֵ: none																			   	   **
**	��		  ע: 						                                                               **
********************************************************************************************************/
void CreateMboxs4(void)
{

	LcmSem = OSSemCreate(1);
    if (LcmSem == NULL)
    {
       	while (1);
    }
		
	KeyMessageQ = OSQCreate(KeyMessageGrp, KEY_MSG_SIZE); 
	
	if (KeyMessageQ == NULL)
    {
        while (1);
    }
    
}
void CreateMutexs4(void)
{
	
}		


