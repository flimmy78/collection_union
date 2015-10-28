/*******************************************Copyright (c)*******************************************
                                                          
**  ��   ��   ��: StoreManage.h
**  ��   ��   ��: ������
**  �� ��  �� ��: 2012.08.09 
**  ��        ��: �洢����ͷ�ļ�
**	��		  ע: ��ϵͳ�Ĵ洢����ΪSD��������Ϊ2GB���ϣ���֤�˾��Թ��Ĵ洢�ռ䡣SD����512�ֽ�Ϊһ�飬
**				  ���������������˿����ʴ������Է���ʱ���ع��࿼�ǿ�����⡣
**				  �洢�ռ�ϴ�ÿ����ز��������ݾ����� 1MByte �ռ�
*****************************************************************************************************/
#ifndef _STOREMANGE_H
#define _STOREMANGE_H

#define TIME_NODE_NUM_MAX		24							//���ó���ʱ��ڵ��������

#define PACK_MAX				900



/********************************�����������洢���� -- End********************************************/

/********************************�ȼ��������ݴ洢���� -- Start****************************************/

#define METET_FILE_PER_LEN_STORE	64				//ÿ������ʵ��ռ��25���ֽڣ��洢ʱռ��64�ֽڿռ䣬����Ԥ��
#define METER_NUM_MAX				600								//�������¿ɽӵ��ȼ�������������
#define METER_PER_CHANNEL_NUM		256								//ÿ��ͨ������������
#define METER_FRAME_LEN_MAX			128								//����֡��󳤶�, �����ݱ��뱣֤Ϊ2��N�η�
#define STORE_DAYS_PER_MONTH		31								//ÿ�´洢31������ݣ�������û31�ŵȣ������ദ��
#define METERS_DATA_LEN				0x12E00							//ÿһ��ʱ��ڵ㣬�洢�����ݣ�ռ151�����ݿ�

#define METER_CHANNEL_NUM  7 

#define METER_DATA_TEMP			0xA00000							//��10M��ʼ�洢����
																	//�洢�����ȼ������¿���壬����״̬����Ҫ��ʱ�洢������
																	//����洢�����һ�������ݸ�ʽ
																	//��Ϣ����(1�ֽ�) + ָ������(1�ֽ�) + ���ֽ���(2�ֽ�) + ���� + ���� + ����....



#define	METERS_DATA_STORE_PUBLIC	0x1400000						//���ڴ洢��ָ���µ�����, ��20M��ַ��ʼ

//METER_DATA_STORE_ADDR,���ݴ洢�Ļ���ַ����2012��1��Ϊ��׼															
#define METERS_DATA_STORE_ADDR		0x1E00000						//���ڹ�ů���洢, ��30M��ַ��ʼ

#pragma	pack(1)
//���ݴ洢���䣺ÿ������TIME_NODE_NUM_MAX��������ݣ����յĳ���ʱ���洢�ڳ���ʱ���ĵ�һ��ʱ��ڵ�Ĳ�������ֻ��ÿ�յ�һ����
//��ʱ��ڵ����� ����Ч�ģ�
//���ṹ��Ϊÿһ������ʱ��ڵ�����ݴ洢�Ĳ���������ÿ������ʱ��㣬��ʼ����ǰ��Ҫ������г�ʼ��
typedef struct{
		uint8 	DataRecordTime[6];								//���ݴ洢��¼��ʱ��ڵ� ������ʱ���룬����һ���ò���
		//uint8   ReadCmplFlag[75];								//����������ɱ�־ 75*8=600
		uint16	MeterNums;										//��ʱ��ڵ㹲�洢�˶��ٸ���
		uint16  TimeNode[TIME_NODE_NUM_MAX];					//��ÿ����ʼʱ��㣬�ò�����Ч���洢���յ�ʱ��ڵ�����
		uint16 	TimeNodeNum;//�������˼�����ʱ�㡣	//���ڸ����ڣ�����������ʱ��ڵ㣬����¸ò�����
																//������ǰ��������Ч,�Ա�֤ÿ��ֻ�洢TIME_NODE_NUM_MAX���������
		//uint8	DataIdle[128-6-2-2*TIME_NODE_NUM_MAX - 2];
		uint8	DataIdle[118 - 2*TIME_NODE_NUM_MAX];		//DataIdle[128-6-2-2*TIME_NODE_NUM_MAX - 2];Ϊ������֡�Կ���룬��Ӵ˿�����
	}DataStoreParaType;											//����Ϊһ��ʱ��ڵ�Ĳ�������128�ֽ�
typedef struct{
		uint16 WriteFlag;					//�������Ƿ�д������
		uint16 LCDContrast;				//LCD�Աȶ�
	}LCDContrastPara;	//muxiaoqing add
