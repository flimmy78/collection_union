/*
  ********************************************************************************************************
  * @file    valve_binihi.c
  * @author  zjjin
  * @version V0.0.0
  * @date    04-20-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		百暖汇协议阀控器功能实现相关代码。
  *
  ********************************************************************************************************
  */

//#include <includes.h>

#include "Valve.h"
#include "valve_binihi.h"


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

uint8 ValveContron_Binihi(MeterFileType *p_mf, uint8 functype, uint8 *p_DataIn, uint8 *p_databuf, uint8 *p_datalenback)
{
    uint8 Err = 0;
    uint8 lu8dataframe[100] = {0};
    uint8 lu8datalen = 0;
    uint8 lu8databuf[20] = {0xee};
    uint8 lu8datalenback = 0;
    uint8 i = 0;
    //	uint8 lu8tmp = 0;
    uint8 lu8valvestatus = 0;



    switch(functype)
    {
    case ReadVALVE_All:
    {
        Create_Binihi_Vave_ReadInfo_Frame(p_mf, lu8dataframe, &lu8datalen); //组建获取阀门数据命令帧
        Err = Binihi_ReadData_Communicate(lu8dataframe, &lu8datalen);

        if(Err == NO_ERR)
        {
            lu8databuf[i++] = 0x00;  //小数位固定为0.
            lu8datalenback++;
            lu8databuf[i++] = HEX_TO_BCD(lu8dataframe[13]);
            lu8datalenback++;
            lu8databuf[i++] = 0x00;  //符号位
            lu8datalenback++;

            lu8databuf[4] = 0; //先将状态字节初始化为0 。

            lu8valvestatus = (lu8dataframe[9] >> 5) & 0x03;
            switch(lu8valvestatus)
            {
            case 0x00:   //解锁关阀
            {
                lu8databuf[i++] = 0x99;  //全关。

                break;
            }
            case 0x01:  //解锁开阀。
            {
                lu8databuf[i++] = 0x55;  // 全开
                break;
            }
            case 0x02:  //上锁关阀
            {
                lu8databuf[i++] = 0x99;  //全关。
                lu8databuf[4] |= 0x08;  //指示阀门锁定。
                break;
            }
            case 0x03:  //上锁开阀
            {
                lu8databuf[i++] = 0x55;  // 全开
                lu8databuf[4] |= 0x08;  //指示阀门锁定。
                break;
            }
            default:
            {
                lu8databuf[i++] = 0x99;  //全关。
                break;

            }

            }

            lu8datalenback++;

            //begin:阀控状态位字节处理。

            //阀门是否锁定在上面的switch中已经判断。
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
        Create_Binihi_Vave_SetInfo_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
        Err = Binihi_ReadData_Communicate(lu8dataframe, &lu8datalen);
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
        Err = 1;    //百暖汇只能同时设定温度和上下限，单设温度报错。

        break;
    }

    case SETTEMP_RANGE:
    {
        Err = 1;   //百暖汇只能同时设定温度和上下限，单设上下限报错。

        break;
    }

    case SETVALVE_STATUS:
    {
        Create_Binihi_SetValve_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
        Err = Binihi_ReadData_Communicate(lu8dataframe, &lu8datalen);

        memcpy(p_databuf, lu8dataframe, lu8datalen);
        *p_datalenback = lu8datalen;

        break;
    }

    case SETTEMP_ANDRANGE:
    {
        Create_Binihi_SetTemp_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
        Err = Binihi_ReadData_Communicate(lu8dataframe, &lu8datalen);

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

    case SETVALVE_CONTROLTYPE:
    {
        if(*(p_DataIn + 0) == 0x09)
        {
            Create_Binihi_SetValve_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
            Err = Binihi_ReadData_Communicate(lu8dataframe, &lu8datalen);

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
  * 函数名称： void Create_Binihi_Vave_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
  * 说    明：此函数用于将当前热量值写入百暖汇阀控器。
  * 参    数：
  ******************************************************************************
  */
void Create_Binihi_Vave_SetInfo_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    /*
    {
    uint32   DailyHeat;		                结算日热量	   +0
    uint8     DailyHeatUnit;                                                   +4
    uint32   CurrentHeat;				   当前热量    +5
    uint8     CurrentHeatUnit;                                                +9
    uint32 	 HeatPower;			      热功率             +10
    uint8     HeatPowerUnit;                                                  +14
    uint32 	 Flow;			                 流量流速   +15
    uint8 	 FlowUnit;                                                            +19
    uint32 	AccumulateFlow;			累积流量       +20
    uint8 	AccumulateFlowUnit;	                                         +24

    uint8	WaterInTemp[3];		        供水温度	 +25
    uint8 	WaterOutTemp[3];				回水温度 +28
    uint8 	AccumulateWorkTime[3];	累计工作时间   +31
    uint8	RealTime[7];		                实时时间	  +34
    uint16 	ST;					                    状态ST+41
    }*/
    uint8   setInform[23] = {0x68, 0x11, 0x11, 0x68, 0x53, 0x00, 0x00, 0x00, 0x53,
                             0x60, 0x1C, 0x10, 0x1E, 0x00, 0x00, 0x00, 0x2B, 0xA1, 0x07, 0x03, 0x01, 0x07, 0x16
                            };

    //	uint8 templen	= 0x00;
    uint8 cs		= 0x00;
    uint8 *pTemp;
    //    uint16 lenFrame = 0x00;
    //    uint16 CheckLen = 0x00;
    uint8 i = 0;
    uint8 lu8currentheat[4] = {0x00};
    uint32 lu32currentheat = 0;
    uint8 lu8valveaddr[7] = {0x00};
    uint16 lu16valveaddr = 0;
    uint8 lu8PanelAddr[7] = {0}; //温控面板的地址
    uint16 lu16PanelAddr = 0;

    LOG_assert_param(p_mf == NULL);
    LOG_assert_param(pSendFrame == NULL);
    LOG_assert_param(plenFrame == NULL);

    pTemp = pSendFrame;

    memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);
    for(i = 0; i < 7; i++)
    {
        lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
    }
    lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1] * 100 + lu8valveaddr[2] * 10000;

    setInform[5] = lu16valveaddr & 0x00ff;
    setInform[6] = (lu16valveaddr >> 8) & 0x00ff;

    memcpy(lu8PanelAddr, p_mf->ControlPanelAddr, 7);
    for(i = 0; i < 7; i++)
    {
        lu8PanelAddr[i] = BCD_TO_HEX(lu8PanelAddr[i]);
    }
    lu16PanelAddr = lu8PanelAddr[0] + lu8PanelAddr[1] * 100 + lu8PanelAddr[2] * 10000;
    if(lu16PanelAddr > 0x07)  //因为百暖汇子机号范围为0-7，如果超出此范围，则默认0.
        lu16PanelAddr = 0;
    else
        lu16PanelAddr = lu8PanelAddr[0];

    setInform[7] = (uint8)lu16PanelAddr;


    memcpy(lu8currentheat, p_DataIn, 4);			  //当前热量取出。
    for(i = 0; i < 4; i++)
    {
        lu8currentheat[i] = BCD_TO_HEX(lu8currentheat[i]);
    }
    lu32currentheat = lu8currentheat[1] + lu8currentheat[2] * 100 + lu8currentheat[3] * 10000; //当前热量的整数部分
    lu32currentheat = lu32currentheat * 10 + lu8currentheat[0] / 10; //添加小数部分，并且扩大10倍。

    setInform[16] = lu32currentheat & 0x00ff;
    setInform[17] = (lu32currentheat >> 8) & 0x00ff;
    setInform[18] = (lu32currentheat >> 16) & 0x00ff;
    setInform[19] = 0x00;  //设置其他无效，只写入当前热量值。

    cs = 0;
    for(i = 9; i < 21; i++)
    {
        cs += setInform[i];
    }
    cs += setInform[4];
    cs += setInform[8];
    setInform[21] = cs;


    memcpy(pTemp, &setInform[0], 23);
    *plenFrame = 23;



}



/*
  ******************************************************************************
  * 函数名称： void Create_Binihi_SetValve_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 valvestate,uint8 *plenFrame)
  * 说    明：此函数用于设置百暖汇阀开关状态，
  * 参    数： valvestate = 1  锁定开阀
  				valvestate = 0 锁定关阀
  				valvestate = 2 锁定，上位机不控制阀门
  				valvestate = other，解锁，阀门由面板控制。
  ******************************************************************************
  */
void Create_Binihi_SetValve_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8   setInform[23] = {0x68, 0x11, 0x11, 0x68, 0x53, 0x55, 0x01, 0x00, 0x53,
                             0x60, 0x1C, 0x10, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x07, 0x16
                            };

    //	uint8 templen	= 0x00;
    uint8 cs		= 0x00;
    uint8 *pTemp;
    //    uint16 lenFrame = 0x00;
    //    uint16 CheckLen = 0x00;
    uint8 i = 0;
    uint8 lu8valveaddr[7] = {0x00};
    uint16 lu16valveaddr = 0;
    uint8 lu8PanelAddr[7] = {0}; //温控面板的地址
    uint16 lu16PanelAddr = 0;

    LOG_assert_param(p_mf == NULL);
    LOG_assert_param(pSendFrame == NULL);
    LOG_assert_param(plenFrame == NULL);

    pTemp = pSendFrame;

    memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);
    for(i = 0; i < 7; i++)
    {
        lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
    }
    lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1] * 100 + lu8valveaddr[2] * 10000;

    setInform[5] = lu16valveaddr & 0x00ff;
    setInform[6] = (lu16valveaddr >> 8) & 0x00ff;

    memcpy(lu8PanelAddr, p_mf->ControlPanelAddr, 7);
    for(i = 0; i < 7; i++)
    {
        lu8PanelAddr[i] = BCD_TO_HEX(lu8PanelAddr[i]);
    }
    lu16PanelAddr = lu8PanelAddr[0] + lu8PanelAddr[1] * 100 + lu8PanelAddr[2] * 10000;
    if(lu16PanelAddr > 0x07)  //因为百暖汇子机号范围为0-7，如果超出此范围，则默认0.
        lu16PanelAddr = 0;
    else
        lu16PanelAddr = lu8PanelAddr[0];

    setInform[7] = (uint8)lu16PanelAddr;


    switch(*(p_DataIn + 0))
    {
    case 0x99:  //关
    {
        setInform[9] = 0x20;
        break;
    }
    case 0x55:	//开
    {
        setInform[9] = 0x00;
        break;
    }
    case 0x09:  //解锁，阀门由温控面板控制。
    {
        setInform[9] = 0x60;
        break;
    }
    default:
    {
        setInform[9] = 0x00;  //其他全开阀，安全第一。
        break;
    }


    }

    setInform[19] = 0x01;  //阀控字节有效。

    cs = 0;
    for(i = 9; i < 21; i++)
    {
        cs += setInform[i];
    }
    cs += setInform[4];
    cs += setInform[8];
    setInform[21] = cs;


    memcpy(pTemp, &setInform[0], 23);
    *plenFrame = 23;



}


