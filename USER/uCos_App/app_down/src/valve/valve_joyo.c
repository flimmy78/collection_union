/*
  ********************************************************************************************************
  * @file    valve_joyo.c
  * @author  zjjin
  * @version V0.0.0
  * @date    04-20-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		������ԴЭ�鷧��������ʵ����ش��롣
  *
  ********************************************************************************************************
  */  

//#include <includes.h>

#include "Valve.h"  
#include "valve_joyo.h"


#define HEX_TO_BCD(x) ((x/0x0A)*0x10+(x%0x0A))
#define BCD_TO_HEX(x) ((x/0x10)*0x0A+(x%0x10))





/*
  ********************************************************************************************************
  * ��������: uint8 ValveContron_Elsonic(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
  
  * ˵    ����������Դ ���������ƺ������ڴ˺�����ʵ�����ַ��ز�ͬ���ơ�
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

uint8 ValveContron_Joyo(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
{
	uint8 Err = 0;
	uint8 lu8dataframe[100] = {0};
	uint8 lu8datalen = 0;
	uint8 lu8databuf[20] = {0xee};
	uint8 lu8datalenback = 0;
	uint8 i =0;
	uint8 lu8tmp = 0;
	

	
	switch(functype){
		case ReadVALVE_All:{						
			Create_JOYOVave_ReadInfo_Frame(p_mf,lu8dataframe, &lu8datalen);//�齨��ȡ������������֡
			Err = JOYO_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
			if(Err == NO_ERR){	  
				lu8databuf[i++] = 0x00;  //С��λ�̶�Ϊ0.
				lu8datalenback++;
				lu8databuf[i++] = HEX_TO_BCD(lu8dataframe[9]);
				lu8datalenback++;
				lu8databuf[i++] = 0x00;  //����λ
				lu8datalenback++;
			
				lu8tmp = lu8dataframe[4];
				if(lu8tmp & 0x04)	   //���bit2=1��ʾ���ſ�״̬��0��״̬��
					lu8databuf[i++] = 0x55;  // ȫ��
				else
					lu8databuf[i++] = 0x99;  //ȫ�ء�
								
				lu8datalenback++;
				
				//begin:����״̬λ�ֽڴ���
				lu8databuf[4] = 0; //�Ƚ�״̬�ֽڳ�ʼ��Ϊ0 ��
				lu8tmp = lu8dataframe[3];
				if((lu8tmp & 0x10) == 0)  //��忪��,bit4=1������0�ػ���
					lu8databuf[4] |= 0x02;

				if(lu8tmp & 0x0c)          //�����Ƿ�������bit2bit3=01ǿ�ƿ�����10ǿ�ƹرա�
					lu8databuf[4] |= 0x08;	//�˴�ֻ�ж��Ƿ���������Ϸ���״̬�ж�����������or�رա�

				lu8tmp = lu8dataframe[4];
				if(lu8tmp & 0x02)         //����Ƿ�������bit1=1������
					lu8databuf[4] |= 0x04;


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
			Create_JOYOVave_SetRoomTemp_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
			Err = JOYO_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
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
			Create_JOYOVave_SetRoomTempRange_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
			Err = JOYO_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
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
			Create_JOYOVave_SetValve_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
			Err = JOYO_ReadData_Communicate(lu8dataframe, &lu8datalen);

			memcpy(p_databuf,lu8dataframe,lu8datalen);
			*p_datalenback = lu8datalen;

			break;
		}

		default:
			break;
	}



	return Err;
}

/*
  ******************************************************************************
  * �������ƣ�Create_JOYOVave_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
  * ˵    �����˺������ڶ�ȡ��Դ������״̬��Ϣ��
  * ��    ���� 
  ******************************************************************************
  */
void Create_JOYOVave_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
{
		uint8 setInform[8]={0xa0,0x10,0x00,0x00,0x00,0x00,0x00,0x15};
//		uint8 templen	= 0x00;
		uint8 cs		= 0x00;
		uint8 *pTemp;
//		uint16 lenFrame = 0x00;
//		uint16 CheckLen = 0x00;
		uint8 i =0;
		uint8 lu8valveaddr[7] = {0x00};
//		uint16 lu16valveaddr =0;
			
		LOG_assert_param(p_mf == NULL);
		LOG_assert_param(pSendFrame == NULL);
		LOG_assert_param(plenFrame == NULL);
			
		pTemp = pSendFrame;
		
		memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);				
		//for(i=0;i<7;i++){
		//	lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
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
  * �������ƣ�Create_JOYOVave_SetRoomTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
  * ˵    �����˺����������������¶ȡ�
  * ��    ���� 
  ******************************************************************************
  */
