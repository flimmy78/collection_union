
#ifndef _ZIGBEE_PROTOCOL_H
#define _ZIGBEE_PROTOCOL_H

#include "BaseProtocol.h"


#define 	PROTOCOL_VER 	0x03  //Э��汾��,���Ʊ������Э��汾��0x03��Ϊ0x04. modify by zjjin,20150504.

class _Protocol : public BaseProtocol
	{
		private:
			uint8 ControlSave[20];				//��֡����ʱ���֣���֡ѭ�����Ʊ���
			_ProtocolType MultiBackup;			//���ڶ�֡����ʱ��������һ������֡
			uint8 DataFiledBack[100];			//���ڶ�֡����ʱ��������һ������֡��������
	
			
			uint8 ProcessFrame(_ProtocolType &_ProData);
			uint8 ProcessMsg_11(_ProtocolType &_ProData, uint8 *pControlSave);
			uint8 CountCheck(uint8 *_data, uint16 _len);
			
		public:
		
			_Protocol()
			{
				minFrameLen = 28;
				memset(ControlSave, 0x00, 20);
				memset((uint8 *)&MultiBackup, 0x00, sizeof(_ProtocolType));
				memset(DataFiledBack, 0x00, sizeof(DataFiledBack));
			};
			
			virtual uint8 IfFrameHead(uint8 bHead);
			virtual void  CreateFrame(uint8* _data, uint8* _sendBuf, uint16 &lenFrame);
			virtual uint8 ReceiveFrame(uint8* _receBuf, uint16 &lenFrame);
			virtual uint8 AnalysisFrame(uint8* _receBuf, uint16 &lenBuf);
			virtual uint8 CreateHeartFrm(uint8* HeartFrm, uint8 flag);
	
	};
  

extern void PRO_InitGlobalVar(void);

extern uint8 gDebugModule[20];	
   
#endif


