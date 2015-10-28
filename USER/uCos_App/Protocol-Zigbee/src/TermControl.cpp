/*******************************************Copyright (c)*******************************************
**									ɽ������ռ似����˾(�����ֲ�)                                **                            
**  ��   ��   ��: TermMeterData.cpp																  **
**  ��   ��   ��: ������																		  **
**	��   ��   ��: 0.1																		      **
**  �� ��  �� ��: 2012��9��20�� 													     	      **
**  ��        ��: ��Ŀ����									      						          **
**	�� ��  �� ¼:   							  												  **
****************************************************************************************************/
/********************************************** include *********************************************/
#include <includes.h>
#include "app_down.h"
#include "app_flashmem.h"
#include "app_up.h"
/**********global************/
/*begin:yangfei added */
uint16 gVersion=(((((SW_VERSION)/100)<<8)&0x0F00)+((((SW_VERSION%100)/10)<<4)&0x00F0)+((SW_VERSION%10)&0x000F));  //����汾��
uint16 gHWVersion=(((((HW_VERSION)/100)<<8)&0x0F00)+((((HW_VERSION%100)/10)<<4)&0x00F0)+((HW_VERSION%10)&0x000F));  //Ӳ���汾��


static char  UpdatePath[] = "/update/V02.01.bin";
static uint16 gu16TotalPacket = 0;  //��̬ȫ�֣���¼Զ�������ܰ�����
static uint32 gu32TotalSize = 0;    //Զ������Դ�����С��
static uint16 gu16MaxPackSize = 0;  //Զ�������ܰ�����
static uint16 gu16Version = 0;
static uint8 gu8UpdataCheckTimes = 0;  //��ѯȱ��������¼����һ��������ѯȱ����������15��ʱ����������ʧ�ܴ���
uint16 PacketReceiveFlag[1000]={0};
uint8 UpdateData[1024] 	= {0x00};	 /*�����ļ�����*/
static uint16 UpdateStartTime = 0;
static uint16 UpdateEndTime = 0;
uint16 gUpdateBegin = 0;
uint16 SpecialMeter[1024];								//��Ҫ�ϴ����ض�������muxiaoqing
uint8  BinaryDA[32]={0};
uint8  BinaryDT[32]={0};
static uint8 counter=0;

#if 0
const uint8 DADT[32][32]=//muxiaoqing DADT data
{
  
  32*0+1,32*0+2,32*0+3,32*0+4,32*0+5,32*0+6,32*0+7,32*0+8,32*0+9,32*0+10,32*0+11,32*0+12,32*0+13,32*0+14,32*0+15,32*0+16,32*0+17,32*0+18,32*0+19,32*0+20,32*0+21,32*0+22,32*0+23,32*0+24,32*0+25,32*0+26,32*0+27,32*0+28,32*0+29,32*0+30,32*0+31,32*0+32,
  32*1+1,32*1+2,32*1+3,32*1+4,32*1+5,32*1+6,32*1+7,32*1+8,32*1+9,32*1+10,32*1+11,32*1+12,32*1+13,32*1+14,32*1+15,32*1+16,32*1+17,32*1+18,32*1+19,32*1+20,32*1+21,32*1+22,32*1+23,32*1+24,32*1+25,32*1+26,32*1+27,32*1+28,32*1+29,32*1+30,32*1+31,32*1+32,
  32*2+1,32*2+2,32*2+3,32*2+4,32*2+5,32*2+6,32*2+7,32*2+8,32*2+9,32*2+10,32*2+11,32*2+12,32*2+13,32*2+14,32*2+15,32*2+16,32*2+17,32*2+18,32*2+19,32*2+20,32*2+21,32*2+22,32*2+23,32*2+24,32*2+25,32*2+26,32*2+27,32*2+28,32*2+29,32*2+30,32*2+31,32*2+32,
  32*3+1,32*3+2,32*3+3,32*3+4,32*3+5,32*3+6,32*3+7,32*3+8,32*3+9,32*3+10,32*3+11,32*3+12,32*3+13,32*3+14,32*3+15,32*3+16,32*3+17,32*3+18,32*3+19,32*3+20,32*3+21,32*3+22,32*3+23,32*3+24,32*3+25,32*3+26,32*3+27,32*3+28,32*3+29,32*3+30,32*3+31,32*3+32,
	  32*4+1,32*4+2,32*4+3,32*4+4,32*4+5,32*4+6,32*4+7,32*4+8,32*4+9,32*4+10,32*4+11,32*4+12,32*4+13,32*4+14,32*4+15,32*4+16,32*4+17,32*4+18,32*4+19,32*4+20,32*4+21,32*4+22,32*4+23,32*4+24,32*4+25,32*4+26,32*4+27,32*4+28,32*4+29,32*4+30,32*4+31,32*4+32,
	  32*5+1,32*5+2,32*5+3,32*5+4,32*5+5,32*5+6,32*5+7,32*5+8,32*5+9,32*5+10,32*5+11,32*5+12,32*5+13,32*5+14,32*5+15,32*5+16,32*5+17,32*5+18,32*5+19,32*5+20,32*5+21,32*5+22,32*5+23,32*5+24,32*5+25,32*5+26,32*5+27,32*5+28,32*5+29,32*5+30,32*5+31,32*5+32,
	  32*6+1,32*6+2,32*6+3,32*6+4,32*6+5,32*6+6,32*6+7,32*6+8,32*6+9,32*6+10,32*6+11,32*6+12,32*6+13,32*6+14,32*6+15,32*6+16,32*6+17,32*6+18,32*6+19,32*6+20,32*6+21,32*6+22,32*6+23,32*6+24,32*6+25,32*6+26,32*6+27,32*6+28,32*6+29,32*6+30,32*6+31,32*6+32,
	  32*7+1,32*7+2,32*7+3,32*7+4,32*7+5,32*7+6,32*7+7,32*7+8,32*7+9,32*7+10,32*7+11,32*7+12,32*7+13,32*7+14,32*7+15,32*7+16,32*7+17,32*7+18,32*7+19,32*7+20,32*7+21,32*7+22,32*7+23,32*7+24,32*7+25,32*7+26,32*7+27,32*7+28,32*7+29,32*7+30,32*7+31,32*7+32,
	  32*8+1,32*8+2,32*8+3,32*8+4,32*8+5,32*8+6,32*8+7,32*8+8,32*8+9,32*8+10,32*8+11,32*8+12,32*8+13,32*8+14,32*8+15,32*8+16,32*8+17,32*8+18,32*8+19,32*8+20,32*8+21,32*8+22,32*8+23,32*8+24,32*8+25,32*8+26,32*8+27,32*8+28,32*8+29,32*8+30,32*8+31,32*8+32,
	  32*9+1,32*9+2,32*9+3,32*9+4,32*9+5,32*9+6,32*9+7,32*9+8,32*9+9,32*9+10,32*9+11,32*9+12,32*9+13,32*9+14,32*9+15,32*9+16,32*9+17,32*9+18,32*9+19,32*9+20,32*9+21,32*9+22,32*9+23,32*9+24,32*9+25,32*9+26,32*9+27,32*9+28,32*9+29,32*9+30,32*9+31,32*9+32,
	  32*10+1,32*10+2,32*10+3,32*10+4,32*10+5,32*10+6,32*10+7,32*10+8,32*10+9,32*10+10,32*10+11,32*10+12,32*10+13,32*10+14,32*10+15,32*10+16,32*10+17,32*10+18,32*10+19,32*10+20,32*10+21,32*10+22,32*10+23,32*10+24,32*10+25,32*10+26,32*10+27,32*10+28,32*10+29,32*10+30,32*10+31,32*10+32,
	  32*11+1,32*11+2,32*11+3,32*11+4,32*11+5,32*11+6,32*11+7,32*11+8,32*11+9,32*11+10,32*11+11,32*11+12,32*11+13,32*11+14,32*11+15,32*11+16,32*11+17,32*11+18,32*11+19,32*11+20,32*11+21,32*11+22,32*11+23,32*11+24,32*11+25,32*11+26,32*11+27,32*11+28,32*11+29,32*11+30,32*11+31,32*11+32,
	  32*12+1,32*12+2,32*12+3,32*12+4,32*12+5,32*12+6,32*12+7,32*12+8,32*12+9,32*12+10,32*12+11,32*12+12,32*12+13,32*12+14,32*12+15,32*12+16,32*12+17,32*12+18,32*12+19,32*12+20,32*12+21,32*12+22,32*12+23,32*12+24,32*12+25,32*12+26,32*12+27,32*12+28,32*12+29,32*12+30,32*12+31,32*12+32,
	  32*13+1,32*13+2,32*13+3,32*13+4,32*13+5,32*13+6,32*13+7,32*13+8,32*13+9,32*13+10,32*13+11,32*13+12,32*13+13,32*13+14,32*13+15,32*13+16,32*13+17,32*13+18,32*13+19,32*13+20,32*13+21,32*13+22,32*13+23,32*13+24,32*13+25,32*13+26,32*13+27,32*13+28,32*13+29,32*13+30,32*13+31,32*13+32,
	  32*14+1,32*14+2,32*14+3,32*14+4,32*14+5,32*14+6,32*14+7,32*14+8,32*14+9,32*14+10,32*14+11,32*14+12,32*14+13,32*14+14,32*14+15,32*14+16,32*14+17,32*14+18,32*14+19,32*14+20,32*14+21,32*14+22,32*14+23,32*14+24,32*14+25,32*14+26,32*14+27,32*14+28,32*14+29,32*14+30,32*14+31,32*14+32,
	  32*15+1,32*15+2,32*15+3,32*15+4,32*15+5,32*15+6,32*15+7,32*15+8,32*15+9,32*15+10,32*15+11,32*15+12,32*15+13,32*15+14,32*15+15,32*15+16,32*15+17,32*15+18,32*15+19,32*15+20,32*15+21,32*15+22,32*15+23,32*15+24,32*15+25,32*15+26,32*15+27,32*15+28,32*15+29,32*15+30,32*15+31,32*15+32,
	  32*16+1,32*16+2,32*16+3,32*16+4,32*16+5,32*16+6,32*16+7,32*16+8,32*16+9,32*16+10,32*16+11,32*16+12,32*16+13,32*16+14,32*16+15,32*16+16,32*16+17,32*16+18,32*16+19,32*16+20,32*16+21,32*16+22,32*16+23,32*16+24,32*16+25,32*16+26,32*16+27,32*16+28,32*16+29,32*16+30,32*16+31,32*16+32,
	  32*17+1,32*17+2,32*17+3,32*17+4,32*17+5,32*17+6,32*17+7,32*17+8,32*17+9,32*17+10,32*17+11,32*17+12,32*17+13,32*17+14,32*17+15,32*17+16,32*17+17,32*17+18,32*17+19,32*17+20,32*17+21,32*17+22,32*17+23,32*17+24,32*17+25,32*17+26,32*17+27,32*17+28,32*17+29,32*17+30,32*17+31,32*17+32,
	  32*18+1,32*18+2,32*18+3,32*18+4,32*18+5,32*18+6,32*18+7,32*18+8,32*18+9,32*18+10,32*18+11,32*18+12,32*18+13,32*18+14,32*18+15,32*18+16,32*18+17,32*18+18,32*18+19,32*18+20,32*18+21,32*18+22,32*18+23,32*18+24,32*18+25,32*18+26,32*18+27,32*18+28,32*18+29,32*18+30,32*18+31,32*18+32,
	  32*19+1,32*19+2,32*19+3,32*19+4,32*19+5,32*19+6,32*19+7,32*19+8,32*19+9,32*19+10,32*19+11,32*19+12,32*19+13,32*19+14,32*19+15,32*19+16,32*19+17,32*19+18,32*19+19,32*19+20,32*19+21,32*19+22,32*19+23,32*19+24,32*19+25,32*19+26,32*19+27,32*19+28,32*19+29,32*19+30,32*19+31,32*19+32,
	  32*20+1,32*20+2,32*20+3,32*20+4,32*20+5,32*20+6,32*20+7,32*20+8,32*20+9,32*20+10,32*20+11,32*20+12,32*20+13,32*20+14,32*20+15,32*20+16,32*20+17,32*20+18,32*20+19,32*20+20,32*20+21,32*20+22,32*20+23,32*20+24,32*20+25,32*20+26,32*20+27,32*20+28,32*20+29,32*20+30,32*20+31,32*20+32,
	  32*21+1,32*21+2,32*21+3,32*21+4,32*21+5,32*21+6,32*21+7,32*21+8,32*21+9,32*21+10,32*21+11,32*21+12,32*21+13,32*21+14,32*21+15,32*21+16,32*21+17,32*21+18,32*21+19,32*21+20,32*21+21,32*21+22,32*21+23,32*21+24,32*21+25,32*21+26,32*21+27,32*21+28,32*21+29,32*21+30,32*21+31,32*21+32,
	  32*22+1,32*22+2,32*22+3,32*22+4,32*22+5,32*22+6,32*22+7,32*22+8,32*22+9,32*22+10,32*22+11,32*22+12,32*22+13,32*22+14,32*22+15,32*22+16,32*22+17,32*22+18,32*22+19,32*22+20,32*22+21,32*22+22,32*22+23,32*22+24,32*22+25,32*22+26,32*22+27,32*22+28,32*22+29,32*22+30,32*22+31,32*22+32,
	  32*23+1,32*23+2,32*23+3,32*23+4,32*23+5,32*23+6,32*23+7,32*23+8,32*23+9,32*23+10,32*23+11,32*23+12,32*23+13,32*23+14,32*23+15,32*23+16,32*23+17,32*23+18,32*23+19,32*23+20,32*23+21,32*23+22,32*23+23,32*23+24,32*23+25,32*23+26,32*23+27,32*23+28,32*23+29,32*23+30,32*23+31,32*23+32,
	  32*24+1,32*24+2,32*24+3,32*24+4,32*24+5,32*24+6,32*24+7,32*24+8,32*24+9,32*24+10,32*24+11,32*24+12,32*24+13,32*24+14,32*24+15,32*24+16,32*24+17,32*24+18,32*24+19,32*24+20,32*24+21,32*24+22,32*24+23,32*24+24,32*24+25,32*24+26,32*24+27,32*24+28,32*24+29,32*24+30,32*24+31,32*24+32,
	  32*25+1,32*25+2,32*25+3,32*25+4,32*25+5,32*25+6,32*25+7,32*25+8,32*25+9,32*25+10,32*25+11,32*25+12,32*25+13,32*25+14,32*25+15,32*25+16,32*25+17,32*25+18,32*25+19,32*25+20,32*25+21,32*25+22,32*25+23,32*25+24,32*25+25,32*25+26,32*25+27,32*25+28,32*25+29,32*25+30,32*25+31,32*25+32,
	  32*26+1,32*26+2,32*26+3,32*26+4,32*26+5,32*26+6,32*26+7,32*26+8,32*26+9,32*26+10,32*26+11,32*26+12,32*26+13,32*26+14,32*26+15,32*26+16,32*26+17,32*26+18,32*26+19,32*26+20,32*26+21,32*26+22,32*26+23,32*26+24,32*26+25,32*26+26,32*26+27,32*26+28,32*26+29,32*26+30,32*26+31,32*26+32,
	  32*27+1,32*27+2,32*27+3,32*27+4,32*27+5,32*27+6,32*27+7,32*27+8,32*27+9,32*27+10,32*27+11,32*27+12,32*27+13,32*27+14,32*27+15,32*27+16,32*27+17,32*27+18,32*27+19,32*27+20,32*27+21,32*27+22,32*27+23,32*27+24,32*27+25,32*27+26,32*27+27,32*27+28,32*27+29,32*27+30,32*27+31,32*27+32,
	  32*28+1,32*28+2,32*28+3,32*28+4,32*28+5,32*28+6,32*28+7,32*28+8,32*28+9,32*28+10,32*28+11,32*28+12,32*28+13,32*28+14,32*28+15,32*28+16,32*28+17,32*28+18,32*28+19,32*28+20,32*28+21,32*28+22,32*28+23,32*28+24,32*28+25,32*28+26,32*28+27,32*28+28,32*28+29,32*28+30,32*28+31,32*28+32,
	  32*29+1,32*29+2,32*29+3,32*29+4,32*29+5,32*29+6,32*29+7,32*29+8,32*29+9,32*29+10,32*29+11,32*29+12,32*29+13,32*29+14,32*29+15,32*29+16,32*29+17,32*29+18,32*29+19,32*29+20,32*29+21,32*29+22,32*29+23,32*29+24,32*29+25,32*29+26,32*29+27,32*29+28,32*29+29,32*29+30,32*29+31,32*29+32,
	  32*30+1,32*30+2,32*30+3,32*30+4,32*30+5,32*30+6,32*30+7,32*30+8,32*30+9,32*30+10,32*30+11,32*30+12,32*30+13,32*30+14,32*30+15,32*30+16,32*30+17,32*30+18,32*30+19,32*30+20,32*30+21,32*30+22,32*30+23,32*30+24,32*30+25,32*30+26,32*30+27,32*30+28,32*30+29,32*30+30,32*30+31,32*30+32,
	  32*31+1,32*31+2,32*31+3,32*31+4,32*31+5,32*31+6,32*31+7,32*31+8,32*31+9,32*31+10,32*31+11,32*31+12,32*31+13,32*31+14,32*31+15,32*31+16,32*31+17,32*31+18,32*31+19,32*31+20,32*31+21,32*31+22,32*31+23,32*31+24,32*31+25,32*31+26,32*31+27,32*31+28,32*31+29,32*31+30,32*31+31,32*31+32,

};
#endif
/*end:yangfei added */
/**************************/
/********************************************** static *********************************************/
//����������������ã��ȴ洢��SD���У� �ٸ���ȫ�ֱ���

