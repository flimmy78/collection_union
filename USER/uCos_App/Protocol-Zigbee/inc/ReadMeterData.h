
#ifndef _READMETERDATA_H
#define _READMETERDATA_H

#define TIMING_ALL		1
#define MSG_18			2
#define MSG_1C			3
#define MSG_1D			4




#define FORALL	0x0A	//Ⱥ��
#define FORONE	0x0B	//����


extern void* CmdMsgQ[10];               	//������Ϣָ������, ���洢10����Ϣ
extern OS_EVENT *CMD_Q;


#pragma pack(1)

typedef struct{
	uint8  MultiFlag;
	uint8  SendPackIndex;
	uint16 StartMeterSn;
	uint16 SendMeterNums;
	
	}MultiFrameMsg_0E;
	
typedef struct{
	uint8  MultiFlag;
	uint32 SendPackIndex;
	uint32 StartReadAddr;
	uint32 SendByteNums;
	uint32 EndReadAddr;
	
	}MultiFrameMsg_1A;
	
typedef struct{
	uint8  MultiFlag;
	uint32 SendPackIndex;
	uint32 StartReadAddr;
	uint32 SendByteNums;
	uint32 EndReadAddr;
	
	}MultiFrameMsg_1E;
#if 0
typedef struct{
	uint8  MultiFlag;
	uint8  SendPackIndex;
	uint16 StartMeterSn;
	uint16 SendMeterNums;
	
	}MultiFrameMsg_22;
#else
typedef struct{
	uint8  MultiFlag;
	uint8  SendPackIndex;
	uint8  time[6];/*�洢ʱ��*/
	uint8  device_type;/*�豸����*/
	uint8 SendMeterNums;
	
	uint16 StartMeterSn;
	}MultiFrameMsg_22;

typedef struct
	{
	uint8  MultiFlag;
	uint8  SendPackIndex;
	uint8  time[6];/*�洢ʱ��*/
	uint8  device_type;/*�豸����*/
	uint8 SendMeterNums;
	
	uint16 StartMeterSn;
	}MultiFrameMsg_92;


#endif
typedef struct{
	uint8  MultiFlag;
	uint8  SendPackIndex;
	uint8  time[6];/*�洢ʱ��*/
	uint8  device_type;/*�豸����*/
	uint8 SendMeterNums;
	
	uint16 StartMeterSn;
	}MultiFrameMsg_40;
/*begin:yangfei added 2014-02-24 for support ʱ��ͨ��������ȷ����*/
typedef struct 
{
	uint8 have_next_flag; /*��������־,0��ʾ�޺�������1��ʾ�к�����;0x10��ʾ�쳣��Ӧ*/
	uint8 packet_serial;/*�����*/
	uint8 time[6];/*�洢ʱ��*/
	uint8 device_type;/*�豸����*/
	uint8 userdata_number;/*�û����ݸ���*/
}HistoryDataHead;

typedef struct heatcost_meter /*�ȷ�����û�����9B*/
{
	uint16 address;/*�Ǳ��ַ*/
	uint8  length;/*�Ǳ����ݳ���*/
	uint8 currut_cost[3];/*��ǰ����ֵ*/
	uint8 front_temperature;/*(BCD��)ǰ����¶�,��λ:���϶�*/
	uint8 back_temperature;/*(BCD��)������¶�,��λ:���϶�*/
	uint8 error_code;/*(HEX��)����澯����*/
}HeatCost;

typedef struct time_on_off_area/*ʱ��ͨ��������û�����*/
{
	uint8 user_id;/*�û����*/
	uint16 address;/*�豸��ַ*/
	uint16 area;/*�û����*/
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
                             bit1 ����NTC����        1���ϣ�0����;
                             bit0 ������״̬��1������0����*/	
}TimeOnOffArea;
/*end:yangfei added 2014-02-24 for support ʱ��ͨ��������ȷ����*/