typedef struct{
		uint16 WriteFlag;					//�������Ƿ�д������
		uint16 MeterStoreNums;				//���洢���ٿ������Ϣ
#ifdef DEMO_APP
        
        uint16 DEMOWriteFlag;					//�������Ƿ�д����ʾ�汾����
#endif		
	}MeterFilePara;

typedef struct{
		uint16 MeterID;						//�ȱ��ţ�Ψһ�������ܲ�������
		uint8 MeterAddr[7];					//�ȼ������ַ
		uint8 Manufacturer;					//���̴���
		uint8 ProtocolVer;					//Э��汾
		uint8 EquipmentType;				//�豸����
		uint8 ChannelIndex;					//ͨ����
		uint8 ValveProtocal;				//������Э��ţ����ڼ��ݶ೧�Ҳ�ͬ��������
		uint8 ValveAddr[7];					//���ŵ�ַ
		uint8 ControlPanelAddr[7];			//��������ַ
		uint8 BulidID;						//¥��
		uint8 UnitID;						//��Ԫ��
		uint16 RoomID;						//�����
		uint16 Reserved1;					//һ��Ԥ��8�ֽڡ�
		uint16 Reserved2;
		uint16 Reserved3;
		uint16 Reserved4;
	}MeterFileType;							//��25���ֽ�

typedef struct
{
	uint8  user_id;/*�û����*/
	uint16	address;  /*�豸��ַ*/
	uint16	area;/*�û����*/
	uint8  data_valid;/*�����Ƿ���Ч  ��ЧΪ0xA5����ЧΪ0x00*/
	uint32 total_energy;/*(BCD��)�û��������ۼ�ֵ,��2λΪС��λ����λ:kwh*/
	uint32 open_time;/*(BCD��)����ʱ�䣬��2λΪС��λ����λ:h*/
	uint32 proportion_energy;/*(BCD��)��ǰʱ����ڵ�������ֵ̯*/
	uint8  open_percent;/*��ǰʱ����ڵķ��ſ�������(HEX�� 0-100) */
	uint16 forward_temperature;/*(BCD��)��ˮ�¶ȣ���2λΪС��λ����λ:���϶�*/
	uint16 return_temperature;/*(BCD��)��ˮ�¶ȣ���2λΪС��λ����λ:���϶�*/
	uint16 room_temperature;/*(BCD��)�����¶ȣ���2λΪС��λ����λ:���϶�*/
	uint8  state;/*bit7 �������߹��ϣ�1���� 0���� 
								  bit6 ����Ƿ��״̬ 1Ƿ�� 0����  
								 bit5 �����ֵ״̬ 1��ֵ 0����	
								 bit4 �����ػ�״̬��1������0�ػ�; 
								 bit3 �������� 0������1����;
								 bit2 �����Ŷ�ת���ϣ�1���ϣ�0����; 
								 bit1 ����NTC����		 1���ϣ�0����;
								 bit0 ������״̬��1������0����*/	
	uint8 channel;/*ͨ����*/

}TimeAreaArith;
typedef struct{
		uint16 FrameLen_Max;				//������ÿ���Ϸ�����֡����󳤶ȣ����ݰ��ɶ������֡��ɣ�Ĭ�����ݰ����500Byte
		uint8  FrameToFrameTime;			//֡�� ʱ�� ��� ��λΪ �� 
	}DebugSetPara;

typedef struct{
	uint16 WriteFlag;						//�������Ƿ�д������
	uint8 TermAddr[6];
	uint8 HostAddr[6];
	DebugSetPara DebugPara;
	/*begin:yangfei added for add support time_on_off*/
	uint8 DeviceType;
	/*end:yangfei added for add support time_on_off*/
	}TermParaSaveType;
	
typedef struct{
	uint16 WriteFlag;						//�������Ƿ�д������
	uint8 HostIPAddr[4];					//��վIP��ַ
	uint16 HostPort;						//��վ�˿�
	uint8 HeartCycle;						//�������ڣ���λΪ Min
	uint8 RelandDelay;						//�������µ�¼��ʱʱ�䣬��λΪ Min
	uint8 RelandTimes;						//�������µ�¼����
	}UpCommParaSaveType;

typedef struct{
	uint16 WriteFlag;						//�������Ƿ�д������
	uint8 MeterReplenishTimes;					//�������������Χ0-9.Ĭ�ϲ���1�Ρ�
	uint16 MeterInterval;						//������ʱ��������λms.��Χ200-9999ms.
	uint8 ValveReplenishTimes;					//��������������Χ0-9.Ĭ�ϲ���1�Ρ�
	uint16 ValveInterval;						//������ʱ��������λms.��Χ200-9999ms.
	}ReplenishReadMeterSaveType;



typedef struct{
		uint16 MeterSn;
		uint8 HeatMeterAddr[7];				//�ȼ������ַ
	}MeterChannelType;