extern uint16 gPARA_MeterNum;										//��ǰ����������
extern MeterFileType gPARA_MeterFiles[METER_NUM_MAX];				//�ڴ��д洢��ǰ������Ϣ
extern uint8 gPARA_SendNum_Prevous;									//������һ�����͵ı�������
extern uint8 gPARA_SendIndex_Prevous;								//������һ�����ݰ������к�

extern uint16 gPARA_TimeNodes[TIME_NODE_NUM_MAX];					//��ǰ����ʱ��㣬���ڴ��еı���
extern TermParaSaveType gPARA_TermPara;

extern uint8 gRestartFlag ;
/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_02				    							                       
**	�� ��  �� ��: ��վ���ü�����                                                                         			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_02(_ProtocolType &_ProData)
{
	uint8 Err 			= 0;
	uint8 *pTemp 		= _ProData.Data.pData;
	TermParaSaveType TermPara;
	CPU_SR		cpu_sr;
	
	TermPara.WriteFlag = 0xAA55;
	
	memcpy(TermPara.TermAddr, pTemp, 6);
	pTemp += 6;
	
	memcpy(TermPara.HostAddr, pTemp, 6);
	pTemp += 6;
	
	memcpy((uint8 *)&(TermPara.DebugPara.FrameLen_Max), pTemp, 2);
	pTemp += 2;
	
	TermPara.DebugPara.FrameToFrameTime = *pTemp++;
	
	OSMutexPend (FlashMutex,0,&Err);
	/*begin:yangfei changed 2012-12-15 for ���ַ��Ϣ�洢*/
	#if 0
    Err =AppSdWrRdBytes(TERMINAL_PARA, sizeof(TermParaSaveType), (uint8 *)&TermPara.WriteFlag, SD_WR);
	#endif
	Err = SDSaveData("/TERMINAL_PARA", &TermPara, sizeof(TermParaSaveType), 0);
	/*end   :yangfei changed 2012-12-15 for ���ַ��Ϣ�洢*/
    OSMutexPost (FlashMutex);
	
	OS_ENTER_CRITICAL();											//���µ�ǰ�ڴ��ַ����
	gPARA_TermPara = TermPara;
	OS_EXIT_CRITICAL();
	
	//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= 0x01;
	_ProData.MsgType		= 0x03;
	ReadDateTime(_ProData.TimeSmybol);
	
	if(Err == NO_ERR)
		{
			*_ProData.Data.pDataBack= 0x01;					//��ȷ��Ӧ
		}
	else
		{
			*_ProData.Data.pDataBack= 0x10;					//�쳣��Ӧ
		}
		
	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_0C				    							                       
**	�� ��  �� ��: ��վ�·������Ǳ��ַ                                                       			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_0C(_ProtocolType &_ProData)
{
	uint8 i;
	uint8 Err 			= 0;
	uint8 *pTemp 		= _ProData.Data.pData;
	uint8 *pTempBack	= _ProData.Data.pDataBack;
	uint8 SendNum		= 0;
	uint8 SendIndex		= 0;
	uint8 mfChannel		= 0;
	uint8 NewDataFlag	= 0;
	uint16 ChannelTemp  = 0;
	uint16 MeterSn		= 0;
	uint16 MeterChannelSn[7]	= {0x00};
	uint16 PanelChannelSn[7]	= {0x00};
	uint16 ValveChannelSn[7]	= {0x00};
	//uint8 AddrTemp[7]	= {0x00};									//���ڵ���Դ��ַ �� Ŀ���ַ
	MeterFileType	*pMeterFile = NULL;
	MeterFilePara	para;
	CPU_SR			cpu_sr;
	/*begin:yangfei modified 2012-12-24 find path*/
	char  NodePath[] = "/2012/12/24/timenode";
	uint8 SystemTime[6] = {0};
	/*end   :yangfei modified 2012-12-24*/

		
	pTemp++;														//������������
	SendIndex			= *pTemp++;									//��վ���ݰ��������
	SendNum 		    = *pTemp++;									//���η��͵ı�������
	
	pMeterFile			= new MeterFileType[SendNum];
	if(pMeterFile == NULL)		while(1)		;					//�ڴ�ռ��������

	if(SendIndex == FIRST_PACK)							//��վ�·�������Ϣ�����ǵ�һ������
		{
			OS_ENTER_CRITICAL();
			gPARA_MeterNum = 0x00;
			gPARA_SendNum_Prevous = 0x00;
			gPARA_SendIndex_Prevous = 0x00;
			memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
			memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
			memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
			memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
			memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
			memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
			memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
			OS_EXIT_CRITICAL();

			NewDataFlag		= 1;									//���µ����ݰ�
		}
	else
	{
		if(SendIndex > gPARA_SendIndex_Prevous)
		{
			NewDataFlag		= 1;									//���µ����ݰ�
		}
		else if(SendIndex == gPARA_SendIndex_Prevous)
			{
				NewDataFlag		= 1;								//���µ����ݰ�
				OS_ENTER_CRITICAL();	
				gPARA_MeterNum -= gPARA_SendNum_Prevous; 			//����һ���ط���ʼ���´洢
				OS_EXIT_CRITICAL();
			}
			else
			{
				NewDataFlag		= 0;								//�����µ����ݰ�
			}	
	}
	
	OS_ENTER_CRITICAL();	
	MeterSn = gPARA_MeterNum; 						//ȷ���洢λ��
	OS_EXIT_CRITICAL();

	if(NewDataFlag == 1)
		{	
 			for(i=0; i<SendNum; i++)
			{
				memcpy((uint8 *)&(pMeterFile[i].MeterID), pTemp, sizeof(MeterFileType));
				pTemp += sizeof(MeterFileType);
			}
			/*begin:yangfei changed 2012-12-15 for ���ַ��Ϣ�洢*/
			/* ����Ϣ����MeterSn ���δ�����ļ�meter ��*/
			Err = SDSaveData("/METER_FILE_ADDR", pMeterFile, sizeof(*pMeterFile)*SendNum, sizeof(*pMeterFile)*MeterSn);		
			/*end:yangfei changed 2012-12-15 for ���ַ��Ϣ�洢*/
			OSSchedLock ();				//���µ�ǰ�����ڴ汸�� -- ֹͣ�������
			memcpy((uint8 *)&(gPARA_MeterFiles[gPARA_MeterNum].MeterID), (uint8 *)&(pMeterFile->MeterID), SendNum*sizeof(MeterFileType));
			gPARA_MeterNum += SendNum;
    		
			memcpy((uint8 *)MeterChannelSn, (uint8 *)gPARA_MeterChannelNum, sizeof(gPARA_MeterChannelNum));
			memcpy((uint8 *)PanelChannelSn, (uint8 *)gPARA_ConPanelChannelNum, sizeof(gPARA_ConPanelChannelNum));
			memcpy((uint8 *)ValveChannelSn, (uint8 *)gPARA_ValveConChannelNum, sizeof(gPARA_ValveConChannelNum));
			
			
			for(i=0; i<SendNum; i++)
			{
				mfChannel =  (pMeterFile[i].ChannelIndex) - 1;
				if(mfChannel > (METER_CHANNEL_NUM - 1))  //��ֹ��������Ϣ���󣬵������渳ֵʱ�������鷶Χ��
						mfChannel = METER_CHANNEL_NUM - 1;
				
				//�����ȼ�����ͨ��
				ChannelTemp = MeterChannelSn[mfChannel]++;
				if(ChannelTemp >= METER_PER_CHANNEL_NUM)  //��ֹ���鳬�ޡ�
						ChannelTemp = METER_PER_CHANNEL_NUM - 1;
										
				gPARA_MeterChannel[mfChannel][ChannelTemp] = MeterSn;
				
				//�����¿����ͨ��
				if(MeterNoBcdCheck(pMeterFile->ControlPanelAddr))
					{
						ChannelTemp = PanelChannelSn[mfChannel]++;
						if(ChannelTemp >= METER_PER_CHANNEL_NUM)  //��ֹ���鳬�ޡ�
								ChannelTemp = METER_PER_CHANNEL_NUM - 1;
										
						gPARA_ConPanelChannel[mfChannel][ChannelTemp] = MeterSn;
					}
				
				//���䷧��ͨ��
				if(MeterNoBcdCheck(pMeterFile->ValveAddr))
					{
						ChannelTemp = ValveChannelSn[mfChannel]++;
						if(ChannelTemp >= METER_PER_CHANNEL_NUM)  //��ֹ���鳬�ޡ�
								ChannelTemp = METER_PER_CHANNEL_NUM - 1;
						
						gPARA_ValveConChannel[mfChannel][ChannelTemp] = MeterSn;
					}
					
				MeterSn++;
			}
			
			memcpy((uint8 *)gPARA_MeterChannelNum, (uint8 *)MeterChannelSn, 7*sizeof(uint16));
			memcpy((uint8 *)gPARA_ConPanelChannelNum, (uint8 *)PanelChannelSn, 7*sizeof(uint16));
			memcpy((uint8 *)gPARA_ValveConChannelNum, (uint8 *)ValveChannelSn, 7*sizeof(uint16));
			OSSchedUnlock ();
			
			para.WriteFlag = 0xAA55;

			OS_ENTER_CRITICAL();	
			para.MeterStoreNums = gPARA_MeterNum;
			/*begin:yangfei changed 2012-12-15 */
			memcpy(SystemTime, gSystemTime, 6);
			/*end   :yangfei changed 2012-12-15 */
			OS_EXIT_CRITICAL();
			 
			//�洢������Ϣ����
			OSMutexPend (FlashMutex,0,&Err);
			/*begin:yangfei changed 2012-12-15 for �����洢*/
			Err = SDSaveData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
			GetFilePath(NodePath, SystemTime, ARRAY_DAY);
			Err = SDSaveData(NodePath, &gPARA_MeterNum, sizeof(gPARA_MeterNum), 6);
			/*end:yangfei changed 2012-12-15 for �����洢*/
    		OSMutexPost (FlashMutex);
			if(Err==TRUE)		Err = NO_ERR;
			
				
			gPARA_SendIndex_Prevous 	= SendIndex;
			gPARA_SendNum_Prevous 	= SendNum;
		}
	delete	pMeterFile;
	
	//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength	= 0x02;
	_ProData.MsgType		= 0x0D;
	ReadDateTime(_ProData.TimeSmybol);
	
	if(Err == NO_ERR)
		{
			*pTempBack++ = 0x01;					//��ȷ��Ӧ
		}
	else
		{
			*pTempBack++ = 0x10;					//�쳣��Ӧ
		}
	*pTempBack++ = SendIndex;
		
	return Err;
}


/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_13				    							                       
**	�� ��  �� ��: ��վ�·������Ǳ��ַ                                                       			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	��λ�����͵����Ǳ��ַ��ͨ��MeterID��ѯ�滻ԭ������Ϣ��
					�����ѯ������MeterID���򷵻�0x11��ָʾ��ѯ����MeterID��
					
*********************************************************************************************************/
uint8 PARA_ProcessMsg_13(_ProtocolType &_ProData)
{
	uint8 i;
	uint8 Err 			= 0;
	uint8 *pTemp 		= _ProData.Data.pData;
	uint8 *pTempBack	= _ProData.Data.pDataBack;
	uint8 mfChannel		= 0;
	uint16 ChannelTemp  = 0;
	uint16 MeterChannelSn[METER_CHANNEL_NUM]	= {0x00};
	uint16 PanelChannelSn[METER_CHANNEL_NUM]	= {0x00};
	uint16 ValveChannelSn[METER_CHANNEL_NUM]	= {0x00};
	uint16 lu16MeterID = 0;
	uint16 lu16MeterSn = 0;
	MeterFileType	mf;
	
//	CPU_SR			cpu_sr;


	lu16MeterID = *((uint16 *)pTemp);  //�õ�MeterID.
	

	//����lu16MeterID ��ѯ����Ϣ��
	Err = PARA_GetMeterSn_ByMeterID(&lu16MeterID,&lu16MeterSn);

	if(Err == NO_ERR){
		memcpy(&mf,pTemp,sizeof(mf)); 
		gPARA_MeterFiles[lu16MeterSn] = mf;

		OSMutexPend (FlashMutex,0,&Err);
		Err = SDSaveData("/METER_FILE_ADDR", &mf, sizeof(mf), sizeof(mf)*lu16MeterSn); 	
    OSMutexPost (FlashMutex);

		if(Err == NO_ERR){

			OSSchedLock ();	//��ֹͨ���б仯�����µ�ǰ�����ڴ汸�� -- ֹͣ�������
			memset((uint8 *)MeterChannelSn, 0, sizeof(MeterChannelSn));
			memset((uint8 *)PanelChannelSn, 0, sizeof(PanelChannelSn));
			memset((uint8 *)ValveChannelSn, 0, sizeof(ValveChannelSn));
			memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
			memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
			memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
			memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
			memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
			memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
			
			
			for(i=0; i<gPARA_MeterNum; i++){
				mfChannel =  (gPARA_MeterFiles[i].ChannelIndex) - 1;
				//�����ȼ�����ͨ��
				ChannelTemp = MeterChannelSn[mfChannel]++;
				gPARA_MeterChannel[mfChannel][ChannelTemp] = i;
				
				//�����¿����ͨ��
				if(MeterNoBcdCheck(gPARA_MeterFiles[i].ControlPanelAddr)){
					ChannelTemp = PanelChannelSn[mfChannel]++;
					gPARA_ConPanelChannel[mfChannel][ChannelTemp] = i;
				}
				
				//���䷧��ͨ��
				if(MeterNoBcdCheck(gPARA_MeterFiles[i].ValveAddr)){
					ChannelTemp = ValveChannelSn[mfChannel]++;
					gPARA_ValveConChannel[mfChannel][ChannelTemp] = i;
				}
					
			}
			
			memcpy((uint8 *)gPARA_MeterChannelNum, (uint8 *)MeterChannelSn, 7*sizeof(uint16));
			memcpy((uint8 *)gPARA_ConPanelChannelNum, (uint8 *)PanelChannelSn, 7*sizeof(uint16));
			memcpy((uint8 *)gPARA_ValveConChannelNum, (uint8 *)ValveChannelSn, 7*sizeof(uint16));
			OSSchedUnlock ();

		}

		_ProData.MsgLength	= 0x02;
		_ProData.MsgType		= 0x14;
		ReadDateTime(_ProData.TimeSmybol);

		if(Err == NO_ERR){
			*pTempBack++ = 0x01;    //��ȷ��Ӧ
			*pTempBack++ = 0x00;
		}
		else{
			*pTempBack++ = 0x10;    //����
			*pTempBack++ = 0x00;
		}			
			

		return Err;

		
		

	}
	else{
		//�Ҳ���Ҫ�滻�ļ�������Ϣ��
		_ProData.MsgLength	= 0x02;
		_ProData.MsgType		= 0x14;
		ReadDateTime(_ProData.TimeSmybol);
		*pTempBack++ = 0x11;
		*pTempBack++ = 0x00;
		

		return Err;
	
	}



}


/********************************************************************************************************
**  �� ��  �� ��: PARA_GetMeterSn				    							                       
**	�� ��  �� ��: ��ȡ���������Ų���                                                                         			
**	�� ��  �� ��: uint8 *pMeterAddr...������ĵ�ַ
**                uint16 *pMeterSn...����������							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: ������Ӧ����δ���ҵ����󷵻ر�־	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_GetMeterSn(uint8 *pMeterAddr, uint16 *pMeterSn)
{
	int8   Res		= 0x00;
	uint16 i 		= 0x00;
	uint16 MeterNum = 0x00;
	uint8 ReadMeterAddr[7] = {0x00};
	uint8 MeterAddrStore[7] = {0x00};
	CPU_SR			cpu_sr;
	
	OS_ENTER_CRITICAL();
	MeterNum = gPARA_MeterNum;
	OS_EXIT_CRITICAL();

	LOG_assert_param( pMeterAddr == NULL );
	LOG_assert_param( pMeterSn == NULL );
	
	memcpy(ReadMeterAddr, pMeterAddr, 7);
	for(i=0; i<MeterNum; i++)
	{
		OS_ENTER_CRITICAL();
		memcpy(MeterAddrStore, gPARA_MeterFiles[i].MeterAddr, 7);
		OS_EXIT_CRITICAL();

		Res = CmpNByte(ReadMeterAddr, MeterAddrStore, 7);
		if(Res == TRUE)							//2�߱Ƚ�һֱ
			{
				*pMeterSn = i;
				return NO_ERR;
			}
	}
	
	return 1;
}


/********************************************************************************************************
**  �� ��  �� ��: PARA_GetMeterSn_ByMeterID				    							                       
**	�� ��  �� ��: ���ݼ�����(MeterID)��ȡ���������Ų���                                                                         			
**	�� ��  �� ��:uint16 *pMeterID...������ļ�����
**                uint16 *pMeterSn...����������							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: ������Ӧ����δ���ҵ����󷵻ر�־	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_GetMeterSn_ByMeterID(uint16 *pMeterID, uint16 *pMeterSn)
{
//	int8   Res		= 0x00;
	uint16 i 		= 0x00;
	uint16 MeterNum = 0x00;
	uint16 lu16MeterID = 0;
	uint16 lu16MeterAddrStore = 0;
	CPU_SR			cpu_sr;
	
	OS_ENTER_CRITICAL();
	MeterNum = gPARA_MeterNum;
	OS_EXIT_CRITICAL();

	LOG_assert_param( pMeterID == NULL );
	LOG_assert_param( pMeterSn == NULL );
	
	lu16MeterID = *pMeterID;
	for(i=0; i<MeterNum; i++)
	{
		OS_ENTER_CRITICAL();
		lu16MeterAddrStore = gPARA_MeterFiles[i].MeterID;
		OS_EXIT_CRITICAL();

		if(lu16MeterAddrStore == lu16MeterID)							//2�߱Ƚ�һ�¡�
			{
				*pMeterSn = i;
				return NO_ERR;
			}
	}
	
	return 1;
}

/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_20				    							                       
**	�� ��  �� ��: ��վ���ó���ʱ��                                                                      			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_20(_ProtocolType &_ProData)
{
	uint8 i,j;
	uint8 Err 			= 0;
	uint8 SetNums		= 0;
	uint8 *pTemp 		= _ProData.Data.pData;
	//uint8 ReadTime[6]   = {0x00};
	uint16 TimeNodeArry[TIME_NODE_NUM_MAX+1] = {0x00};	//��һ������Ԫ�أ������洢��SD����ʾ�ò�����д���
	uint16 lu16TimeNodesNum = 0;						//��ű�������ʱ��������
	uint16 lu16TempArray[TIME_NODE_NUM_MAX] = {0};
	uint16 lu16Temp = 0;
	//uint32 LogicAddr	= 0x00;
	//DataStoreParaType  DataPara;
	CPU_SR		cpu_sr;
	/*begin:yangfei modified 2012-12-24 find path*/
	char  NodePath[] = "/2012/12/24/timenode";
	uint8 SystemTime[6] = {0};
	/*end   :yangfei modified 2012-12-24*/
	
	memset((uint8 *)TimeNodeArry, 0xFF, sizeof(TimeNodeArry));
	SetNums	= *pTemp++;								//�������õ�ʱ�������
	
	//�˴���ý�ʱ��ڵ㰴�����������Ա����ȷ��λ��
	//����ʱ��ĸ�ʽ ��+ʱ ���ڵ��ֽڣ� ���ø��Ƹ�16bit����֮�󷽱����Ƚϵ��ã��ʶ�������ڽṹ����

	memcpy((uint8 *)&TimeNodeArry[1], pTemp, SetNums*sizeof(uint16));
	TimeNodeArry[0] = 0xAA55;
	lu16TimeNodesNum = SetNums;

	memcpy((uint8*)lu16TempArray,pTemp,SetNums*sizeof(uint16)); //׼�����ݣ�ð�ݷ���С�������С�

	for(i=0;i<lu16TimeNodesNum-1;i++)
		{
			for(j=0;j<lu16TimeNodesNum-i-1;j++)
			{
				if(lu16TempArray[j+1]<lu16TempArray[j])
				{
				  lu16Temp = lu16TempArray[j+1];
				  lu16TempArray[j+1] = lu16TempArray[j];
				  lu16TempArray[j] = lu16Temp;
				 }
			}
		}

	memcpy((uint8 *)&TimeNodeArry[1],(uint8*)lu16TempArray,SetNums*sizeof(uint16));
	
	//�洢��SD����
	OSMutexPend (FlashMutex,0,&Err);
	/*begin:yangfei changed 2012-12-15 for��վ���ö�ʱ����ʱ��*/
	#if 0
        Err =AppSdWrRdBytes(TIME_NODE_ADDR, (TIME_NODE_NUM_MAX+1)*sizeof(uint16), (uint8 *)TimeNodeArry, SD_WR);
	#else
	Err = SDSaveData("/TIME_NODE_ADDR",TimeNodeArry, (TIME_NODE_NUM_MAX+1)*sizeof(uint16), 0);
	Err = SDSaveData("/TIME_NODE_ADDR",&lu16TimeNodesNum, sizeof(uint16), (TIME_NODE_NUM_MAX+1)*sizeof(uint16));  //�ڵ������洢������档
	#endif
	/*end:yangfei changed 2012-12-15 for��վ���ö�ʱ����ʱ��*/
        OSMutexPost (FlashMutex);
	
	//���¾�̬ȫ�ֱ������Ա��ڴ���ٲ���
	OS_ENTER_CRITICAL();
	memcpy((uint8 *)gPARA_TimeNodes, (uint8 *)&TimeNodeArry[1], (TIME_NODE_NUM_MAX)*sizeof(uint16));
	gPARA_TimeNodesNum = lu16TimeNodesNum;
	memcpy(SystemTime, gSystemTime, 6);
	OS_EXIT_CRITICAL();
	/*begin:yangfei changed 2012-12-28 for save timenode to current daily timenode path*/
	//���洢�е� ���� ��һ��ʱ��ڵ�Ĳ�����
	//�˴����޸�Ϊ �洢�ڵ�һ��ʱ��ڵ㴦 --- ��δ������
	#if 0
	ReadDateTime(ReadTime);
	Err = STORE_FindDayStartAddr(ReadTime, &LogicAddr);
	if(Err != NO_ERR)			;
		
	OSMutexPend (FlashMutex, 0, &Err);
    Err =AppSdWrRdBytes(LogicAddr, sizeof(DataStoreParaType), (uint8 *)&DataPara, SD_RD);
    memcpy((uint8 *)DataPara.TimeNode, (uint8 *)&TimeNodeArry[1], TIME_NODE_NUM_MAX*sizeof(uint16));
    Err =AppSdWrRdBytes(LogicAddr, sizeof(DataStoreParaType), (uint8 *)&DataPara, SD_WR);
    OSMutexPost (FlashMutex);
	if(Err != NO_ERR)		Err = NO_ERR;	
	#endif
	GetFilePath(NodePath, SystemTime, ARRAY_DAY);
	OSMutexPend (FlashMutex, 0, &Err);
	Err = SDSaveData(NodePath, &TimeNodeArry[1], (TIME_NODE_NUM_MAX)*sizeof(uint16),8);
	Err = SDSaveData(NodePath, &lu16TimeNodesNum, sizeof(lu16TimeNodesNum),8+2*TIME_NODE_NUM_MAX); 
	OSMutexPost (FlashMutex);
	if(Err!=NO_ERR)
    	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
    	}
	/*end:yangfei changed 2012-12-28*/
	//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= 0x01;
	_ProData.MsgType		= 0x21;
	ReadDateTime(_ProData.TimeSmybol);
	
	if(Err == NO_ERR)
		{
			*_ProData.Data.pDataBack= 0x01;					//��ȷ��Ӧ
		}
	else
		{
			*_ProData.Data.pDataBack= 0x10;					//�쳣��Ӧ
		}
	return 0;
}