/*
  ******************************************************************************
  * 函数名称：
  void Create_Binihi_SetTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame,uint8 *plenFrame)

  * 说    明：此函数用于设置百暖汇阀室内温度和上下限温度。
  * 参    数：
  ******************************************************************************
  */
void Create_Binihi_SetTemp_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8   setInform[23] = {0x68, 0x11, 0x11, 0x68, 0x53, 0x55, 0x01, 0x00, 0x53,
                             0x60, 0x1C, 0x10, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x07, 0x16
                            };

    //	uint8 templen	= 0x00;
    uint8 cs		= 0x00;
    uint8 *pTemp;
    //    uint16 lenFrame = 0x00;
    //    uint16 CheckLen = 0x00;
    uint8 i = 0;
    uint8 lu8valveaddr[7] = {0x00};
    uint16 lu16valveaddr = 0;
    uint8 lu8PanelAddr[7] = {0}; //温控面板的地址
    uint16 lu16PanelAddr = 0;

    LOG_assert_param(p_mf == NULL);
    LOG_assert_param(pSendFrame == NULL);
    LOG_assert_param(plenFrame == NULL);

    pTemp = pSendFrame;

    memcpy(lu8valveaddr, &p_mf->ValveAddr[0], 7);
    for(i = 0; i < 7; i++)
    {
        lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
    }
    lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1] * 100 + lu8valveaddr[2] * 10000;

    setInform[5] = lu16valveaddr & 0x00ff;
    setInform[6] = (lu16valveaddr >> 8) & 0x00ff;

    memcpy(lu8PanelAddr, p_mf->ControlPanelAddr, 7);
    for(i = 0; i < 7; i++)
    {
        lu8PanelAddr[i] = BCD_TO_HEX(lu8PanelAddr[i]);
    }
    lu16PanelAddr = lu8PanelAddr[0] + lu8PanelAddr[1] * 100 + lu8PanelAddr[2] * 10000;
    if(lu16PanelAddr > 0x07)  //因为百暖汇子机号范围为0-7，如果超出此范围，则默认0.
        lu16PanelAddr = 0;
    else
        lu16PanelAddr = lu8PanelAddr[0];

    setInform[7] = (uint8)lu16PanelAddr;


    setInform[10] = BCD_TO_HEX(*(p_DataIn + 1)); //设定温度

    setInform[11] = BCD_TO_HEX(*(p_DataIn + 7)); //设定下限温度

    setInform[12] = BCD_TO_HEX(*(p_DataIn + 4)); //设定上限温度。




    setInform[19] = 0x02;  //设定上下限温度和室内温度有效。

    cs = 0;
    for(i = 9; i < 21; i++)
    {
        cs += setInform[i];
    }
    cs += setInform[4];
    cs += setInform[8];
    setInform[21] = cs;


    memcpy(pTemp, &setInform[0], 23);
    *plenFrame = 23;



}


