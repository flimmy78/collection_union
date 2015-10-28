/*
  ********************************************************************************************************
  * @file    valve_ADE.h
  * @author  zjjin
  * @version V0.0.0
  * @date    08-10-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		�����³Э�鷧��������ʵ����ش��룬�����ԭ��³����Э����Щ����
  		Ϊ����ԭ���ݣ���ӱ��ļ���
  *
  ********************************************************************************************************
  */  


#ifndef _VALVE_ADE_H_
#define _VALVE_ADE_H_

#include "publicfunc.h"
//#include "storeManage.h"
//#include "bsp.h"
#include "meter.h"


#define ADE_SETVALVE_STATUS			0	//�·����Ʒ���������
#define ADE_SETHEAT_VALUE			1	//�·��������ݵ���������
#define ADE_READROOM_TEMP			2	//�������¶�
#define ADE_SETTEMP_RANGE			3	//���������������¶ȡ�
#define ADE_SETVALVE_CONTROLTYPE	4	//���÷��ſ��Ʒ�ʽ��ʹ�ܡ����ܡ��Զ�����ʱ�ȡ�
#define ADE_SETROOM_TEMP			5	//���������¶�
#define ADE_READVALVE_STATUS		6	//������״̬
#define ADE_SETVALVE_OFFSETTEMP		7	//���ò����¶ȡ�
#define ADE_SETVALVE_HYSTEMP		8	//���÷��������²�(�ͻ��¶�)
#define ADE_SETTEMP_GATHERPERIOD	9	//�����¿�����¶Ȳɼ����ڡ�
#define ADE_SETTEMP_UPLOADPERIOD	10	//�����¿�����¶��ϱ����ڡ�
#define ADE_READ_VALVEDATA			11	//ͨ��ʱ��������ã���ȡ���������ݣ���������ʱ�䡢����ˮ�¶ȡ������¶ȡ�
#define ADE_WR_HEATDATA				12	//ͨ��ʱ��������ã��·�д���̯����ֵ��





extern uint16 gADEVALVE_Table[][2];











extern uint8 ValveContron_ADE(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback);
extern uint8 ADEValveCommunicate(MeterFileType *p_mf,uint8 ADE_Functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback);

#endif  //#ifndef _VALVE_ADE_H_