/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_24				    							                       
**	�� ��  �� ��: ������Ϣ�Ĳ���                                                                         			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_24(_ProtocolType &_ProData)
{
	uint8 Err=0;
	uint8 *pTemp;
	
	pTemp = _ProData.Data.pData;
	
	UserSetDateTime(pTemp);
	TIME_UpdateSystemTime();  					//����ϵͳʱ��
	
	_ProData.MsgLength		= 1;
	_ProData.MsgType		= 0x25;
	Err = ReadDateTime(_ProData.TimeSmybol);
	
	if(Err != NO_ERR)
		{
			*_ProData.Data.pDataBack= 0x10;					//�쳣��Ӧ
			Err = 0;
		}
	else
		{
			*_ProData.Data.pDataBack= 0x01;					//������Ӧ
		}
	
	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_26				    							                       
**	�� ��  �� ��: ��ȡ������ʱ��ʱ��                                                                      			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_26(_ProtocolType &_ProData)
{
	uint8 Err=0;
	uint8 *pTemp;
	
	pTemp = _ProData.Data.pDataBack;
	
	pTemp++;
	Err = ReadDateTime(pTemp);
	
	_ProData.MsgLength		= 7;
	_ProData.MsgType		= 0x27;
	ReadDateTime(_ProData.TimeSmybol);
	
	if(Err != NO_ERR)
		{
			*_ProData.Data.pDataBack= 0x10;					//�쳣��Ӧ
			Err = 0;
		}
	else
		{
			*_ProData.Data.pDataBack= 0x01;					//������Ӧ
		}
	
	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_28				    							                       
**	�� ��  �� ��: ��վͨѶ��������                                                                         			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_28(_ProtocolType &_ProData)
{
	uint8 Err	 = 0x00;
	uint8 *pTemp = _ProData.Data.pData;
	UpCommParaSaveType	UpPara;
	CPU_SR cpu_sr;
	
	UpPara.WriteFlag 	= 0xAA55;
	memcpy(UpPara.HostIPAddr, pTemp, 4);
	pTemp += 4;
	
	UpPara.HostPort		= (*pTemp++) + ((*pTemp++)<<8);
	
	UpPara.HeartCycle	= *pTemp++;
	UpPara.RelandDelay  = *pTemp++;
	UpPara.RelandTimes  = *pTemp;
	
	OS_ENTER_CRITICAL();
	gPARA_UpPara = UpPara;
	OS_EXIT_CRITICAL();
	
	OSMutexPend (FlashMutex, 0, &Err);
	#if 0
    Err =AppSdWrRdBytes(UP_COMM_PARA, sizeof(UpCommParaSaveType), (uint8 *)&UpPara, SD_WR);
	#else
	Err = SDSaveData("/UP_COMM_PARA", (uint8 *)&UpPara, sizeof(UpCommParaSaveType), 0);
	#endif
    OSMutexPost (FlashMutex);
	if(Err != NO_ERR)		Err = NO_ERR;
		
	_ProData.MsgLength		= 1;
	_ProData.MsgType		= 0x29;
	ReadDateTime(_ProData.TimeSmybol);
	
	if(Err != NO_ERR)
		{
			*_ProData.Data.pDataBack= 0x10;					//�쳣��Ӧ
			Err = 0;
		}
	else
		{
			*_ProData.Data.pDataBack= 0x01;					//������Ӧ
		}
	
	return Err;
}





/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_34				    							                       
**	�� ��  �� ��: ��վ�·�����������ָ��                                                  			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	
					
*********************************************************************************************************/
uint8 PARA_ProcessMsg_34(_ProtocolType &_ProData)
{
	uint8 Err 			= 0;
	uint8 *pTemp 		= _ProData.Data.pData;
	uint8 *pTempBack	= _ProData.Data.pDataBack;


	gRestartFlag = TRUE;    /*delay 5s reset*/


	_ProData.MsgLength	= 0x01;
	_ProData.MsgType		= 0x35;
	ReadDateTime(_ProData.TimeSmybol);
	*pTempBack++ = 0x01;//�ɹ��յ�����ָ�ֻ���سɹ���5S��������
			
	return Err;


}



/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_3A				    							                       
**	�� ��  �� ��: ��վ�·����������                                                  			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	���ò��������(��������������ʱ����)��
					
*********************************************************************************************************/
uint8 PARA_ProcessMsg_3A(_ProtocolType &_ProData)
{
	uint8 Err 			= 0;
	uint8 *pTemp 		= _ProData.Data.pData;
	uint8 *pTempBack	= _ProData.Data.pDataBack;

	ReplenishReadMeterSaveType ReplenishPara;


	ReplenishPara.MeterReplenishTimes = *pTemp;
	pTemp += 1;
	ReplenishPara.MeterInterval = *((uint16 *)pTemp);
	pTemp += 2;
	ReplenishPara.ValveReplenishTimes = *pTemp;
	pTemp += 1;
	ReplenishPara.ValveInterval = *((uint16 *)pTemp);
	pTemp += 2;

	//��Χ�޶������顣
		if(ReplenishPara.MeterReplenishTimes > 9)
			Err = 1;
		if((ReplenishPara.MeterInterval>9999) || (ReplenishPara.MeterInterval<100))
			Err = 1;
		if(ReplenishPara.ValveReplenishTimes > 9)
			Err = 1;
		if((ReplenishPara.ValveInterval>9999) || (ReplenishPara.ValveInterval<100))
			Err = 1;

		if(Err == NO_ERR){
			ReplenishPara.WriteFlag = 0xaa55;
			gPARA_ReplenishPara = ReplenishPara;  //��ֵ��ȫ�ֱ�����
			//�洢��SD���С�
			OSMutexPend (FlashMutex,0,&Err);
			Err = SDSaveData("/REPLENISH_PARA", &ReplenishPara, sizeof(ReplenishPara), 0);
			OSMutexPost (FlashMutex);

			
				_ProData.MsgLength	= 0x01;
				_ProData.MsgType		= 0x3B;
				ReadDateTime(_ProData.TimeSmybol);

			if(Err == NO_ERR)
				*pTempBack++ = 0x01;//�������ء�
			else
				*pTempBack++ = 0x10;//�쳣���ء�
				
				return Err;
			

		}
		else{  
			_ProData.MsgLength	= 0x01;
			_ProData.MsgType		= 0x3B;
			ReadDateTime(_ProData.TimeSmybol);
			*pTempBack++ = 0x13;//������������Χ��
			
			return Err;

		}



}


/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_82				    							                       
**	�� ��  �� ��: ��վ�·�Զ��������ʼ��־                                                             			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none	
** author:yangfei
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_82(_ProtocolType &_ProData)
{
	uint8 Err=0;
	uint8 *pTemp = _ProData.Data.pData;

	uint16 lu16TotalPacket = 0;
	uint32 lu32TotalSize= 0;
	uint16 lu16Version = 0;
	CPU_SR		cpu_sr;
	char* Path="/update";
	MakeDir(Path);
	
	uint8 SystemTime[6] = {0};
	OS_ENTER_CRITICAL();
	memcpy(SystemTime, gSystemTime, 6);
	
	OS_EXIT_CRITICAL();
	UpdateStartTime = SystemTime[ARRAY_HOUR]*3600+SystemTime[ARRAY_MINUTE]*60+SystemTime[ARRAY_SECOND];
	
	lu16TotalPacket = *(uint16*)pTemp;
	pTemp+=2;
	gu16MaxPackSize = *(uint16*)pTemp;
	pTemp+=2;
	lu32TotalSize = *(uint32*)pTemp;
	pTemp+=4;
	lu16Version = *(uint16*)pTemp;
	
	

	UpdatePath[13] =  DEC_TO_ASCII(lu16Version&0x0f);
	UpdatePath[12] = DEC_TO_ASCII(lu16Version>>4&0x0f);
	UpdatePath[10] = DEC_TO_ASCII(lu16Version>>8&0x0f);
	UpdatePath[9] = DEC_TO_ASCII(lu16Version>>12&0x0f);
	
	debugX(LOG_LEVEL_INFO,"TotalPacket=%d  MaxPackSize=%d TotalSize=%x Version=%x \r\n",lu16TotalPacket,
		gu16MaxPackSize,lu32TotalSize,lu16Version);
	_ProData.MsgLength		= 1;
	_ProData.MsgType		= 0x82;
	ReadDateTime(_ProData.TimeSmybol);

	if(lu16Version <= gVersion){   //�汾���ԣ�����0x10.
		*_ProData.Data.pDataBack= 0x10; 				//�쳣��Ӧ
		Err = 0;
		debugX(LOG_LEVEL_ERROR,"gVersion=%x Version=%x update Version is lower than gVersion.\r\n ",gVersion,lu16Version);
	}
	else if(gu16MaxPackSize > 900){
		*_ProData.Data.pDataBack= 0x0f; //�쳣��Ӧ,0x0f��ʾ����С̫��
		Err = 0;
	}
	else if(lu16Version != gu16Version){  //�汾��ͬ
		memset(PacketReceiveFlag,0,sizeof(PacketReceiveFlag)); //�汾��ͬ�������ǣ���ͷ��ʼ��
		*_ProData.Data.pDataBack= 0x01; 	//������Ӧ
		Err = 0;
		gu16Version = lu16Version;
		gu32TotalSize = lu32TotalSize;
		gu8UpdataCheckTimes = 0;
		gUpdateBegin =1;
	}
	else if(lu32TotalSize != gu32TotalSize){  //�����ļ���С��ͬ
		memset(PacketReceiveFlag,0,sizeof(PacketReceiveFlag)); //�����ļ���С��ͬ�������ǣ���ͷ��ʼ��
		*_ProData.Data.pDataBack= 0x01; 	//������Ӧ
		Err = 0;
		gu32TotalSize = lu32TotalSize;
		gu16Version = lu16Version;
		gu8UpdataCheckTimes = 0;
		gUpdateBegin =1;
	}
	else{
		*_ProData.Data.pDataBack= 0x01; 	//������Ӧ
		Err = 0;
		gUpdateBegin =1;
	}

	
	
	return Err;
}

/********************************************************************************************************
**  	�� ��  �� ��: PARA_ProcessMsg_83				    							                       
**	�� ��  �� ��: ��վ������������                                                                   			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  	��   ��   ֵ: none	
**  	author:yangfei
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_83(_ProtocolType &_ProData)
{
	uint8 Err=0;
	uint8 *pTemp = _ProData.Data.pData;

	uint16 TotalPacket = 0;
	uint16 NowPacket = 0;
	uint16 PackSize= 0;
	//uint8 UpdateData[1024] 	= {0x00};	 /*�����ļ�����*/
	uint32 Crc= 0;
	vu32 CRCValue = 0;
	
	TotalPacket =*(uint16*)pTemp;
	pTemp+=2;
	NowPacket =*(uint16*)pTemp;
	pTemp+=2;
	PackSize =*(uint16*)pTemp;
	pTemp+=2;
	debugX(LOG_LEVEL_INFO,"TotalPacket=%d  NowPacket=%d PackSize=%d \r\n",TotalPacket,NowPacket,PackSize);
	
	memcpy(UpdateData,pTemp,PackSize);
	//debugX(LOG_LEVEL_INFO,"UpdateData=%s\r\n",UpdateData);
	pTemp+=PackSize;
	Crc=*(uint32*)pTemp;
	/*����CRC*/
	CRC_ResetDR();
    CRCValue = CRC_CalcBlockCRC((u32 *)UpdateData, PackSize/4);/* Compute the CRC of "DataBuffer" */
	debugX(LOG_LEVEL_DEBUG,"Crc=%x    CRCValue=%x\r\n",Crc,CRCValue);	
	if(CRCValue == Crc)	
	{
		PacketReceiveFlag[NowPacket]=0xaa;
	}
	else
	{
		debugX(LOG_LEVEL_ERROR,"NowPacket=%d crc err!\r\n",NowPacket);
	}
	OSMutexPend (FlashMutex,0,&Err);
	if(NowPacket<=TotalPacket)
	{
		//Err = SDSaveUpdateData("/update.bin",UpdateData, PackSize, (NowPacket-1)*MaxPackSize);
		Err = SDSaveData(UpdatePath,UpdateData, PackSize, (NowPacket-1)*gu16MaxPackSize);
		if(Err!=NO_ERR)
		{
			debugX(LOG_LEVEL_ERROR,"NowPacket=%d SDSaveUpdateData err!\r\n",NowPacket);
		}
		//Err = SDSaveData(UpdatePath,UpdateData, PackSize, (NowPacket-1)*MaxPackSize);/*NowPacket from 1 start*/
	}
    OSMutexPost (FlashMutex);
	
	_ProData.MsgLength		= 1;
	_ProData.MsgType		= 0x83;
	ReadDateTime(_ProData.TimeSmybol);
	
	if(Err != NO_ERR)
		{
			*_ProData.Data.pDataBack= 0x10;					//�쳣��Ӧ
			Err = 0;
		}
	else
		{
			*_ProData.Data.pDataBack= 0x01;					//������Ӧ
		}
	Err = 1;/*���ظ���վ*/

	
	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_84				    							                       
**	�� ��  �� ��: ��վ�·�Զ������������־                                                           			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none	
** author:yangfei
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_84(_ProtocolType &_ProData)
{
	uint8 Err=0;
	uint8 *pTemp = _ProData.Data.pData;

	uint16 TotalPacket = 0;
	uint32 TotalSize= 0;
	uint16 Version = 0;
	uint32 Crc= 0;
	vu32 CRCValue = 0;
	uint16 NowPacket= 0;
	uint16 ReadSize= 1024;
	CPU_SR			cpu_sr;
	uint16 UpdateUsedTime;
	
	uint8 SystemTime[6] = {0};
	OS_ENTER_CRITICAL();
	memcpy(SystemTime, gSystemTime, 6);
	OS_EXIT_CRITICAL();
	UpdateEndTime = SystemTime[ARRAY_HOUR]*3600+SystemTime[ARRAY_MINUTE]*60+SystemTime[ARRAY_SECOND];
	UpdateUsedTime =UpdateEndTime-UpdateStartTime;
	debug("----------------------------\r\n");
	debug("Congratulate update OK  \r\n");
	debug("Update Used Time is %d\r\n",UpdateUsedTime);
	debug("----------------------------\r\n");
	TotalPacket = *(uint16*)pTemp;
	pTemp+=2;
	TotalSize = *(uint32*)pTemp;
	pTemp+=4;
	Version = *(uint16*)pTemp;
	pTemp+=2;
	Crc = *(uint32*)pTemp;
	debug_info(gDebugModule[UPDATE_MODULE],"TotalPacket=%d  TotalSize=%d   Version=%x \r\n",TotalPacket,TotalSize,Version);

	OSMutexPend (FlashMutex, 0, &Err);
	CRC_ResetDR();
	
	/*ÿ�ζ�ȡ1024�ֽ�ȥУ��*/
	for(NowPacket=1;NowPacket<=(TotalSize/1024+1);NowPacket++)
	{
		if(NowPacket == (TotalSize/1024+1))
		{
			ReadSize = TotalSize%1024;
		}
		Err = SDReadData(UpdatePath,UpdateData, ReadSize, (NowPacket-1)*1024);
		if(Err != NO_ERR)
		{
			debugX(LOG_LEVEL_ERROR,"NowPacket=%d SDReadData err!\r\n",NowPacket);
		}
		CRCValue = CRC_CalcBlockCRC((u32 *)UpdateData, ReadSize/4);
	}
	OSMutexPost (FlashMutex);
	
	if(Crc == CRCValue)
	{
		debug_info(gDebugModule[UPDATE_MODULE]," CRC OK !\r\n");
		debugX(LOG_LEVEL_ERROR,"%s Update file CRC OK!\r\n",__FUNCTION__);
		
		BKP_WriteBackupRegister(BKP_DR2, Version);
		/*����Զ������������־  */
		BKP_WriteBackupRegister(BKP_DR1, 0xaa55);
		gVersion = Version;/*update Version*/
		/*needed to be modified*/
		gRestartFlag = TRUE;    /*delay 5s reset*/
		gUpdateBegin =0;
		Err = NO_ERR;

		memset(PacketReceiveFlag,0,sizeof(PacketReceiveFlag)); //���������������ǡ�
		gu16Version = 0;
		gu16TotalPacket = 0;
		gu32TotalSize = 0;
		gu16MaxPackSize = 0; 
		gu8UpdataCheckTimes = 0;
	}
	else
	{
		debugX(LOG_LEVEL_ERROR,"%s CRC err!\r\n",__FUNCTION__);
		Err = ERR_1;

		memset(PacketReceiveFlag,0,sizeof(PacketReceiveFlag)); //���������������ǡ�
		gu16Version = 0;
		gu16TotalPacket = 0;
		gu32TotalSize = 0;
		gu16MaxPackSize = 0; 
		gu8UpdataCheckTimes = 0;
		gUpdateBegin = 0;
	}
	
	OS_ENTER_CRITICAL();
	memset(PacketReceiveFlag,0,sizeof(PacketReceiveFlag));
	OS_EXIT_CRITICAL();
	
	_ProData.MsgLength		= 1;
	_ProData.MsgType		= 0x84;
	ReadDateTime(_ProData.TimeSmybol);
	
	if(Err != NO_ERR)
		{
			*_ProData.Data.pDataBack= 0x10;					//�쳣��Ӧ
			Err = 0;
		}
	else
		{
			*_ProData.Data.pDataBack= 0x01;					//������Ӧ
		}	
	return Err;
}

/******************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_85				    							                       
**	�� ��  �� ��: ��վѯ���ļ�ȱ�����                                                			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none		
** author:yangfei
**	��		  ע: 	                                                                                   																									
*******************************************************************************************/
uint8 PARA_ProcessMsg_85(_ProtocolType &_ProData)
{
	uint8 Err=0;
	uint8 *pTemp = _ProData.Data.pData;
	uint16 i;
	uint16 TotalPacket = 0;
	uint32 TotalSize= 0;
	uint16 Version = 0;
	uint8 *pDataBack = _ProData.Data.pDataBack;
	uint16 *pLackPacket = (uint16*)pDataBack;
	uint16 LackPacket = 0;

	gu8UpdataCheckTimes += 1; //��ѯȱ������++.
	
	debugX(LOG_LEVEL_INFO,"line=%d LackPacket=%d \r\n",__LINE__,LackPacket);
	pDataBack+=2;
	
	TotalPacket = *(uint16*)pTemp;
	pTemp+=2;
	TotalSize = *(uint32*)pTemp;
	pTemp+=4;
	Version = *(uint16*)pTemp;

	if(Version != gu16Version){  //����汾�Ų��ԣ�ֱ�ӷ���ʧ�ܡ�
		_ProData.MsgLength		= 1;
		_ProData.MsgType		= 0x85;
		*_ProData.Data.pDataBack= 0x10; 				//�쳣��Ӧ
		ReadDateTime(_ProData.TimeSmybol);
		Err = 0;

	}
	else if(gu8UpdataCheckTimes > 15){  //�����ѯȱ������15�Σ���Ϊʧ�ܣ���������ʧ�ܴ���
		_ProData.MsgLength		= 1;
		_ProData.MsgType		= 0x85;
		*_ProData.Data.pDataBack= 0x10; 				//�쳣��Ӧ
		ReadDateTime(_ProData.TimeSmybol);
		Err = 0;
	}
	else{
		/*����ȱ�����*/
		for(i=1;i <= TotalPacket;i++){
			if(PacketReceiveFlag[i] != 0xaa){
				if(LackPacket>100){/*��ֹ�ظ�������̫������1024*/
						break;
					}
					LackPacket++;
					*(uint16*)pDataBack = i;
					pDataBack+=2;
				}
		}
		debugX(LOG_LEVEL_INFO,"line=%d LackPacket=%d Version=%x TotalSize=%x\r\n",__LINE__,LackPacket,Version,TotalSize);
		*pLackPacket = LackPacket;
		_ProData.MsgLength		= 2+2*LackPacket;
		_ProData.MsgType		= 0x85;
		ReadDateTime(_ProData.TimeSmybol);
		Err = 0;
	}


			
	return Err;
}


#ifdef PARA_READ_ALLOW
void DecimalToBinary(int num,uint8 *BinaryArry)
	{   
	uint8 *Binary;
	Binary =BinaryArry;
	if(num ==0)    
		{
		return;   
	    }  
	else   
		{    
		if(num/2)
			{			    
        		counter++;
        		DecimalToBinary(num/2,Binary);           
        		Binary[31-(--counter)]= num%2;
			}
		else
			{
                Binary[31-counter]=num%2;
			
		    }
		}
	}

/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_50			    							                       
**	�� ��  �� ��: ��ȡ�ն˻�����Ϣ                                                                      			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 PARA_ProcessMsg_50(_ProtocolType &_ProData)
{
	uint8 Err			= 0;
	uint8 *pTemp		= _ProData.Data.pData;		
	uint8 *pTempback		= _ProData.Data.pDataBack;
	TermParaSaveType TermPara;
	CPU_SR		cpu_sr;
	if(*pTemp == 0xff)
    	{
    		MakeFile("/test");
    		do{
    			OSMutexPend (FlashMutex,0,&Err);
    			Err = SDReadData("/test", &TermPara, sizeof(TermParaSaveType), 0);
    			OSMutexPost (FlashMutex);
    			if(Err!=NO_ERR) 
    			  {
    				debug("%s %d read test err=%d!\r\n",__FUNCTION__,__LINE__,Err);
    			  }
    		}while(Err!=NO_ERR);
    		/*end:yangfei modified 2013-4-3 */
    		OSMutexPend (FlashMutex,0,&Err);
    		Err = SDReadData("/TERMINAL_PARA", &TermPara, sizeof(TermParaSaveType), 0);
    		OSMutexPost (FlashMutex);
    
    
    		if(Err!=NO_ERR) 
    		  {
    			debug("%s %d read TERMINAL_PARA err=%d!\r\n",__FUNCTION__,__LINE__,Err);
    		  }
    		
    		if(TermPara.WriteFlag == 0xAA55)
    			{
    			
				OS_ENTER_CRITICAL();											//���µ�ǰ�ڴ��ַ����
    				gPARA_TermPara = TermPara;
				OS_EXIT_CRITICAL();
    				
    			}
            else
            	{
				memset(gPARA_TermPara.TermAddr, 0x00, 6);
				memset(gPARA_TermPara.HostAddr, 0x00, 6);
				gPARA_TermPara.TermAddr[0]	= 0x34;
				gPARA_TermPara.TermAddr[1]	= 0x12; 
				
				gPARA_TermPara.HostAddr[0]	= 0x01;
				gPARA_TermPara.HostAddr[1]	= 0x00;
				/*begin:yangfei added 20140224 for add support time_on_off*/
				gPARA_TermPara.DeviceType	= HEAT_METER_TYPE;
    
         		gPARA_TermPara.DebugPara.FrameLen_Max = 450;
    		    }
    
        		memcpy(pTempback, gPARA_TermPara.TermAddr, 6);//�������ն˱��
        		pTempback +=6;
        		memcpy(pTempback, gPARA_TermPara.HostAddr, 6);	//��������վ���					
        		pTempback +=6;
        		memcpy(pTempback,(uint8 *)&(gPARA_TermPara.DebugPara.FrameLen_Max),2);	//���ݰ�����					
        		pTempback +=2;
        		*pTempback = gPARA_TermPara.DebugPara.FrameToFrameTime;//���ݰ���ʱʱ��
        		pTempback++;
        		*pTempback = 0x00;//���ݰ��ط�����
    	}
	else
		{
          Err =1;
	    }
	
	
	
	//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= 0x10;
	_ProData.MsgType		= 0x50;
		
	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_51			    							                       
**	�� ��  �� ��: ��ȡ������Ϣ                                                                      			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 PARA_ProcessMsg_51(_ProtocolType &_ProData)
	{
	
        	uint16 i	= 0;
			uint8 Err			= 0;
			uint8 *pTemp		= _ProData.Data.pData;		
			uint8 *pTempback		= _ProData.Data.pDataBack;
			TermParaSaveType TermPara;
			CPU_SR		cpu_sr;
			MeterFilePara	para;
			MeterFileType mf;
//			UpCommParaSaveType	UpPara;
			/*begin:yangfei modified 2012-12-24 find path*/
//			char  NodePath[] = "/2012/12/24/timenode";
    		  uint8 counter=0;
    		  uint32 DA,DT;
			  uint16 j	  = 0;
    		  
			  counter=0;
    		  memset((uint8 *)SpecialMeter, 0x00, sizeof(SpecialMeter));
    		  memset((uint8 *)BinaryDA, 0x00, sizeof(BinaryDA)); 
    		  memset((uint8 *)BinaryDT, 0x00, sizeof(BinaryDT));
    		  DA = (*pTemp++)+((*pTemp++)<<8)+((*pTemp++)<<16)+((*pTemp++)<<24);//ȡDA ȫ���ĸ��ֽ�ת������ֵ
    		  
    		  DT = (*pTemp++)+((*pTemp++)<<8)+((*pTemp++)<<16)+((*pTemp++)<<24);//ȡDT ȫ���ĸ��ֽ�ת������ֵ
    		if((DA==0)&&(DT==0))
    		{
    		}
    		else
    		{
    		  DecimalToBinary(DA,BinaryDA);
    		  DecimalToBinary(DT,BinaryDT);
    		  for(i=0;i<32;i++)
    			{
    			   for(j=0;j<32;j++)
    				{
    				  if(BinaryDA[i]==1)
    					{
    					  if(BinaryDT[j]==1)
    						{
    						 SpecialMeter[counter]= 32*i+j+1;
    						 counter ++;
    						}
    					}
    				}
    			}
    		}
    		OSMutexPend (FlashMutex,0,&Err);
    		Err = SDReadData("/TERMINAL_PARA", &TermPara, sizeof(TermParaSaveType), 0);
    		OSMutexPost (FlashMutex);
    
    
    		if(Err!=NO_ERR) 
    		  {
    			debug("%s %d read TERMINAL_PARA err=%d!\r\n",__FUNCTION__,__LINE__,Err);
    		  }
    		if(TermPara.WriteFlag == 0xAA55)
    			{
    			
				OS_ENTER_CRITICAL();											//���µ�ǰ�ڴ��ַ����
    				gPARA_TermPara = TermPara;
				OS_EXIT_CRITICAL();
    				
    			}
            else
            	{
				memset(gPARA_TermPara.TermAddr, 0x00, 6);
				memset(gPARA_TermPara.HostAddr, 0x00, 6);
				gPARA_TermPara.TermAddr[0]	= 0x34;
				gPARA_TermPara.TermAddr[1]	= 0x12; 
				
				gPARA_TermPara.HostAddr[0]	= 0x01;
				gPARA_TermPara.HostAddr[1]	= 0x00;
				/*begin:yangfei added 20140224 for add support time_on_off*/
				gPARA_TermPara.DeviceType	= HEAT_METER_TYPE;
    
         		gPARA_TermPara.DebugPara.FrameLen_Max = 450;
    		    }
			
			//��ʼ��������Ϣ
			OSMutexPend (FlashMutex,0,&Err);
			Err = SDReadData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
			OSMutexPost (FlashMutex);
			if(Err!=NO_ERR) 
			  {
				debug("%s %d read METER_FILE_PARA err=%d!\r\n",__FUNCTION__,__LINE__,Err);
			  }
			
			if(para.WriteFlag == 0xAA55)
				{														//�Ѿ�д������Ч��������
					memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
#if 0					
					memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
					memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));
					memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
					memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
					memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
					memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
					memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
#endif				
					*pTempback++ = 0x10;//�����ϱ�������
                    *pTempback++ = FIRST_PACK;
					for(i=0; i<para.MeterStoreNums; i++)
					{
						OSMutexPend (FlashMutex,0,&Err);
						Err = SDReadData("/METER_FILE_ADDR", &mf, sizeof(MeterFileType), sizeof(MeterFileType)*i);
						OSMutexPost (FlashMutex);
						/*end  :yangfei modified 2012-12-27  */
						gPARA_MeterFiles[i] = mf;
#if 0						
						mfChannel = mf.ChannelIndex-1;				//�������洢ͨ����0��ʼ������վ����ͨ����1��ʼ
						//�ȼ�������ͨ������
						/*begin:yangfei deleted 2014-02-27 for ʱ��ͨ����������ȱ��ַΪ0����ַ���±��������*/
						//if(MeterNoBcdCheck(mf.MeterAddr))
							{
								ChannelTemp = gPARA_MeterChannelNum[mfChannel]++;
								gPARA_MeterChannel[mfChannel][ChannelTemp] = i;
							}
							
						//�����¿����ͨ��
						if(MeterNoBcdCheck(mf.ControlPanelAddr))
							{
								ChannelTemp = gPARA_ConPanelChannelNum[mfChannel]++;
								gPARA_ConPanelChannel[mfChannel][ChannelTemp] = i;
							}
				
						//�����¿����ͨ��
						if(MeterNoBcdCheck(mf.ValveAddr))
							{
								ChannelTemp = gPARA_ValveConChannelNum[mfChannel]++;
								gPARA_ValveConChannel[mfChannel][ChannelTemp] = i;
							}
#endif						
					}
					gPARA_MeterNum = para.MeterStoreNums;
					if((DA==0)&&(DT==0))
						{
        					for(i=0; i<gPARA_MeterNum; i++)
        					{
        					
                					*pTempback++ =gPARA_TermPara.TermAddr[0];//�豸�ŵͰ�λ
                					*pTempback++ =gPARA_TermPara.TermAddr[1];//�豸�Ÿ߰�λ       					
        							memcpy(pTempback, gPARA_MeterFiles, sizeof(MeterFileType));
        					
        						pTempback += sizeof(MeterFileType);
        					}
						}
					else
						{
                          
						  for(i=0; i<counter; i++)
						  {
						  
						  *pTempback++ =gPARA_TermPara.TermAddr[0];//�豸�ŵͰ�λ
						  *pTempback++ =gPARA_TermPara.TermAddr[1];//�豸�Ÿ߰�λ						  
							memcpy(pTempback, (uint8*)&gPARA_MeterFiles[SpecialMeter[counter]], sizeof(MeterFileType));
						  
							  pTempback += sizeof(MeterFileType);
						  }

					    }
					
				}
			else															//δд�����Ч��������
				{
					gPARA_MeterNum			= 0;
					gPARA_SendIndex_Prevous = 0;
					memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
					memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
					memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
					memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
					memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
					memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
					memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
					Err =1;
				}
			
			
			//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
			if((DA==0)&&(DT==0))			
			_ProData.MsgLength		=  (sizeof(MeterFileType)+2)*gPARA_MeterNum+2;
			else
			_ProData.MsgLength		=  (sizeof(MeterFileType)+2)*counter+2;				
			_ProData.MsgType		= 0x51;
				
			return Err;
		}
/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_51			    							                       
**	�� ��  �� ��: ��ȡ����ͳ����Ϣ                                                                      			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 PARA_ProcessMsg_52(_ProtocolType &_ProData)
{
	
	uint16 i	= 0;
	uint8 Err			= 0;
	uint8 *pTemp		= _ProData.Data.pData;		
	uint8 *pTempback		= _ProData.Data.pDataBack;
	uint8 ChannelTemp = 0;
	uint8 mfChannel	  = 0;
//	CPU_SR		cpu_sr;
	MeterFilePara	para;
	MeterFileType mf;
	
	if(*pTemp == 0xff)
	 {
	//��ʼ��������Ϣ
     	OSMutexPend (FlashMutex,0,&Err);
     	Err = SDReadData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
     	OSMutexPost (FlashMutex);
     	if(Err!=NO_ERR) 
     	  {
     		debug("%s %d read METER_FILE_PARA err=%d!\r\n",__FUNCTION__,__LINE__,Err);
     	  }
     	
     	if(para.WriteFlag == 0xAA55)
     		{														//�Ѿ�д������Ч��������
     			memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
     #if 1					
     			memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
     			memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));
     			memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
     			memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
     			memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
     			memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
     			memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
     #endif				
     			for(i=0; i<para.MeterStoreNums; i++)
     			{
     				OSMutexPend (FlashMutex,0,&Err);
     				Err = SDReadData("/METER_FILE_ADDR", &mf, sizeof(MeterFileType), sizeof(MeterFileType)*i);
     				OSMutexPost (FlashMutex);
     				/*end  :yangfei modified 2012-12-27  */
     				gPARA_MeterFiles[i] = mf;
     #if 1						
     				mfChannel = mf.ChannelIndex-1;				//�������洢ͨ����0��ʼ������վ����ͨ����1��ʼ
     				//�ȼ�������ͨ������
     				/*begin:yangfei deleted 2014-02-27 for ʱ��ͨ����������ȱ��ַΪ0����ַ���±��������*/
     				//if(MeterNoBcdCheck(mf.MeterAddr))
     					{
     						ChannelTemp = gPARA_MeterChannelNum[mfChannel]++;
     						gPARA_MeterChannel[mfChannel][ChannelTemp] = i;
     					}
     					
     				//�����¿����ͨ��
     				if(MeterNoBcdCheck(mf.ControlPanelAddr))
     					{
     						ChannelTemp = gPARA_ConPanelChannelNum[mfChannel]++;
     						gPARA_ConPanelChannel[mfChannel][ChannelTemp] = i;
     					}
     		
     				//���䷧��ͨ��
     				if(MeterNoBcdCheck(mf.ValveAddr))
     					{
     						ChannelTemp = gPARA_ValveConChannelNum[mfChannel]++;
     						gPARA_ValveConChannel[mfChannel][ChannelTemp] = i;
     					}
     #endif						
     			}
     			gPARA_MeterNum = para.MeterStoreNums;
				
     		}
     	else															//δд�����Ч��������
     		{
     			gPARA_MeterNum			= 0;
     			gPARA_SendIndex_Prevous = 0;
     			memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
     			memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
     			memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
     			memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
     			memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
     			memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
     			memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
				
		    }
		
		//�ȴ�Ͱ�λ�ٴ�߰�λ
		
		*pTempback++ = (gPARA_MeterNum)&0x00ff;//�ȼ������ܸ���
		*pTempback++ = (gPARA_MeterNum>>8)&0x00ff;//�ȼ������ܸ����߰�λ		
		for(i=0; i<6; i++)
		{
			
			*(pTempback++) = (gPARA_MeterChannelNum[i])&0x00ff;//ͨ��n�ȼ��������
			*(pTempback++) = (gPARA_ValveConChannelNum[i])&0x00ff;//ͨ��n���Ÿ���
			*(pTempback++) = (gPARA_ConPanelChannelNum[i])&0x00ff;//ͨ��n�¿�������
		}
		*pTempback++ = 0x00;//�Ǳ�ͨѶ��ַ���û���Ϣ��Ӧ�������				
		*pTempback++ = 0x00;//�Ǳ�ͨѶ��ַ���û���Ϣ��Ӧ�������Ԥ�����ֽ�
		memset(pTempback, 0x00, 40);//Ԥ��40�ֽ�
	
		}
	   else
	   	{
	   	  Err =1;
	   	}
	   
	//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		=  62;
	_ProData.MsgType		= 0x52;
		
	return Err;
}

