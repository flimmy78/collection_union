 /*=================================Copyright (c)===========================
                             ɽ������ռ似����˾
                             
 ==---------------------------�ļ���Ϣ-------------------------------------                                 
  ��   ��   ��: BaseProtocol.h
  ��   ��   ��: ������
  ��   ��   ��: 0.1
  ��  �� �� ��: 2012��8��6��
  ��        ��: ����Э��Ļ���
 ==------------------------------------------------------------------------
==========================================================================*/

#ifndef _BaseProtocol_H
#define _BaseProtocol_H

class BaseProtocol{
	
	protected:
 	  
 	BaseProtocol(){ frameStart = FRAME_START;frameEnd = FRAME_END;} 
	public : 
 		uint16 minFrameLen;      //Э����̳���
		uint16 FrameLen;      
		uint8 frameStart;
		uint8 frameEnd;	  
		//����֡У���
		//uint8 CountCS(uint8* _data,uint16 _len);//�ۼ�У��
		//uint8 CountCS(uint8* _data,uint16 _len,uint8 cs);//�ۼ�У��
		//uint16 CRCCheck(uint8* _data,uint16 _len);  //CRCУ��
		//uint16 CRCCheck_Table(uint8 *puchMsg, uint16 usDataLen,uint16 CRC); /* puchMsg:Ҫ����CRCУ�����Ϣ  usDataLen��Ϣ���ֽ��� */ 
		uint8 flagFir; //֡ͷ��־
	   	uint8 flagFin; //֡β��־
		//static void  Reversal(uint8* pStr,uint16 len) ;  //�ߵ��ַ���
		
		//void   Reversalmemcpy(uint8 *destin, uint8 *source, uint16 n);//��Դ��ַ����N���ֽڷ���浽Ŀ�ĵ�ַ 
	    
	
	      
		//��֡������������ʵ�֣����麯����  
		virtual void CreateFrame(uint8* _data,uint8* _sendFrame,uint16 &lenFrame) = 0;    
		virtual uint8 CreateHeartFrm(uint8* HeartFrm,uint8 flag)=0;
		//����Э���������֡������������ʵ�֣����麯����
		 
		virtual uint8 ReceiveFrame(uint8* _receFrame,uint16 &lenFrame)=0;
		virtual uint8 AnalysisFrame(uint8* _receBuf,uint16 &lenBuf)=0;
		virtual uint8 IfFrameHead(uint8 bHead)=0;
		

};

#endif
