/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)
**  ��   ��   ��: timingpro.h
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��8��10��
**  ��        ��: ��ʱ�������ļ�
**	�� ��  �� ¼:
*****************************************************************************************************/
#ifndef				_TIMINGPRO_H
#define				_TIMINGPRO_H

extern TimingState	gREAD_TimingState;
extern uint16 gu16ReadMeterNum;
extern uint16 gu16ReadMeterSuccessNum;

/*begin:yangfei modified 2012-12-24 add path*/
#if  0
extern uint8 READ_ReadOneMeter(uint16 MeterSn);
#endif
extern uint8 READ_ReadOneMeter(char *path, uint16 MeterSn);
/*end  :yangfei modified 2012-12-24 */
extern uint8 READ_SaveReadedDay(uint8 *pReadTime, uint16 MeterSn, uint8 *DataBuf, uint32 DataLen);
extern uint8 READ_SaveCmplFlag(uint16 MeterSn);
extern uint8 READ_ReadAllMeters(void);
extern void READ_ParaInit(void);
/*begin:yangfei modified 2013-08-03 for added HYDROMETER*/
int  HYDROMETER(DELU_Protocol *pProtocoalInfo, uint8 ProtocolVer);
uint8 HYDROMETER_METER_ReceiveFrame(uint8 dev, uint8 *buf, uint16 Out_Time, uint8 *datalen, uint8 ProtocolVer);
/*end:yangfei modified 2013-08-03 for added HYDROMETER*/

#endif
