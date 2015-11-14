/*
  ********************************************************************************************************
  * @file    tempgather_guoneng.h
  * @author  zjjin
  * @version V0.0.0
  * @date    09-17-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		威海国能自控科技"户用温度采集装置"用。
  		该设备只需要采集用户户内温度即可。
  *
  ********************************************************************************************************
  */


#ifndef _TEMPGATHER_GUONENG_H_
#define _TEMPGATHER_GUONENG_H_

#include "publicfunc.h"
#include "storeManage.h"
#include "bsp.h"
//#include "meter.h"



extern uint8 TempGather_GuoNeng(MeterFileType *p_mf, uint8 functype, uint8 *p_DataIn, uint8 *p_databuf, uint8 *p_datalenback);
extern void Create_GuoNengTemp_ReadFrame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame);
extern uint8 GuonengTemp_ReadData_Communicate(uint8 *DataFrame, uint8 *DataLen);
//extern uint8 Receive_Read_ElsonicParamFrame(uint8 dev, uint8 *buf, uint16 Out_Time, uint8 *datalen);




#endif  //#ifndef _TEMPGATHER_GUONENG_H_

