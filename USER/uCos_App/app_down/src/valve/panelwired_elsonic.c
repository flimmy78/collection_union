/*
  ********************************************************************************************************
  * @file    panelwired_elsonic.c
  * @author  zjjin
  * @version V0.0.0
  * @date    09-29-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		���������¿����Э�鹦��ʵ����ش��롣
  *
  ********************************************************************************************************
  */  

//#include <includes.h>

#include "Valve.h"  
#include "panelwired_elsonic.h"
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

uint8 PanelWiredControl_Elsonic(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
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
			Create_ElsonicWiredPanel_ReadInfo_Frame(p_mf,lu8dataframe, &lu8datalen);//�齨��ȡ������������֡
			Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
			if(Err == NO_ERR){	  
				l8tmp = (int8)lu8dataframe[5];  //�����¶�+�����¶ȡ�
				l8tmp += lu8dataframe[9];
				lu8tmp = (uint8)l8tmp;
				lu8databuf[i++] = 0x00;  //С��λ�̶�Ϊ0.
				lu8datalenback++;
				lu8databuf[i++] = HEX_TO_BCD(lu8tmp);
				lu8datalenback++;
				lu8databuf[i++] = 0x00;  //����λ
				lu8datalenback++;
			
				lu8tmp = lu8dataframe[4];
				if(lu8tmp & 0x04)	   //���bit0=1��ʾ���ſ�״̬��0��״̬��
					lu8databuf[i++] = 0x55;  // ȫ��
				else
					lu8databuf[i++] = 0x99;  //ȫ�ء�
								
				lu8datalenback++;
				
				//begin:����״̬λ�ֽڴ���
				lu8databuf[4] = 0; //�Ƚ�״̬�ֽڳ�ʼ��Ϊ0 ��

				lu8tmp = lu8dataframe[3];
				if((lu8tmp & 0x10) == 0)  //��忪��
					lu8databuf[4] |= 0x02;

				lu8tmp = lu8dataframe[4];
				if(lu8tmp & 0x02)  //��������Ƿ�
					lu8databuf[4] |= 0x04;

				lu8tmp = lu8dataframe[3];
				if(lu8tmp & 0x08)  //�����Ƿ�������
					lu8databuf[4] |= 0x08;
				if(lu8tmp & 0x04)  //�����Ƿ�������
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

			break;
		}

		case SETHEAT_VALUE:{

			break;
		}

		case SETROOM_TEMP:{
			Create_ElsonicWiredPanel_SetRoomTemp_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
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

			break;
		}

		case SETVALVE_STATUS:{
			Create_ElsonicWiredPanel_SetValve_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
			Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);

			memcpy(p_databuf,lu8dataframe,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETVALVE_CONTROLTYPE:{
			if(*(p_DataIn+0) == 0x09){ //���ַ�ֻ������ʹ�ܷ��������¿�������ʱ��ִ�С�
				Create_ElsonicWiredPanel_SetValve_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
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
	 * �������ƣ�Create_ElsonicWiredPanel_ReadInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺������ڶ�ȡ���������¿����״̬��Ϣ��
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicWiredPanel_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
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
	 * �������ƣ�Create_ElsonicWiredPanel_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺������ڽ����������������ʹ��������ʾ���ܡ�
	 				Ŀǰ���������¿����û�д˹��ܡ�
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicWiredPanel_SetInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
   {
			   
		   
   }
  
   
   
   
   /*
	 ******************************************************************************
	 * �������ƣ�Create_ElsonicWiredPanel_SetHeat_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺������ڽ�����ֵд�����������¿���塣
	 				Ŀǰ���������¿����û�д˹��ܡ�
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicWiredPanel_SetHeat_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {
			   
		   
   }
   
   /*
	 ******************************************************************************
	 * �������ƣ�Create_ElsonicWiredPanel_SetRoomTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺��������������������¿���������¶ȡ�
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicWiredPanel_SetRoomTemp_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {
		   uint8 setInform[8]={0xa9,0x19,0x0c,0x00,0x00,0x00,0x00,0x7b};
		   uint8 cs 	   = 0x00;
		   uint8 *pTemp;
		   uint8 i =0;
		   uint8 lu8valveaddr[7] = {0x00};
			   
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
   
   
		   setInform[6] = BCD_TO_HEX(*(p_DataIn + 1));  //�趨�¶�,�˴���Ҫ������ȷƫ����
		   if(setInform[6] < 5)	//�¶��趨��Χ5-35�档
			   setInform[6] = 5;
		   if(setInform[6] > 35)
			   setInform[6] = 35;
				   
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
	 * �������ƣ�Create_ElsonicWiredPanel_SetRoomTempRange_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺��������������������¿���������¶������ޡ�
	 				Ŀǰ���������¿����û�д˹��ܡ�
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicWiredPanel_SetRoomTempRange_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {
			   
		   
   }
		   
   
   /*
	 ******************************************************************************
	 * �������ƣ�Create_ElsonicWiredPanel_SetValve_Frame(DELU_Protocol *pData, uint8 valvestate,uint8 *pSendFrame, uint8 *plenFrame)
	 * ˵	 �����˺�������ǿ�����������¿���忪�ط���
	 * ��	 ���� 
	 ******************************************************************************
	 */
   void Create_ElsonicWiredPanel_SetValve_Frame(MeterFileType *p_mf, uint8 *p_DataIn,uint8 *pSendFrame, uint8 *plenFrame)
   {
		   uint8 setInform[8]={0xa3,0x19,0x0c,0x00,0x00,0x00,0x00,0x7b};
		   uint8 cs 	   = 0x00;
		   uint8 *pTemp;
		   uint8 i =0;
		   uint8 lu8valveaddr[7] = {0x00};
			   
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
			   setInform[3] = 0x08;  //Э�������Դ�Ϊ׼��
		   else if(*p_DataIn == 0x55)
			   setInform[3] = 0x04;
		   else if(*p_DataIn == 0x09)  //�������������������¿������ơ�
		   	 	setInform[3] = 0;
		   else
			   setInform[3] = 0x04;//����Ҳȫ���򿪷��ţ���ȫ��
   
		   cs = 0;
		   for(i=0;i<7;i++){
			   cs += setInform[i];
		   }
			   cs ^= 0xA5;
			   setInform[7] = cs;
		   
		   
		   memcpy(pTemp, &setInform[0], 8);
		   *plenFrame = 8;
			   
		   
   }
   
   
