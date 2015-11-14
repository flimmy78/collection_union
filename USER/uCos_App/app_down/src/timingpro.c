/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)
**  ��   ��   ��: timingpro.c
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��8��10��
**  ��        ��: ��ʱ�������ļ�
**	�� ��  �� ¼:
*****************************************************************************************************/

/********************************************** include *********************************************/
#include <includes.h>
#include "app_flashmem.h"
#include "app_down.h"
#include "Tasks_up.h"
#include "Ul_array.h"
/********************************************** define *********************************************/
//#define  HYDROMETER_DEBUG
#define METER_PRE 		1
#define METER_START		2

#define TIME_AREA_PATH  "/timearea"
/********************************************** global *********************************************/
TimingState	gREAD_TimingState;										//����״̬
ul_array_t  *gpValve_Array;
/********************************************** static *********************************************/
//��Э���޸ĵĻ����ϣ��ڳ����ȼ���������ʱ����Ҫ������¿�������ݳ������ֲ���2�������������֣�����2�߶������ɹ�
//�����ó���ɹ���־,   ���ܳ����Ƿ�ɹ������洢��������Ϣ
static uint8  gREAD_ReadCmplFlag[75] = {0};							//������ɱ�־

extern ul_array_t  *gpValve_Array;
extern save_timearea_info savetimeinfo;

extern uint8 gDebugBuffer[250];
extern uint8  gDownCommDev485;
uint16 gDebugLength = 0;


extern OS_EVENT *HeartFrmMbox;
extern uint16 VirtualMeterSn;

#ifdef DEMO_APP
extern uint8 Channel_1_SUCESS ;
extern uint8 Channel_2_SUCESS ;
extern uint8 Channel_3_SUCESS ;
extern uint8 Meter_R_End ;

#endif

extern void SaveVirtualMeterSn(uint16 VirtualMeterSn);
/**********************************************************************************************
**	�� ��  �� ��: CopyDatatoDisplay(uint16 MeterSN,uint8 *p_DstAddr,uint8 *p_SrcAddr,uint8 len)
**	�� ��  �� ��: ���������ݷ�����Ӧ���ݹ���ʾ����ʾ�������ʽ��һ�£�
						��Ҫ�����ɵ�³��ʽ��
**	�� ��  �� ��:
**	�� ��  �� ��:
**    ��   ��   ֵ    : ��

**********************************************************************************************/
void CopyDatatoDisplay(uint16 MeterSN, uint8 *p_DstAddr, uint8 *p_SrcAddr, uint8 len)
{
    uint8 Err = 0;
    MeterFileType mf;
    uint8 lu8tmp = 0;
    CJ188_Format  MeterData = {0, 0x05, 0, 0x05, 0, 0x17, 0, 0x35, 0, 0x2c};

    if(len > METER_FRAME_LEN_MAX)  //���޼�顣
        len = METER_FRAME_LEN_MAX;

    Err = PARA_ReadMeterInfo(MeterSN, &mf);

    memcpy(p_DstAddr, p_SrcAddr, len); //��ת�����ݵ�Ŀ�ĵ�ַ��

    if(Err == NO_ERR)
    {
        switch(mf.ProtocolVer)
        {
        case WANHUA_VER: //����򻪵�λ��ǰ�����ҽ���ʱ��Ҫ��ȥ0x33.
        {

            WANHUA_Format  Data;
            uint8 *pTemp = (uint8 *)&Data;
            uint8 i = 0;

            lu8tmp = *(p_DstAddr + 17);
            memcpy(&Data, (p_DstAddr + 18), lu8tmp);
            for(i = 0; i < lu8tmp; i++)
            {
                *pTemp -= 0x33;
                pTemp++;
            }
            MeterData.DailyHeat = Data.DailyHeat;
            MeterData.DailyHeatUnit = Data.DailyHeatUnit;
            MeterData.CurrentHeat = Data.CurrentHeat;
            MeterData.CurrentHeatUnit = Data.CurrentHeatUnit;
            MeterData.HeatPower = Data.HeatPower;
            MeterData.HeatPowerUnit = Data.HeatPowerUnit;
            MeterData.Flow = Data.Flow;
            MeterData.FlowUnit = Data.FlowUnit;
            MeterData.AccumulateFlow = Data.AccumulateFlow;
            MeterData.AccumulateFlowUnit = Data.AccumulateFlowUnit;
            memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);
            memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);
            memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
            memcpy(MeterData.RealTime, Data.RealTime, 7);
            MeterData.ST = Data.ST;

            memcpy((p_DstAddr + 18), &MeterData, lu8tmp);

            break;
        }

        default:
        {

            break;
        }

        }

    }


}

