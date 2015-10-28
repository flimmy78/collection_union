/*
  ********************************************************************************************************
  * @file    valve_ADE.c
  * @author  zjjin
  * @version V0.0.0
  * @date    08-10-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		�����³Э�鷧��������ʵ����ش��룬�����ԭ��³����Э����Щ����
  		Ϊ����ԭ���ݣ���ӱ��ļ���
  *
  ********************************************************************************************************
  */  

//#include <includes.h>
#include <string.h>
#include "Valve.h"  
#include "valve_ADE.h"


/*
**************************************************************************************************
uint16 gADEVALVE_Table[][2],�����³������Э�飬���������á�
						gADEVALVE_Table[][0] --- ���ع��ܱ�ʶ�š�
						gADEVALVE_Table[][1] ---ָ�����ݳ��ȡ�

**************************************************************************************************
*/
uint16 gADEVALVE_Table[][2] = {
		0x17A0, 0x04,		//0,�·����Ʒ���������.
		0x25A0, 0x2E,		//1,�·��������ݵ���������
		0x20A0, 0x0B,		//2,�������¶ȡ�
		0x21A0, 0x10,		//3,���������������¶ȡ�
		0x22A0, 0x0B,		//4,���÷��ſ��Ʒ�ʽ��ʹ�ܡ����ܡ��Զ�����ʱ�ȡ�
		0x23A0, 0x0D,   	//5�����������¶ȡ�
		0x24A0, 0x04,   	//6��������״̬��
		0x27A0, 0x0D,   	//7�����ò����¶�
		0x28A0, 0x0B,   	//8�����÷����������²�
		0x29A0, 0x0B,   	//9�������¿�����¶Ȳɼ����ڡ�
		0x30A0, 0x0B,   	//10�������¿�����¶��ϱ����ڡ�
		0x31A0, 0x0B,   	//11����ȡ���������ݣ���������ʱ�䡢����ˮ�¶ȡ������¶ȡ�
		0x32A0, 0x16,   	//12���·�д���̯����ֵ��
		
		
};




