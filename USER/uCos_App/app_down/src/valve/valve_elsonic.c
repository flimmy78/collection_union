/*
  ********************************************************************************************************
  * @file    valve_elsonic.c
  * @author  zjjin
  * @version V0.0.0
  * @date    04-15-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		亿林协议阀控器功能实现相关代码。
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

uint8 ValveContron_Elsonic(MeterFileType *p_mf, uint8 functype, uint8 *p_DataIn, uint8 *p_databuf, uint8 *p_datalenback)
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
        Create_ElsonicVave_ReadInfo_Frame(p_mf, lu8dataframe, &lu8datalen); //组建获取阀门数据命令帧
        Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);

        if(Err == NO_ERR)
        {
            l8tmp = (int8)lu8dataframe[4];  //房间温度+补偿温度。
            l8tmp += lu8dataframe[8];
            lu8tmp = (uint8)l8tmp;
            lu8databuf[i++] = 0x00;  //小数位固定为0.
            lu8datalenback++;
            lu8databuf[i++] = HEX_TO_BCD(lu8tmp);
            lu8datalenback++;
            lu8databuf[i++] = 0x00;  //符号位
            lu8datalenback++;

            lu8tmp = lu8dataframe[7];
            if(lu8tmp & 0x01)	   //如果bit0=1表示阀门开状态，0关状态。
                lu8databuf[i++] = 0x55;  // 全开
            else
                lu8databuf[i++] = 0x99;  //全关。

            lu8datalenback++;

            //begin:阀控状态位字节处理。
            lu8databuf[4] = 0; //先将状态字节初始化为0 。
            lu8tmp = lu8dataframe[7];
            if(lu8tmp & 0x02)  //无线异常。
                lu8databuf[4] |= 0x01;

            lu8tmp = lu8dataframe[3];
            if((lu8tmp & 0x01) == 0)  //面板开关
                lu8databuf[4] |= 0x02;

            if(lu8tmp & 0x04)  //面板锁定是否。
                lu8databuf[4] |= 0x04;

            if(lu8tmp & 0x02)  //阀门是否锁定。
                lu8databuf[4] |= 0x08;
            if(lu8tmp & 0x80)  //阀门是否锁定。
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
        Create_ElsonicVave_SetInfo_Frame(p_mf, lu8dataframe, &lu8datalen);	//设置请用热量显示。
        Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);
        if(Err != NO_ERR)
        {
            debugX(LOG_LEVEL_ERROR, "%s %d Send HeatMeter data  to valve  failed!\r\n", __FUNCTION__, __LINE__);
        }

        memcpy(p_databuf, lu8dataframe, lu8datalen);
        *p_datalenback = lu8datalen;

        break;
    }

    case SETHEAT_VALUE:
    {
        Create_ElsonicVave_SetHeat_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
        Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);
        if(Err == NO_ERR)
        {
            debug_info(gDebugModule[TASKDOWN_MODULE], "%s %d Send HeatMeter data  to valve ok", __FUNCTION__, __LINE__);
        }
        else
        {
            debugX(LOG_LEVEL_ERROR, "%s %d Send HeatMeter data  to valve  failed!\r\n", __FUNCTION__, __LINE__);
        }

        memcpy(p_databuf, lu8dataframe, lu8datalen);
        *p_datalenback = lu8datalen;

        break;
    }

    case SETROOM_TEMP:
    {
        Create_ElsonicVave_SetRoomTemp_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
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
        Create_ElsonicVave_SetRoomTempRange_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
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

    case SETVALVE_STATUS:
    {
        Create_ElsonicVave_SetValve_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
        Err = Elsonic_ReadData_Communicate(lu8dataframe, &lu8datalen);

        memcpy(p_databuf, lu8dataframe, lu8datalen);
        *p_datalenback = lu8datalen;

        break;
    }

    case SETVALVE_CONTROLTYPE:
    {
        if(*(p_DataIn + 0) == 0x09) //亿林阀只有重新使能阀控器由温控面板控制时才执行。
        {
            Create_ElsonicVave_SetValve_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
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
 * 函数名称：Create_ElsonicVave_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于读取亿林阀控器状态信息。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicVave_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
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
 * 函数名称：Create_ElsonicVave_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于将设置亿林阀控器使能热量显示功能。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicVave_SetInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[8] = {0xb1, 0x19, 0x0c, 0x08, 0x00, 0x00, 0x00, 0x7b};
    //uint8 templen   = 0x00;
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
    setInform[3] = 0x08; // 启用热量显示功能。

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
 * 函数名称：Create_ElsonicVave_SetHeat_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于将热量值写入亿林阀控器。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicVave_SetHeat_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[8] = {0xa4, 0x19, 0x0c, 0x08, 0x00, 0x00, 0x00, 0x7b};
    //uint8 templen   = 0x00;
    uint8 cs 	   = 0x00;
    uint8 *pTemp;
    //		   uint16 lenFrame = 0x00;
    //		   uint16 CheckLen = 0x00;
    uint8 i = 0;
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

    memcpy(lu8currentheat, p_DataIn, 4);			  //当前热量取出。
    setInform[3] = lu8currentheat[1];
    setInform[4] = lu8currentheat[2];
    setInform[5] = lu8currentheat[3];
    setInform[6] = 0x00;

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
 * 函数名称：Create_ElsonicVave_SetRoomTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于设置室内温度。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicVave_SetRoomTemp_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[8] = {0xa6, 0x19, 0x0c, 0x00, 0x00, 0x32, 0x00, 0x7b};
    //		   uint8 templen   = 0x00;
    uint8 cs 	   = 0x00;
    uint8 *pTemp;
    //		   uint16 lenFrame = 0x00;
    //		   uint16 CheckLen = 0x00;
    uint8 i = 0;
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


    setInform[5] = BCD_TO_HEX(*(p_DataIn + 1));  //设定温度,此处需要调整正确偏量。
    if(setInform[5] < 10)	//温度设定范围10-30℃。
        setInform[5] = 10;
    if(setInform[5] > 30)
        setInform[5] = 30;

    setInform[5] = 2 * setInform[5]; 	 //  设定室内温度，设定值是温度值的2倍。

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
 * 函数名称：Create_ElsonicVave_SetRoomTempRange_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于设置室内温度上下限。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicVave_SetRoomTempRange_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[8] = {0xb0, 0x19, 0x0c, 0xa5, 0x3c, 0xa5, 0x26, 0x7b};
    //		   uint8 templen   = 0x00;
    uint8 cs 	   = 0x00;
    uint8 *pTemp;
    //		   uint16 lenFrame = 0x00;
    //		   uint16 CheckLen = 0x00;
    uint8 i = 0;
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


    setInform[4] = BCD_TO_HEX(*(p_DataIn + 1)); //设定温度上限值。
    if(setInform[4] < 20)	//上限范围20-30℃。
        setInform[4] = 20;
    if(setInform[4] > 30)	//上限范围20-30℃。
        setInform[4] = 30;

    setInform[4] = 2 * setInform[4]; 	 //  设定值是温度值的2倍。

    setInform[6] = BCD_TO_HEX(*(p_DataIn + 4)); //设定温度下限值。
    if(setInform[6] < 10)	//上限范围10-19℃。
        setInform[6] = 10;
    if(setInform[6] > 19)
        setInform[6] = 19;

    setInform[6] = 2 * setInform[6]; 	 //  设定值是温度值的2倍。

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
 * 函数名称：Create_ElsonicVave_SetValve_Frame(DELU_Protocol *pData, uint8 valvestate,uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于强制开关阀。
 * 参	 数：
 ******************************************************************************
 */