/****************************************************************************************************
**	�� ��  �� ��: READ_ReadOneMeter
**	�� ��  �� ��: ��ȡһ���������
**	�� ��  �� ��: char*path--�����ļ�·�� uint16 MeterSn -- �����
**	�� ��  �� ��:
**    ��   ��   ֵ    : ��
**	��		  ע        : char  path[] = "/2012/12/24/1530"; 1530 :DataStoreParaType=128byte + MeterSn*128byte
**     �޸ļ�¼   :1. add input para char*path 2012-12-26
                                        old:READ_ReadOneMeter(uint16 MeterSn)
**                                    new:READ_ReadOneMeter(char*path,uint16 MeterSn)
*****************************************************************************************************/
uint8 READ_ReadOneMeter(char *path, uint16 MeterSn)
{
    uint8 Err = 0;
    uint8 Res = 0;
    uint8 ReadTime[6]					   	= {0x00};
    MeterFileType MeterFile;
    //	uint32 totalHot = 0;
    //	CJ188_Format *cj188;
    uint8 MeterDataBuf[METER_FRAME_LEN_MAX] = {0x00};				//��ʱ�洢�������ı�����
    LOG_assert_param(MeterSn > METER_NUM_MAX);

    FeedTaskDog();
    ReadDateTime(ReadTime);											//��ȡ��ǰʱ��

    Res = PARA_ReadMeterInfo(MeterSn, &MeterFile);
    if(Res != NO_ERR)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <READ_ReadOneMeter> Read Meter filed failed, MeterSn is %4XH, Return is %d", MeterSn, Res);
    }

    //ym - 2014-03-02 �豸����0xb0�����ַ���Э��
    if(MeterFile.EquipmentType == 0xB0)
    {
        Res =  YILINVALVE_ReadMeterDataTiming(MeterSn, MeterDataBuf);
        if(Res == NO_ERR)
        {
            debug_info(gDebugModule[TIME_AREA], "INFO: %s  MeterSn=%d Read Successful!", __FUNCTION__, MeterSn);
        }
        else
        {
            debug_err(gDebugModule[TIME_AREA], "ERROR: %s  MeterSn=%d Read fail!", __FUNCTION__, MeterSn);
        }
    }
    else if((MeterFile.EquipmentType == 0xA0)) 			/*�ȷ����*/
    {
        Res =  HeatCost_ReadMeterDataTiming(MeterSn, MeterDataBuf, path);
        if(Res == NO_ERR)
        {
            debug_info(gDebugModule[TIME_AREA], "INFO: %s  MeterSn=%d Read Successful!", __FUNCTION__, MeterSn);
        }
        else
        {
            debug_err(gDebugModule[TIME_AREA], "ERROR: %s  MeterSn=%d Read fail!", __FUNCTION__, MeterSn);
        }
    }
    else 			 //if(MeterFile.EquipmentType == 0x20)
    {
        Res = METER_ReadMeterDataTiming(MeterSn, MeterDataBuf);			//��ȡĳһ����ļ�������

        if(Res == NO_ERR)
        {
            /*begin:yangfei modified 2012-12-24 find crruent file path*/
            OSMutexPend (FlashMutex, 0, &Err);
            Err = SDSaveData(path, MeterDataBuf, METER_FRAME_LEN_MAX, MeterSn * 128);
            OSMutexPost (FlashMutex);

            if(Err != NO_ERR)
            {
                LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!", __FUNCTION__, __LINE__ );
            }
            /*end   :yangfei modified 2012-12-24*/
            debug_info(gDebugModule[TASKDOWN_MODULE], "INFO: %s  MeterSn=%d Read Successful!", __FUNCTION__, MeterSn);
        }
        else
        {
            OSMutexPend (FlashMutex, 0, &Err);
            Err = SDSaveData(path, MeterDataBuf, METER_FRAME_LEN_MAX, MeterSn * 128);
            OSMutexPost (FlashMutex);
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <READ_ReadOneMeter> The HeatMeter is no answer!");
            //�����ó���ɹ���־���Ա������ٴ˴γ���
        }
    }

    //���������ݴ���gu8ReadDataStore���������á�
    CopyDatatoDisplay(MeterSn, gu8ReadDataStore, &MeterDataBuf[1], 66);


    return Res;

}


/****************************************************************************************************
**	�� ��  �� ��: HandleOneMBusShortMeter
**	�� ��  �� ��:����������MBUS��·���Ա���д���
**	�� ��  �� ��: char*path--�����ļ�·�� uint16 MeterSn -- �����
**	�� ��  �� ��:
**    ��   ��   ֵ    : ��
**	��		  ע        : char  path[] = "/2012/12/24/1530"; 1530 :DataStoreParaType=128byte + MeterSn*128byte
**     �޸ļ�¼   :1.

*****************************************************************************************************/
uint8 HandleOneMBusShortMeter(char *path, uint16 MeterSn)
{
    uint8 Err = 0;
    uint8 Res = 0;
    MeterFileType MeterFile;
    uint8 DataBuf[METER_FRAME_LEN_MAX]	= {0x00};
    uint8 *pTemp = DataBuf;
    uint8 DataLen = 0;
    uint8 lu8ReadTime[6] = {0};

    LOG_assert_param(MeterSn > METER_NUM_MAX);

    FeedTaskDog();

    Res = PARA_ReadMeterInfo(MeterSn, &MeterFile);
    if(Res != NO_ERR)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <READ_ReadOneMeter> Read Meter filed failed, MeterSn is %4XH, Return is %d", MeterSn, Res);
    }

    pTemp++;	//Ϊ���ݳ���Ԥ��
    memcpy(pTemp, (uint8 *) & (MeterFile.MeterID), 2); //MeterID.
    pTemp += 2;
    DataLen = 2;
    *pTemp = MeterFile.EquipmentType;  //�豸���͡�
    pTemp += 1;
    DataLen += 1;
    memcpy(pTemp, MeterFile.MeterAddr, 7);
    pTemp += 7;
    DataLen += 7;
    *pTemp = MeterFile.BulidID;  //¥�š�
    pTemp += 1;
    DataLen += 1;
    *pTemp = MeterFile.UnitID;  //��Ԫ�š�
    pTemp += 1;
    DataLen += 1;
    memcpy(pTemp, (uint8 *) & (MeterFile.RoomID), 2); //�����
    pTemp += 2;
    DataLen += 2;

    ReadDateTime(lu8ReadTime);
    memcpy(pTemp, lu8ReadTime, 3); //���볭��ǰʱ�䣬ֻ��ʱ���֡��롣
    pTemp += 3;
    DataLen += 3;

    *pTemp = 1;//MBUS��·�������ȱ����ݹ̶�Ϊff,����Ϊ1��������λ����ʾ��·��
    pTemp += 1;
    DataLen += 1;
    *pTemp = 0xff;
    pTemp += 1;
    DataLen += 1;

    ReadDateTime(lu8ReadTime);
    memcpy(pTemp, lu8ReadTime, 3); //���볭����ǰʱ�䣬ֻ��ʱ���֡��롣
    pTemp += 3;
    DataLen += 3;

    memset(pTemp, 0xee, 6); //��������ֽ�ȫ��ֵ0xee����ʾ����ʧ�ܡ�
    pTemp += 6;
    DataLen += 6;

    DataBuf[0]	= DataLen;						//�����򳤶�
    *pTemp = PUBLIC_CountCS(&DataBuf[1], DataLen);

    OSMutexPend (FlashMutex, 0, &Err);
    Err = SDSaveData(path, DataBuf, METER_FRAME_LEN_MAX, MeterSn * 128);
    OSMutexPost (FlashMutex);

    if(Err != NO_ERR)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!", __FUNCTION__, __LINE__ );
    }


    memcpy(gu8ReadDataStore, &DataBuf[1], 66); //���ճ����ı����ݴ���gu8ReadDataStore���������á�

    return Res;
}

