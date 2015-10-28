/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.blkeji.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               app_cfg.h
** Descriptions:            ucosii configuration
**
**--------------------------------------------------------------------------------------------------------

** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__
					  

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/
#define  OS_VIEW_MODULE                  DEF_DISABLED	     	/* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define 	APP_TASK_START_PRIO    				2 	//�����������������ȼ�����ߵģ�������Ӳ����ʼ�������򿪶�ʱ���ж�

#define 	PRIO_TASK_I2C_MANAGEMENT		3

#define 	PRIO_TASK_USART2_REC				4  
#define 	PRIO_TASK_USART3_REC				5
#define 	PRIO_TASK_UART4_REC					6
#define 	PRIO_TASK_UART5_REC					7
#define 	PRIO_TASK_USART1_REC				8

//#define 	PRIO_TASK_USART2_REC				8
//#define 	PRIO_TASK_USART3_REC				4
//#define 	PRIO_TASK_UART4_REC					5
//#define 	PRIO_TASK_UART5_REC					6
//#define 	PRIO_TASK_USART1_REC				7


#define 	PRIO_TASK_DOWN_UATR_MANAGEMENT		10

#define		PRIO_TASK_KEY						11  //���������������ȼ�

/*begin:yangfei added 2013-01-23  */
#define		PRIO_TASK_UART_SHELL			12   
/*end   :yangfei added 2013-01-23  */


//#define 	PRIO_TIME_MUTEX_PIP					17	//IOģ��I2C��ȡʱ��
#define 	PRIO_FLASH_MUTEX_PIP				18  //FLASH�����ź����̳����ȼ�

#define 	PRIO_TASK_MANAGE_DEBUG_INFO			19  //���������ؿ����������������Ϣ

#define 	 PRIO_TASK_GPRS_IPD	 				20
#define     PRIO_TASK_UP_SEND					21
#define	 PRIO_TASK_UP_ANAL_FR				    22
#define     PRIO_TASK_UP_REC_GPRS 				23
#define     PRIO_TASK_UP_REC_ZIGBEE				25
#define     PRIO_TASK_UP_REC_RS485				26
#define     PRIO_TASK_UP_REC_RS232				27
#define		PRIO_TASK_REPORT_UP					28
#define		PRIO_TASK_GPRS_MANA					29

#define    	PRIO_TASK_CLOCK          			30
#define    	PRIO_TASK_READ_ALL_CUR         		31
#define 	PRIO_TASK_LOG						32

#define		PRIO_TASK_LCM						33  //Һ��ˢ�����ȼ�
#define		PRIO_TASK_LCM_TOP					34  //Һ��ѭ�����ȼ�


#ifdef HW_VERSION_1_01
#ifdef   _MBUS_DETACH_
#define		PRIO_TASK_GPIO_POLLING				35



#endif
#endif

#define 	PRIO_TASK_MBUSSHORTC 				36


#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                        256
#define  TASK_STK_SIZE      							1024




/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/

#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