void Create_JOYOVave_SetRoomTemp_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
		uint8 setInform[8]={0xa9,0x10,0x00,0x00,0x00,0x00,0x20,0x7b};
//		uint8 templen	= 0x00;
		uint8 cs		= 0x00;
		uint8 *pTemp;
//		uint16 lenFrame = 0x00;
//		uint16 CheckLen = 0x00;
		uint8 i =0;
		uint8 lu8valveaddr[7] = {0x00};
//		uint16 lu16valveaddr =0;
//		uint8 lu8currentheat[4] = {0x00};
//		uint32 lu32currentheat =0;
			
		LOG_assert_param(p_mf == NULL);
		LOG_assert_param(pSendFrame == NULL);
		LOG_assert_param(plenFrame == NULL);
			
		pTemp = pSendFrame;
		
		memcpy(lu8valveaddr, &p_mf->MeterAddr[0], 7);				
		//for(i=0;i<7;i++){
		//	lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
		//}
		//lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1]*100;
			
		//setInform[1] = lu16valveaddr & 0x00ff;
		//setInform[2] = (lu16valveaddr>>8) & 0x00ff;

		setInform[1] = lu8valveaddr[0];
		setInform[2] = lu8valveaddr[1];


		setInform[6] = BCD_TO_HEX(*(p_DataIn+1));  //�趨�¶�
		if(setInform[6] < 5)	 //�¶��趨��Χ5-35�档
			setInform[6] = 5;
		if(setInform[6] > 30)
			setInform[6] = 30;
		
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
  * �������ƣ�Create_JOYOVave_SetRoomTempRange_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
  * ˵    �����˺����������������¶������ޡ�
  * ��    ���� 
  ******************************************************************************
  */
void Create_JOYOVave_SetRoomTempRange_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
		uint8 setInform[8]={0xa6,0x10,0x00,0x00,0x0a,0x20,0x00,0x7b};
//		uint8 templen	= 0x00;
		uint8 cs		= 0x00;
		uint8 *pTemp;
//		uint16 lenFrame = 0x00;
//		uint16 CheckLen = 0x00;
		uint8 i =0;
		uint8 lu8valveaddr[7] = {0x00};
//		uint16 lu16valveaddr =0;
//		uint8 lu8currentheat[4] = {0x00};
//		uint32 lu32currentheat =0;
			
		LOG_assert_param(p_mf == NULL);
		LOG_assert_param(pSendFrame == NULL);
		LOG_assert_param(plenFrame == NULL);
			
		pTemp = pSendFrame;
		
		memcpy(lu8valveaddr, &p_mf->MeterAddr[0], 7);				
		//for(i=0;i<7;i++){
		//	lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
		//}
		//lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1]*100;
			
		//setInform[1] = lu16valveaddr & 0x00ff;
		//setInform[2] = (lu16valveaddr>>8) & 0x00ff;

		setInform[1] = lu8valveaddr[0];
		setInform[2] = lu8valveaddr[1];


		setInform[5] = BCD_TO_HEX(*(p_DataIn+1));  //�趨�¶�����ֵ��
		if(setInform[5] < 35)    //���޷�Χ35-70�档
			setInform[5] = 35;
		if(setInform[5] > 70)   
			setInform[5] = 70;

		setInform[4] = BCD_TO_HEX(*(p_DataIn+4));  //�趨�¶�����ֵ��
		if(setInform[4] < 5)    //���޷�Χ5-10�档
			setInform[4] = 5;
		if(setInform[4] > 10)   
			setInform[4] = 10;

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
  * �������ƣ�Create_JOYOVave_SetValve_Frame(DELU_Protocol *pData, uint8 valvestate,uint8 *pSendFrame, uint8 *plenFrame)
  * ˵    �����˺�������ǿ�ƿ��ط���
  * ��    ���� 
  ******************************************************************************
  */
