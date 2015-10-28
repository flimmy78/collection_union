/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)                                                          
**  ��   ��   ��: meter.h
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��9��10�� 
**  ��        ��: �ȼ�����Э�����
**	�� ��  �� ¼:   	
*****************************************************************************************************/
#ifndef				_HEAT_COST_METER_H
#define				_HEAT_COST_METER_H

#include "StoreManage.h"

#pragma pack(1)

typedef struct
{
  uint8 PackageTotal;
  uint8 PackageSerial;
  uint8 HavePackage;
  uint8 MeterTotal;
  uint8 Timenode[5];
}HeatCost_DataHead;

#pragma pack()

extern uint8 HeatCost_ReadMeterDataTiming(uint16 MeterSn, uint8 *EleBuf,char*path);

extern uint8 HeatCost_METER_DataItem(DELU_Protocol *pProtocoalInfo,char*path);
extern void HeatCost_METER_CreateFrame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame);
extern uint8 HeatCost_METER_MeterCommunicate(DELU_Protocol *pData, uint8 *pResBuf, uint8 *pDataLenBack);
extern uint8 HeatCost_METER_DELU_AnalDataFrame(DELU_Protocol *pProtoclData, uint8 *pRecFrame,HeatCost_DataHead *pHeatCost_DataHead);
#endif