typedef struct{
		uint8 HeatMeterAddr[7];				//�ȼ������ַ
		uint8 ValveAddr[7];					//���ŵ�ַ
	}ValveControlType;

typedef struct{
		uint8 HeatMeterAddr[7];				//�ȼ������ַ
		uint8 ControlPanelAddr[7];			//�¿�����ַ
	}ControlPanelType;
	
typedef struct{								//����״̬��Ϣ
	uint8 TimingStartTime[6];
	uint8 TimingEndTime[6];
	uint16 TimingMeterNums;
	uint16 ReadCmplNums;
	}TimingState;

typedef struct{					//GPRS APN�����ѡ�����á�
	uint16 WriteFlag;
	uint8 GprsAPNSet;
	}GprsAPNSelect;



#pragma pack()


/********************************�ȼ��������ݴ洢���� -- End****************************************/	

extern uint16 gPARA_MeterNum;										//��ǰ����������
extern MeterFileType gPARA_MeterFiles[METER_NUM_MAX];				//�ڴ��д洢��ǰ������Ϣ
extern uint8 gPARA_SendIndex_Prevous;								//������һ�����ݰ������к�
extern uint16 gPARA_TimeNodes[TIME_NODE_NUM_MAX];
extern uint16 gPARA_TimeNodesNum;


extern uint16 gPARA_MeterChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];			//6��ͨ���ȼ�������Ϣ, MeterSn
extern uint8   gPARA_Meter_Failed[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];	/*7��ͨ��ʧ�ܼ�¼*/
extern uint16 gPARA_MeterChannelNum[METER_CHANNEL_NUM];								//ÿ��ͨ��������

extern uint16 gPARA_ConPanelChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];		//6��ͨ��ÿ��ͨ�����¿������Ϣ, MeterSn
extern uint16 gPARA_ConPanelChannelNum[METER_CHANNEL_NUM];							//ÿ��ͨ�����ص��¿��������

extern uint16 gPARA_ValveConChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];		//6��ͨ��ÿ��ͨ���ķ�����Ϣ, MeterSn
extern uint16 gPARA_ValveConChannelNum[METER_CHANNEL_NUM];							//ÿ��ͨ�����صķ�������

extern TermParaSaveType gPARA_TermPara;								//��������������
extern UpCommParaSaveType	gPARA_UpPara;							//GPRS����ͨѶ����
extern CommPara uPCommPara;//muxiaoqing
extern ReplenishReadMeterSaveType gPARA_ReplenishPara;

extern GlobalPara gStore_CommPara;
extern uint8 gLOG_SystemLog[1024];									//�洢ϵͳ��־
extern OS_EVENT *LogQueSem; 										//��������־���ź���
extern OS_EVENT *LogWriteSem;										//����д����־���ź���


/********************������Ϣ�鿴�������õ��ı���****************************/
extern uint8 gu8ReadAllMeterFlag;
extern uint8 gu8ReadDataStore[METER_FRAME_LEN_MAX];

/********************MBUS��·�����õ���ȫ�ֱ���*********************************/
extern uint8 gu8MBusShortRising; //MBUS��·ָʾ�����Ƿ���������أ�0-û�У�1-���������ء�
extern uint8 gu8MBusShortFlag[METER_CHANNEL_NUM];  //��Ƕ�ӦMBUSͨ��ͨ����·,1-��·��0-����·��
extern uint8 gu8NowMBUSChannel;    //��¼��ǰMBUSͨ���

extern uint8 gu8MBusShortTime;

extern uint8 gu8ReadValveFail;
	

extern uint8 gu8ErrFlag;
extern uint16 gu16ErrOverTime;

extern char  gcYesterdayPath[];




extern uint8 STORE_GetAddrOfMeterData(uint8 *pTime, uint16 MeterSn, uint32 *AddrOut, uint8 Flag);
extern uint8 PARA_ReadMeterInfo(uint16 MeterSn, MeterFileType *pMeterFile);
extern uint8 STORE_CalcTime(uint8 *pReadTime, uint32 *pTimeOffSet, uint8 Flag);
extern uint8 StoreYesterday(void);
extern uint8 STORE_GetTimeNodeInfo(uint16 InTime, uint8 *pNodeIndex, int8 *pNodeOffset);
extern uint8 STORE_FindTimeNodeIndex(uint16 InTime, uint16 *pTimeNode, uint8 *pNodeIndexOut);
extern 	uint8 FindTimeNodeIndex_Forward(uint16 InTime, DataStoreParaType *datastorePara, uint8 *pNodeIndexOut,uint8 *pYesterday);
extern uint8 STORE_InitTimeNodePara(uint8 *pDateTime, uint8 NodeIndex);
extern uint8 STORE_FindDayStartAddr(uint8 *pReadTime, uint32 *pAddrOut);

extern void PARA_InitGlobalVar(void);
extern void GetVipDataFormSD(void);


#endif