void Create_Binihi_Vave_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
{

    uint8 readinfo[7] = {0x10, 0x5b, 0x34, 0x12, 0x00, 0x00, 0x16}; //默认表号:1234

    //	uint8 templen	= 0x00;
    uint8 cs		= 0x00;
    uint8 *pTemp;
    //	uint16 lenFrame = 0x00;
    //	uint16 CheckLen = 0x00;
    uint8 i = 0;
    uint8 lu8valveaddr[7] = {0};
    uint16 lu16valveaddr = 0;
    uint8 lu8PanelAddr[7] = {0}; //温控面板的地址
    uint16 lu16PanelAddr = 0;
    LOG_assert_param(p_mf == NULL);
    LOG_assert_param(pSendFrame == NULL);
    LOG_assert_param(plenFrame == NULL);

    pTemp = pSendFrame;

    memcpy(lu8valveaddr, p_mf->ValveAddr, 7);
    for(i = 0; i < 7; i++)
    {
        lu8valveaddr[i] = BCD_TO_HEX(lu8valveaddr[i]);
    }
    lu16valveaddr = lu8valveaddr[0] + lu8valveaddr[1] * 100 + lu8valveaddr[2] * 10000;
    readinfo[2] = lu16valveaddr & 0x00ff;
    readinfo[3] = (lu16valveaddr >> 8) & 0x00ff;

    memcpy(lu8PanelAddr, p_mf->ControlPanelAddr, 7);
    for(i = 0; i < 7; i++)
    {
        lu8PanelAddr[i] = BCD_TO_HEX(lu8PanelAddr[i]);
    }
    lu16PanelAddr = lu8PanelAddr[0] + lu8PanelAddr[1] * 100 + lu8PanelAddr[2] * 10000;
    if(lu16PanelAddr > 0x07)  //因为百暖汇子机号范围为0-7，如果超出此范围，则默认0.
        lu16PanelAddr = 0;
    else
        lu16PanelAddr = lu8PanelAddr[0];

    readinfo[4] = (uint8)lu16PanelAddr;

    for(i = 1; i < 5; i++)
    {
        cs += readinfo[i];
    }
    readinfo[5] = cs;
    memcpy(pTemp, &readinfo[0], 7);
    *plenFrame = 7;
}