void Create_ElsonicVave_SetValve_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[8] = {0xa9, 0x19, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x7b};
    //		   uint8 templen   = 0x00;
    uint8 cs 	   = 0x00;
    uint8 *pTemp;
    //		   uint16 lenFrame = 0x00;
    //		   uint16 CheckLen = 0x00;
    uint8 i = 0;
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
    else if(*p_DataIn == 0x09)  //阀正常工作，还是由温控面板控制。
        setInform[3] = 0;
    else
        setInform[3] = 0x02;//其他也全部打开阀门，安全。

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



uint8 Elsonic_ReadData_Communicate(uint8 *DataFrame, uint8 *DataLen)
{

    uint8 err;
    //uint8 RetryTimes	 = 0x02;
    //		uint8 data=0;
    uint8 dev = DOWN_COMM_DEV_MBUS;
    //		uint8 i=0;
    uint8 len = 0;

    uint8 DataBuf[50];
    LOG_assert_param(DataFrame == NULL);
    LOG_assert_param(DataLen == NULL);

    //for(i=0;i<RetryTimes;i++){
    DuQueueFlush(dev);				 //清空缓冲区
    DuSend(dev, (uint8 *)DataFrame,  *DataLen);

    //OSTimeDly(OS_TICKS_PER_SEC/10);

    err = Receive_Read_ElsonicParamFrame(dev, DataBuf, 0, &len);

    if(err == NO_ERR)
    {
        memcpy(DataFrame, &DataBuf[0], len);
        *DataLen = len;
        return NO_ERR;
    }
    //else{
    //	OSTimeDlyHMSM(0,0,0,500);    //如果不成功则补抄，间隔500ms。
    //}

    // }

    return 1;
}


uint8 Receive_Read_ElsonicParamFrame(uint8 dev, uint8 *buf, uint16 Out_Time, uint8 *datalen)
{
    uint8 data = 0;
    uint8 i = 0;
    uint8 readinfom[32] = {0};
    //	   uint8 len = 0;
    //	   uint8 len1 = 0;
    uint8 Cs = 0;

    i = 10;

    while(i--) 												   //找帧头
    {
        if(DuGetch(dev, &data, 3 * OS_TICKS_PER_SEC))
        {
            return 1;
        }

        if(data == 0x50) //找到帧头
        {
            break;
        }

    }

    readinfom[0] = 0x50;
    Cs += readinfom[0];

    for(i = 1; i < 11; i++)
    {
        if(DuGetch(dev, &data, 3 * OS_TICKS_PER_SEC))
        {
            return 2;
        }
        readinfom[i] = data;
        Cs  += data;
    }

    Cs ^= 0xa5;

    if(DuGetch(dev, &data, 3 * OS_TICKS_PER_SEC))
    {
        return 3;
    }
    readinfom[11] = data;

    if(Cs != readinfom[11])
    {
        return 4;
    }

    *datalen = 12;
    memcpy(buf, &readinfom[0], *datalen);

    return 0;


}