/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_53			    							                       
**	�� ��  �� ��: ��ȡ�ն˳���ʱ���                                                        			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 PARA_ProcessMsg_53(_ProtocolType &_ProData)
{
//	uint16 i	= 0;
	uint8 Err 	= 0;	
//	uint8 *pTemp		= _ProData.Data.pData;		
	uint8 *pTempback		= _ProData.Data.pDataBack;
//	uint8 SystemTime[6] = {0x00};
	uint16 TimeNode[TIME_NODE_NUM_MAX+1] = {0x00};
//	char  NodePath[] = "/2012/12/24/timenode";
	uint8 gPara_timenodeNum=0;	
	CPU_SR		cpu_sr;
	//��ʼ������ʱ��ڵ���Ϣ
	OSMutexPend (FlashMutex,0,&Err);
	Err = SDReadData("/TIME_NODE_ADDR", TimeNode, sizeof(TimeNode), 0);
    OSMutexPost (FlashMutex);
	if(Err!=NO_ERR)	
	  {
	    debug("%s %d read TIME_NODE_ADDR err=%d!\r\n",__FUNCTION__,__LINE__,Err);
	  }
	if(TimeNode[0] == 0xAA55)
		{
			OS_ENTER_CRITICAL();
			memcpy((uint8 *)gPARA_TimeNodes, (uint8 *)&TimeNode[1], (TIME_NODE_NUM_MAX)*sizeof(uint16));
			OS_EXIT_CRITICAL();
			while(1)
			{
              if(gPARA_TimeNodes[gPara_timenodeNum])
			  	gPara_timenodeNum++;
			  else
			  	break;
			}
		}
	else
		{
			OS_ENTER_CRITICAL();
			memset((uint8 *)gPARA_TimeNodes, 0xFF, (TIME_NODE_NUM_MAX)*sizeof(uint16));
			gPARA_TimeNodes[0] = 0x0100;
			gPARA_TimeNodes[1] = 0x0700;
			gPARA_TimeNodes[2] = 0x1100;
			gPARA_TimeNodes[3] = 0x2000;

			OS_EXIT_CRITICAL();
			Err =1;
		}	
	*pTempback++ =gPara_timenodeNum;
	memcpy(pTempback,(uint8 *)&gPARA_TimeNodes[1], gPara_timenodeNum*sizeof(uint16));
	//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		=  gPara_timenodeNum*sizeof(uint16)+1;
	_ProData.MsgType		= 0x52;
		
	return Err;

}
/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_54			    							                       
**	�� ��  �� ��: ��ȡ��վIP�Ͷ˿�ͨѶ����                                                              			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 PARA_ProcessMsg_54(_ProtocolType &_ProData)
{
//	uint16 i	= 0;
	uint8 Err 	= 0;
//	MeterFilePara	para;
//	TermParaSaveType TermPara;
	UpCommParaSaveType	UpPara;
	uint8 *pTemp		= _ProData.Data.pData;		
	uint8 *pTempback		= _ProData.Data.pDataBack;		
	CPU_SR		cpu_sr;
	if(*pTemp == 0xff)
		{
          OSMutexPend (FlashMutex,0,&Err);
          Err = SDReadData("/UP_COMM_PARA", &UpPara, sizeof(UpCommParaSaveType), 0);
          OSMutexPost (FlashMutex);
          if(Err!=NO_ERR) 
            {
          	debug("%s %d read UP_COMM_PARAerr=%d!\r\n",__FUNCTION__,__LINE__,Err);
            }
          
		  memset(&uPCommPara, 0x00, sizeof(uPCommPara));//������Ҫ���� ����վ�·�������ʱ��Ҫ���и�ֵmuxiaoqing
          
          if(UpPara.WriteFlag == 0xAA55)
          	{
          		OS_ENTER_CRITICAL();
          		gPARA_UpPara = UpPara;
          		OS_EXIT_CRITICAL();
          	}	
          else
          	{
          		OS_ENTER_CRITICAL();
          		gPARA_UpPara.HostIPAddr[3] = 0x3A;
          		gPARA_UpPara.HostIPAddr[2] = 0x6D;
          		gPARA_UpPara.HostIPAddr[1] = 0x50;
          		gPARA_UpPara.HostIPAddr[0] = 0x01;
          		
          		gPARA_UpPara.HostPort	   = 0x475;
          		gPARA_UpPara.HeartCycle    = 0x01;
          		gPARA_UpPara.RelandDelay   = 0x02;
          		gPARA_UpPara.RelandTimes   = 0x05;
          		OS_EXIT_CRITICAL();
          	}

		}
	else
		{
          Err =1;
	    }

	memcpy(pTempback,UpPara.HostIPAddr, 4);
	
	memcpy(uPCommPara.PPIP1,UpPara.HostIPAddr, 4);
	uPCommPara.PPPort= UpPara.HostPort;
	uPCommPara.HeartCycle= UpPara.HeartCycle;
	uPCommPara.RelandDelay= UpPara.RelandDelay;
	uPCommPara.RelandTimes= UpPara.RelandTimes;
	
	pTempback += 4;
	//�ȴ�Ͱ�λ�ٴ�߰�λ
	
	*pTempback++ = (UpPara.HostPort)&0x00ff;//�˿ں�
	*pTempback++ = (UpPara.HostPort>>8)&0x00ff;//�˿ںŸ߰�λ		
	
	memcpy(pTempback,uPCommPara.DNSName, sizeof(uPCommPara.DNSName));//��վ��̬����
	pTempback +=40;
	//�ȴ�Ͱ�λ�ٴ�߰�λ		
	*pTempback++ = (uPCommPara.PPport)&0x00ff;//���ö˿ں�	
	*pTempback++ = (uPCommPara.PPport>>8)&0x00ff;//���ö˿ں�
	*pTempback++ = uPCommPara.SwitchIPorDNS;//IP��ַ�������л�ĿǰĬ��Ϊ0	
	 memcpy(pTempback,uPCommPara.APNName, sizeof(uPCommPara.APNName));//��վ��̬����
	 pTempback +=16;	 
	*pTempback++ = UpPara.HeartCycle;
	*pTempback++ = UpPara.RelandDelay;
	*pTempback++ = UpPara.RelandTimes;
	//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		=  68;
	_ProData.MsgType		= 0x54;
		
return Err;
}
/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_55			    							                       
**	�� ��  �� ��: ��ȡ�ն�IP�Ͷ˿�                                                                      			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 PARA_ProcessMsg_55(_ProtocolType &_ProData)
	{
//		uint16 i	= 0;
		uint8 Err	= 0;
//		MeterFilePara	para;
//		TermParaSaveType TermPara;
		UpCommParaSaveType	UpPara;
		uint8 *pTemp		= _ProData.Data.pData;		
		uint8 *pTempback		= _ProData.Data.pDataBack;		
		CPU_SR		cpu_sr;
		if(*pTemp == 0xff)
			{
			  OSMutexPend (FlashMutex,0,&Err);
			  Err = SDReadData("/UP_COMM_PARA", &UpPara, sizeof(UpCommParaSaveType), 0);
			  OSMutexPost (FlashMutex);
			  if(Err!=NO_ERR) 
				{
				debug("%s %d read UP_COMM_PARAerr=%d!\r\n",__FUNCTION__,__LINE__,Err);
				}
			  
			  memset(&uPCommPara, 0x00, sizeof(uPCommPara));//������Ҫ���� ����վ�·�������ʱ��Ҫ���и�ֵmuxiaoqing
			  
			  if(UpPara.WriteFlag == 0xAA55)
				{
					OS_ENTER_CRITICAL();
					gPARA_UpPara = UpPara;
					OS_EXIT_CRITICAL();
				}	
			  else
				{
					OS_ENTER_CRITICAL();
					gPARA_UpPara.HostIPAddr[3] = 0x3A;
					gPARA_UpPara.HostIPAddr[2] = 0x6D;
					gPARA_UpPara.HostIPAddr[1] = 0x50;
					gPARA_UpPara.HostIPAddr[0] = 0x01;
					
					gPARA_UpPara.HostPort	   = 0x475;
					gPARA_UpPara.HeartCycle    = 0x01;
					gPARA_UpPara.RelandDelay   = 0x02;
					gPARA_UpPara.RelandTimes   = 0x05;
					OS_EXIT_CRITICAL();
				}
	
			}
		else
			{
			  Err =1;
			}
	
		memcpy(pTempback,UpPara.HostIPAddr, 4);//IP
		
		memcpy(uPCommPara.PPIP1,UpPara.HostIPAddr, 4);
		uPCommPara.PPPort= UpPara.HostPort;
		uPCommPara.HeartCycle= UpPara.HeartCycle;
		uPCommPara.RelandDelay= UpPara.RelandDelay;
		uPCommPara.RelandTimes= UpPara.RelandTimes;
		
		pTempback += 4;
		memcpy(pTempback,uPCommPara.TermSubMask, 4);//���������ַ��
		
		pTempback += 4;
		memcpy(pTempback,uPCommPara.TermGateWay, 4);//���ص�ַ��
		pTempback += 4;
		
		//�ȴ�Ͱ�λ�ٴ�߰�λ
		*pTempback++ = (UpPara.HostPort)&0x00ff;//�˿ں�
		*pTempback++ = (UpPara.HostPort>>8)&0x00ff;//�˿ںŸ߰�λ		
		

		//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
		_ProData.MsgLength		=  14;
		_ProData.MsgType		= 0x55;
			
	return Err;
	}

	


