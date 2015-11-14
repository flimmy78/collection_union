/*
  ********************************************************************************************************
  * @file    panelwired_elsonic.c
  * @author  zjjin
  * @version V0.0.0
  * @date    09-29-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		亿林有线温控面板协议功能实现相关代码。
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
  * 函数名称: uint8 ValveContron_Elsonic(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)

  * 说    明：亿林 阀控器控制函数，在此函数中实现亿林阀控不同控制。
  *
  * 输入参数：
  				MeterFileType *p_mf   表参数，含有阀控协议版本、阀控地址等信息。
  				uint8 functype  对阀进行什么样的控制，比如读信息、设置上下限温度等。
				uint8 *p_datain  输入函数可能要使用的数据。
  				uint8 *p_databuf    从阀控中读上来的数据存储指针。
  				uint8 p_datalenback  从阀控中读上来的数据长度。
  * 输出参数:
  				执行是否成功。
  ********************************************************************************************************
  */

uint8 PanelWiredControl_Elsonic(MeterFileType *p_mf, uint8 functype, uint8 *p_DataIn, uint8 *p_databuf, uint8 *p_datalenback)
{
    uint8 Err = 0;
    uint8 lu8dataframe[100] = {0};
    uint8 lu8datalen = 0;
    uint8 lu8databuf[20] = {0xee};
    uint8 lu8datalenback = 0;
    uint8 i = 0;
    int8 l8tmp = 0;
    uint8 lu8tmp = 0;



    switch(functype)
    {
    case ReadVALVE_All:
    {
        Create_ElsonicWiredPanel_ReadInfo_Frame(p_mf, lu8dataframe, &lu8datalen); //组建获取阀门数据命令帧
        Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);

        if(Err == NO_ERR)
        {
            l8tmp = (int8)lu8dataframe[5];  //房间温度+补偿温度。
            l8tmp += lu8dataframe[9];
            lu8tmp = (uint8)l8tmp;
            lu8databuf[i++] = 0x00;  //小数位固定为0.
            lu8datalenback++;
            lu8databuf[i++] = HEX_TO_BCD(lu8tmp);
            lu8datalenback++;
            lu8databuf[i++] = 0x00;  //符号位
            lu8datalenback++;

            lu8tmp = lu8dataframe[4];
            if(lu8tmp & 0x04)	   //如果bit0=1表示阀门开状态，0关状态。
                lu8databuf[i++] = 0x55;  // 全开
            else
                lu8databuf[i++] = 0x99;  //全关。

            lu8datalenback++;

            //begin:阀控状态位字节处理。
            lu8databuf[4] = 0; //先将状态字节初始化为0 。

            lu8tmp = lu8dataframe[3];
            if((lu8tmp & 0x10) == 0)  //面板开关
                lu8databuf[4] |= 0x02;

            lu8tmp = lu8dataframe[4];
            if(lu8tmp & 0x02)  //面板锁定是否。
                lu8databuf[4] |= 0x04;

            lu8tmp = lu8dataframe[3];
            if(lu8tmp & 0x08)  //阀门是否锁定。
                lu8databuf[4] |= 0x08;
            if(lu8tmp & 0x04)  //阀门是否锁定。
                lu8databuf[4] |= 0x08;
            //end:阀控状态位字节处理?

            i++;
            lu8datalenback++;
            lu8databuf[i++] = 0x00;  //预留。
            lu8datalenback++;

        }
        else
        {
            memset(lu8databuf, 0xee, 6);
            lu8datalenback += 6;
            debug_err(gDebugModule[TASKDOWN_MODULE], "%s %d Read Valve state failed!\r\n", __FUNCTION__, __LINE__);
        }

        memcpy(p_databuf, lu8databuf, lu8datalenback);
        *p_datalenback = lu8datalenback;

        break;
    }

    case SETHEAT_DISPLAY:
    {

        break;
    }

    case SETHEAT_VALUE:
    {

        break;
    }

    case SETROOM_TEMP:
    {
        Create_ElsonicWiredPanel_SetRoomTemp_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
        Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);

        if(Err == NO_ERR)
        {
            debug_info(gDebugModule[TASKDOWN_MODULE], "%s Set indoor given temperature ok ", __FUNCTION__);
        }
        else
        {
            debug_err(gDebugModule[TASKDOWN_MODULE], "%s Set indoor given temperature failed ", __FUNCTION__);
        }

        memcpy(p_databuf, lu8dataframe, lu8datalen);
        *p_datalenback = lu8datalen;


        break;
    }

    case SETTEMP_RANGE:
    {

        break;
    }

    case SETVALVE_STATUS:
    {
        Create_ElsonicWiredPanel_SetValve_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
        Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);

        memcpy(p_databuf, lu8dataframe, lu8datalen);
        *p_datalenback = lu8datalen;

        break;
    }

    case SETVALVE_CONTROLTYPE:
    {
        if(*(p_DataIn + 0) == 0x09) //亿林阀只有重新使能阀控器由温控面板控制时才执行。
        {
            Create_ElsonicWiredPanel_SetValve_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
            Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);

            memcpy(p_databuf, lu8dataframe, lu8datalen);
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
 * 函数名称：Create_ElsonicWiredPanel_ReadInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于读取亿林有线温控面板状态信息。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicWiredPanel_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[8] = {0xa0, 0x19, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x7b};
    uint8 cs 	   = 0x00;
    uint8 *pTemp;
    uint8 i = 0;
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
    for(i = 0; i < 7; i++)
    {
        cs += setInform[i];
    }
    cs ^= 0xA5;
    setInform[7] = cs;


    memcpy(pTemp, &setInform[0], 8);
    *plenFrame = 8;


}


