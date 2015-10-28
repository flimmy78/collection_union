/*
  ********************************************************************************************************
  * @file    valve_elsonic.c
  * @author  zjjin
  * @version V0.0.0
  * @date    04-15-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		����Э�鷧��������ʵ����ش��롣
  *
  ********************************************************************************************************
  */  

//#include <includes.h>

#include "Valve.h"  
#include "valve_elsonic.h"


#define HEX_TO_BCD(x) ((x/0x0A)*0x10+(x%0x0A))
#define BCD_TO_HEX(x) ((x/0x10)*0x0A+(x%0x10))



/*
  ********************************************************************************************************
  * ��������: uint8 ValveContron_Elsonic(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
  
  * ˵    �������� ���������ƺ������ڴ˺�����ʵ�����ַ��ز�ͬ���ơ�
  *					
  * ��������� 
  				MeterFileType *p_mf   ����������з���Э��汾�����ص�ַ����Ϣ��
  				uint8 functype  �Է�����ʲô���Ŀ��ƣ��������Ϣ�������������¶ȵȡ�
				uint8 *p_datain  ���뺯������Ҫʹ�õ����ݡ�
  				uint8 *p_databuf    �ӷ����ж����������ݴ洢ָ�롣
  				uint8 p_datalenback  �ӷ����ж����������ݳ��ȡ�
  * �������:
  				ִ���Ƿ�ɹ���
  ********************************************************************************************************
  */

