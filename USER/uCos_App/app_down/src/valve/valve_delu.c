/*
  ********************************************************************************************************
  * @file    valve_delu.c
  * @author  zjjin
  * @version V0.0.0
  * @date    06-13-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		��³Э�鷧��������ʵ����ش��롣
  *
  ********************************************************************************************************
  */  

//#include <includes.h>
#include <string.h>
#include "Valve.h"  
#include "valve_delu.h"




/*
  ********************************************************************************************************
  * ��������: uint8 ValveContron_Delu(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
  
  * ˵    ������³ ���������ƺ������ڴ˺�����ʵ�����ַ��ز�ͬ���ơ�
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

uint8 ValveContron_Delu(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
{
	uint8 Err = 0;
	uint8 lu8data[100] = {0};
	uint8 lu8databuf[20] = {0xee};
	uint8 lu8datalenback = 0;

	CJ188_Format  lMeterData;

	DELU_Protocol	ProtocoalInfo;

	ProtocoalInfo.PreSmybolNum  = 0x04;
 	ProtocoalInfo.MeterType 	= 0x20;
	memcpy(ProtocoalInfo.MeterAddr, p_mf->ValveAddr, 7);
	ProtocoalInfo.ControlCode 	= 0x04;
	ProtocoalInfo.SER = 0;

	
	switch(functype){
		case ReadVALVE_All:{	//�Ե�³Э����˵�������������¶Ⱥͷ���״̬2���档
			ProtocoalInfo.Length		= 0x0B;
			ProtocoalInfo.DataIdentifier= 0x20A0;
			memcpy(ProtocoalInfo.DataBuf, p_mf->ControlPanelAddr, 7);
			ProtocoalInfo.DataBuf[7]	= 0x11;
						
			Err = METER_DataItem(&ProtocoalInfo);
			if(Err == NO_ERR){
				memcpy(lu8data, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length-3));
				
				if(lu8data[0] == 0xff){
					memset(lu8databuf, 0xee, 3);
				}
				else{
					memcpy(lu8databuf,lu8data,3);
				}

				lu8datalenback += 3;
				
			}
			else{
				memset(lu8databuf, 0xee, 3);								//�¶�3���ֽ�
				lu8datalenback += 3;
			}

			
 			//��ȡ����״̬!!
 			ProtocoalInfo.PreSmybolNum  = 0x04;   //��������������¶ȣ�ProtocoalInfo����һЩ���ı��ˣ������ظ�ֵ��
 			ProtocoalInfo.MeterType 	= 0x20;
			memcpy(ProtocoalInfo.MeterAddr, p_mf->ValveAddr, 7);
			ProtocoalInfo.ControlCode 	= 0x04;
			ProtocoalInfo.SER = 0;
			ProtocoalInfo.Length		= 0x04;
			ProtocoalInfo.DataIdentifier= 0x24A0;
			ProtocoalInfo.DataBuf[0]	= 0x11;
			
			Err = METER_DataItem(&ProtocoalInfo);
			if(Err == NO_ERR){     
					uint16  ValveState = 0;
					ValveState = *(uint16*)ProtocoalInfo.DataBuf;
					if(ValveState == 0x0800||ValveState == 0x9900){
						ValveState = 0x0099;
					}/*ȫ��*/
					else if(ValveState == 0x0400||ValveState == 0x8800){ValveState = 0x0088;}
					else if(ValveState == 0x0200||ValveState == 0x7700){ValveState = 0x0077;}
					else if(ValveState == 0x0100||ValveState == 0x6600){ValveState = 0x0066;}
					else if(ValveState == 0x0000||ValveState == 0x5500){
						ValveState = 0x0055;
					}/*ȫ��*/
					else{
						ValveState = 0x0099;
					}

					lu8databuf[3] = (uint8)ValveState;  //���ſ����ֽ�

					lu8databuf[4] = 0;//���ո�ʽ���롣
					lu8databuf[5] = 0;

					lu8datalenback += 3;
					
				}
			else{
				memset(&lu8databuf[3], 0xee, 3);								//�¶�3���ֽ�
				lu8datalenback += 3;
			}

			memcpy(p_databuf,lu8databuf,lu8datalenback);
			*p_datalenback = lu8datalenback;
			
			break;
		}

		case READROOM_TEMP:{
			ProtocoalInfo.Length		= 0x0B;
			ProtocoalInfo.DataIdentifier= 0x20A0;
			memcpy(ProtocoalInfo.DataBuf, p_mf->ControlPanelAddr, 7);
			ProtocoalInfo.DataBuf[7]	= 0x11;
						
			Err = METER_DataItem(&ProtocoalInfo);
			if(Err == NO_ERR){
				memcpy(lu8data, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length-3));
				
				if(lu8data[0] == 0xff){
					memset(lu8databuf, 0xee, 3);
				}
				else{
					lu8databuf[0] = lu8data[2];   //��Э���ʽ������˳��
					lu8databuf[1] = lu8data[1];
					lu8databuf[2] = lu8data[0];

				}

				lu8datalenback += 3;
				
			}
			else{
				memset(lu8databuf, 0xee, 3);								//�¶�3���ֽ�
				lu8datalenback += 3;
			}

			memcpy(p_databuf,lu8databuf,lu8datalenback);
			*p_datalenback = lu8datalenback;

			break;
		}

		case SETHEAT_DISPLAY:{
	
			break;
		}

		case SETHEAT_VALUE:{
			ProtocoalInfo.Length		= 0x2E;
			ProtocoalInfo.DataIdentifier= 0x25A0;
			memcpy(ProtocoalInfo.DataBuf,p_DataIn,sizeof(lMeterData));
			Err = METER_DataItem(&ProtocoalInfo);		
			if(Err == NO_ERR){
				debug_info(gDebugModule[TASKDOWN_MODULE],"%s %d Send HeatMeter data  to valve ok",__FUNCTION__,__LINE__);
			}
			else  {
				debugX(LOG_LEVEL_ERROR,"%s %d Send HeatMeter data  to valve  failed!\r\n",__FUNCTION__,__LINE__);
			}


			break;
		}

		case SETROOM_TEMP:{
			ProtocoalInfo.Length			= 0x0D;
			ProtocoalInfo.DataIdentifier	= 0x23A0;
			memcpy(ProtocoalInfo.DataBuf, p_mf->ControlPanelAddr, 7);
			//memcpy(&ProtocoalInfo.DataBuf[7], p_DataIn, 3);
			ProtocoalInfo.DataBuf[7] = *(p_DataIn + 2);   //���յ�³����Э�����˳��
			ProtocoalInfo.DataBuf[8] = *(p_DataIn + 1);
			ProtocoalInfo.DataBuf[9] = *(p_DataIn + 0);
			
			Err = METER_DataItem(&ProtocoalInfo);
			if(Err == NO_ERR){
				memcpy(lu8data, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length-3));
				lu8datalenback = ProtocoalInfo.Length-3;
			
			}
			
			memcpy(p_databuf,lu8data,lu8datalenback);
			*p_datalenback = lu8datalenback;

			break;
		}

		case SETTEMP_RANGE:{
			ProtocoalInfo.Length			= 0x10;
			ProtocoalInfo.DataIdentifier	= 0x21A0;
			memcpy(ProtocoalInfo.DataBuf, p_mf->ControlPanelAddr, 7);
			//memcpy(&ProtocoalInfo.DataBuf[7], p_DataIn, 6);
			ProtocoalInfo.DataBuf[7] = *(p_DataIn + 2);   //���յ�³����Э�����˳��
			ProtocoalInfo.DataBuf[8] = *(p_DataIn + 1);
			ProtocoalInfo.DataBuf[9] = *(p_DataIn + 0);
			ProtocoalInfo.DataBuf[10] = *(p_DataIn + 5);   //���յ�³����Э�����˳��
			ProtocoalInfo.DataBuf[11] = *(p_DataIn + 4);
			ProtocoalInfo.DataBuf[12] = *(p_DataIn + 3);
			
			Err = METER_DataItem(&ProtocoalInfo);
			if(Err == NO_ERR){
				memcpy(lu8data, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length-3));
				lu8datalenback = ProtocoalInfo.Length-3;
			
			}
			
			memcpy(p_databuf,lu8data,lu8datalenback);
			*p_datalenback = lu8datalenback;



			break;
		}

		case SETVALVE_STATUS:{
			ProtocoalInfo.Length		= 0x04;
			ProtocoalInfo.DataIdentifier= 0x17A0;
			ProtocoalInfo.DataBuf[0]	= *(p_DataIn + 0);
			
			Err = METER_DataItem(&ProtocoalInfo);
			if(Err == NO_ERR){
				memcpy(lu8data, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length-3));
				lu8datalenback = ProtocoalInfo.Length-3;
			
			}
			
			memcpy(p_databuf,lu8data,lu8datalenback);
			*p_datalenback = lu8datalenback;



			break;
		}

		case SETVALVE_CONTROLTYPE:{
			ProtocoalInfo.Length			= 0x0B;
			ProtocoalInfo.DataIdentifier	= 0x22A0;
			memcpy(ProtocoalInfo.DataBuf, p_mf->ControlPanelAddr, 7);
			if(*(p_DataIn+0) == 0x09)
				{
					ProtocoalInfo.DataBuf[7]		= 0x55;
				}
			if(*(p_DataIn+0) == 0x0B)
				{
					ProtocoalInfo.DataBuf[7]		= 0x66;
				}
			if(*(p_DataIn+0) == 0x0C)
				{
					ProtocoalInfo.DataBuf[7]		= 0x77;
				}
			if(*(p_DataIn+0) == 0x0D)
				{
					ProtocoalInfo.DataBuf[7]		= 0x88;
				}
			
			Err = METER_DataItem(&ProtocoalInfo);
			if(Err == NO_ERR){
				memcpy(lu8data, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length-3));
				lu8datalenback = ProtocoalInfo.Length-3;
			
			}
			
			memcpy(p_databuf,lu8data,lu8datalenback);
			*p_datalenback = lu8datalenback;


			break;
		}

		case READVALVE_STATUS:{
			//��ȡ����״̬!!
			ProtocoalInfo.Length		= 0x04;
			ProtocoalInfo.DataIdentifier= 0x24A0;
			ProtocoalInfo.DataBuf[0]	= 0x11;
			
			Err = METER_DataItem(&ProtocoalInfo);
			if(Err == NO_ERR){     
					uint16  ValveState = 0;
					ValveState = *(uint16*)ProtocoalInfo.DataBuf;
					if(ValveState == 0x0800||ValveState == 0x9900){
						ValveState = 0x0099;
					}/*ȫ��*/
					else if(ValveState == 0x0400||ValveState == 0x8800){ValveState = 0x0088;}
					else if(ValveState == 0x0200||ValveState == 0x7700){ValveState = 0x0077;}
					else if(ValveState == 0x0100||ValveState == 0x6600){ValveState = 0x0066;}
					else if(ValveState == 0x0000||ValveState == 0x5500){
						ValveState = 0x0055;
					}/*ȫ��*/
					else{
						ValveState = 0x0099;
					}

					lu8databuf[0] = (uint8)ValveState;  //���ſ����ֽ�
					lu8datalenback += 1;
					
			}

			memcpy(p_databuf,lu8databuf,lu8datalenback);
			*p_datalenback = lu8datalenback;

			break;
		}
			

		default:
			break;
	}



	return Err;
}