uint8 Binihi_ReadData_Communicate(uint8 *DataFrame, uint8 *DataLen)
{

    uint8 err;
    uint8 RetryTimes	  = 0x02;
    //	 uint8 data=0;
    uint8 dev = DOWN_COMM_DEV_MBUS;
    uint8 i = 0;
    uint8 len = 0;

    uint8 DataBuf[METER_FRAME_LEN_MAX * 2];
    LOG_assert_param(DataFrame == NULL);
    LOG_assert_param(DataLen == NULL);

    for(i = 0; i < RetryTimes; i++)
    {
        DuQueueFlush(dev);				  //清空缓冲区
        DuSend(dev, (uint8 *)DataFrame,  *DataLen);

        OSTimeDly(OS_TICKS_PER_SEC / 10);

        err = Receive_ReadBinihiParamFrame(dev, DataBuf, 0, &len);

        if(err == NO_ERR)
        {
            memcpy(DataFrame, &DataBuf[0], len);
            *DataLen = len;
            return NO_ERR;
        }
        else
        {
            OSTimeDlyHMSM(0, 0, 0, 500);
        }

    }

    return 1;
}




uint8 Binihi_VaveCommunicate(uint8 *DataFrame, uint8 *DataLen)
{
    uint8 err;
    //      uint8 *cp = NULL;
    uint8 RetryTimes	  = 0x02;
    uint8 dev = DOWN_COMM_DEV_MBUS;
    uint8 i = 0;
    uint8 length = 0;
    uint8   DataBuf[METER_FRAME_LEN_MAX];
    LOG_assert_param(DataFrame == NULL);
    LOG_assert_param(DataLen == NULL);
    //  LOG_assert_param(pDataLenBack == NULL);
    for(i = 0; i < RetryTimes; i++)
    {
        DuQueueFlush(dev);										  //清空缓冲区
        DuSend(dev, (uint8 *)DataFrame,  *DataLen);

        //OSTimeDly(OS_TICKS_PER_SEC/10);
        err = Receive_ReadBinihiParamFrame(dev, DataBuf, 0, &length);
        if(err == NO_ERR)
        {
            return NO_ERR;
        }
    }


    return 1;
}