/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_56			    							                       
**	�� ��  �� ��: ��ȡ�����ñ�����Ϣ                                                                      			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 PARA_ProcessMsg_56(_ProtocolType &_ProData)
{
  
  uint8 Err 		  = 0;
  uint8 *pTemp		  = _ProData.Data.pData;	  
  uint8 *pTempback		  = _ProData.Data.pDataBack;
  TermParaSaveType TermPara;
  CPU_SR	  cpu_sr;
  int8 i,j;
  uint8 counter=0;
  uint32 DA,DT;
  
  MeterFilePara   para;
  MeterFileType mf;
//  UpCommParaSaveType  UpPara;
  /*begin:yangfei modified 2012-12-24 find path*/
//  char	NodePath[] = "/2012/12/24/timenode";
  memset((uint8 *)SpecialMeter, 0x00, sizeof(SpecialMeter));
  memset((uint8 *)BinaryDA, 0x00, sizeof(BinaryDA)); 
  memset((uint8 *)BinaryDT, 0x00, sizeof(BinaryDT));
  DA = (*pTemp++)+((*pTemp++)<<8)+((*pTemp++)<<16)+((*pTemp++)<<24);//ȡDA ȫ���ĸ��ֽ�ת������ֵ
  
  DT = (*pTemp++)+((*pTemp++)<<8)+((*pTemp++)<<16)+((*pTemp++)<<24);//ȡDT ȫ���ĸ��ֽ�ת������ֵ
if((DA==0)&&(DT==0))
{
}
else
{
  DecimalToBinary(DA,BinaryDA);
  DecimalToBinary(DT,BinaryDT);
  for(i=0;i<32;i++)
  	{
       for(j=0;j<32;j++)
       	{
       	  if(BinaryDA[i]==1)
       	  	{
       	  	  if(BinaryDT[j]==1)
       	  	  	{
       	  	  	 SpecialMeter[counter]= 32*i+j+1;
				 counter ++;
       	  	  	}
       	  	}
       	}
    }
}
  OSMutexPend (FlashMutex,0,&Err);
  Err = SDReadData("/TERMINAL_PARA", &TermPara, sizeof(TermParaSaveType), 0);
  OSMutexPost (FlashMutex);
  
  
  if(Err!=NO_ERR) 
	{
	  debug("%s %d read TERMINAL_PARA err=%d!\r\n",__FUNCTION__,__LINE__,Err);
	}
  if(TermPara.WriteFlag == 0xAA55)
	  {
	  
	  OS_ENTER_CRITICAL();											  //���µ�ǰ�ڴ��ַ����
		  gPARA_TermPara = TermPara;
	  OS_EXIT_CRITICAL();
		  
	  }
  else
	  {
	  memset(gPARA_TermPara.TermAddr, 0x00, 6);
	  memset(gPARA_TermPara.HostAddr, 0x00, 6);
	  gPARA_TermPara.TermAddr[0]  = 0x34;
	  gPARA_TermPara.TermAddr[1]  = 0x12; 
	  
	  gPARA_TermPara.HostAddr[0]  = 0x01;
	  gPARA_TermPara.HostAddr[1]  = 0x00;
	  /*begin:yangfei added 20140224 for add support time_on_off*/
	  gPARA_TermPara.DeviceType   = HEAT_METER_TYPE;
  
	  gPARA_TermPara.DebugPara.FrameLen_Max = 450;
	  }

  //��ʼ��������Ϣ
  OSMutexPend (FlashMutex,0,&Err);
  Err = SDReadData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
  OSMutexPost (FlashMutex);
  if(Err!=NO_ERR) 
	{
	  debug("%s %d read METER_FILE_PARA err=%d!\r\n",__FUNCTION__,__LINE__,Err);
	}
  
  if(para.WriteFlag == 0xAA55)
				{														//�Ѿ�д������Ч��������
					memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
#if 0					
					memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
					memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));
					memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
					memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
					memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
					memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
					memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
