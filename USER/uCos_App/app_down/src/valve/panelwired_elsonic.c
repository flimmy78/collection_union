/*
  ********************************************************************************************************
  * @file    panelwired_elsonic.c
  * @author  zjjin
  * @version V0.0.0
  * @date    09-29-2015
  * @brief   
  ********************************************************************************************************
  * @attention
  *		ÒÚÁÖÓĞÏßÎÂ¿ØÃæ°åĞ­Òé¹¦ÄÜÊµÏÖÏà¹Ø´úÂë¡£
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
  * º¯ÊıÃû³Æ: uint8 ValveContron_Elsonic(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)
  
  * Ëµ    Ã÷£ºÒÚÁÖ ·§¿ØÆ÷¿ØÖÆº¯Êı£¬ÔÚ´Ëº¯ÊıÖĞÊµÏÖÒÚÁÖ·§¿Ø²»Í¬¿ØÖÆ¡£
  *					
  * ÊäÈë²ÎÊı£º 
  				MeterFileType *p_mf   ±í²ÎÊı£¬º¬ÓĞ·§¿ØĞ­Òé°æ±¾¡¢·§¿ØµØÖ·µÈĞÅÏ¢¡£
  				uint8 functype  ¶Ô·§½øĞĞÊ²Ã´ÑùµÄ¿ØÖÆ£¬±ÈÈç¶ÁĞÅÏ¢¡¢ÉèÖÃÉÏÏÂÏŞÎÂ¶ÈµÈ¡£
				uint8 *p_datain  ÊäÈëº¯Êı¿ÉÄÜÒªÊ¹ÓÃµÄÊı¾İ¡£
  				uint8 *p_databuf    ´Ó·§¿ØÖĞ¶ÁÉÏÀ´µÄÊı¾İ´æ´¢Ö¸Õë¡£
  				uint8 p_datalenback  ´Ó·§¿ØÖĞ¶ÁÉÏÀ´µÄÊı¾İ³¤¶È¡£
  * Êä³ö²ÎÊı:
  				Ö´ĞĞÊÇ·ñ³É¹¦¡£
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
			Create_ElsonicWiredPanel_ReadInfo_Frame(p_mf,lu8dataframe, &lu8datalen);//×é½¨»ñÈ¡·§ÃÅÊı¾İÃüÁîÖ¡
			Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);
			
			if(Err == NO_ERR){	  
				l8tmp = (int8)lu8dataframe[5];  //·¿¼äÎÂ¶È+²¹³¥ÎÂ¶È¡£
				l8tmp += lu8dataframe[9];
				lu8tmp = (uint8)l8tmp;
				lu8databuf[i++] = 0x00;  //Ğ¡ÊıÎ»¹Ì¶¨Îª0.
				lu8datalenback++;
				lu8databuf[i++] = HEX_TO_BCD(lu8tmp);
				lu8datalenback++;
				lu8databuf[i++] = 0x00;  //·ûºÅÎ»
				lu8datalenback++;
			
				lu8tmp = lu8dataframe[4];
				if(lu8tmp & 0x04)	   //Èç¹ûbit0=1±íÊ¾·§ÃÅ¿ª×´Ì¬£¬0¹Ø×´Ì¬¡£
					lu8databuf[i++] = 0x55;  // È«¿ª
				else
					lu8databuf[i++] = 0x99;  //È«¹Ø¡£
								
				lu8datalenback++;
				
				//begin:·§¿Ø×´Ì¬Î»×Ö½Ú´¦Àí¡£
				lu8databuf[4] = 0; //ÏÈ½«×´Ì¬×Ö½Ú³õÊ¼»¯Îª0 ¡£

				lu8tmp = lu8dataframe[3];
				if((lu8tmp & 0x10) == 0)  //Ãæ°å¿ª¹Ø
					lu8databuf[4] |= 0x02;

				lu8tmp = lu8dataframe[4];
				if(lu8tmp & 0x02)  //Ãæ°åËø¶¨ÊÇ·ñ¡£
					lu8databuf[4] |= 0x04;

				lu8tmp = lu8dataframe[3];
				if(lu8tmp & 0x08)  //·§ÃÅÊÇ·ñËø¶¨¡£
					lu8databuf[4] |= 0x08;
				if(lu8tmp & 0x04)  //·§ÃÅÊÇ·ñËø¶¨¡£
					lu8databuf[4] |= 0x08;
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
			if(*(p_DataIn+0) == 0x09){ //ÒÚÁÖ·§Ö»ÓĞÖØĞÂÊ¹ÄÜ·§¿ØÆ÷ÓÉÎÂ¿ØÃæ°å¿ØÖÆÊ±²ÅÖ´ĞĞ¡£
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
	 * º¯ÊıÃû³Æ£ºCreate_ElsonicWiredPanel_ReadInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * Ëµ	 Ã÷£º´Ëº¯ÊıÓÃÓÚ¶ÁÈ¡ÒÚÁÖÓĞÏßÎÂ¿ØÃæ°å×´Ì¬ĞÅÏ¢¡£
	 * ²Î	 Êı£º 
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
	 * º¯ÊıÃû³Æ£ºCreate_ElsonicWiredPanel_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * Ëµ	 Ã÷£º´Ëº¯ÊıÓÃÓÚ½«ÉèÖÃÒÚÁÖÓĞÏßÃæ°åÊ¹ÄÜÈÈÁ¿ÏÔÊ¾¹¦ÄÜ¡£
	 				Ä¿Ç°ÒÚÁÖÓĞÏßÎÂ¿ØÃæ°åÃ»ÓĞ´Ë¹¦ÄÜ¡£
	 * ²Î	 Êı£º 
	 ******************************************************************************
	 */
   void Create_ElsonicWiredPanel_SetInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
   {
			   
		   
   }
  
   
   
   
   /*
	 ******************************************************************************
	 * º¯ÊıÃû³Æ£ºCreate_ElsonicWiredPanel_SetHeat_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * Ëµ	 Ã÷£º´Ëº¯ÊıÓÃÓÚ½«ÈÈÁ¿ÖµĞ´ÈëÒÚÁÖÓĞÏßÎÂ¿ØÃæ°å¡£
	 				Ä¿Ç°ÒÚÁÖÓĞÏßÎÂ¿ØÃæ°åÃ»ÓĞ´Ë¹¦ÄÜ¡£
	 * ²Î	 Êı£º 
	 ******************************************************************************
	 */
   void Create_ElsonicWiredPanel_SetHeat_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {
			   
		   
   }
   
   /*
	 ******************************************************************************
	 * º¯ÊıÃû³Æ£ºCreate_ElsonicWiredPanel_SetRoomTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * Ëµ	 Ã÷£º´Ëº¯ÊıÓÃÓÚÉèÖÃÒÚÁÖÓĞÏßÎÂ¿ØÃæ°åÊÒÄÚÎÂ¶È¡£
	 * ²Î	 Êı£º 
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
   
   
		   setInform[6] = BCD_TO_HEX(*(p_DataIn + 1));  //Éè¶¨ÎÂ¶È,´Ë´¦ĞèÒªµ÷ÕûÕıÈ·Æ«Á¿¡£
		   if(setInform[6] < 5)	//ÎÂ¶ÈÉè¶¨·¶Î§5-35¡æ¡£
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
	 * º¯ÊıÃû³Æ£ºCreate_ElsonicWiredPanel_SetRoomTempRange_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
	 * Ëµ	 Ã÷£º´Ëº¯ÊıÓÃÓÚÉèÖÃÒÚÁÖÓĞÏßÎÂ¿ØÃæ°åÊÒÄÚÎÂ¶ÈÉÏÏÂÏŞ¡£
	 				Ä¿Ç°ÒÚÁÖÓĞÏßÎÂ¿ØÃæ°åÃ»ÓĞ´Ë¹¦ÄÜ¡£
	 * ²Î	 Êı£º 
	 ******************************************************************************
	 */
   void Create_ElsonicWiredPanel_SetRoomTempRange_Frame(MeterFileType *p_mf,uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
   {
			   
		   
   }
		   
   
   /*
	 ******************************************************************************
	 * º¯ÊıÃû³Æ£ºCreate_ElsonicWiredPanel_SetValve_Frame(DELU_Protocol *pData, uint8 valvestate,uint8 *pSendFrame, uint8 *plenFrame)
	 * Ëµ	 Ã÷£º´Ëº¯ÊıÓÃÓÚÇ¿ÖÆÒÚÁÖÓĞÏßÎÂ¿ØÃæ°å¿ª¹Ø·§¡£
	 * ²Î	 Êı£º 
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
			   setInform[3] = 0x08;  //Ğ­ÒéÓĞÎó£¬ÒÔ´ËÎª×¼¡£
		   else if(*p_DataIn == 0x55)
			   setInform[3] = 0x04;
		   else if(*p_DataIn == 0x09)  //·§Õı³£¹¤×÷£¬»¹ÊÇÓÉÎÂ¿ØÃæ°å¿ØÖÆ¡£
		   	 	setInform[3] = 0;
		   else
			   setInform[3] = 0x04;//ÆäËûÒ²È«²¿´ò¿ª·§ÃÅ£¬°²È«¡£
   
		   cs = 0;
		   for(i=0;i<7;i++){
			   cs += setInform[i];
		   }
			   cs ^= 0xA5;
			   setInform[7] = cs;
		   
		   
		   memcpy(pTemp, &setInform[0], 8);
		   *plenFrame = 8;
			   
		   
   }
   
   
