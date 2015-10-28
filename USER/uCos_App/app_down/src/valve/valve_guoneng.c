/*
  ********************************************************************************************************
  * @file    valve_guoneng.c
  * @author  zjjin
  * @version V0.0.0
  * @date    10-15-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		Íþº£¹úÄÜ×Ô¿Ø¿Æ¼¼"¹úÄÜÎÂ¿Ø·§"ÓÃ¡£
  *
  ********************************************************************************************************
  */  

//#include <includes.h>

#include "Valve.h"  
#include "valve_guoneng.h"


#define HEX_TO_BCD(x) ((x/0x0A)*0x10+(x%0x0A))
#define BCD_TO_HEX(x) ((x/0x10)*0x0A+(x%0x10))



/*
  ********************************************************************************************************
  * º¯ÊýÃû³Æ: uint8 ValveContron_GuoNeng(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
  
  * Ëµ    Ã÷£ºÍþº£¹úÄÜ·§¿ØÆ÷¿ØÖÆº¯Êý£¬ÔÚ´Ëº¯ÊýÖÐÊµÏÖÍþº£¹úÄÜ·§¿Ø²»Í¬¿ØÖÆ¡£
  *					
  * ÊäÈë²ÎÊý£º 
  				MeterFileType *p_mf   ±í²ÎÊý£¬º¬ÓÐ·§¿ØÐ­Òé°æ±¾¡¢·§¿ØµØÖ·µÈÐÅÏ¢¡£
  				uint8 functype  ¶Ô·§½øÐÐÊ²Ã´ÑùµÄ¿ØÖÆ£¬±ÈÈç¶ÁÐÅÏ¢¡¢ÉèÖÃÉÏÏÂÏÞÎÂ¶ÈµÈ¡£
				uint8 *p_datain  ÊäÈëº¯Êý¿ÉÄÜÒªÊ¹ÓÃµÄÊý¾Ý¡£
  				uint8 *p_databuf    ´Ó·§¿ØÖÐ¶ÁÉÏÀ´µÄÊý¾Ý´æ´¢Ö¸Õë¡£
  				uint8 p_datalenback  ´Ó·§¿ØÖÐ¶ÁÉÏÀ´µÄÊý¾Ý³¤¶È¡£
  * Êä³ö²ÎÊý:
  				Ö´ÐÐÊÇ·ñ³É¹¦¡£
  ********************************************************************************************************
  */