uint8 Receive_ReadBinihiParamFrame(uint8 dev, uint8 *buf, uint16 Out_Time, uint8 *datalen)
{
    uint8 data = 0;
    uint8 i = 0;
    uint8 readinfom[32] = {0};
    uint8 len = 0;
    uint8 len1 = 0;
    uint8 Cs = 0;

    i = 20;

    while(i--) 													//找帧头
    {
        if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
        {
            return 1;
        }

        if((data == 0x68) || (data == 0x5e))
        {
            break;
        }

    }

    if(data == 0x5e) //如果开始没有收到0x68，而是收到0x5e，说明是设置参数返回成功。
    {
        readinfom[0] = 0x5e;
        *datalen = 1;
        memcpy(buf, &readinfom[0], 1);

        return 0;
    }
    else
    {
        readinfom[0] = 0x68;
        if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
        {
            return 2;
        }

        len = data;
        readinfom[1] = len;

        if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
        {
            return 3;
        }
        len1 = data;
        readinfom[2] = len1;

        if(len == len1)
        {
            if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
            {
                return 4;
            }
            readinfom[3] = data;

            for(i = 4; i < 9; i++)
            {
                if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
                {
                    return 6;
                }
                readinfom[i] = data;
            }

            for(i = 9; i < len + 4; i++)
            {
                if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
                {
                    return 6;
                }
                readinfom[i] = data;
                Cs	+= data;
            }

            Cs += readinfom[4];
            Cs += readinfom[8];

            if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
            {
                return 7;
            }
            readinfom[len + 4] = data;

            if(Cs == data)   //校验是否正确。
            {
                if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
                {
                    return 9;
                }
                readinfom[len + 5] = data; //截止符号。

                *datalen = len + 6;

                memcpy(buf, &readinfom[0], *datalen);

                return 0;


            }
            else
            {
                return 8;
            }




        }
        else
        {
            return 5;
        }

    }


}

