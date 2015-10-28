/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)                                                          
**  ��   ��   ��: SysLog.H
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��9��10�� 
**  ��        ��: ��־��¼ϵͳ������Ϣ
**	�� ��  �� ¼:   	
*****************************************************************************************************/
#ifndef _SYSLOG_H
#define _SYSLOG_H

extern uint32 gLOG_TimeCounter;
extern uint8 gLOG_LogLevelFlag;

extern void LOG_QueueCreate(void);
extern uint8 LOG_WriteSysLog(const uint8 *pInLog, uint16 LogLen);
extern uint8 LOG_WriteSysLog_Format(uint8 Flag, const char *format, ...);
extern void TaskLog(void *pdata);
extern void LOG_assert_failed(uint8* file, uint32 line);
extern void LOG_HandleLogBuf(void);
extern void LOG_HandleEXTIBuf(const uint8 *pInLog,uint8 index);
typedef struct{	
	    uint16 WriteFlag;					//�������Ƿ�д������
	    uint16 LOGStoreNums;				//���洢���ٿ�LOG��Ϣ
	}ERRLogFilePara;	

typedef struct{	
		uint8 LOGsting[96];					//�ַ�����Ϣ
		uint8 LOGLevel;//��־�ȼ�
		uint8 EquipmentType;				//�豸����Ԥ��
	}ERRLogFileType;							
typedef struct{	
	    uint16 WriteFlag;					//�������Ƿ�д������
	    uint16 LOGStoreNums;				//���洢���ٿ�LOG��Ϣ
	}ERRLogMBUSFilePara;	

typedef struct{	
		uint8 LOGsting[96];					//�ַ�����Ϣ
		uint8 LOGLevel;//��־�ȼ�
		uint8 EquipmentType;				//�豸����Ԥ��
	}ERRLogMBUSFileType;	

#endif