/****************************************************************************************************
**	�� ��  �� ��: get_build_hot
**	�� ��  �� ��: ��ȡ�ܵ�����
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/

static double get_build_hot()
{
    return 123456.6	;
    //return savetimeinfo.allocHot;
}


/****************************************************************************************************
**	�� ��  �� ��: ComputeHotForError
**	�� ��  �� ��: ���������ϱ�ʧ��ʱ�������̯������
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/
static double ComputeHotForError(double area, double totalArea, double totalHot)
{
    double allHot = totalHot * area / totalArea;
    return allHot;
}


/****************************************************************************************************
**	�� ��  �� ��: ComputeHotForError
**	�� ��  �� ��: ���������ϱ��ɹ�ʱ�������̯������
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/

static double ComputeHotForOk(double rOpentime, double area, double averageTemp, double totalValue, double buildHot)
{
    double allocationhot = (rOpentime * area * buildHot * (averageTemp + 15)) / (totalValue * 33);
    return allocationhot;
}
/****************************************************************************************************
**	�� ��  �� ��: ComputeHotForError
**	�� ��  �� ��: ���㳭��ɹ��ķ��ŵ�������
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/

static double  ComputeTotalHot(double rOpentime, double area, double averageTemp)
{
    double totalValue = rOpentime * area * (averageTemp + 15) / 33;
    return totalValue;
}
/****************************************************************************************************
**	�� ��  �� ��: save_allocation_hot
**	�� ��  �� ��:������������ݵ��ļ�
**	�� ��  �� ��: path - ������ļ�·��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/

static void save_allocation_hot(char *path)
{
    uint32 loop = 0;
    uint8 Err = 0;
    ul_array_t	*pArray = gpValve_Array;

    TimeAreaArith *pValveCmd = (TimeAreaArith *)pArray->elts;
    uint8 data[128] = {0};

    data[0] = sizeof(TimeAreaArith);
    for(loop = 0; loop < pArray->nelts; loop++)
    {
        memcpy(&data[1], &pArray->elts[loop], sizeof(TimeAreaArith));
        debug("\r\nuser_id = %d\r\n", pArray->elts[loop].user_id);
        data[sizeof(TimeAreaArith) + 1] = PUBLIC_CountCS(&data[1], sizeof(TimeAreaArith)); /*yangfei modified*/
        OSMutexPend (FlashMutex, 0, &Err);
        Err = SDSaveData(path, data, METER_FRAME_LEN_MAX, (loop + 1) * 128); //��0�б�������ȱ�����
        OSMutexPost (FlashMutex);
        if(Err != NO_ERR)
        {
            debug("WARNING: %s %d SDSaveData Error(%4x)!", __FUNCTION__, __LINE__, pValveCmd[loop].address);
        }
        OSTimeDly(OS_TICKS_PER_SEC / 10);
        //debug("INFO: %s	MeterSn=%d Write Successful!",__FUNCTION__,(loop + 1));
    }
}
/****************************************************************************************************
**	�� ��  �� ��: write_to_panel
**	�� ��  �� ��:����̯�������������ǰ���������д��������
**	�� ��  �� ��:
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/


static void write_to_panel()
{
    uint32 loop = 0;
    uint8 Err = 0;
    uint8 err = 0;
    ul_array_t	*pArray = gpValve_Array;
    TimeAreaArith *pValveCmd = (TimeAreaArith *)pArray->elts;
    DELU_Protocol	ProtocoalInfo;
    uint32 hot;
    save_valve_info *valve_info;
    uint8 RetryTimes = 0;

    (*METER_ComParaSetArray[gMETER_Table[12][0]])();
    gDownCommDev485 = gMETER_Table[12][3];

    /*begin:yangfei added 20140309 for ��ֹͨ�����л�����*/
    do
    {
        FeedTaskDog();
        OSSemPend(METERChangeChannelSem, 5 * OS_TICKS_PER_SEC, &err); //����MBUSͨ���л�Ȩ��
    }
    while(err != OS_ERR_NONE);
    /*end:yangfei added 20140309 for ��ֹͨ�����л�����*/
    for(loop = 0; loop < pArray->nelts; loop++)
    {
        static uint8 channel = 7;
        RetryTimes = 2;
        debug("\r\n pValveCmd[loop].channel = %d\r\n", pValveCmd[loop].channel);
        if(channel != pValveCmd[loop].channel)
        {
            debug("\r\n change channel\r\n");
            channel = pValveCmd[loop].channel;
            METER_ChangeChannel(pValveCmd[loop].channel);
        }
        ProtocoalInfo.PreSmybolNum	= 0;
        ProtocoalInfo.MeterType 	= 0xB0;
        ProtocoalInfo.ControlCode 	= 0xA4;
        memcpy(ProtocoalInfo.MeterAddr, (char *)(&pValveCmd[loop].address), 2);

        valve_info = getValveHot(pValveCmd[loop].user_id);
        if(valve_info == NULL)
            continue;
        hot =  valve_info->oldValveHot;
        hot = hot + pValveCmd[loop].proportion_energy;
        valve_info->oldValveHot = hot;
        hot = HexToBcdUint32(hot);

        pValveCmd[loop].proportion_energy = HexToBcdUint32(pValveCmd[loop].proportion_energy);
        pValveCmd[loop].total_energy = hot;
        hot = hot / 0x100;


        debug_info(gDebugModule[TIME_AREA], "meterid=%d,meteradd=%4x:hot = %x,allochot=%4x\r\n", pValveCmd[loop].user_id, pValveCmd[loop].address, hot, pValveCmd[loop].proportion_energy / 0x100);
        memset(ProtocoalInfo.DataBuf, 0x00, METER_FRAME_LEN_MAX);

        ProtocoalInfo.DataBuf[0] = hot & 0xff;
        ProtocoalInfo.DataBuf[1] = (hot >> 8) & 0xff;
        ProtocoalInfo.DataBuf[2] = (hot >> 16) & 0xff;
        ProtocoalInfo.DataBuf[3] = (hot >> 24) & 0xff;
        do
        {
            Err = METER_DataItem(&ProtocoalInfo);
            if(Err == NO_ERR)
            {
                debug_info(gDebugModule[TIME_AREA], "%s %d:write yilin hot OK\r\n", __FUNCTION__, __LINE__);
                break;
            }
            else
            {
                debug_err(gDebugModule[TIME_AREA], "ERROR:meterid=%d,channel=%d:write yilin valve fail\r\n", pValveCmd[loop].user_id, pValveCmd[loop].channel);
                //	return Err;
            }
        }
        while(--RetryTimes);

    }
    OSSemPost(METERChangeChannelSem);
}