void Create_JOYOVave_SetValve_Frame(MeterFileType *p_mf, uint8 *p_DataIn,uint8 *pSendFrame, uint8 *plenFrame)
{
		uint8 setInform[8]={0xa3,0x10,0x00,0x00,0x00,0x00,0x00,0x7b};
//		uint8 templen	= 0x00;
		uint8 cs		= 0x00;
		uint8 *pTemp;
//		uint16 lenFrame = 0x00;
//		uint16 CheckLen = 0x00;
		uint8 i =0;
		uint8 lu8valveaddr[7] = {0x00};
//		uint16 lu16valveaddr =0;
//		uint8 lu8currentheat[4] = {0x00};
//		uint32 lu32currentheat =0;
			
		LOG_assert_param(p_mf == NULL);
		LOG_assert_param(pSendFrame == NULL);
		LOG_assert_param(plenFrame == NULL);
			
		pTemp = pSendFrame;
		
		memcpy(lu8valveaddr, &p_mf->MeterAddr[0], 7);				
		//for(i=0;i<7;i++){
		//	lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
		//}
		//lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1]*100;
			
		//setInform[1] = lu16valveaddr & 0x00ff;
		//setInform[2] = (lu16valveaddr>>8) & 0x00ff;

		setInform[1] = lu8valveaddr[0];
		setInform[2] = lu8valveaddr[1];

		if(*p_DataIn == 0x99)  //ȫ��
			setInform[3] = 0x02;
		else if(*p_DataIn == 0x55)	//ȫ��
			setInform[3] = 0x04;
		else
			setInform[3] = 0x00;

    	cs = 0;
		for(i=0;i<7;i++){
			cs += setInform[i];
		}
			cs ^= 0xA5;
			setInform[7] = cs;
		
		
		memcpy(pTemp, &setInform[0], 8);
		*plenFrame = 8;
			
		
}




uint8 JOYO_ReadData_Communicate(uint8 *DataFrame, uint8 *DataLen)
{

	 uint8 err;
	 uint8 RetryTimes	  = 0x02;
//	 uint8 data=0;
	 uint8 dev = DOWN_COMM_DEV_MBUS;
     uint8 i=0; 
	 uint8 len=0;
	  
	 uint8 DataBuf[METER_FRAME_LEN_MAX];
     LOG_assert_param(DataFrame == NULL);
	 LOG_assert_param(DataLen == NULL);
	
	for(i=0;i<RetryTimes;i++){
		DuQueueFlush(dev);				  //��ջ�����	  
		DuSend(dev, (uint8*)DataFrame,  *DataLen);
		
		//OSTimeDly(OS_TICKS_PER_SEC/10);
		
		err = Receive_Read_ElsonicParamFrame(dev, DataBuf, 0, &len);

		if(err==NO_ERR){
			 memcpy(DataFrame, &DataBuf[0], len);
			 *DataLen = len;
			 return NO_ERR;
		}
		else{
			OSTimeDlyHMSM(0,0,0,500);	 //������ɹ��򲹳������500ms��
		}
		  
	}           
         
     return 1;
}



uint8 Receive_Read_JOYOParamFrame(uint8 dev, uint8 *buf, uint16 Out_Time, uint8 *datalen)
{
	uint8 data=0;
	uint8 i=0;
	uint8 readinfom[32]={0};
//	uint8 len = 0;
//	uint8 len1 = 0;
	uint8 Cs=0;
	
    i = 10;

	while(i--){													//��֡ͷ
			if(DuGetch(dev, &data, 2*OS_TICKS_PER_SEC))
				{return 1;}
			
			if(data == 0x50) //�ҵ�֡ͷ	
				{break; }
			
	}

	readinfom[0] = 0x50;
	Cs += readinfom[0];

	for(i=1;i<11;i++){
		if(DuGetch(dev, &data, 2*OS_TICKS_PER_SEC)){
			return 2;
		}
		readinfom[i] = data;
		Cs	+= data;
	}

	Cs ^= 0xa5;

	if(DuGetch(dev, &data, 2*OS_TICKS_PER_SEC)){
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

