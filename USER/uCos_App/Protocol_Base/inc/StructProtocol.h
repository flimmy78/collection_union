/*=================================Copyright (c)===========================
                             ɽ������ռ似����˾
                             
 ==---------------------------�ļ���Ϣ-------------------------------------                                 
  ��   ��   ��: BaseProtocol.h
  ��   ��   ��: ������
  ��   ��   ��: 0.1
  ��  �� �� ��: 2012��8��6��
  ��        ��: ����Э���֡���ṹ��ʹ�õĳ���
 ==------------------------------------------------------------------------
==========================================================================*/

#ifndef _STRUCT_PROTOCOL_H
#define _STRUCT_PROTOCOL_H

#include <includes.h>

	#ifdef __cplusplus
	extern "C" {
	#endif

#define  FRAME_START  		0x7B           	//֡��ʼ�ֽ�
#define  FRAME_END    		0x7D           	//֡�����ֽ�
#define MAX_FRMBUF_LEN      5120     		//����֡���������� 5K

#pragma pack(1)
typedef struct DataStruct
{
 	uint8* pData;
 	uint16 lenData;
 	uint8* pDataBack;
 	uint16 lenDataBack;
}DataField;

typedef  struct
{                
	uint8 _Ver;								//Э��汾��
	uint8 SourceAddr[6];		    		//Դ��ַ�����ͷ���ַ
 	uint8 DestAddr[6];						//Ŀ�ĵ�ַ�����շ���ַ
 	uint8 MsgID;							//��Ϣ����
 	uint16 MsgLength;						//��Ϣ�峤��
 	uint8 MsgType;							//��Ϣ����
 	uint8 TimeSmybol[6];					//TS ʱ���ǩ
 	uint8 HeadCheck;						//��ϢͷЧ��
 	uint8 ContentCheck;						//��Ϣ��Ч��
 	
   	DataField Data;       					//	������
} _ProtocolType;

typedef struct SelfDefineParaStruct
{
	
	uint8  LCDContrastValue;//Һ���Աȶ�ֵ
	uint8  ChannelType;//ͨ�����ͣ�0:GPRS,1��CDMA,2:��̫����3:SMS,4:ר������
	
}SelfDefinePara;

//ͨѶ��������
typedef struct CommParaStruct  
{
  	//�ն�Ϊ�ͻ�ʱʹ�õ�ͨ�Ų���GPRS/CDMA
  	uint8  PPIP1[4];         				//����IP
  	uint8  PPIP2[4];        			 	//����IP
  	uint16 PPPort;           				//ǰ�û��˿� 
  	uint16 PPport;           				//ǰ�û����ö˿�  
  	uint8  APNName[16];      			// APN����
  	
  	uint8  MasPhNum[8];      			//��վ�绰����
  	uint8  SmsNum[8];       				 //�������ĺ���
  	
  	uint8  SocketType;       			//GPRSͨѶ��ʽ��TCP/UDP     0:TCP    1:UDP
  	
  	uint8  HeartCycle;       				//��������
  	uint8  flagCon;          				//��Ҫ��վȷ�ϵ�ͨѶ����
  	
  	uint8  InterfacePw[6];   			//�������룬6�ֽ�ASCII��
  	uint8  Jzqpassword[16];  	 		//���������� 
  	
  	//�ն���̫��ͨ�Ų��� 
  	uint8  TermIP[4];
  	uint8  TermSubMask[4];       		//��������
  	uint8  TermGateWay[4];       		//���� 
  	uint8  UserName[16];  				//��¼��
  	uint8  lenUserName;
  	uint8  UserPw[16];    				//��¼����  
  	uint8  AgentSer[4];   				//���������
  	uint16 AgentSerport;    		 	//����������˿�
  	uint8  AgentConnMode;        		//�������ӷ�ʽ
  	uint16 TermPort;         				//�ն˼����˿�
  	uint8  DNSName[40];                     //��̬����muxiaoqing
  	uint8  SwitchIPorDNS;                   //IP��ַ�������л�muxiaoqing
  	
	uint8 RelandDelay;						//�������µ�¼��ʱʱ�䣬��λΪ Min
	uint8 RelandTimes;						//�������µ�¼����
  	uint8  HeartFlag;        				//��������
} CommPara;

typedef	struct	GlobalVariableStruct
{
	uint8 addr[5];
	uint8  TermPw[16];      //����������
	uint8  InterfacePw[6]; //�������룬6�ֽ�ASCII��           //IP	
	uint8  PPIP[15];       //ǰ�û�IP
	uint8  LenPPIP;        //IP����
 	uint8  PPPort[5];      //ǰ�û��˿� 
	uint8  LenPPPort;      //�˿ںų��� 
	uint8  APNName[16];    //APN����
	uint8  LenAPNName;     //APN���Ƴ���
	uint8  HeartCycle;     //��������
	uint8  SocketType;//0:TCP;1:UDP
	uint8  ChannelType;//ͨ�����ͣ�0:GPRS,1��CDMA,2:��̫����3:SMS,4:ר������

	uint8  AllowAutoToMas; //������ֹ�����ϴ�
	
	//��̫������
	uint8  TermIP[4];
	//uint8  LenTermIP;
	uint8  TermSubMask[4];       //��������
	//uint8  LenTermSubMask;
	uint8  TermGateWay[4];       //����
	//uint8  LenTermGateWay;
	uint16 TermPort;        //uint8 TermPort[5]; //�ն˼����˿�
	//uint8  LenTermPort;
	uint8  TermMACAddr[6];		//��̫��MAC��ַ
	
	
	//��������
	uint8 LCDContrastValue;
	uint8 ErrCode; //���洢
}GlobalPara;

#pragma pack()

	#ifdef __cplusplus
	}
	#endif
#endif