#endif				
					*pTempback++ = 0x10;//�����ϱ�������
                    *pTempback++ = FIRST_PACK;
					for(i=0; i<para.MeterStoreNums; i++)
					{
						OSMutexPend (FlashMutex,0,&Err);
						Err = SDReadData("/METER_FILE_ADDR", &mf, sizeof(MeterFileType), sizeof(MeterFileType)*i);
						OSMutexPost (FlashMutex);
						/*end  :yangfei modified 2012-12-27  */
						gPARA_MeterFiles[i] = mf;
#if 0						
						mfChannel = mf.ChannelIndex-1;				//�������洢ͨ����0��ʼ������վ����ͨ����1��ʼ
						//�ȼ�������ͨ������
						/*begin:yangfei deleted 2014-02-27 for ʱ��ͨ����������ȱ��ַΪ0����ַ���±��������*/
						//if(MeterNoBcdCheck(mf.MeterAddr))
							{
								ChannelTemp = gPARA_MeterChannelNum[mfChannel]++;
								gPARA_MeterChannel[mfChannel][ChannelTemp] = i;
							}
							
						//�����¿����ͨ��
						if(MeterNoBcdCheck(mf.ControlPanelAddr))
							{
								ChannelTemp = gPARA_ConPanelChannelNum[mfChannel]++;
								gPARA_ConPanelChannel[mfChannel][ChannelTemp] = i;
							}
				
						//�����¿����ͨ��
						if(MeterNoBcdCheck(mf.ValveAddr))
							{
								ChannelTemp = gPARA_ValveConChannelNum[mfChannel]++;
								gPARA_ValveConChannel[mfChannel][ChannelTemp] = i;
							}
#endif						
					}
					gPARA_MeterNum = para.MeterStoreNums;
					if((DA==0)&&(DT==0))
						{
        					for(i=0; i<gPARA_MeterNum; i++)
        					{
        					
							*pTempback++ =gPARA_TermPara.TermAddr[0];//�豸�ŵͰ�λ
							*pTempback++ =gPARA_TermPara.TermAddr[1];//�豸�Ÿ߰�λ 						
        							memcpy(pTempback, gPARA_MeterFiles, sizeof(MeterFileType));
        					
        						pTempback += sizeof(MeterFileType);
        					}
						}
					else
						{
                          
						  for(i=0; i<counter; i++)
						  {
						  
						  *pTempback++ =gPARA_TermPara.TermAddr[0];//�豸�ŵͰ�λ
						  *pTempback++ =gPARA_TermPara.TermAddr[1];//�豸�Ÿ߰�λ						  
								  memcpy(pTempback, (uint8*)&gPARA_MeterFiles[SpecialMeter[counter]], sizeof(MeterFileType));
						  
							  pTempback += sizeof(MeterFileType);
						  }

					    }
				}
			else															//δд�����Ч��������
				{
					gPARA_MeterNum			= 0;
					gPARA_SendIndex_Prevous = 0;
					memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
					memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
					memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
					memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
					memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
					memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
					memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
					Err =1;
				}
			if((DA==0)&&(DT==0))			
			_ProData.MsgLength		=  (sizeof(MeterFileType)+2)*gPARA_MeterNum+2;
			else
			_ProData.MsgLength		=  (sizeof(MeterFileType)+2)*counter+2;				
			_ProData.MsgType		= 0x56;
			
			return Err;  //ԭû����仰�������о��棬��ӡ�
}
/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_57			    							                       
**	�� ��  �� ��: ��ȡ�ն����ݲɼ���ʱ���                                                                      			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	                                                                                   																									
*********************************************************************************************************/