uint8 ValveContron_Elsonic(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
{
	uint8 Err = 0;
	uint8 lu8dataframe[100] = {0};
	uint8 lu8datalen = 0;
	uint8 lu8databuf[20] = {0xee};
	uint8 lu8datalenback = 0;
	uint8 i =0;
	int8 l8tmp = 0;
	uint8 lu8tmp = 0;
	

	
	switch(functype){
		case ReadVALVE_All:{						
			Create_ElsonicVave_ReadInfo_Frame(p_mf,lu8dataframe, &lu8datalen);//�齨��ȡ������������֡
			Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
			if(Err == NO_ERR){	  
				l8tmp = (int8)lu8dataframe[4];  //�����¶�+�����¶ȡ�
				l8tmp += lu8dataframe[8];
				lu8tmp = (uint8)l8tmp;
				lu8databuf[i++] = 0x00;  //С��λ�̶�Ϊ0.
				lu8datalenback++;
				lu8databuf[i++] = HEX_TO_BCD(lu8tmp);
				lu8datalenback++;
				lu8databuf[i++] = 0x00;  //����λ
				lu8datalenback++;
			
				lu8tmp = lu8dataframe[7];
				if(lu8tmp & 0x01)	   //���bit0=1��ʾ���ſ�״̬��0��״̬��
					lu8databuf[i++] = 0x55;  // ȫ��
				else
					lu8databuf[i++] = 0x99;  //ȫ�ء�
								
				lu8datalenback++;
				
				//begin:����״̬λ�ֽڴ���
				lu8databuf[4] = 0; //�Ƚ�״̬�ֽڳ�ʼ��Ϊ0 ��
				lu8tmp = lu8dataframe[7];
				if(lu8tmp & 0x02)  //�����쳣��
					lu8databuf[4] |= 0x01;

				lu8tmp = lu8dataframe[3];
				if((lu8tmp & 0x01) == 0)  //��忪��
					lu8databuf[4] |= 0x02;

				if(lu8tmp & 0x04)  //��������Ƿ�
					lu8databuf[4] |= 0x04;

				if(lu8tmp & 0x02)  //�����Ƿ�������
					lu8databuf[4] |= 0x08;
				if(lu8tmp & 0x80)  //�����Ƿ�������
					lu8databuf[4] |= 0x08;
				//end:����״̬λ�ֽڴ���

				i++;
				lu8datalenback++;
				lu8databuf[i++] = 0x00;  //Ԥ����
				lu8datalenback++;
							
			}				
			else{
				memset(lu8databuf, 0xee, 6);
				lu8datalenback += 6;								
				debug_err(gDebugModule[TASKDOWN_MODULE],"%s %d Read Valve state failed!\r\n",__FUNCTION__,__LINE__);
			}

			memcpy(p_databuf,lu8databuf,lu8datalenback);
			*p_datalenback = lu8datalenback;
			
			break;
		}

		case SETHEAT_DISPLAY:{
			Create_ElsonicVave_SetInfo_Frame(p_mf,lu8dataframe, &lu8datalen);	//��������������ʾ��		  
			Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);
			if(Err != NO_ERR)  {
				debugX(LOG_LEVEL_ERROR,"%s %d Send HeatMeter data  to valve  failed!\r\n",__FUNCTION__,__LINE__);
			}

			memcpy(p_databuf,lu8dataframe,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETHEAT_VALUE:{
			Create_ElsonicVave_SetHeat_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
			Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);
			if(Err == NO_ERR){
				debug_info(gDebugModule[TASKDOWN_MODULE],"%s %d Send HeatMeter data  to valve ok",__FUNCTION__,__LINE__);
			}
			else  {
				debugX(LOG_LEVEL_ERROR,"%s %d Send HeatMeter data  to valve  failed!\r\n",__FUNCTION__,__LINE__);
			}

			memcpy(p_databuf,lu8dataframe,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETROOM_TEMP:{
			Create_ElsonicVave_SetRoomTemp_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
			Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
			if(Err == NO_ERR){
					debug_info(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature ok ",__FUNCTION__);
				}
			else{
					debug_err(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature failed ",__FUNCTION__);
			}
			
			memcpy(p_databuf,lu8dataframe,lu8datalen);
			*p_datalenback = lu8datalen;


			break;
		}

		case SETTEMP_RANGE:{
			Create_ElsonicVave_SetRoomTempRange_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
			Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
			if(Err == NO_ERR)	{
					debug_info(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature ok ",__FUNCTION__);
				}
			else{
					debug_err(gDebugModule[TASKDOWN_MODULE],"%s Set indoor given temperature failed ",__FUNCTION__);
			}

			memcpy(p_databuf,lu8dataframe,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETVALVE_STATUS:{
			Create_ElsonicVave_SetValve_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
			Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);

			memcpy(p_databuf,lu8dataframe,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETVALVE_CONTROLTYPE:{
			if(*(p_DataIn+0) == 0x09){ //���ַ�ֻ������ʹ�ܷ��������¿�������ʱ��ִ�С�
				Create_ElsonicVave_SetValve_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
				Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);

				memcpy(p_databuf,lu8dataframe,lu8datalen);
				*p_datalenback = lu8datalen;
			}

			break;
		}
		

		default:
			break;
	}



	return Err;
}


  
   /*
	 ******************************************************************************
	 * �������ƣ�Create_ElsonicVave_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺������ڶ�ȡ���ַ�����״̬��Ϣ��
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicVave_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
   {
		   uint8 setInform[8]={0xa0,0x19,0x0c,0x00,0x00,0x00,0x00,0x7b};
		   uint8 cs 	   = 0x00;
		   uint8 *pTemp;
		   uint8 i =0;
		   uint8 lu8valveaddr[7] = {0x00};
		   //uint16 lu16valveaddr =0;
			   
		   LOG_assert_param(p_mf == NULL);
		   LOG_assert_param(pSendFrame == NULL);
		   LOG_assert_param(plenFrame == NULL);
			   
		   pTemp = pSendFrame;
		   
		   memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);			   
		   //for(i=0;i<7;i++){
		   //  lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
		   //}
		   //lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1]*100;
			   
		   //setInform[1] = lu16valveaddr & 0x00ff;
		   //setInform[2] = (lu16valveaddr>>8) & 0x00ff;
		   setInform[1] = lu8valveaddr[0];
		   setInform[2] = lu8valveaddr[1];
		   
		   cs = 0;
		   for(i=0;i<7;i++){
			   cs += setInform[i];
		   }
			   cs ^= 0xA5;
			   setInform[7] = cs;
		   
		   
		   memcpy(pTemp, &setInform[0], 8);
		   *plenFrame = 8;
			   
		   
   }
   
   
   /*
	 ******************************************************************************
	 * �������ƣ�Create_ElsonicVave_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺������ڽ��������ַ�����ʹ��������ʾ���ܡ�
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicVave_SetInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
   {
		   uint8 setInform[8]={0xb1,0x19,0x0c,0x08,0x00,0x00,0x00,0x7b};
		   //uint8 templen   = 0x00;
		   uint8 cs 	   = 0x00;
		   uint8 *pTemp;
		   uint8 i =0;
		   uint8 lu8valveaddr[7] = {0x00};
		   //uint16 lu16valveaddr =0;
			   
		   LOG_assert_param(p_mf == NULL);
		   LOG_assert_param(pSendFrame == NULL);
		   LOG_assert_param(plenFrame == NULL);
			   
		   pTemp = pSendFrame;
		   
		   memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);		   
		   //for(i=0;i<7;i++){
		   //  lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
		   //}
		   //lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1]*100;
			   
		   //setInform[1] = lu16valveaddr & 0x00ff;
		   //setInform[2] = (lu16valveaddr>>8) & 0x00ff;
   
   
		   setInform[1] = lu8valveaddr[0];
		   setInform[2] = lu8valveaddr[1];
		   setInform[3] = 0x08; // ����������ʾ���ܡ�
		   
		   cs = 0;
		   for(i=0;i<7;i++){
			   cs += setInform[i];
		   }
			   cs ^= 0xA5;
			   setInform[7] = cs;
		   
		   
		   memcpy(pTemp, &setInform[0], 8);
		   *plenFrame = 8;
			   
		   
   }
  
   
   
   
   /*
	 ******************************************************************************
	 * �������ƣ�Create_ElsonicVave_SetHeat_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺������ڽ�����ֵд�����ַ�������
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicVave_SetHeat_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {
		   uint8 setInform[8]={0xa4,0x19,0x0c,0x08,0x00,0x00,0x00,0x7b};
		   //uint8 templen   = 0x00;
		   uint8 cs 	   = 0x00;
		   uint8 *pTemp;
//		   uint16 lenFrame = 0x00;
//		   uint16 CheckLen = 0x00;
		   uint8 i =0;
		   uint8 lu8valveaddr[7] = {0x00};
//		   uint16 lu16valveaddr =0;
		   uint8 lu8currentheat[4] = {0x00};
//		   uint32 lu32currentheat =0;
			   
		   LOG_assert_param(p_mf == NULL);
		   LOG_assert_param(pSendFrame == NULL);
		   LOG_assert_param(plenFrame == NULL);
			   
		   pTemp = pSendFrame;
		   
		    memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);			   
		   //for(i=0;i<7;i++){
		   //  lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
		   //}
		   //lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1]*100;
			   
		   //setInform[1] = lu16valveaddr & 0x00ff;
		   //setInform[2] = (lu16valveaddr>>8) & 0x00ff;
   
		   setInform[1] = lu8valveaddr[0];
		   setInform[2] = lu8valveaddr[1];
   
		   memcpy(lu8currentheat,p_DataIn, 4);			   //��ǰ����ȡ����
		   setInform[3] = lu8currentheat[1];
		   setInform[4] = lu8currentheat[2];
		   setInform[5] = lu8currentheat[3];
		   setInform[6] = 0x00;  
		   
		   cs = 0;
		   for(i=0;i<7;i++){
			   cs += setInform[i];
		   }
			   cs ^= 0xA5;
			   setInform[7] = cs;
		   
		   
		   memcpy(pTemp, &setInform[0], 8);
		   *plenFrame = 8;
			   
		   
   }
   
   /*
	 ******************************************************************************
	 * �������ƣ�Create_ElsonicVave_SetRoomTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺����������������¶ȡ�
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicVave_SetRoomTemp_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {
		   uint8 setInform[8]={0xa6,0x19,0x0c,0x00,0x00,0x32,0x00,0x7b};
//		   uint8 templen   = 0x00;
		   uint8 cs 	   = 0x00;
		   uint8 *pTemp;
//		   uint16 lenFrame = 0x00;
//		   uint16 CheckLen = 0x00;
		   uint8 i =0;
		   uint8 lu8valveaddr[7] = {0x00};
//		   uint16 lu16valveaddr =0;
//		   uint8 lu8currentheat[4] = {0x00};
//		   uint32 lu32currentheat =0;
			   
		   LOG_assert_param(p_mf == NULL);
		   LOG_assert_param(pSendFrame == NULL);
		   LOG_assert_param(plenFrame == NULL);
			   
		   pTemp = pSendFrame;
		   
		   memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);			   
		   //for(i=0;i<7;i++){
		   //  lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
		   //}
		   //lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1]*100;
			   
		   //setInform[1] = lu16valveaddr & 0x00ff;
		   //setInform[2] = (lu16valveaddr>>8) & 0x00ff;
   
		   setInform[1] = lu8valveaddr[0];
		   setInform[2] = lu8valveaddr[1];
   
   
		   setInform[5] = BCD_TO_HEX(*(p_DataIn + 1));  //�趨�¶�,�˴���Ҫ������ȷƫ����
		   if(setInform[5] < 10)	//�¶��趨��Χ10-30�档
			   setInform[5] = 10;
		   if(setInform[5] > 30)
			   setInform[5] = 30;
		   
		   setInform[5] = 2 * setInform[5]; 	 //  �趨�����¶ȣ��趨ֵ���¶�ֵ��2����
				   
		   cs = 0;
		   for(i=0;i<7;i++){
			   cs += setInform[i];
		   }
			   cs ^= 0xA5;
			   setInform[7] = cs;
		   
		   
		   memcpy(pTemp, &setInform[0], 8);
		   *plenFrame = 8;
			   
		   
   }
   
   /*
	 ******************************************************************************
	 * �������ƣ�Create_ElsonicVave_SetRoomTempRange_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺����������������¶������ޡ�
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicVave_SetRoomTempRange_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {
		   uint8 setInform[8]={0xb0,0x19,0x0c,0xa5,0x3c,0xa5,0x26,0x7b};
//		   uint8 templen   = 0x00;
		   uint8 cs 	   = 0x00;
		   uint8 *pTemp;
//		   uint16 lenFrame = 0x00;
//		   uint16 CheckLen = 0x00;
		   uint8 i =0;
		   uint8 lu8valveaddr[7] = {0x00};
//		   uint16 lu16valveaddr =0;
//		   uint8 lu8currentheat[4] = {0x00};
//		   uint32 lu32currentheat =0;
			   
		   LOG_assert_param(p_mf == NULL);
		   LOG_assert_param(pSendFrame == NULL);
		   LOG_assert_param(plenFrame == NULL);
			   
		   pTemp = pSendFrame;
		   
		    memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);			   
		   //for(i=0;i<7;i++){
		   //  lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
		   //}
		   //lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1]*100;
			   
		   //setInform[1] = lu16valveaddr & 0x00ff;
		   //setInform[2] = (lu16valveaddr>>8) & 0x00ff;
   
		   setInform[1] = lu8valveaddr[0];
		   setInform[2] = lu8valveaddr[1];
   
   
		   setInform[4] = BCD_TO_HEX(*(p_DataIn+1));  //�趨�¶�����ֵ��
		   if(setInform[4] < 20)	//���޷�Χ20-30�档
			   setInform[4] = 20;
		   if(setInform[4] > 30)	//���޷�Χ20-30�档
			   setInform[4] = 30;
   
		   setInform[4] = 2 * setInform[4]; 	 //  �趨ֵ���¶�ֵ��2����
   
		   setInform[6] = BCD_TO_HEX(*(p_DataIn+4));  //�趨�¶�����ֵ��
		   if(setInform[6] < 10)	//���޷�Χ10-19�档
			   setInform[6] = 10;
		   if(setInform[6] > 19)   
			   setInform[6] = 19;
   
		   setInform[6] = 2 * setInform[6]; 	 //  �趨ֵ���¶�ֵ��2����
			   
		   cs = 0;
		   for(i=0;i<7;i++){
			   cs += setInform[i];
		   }
			   cs ^= 0xA5;
			   setInform[7] = cs;
		   
		   
		   memcpy(pTemp, &setInform[0], 8);
		   *plenFrame = 8;
			   
		   
   }
		   
   
   /*
	 ******************************************************************************
	 * �������ƣ�Create_ElsonicVave_SetValve_Frame(DELU_Protocol *pData, uint8 valvestate,uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺�������ǿ�ƿ��ط���
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicVave_SetValve_Frame(MeterFileType *p_mf, uint8 *p_DataIn,uint8 *pSendFrame, uint8 *plenFrame)
   {
		   uint8 setInform[8]={0xa9,0x19,0x0c,0x00,0x00,0x00,0x00,0x7b};
//		   uint8 templen   = 0x00;
		   uint8 cs 	   = 0x00;
		   uint8 *pTemp;
//		   uint16 lenFrame = 0x00;
//		   uint16 CheckLen = 0x00;
		   uint8 i =0;
		   uint8 lu8valveaddr[7] = {0x00};
//		   uint16 lu16valveaddr =0;
//		   uint8 lu8currentheat[4] = {0x00};
//		   uint32 lu32currentheat =0;
			   
		   LOG_assert_param(p_mf == NULL);
		   LOG_assert_param(pSendFrame == NULL);
		   LOG_assert_param(plenFrame == NULL);
			   
		   pTemp = pSendFrame;
		   
		   memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);			   
		   //for(i=0;i<7;i++){
		   //  lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
		   //}
		   //lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1]*100;
			   
		   //setInform[1] = lu16valveaddr & 0x00ff;
		   //setInform[2] = (lu16valveaddr>>8) & 0x00ff;
   
		   setInform[1] = lu8valveaddr[0];
		   setInform[2] = lu8valveaddr[1];
   
		   if(*p_DataIn == 0x99)
			   setInform[3] = 0x80;
		   else if(*p_DataIn == 0x55)
			   setInform[3] = 0x02;
		   else if(*p_DataIn == 0x09)  //�������������������¿������ơ�
		   	 	setInform[3] = 0;
		   else
			   setInform[3] = 0x02;//����Ҳȫ���򿪷��ţ���ȫ��
   
		   cs = 0;
		   for(i=0;i<7;i++){
			   cs += setInform[i];
		   }
			   cs ^= 0xA5;
			   setInform[7] = cs;
		   
		   
		   memcpy(pTemp, &setInform[0], 8);
		   *plenFrame = 8;
			   
		   
   }
   
   
   
   uint8 Elsonic_ReadData_Communicate(uint8 *DataFrame, uint8 *DataLen)
   {
   
		uint8 err;
		//uint8 RetryTimes	 = 0x02;
//		uint8 data=0;
		uint8 dev = DOWN_COMM_DEV_MBUS;
//		uint8 i=0; 
		uint8 len=0;
		 
		uint8 DataBuf[50];
		LOG_assert_param(DataFrame == NULL);
		LOG_assert_param(DataLen == NULL);
	   
	   //for(i=0;i<RetryTimes;i++){
		   DuQueueFlush(dev);				 //��ջ�����	 
		   DuSend(dev, (uint8*)DataFrame,  *DataLen);
		   
		   //OSTimeDly(OS_TICKS_PER_SEC/10);
		   
		   err = Receive_Read_ElsonicParamFrame(dev, DataBuf, 0, &len);
   
		   if(err==NO_ERR){
				memcpy(DataFrame, &DataBuf[0], len);
				*DataLen = len;
				return NO_ERR;
			}
		   	//else{
			//	OSTimeDlyHMSM(0,0,0,500);    //������ɹ��򲹳������500ms��
			//}
			 
	  // }		   
			
		return 1;
   }
   
   
   uint8 Receive_Read_ElsonicParamFrame(uint8 dev, uint8 *buf, uint16 Out_Time, uint8 *datalen)
   {
	   uint8 data=0;
	   uint8 i=0;
	   uint8 readinfom[32]={0};
//	   uint8 len = 0;
//	   uint8 len1 = 0;
	   uint8 Cs=0;
	   
	   i = 10;
   
	   while(i--){												   //��֡ͷ
			   if(DuGetch(dev, &data, 3*OS_TICKS_PER_SEC))
				   {return 1;}
			   
			   if(data == 0x50) //�ҵ�֡ͷ 
				   {break; }
			   
	   }
   
	   readinfom[0] = 0x50;
	   Cs += readinfom[0];
   
	   for(i=1;i<11;i++){
		   if(DuGetch(dev, &data, 3*OS_TICKS_PER_SEC)){
			   return 2;
		   }
		   readinfom[i] = data;
		   Cs  += data;
	   }
   
	   Cs ^= 0xa5;
   
	   if(DuGetch(dev, &data, 3*OS_TICKS_PER_SEC)){
			   return 3;
	   }
	   readinfom[11] = data;
   
	   if(Cs != readinfom[11]){
			   return 4;
	   }
   
	   *datalen = 12;
	   memcpy(buf, &readinfom[0], *datalen);
   
	   return 0;
	   
		   
   }   