/****************************************************************************************************
**	�� ��  �� ��: computer_hot
**	�� ��  �� ��: ͨ��ʱ������������̯����
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/
static void Computer_hot(char *dataPath)
{
    char *path = dataPath;
    uint32 loop = 0;
    uint32 totalArea = 0;
    uint16 area = 0;
    uint16 roomtmp;
    fp64 buildHot, allocationHot, singleValueHot;
    fp64 allocationErrorHot = 0.0, totalValueHot = 0.0;
    ul_array_t	*pArray = gpValve_Array;
    TimeAreaArith *pValveCmd = (TimeAreaArith *)pArray->elts;
    buildHot = get_build_hot();

    if(buildHot == 0)
        return;
    /**begin    �����ܵ����**/
    for(loop = 0; loop < pArray->nelts; loop++)
    {
        totalArea += pValveCmd[loop].area;
    }
    /**end     �����ܵ����**/
    for(loop = 0; loop < pArray->nelts; loop++)
    {
        if((pValveCmd[loop].data_valid != Valve_Ok) || (pValveCmd[loop].open_percent == 0))
        {
            area = pValveCmd[loop].area;
            allocationHot = ComputeHotForError(area, totalArea, buildHot);

            allocationErrorHot += allocationHot;
            pValveCmd[loop].proportion_energy = allocationHot;
            debug_info(gDebugModule[TIME_AREA], "data_valid = %x||open_percent=%d! ", pValveCmd[loop].data_valid, pValveCmd[loop].open_percent);
            debug_info(gDebugModule[TIME_AREA], "userid=%d data invalid,area=%d\r\n ", pValveCmd[loop].user_id, pValveCmd[loop].area);
        }
    }
    //��������ȥ�����쳣���û�����������ʹ��ʱ�����ͨ�Ϸ����������û�������
    buildHot -= allocationErrorHot;

    //������ֵ
    // ¥����ÿ�����* �������ڿ���ʱ��ĺ�
    for(loop = 0; loop < pArray->nelts; loop++)
    {
        if(pValveCmd[loop].data_valid == Valve_Ok && pValveCmd[loop].open_percent != 0)
        {
            area = pValveCmd[loop].area;
            roomtmp = BcdToHex_16bit1(pValveCmd[loop].room_temperature / 0x100);
            singleValueHot = ComputeTotalHot(pValveCmd[loop].open_percent, area, roomtmp);
            totalValueHot += singleValueHot;
        }
    }
    //���㵥���û���������̯
    for(loop = 0; loop < pArray->nelts; loop++)
    {
        if(pValveCmd[loop].data_valid == Valve_Ok && pValveCmd[loop].open_percent != 0)
        {
            area = pValveCmd[loop].area;
            roomtmp = BcdToHex_16bit1(pValveCmd[loop].room_temperature / 0x100);
            allocationHot = ComputeHotForOk(pValveCmd[loop].open_percent, area, roomtmp, totalValueHot, buildHot);

            pValveCmd[loop].proportion_energy = allocationHot;

            debug_info(gDebugModule[TIME_AREA], "userid=%d data valid,open_percent = %d,rootmp=%d,area=%d \r\n ", pValveCmd[loop].user_id, pValveCmd[loop].open_percent, roomtmp, pValveCmd[loop].area);
        }
    }
    write_to_panel();
    save_allocation_hot(path);
}
/****************************************************************************************************
**	�� ��  �� ��: save_large_file
**	�� ��  �� ��: �������ݴ���1k�����ݣ��������ݴ���1k�󱣴����׳���
                                 �ú������ļ����ݽ��в�ֱ���
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/

static void save_large_file(const char *path, const void *buffer, uint16 NumByteToWrite, uint32 off)
{
    u32 i;
    //	u32 j;
    uint8 Err = 0;
    uint8 *pdata = (uint8 *)buffer;
    u32 loop = NumByteToWrite >> 9;
    u32 rev = NumByteToWrite & (0x200 - 1);

    for(i = 0; i < loop; i ++)
    {
        OSMutexPend (FlashMutex, 0, &Err);
        Err = SDSaveData(path, pdata + (i * 512), 512, off + (i * 512)); //��0�б�������ȱ�����
        OSMutexPost (FlashMutex);
        if(Err != NO_ERR)
        {
            debug("WARNING: %s %d SDSaveData Error!", __FUNCTION__, __LINE__);
        }
        OSTimeDly(OS_TICKS_PER_SEC / 10);
    }
    OSMutexPend (FlashMutex, 0, &Err);
    Err = SDSaveData(path, pdata + (loop * 512), rev, off + (loop * 512)); //��0�б�������ȱ�����
    OSMutexPost (FlashMutex);
    if(Err != NO_ERR)
    {
        debug("WARNING: %s %d SDSaveData Error!", __FUNCTION__, __LINE__);
    }
}


/****************************************************************************************************
**	�� ��  �� ��: READ_ReadAllMeters
**	�� ��  �� ��: ��ȡ���б����е��ȼ�����ͷ��ţ��¿����
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/
uint16 gu16ReadMeterNum = 0;  //��ʾ�������ʱ�д��󣬴˱���Ϊ���¼�����
uint16 gu16ReadMeterSuccessNum = 0;

uint8 READ_ReadAllMeters(void)
{
    uint8 i, j;
    uint8 TempNums	= 0x00;
    uint8 ReadTime[6] = {0x00};
    uint16 TempArray[METER_PER_CHANNEL_NUM] = {0x00};
    uint16 ReadingMeterSn	= 0x00;
    /*begin:yangfei added 2012-12-24 find path*/
    uint8 SystemTime[6] = {0x00};
    char  DataPath[] = "/2012/12/24/1530";
    uint8 Res = 0;
    //	uint8 Err = 0;
    uint8 err = 0;
    uint8 RetryTimes = 0;
    uint8 lu8ReadMeterTimes = 0; //��೭�������
    uint16 lu16Second = 0;  //����
    uint16 lu16ms = 0;      //����
    /*end   :yangfei added 2012-12-24*/
    CPU_SR			cpu_sr;
    ReadDateTime(ReadTime);
    //��ʼ������״̬, ���Ը�ȫ�ֱ�������Чʱ���Ϊ2������ʱ�����
    memset(gREAD_TimingState.TimingStartTime, 0x00, sizeof(gREAD_TimingState));
    memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));/*yangfei added 2013-11-17*/
    memset((uint8 *)gu8ReadDataStore, 0, sizeof(gu8ReadDataStore)); //added by zjjin,���������ã����������
    OS_ENTER_CRITICAL();
    memcpy(gREAD_TimingState.TimingStartTime, ReadTime, 6);
    gREAD_TimingState.TimingMeterNums = 0;
    /*begin:yangfei added 2012-12-24 find path*/
    memcpy(SystemTime, gSystemTime, 6);
    /*end   :yangfei added 2012-12-24*/

    gu16ReadMeterNum = 0;
    gu16ReadMeterSuccessNum = 0;
    OS_EXIT_CRITICAL();

    /*begin:yangfei added 2012-12-24 find path*/
    GetFilePath(DataPath, SystemTime, ARRAY_DAY);
    GetTimeNodeFilePath(DataPath, SystemTime, gPARA_TimeNodes);
    /*end   :yangfei added 2012-12-24*/
    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <READ_ReadAllMeters> Start Reading All of Meters!");



    /*begin:yangfei added 20140309 for ��ֹͨ�����л�����*/
    do
    {
        FeedTaskDog();
        OSSemPend(METERChangeChannelSem, 5 * OS_TICKS_PER_SEC, &err); //����MBUSͨ���л�Ȩ��
    }
    while(err != OS_ERR_NONE);
    /*end:yangfei added 20140309 for ��ֹͨ�����л�����*/

    memset(gu8MBusShortFlag, 0, sizeof(gu8MBusShortFlag)); //MBUS��·��ʶ�������

    //��ʼ������������ͳ���ʱ������
    lu8ReadMeterTimes = gPARA_ReplenishPara.MeterReplenishTimes + 1;  //�����������
    lu16Second = gPARA_ReplenishPara.MeterInterval / 1000;
    lu16ms = gPARA_ReplenishPara.MeterInterval % 1000;

    for(RetryTimes = 0; RetryTimes < lu8ReadMeterTimes; RetryTimes++) /*����ʧ�ܵı���*/
    {
        for(i = 0; i < METER_CHANNEL_NUM; i++) /*METER_CHANNEL_NUM*/
        {
            gu8NowMBUSChannel = i + 1;//��¼��ǰMBUSͨ����

            if(gu8MBusShortFlag[i] == 0)   //ֻ�б�ͨ������·�ų���
            {

                if(i == METER_CHANNEL_NUM - 1)
                {
                    METER_ChangeChannel(i + 1);	 /*485 �ȱ�ͨ��*/
                    gDownCommDev485 = DOWN_COMM_DEV_485; //��7ͨ���̶�Ϊ��RS485�ȱ�
                }
                else
                {
                    //if(!STA_MBUS_OFF())//muxiaoqing add
                    METER_ChangeChannel(i + 1);		/*ͨ���� �� 1��ʼ �л�Mbusͨ��*/
                    gDownCommDev485 = DOWN_COMM_DEV_MBUS;

                }

                OS_ENTER_CRITICAL();
                //��Ԥ�Ȱ���ͨ����ŵı���Ϣת�档
                memcpy((uint8 *)TempArray, (uint8 *)&gPARA_MeterChannel[i], gPARA_MeterChannelNum[i]*sizeof(uint16));
                TempNums = gPARA_MeterChannelNum[i];
                OS_EXIT_CRITICAL();
                if(TempNums > METER_PER_CHANNEL_NUM)  //���޼�顣
                    TempNums = METER_PER_CHANNEL_NUM;


                //�������ͨ���ı�
                for(j = 0; j < TempNums; j++)
                {
                    if(RetryTimes == 0)  //������ǲ�����
                    {
                        OS_ENTER_CRITICAL();
                        gREAD_TimingState.TimingMeterNums++;
                        gu16ReadMeterNum += 1;
                        OS_EXIT_CRITICAL();
                    }

                    ReadingMeterSn = TempArray[j];
                    debug("\r\n MeterSn = %d\r\n", ReadingMeterSn);

                    if(gPARA_Meter_Failed[i][j] == 0x00)
                    {
                        if(gu8MBusShortFlag[i] == 0)
                        {
                            Res = READ_ReadOneMeter(DataPath, ReadingMeterSn);

                            OSTimeDlyHMSM(0, 0, lu16Second, lu16ms); //����ʱ������
                            if(Res == NO_ERR)
                            {
                                if(gu8ReadValveFail == 0)  //���������ɹ�
                                    gPARA_Meter_Failed[i][j] = 0xaa; // ���ɹ����AA.
                                else
                                    gPARA_Meter_Failed[i][j] = 0xbb; //�ȱ�ɹ���ʧ�ܱ��BB.
                                debug_info(gDebugModule[METER_DATA], "ReadingMeterSn = %d  read success!\r\n", ReadingMeterSn);
                            }
                            else
                            {
                                debug_err(gDebugModule[METER_DATA], "ReadingMeterSn = %d  read failed\r\n", ReadingMeterSn);
                                gPARA_Meter_Failed[i][j] = 0x00; /*Failed:0x00  Success:0xaa*/
                            }
                        }
                        else if(gu8MBusShortFlag[i] == 1)
                        {
                            Res = HandleOneMBusShortMeter(DataPath, ReadingMeterSn); //MBUS��·�Ա������Ӧ����
                            gPARA_Meter_Failed[i][j] = 0x00; /*Failed:0x00  Success:0xaa*/
                            OSTimeDlyHMSM(0, 0, 0, 100); //��ֹдSD��̫Ƶ����

                        }
                    }

                }


            }


        }

    }
    OSSemPost(METERChangeChannelSem);

    //�˾�ע����gPARA_Meter_Failed����ʧ�ܲ�ѯ����ȫ����֮ǰ���㡣2015.04.21
    //memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));/*yangfei added 2014-02-28*/

    //begin:������ʧ����Ϣ���浽��SD������ʧ����Ϣ��ѯʱ�������á�
    OSTimeDlyHMSM(0, 0, 0, 500);

    OSMutexPend (FlashMutex, 0, &err);
    for(i = 0; i < METER_CHANNEL_NUM; i++) //һ��д��̫��������ʱ���ɹ����ֶ��д��SD���С�
    {
        for(RetryTimes = 0; RetryTimes < 5; RetryTimes++)
        {
            err = SDSaveData("/METER_Failed_Info", (uint8 *)(&gPARA_Meter_Failed[i][0]), METER_PER_CHANNEL_NUM, i * METER_PER_CHANNEL_NUM);
            if(err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, 0, 100);
            }
        }
        OSTimeDlyHMSM(0, 0, 0, 100);
    }
    OSMutexPost (FlashMutex);
    //end:������ʧ����Ϣ���浽��SD������ʧ����Ϣ��ѯʱ�������á�


    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <READ_ReadAllMeters> All of Meters Readed!");
    ReadDateTime(ReadTime);
    OS_ENTER_CRITICAL();
    memcpy(gREAD_TimingState.TimingEndTime, ReadTime, 6);
    OS_EXIT_CRITICAL();
    Meter_R_End = 1;
    DisableAllMBusChannel();//�Ͽ�����ͨ����
    return NO_ERR;
}

