/*
  ********************************************************************************************************
  * @file    valve_precise.c
  * @author  zjjin
  * @version V0.0.0
  * @date    06-16-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		��������ͨѶЭ�鷧��������ʵ����ش��롣
  *
  ********************************************************************************************************
  */  

//#include <includes.h>
#include <string.h>
#include "Valve.h"  
#include "valve_precise.h"




/*
  ********************************************************************************************************
  * ��������: uint8 ValveContron_Precise(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
  
  * ˵    ������������ͨѶ ���������ƺ������ڴ˺�����ʵ�����ַ��ز�ͬ���ơ�
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

uint8 ValveContron_Precise(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
{
	uint8 Err = 0;
	uint8 lu8data[100] = {0};
	uint8 lu8databuf[20] = {0xee};
	uint8 lu8datalenback = 0;

	DELU_Protocol	ProtocoalInfo;

	ProtocoalInfo.PreSmybolNum  = 0x04;
 	ProtocoalInfo.MeterType 	= 0x41;
	memcpy(ProtocoalInfo.MeterAddr, p_mf->ValveAddr, 7);
	ProtocoalInfo.SER = 0;

	
	switch(functype){
		case ReadVALVE_All:{	//
			ProtocoalInfo.ControlCode	= 0x01;
			ProtocoalInfo.Length		= 0x03;
			ProtocoalInfo.DataIdentifier= 0x902D;
						
			Err = METER_DataItem(&ProtocoalInfo);
			if(Err == NO_ERR){
				memcpy(lu8data, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length-3));
				
				lu8databuf[0] = lu8data[18];   //��Э���ʽ������˳��
				lu8databuf[1] = lu8data[19];
				lu8databuf[2] = lu8data[20];

				lu8datalenback += 3;

				lu8databuf[3] = 0x99;  //���ſ����ֽ�,�����ڵ������û�з����̶�д��ȫ�ء�

				lu8databuf[4] = 0;//���ո�ʽ���롣
				lu8databuf[5] = 0;

				lu8datalenback += 3;
				
			}
			else{
				memset(lu8databuf, 0xee, 6);								//�¶�3���ֽ�
				lu8datalenback += 6;
			}


			memcpy(p_databuf,lu8databuf,lu8datalenback);
			*p_datalenback = lu8datalenback;
			
			break;
		}

		case SETHEAT_DISPLAY:{
	
			break;
		}

		case SETHEAT_VALUE:{
			ProtocoalInfo.ControlCode 	= 0x04;
			ProtocoalInfo.Length		= 0x08;
			ProtocoalInfo.DataIdentifier= 0xA101;
			memcpy(ProtocoalInfo.DataBuf,p_DataIn,5);
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


			break;
		}

		case SETTEMP_RANGE:{


			break;
		}

		case SETVALVE_STATUS:{


			break;
		}

		default:
			break;
	}



	return Err;
}


