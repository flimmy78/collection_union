/*
  ********************************************************************************************************
  * @file    valve_guoneng.h
  * @author  zjjin
  * @version V0.0.0
  * @date    10-15-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		威海国能自控科技"国能温控阀"用。
  *
  ********************************************************************************************************
  */


#ifndef _VALVE_GUONENG_H_
#define _VALVE_GUONENG_H_

#include "publicfunc.h"
#include "storeManage.h"
#include "bsp.h"
//#include "meter.h"



extern uint8 ValveContron_GuoNeng(MeterFileType *p_mf, uint8 functype, uint8 *p_DataIn, uint8 *p_databuf, uint8 *p_datalenback);
extern void Create_GuoNengVave_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame);
extern void Create_GuoNengVave_SetHeat_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame);
extern void Create_GuoNengVave_SetRoomTemp_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame);
extern uint8 GuonengValve_ReadData_Communicate(uint8 *DataFrame, uint8 *DataLen);



#endif  //#ifndef _VALVE_GUONENG_H_