/****************************************************************************************************
**	�� ��  �� ��: READ_ParaInit
**	�� ��  �� ��: ��ʼ��timingpro.c�ļ��е�ȫ�ֱ���
**	�� ��  �� ��: ��
**	�� ��  �� ��: ��
**  ��   ��   ֵ: NO_ERR;
**	��		  ע:
*****************************************************************************************************/
void READ_ParaInit(void)
{
    memset(gREAD_ReadCmplFlag, 0x00, sizeof(gREAD_ReadCmplFlag));
    memset(gREAD_TimingState.TimingStartTime, 0x00, sizeof(gREAD_TimingState));
}
/********************************************************************************************************
**                                             �͵�³���ر�

yangfei added 20130802
68 0B 0B 68 53 FD 52 73 60 72 44 FF FF FF FF 27 16

68 04 04 68 53 fd 50 00 a0 16

10 7b fd 78 16

10 40 fd 3d 16
********************************************************************************************************/
int  HYDROMETER(DELU_Protocol *pProtocoalInfo, uint8 ProtocolVer)
{
    char  Selection[] = {0x68 , 0x0B, 0x0B, 0x68, 0x53, 0xFD, 0x52, 0x73, 0x60, 0x72, 0x44, 0xFF, 0xFF, 0xFF, 0xFF, 0x27, 0x16};
    //    char  Standard_data_reading[]={0x68, 0x04, 0x04, 0x68, 0x53, 0xfd, 0x50, 0x00, 0xa0, 0x16};
    //char  Standard_data_reading[]={0x68, 0x04, 0x04, 0x68, 0x53, 0xfd, 0x50, 0x50, 0xf0, 0x16};
    //char  Standard_data_reading[]={0x68, 0x04, 0x04, 0x68, 0x53, 0xfd, 0x50, 0x30, 0xd0, 0x16};
    char  Request_response[] = {0x10, 0x7b, 0xfd, 0x78, 0x16};
    char  Deselection[] = {0x10, 0x40, 0xfd, 0x3d, 0x16};
    uint8 cs		= 0x00;
    uint8 Res_data = 0;
    uint8 Err = 0;
    int i = 0, j;
    //char  meterAddr[][4]={{0x44,0x69,0x96,0x41},{0x44,0x69,0x96,0x43},{0x44,0x67,0x15,0x27},{0x44,0x72,0x60,0x73}};
    //char  meterAddr[][4]={{0x41,0x96,0x69,0x44},{0x43,0x96,0x69,0x44},{0x27,0x15,0x67,0x44},{0x73,0x60,0x72,0x44}};

    uint8 dev = DOWN_COMM_DEV_MBUS;

    //uint8 *pBuf=gDebugBuffer;
    uint8 ret;
    //uint8 Err = NO_ERR;
    uint8 RetryTimes			= 3;

    memset((char *)gDebugBuffer, 0, strlen((char *)gDebugBuffer));
    //METER_ChangeChannel(1);

    memcpy(&Selection[7], pProtocoalInfo->MeterAddr, 4);				//�齨 ��ַ��
    //memcpy(&Selection[7], meterAddr[i], 4);				//�齨 ��ַ��
    cs = PUBLIC_CountCS((uint8 *)&Selection[4], 11);			//���� У���ֽ�
    Selection[15] = cs;
    debug_info(gDebugModule[METER_DATA], "send frame is:\r\n");
    for(j = 0; j < strlen(Selection); j++)
    {
        if(gDebugModule[METER_DATA] >= KERN_DEBUG)
        {
            debug("%2x ", Selection[j]);
        }
    }
    debug_info(gDebugModule[METER_DATA], "\r\n");

    {
        char Sn_str[16] = {0};
        PUBLIC_MeterAddrToString(pProtocoalInfo->MeterAddr, Sn_str, 7);
        debug_info(gDebugModule[METER_DATA], "SN = %s!", Sn_str);
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "SN = %s!", Sn_str);
    }

    //METER_ComSet1();/*yangfei deleted 2013-11-13*/
    //OSTimeDly(OS_TICKS_PER_SEC);/*yangfei deleted 2013-11-02*/

    do
    {
        FeedTaskDog();
        OSSemPend(UpAskMeterSem, 5 * OS_TICKS_PER_SEC, &Err);                       //����MBUS��ͨ��
    }
    while(Err != OS_ERR_NONE);

    for(i = 0; i < RetryTimes; i++)
    {
        DuQueueFlush(dev);   										//��ջ�����
        DuSend(dev, (uint8 *)Selection, sizeof(Selection));
#if 0
        if(DuGetch(dev, &Res_data, 2 * OS_TICKS_PER_SEC))
        {
            debug_info(gDebugModule[METER_DATA], "outtime \r\n");
            continue;
        }
        if(Res_data == 0xE5)
        {
            debug_info(gDebugModule[METER_DATA], "Res_data==0xE5 \r\n");
        }
        else
        {
            debug_info(gDebugModule[METER_DATA], "Res_data!=0xE5 \r\n");
            continue;
        }
#endif
        OSTimeDly(OS_TICKS_PER_SEC);
#if 0
        if(ProtocolVer != ZENNER_VER)
        {
            DuQueueFlush(dev);   										//��ջ�����
            DuSend(dev, (uint8 *)Standard_data_reading, sizeof(Standard_data_reading));
            debug_info(gDebugModule[METER_DATA], "strlen(Standard_data_reading)=%d\r\n", sizeof(Standard_data_reading));
            OSTimeDly(OS_TICKS_PER_SEC);
        }
#endif
        //for(i=0;i<RetryTimes;i++)
        {
            DuQueueFlush(dev);   										//��ջ�����
            DuSend(dev, (uint8 *)Request_response, sizeof(Request_response));
            ret = HYDROMETER_METER_ReceiveFrame(dev, pProtocoalInfo->DataBuf, 0, &pProtocoalInfo->Length, ProtocolVer);
            if(ret == 0)
            {
                debug_info(gDebugModule[METER_DATA], "pProtocoalInfo->Length =%d \r\n", pProtocoalInfo->Length);
                debug_info(gDebugModule[METER_DATA], "HYDROMETER_METER_ReceiveFrame ok\r\n");
                LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "HYDROMETER_METER_ReceiveFrame ok\r\n");
                {
#ifdef  HYDROMETER_DEBUG
                    uint8 Err = 0;
                    uint8 Res = 0;
                    char  path[] = "/ZENNER1.txt";
                    static uint8 MeterSn = 0;
                    /*begin:yangfei modified 2013-11-15 for add debug*/
                    OSMutexPend (FlashMutex, 0, &Err);
                    Res = SDSaveData(path, pProtocoalInfo->DataBuf, pProtocoalInfo->Length - 3, (MeterSn++) * 256);
                    OSMutexPost (FlashMutex);
                    if(Res != NO_ERR)
                    {
                        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!", __FUNCTION__, __LINE__ );
                    }
                    /*end   :yangfei modified 2013-11-15 for add debug*/
#endif
                }
                DuQueueFlush(dev);   										//��ջ�����
                DuSend(dev, (uint8 *)Deselection, sizeof(Deselection));
                OSTimeDly(OS_TICKS_PER_SEC);/*yangfei deleted 2013-11-02*/
                break;
            }
            else
            {
                debug_err(gDebugModule[METER_DATA], "HYDROMETER_METER_ReceiveFrame ret = %d \r\n", ret);
                LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "ERROR: HYDROMETER_METER_ReceiveFrame ret = %d \r\n", ret);
                OSTimeDly(OS_TICKS_PER_SEC);
            }
        }
        DuQueueFlush(dev);   										//��ջ�����
        DuSend(dev, (uint8 *)Deselection, sizeof(Deselection));
        OSTimeDly(OS_TICKS_PER_SEC);/*yangfei deleted 2013-11-02*/
    }

    OSSemPost(UpAskMeterSem);
    if(i >= RetryTimes)										//����3��δ����������
    {
        debug_err(gDebugModule[METER_DATA], "%s line=%d  HYDROMETER read out  RetryTimes\r\n", __FUNCTION__, __LINE__);
        return 1;
    }

    return 0;
}