/*
  ********************************************************************************************************
  * ��������: uint8 ValveContron_ADE(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
  
  * ˵    ���������³���������ƺ������ڴ˺�����ʵ�����ַ��ز�ͬ���ơ�
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

uint8 ValveContron_ADE(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
{
	uint8 Err = 0;
	uint8 lu8DataIn[50] = {0};  //ע�ⲻҪ���������
	uint8 lu8databuf[50] = {0};	//ע�ⲻҪ���������
	uint8 lu8datalen = 0;
	uint8 lu8BackDataBuf[50] = {0};	//ע�ⲻҪ���������
	uint8 lu8datalenback = 0;
	uint8 lu8ReadTime[6] = {0};
	CJ188_Format *p_CJ188Data = NULL;

	switch(functype){
		case ReadVALVE_All:{	//�Ե�³Э����˵�������������¶Ⱥͷ���״̬2���档
			//�ȶ������¶ȡ�
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			lu8DataIn[7] = 0x11;
			Err = ADEValveCommunicate(p_mf,ADE_READROOM_TEMP,lu8DataIn,lu8databuf,&lu8datalen);

			if(Err == NO_ERR){
				memcpy(lu8BackDataBuf,lu8databuf,3);  //�����¶����ݡ�
				lu8datalenback += 3;
				
			}
			else{
				memset(lu8BackDataBuf, 0xee, 3);	  //�¶�3���ֽ�
				lu8datalenback += 3;
			}

			//��ȡ����״̬��
			lu8DataIn[0] = 0x11;
			Err = ADEValveCommunicate(p_mf,ADE_READVALVE_STATUS,lu8DataIn,lu8databuf,&lu8datalen);
			if(Err == NO_ERR){     
					uint16  ValveState = 0;
					ValveState = *(uint16*)lu8databuf;
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

					lu8BackDataBuf[3] = (uint8)ValveState;  //���ſ����ֽ�

					lu8BackDataBuf[4] = 0;//���ո�ʽ���롣
					lu8BackDataBuf[5] = 0;

					lu8datalenback += 3;
					
			}
			else{
				memset(&lu8BackDataBuf[3], 0xee, 3);								//�¶�3���ֽ�
				lu8datalenback += 3;
			}

			memcpy(p_databuf,lu8BackDataBuf,lu8datalenback);
			*p_datalenback = lu8datalenback;
			
			break;
		}

		case READROOM_TEMP:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			lu8DataIn[7] = 0x11;
			Err = ADEValveCommunicate(p_mf,ADE_READROOM_TEMP,lu8DataIn,lu8databuf,&lu8datalen);
			if(Err == NO_ERR){
				memcpy(lu8BackDataBuf,lu8databuf,3);  //�����¶����ݡ�
				lu8datalenback += 3;
				
			}
			else{
				memset(lu8BackDataBuf, 0xee, 3);	  //�¶�3���ֽ�
				lu8datalenback += 3;
			}

			memcpy(p_databuf,lu8BackDataBuf,lu8datalenback);
			*p_datalenback = lu8datalenback;

			break;
		}

		case SETHEAT_DISPLAY:{
	
			break;
		}

		case SETHEAT_VALUE:{
			memcpy(lu8DataIn,p_DataIn,gADEVALVE_Table[ADE_SETHEAT_VALUE][1] - 3);
			//�ĵ�ʱ��Ϊ��ǰʱ�䡣�����³����Э��Ҫ��
			ReadDateTime(lu8ReadTime);
			p_CJ188Data = (CJ188_Format *)lu8DataIn;
			memcpy(p_CJ188Data->RealTime,lu8ReadTime,6);
			p_CJ188Data->RealTime[6] = 0x20;  //�̶�Ϊ20xx�ꡣ
			Err = ADEValveCommunicate(p_mf,ADE_SETHEAT_VALUE,lu8DataIn,lu8databuf,&lu8datalen);
			if(Err == NO_ERR){
				//debug_info(gDebugModule[TASKDOWN_MODULE],"%s %d Send HeatMeter data  to valve ok",__FUNCTION__,__LINE__);
			}
			else  {
				//debugX(LOG_LEVEL_ERROR,"%s %d Send HeatMeter data  to valve  failed!\r\n",__FUNCTION__,__LINE__);
			}

			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETROOM_TEMP:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			memcpy(&lu8DataIn[7],p_DataIn,3);
			Err = ADEValveCommunicate(p_mf,ADE_SETROOM_TEMP,lu8DataIn,lu8databuf,&lu8datalen);
			
			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETTEMP_RANGE:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			memcpy(&lu8DataIn[7],p_DataIn,6);
			Err = ADEValveCommunicate(p_mf,ADE_SETTEMP_RANGE,lu8DataIn,lu8databuf,&lu8datalen);
			
			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETVALVE_STATUS:{
			lu8DataIn[0] =  *(p_DataIn + 0);
			Err = ADEValveCommunicate(p_mf,ADE_SETVALVE_STATUS,lu8DataIn,lu8databuf,&lu8datalen);
			
			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETVALVE_CONTROLTYPE:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			if(*(p_DataIn+0) == 0x09){
					lu8DataIn[7]		= 0x55;
			}
			if(*(p_DataIn+0) == 0x0B){
					lu8DataIn[7]		= 0x66;
			}
			if(*(p_DataIn+0) == 0x0C){
					lu8DataIn[7]		= 0x77;
			}
			if(*(p_DataIn+0) == 0x0D){
					lu8DataIn[7]		= 0x88;
			}
			Err = ADEValveCommunicate(p_mf,ADE_SETVALVE_CONTROLTYPE,lu8DataIn,lu8databuf,&lu8datalen);
			
			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;


			break;
		}

		case READVALVE_STATUS:{
			//��ȡ����״̬!!
			lu8DataIn[0] = 0x11;
			Err = ADEValveCommunicate(p_mf,ADE_READVALVE_STATUS,lu8DataIn,lu8databuf,&lu8datalen);
			
			if(Err == NO_ERR){     
					uint16  ValveState = 0;
					ValveState = *(uint16*)lu8databuf;
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

					lu8BackDataBuf[0] = (uint8)ValveState;  //���ſ����ֽ�
					lu8datalenback += 1;
					
			}

			memcpy(p_databuf,lu8BackDataBuf,lu8datalenback);
			*p_datalenback = lu8datalenback;

			break;
		}

		case READ_VALVEDATA:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			lu8DataIn[7] = 0;
			Err = ADEValveCommunicate(p_mf,ADE_READ_VALVEDATA,lu8DataIn,lu8databuf,&lu8datalen);
			
			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case WRITE_DIVHEATDATA:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			memcpy(&lu8DataIn[7],p_DataIn,12);
			Err = ADEValveCommunicate(p_mf,ADE_WR_HEATDATA,lu8DataIn,lu8databuf,&lu8datalen);
			if(Err == NO_ERR){
				//debug_info(gDebugModule[TASKDOWN_MODULE],"%s %d Send HeatMeter data  to valve ok",__FUNCTION__,__LINE__);
			}
			else  {
				//debugX(LOG_LEVEL_ERROR,"%s %d Send HeatMeter data  to valve  failed!\r\n",__FUNCTION__,__LINE__);
			}

			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETVALVE_OFFSETTEMP:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			memcpy(&lu8DataIn[7],p_DataIn,3);
			Err = ADEValveCommunicate(p_mf,ADE_SETVALVE_OFFSETTEMP,lu8DataIn,lu8databuf,&lu8datalen);
	
			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		case SETVALVE_HYSTEMP:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			lu8DataIn[7] = *(p_DataIn + 0);
			Err = ADEValveCommunicate(p_mf,ADE_SETVALVE_HYSTEMP,lu8DataIn,lu8databuf,&lu8datalen);
	
			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;


			break;
		}

		case SETTEMP_GATHERPERIOD:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			lu8DataIn[7] = *(p_DataIn + 0);
			Err = ADEValveCommunicate(p_mf,ADE_SETTEMP_GATHERPERIOD,lu8DataIn,lu8databuf,&lu8datalen);
	
			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;


			break;
		}

		case SETTEMP_UPLOADPERIOD:{
			memcpy(lu8DataIn,p_mf->ControlPanelAddr,7);
			lu8DataIn[7] = *(p_DataIn + 0);
			Err = ADEValveCommunicate(p_mf,ADE_SETTEMP_UPLOADPERIOD,lu8DataIn,lu8databuf,&lu8datalen);
	
			memcpy(p_databuf,lu8databuf,lu8datalen);
			*p_datalenback = lu8datalen;


			break;
		}

			

		default:
			break;
	}



	return Err;
}


/*
  ********************************************************************************************************
  * ��������: uint8 ADEValveCommunicate(MeterFileType *p_mf,uint8 ADE_Functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
  
  * ˵    ���������³������Э��ͨѶʵ�ֺ��������ݹ���ADE_Functype��gADEVALVE_Table�е�����Ӧ
  				�����룬������ɹ����򷵻س�����������ȫ�����ݡ�
  *					
  * ��������� 
  				MeterFileType *p_mf   ���������ݡ�
  				uint8 ADE_Functype  �Է�����ʲô���Ŀ��ƣ��������Ϣ�������������¶ȵȡ�
				uint8 *p_datain  ���뺯������Ҫʹ�õ����ݡ�
  				uint8 *p_databuf    �ӷ����ж����������ݴ洢ָ�롣
  				uint8 p_datalenback  �ӷ����ж����������ݳ��ȡ�
  * �������:
  				ִ���Ƿ�ɹ���
  ********************************************************************************************************
  */
uint8 ADEValveCommunicate(MeterFileType *p_mf,uint8 ADE_Functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
{
	uint8 Err = 0;
	DELU_Protocol	ProtocoalInfo;

	ProtocoalInfo.PreSmybolNum  = 0x02;
 	ProtocoalInfo.MeterType 	= 0x20;
	memcpy(ProtocoalInfo.MeterAddr, p_mf->ValveAddr, 7);
	ProtocoalInfo.ControlCode 	= 0x04;
	ProtocoalInfo.Length		= gADEVALVE_Table[ADE_Functype][1];
	ProtocoalInfo.DataIdentifier= gADEVALVE_Table[ADE_Functype][0];
	ProtocoalInfo.SER = 0;
	memcpy(ProtocoalInfo.DataBuf, p_DataIn, ProtocoalInfo.Length-3);  //�������������ݡ�
						
	Err = METER_DataItem(&ProtocoalInfo);

	if(Err == NO_ERR){
		memcpy(p_databuf, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length-3));
		*p_datalenback = ProtocoalInfo.Length - 3;
	}

	
	return Err;


}

