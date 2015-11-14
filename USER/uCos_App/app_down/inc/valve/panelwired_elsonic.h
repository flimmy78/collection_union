/*
  ********************************************************************************************************
  * @file    panelwired_elsonic.h
  * @author  zjjin
  * @version V0.0.0
  * @date    09-29-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		亿林有线温控面板协议功能实现相关代码头文件。
  *
  ********************************************************************************************************
  */

#ifndef _PANELWIRED_ELSONIC_H_
#define _PANELWIRED_ELSONIC_H_

#include "publicfunc.h"
#include "storeManage.h"
#include "bsp.h"
//#include "meter.h"



extern uint8 PanelWiredControl_Elsonic(MeterFileType *p_mf, uint8 functype, uint8 *p_DataIn, uint8 *p_databuf, uint8 *p_datalenback);
extern void Create_ElsonicWiredPanel_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame);
extern void Create_ElsonicWiredPanel_SetInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame);
extern void Create_ElsonicWiredPanel_SetHeat_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame);
extern void Create_ElsonicWiredPanel_SetRoomTemp_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame);
extern void Create_ElsonicWiredPanel_SetRoomTempRange_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame);
extern void Create_ElsonicWiredPanel_SetValve_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame);


#endif  //#ifndef _PANELWIRED_ELSONIC_H_