uint8 PARA_ProcessMsg_57(_ProtocolType &_ProData)
{

	return 0;
}

#endif



/******************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_90			    							                       
**	�� ��  �� ��:����������������Ӳ���汾�š�����汾�š�IP/�˿ںŵȡ�
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none				
**	��		  ע: 	                                                                                   																									
*******************************************************************************************/

uint8 PARA_ProcessMsg_90(_ProtocolType &_ProData)
{
	uint8 i = 0;
	uint8 Err=0;
	
	StruJZQInit struJZQInit = {0};
	

	struJZQInit.u16HardwareVer = gHWVersion; //Ӳ���汾�����
	struJZQInit.u16SoftVer =  gVersion;  //����汾��

	for(i=0 ;i<4; i++)	{  //IP��ַ
		struJZQInit.u8IP[i] = gPARA_UpPara.HostIPAddr[i];
	}
	struJZQInit.u16Port = gPARA_UpPara.HostPort;  //�˿ں�

	for(i=0 ;i<6; i++)	{  //���ص�ַ
		struJZQInit.u8HostAddress[i] = gPARA_TermPara.HostAddr[i];
	}

	for(i=0 ;i<6; i++)	{  //Ŀ�ĵ�ַ
		struJZQInit.u8Address[i] = gPARA_TermPara.TermAddr[i];
	}

	//struJZQInit.Method = gMethod;
	//struJZQInit.DataSource = gDataSource;
	//struJZQInit.Period = gPeriod;
	//struJZQInit.u8LogCompressType = gu8LogCompressType;
	//struJZQInit.u8LogReportTime = gu8LogReportTime;
	//struJZQInit.u8LogOpenType = gu8LogOpenType;

	//debug("HardWareVesion=%x  Port=%x   SoftVersion=%x \r\n",struJZQInit.u16HardwareVer,struJZQInit.u16Port,struJZQInit.u16SoftVer);


	struJZQInit.Method = 0;
	struJZQInit.DataSource = 0;
	struJZQInit.Period = 0;
	struJZQInit.u8LogCompressType = 0;
	struJZQInit.u8LogReportTime = 0;
	struJZQInit.u8LogOpenType = 0;

	_ProData.Data.lenData   = sizeof(StruJZQInit);
	memcpy(_ProData.Data.pDataBack,&struJZQInit,sizeof(StruJZQInit));



	_ProData.MsgLength		= sizeof(StruJZQInit);
	_ProData.MsgType		= 0x91;

	ReadDateTime(_ProData.TimeSmybol);
	return Err;
	
}