/****************************************************************************************************
**	�� ��  �� ��: METER_ReceiveFrame
**	�� ��  �� ��: �������еļ���ģ�������λ���·��Ķ������������
**	�� ��  �� ��: uint8 dev -- ����ͨ���豸;
**                uint16 Out_Time -- �������ʱ�ȴ�ʱ��
**	�� ��  �� ��: uint8 *buf -- �������ָ��;
**                uint8 *datalen -- ������յ�����֡����
**  ��   ��   ֵ: 0 -- ���յ���Ч֡;		1 -- ����֡ͷ����ȷ; 		2 -- ���յ�������֡���Ǳ����ܱ��ַ
**				  4 -- ����֡���Ȳ���		5 -- ����֡У�鲻��ȷ		6 -- ����֡β����ȷ
**	��		  ע:
68 1C 1C 68     -----1c 1c Ϊ����1c= 15byte+data
08 00 72 73 60 72 44 24 23 52 0C 1A 50 00 00  ----15byte
0C FB 08 00 00 00 00 04 6D 02 0F A1 18            ----data
5C 16
*****************************************************************************************************/

uint8 HYDROMETER_METER_ReceiveFrame(uint8 dev, uint8 *buf, uint16 Out_Time, uint8 *datalen, uint8 ProtocolVer)
{
    uint8 data 	= 0x00;
    uint8 len	= 0x00;
    uint8 Cs	= 0x00;
    uint8 RevState = METER_PRE;
    uint32 i;
    uint8 RcvData[256]	= {0x00};
    //uint8 BufData[256]	= {0x00};

#ifdef  HYDROMETER_DEBUG
    uint8 BufData[256]	= {0x00};
    *datalen = 0;
    for(i = 0; i < 100; i++)
    {
        if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
        {
            return 1;
        }
        BufData[i] = data;
        if(i == 99)
        {
            PUBLIC_HexStreamToString(BufData, 100, RcvData);
            LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "%s", RcvData);
            return 2;
        }
    }
