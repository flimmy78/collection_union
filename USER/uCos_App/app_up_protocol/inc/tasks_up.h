/*******************************************Copyright (c)*******************************************
         							ɽ������ռ似����˾(�����ֲ�)                                                 
**  ��   ��   ��: tasks_up.h
**  ��   ��   ��: ������
**  �� ��  �� ��: 2012.08.07 
**  ��        ��: ����վ��ͨ������
**  �� ��  �� ¼:   	
*****************************************************************************************************/
#ifndef _TASKS_UP_H
#define _TASKS_UP_H

#define  BLK_NUM                         	10    		//�ڴ������Ҳ����Ϣ���еĳ��ȣ��ɽ���֡��������
/*begin:yangfei added 2013-03-15  for ����buff �����С*/
#define  RECE_BUF_LEN                    300 	//256    		//ÿ���ڴ泤�ȣ�Ҳ�ǿɽ���֡����󳤶�
/*end:yangfei added 2013-03-15  for ����buff �����С*/
//�����ڴ����
extern OS_MEM* CommTxBuffer;
extern uint32 CommTxPart[BLK_NUM][RECE_BUF_LEN];
extern OS_EVENT *CommTxPartSem;								//���ڴ����󶨵��ź���

extern void* MsgGrp[BLK_NUM];               				//������Ϣָ������
extern OS_EVENT *Str_Q;

extern void* UpSend_Q_Grp[BLK_NUM];
extern OS_EVENT *UpSend_Q;

#pragma	pack(1)
typedef struct {
	uint8   GprsGvc;		// TRUE OR  FALSE��ΪTRUEʱ����ʾ��GPRS�Ĺ������漸��������Ч
    uint8   Device;			// �������ݵ��豸��GPRS, Zigbee, RS485
    uint16  ByteNum;		// �����ֽ���
    uint8   buff[1];		//��������
} UP_COMM_MSG;

#pragma pack()

void Createtasks_up(void);
void CreateMboxs_up(void);


#endif
/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/