uint8 ValveContron_GuoNeng(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
{
	uint8 Err = 0;
	uint8 lu8dataframe[100] = {0};
	uint8 lu8datalen = 0;
	uint8 lu8databuf[20] = {0xee};
	uint8 lu8datalenback = 0;
	uint8 i =0;
//	int8 l8tmp = 0;
	uint8 lu8tmp = 0;
	

	
	switch(functype){
		case ReadVALVE_All:{						
			Create_GuoNengVave_ReadInfo_Frame(p_mf,lu8dataframe, &lu8datalen);//×é½¨»ñÈ¡·§ÃÅÊý¾ÝÃüÁîÖ¡
			Err = GuonengValve_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
			if(Err == NO_ERR){	  
				lu8databuf[i++] = lu8dataframe[22] & 0x0f;  
				lu8datalenback++;
				lu8tmp = lu8dataframe[22] >> 4;
				lu8tmp += lu8dataframe[23] << 4;
				lu8databuf[i++] = lu8tmp;
				lu8datalenback++;
				lu8databuf[i++] = 0x00;  //·ûºÅÎ»
				lu8datalenback++;
				
				lu8tmp = BCD_TO_HEX(lu8dataframe[25]);  //¹úÄÜ·§¿ª¶ÈÊÇ°Ù·Ö±È£¬BCDÂë2×Ö½Ú¡£
				lu8tmp = lu8tmp * 100;
				lu8tmp += BCD_TO_HEX(lu8dataframe[24]);
				if(lu8tmp > 90)
					lu8databuf[i++] = 0x55;  // È«¿ª
				else if(lu8tmp > 75)
					lu8databuf[i++] = 0x66;  // 3/4¿ª¡£
				else if(lu8tmp > 40)
					lu8databuf[i++] = 0x77;  // 2/4¿ª
				else if(lu8tmp > 20)
					lu8databuf[i++] = 0x88;  // 1/4
				else
					lu8databuf[i++] = 0x99;//È«¹Ø¡£
								
				lu8datalenback++;

					
				//begin:·§¿Ø×´Ì¬Î»×Ö½Ú´¦Àí¡£
				lu8databuf[4] = 0; //ÏÈ½«×´Ì¬×Ö½Ú³õÊ¼»¯Îª0 ¡£
				lu8tmp = lu8dataframe[29];
				if(lu8tmp & 0x04)  //ÎÞÏßÒì³£¡£
					lu8databuf[4] |= 0x01;

				//lu8databuf[4] |= 0x02;  //¹úÄÜ·§¿ØÃ»ÓÐÃæ°å¿ª¹Ø±êÖ¾£¬ËùÒÔ²»ÓÃÅÐ¶Ï¡£

				lu8tmp = lu8dataframe[20];
				if(lu8tmp & 0x02)  //Ãæ°åËø¶¨ÊÇ·ñ¡£
					lu8databuf[4] |= 0x04;

				//if(lu8tmp & 0x02)  //·§ÃÅÊÇ·ñËø¶¨¡£
				//	lu8databuf[4] |= 0x08;
				//if(lu8tmp & 0x80)  //·§ÃÅÊÇ·ñËø¶¨¡£
				//	lu8databuf[4] |= 0x08;
				//end:·§¿Ø×´Ì¬Î»×Ö½Ú´¦Àí¡

				i++;
				lu8datalenback++;
				lu8databuf[i++] = 0x00;  //Ô¤Áô¡£
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


		case SETHEAT_VALUE:{  //¹úÄÜÈÈÁ¿ÏÔÊ¾¹¦ÄÜ²»¼Ó¡£

			break;
		}

		case SETROOM_TEMP:{
			Create_GuoNengVave_SetRoomTemp_Frame(p_mf,p_DataIn,lu8dataframe, &lu8datalen);
			Err = GuonengValve_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
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

		case SETTEMP_RANGE:{  //¹úÄÜ²»ÄÜÉè¶¨ÎÂ¶È·¶Î§£¬Ö»ÄÜÊÇ×î¸ß35¡æ£¬²»×ö´¦Àí¾ÍÊÇ35ÉãÊÏ¶È¡£


			break;
		}

		case SETVALVE_STATUS:{  //Íþº£¹úÄÜÎÂ¿ØÆ÷²»ÄÜÇ¿ÖÆ¿ª·§£¬ÔÚ·§ÃÅ·ÇÇ¿ÖÆ¹Ø±ÕÊ±ÓÉÃæ°å¾ö¶¨¿ª¹Ø¡£
								//³ÌÐòÖÐÃ»ÓÐÐ´Ç¿ÖÆ¹Ø·§£¬ËùÒÔ´Ë´¦Îª¿Õ¡£
			break;
		}

		case SETVALVE_CONTROLTYPE:{

			break;
		}
		

		default:
			break;
	}



	return Err;
}


  
   /*
	 ******************************************************************************
	 * º¯ÊýÃû³Æ£ºCreate_GuoNengVave_ReadInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * Ëµ	 Ã÷£º´Ëº¯ÊýÓÃÓÚ¶ÁÈ¡¹úÄÜ·§¿ØÆ÷×´Ì¬ÐÅÏ¢¡£
	 * ²Î	 Êý£º 
	 ******************************************************************************
	 */
   void Create_GuoNengVave_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
   {
		   uint8 setInform[20]={0xFE,0xFE,0xFE,0xFE,0x68,0x41,0x00,0x00,0x00,0x00,0x00,0x30,0x09,0x01,0x03,0x82,0x1F,0x00,0x87,0x16};
		   uint8 cs 	   = 0x00;
		   uint8 *pTemp;
		   uint8 i =0;
		   //uint8 lu8valveaddr[7] = {0x00};
		   //uint16 lu16valveaddr =0;
			   
		   LOG_assert_param(p_mf == NULL);
		   LOG_assert_param(pSendFrame == NULL);
		   LOG_assert_param(plenFrame == NULL);
			   
		   pTemp = pSendFrame;
		   	
		   memcpy(setInform+6, &p_mf->ValveAddr[0], 7);
		   
		   cs = 0;
		   for(i=4;i<=17;i++){
			   cs += setInform[i];
		   }
	
		   setInform[18] = cs;
		   
		   
		   memcpy(pTemp, &setInform[0], 20);
		   *plenFrame = 20;
			   

}

   
   /*
	 ******************************************************************************
	 * º¯ÊýÃû³Æ£ºCreate_GuoNengVave_SetHeat_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * Ëµ	 Ã÷£º´Ëº¯ÊýÓÃÓÚ½«ÈÈÁ¿ÖµÐ´Èë·§¿ØÆ÷¡£
	 * ²Î	 Êý£º 
	 ******************************************************************************
	 */
   void Create_GuoNengVave_SetHeat_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {

			   
		   
   }


   
   /*
	 ******************************************************************************
	 * º¯ÊýÃû³Æ£ºCreate_ElsonicVave_SetRoomTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * Ëµ	 Ã÷£º´Ëº¯ÊýÓÃÓÚÉèÖÃÊÒÄÚÎÂ¶È¡£
	 * ²Î	 Êý£º 
	 ******************************************************************************
	 */
   void Create_GuoNengVave_SetRoomTemp_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {
	   uint8 setInform[30]={0xFE,0xFE,0xFE,0xFE,0x68,0x41,0x00,0x00,0x00,0x00,0x00,0x30,0x09,0x04,0x0B,0x93,0x2F,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x87,0x16};
	   uint8 cs 	  = 0x00;
	   uint8 *pTemp;
	   uint8 i =0;
				  
	   LOG_assert_param(p_mf == NULL);
	   LOG_assert_param(pSendFrame == NULL);
	   LOG_assert_param(plenFrame == NULL);
				  
	   pTemp = pSendFrame;
			   
	   memcpy(setInform+6, &p_mf->ValveAddr[0], 7);
		   
	   setInform[18] = *(p_DataIn+1);
	   if(setInform[18] < 0x05)
	   		setInform[18] = 0x05;
	   if(setInform[18] > 0x35)
	   		setInform[18] = 0x35;
			  
	   cs = 0;
	   for(i=4;i<=25;i++){
				cs += setInform[i];
	   }
		   
	   setInform[26] = cs;
			  
			  
	   memcpy(pTemp, &setInform[0], 28);
	   *plenFrame = 28;

			   
		   
   }

   
uint8 GuonengValve_ReadData_Communicate(uint8 *DataFrame, uint8 *DataLen)
{
   
    uint8 err;
	uint8 dev = DOWN_COMM_DEV_MBUS;
	uint8 len=0;		 
	uint8 DataBuf[50];
	uint16 OutTime = OS_TICKS_PER_SEC*2;
	
	LOG_assert_param(DataFrame == NULL);
	LOG_assert_param(DataLen == NULL);
	
	DuQueueFlush(dev);				 //Çå¿Õ»º³åÇø	 
	DuSend(dev, (uint8*)DataFrame,  *DataLen);
		   
	err = METER_ReceiveFrame(dev, DataBuf, OutTime, &len);		   
   
	if(err==NO_ERR){
		memcpy(DataFrame, &DataBuf[0], len);
		*DataLen = len;
		return NO_ERR;
	}
	   
			
	return 1;
}


