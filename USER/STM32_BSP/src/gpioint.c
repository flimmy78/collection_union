/****************************************Copyright (c)****************************************************
**											ct                              
**                                     
**                         
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			gpioint.c
** Last modified Date:  
** Last Version:		1.0
** Descriptions:		GPIO_INT driver
**
**--------------------------------------------------------------------------------------------------------
** Created by:			GouJiangtao
** Created date:		2012-08-26
** Version:				1.0
** Descriptions:		The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
*********************************************************************************************************/
#include <includes.h>

OS_EVENT* GpioIntMbox;
#ifdef   _MBUS_DETACH_
OS_EVENT *GpioPwrMbox;
#endif
extern uint8  EXTI_ERRO_Happen ;
extern uint8  Powerdown;

uint8 GPIOINTInit(void)
{
	GpioIntMbox = OSMboxCreate(NULL);
	if(GpioIntMbox==(OS_EVENT*)0){
		while(1) ;
	}
	
	return (0);
}

#ifdef   _MBUS_DETACH_
uint8 GPIOPwrInit(void)
{
	GpioPwrMbox = OSMboxCreate(NULL);
	if(GpioPwrMbox==(OS_EVENT*)0){
		while(1) ;
	}

	return (0);
}
#endif

void EXTI9_5_IRQHandler(void)
{
	CPU_SR		cpu_sr;
	OS_ENTER_CRITICAL();
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
	OSMboxPost(GpioIntMbox, (void*)1);
	
	EXTI_ClearITPendingBit(EXTI_Line6 | EXTI_Line7 | EXTI_Line8 | EXTI_Line9);
	
	OSIntExit();
	
}

void EXTI15_10_IRQHandler(void)
{
	CPU_SR		cpu_sr;
	OS_ENTER_CRITICAL();
	OSIntNesting++;
	OS_EXIT_CRITICAL();
#ifdef HW_VERSION_1_01
	if(EXTI_GetITStatus(EXTI_Line13)!= SET )
	OSMboxPost(GpioIntMbox, (void*)1);
	else
		{
        	OSMboxPost(GpioPwrMbox, (void*)1);
			if(Powerdown)
	        EXTI_ERRO_Happen =3;//�ϵ�������ϵ�
		}
		
	EXTI_ClearITPendingBit(EXTI_Line11 | EXTI_Line12 | EXTI_Line13);
#else
    OSMboxPost(GpioIntMbox, (void*)1);
    	
    EXTI_ClearITPendingBit(EXTI_Line11 | EXTI_Line12);

#endif
	//�˴����ж��Ƿ��ǵ����նˣ�����Ӧ�Ĵ���
	
	OSTimeDly(OS_TICKS_PER_SEC/20);
	OSIntExit();
	
}
#ifdef HW_VERSION_1_01


//MBUS��·�ж�

void EXTI3_IRQHandler(void)
{
	//OSIntEnter ();
	CPU_SR		cpu_sr;
	OS_ENTER_CRITICAL();
	OSIntNesting++;
	OS_EXIT_CRITICAL();
	
  	//OSMboxPost(GpioPwrMbox, (void*)1);
	//����л�MBUSͨ��ʱ���ּٶ�·��������˴���Ҫ������ʱ�ж��Ƿ����·���жϼ�������

	gu8MBusShortRising = 1;  //��Ǽ�⵽�����أ��Ƿ��·��XX�н�һ���жϡ�


	
	EXTI_ClearITPendingBit(EXTI_Line3);
	
	OSTimeDly(OS_TICKS_PER_SEC/20);
	OSIntExit();
}



void EXTI0_IRQHandler(void)
{
	OSIntEnter ();
      OSMboxPost(GpioPwrMbox, (void*)1);
	EXTI_ClearITPendingBit(EXTI_Line0);
	OSTimeDly(OS_TICKS_PER_SEC/20);
	
	OSIntExit();
}
void EXTI1_IRQHandler(void)
{
	OSIntEnter ();
      OSMboxPost(GpioPwrMbox, (void*)1);
	EXTI_ClearITPendingBit(EXTI_Line1);
	
	OSTimeDly(OS_TICKS_PER_SEC/20);
	OSIntExit();
}


#endif