#endif

    i = 7;
    if(ProtocolVer == ZENNER_VER)/*����Э��̶�����Ϊ0XC7*/
    {
        while(i--)														//��֡ͷ
        {
            if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
            {
                return 1;
            }

            switch(RevState)
            {
            case METER_PRE:
            {
                if(data == 0xc7)
                {
                    RevState = METER_START;
                    len = data;
                }
                break;
            }
            case METER_START:
            {

                if(data == 0x68)
                {
                    RevState = METER_PRE;
                    goto FIND_DATA;
                }
                else if(data != 0xC7 )
                {
                    RevState = METER_PRE;
                }
                break;
            }
            default:
            {
                RevState = METER_PRE;
                break;
            }

            }

        }
        if(i == 0x00)
        {
            return 2;
        }

    }
    else
    {
        while(i--)														//��֡ͷ
        {
            if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
            {
                return 1;
            }
            if(data == 0x68)
            {
                break;
            }

        }
        if(DuGetch(dev, &data, OS_TICKS_PER_SEC))
        {
            return 2;
        }
        len = data;
        if(DuGetch(dev, &data, OS_TICKS_PER_SEC))
        {
            return 3;
        };/*len*/
        if(data != len)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "%s %d len is err len = %d data = %d\r\n", __FUNCTION__, __LINE__, len, data);
            //return 4;
        }
    }


    if(DuGetch(dev, &data, OS_TICKS_PER_SEC))
    {
        return 4;
    }
    if(data != 0x68)
    {
        debug_err(gDebugModule[METER_DATA], "%s %d can't find 2nd 0x68\r\n", __FUNCTION__, __LINE__);
        return 5;
    }
    debug_info(gDebugModule[METER_DATA], "len = %d \r\n", len);
