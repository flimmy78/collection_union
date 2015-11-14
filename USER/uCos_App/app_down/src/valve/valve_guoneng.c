/*
  ********************************************************************************************************
  * @file    valve_guoneng.c
  * @author  zjjin
  * @version V0.0.0
  * @date    10-15-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		威海国能自控科技"国能温控阀"用。
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
  * 函数名称: uint8 ValveContron_GuoNeng(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)

  * 说    明：威海国能阀控器控制函数，在此函数中实现威海国能阀控不同控制。
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

uint8 ValveContron_GuoNeng(MeterFileType *p_mf, uint8 functype, uint8 *p_DataIn, uint8 *p_databuf, uint8 *p_datalenback)
{
    uint8 Err = 0;
    uint8 lu8dataframe[100] = {0};
    uint8 lu8datalen = 0;
    uint8 lu8databuf[20] = {0xee};
    uint8 lu8datalenback = 0;
    uint8 i = 0;
    //	int8 l8tmp = 0;
    uint8 lu8tmp = 0;



    switch(functype)
    {
    case ReadVALVE_All:
    {
        Create_GuoNengVave_ReadInfo_Frame(p_mf, lu8dataframe, &lu8datalen); //组建获取阀门数据命令帧
        Err = GuonengValve_ReadData_Communicate(lu8dataframe, &lu8datalen);

        if(Err == NO_ERR)
        {
            lu8databuf[i++] = lu8dataframe[22] & 0x0f;
            lu8datalenback++;
            lu8tmp = lu8dataframe[22] >> 4;
            lu8tmp += lu8dataframe[23] << 4;
            lu8databuf[i++] = lu8tmp;
            lu8datalenback++;
            lu8databuf[i++] = 0x00;  //符号位
            lu8datalenback++;

            lu8tmp = BCD_TO_HEX(lu8dataframe[25]);  //国能阀开度是百分比，BCD码2字节。
            lu8tmp = lu8tmp * 100;
            lu8tmp += BCD_TO_HEX(lu8dataframe[24]);
            if(lu8tmp > 90)
                lu8databuf[i++] = 0x55;  // 全开
            else if(lu8tmp > 75)
                lu8databuf[i++] = 0x66;  // 3/4开。
            else if(lu8tmp > 40)
                lu8databuf[i++] = 0x77;  // 2/4开
            else if(lu8tmp > 20)
                lu8databuf[i++] = 0x88;  // 1/4
            else
                lu8databuf[i++] = 0x99;//全关。

            lu8datalenback++;


            //begin:阀控状态位字节处理。
            lu8databuf[4] = 0; //先将状态字节初始化为0 。
            lu8tmp = lu8dataframe[29];
            if(lu8tmp & 0x04)  //无线异常。
                lu8databuf[4] |= 0x01;

            //lu8databuf[4] |= 0x02;  //国能阀控没有面板开关标志，所以不用判断。

            lu8tmp = lu8dataframe[20];
            if(lu8tmp & 0x02)  //面板锁定是否。
                lu8databuf[4] |= 0x04;

            //if(lu8tmp & 0x02)  //阀门是否锁定。
            //	lu8databuf[4] |= 0x08;
            //if(lu8tmp & 0x80)  //阀门是否锁定。
            //	lu8databuf[4] |= 0x08;
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


    case SETHEAT_VALUE:   //国能热量显示功能不加。
    {

        break;
    }

    case SETROOM_TEMP:
    {
        Create_GuoNengVave_SetRoomTemp_Frame(p_mf, p_DataIn, lu8dataframe, &lu8datalen);
        Err = GuonengValve_ReadData_Communicate(lu8dataframe, &lu8datalen);

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

    case SETTEMP_RANGE:   //国能不能设定温度范围，只能是最高35℃，不做处理就是35摄氏度。
    {


        break;
    }

    case SETVALVE_STATUS:   //威海国能温控器不能强制开阀，在阀门非强制关闭时由面板决定开关。
    {
        //程序中没有写强制关阀，所以此处为空。
        break;
    }

    case SETVALVE_CONTROLTYPE:
    {

        break;
    }


    default:
        break;
    }



    return Err;
}



/*
 ******************************************************************************
 * 函数名称：Create_GuoNengVave_ReadInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于读取国能阀控器状态信息。
 * 参	 数：
 ******************************************************************************
 */
void Create_GuoNengVave_ReadInfo_Frame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[20] = {0xFE, 0xFE, 0xFE, 0xFE, 0x68, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x09, 0x01, 0x03, 0x82, 0x1F, 0x00, 0x87, 0x16};
    uint8 cs 	   = 0x00;
    uint8 *pTemp;
    uint8 i = 0;
    //uint8 lu8valveaddr[7] = {0x00};
    //uint16 lu16valveaddr =0;

    LOG_assert_param(p_mf == NULL);
    LOG_assert_param(pSendFrame == NULL);
    LOG_assert_param(plenFrame == NULL);

    pTemp = pSendFrame;

    memcpy(setInform + 6, &p_mf->ValveAddr[0], 7);

    cs = 0;
    for(i = 4; i <= 17; i++)
    {
        cs += setInform[i];
    }

    setInform[18] = cs;


    memcpy(pTemp, &setInform[0], 20);
    *plenFrame = 20;


}


/*
 ******************************************************************************
 * 函数名称：Create_GuoNengVave_SetHeat_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于将热量值写入阀控器。
 * 参	 数：
 ******************************************************************************
 */
void Create_GuoNengVave_SetHeat_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{



}



/*
 ******************************************************************************
 * 函数名称：Create_ElsonicVave_SetRoomTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * 说	 明：此函数用于设置室内温度。
 * 参	 数：
 ******************************************************************************
 */
void Create_GuoNengVave_SetRoomTemp_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 setInform[30] = {0xFE, 0xFE, 0xFE, 0xFE, 0x68, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x09, 0x04, 0x0B, 0x93, 0x2F, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x16};
    uint8 cs 	  = 0x00;
    uint8 *pTemp;
    uint8 i = 0;

    LOG_assert_param(p_mf == NULL);
    LOG_assert_param(pSendFrame == NULL);
    LOG_assert_param(plenFrame == NULL);

    pTemp = pSendFrame;

    memcpy(setInform + 6, &p_mf->ValveAddr[0], 7);

    setInform[18] = *(p_DataIn + 1);
    if(setInform[18] < 0x05)
        setInform[18] = 0x05;
    if(setInform[18] > 0x35)
        setInform[18] = 0x35;

    cs = 0;
    for(i = 4; i <= 25; i++)
    {
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
    uint8 len = 0;
    uint8 DataBuf[50];
    uint16 OutTime = OS_TICKS_PER_SEC * 2;

    LOG_assert_param(DataFrame == NULL);
    LOG_assert_param(DataLen == NULL);

    DuQueueFlush(dev);				 //清空缓冲区
    DuSend(dev, (uint8 *)DataFrame,  *DataLen);

    err = METER_ReceiveFrame(dev, DataBuf, OutTime, &len);

    if(err == NO_ERR)
    {
        memcpy(DataFrame, &DataBuf[0], len);
        *DataLen = len;
        return NO_ERR;
    }


    return 1;
}