typedef struct {  //�������������ṹ�塣
	uint8 u8IP[4];
	uint16 u16Port;
	uint8 u8Address[6];//
	uint8 u8HostAddress[6];// ��վ��ַ
	uint16 u16SoftVer;/*���磺0x0232����2.32*/
	uint16 u16HardwareVer; /*���磺0x0232����2.32*/
	//uint8  u8Reserved[3];  /*�������ֽ�*/

	uint8 Method;// �������ȼ�����20H;����ʽ�ȷ����A0H;ʱ��ͨ�������B0H;
	uint8 DataSource;// ��0X0A ��λ����0X0B���س���
	uint16 Period;// ��̯���ڣ����ӣ�
	uint8 u8LogReportTime; //0x00 :�� ��0x01:����
	uint8 u8LogOpenType; // 0X00: �� ��0X01 ������
	uint8 u8LogCompressType; // 0X00:ѹ����0X01����ѹ��
 
}StruJZQInit;


typedef struct
{

	uint8 u8backpackFlag;
	uint8 u8NumPack;
	uint8 u8DeviceType;
	uint8 u8UserNum;	
}StruZLTB;



typedef struct{
	uint8 Method;// �������ȼ�����20H;����ʽ�ȷ����A0H;ʱ��ͨ�������B0H;
	uint8 DataSource;// ��0X0A ��λ����0X0B���س���
	uint16 Period;// ��̯���ڣ����ӣ�
	uint8 u8LogReportTime; //0x00 :�� ��0x01:����
	uint8 u8LogOpenType; // 0X00: �� ��0X01 ������
	uint8 u8LogCompressType; // 0X00:ѹ����0X01����ѹ��
	uint8 u8IPAddr0;	//0�ǵ�λIP  3�Ǹ�λIP 16�����·�
	uint8 u8IPAddr1;
	uint8 u8IPAddr2;
	uint8 u8IPAddr3;
	uint8 u8HostPortHigh;  // ��վ�˿ںŸ�8λ
	uint8 u8HostPortLow;   // ��վ�˿ں� ��8λ


         //�Ա��������
}StruCSXF;   //�����·��ṹ��





typedef struct
{
	uint8 ForAllOrOne;
	uint8 Device_type;
	uint8 Channel;
	uint8 ProtocolVer;
	uint8 ControlCode;
	uint8 Lenth;
}FrameMsg_42;	
#pragma pack()

extern uint8 test_proMsg_04(_ProtocolType &_ProData);
extern uint8 DEAL_ProcessMsg_06( _ProtocolType &_ProData);
extern uint8 DEAL_ProcessMsg_0E(_ProtocolType &_ProData, uint8 *ControlSave);
extern uint8 DEAL_ProcessMsg_18(_ProtocolType &_ProData);
extern uint8 DEAL_ProcessMsg_18_ForOne(_ProtocolType &_ProData);
extern uint8 DEAL_ProcessMsg_18_ForAll(_ProtocolType &_ProData);
extern void DEAL_MSG18_ForAll(uint8 *pData);
extern uint8 DEAL_ProcessMsg_1A(_ProtocolType &_ProData, uint8 *ControlSave);
extern uint8 DEAL_ProcessMsg_1C(_ProtocolType &_ProData);
extern uint8 DEAL_ProcessMsg_1C_ForOne(_ProtocolType &_ProData);
extern uint8 DEAL_ProcessMsg_1C_ForAll(_ProtocolType &_ProData);
extern void DEAL_MSG1C_ForAll(uint8 *pData);
extern uint8 DEAL_ProcessMsg_1E(_ProtocolType &_ProData, uint8 *ControlSave);
extern uint8 DEAL_ProcessMsg_22(_ProtocolType &_ProData, uint8 *ControlSave);
extern uint8 DEAL_ProcessMsg_3C(_ProtocolType &_ProData);
extern uint8 DEAL_ProcessMsg_3E(_ProtocolType &_ProData);
extern uint8 DEAL_ProcessMsg_40(_ProtocolType &_ProData, uint8 *ControlSave);
extern uint8 DEAL_ProcessMsg_42(_ProtocolType &_ProData);





#endif