/*
 ******************************************************************************
 * 函数名称：Create_ElsonicWiredPanel_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于将设置亿林有线面板使能热量显示功能。
 				目前亿林有线温控面板没有此功能。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicWiredPanel_SetInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
{


}




/*
 ******************************************************************************
 * 函数名称：Create_ElsonicWiredPanel_SetHeat_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于将热量值写入亿林有线温控面板。
 				目前亿林有线温控面板没有此功能。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicWiredPanel_SetHeat_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{


}

/*
 ******************************************************************************
 * 函数名称：Create_ElsonicWiredPanel_SetRoomTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于设置亿林有线温控面板室内温度。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicWiredPanel_SetRoomTemp_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[8] = {0xa9, 0x19, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x7b};
    uint8 cs 	   = 0x00;
    uint8 *pTemp;
    uint8 i = 0;
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


    setInform[6] = BCD_TO_HEX(*(p_DataIn + 1));  //设定温度,此处需要调整正确偏量。
    if(setInform[6] < 5)	//温度设定范围5-35℃。
        setInform[6] = 5;
    if(setInform[6] > 35)
        setInform[6] = 35;

    cs = 0;
    for(i = 0; i < 7; i++)
    {
        cs += setInform[i];
    }
    cs ^= 0xA5;
    setInform[7] = cs;


    memcpy(pTemp, &setInform[0], 8);
    *plenFrame = 8;


}

/*
 ******************************************************************************
 * 函数名称：Create_ElsonicWiredPanel_SetRoomTempRange_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于设置亿林有线温控面板室内温度上下限。
 				目前亿林有线温控面板没有此功能。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicWiredPanel_SetRoomTempRange_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{


}


/*
 ******************************************************************************
 * 函数名称：Create_ElsonicWiredPanel_SetValve_Frame(DELU_Protocol *pData, uint8 valvestate,uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于强制亿林有线温控面板开关阀。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicWiredPanel_SetValve_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[8] = {0xa3, 0x19, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x7b};
    uint8 cs 	   = 0x00;
    uint8 *pTemp;
    uint8 i = 0;
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
        setInform[3] = 0x08;  //协议有误，以此为准。
    else if(*p_DataIn == 0x55)
        setInform[3] = 0x04;
    else if(*p_DataIn == 0x09)  //阀正常工作，还是由温控面板控制。
        setInform[3] = 0;
    else
        setInform[3] = 0x04;//其他也全部打开阀门，安全。

    cs = 0;
    for(i = 0; i < 7; i++)
    {
        cs += setInform[i];
    }
    cs ^= 0xA5;
    setInform[7] = cs;


    memcpy(pTemp, &setInform[0], 8);
    *plenFrame = 8;


}


