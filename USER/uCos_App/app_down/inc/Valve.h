/*
  ********************************************************************************************************
  * @file    Valve.h
  * @author  zjjin
  * @version V0.0.0
  * @date    04-15-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		Ϊ�ڼ�������ʵ�ֶ೧�Ҳ�ͬЭ�鷧�ŵļ��ݣ��ڱ������Ϣ�м�����
  *	����Э��汾�������ҷ�������س�����Valve.c�У����ļ���Valve.c�ļ���ͷ�ļ���
  *
  ********************************************************************************************************
  */  
#ifndef _VALVE_H_
#define _VALVE_H_

#include "publicfunc.h"
#include "StoreManage.h"
#include "Bsp.h"


//begin:���巧����Э��汾��
#define VALVE_DELU			0       //��³����Э�顣
#define VALVE_ELSONIC		1		//���ַ���Э�顣
#define	VALVE_JOYO			2		//������Դ����Э�顣
#define VALVE_BINIHI		3		//��ů�㷧��Э�顣
#define VALVE_LCARBO		4		//�ſ�������Э�顣
#define VALVE_PRECISE		5		//��������ͨѶЭ�顣
#define VALVE_ADE			6		//�����³���з�����Э�顣
#define TEMPGATHER_GUONENG		7	//�������ܻ����¶Ȳɼ�װ����Э�顣
#define PANELWIRED_ELSONIC		8	//������������׼Э�顣
#define VALVE_GUONENG			9	//���������¿ط���Э�顣





//end:���巧����Э��汾��

//begin:���巧�ز������ܺš�
#define ReadVALVE_All				0		//�����ص�������Ϣ
#define READROOM_TEMP				1		//�������¶�
#define READVALVE_STATUS			2		//�����ſ���״̬
#define READTEMP_RANGE				3		//���¶������ޡ�
#define SETHEAT_DISPLAY				4		//�����¿������ʾ����ֵ��
#define SETHEAT_VALUE				5		//�����¿������ʾ������ֵ��
#define SETROOM_TEMP				6		//���������¶�
#define SETTEMP_RANGE				7		//���������������¶ȡ�
#define SETVALVE_STATUS				8		//���÷�����״̬��
#define SETVALVE_LOCK				9		//������,������ʱҪ��֤����ȫ����
#define SETTEMP_ANDRANGE			10		//ͬʱ�趨�����¶Ⱥ��¶������޷�Χ��
#define SETVALVE_CONTROLTYPE		11		//���÷��ſ��Ʒ�ʽ��ʹ�ܡ����ܡ��Զ�����ʱ�ȡ�
#define SETVALVE_OFFSETTEMP			12		//���ò����¶ȡ�
#define SETVALVE_HYSTEMP			13		//���÷��������²�(�ͻ��¶�)
#define SETTEMP_GATHERPERIOD		14		//�����¿�����¶Ȳɼ����ڡ�
#define SETTEMP_UPLOADPERIOD		15		//�����¿�����¶��ϱ����ڡ�
#define READ_VALVEDATA				16		//ͨ��ʱ��������ã���ȡ���������ݣ���������ʱ�䡢����ˮ�¶ȡ������¶ȡ�
#define WRITE_DIVHEATDATA			17		//ͨ��ʱ��������ã��򷧿����·���̯����ֵ��
	



//end:���巧�ز������ܺš�

extern uint8 gVALVE_Table[][2];



extern uint8 ValveContron(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback);





#endif  //  #ifndef _VALVE_H_

