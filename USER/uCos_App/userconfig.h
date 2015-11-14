

#ifndef  __USERCONFIG_H__
#define  __USERCONFIG_H__


#define   _MBUS_DETACH_

/************************************* IWDT Config  ******************************************************/
//		  ����ʱ����WDT_TYPE ��Ϊ0 ���Է�ֹ���Ź�Ӱ����ԡ�
//        ������ϣ���ؽ� WDT_TYPE ��Ϊ1��WDT_TASK_MONIT_EN��Ϊ1 ����ʹ�ܿ��Ź�

#define   WDT_TYPE  				1				// 0:��ʹ�ÿ��Ź� ; 1:ʹ���ڹ�
#define   WDT_TASK_MONIT_EN   		1		//���Ź��������� ʹ�ܼ��� ��ʹ��
#define   WDT_TASK_FEED_EN  		1			//ʹ�ܸ����� ι����  ��ʹ��

/************************************* UART Config  ******************************************************/
/*begin:yangfei modified 2013-01-30*/
//#define	DEBUG_BAUNDRATE		9600					//usart1
#define	DEBUG_BAUNDRATE		115200
/*end:yangfei modified 2013-01-30*/
#define	DEBUG_CHECK			USART_Parity_No

//#define	MBUS_BAUNDRATE		2400					//usart2
//#define	MBUS_CHECK			USART_Parity_Even
#if 0
#define	GPRS_BAUNDRATE		57600					//usart3
#else
#define	GPRS_BAUNDRATE		115200					//usart3
#endif
#define	GPRS_CHECK			USART_Parity_No

#define	ZIGBEE_BAUNDRATE	9600					//uart4
#define	ZIGBEE_CHECK		USART_Parity_No

#define	RS485_BAUNDRATE		1200
#define	RS485_CHECK			USART_Parity_Even

#define MBUS_DEFAULT_PARA 	WAN_HUA


/*begin:yangfei added 2014-02-24 for support ʱ��ͨ��������ȷ����*/
/*�豸����*/
#define HEAT_METER_TYPE 0x20      /*�ȼ�����*/
#define HEATCOST_METER_TYPE 0xA0  /*����ʽ�ȷ����0xA0*/
#define TIME_ON_OFF_AREA_TYPE 0xB0/*ʱ��ͨ�������0XB0*/
/*end:yangfei added 2014-02-24 for support ʱ��ͨ��������ȷ����*/

/************************************* GPRS Config  ******************************************************/

#if   TEST_SELF_CHECK > 0  //�Լ������Чʱ��GPRSģ��������
#define  GPRS_CHECK_CYCLE	 			3
#else

//GPRSģ�������� ʱ�䵥λΪ��, ��СΪ10��,���Ϊ60��  ,һ������²�Ҫ���ģ�����Ϊ 10
#define  GPRS_CHECK_CYCLE	 			10
#endif



#if   TEST_SELF_CHECK > 0
#define  GPRS_SELF_CHECK_CYCLE	 		5*GPRS_CHECK_CYCLE
#else
//GPRSģ���Լ����� ʱ�䵥λΪ��,����ΪGPRS_CHECK_CYCLE��������,�����Դ�ʱ��Ϊ���ڶ�GPRSģ���״̬���в�ѯ������¼����Ϊ�Ƿ���Ҫ���������ݡ�
//��СΪGPRS_CHECK_CYCLE��,���Ϊ5*GPRS_CHECK_CYCLE������ʱ�����úܴ�
//��100*GPRS_CHECK_CYCLE���Է�ֹ���ݶԱ��ؿ�ͨ����Ӱ��
#define  GPRS_SELF_CHECK_CYCLE	 		5*GPRS_CHECK_CYCLE
#endif



#if   TEST_SELF_CHECK > 0
#define  GPRS_HEART_FRM_TIME			6
#else

//GPRS����֡ʱ����,ʱ�䵥λΪ��,��СΪ2*GPRS_CHECK_CYCLE,
//ע��,ʵ�ʵ�����֡����ʱ������GPRS_CHECK_CYCLEΪʱ�䵥λ��,����������GPRS_CHECK_CYCLE
#define  GPRS_HEART_FRM_TIME		60//30	
#endif


//����֡���Ź������ֵ,ʱ�䵥λΪ��,������֡��������,�������ʱ������û���յ��κ�֡,������GPRSģ��
#define  GPRS_HEART_FRM_REC_OUTTIME		GPRS_HEART_FRM_TIME*4

/************************************* LOG Config  ******************************************************/
#define	LOG_FULL_ASSERT

//#define	LOG_INIT	(LOG_LEVEL_ERROR|LOG_LEVEL_WARN)
#define	LOG_INIT			LOG_ALL
#define PARA_READ_ALLOW //muxiaoqing add������ȡ
#define HW_VERSION_1_01 //muxiaoqing add to control HW feature
#define SW_VERSION  403//����汾�Ű汾��4.01�档
#define HW_VERSION  101  // Ӳ���汾�Ű汾��1.00�档

#define DEMO_APP   //muxiaoqing 0718

#define METERDATALEN	40    //ÿ���ȱ��ַ��Ϣ���ȣ���ǰΪ40�ֽڡ�

#endif

/*********************************************************************************************************
      								END OF FILE
*********************************************************************************************************/