/********************************************************************************************************
**  �� ��  �� ��: DEAL_ProcessMsg_92				    							                   
**	�� ��  �� ��: ��վ��������Ҫ�������д洢�ı������Ϣ.                                                 			
**	�� ��  �� ��: 							         			 	      
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 
                                       
*********************************************************************************************************/
uint8 DEAL_ProcessMsg_92(_ProtocolType &_ProData, uint8 *ControlSave)
{
	uint8 Err					= 0x00;
	uint8 Res					= 0x00;
	uint8 *pTemp = _ProData.Data.pDataBack;
	uint8 *pFirst				= pTemp;
	uint8 *pUserNum;
	uint8 MeterDataTemp[1024] 	= {0x00};					//��ʱ�洢SD��������
	uint8 *pDataTemp 			= MeterDataTemp;
	uint8 MeterFrameLen			= 0x00;						//�����ȡ��������֡��ȷ�ԣ�����������֡�ֽ���
	uint8 DataCounter			= 0x00;
	uint8 SendReadyFalg 		= 0x00;
	uint16 StartMeterSn			= 0x00;
	uint16 MeterSnMax			= 0x00;
	uint16 FrameDataFileLenMax	= 0x00;						//��ȡ��ǰ���õ�������ݰ�������������ֽ���
	uint16 DataBackLen			= 0x00;
	//uint32 LogicAddr			= LOGIC_ADDR_NULL;
	
	/*begin:yangfei modified 2012-12-24 find path*/
	char  DataPath[]="/METER_FILE_ADDR";
	/*end   :yangfei modified 2012-12-24*/
	
	LOG_assert_param( ControlSave == NULL );

	MultiFrameMsg_92 *pMultiFrame = NULL;
	pMultiFrame = (MultiFrameMsg_92 *)ControlSave;
	CPU_SR		cpu_sr;
		
	if(ControlSave == NULL)		while(1);

	//��ȡ��ǰ����������ֽ���
	//��ȡ�����б���ŵ������, ������ʵʱ���ݣ�ֱ�Ӷ�ȡ�ڴ汸�ݼ���
	OS_ENTER_CRITICAL();
	FrameDataFileLenMax = gPARA_TermPara.DebugPara.FrameLen_Max;
	OS_EXIT_CRITICAL();
	
	//��ȡ�ϴ��ϴ����ĸ������
	StartMeterSn = pMultiFrame->StartMeterSn;


	MeterSnMax = gPARA_MeterNum; //��������
	
    pFirst = pTemp++;										//�Ƿ��к���֡��־����֡���ſ�����д
    DataBackLen++;
    *pTemp++ = pMultiFrame->SendPackIndex;					//��ǰ���͵İ����
    DataBackLen++;
	*pTemp++ = 0x20; //�ȱ��̶�Ϊ0x20.
	DataBackLen++;
	pUserNum = pTemp++;  //�û����ݸ��������ֵ��
	DataBackLen++;
	
	
    while(!SendReadyFalg)
    {
	/*begin:yangfei modified 2012-12-24 find path*/
	OSMutexPend (FlashMutex, 0, &Err);
	Res = SDReadData(DataPath, MeterDataTemp, sizeof(MeterDataTemp),StartMeterSn*METERDATALEN); 
	debug_info(gDebugModule[METER_DATA],"%s %d StartMeterSn =%d MeterSnMax=%d!\r\n",__FUNCTION__,__LINE__,StartMeterSn,MeterSnMax);
	OSMutexPost (FlashMutex);
	if(Res!=NO_ERR)
    	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
		    debug_err(gDebugModule[METER_DATA],"ERROR: %s %d SDSaveData Error Res = %d !",__FUNCTION__,__LINE__,Res );
			goto ERROR;
    	}
	/*end  :yangfei modified 2012-12-24 find path*/
    	//���Res�����ж�
		DataCounter = 0;
		pDataTemp =  MeterDataTemp;

    	while(DataCounter< (1024/METERDATALEN))
    	{
			MeterFrameLen = METERDATALEN; // ÿ������Ϣ����40�ֽڡ�
			//�˴�Ҫ�жϳ����Ƿ�Ϊ��Ч
        	
    		if( (DataBackLen+MeterFrameLen) > FrameDataFileLenMax )
    			{						
    				SendReadyFalg = 1;
    				break;										//Ԥ��������жϳ����Ƿ񳬹�����
    			}
    		else{
					
				if(StartMeterSn < MeterSnMax)
    			{
    					/*begin :yangfei modified 2013-08-29 for �����һ����ʱ����һ������,��ֹ������ͣ��Ҫ����*/
                    if(StartMeterSn+1==MeterSnMax)
                    {
                        debug_info(gDebugModule[METER_DATA],"StartMeterSn+1==MeterSnMax\r\n");
						*pFirst = 0x00;	
                    }
				    else
					{
					    *pFirst = 0x01;						//����������Ҫ����
					}
					  /*end :yangfei modified 2013-08-29 for �����һ����ʱ����һ������*/	

					memcpy(pTemp, pDataTemp, MeterFrameLen);
    				pTemp 		+= MeterFrameLen;
    				DataBackLen += MeterFrameLen;

					DataCounter++;
    				pDataTemp 	= &MeterDataTemp[DataCounter*METERDATALEN];
    				StartMeterSn++;
    				
    			}
    			else
    			{
    				*pFirst = 0x00;
    				SendReadyFalg = 1;
    				break;
    			}
    		}
    	}
    }
    pMultiFrame->MultiFlag 		= *pFirst;
    pMultiFrame->SendMeterNums 	= StartMeterSn - (pMultiFrame->StartMeterSn);
    
    debug("StartMeterSn =%d  pMultiFrame->StartMeterSn=%d\r\n ",StartMeterSn,pMultiFrame->StartMeterSn);
    
    //����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= DataBackLen;
	_ProData.MsgType		= 0x93;
	ReadDateTime(_ProData.TimeSmybol);
	
	if(DataCounter == 0)			//���û�б��ַ��Ϣ
		{
			_ProData.MsgLength		= 1;
			*_ProData.Data.pDataBack = 0x10;					//�쳣��Ӧ
			goto ERROR;
		}
	else{
		*pUserNum = DataCounter;
	}
	
	return NO_ERR;

ERROR:
	_ProData.MsgType		= 0x93;
	_ProData.MsgLength		= 1;
	*_ProData.Data.pDataBack = 0x10;					//�쳣��Ӧ

	return ERR_1;
}


/********************************************************************************************************
**  �� ��  �� ��: PARA_ProcessMsg_94			    							                       
**	�� ��  �� ��: ָ���޸ļ�����IP�Ͷ˿ںš�                                                            			
**	�� ��  �� ��: _ProtocolType &_ProData   							         			 	       
**	�� ��  �� ��: none											                                       
**  ��   ��   ֵ: none													                               
**	��		  ע: 	 Ϊ�˺�ͨ��ʱ�������ָ����ݣ������Ĳ������кܶ����ò�����                                                                                  																									
*********************************************************************************************************/
uint8 PARA_ProcessMsg_94(_ProtocolType &_ProData)
{	
	uint8 Err = NO_ERR;
	uint8 *pTemp		= _ProData.Data.pData;
	UpCommParaSaveType TermPara;
	StruCSXF *Pstru_csxf;
	CPU_SR			cpu_sr;
		
	uint8 u8IPAddrTemp = 0;

			
	OS_ENTER_CRITICAL();
	TermPara = gPARA_UpPara;
	OS_EXIT_CRITICAL();
			
	Pstru_csxf = (StruCSXF*)pTemp;
		
	//gMethod = Pstru_csxf->Method;
	//gDataSource = Pstru_csxf->DataSource;
	//gPeriod = Pstru_csxf->Period;
	//gu8LogCompressType = Pstru_csxf->u8LogCompressType;
	//gu8LogOpenType = Pstru_csxf->u8LogOpenType;
	//gu8LogReportTime = Pstru_csxf->u8LogReportTime;

	TermPara.WriteFlag = 0xAA55; 
	TermPara.HostIPAddr[0] = Pstru_csxf->u8IPAddr0;
	TermPara.HostIPAddr[1] = Pstru_csxf->u8IPAddr1;	
	TermPara.HostIPAddr[2] = Pstru_csxf->u8IPAddr2;
	TermPara.HostIPAddr[3] = Pstru_csxf->u8IPAddr3;
	u8IPAddrTemp = Pstru_csxf->u8HostPortHigh;
	TermPara.HostPort =  (u8IPAddrTemp<<8)|(Pstru_csxf->u8HostPortLow);
			
	
	OSMutexPend (FlashMutex,0,&Err);	
	Err = SDSaveData("/UP_COMM_PARA", &TermPara, sizeof(UpCommParaSaveType), 0);
    OSMutexPost (FlashMutex);
    
    OS_ENTER_CRITICAL();
	gPARA_UpPara = TermPara;
	OS_EXIT_CRITICAL();
		
	//����վ����Ӧ֡ ��д���ݽṹ��ֻ����д������Ϣ��������Ϣ��Create�����в���
	_ProData.MsgLength		= 0x01;
	_ProData.MsgType		= 0x95;
	ReadDateTime(_ProData.TimeSmybol);
			
	if(Err == NO_ERR){
		*_ProData.Data.pDataBack= 0x01; 				//��ȷ��Ӧ
	}
	else{
		*_ProData.Data.pDataBack= 0x10; 				//�쳣��Ӧ
	}


	gRestartFlag = TRUE;    /*delay 5s reset*/
				
	return Err;
		
 
}

/*****************************************************************************************
**								End of File
*****************************************************************************************/