FIND_DATA:
    for(i = 0; i < len; i++)												//��ַ
    {
        if(DuGetch(dev, &data, OS_TICKS_PER_SEC))
        {
            return 6;
        }
        Cs 		+= data;
        RcvData[i] = data;
        if(gDebugModule[METER_DATA] >= KERN_DEBUG)
        {
            debug("%2x ", data);
        }
    }
    debug_info(gDebugModule[METER_DATA], "\r\n");

    //memcpy(buf,&RcvData[15],len-15);
    memcpy(buf, RcvData, len); /*yangfei modified 2013-11-12 for explain all data*/

    if(DuGetch(dev, &data, OS_TICKS_PER_SEC))
    {
        return 7;
    }
    if(Cs != data)
    {
        debug_err(gDebugModule[METER_DATA], "HYDROMETER checksum error\r\n");
        return 8;
    }

    if(DuGetch(dev, &data, OS_TICKS_PER_SEC))
    {
        return 9;   //������
    }
    if(data != 0x16)
    {
        debug_err(gDebugModule[METER_DATA], "HYDROMETER end error \r\n");
        return 12;
    }

    //*datalen = len-15 +3;/*Ϊ��Ӧԭ������ṹ+3,ʵ�ʳ���Ϊlen-15*/
    *datalen = len + 3;/*Ϊ��Ӧԭ������ṹ+3*/

    return NO_ERR;
}

/*�����ȷ������VirtualMeterSn*/
void SaveVirtualMeterSn(uint16 VirtualMeterSn)
{
    uint8 err;
    CPU_SR		cpu_sr;
    DataStoreParaType	InitPara;
    uint8 SystemTime[6] = {0x00};
    char  NodePath[] = "/2012/12/24/timenode";
    memset(&InitPara, 0, sizeof(InitPara));
    /*save VirtualMeterSn*/
    OS_ENTER_CRITICAL();
    InitPara.MeterNums = VirtualMeterSn;/*VirtualMeterSn:ÿ�������ɼ����������ܲ�һ��*/
    memcpy((uint8 *)InitPara.TimeNode, (uint8 *)gPARA_TimeNodes, sizeof(gPARA_TimeNodes));
    memcpy(SystemTime, gSystemTime, 6);
    OS_EXIT_CRITICAL();

    GetFilePath(NodePath, SystemTime, ARRAY_DAY);
    OSMutexPend (FlashMutex, 0, &err);
    err = SDSaveData(NodePath, &InitPara, sizeof(DataStoreParaType), 0);
    if(err != NO_ERR)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!", __FUNCTION__, __LINE__ );
        debug("WARNING: %s %d SDSaveData Error!", __FUNCTION__, __LINE__);
    }
    OSMutexPost (FlashMutex);

}

/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/
