/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)
**  ��   ��   ��: Meter.c
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��9��10��
**  ��        ��: �ȼ�����Э�����
**	�� ��  �� ¼:
*****************************************************************************************************/

/********************************************** include *********************************************/
#include <includes.h>
#include "app_down.h"
#include "Ul_array.h"
#include "Valve.h"

#include "ModBus.h" //����modbus---���������
/********************************************** define *********************************************/
#define   BIT2_LOW_VOLTAGE    0x04
#define   BIT3_RESERVE    0x08    /*reserve*/
#define   BIT4_RESERVE    0x10    /*reserve*/
#define   BIT5_TEMP_SENSOR    0x20    /*�¶ȴ���������*/
#define   BIT6_FLOW_SENSOR    0x40   /*��������������*/
#define   BIT7_N_TEMP_DIFF    0x80    /*negative temperature difference*/
#ifdef DEMO_APP
extern CJ188_Format Demo_CJ188_ch1;
extern CJ188_Format Demo_CJ188_ch2;
extern CJ188_Format Demo_CJ188_ch3;
extern uint8 Show_Data_Status;
extern uint8 Data_complete;

#endif

#define  BIT0   0X01
#define  BIT1   0X02
#define  BIT2   0X04

/*CJ188 ��׼*/
#define  WH   0X02
#define  KWH   0X05
#define  MWH   0X08
#define  MWH100   0X0A
#define  J   0X01
#define  KJ   0X0B
#define  MJ   0X0E
#define  GJ   0X11
#define  GJ100   0X13
#define  W   0X14
#define  KW   0X17
#define  MW   0X1A
#define  L  0X29
#define  M3   0X2C
#define  L_H  0X32
#define  M3_H   0X35


#if 0
typedef struct
{
    uint8 Extension: 1;
    uint8 LSB: 1;
    uint8 Function: 2;
    uint8 BCD: 1;    /**/
    uint8 Data: 3;
} ST_DIF;
typedef struct
{
    uint8 Extension: 1;
    uint8 Unit: 1;
    uint8 Tariff: 2;
    uint8 StorageNumber: 4;
} ST_DIFE;
typedef struct
{
    uint8 Extension: 1;
    uint8 Data: 7;
} ST_VIF;
typedef struct
{
    uint8 Extension: 1;
    uint8 value: 7;
} ST_VIFE;
#else
typedef struct
{
    uint8 Data: 3;
    uint8 BCD: 1;    /**/
    uint8 Function: 2;
    uint8 LSB: 1;
    uint8 Extension: 1;
} ST_DIF;
typedef struct
{
    uint8 StorageNumber: 4;
    uint8 Tariff: 2;
    uint8 Unit: 1;
    uint8 Extension: 1;
} ST_DIFE;
typedef struct
{
    uint8 Data: 7;
    uint8 Extension: 1;
} ST_VIF;
typedef struct
{
    uint8 value: 7;
    uint8 Extension: 1;
} ST_VIFE;
#endif
/********************************************** global *********************************************/
OS_EVENT *UpAskMeterSem;
OS_EVENT *METERChangeChannelSem;


/********************************************** static *********************************************/
static uint8 gMETER_FrameSer = 0x00;

extern char gPrintData[1024];

extern uint8 gDownCommDev485;

extern ul_array_t  *gpValve_Array;

uint8 gCurrent_Channel = 0;  //��ǰMBUS���ڵڼ�ͨ����0��ʾͨ��ȫ�ء�



uint8   HYDROMETER_TO_CJ188(CJ188_Format  *MeterData, uint8 *DataBuf, uint8 len);
uint8  ENLEMAN_TO_CJ188(CJ188_Format  *MeterData, uint8 *DataBuf, uint8 len);


extern  void  rtu_read_coil_status ( ModBusMRData *PLCCMD, unsigned char n); //���������
//
uint8 METER_MeterCommunicate_Direct_PLC(uint8 *pData, uint8 InLen, uint8 *pResBuf, uint8 *pOutLen);  //���������
/****************************************************************************************************
**	�� ��  �� ��: METER_CreateFrame
**	�� ��  �� ��: ���������ȼ�����������֡
**	�� ��  �� ��: DELU_Protocol *pReadMeter -- ��ȡ�ȼ���������ݽṹ
**	�� ��  �� ��: uint8  *_sendBuf -- �齨��������֡;
**                uint16 *plenFrame -- �齨��������֡����
**  ��   ��   ֵ: ��
**	��		  ע: ����֡�����������ȼ���������֡�ṹ������Ϣ������֡
*****************************************************************************************************/
void METER_CreateFrame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 templen	= 0x00;
    uint8 cs		= 0x00;
    uint8 *pTemp, *pTemp1;
    uint16 lenFrame = 0x00;
    uint16 CheckLen = 0x00;

    LOG_assert_param(pData == NULL);
    LOG_assert_param(pSendFrame == NULL);
    LOG_assert_param(plenFrame == NULL);

    pTemp = pSendFrame;

    memset(pTemp, 0xFE, pData->PreSmybolNum);		//����ǰ���ַ�0xFE, �������������
    pTemp += pData->PreSmybolNum;
    lenFrame += pData->PreSmybolNum;

    pTemp1 = pTemp;									//Ԥ������У��λ
    *pTemp++ = DELU_FRAME_START;					//�齨 ֡ͷ 0x68
    *pTemp++ = pData->MeterType;					//�Ǳ�����
    memcpy(pTemp, pData->MeterAddr, 7);				//�齨 ��ַ��
    pTemp += 7;
    lenFrame += 9;
    CheckLen += 9;

    *pTemp++ = pData->ControlCode;					//�齨 ������
    *pTemp++ = pData->Length;						//�齨 �����򳤶�
    lenFrame += 2;
    CheckLen += 2;

    *pTemp++ = pData->DataIdentifier;
    *pTemp++ = (pData->DataIdentifier) >> 8;		//�齨 ���ݱ�ʶ��
    *pTemp++ = gMETER_FrameSer++;
    lenFrame += 3;
    CheckLen += 3;

    templen = pData->Length - 3;
    memcpy(pTemp, pData->DataBuf, templen);
    pTemp += templen;
    lenFrame += templen;
    CheckLen += templen;

    cs = PUBLIC_CountCS(pTemp1, CheckLen);			//���� У���ֽ�
    *pTemp++ = cs;									//�齨 У��λ
    *pTemp++ = 0x16;									//�齨 ����λ
    lenFrame += 2;									//�齨 ����֡����

    *pTemp++ = 0x16;	//��ǿ485����1200bps��,���һ�ֽ����ǲ��ԣ���֡���һ�ֽڡ�
    lenFrame += 1;

    *plenFrame = lenFrame;							//��������֡����
}

uint8 Yilin_Calc_Parity(uint8 *pStart, uint8 ucLen)
{
    uint32 i = 0;
    uint32 value = 0;
    for (i = 0; i < ucLen; i++)
    {
        value += pStart[i];
    }
    value = value & 0xff;
    value = value ^ 0xA5;
    return value;
}


/****************************************************************************************************
**	�� ��  �� ��: YINLINVALVE_CreateFrame
**	�� ��  �� ��: ���������ȼ�����������֡
**	�� ��  �� ��: DELU_Protocol *pReadMeter -- ��ȡ�ȼ���������ݽṹ
**	�� ��  �� ��: uint8  *_sendBuf -- �齨��������֡;
**                uint16 *plenFrame -- �齨��������֡����
**  ��   ��   ֵ: ��
**	��		  ע: ����֡�����������ȼ���������֡�ṹ������Ϣ������֡
*****************************************************************************************************/
void YINLINVALVE_CreateFrame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
{
    uint8 i = 0;
    //UINT8 ucTempLoop = 0;
    uint8 *pTemp = pSendFrame;
    uint8 *pCheck = pTemp;

    *pTemp++ = pData->ControlCode;				// ��������

    for(i = 0; i < 2; i++)					// �Ǳ��ַ ������
    {
        *pTemp++ = pData->MeterAddr[i];
    }
    for(i = 0; i < 4; i++)
    {
        *pTemp++ = pData->DataBuf[i];
    }

    *pTemp++ = Yilin_Calc_Parity(pCheck, 7);

    *plenFrame = 8;

    //��������֡����
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
**	��		  ע: �˴��������������������궨�壬����ķ���ֵ
*****************************************************************************************************/

uint8 METER_ReceiveFrame(uint8 dev, uint8 *buf, uint16 Out_Time, uint8 *datalen)
{
    uint8 data 	= 0x00;
    uint8 len	= 0x00;
    uint8 Cs	= 0x00;
    uint32 i, j;
    uint8  rtu_number_long;

    LOG_assert_param(dev != 0x00);
    LOG_assert_param(buf == NULL);
    LOG_assert_param(datalen == NULL);

    *datalen = 0;
    //��������������ӸĶ���ԭ��ֻ�ܹ���ȡ������������Ҫ�ٸ��ݱ�dev����һ���жϹ��̣������485����ñ�׼��modbus������ʹ��ԭ����������ض�����
    if(dev == DOWN_COMM_DEV_485_PLC)
    {
        //����ΪPLC�½�һ������	 ��������ڲ��ԣ������͸�485�������ݵ����ݽ��չ���
        rtu_number_long = 0; //������յ��������� ---��־��
        for(i = 0; i < 9; i++) 	 //�ܹ���ȷ�Ķ���485������
        {
            DuGetch(DOWN_COMM_DEV_485, &buf[i], OS_TICKS_PER_SEC * 2);			//	OS_TICKS_PER_SEC*2  //��485�����ݶ�����
            rtu_number_long++;
        }
        *datalen = rtu_number_long;
    }
    else
    {
        i = 30;
        while(i--)														//��֡ͷ
        {
            FeedTaskDog();
            if(DuGetch(dev, &data, Out_Time))
                return 1;
            if(data == 0x68)	break;
        }
        Cs 		= data;
        *buf++ 	= data;

        if(DuGetch(dev, &data, OS_TICKS_PER_SEC * 3))
            return 2;		//������
        Cs 		+= data;
        *buf++	 = data;

        for(i = 0; i < 7; i++)												//��ַ
        {
            if(DuGetch(dev, &data, OS_TICKS_PER_SEC * 3))
                return 3;
            Cs 		+= data;
            *buf++	 = data;
        }

        if(DuGetch(dev, &data, OS_TICKS_PER_SEC * 3))
            return 4;		//������
        Cs 		+= data;
        *buf++	 = data;

        if(DuGetch(dev, &len, OS_TICKS_PER_SEC * 3))
            return 5;		//���ݳ���
        if(len > METER_FRAME_LEN_MAX)
            return 10;
        Cs 		+= len;
        *buf++	 = len;

        for(j = 0; j < len; j++)											//������
        {
            if(DuGetch(dev, &data, OS_TICKS_PER_SEC * 3))
                return 6;
            *buf++  = data;
            Cs     += data;
        }

        if(DuGetch(dev, &data, OS_TICKS_PER_SEC * 3))
            return 7;		//У���ֽ�
        if(data != Cs)
            return 11;
        *buf++	= data;

        if(DuGetch(dev, &data, OS_TICKS_PER_SEC * 3))
            return 8;		//������
        if(data != 0x16)
            return 12;

        *buf++		= data;
        *datalen 	= len + 16;
    }
    return NO_ERR;
}




/****************************************************************************************************
**	�� ��  �� ��: METER_DELU_AnalDataFrame
**	�� ��  �� ��: ��������ȡ�����ȼ�������
**	�� ��  �� ��: Duint8 *pRecFrame -- ����Ĵ���֡������
**	�� ��  �� ��: DELU_Protocol *pProtoclData -- ��֡������ݽṹ
**  ��   ��   ֵ: NO_ERR
**	��		  ע:
*****************************************************************************************************/

uint8 METER_DELU_AnalDataFrame(DELU_Protocol *pProtoclData, uint8 *pRecFrame)
{
    uint8 *pTemp 		= pRecFrame;

    LOG_assert_param(pProtoclData == NULL);
    LOG_assert_param(pRecFrame == NULL);
    if(pProtoclData->MeterType == 0x20)
    {
        if(0x68 != *pTemp++)	return 1;

        pProtoclData->MeterType = *pTemp++;										//�Ǳ�����
        memcpy(pProtoclData->MeterAddr, pTemp, 7);								//�Ǳ��ַ
        pTemp += 7;

        pProtoclData->ControlCode	= *pTemp++;									//������
        pProtoclData->Length		= *pTemp++;									//����
        pProtoclData->DataIdentifier = (*pTemp++) | (*pTemp++ << 8);
        pProtoclData->SER			= *pTemp++;
        memcpy(pProtoclData->DataBuf, pTemp, (pProtoclData->Length - 3));			//����������
        pTemp += pProtoclData->Length - 3;
    }
    else if(pProtoclData->MeterType == 0xB0)
    {
        pProtoclData->ControlCode	= *pTemp++;									//�Ǳ�����
        memcpy(pProtoclData->MeterAddr, pTemp, 2);								//�Ǳ��ַ
        pTemp += 2;
        memcpy(pProtoclData->DataBuf, pTemp, 8);			//����������
    }
    else if(pProtoclData->MeterType == 0x41)  //��������ͨѶ��������
    {
        if(0x68 != *pTemp++)	return 1;

        pProtoclData->MeterType = *pTemp++;										//�Ǳ�����
        memcpy(pProtoclData->MeterAddr, pTemp, 7);								//�Ǳ��ַ
        pTemp += 7;

        pProtoclData->ControlCode	= *pTemp++;									//������
        pProtoclData->Length		= *pTemp++;									//����
        pProtoclData->DataIdentifier = (*pTemp++) | (*pTemp++ << 8);
        pProtoclData->SER			= *pTemp++;
        memcpy(pProtoclData->DataBuf, pTemp, (pProtoclData->Length - 3));			//����������
        pTemp += pProtoclData->Length - 3;

    }

    return NO_ERR;
}

/****************************************************************************************************
**	�� ��  �� ��: METER_MeterCommunicate
**	�� ��  �� ��: ���������ȼ�����ͨ��
**	�� ��  �� ��: DELU_Protocol *pData -- ���·��ĵ�³Э������֡
**	�� ��  �� ��: uint8 *pResBuf -- ���յ�������֡��Ӧ;
**                uint8 *pDataLenBack -- ����֡����
**  ��   ��   ֵ: NO_ERR, ERR_1 -- û�н��յ�����
**	��		  ע:
*****************************************************************************************************/

uint8 METER_MeterCommunicate(DELU_Protocol *pData, uint8 *pResBuf, uint8 *pDataLenBack)
{
    uint8 err;
    uint8 *cp = NULL;
    uint8 DataFrame[METER_FRAME_LEN_MAX];
    uint8 DataLen = 0x00;

    LOG_assert_param(pData == NULL);
    LOG_assert_param(pResBuf == NULL);
    LOG_assert_param(pDataLenBack == NULL);
    if(pData->MeterType == 0xB0)
        YINLINVALVE_CreateFrame(pData, DataFrame, &DataLen);
    else
        METER_CreateFrame(pData, DataFrame, &DataLen);

    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_MeterCommunicate> CreateFrame successful");
    debug_debug(gDebugModule[TASKDOWN_MODULE], "INFO: <METER_MeterCommunicate> CreateFrame successful!");

    do
    {
        FeedTaskDog();
        OSSemPend(UpAskMeterSem, 5 * OS_TICKS_PER_SEC, &err);                       //����MBUS��ͨ��
    }
    while(err != OS_ERR_NONE);

    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_MeterCommunicate> Start Read HeatMeter!");
    debug_debug(gDebugModule[TASKDOWN_MODULE], "INFO: <METER_MeterCommunicate> Start Read HeatMeter!");

    /*begin:yangfei added 2013-02-21 */
    if(gDebugModule[TASKDOWN_MODULE] >= KERN_DEBUG)
    {
        PUBLIC_HexStreamToString(DataFrame, DataLen, gPrintData);
        debug_debug(gDebugModule[TASKDOWN_MODULE], "Meter send data:%s\r\n", gPrintData);
    }
    /*end:yangfei added 2013-02-21*/
    cp = (uint8 *)Uart0SendThenReceive_PostPend(DataFrame, &DataLen);

    OSSemPost(UpAskMeterSem);
    if(cp[0])  							//����
    {
        debug_debug(gDebugModule[TASKDOWN_MODULE], "WARNING: <METER_MeterCommunicate> Recive Data Time OUT!\r\n");
        return 1;
    }
    else
    {
        /*begin:yangfei added 2013-02-21 */
        if(gDebugModule[TASKDOWN_MODULE] >= KERN_DEBUG)
        {
            PUBLIC_HexStreamToString(DataFrame, DataLen, gPrintData);
            debug_debug(gDebugModule[TASKDOWN_MODULE], "Meter recive data:%s\r\n", gPrintData);
        }
        /*end:yangfei added 2013-02-21*/
    }
    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_MeterCommunicate> Read HeatMeter Data successful");

    memcpy(pResBuf, DataFrame, DataLen);
    *pDataLenBack = DataLen;
    return NO_ERR;
}

/****************************************************************************************************
**	�� ��  �� ��: METER_MeterCommunicate_Direct
**	�� ��  �� ��: ���������ȼ�����ͨ��, ͸��ת��ר��
**	�� ��  �� ��: uint8 *pData -- ��ת��������֡;
**                uint8 InLen -- ��ת��������֡����
**	�� ��  �� ��: uint8 *pResBuf -- ���յ�������֡��Ӧ;
**                uint8 *pOutLen -- ����֡����
**  ��   ��   ֵ: NO_ERR, ERR_1 -- û�н��յ�����
**	��		  ע:
*****************************************************************************************************/
uint8 METER_MeterCommunicate_Direct(uint8 *pData, uint8 InLen, uint8 *pResBuf, uint8 *pOutLen)
{
    uint8 i = 0;
    uint8 *cp = NULL;
    uint8 DataFrame[METER_FRAME_LEN_MAX];
    uint8 DataLen = 0x00;
    uint8 Err = 0x00;

    LOG_assert_param(pData == NULL);
    LOG_assert_param(pResBuf == NULL);
    LOG_assert_param(pOutLen == NULL);
    DataLen = InLen;
    memcpy(DataFrame, pData, InLen);
    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_MeterCommunicate_Direct> Send Datas Direct, The Frame Datas:");

    for(i = 1; i < 7; i++)
    {

        METER_ChangeChannel(i);

        do
        {
            FeedTaskDog();
            OSSemPend(UpAskMeterSem, 5 * OS_TICKS_PER_SEC, &Err);                       //����MBUS��ͨ��
        }
        while(Err != OS_ERR_NONE);
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_MeterCommunicate_Direct> Start Read HeatMeter, Channel is NO.%d", i);
        cp = (uint8 *)Uart0SendThenReceive_PostPend(DataFrame, &DataLen);
        OSSemPost(UpAskMeterSem);
        if(cp[0])
        {
            //����
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <METER_MeterCommunicate_Direct> Recive Data Time OUT, Channel is NO.%d", i);
            Err++;
        }
        else
        {
            break;
        }
    }

    if(Err >= 5)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <METER_MeterCommunicate_Direct> ALL Of Channels Recive Data Time OUT");
        return 1;			//6��ͨ�� ������  ��û�н��յ�����
    }

    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_MeterCommunicate> Read HeatMeter Data successful! Channel is NO.%d", i);
    memcpy(pResBuf, DataFrame, DataLen);
    *pOutLen = DataLen;
    return NO_ERR;
}


/****************************************************************************************************
**	�� ��  �� ��: Communicate_ForAll
**	�� ��  �� ��: ���������ȱ�򷧿���Ⱥ����Ϣ��
**	�� ��  �� ��:
**
**	�� ��  �� ��:
**
**  ��   ��   ֵ: NO_ERR,�̶������޴���
**	��		  ע:
*****************************************************************************************************/
uint8 Communicate_ForAll(uint8 *pData)
{
    uint8 i = 0;
    uint8 *cp = NULL;
    uint8 lu8DataFrame[METER_FRAME_LEN_MAX];
    uint8 lu8DataLen = 0x00;
    uint8 Err = 0x00;
    uint8 lu8RetryTimes = 0;
    uint8 lu8DevType = 0;
    uint8 lu8ProtocolType = 0;
    uint16 lu16meternum = 0;

    LOG_assert_param(pData == NULL);


    lu8DevType = *(pData + 1);  //�豸����
    lu8ProtocolType = *(pData + 3); //Э��汾��

    lu8DataLen = *(pData + 5);  //֡����
    memcpy(lu8DataFrame, pData + 6, lu8DataLen);

    if(lu8DevType == HEAT_METER_TYPE)   //�ȱ�
    {
        (*METER_ComParaSetArray[gMETER_Table[lu8ProtocolType][0]])();
    }
    else if(lu8DevType == TIME_ON_OFF_AREA_TYPE)   //��������
    {
        (*METER_ComParaSetArray[gVALVE_Table[lu8ProtocolType][0]])();  //���÷��Ŷ�Ӧ���ڲ�����
    }
    else
    {
        //����Ҫʱ���䡣
    }

    for(i = 1; i <= 6; i++) 	//MBUSͨ��4����
    {
        lu16meternum = gPARA_MeterChannelNum[i - 1];
        if(lu16meternum > 0)
        {
            METER_ChangeChannel(i);
            do
            {
                FeedTaskDog();
                OSSemPend(UpAskMeterSem, 5 * OS_TICKS_PER_SEC, &Err);                       //����MBUS��ͨ��
            }
            while(Err != OS_ERR_NONE);

            for(lu8RetryTimes = 0; lu8RetryTimes < 5; lu8RetryTimes++) //Ϊ��֤Ⱥ���ɹ��ʣ�Ⱥ��2�Ρ�
            {
                DuQueueFlush(DOWN_COMM_DEV_MBUS);   										//��ջ�����
                DuSend(DOWN_COMM_DEV_MBUS, lu8DataFrame, lu8DataLen);
                OSTimeDly(2 * OS_TICKS_PER_SEC);
            }

            OSSemPost(UpAskMeterSem);

        }
    }

    //����RS485ͨ����
    lu16meternum = gPARA_MeterChannelNum[METER_CHANNEL_NUM - 1];
    if(lu16meternum > 0)
    {

        METER_ChangeChannel(METER_CHANNEL_NUM);  //���һ��ͨ����RS485ͨ����
        do
        {
            FeedTaskDog();
            OSSemPend(UpAskMeterSem, 5 * OS_TICKS_PER_SEC, &Err); //����MBUS��ͨ��
        }
        while(Err != OS_ERR_NONE);

        for(lu8RetryTimes = 0; lu8RetryTimes < 5; lu8RetryTimes++) //Ϊ��֤Ⱥ���ɹ��ʣ�Ⱥ��2�Ρ�
        {
            DuQueueFlush(DOWN_COMM_DEV_485);   										//��ջ�����
            DuSend(DOWN_COMM_DEV_485, lu8DataFrame, lu8DataLen);
            OSTimeDly(2 * OS_TICKS_PER_SEC);
        }

        OSSemPost(UpAskMeterSem);
    }




    return NO_ERR;  //�̶�����NO_ERR��
}



/****************************************************************************************************
**	�� ��  �� ��: METER_MeterCommunicate_Direct
**	�� ��  �� ��: ���������ȼ�����ͨ��, ͸��ת��ר��
**	�� ��  �� ��: uint8 *pData -- ��ת��������֡;
**                uint8 InLen -- ��ת��������֡����
**	�� ��  �� ��: uint8 *pResBuf -- ���յ�������֡��Ӧ;
**                uint8 *pOutLen -- ����֡����
**  ��   ��   ֵ: NO_ERR, ERR_1 -- û�н��յ�����
**	��		  ע:
*****************************************************************************************************/
uint8 METER_MeterCommunicate_Direct_Ex(uint8 *pData, uint8 InLen, uint8 *pResBuf, uint8 *pOutLen, uint8 ForAllOrOne)
{
    uint8 i = 0;
    uint8 *cp = NULL;
    uint8 DataFrame[METER_FRAME_LEN_MAX];
    uint8 DataLen = 0x00;
    uint8 Err = 0x00;
    uint8 RetryTimes = 0;

    LOG_assert_param(pData == NULL);
    LOG_assert_param(pResBuf == NULL);
    LOG_assert_param(pOutLen == NULL);

    DataLen = InLen;
    memcpy(DataFrame, pData, InLen);
    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_MeterCommunicate_Direct> Send Datas Direct, The Frame Datas:");

    if(ForAllOrOne == 0x0a)/*Ⱥ��,��ÿ��ͨ����������*/
    {
        for(i = 1; i < 7; i++)
        {
            METER_ChangeChannel(i);
            do
            {
                FeedTaskDog();
                OSSemPend(UpAskMeterSem, 5 * OS_TICKS_PER_SEC, &Err);                       //����MBUS��ͨ��
            }
            while(Err != OS_ERR_NONE);

            for(RetryTimes = 0; RetryTimes < 5; RetryTimes++)
            {
                DuQueueFlush(DOWN_COMM_DEV_MBUS);   										//��ջ�����
                DuSend(DOWN_COMM_DEV_MBUS, DataFrame, DataLen);
                OSTimeDly(OS_TICKS_PER_SEC / 3);
            }

            OSSemPost(UpAskMeterSem);
        }
    }
    else
    {

        do
        {
            FeedTaskDog();
            OSSemPend(UpAskMeterSem, 5 * OS_TICKS_PER_SEC, &Err);                       //����MBUS��ͨ��
        }
        while(Err != OS_ERR_NONE);
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_MeterCommunicate_Direct> Start Read HeatMeter, Channel is NO.%d", i);
        cp = (uint8 *)Uart0SendThenReceive_PostPend(DataFrame, &DataLen);
        OSSemPost(UpAskMeterSem);
        if(cp[0])
        {
            //����
            LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <METER_MeterCommunicate_Direct> Recive Data Time OUT, Channel is NO.%d", i);
            debug_err(gDebugModule[TIME_AREA], "ERROR:YILIN Uart0SendThenReceive_PostPend Recive Data Time OUT");
            return 1;
        }
        else
        {

        }

        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_MeterCommunicate> Read HeatMeter Data successful! Channel is NO.%d", i);
        memcpy(pResBuf, DataFrame, DataLen);
        *pOutLen = DataLen;
    }
    return NO_ERR;
}



/****************************************************************************************************
**	�� ��  �� ��: METER_ReadMeterDataTiming
**	�� ��  �� ��: ��������ȡ�¹��ȼ���������ݼ���ȡ����
**	�� ��  �� ��: uint16 MeterSn -- �����;
**	�� ��  �� ��: uint8 *EleBuf -- ���յ�������֡��Ӧ;
**  ��   ��   ֵ: NO_ERR, ERR_1 -- û�н��յ�����
**	��		  ע: ��д���ж�ȡ���ݵ�������ݽṹ
**  SD����ÿ���������ռ��128�ֽڵĿռ䣬���е�һ���ֽ�Ϊ���ݳ���(1�ֽ�) + ����(N+11) + CS�ۼӺ�У��(1�ֽ�)
**	�������ݸ�ʽΪ: �ȼ������ַ(7�ֽ�) + �������ݳ���(1�ֽ�) + ��������(N) + �¶�����(3�ֽ�)
**	����ȡ�������ݲ���ȷ����û�ж�ȡ�����ݣ���涨�������ݳ���Ϊ0�����������ݡ�
*****************************************************************************************************/
uint8 METER_ReadMeterDataTiming(uint16 MeterSn, uint8 *EleBuf)
{
    uint8 i				= 0x00;
    uint8 Err 		 	= 0x00;
    //uint8 RetryTimes 	= 0x03;
    uint8 DataLen		= 0x00;
    uint8 DataBuf[128]	= {0x00};
    uint8 ReadMeterSuccess = 0;
    uint8 *pTemp = DataBuf;
    MeterFileType	mf;
    DELU_Protocol	ProtocoalInfo;
    uint8 lu8DataIn[100] = {0};  //�ڷ��ؿ���ʱ���ڴ��ݷ��ؿ�����Ϣ��
    uint8 DataFrame[METER_FRAME_LEN_MAX];
    uint8 DataLen_Vave = 0x00;
    uint8 lu8ReadTime[6] = {0};
    uint8 *p_ReadVavleTime;  //��������ʱ��Ԥ����
    CPU_SR		cpu_sr;

    CJ188_Format CJ188_Data;

    LOG_assert_param(EleBuf == NULL);
    LOG_assert_param(MeterSn > METER_NUM_MAX);

    Err = PARA_ReadMeterInfo(MeterSn, &mf);
    if(Err != NO_ERR)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <METER_ReadMeterDataTiming> Read Meter filed failed, MeterSn is %4XH, Return is %d"
                               , MeterSn, Err);
    }
    if(mf.ProtocolVer > PROTOCOL_MAX)
    {
        mf.ProtocolVer = 0;
    }

    /*begin:yangfei added 2013-11-2 for �ж�485 �ȱ����Ϊ7ͨ��*/
    if(mf.ProtocolVer == 4 && mf.ChannelIndex != 7)
    {
        debug_err(gDebugModule[METER_DATA], "\r\n %s:485 meter channel!=7 \r\n", __FUNCTION__);
        goto ERROR;
    }
    /*end:yangfei added 2013-11-2 for �ж�485 �ȱ����Ϊ7ͨ��*/

    (*METER_ComParaSetArray[gMETER_Table[mf.ProtocolVer][0]])();  //����Э�飬���ó����ڲ�����

    /*begin:yangfei added for support 485 meter*/
    // gDownCommDev485 = gMETER_Table[mf.ProtocolVer][3];  //��ͨ���ž����Ƿ�485���߼��ɡ�
    /*end:yangfei added for support 485 meter*/

    //��д��Ӧ�� �ȼ�����ͨ��Э�� �ṹ��
    ProtocoalInfo.PreSmybolNum  = gMETER_Table[mf.ProtocolVer][2];
    ProtocoalInfo.MeterType 	= 0x20;

    if(mf.ProtocolVer == 2)
    {
        //��Ϊ����˹���򽫱��ַǰ5λΪFFFFF
        for(i = 0; i < 7; i++)
        {
            ProtocoalInfo.MeterAddr[i] = mf.MeterAddr[6 - i];
        }
        ProtocoalInfo.MeterAddr[0] = 0xFF;
        ProtocoalInfo.MeterAddr[1] = 0xFF;
        ProtocoalInfo.MeterAddr[2] |= 0xF0;
    }
    /*begin:yangfei added for support wanhua big meter 2013-08-12*/
    else if(mf.ProtocolVer == 5)
    {
        memcpy(ProtocoalInfo.MeterAddr, mf.MeterAddr, 7);
        ProtocoalInfo.MeterAddr[5] = 0x01;
    }
    /*end:yangfei added for support wanhua big meter 2013-08-12*/
    else
    {
        memcpy(ProtocoalInfo.MeterAddr, mf.MeterAddr, 7);
    }
    ProtocoalInfo.ControlCode 	= 0x01;
    ProtocoalInfo.Length		= 0x03;
    ProtocoalInfo.DataIdentifier = gMETER_Table[mf.ProtocolVer][1];
    memset(ProtocoalInfo.DataBuf, 0x00, METER_FRAME_LEN_MAX);

    pTemp++;	//Ϊ���ݳ���Ԥ��
    memcpy(pTemp, (uint8 *) & (mf.MeterID), 2); //MeterID.
    pTemp += 2;
    DataLen = 2;
    *pTemp = mf.EquipmentType;  //�豸���͡�
    pTemp += 1;
    DataLen += 1;
    memcpy(pTemp, mf.MeterAddr, 7);
    pTemp += 7;
    DataLen += 7;
    *pTemp = mf.BulidID;  //¥�š�
    pTemp += 1;
    DataLen += 1;
    *pTemp = mf.UnitID;  //��Ԫ�š�
    pTemp += 1;
    DataLen += 1;
    memcpy(pTemp, (uint8 *) & (mf.RoomID), 2); //�����
    pTemp += 2;
    DataLen += 2;

    ReadDateTime(lu8ReadTime);
    memcpy(pTemp, lu8ReadTime, 3); //���볭��ʱ�䵱ǰʱ�䣬ֻ��ʱ���֡��롣
    pTemp += 3;
    DataLen += 3;

    if(MeterNoBcdCheck(mf.MeterAddr) == TRUE)   //ֻ�б��ַ��Ϊ��ʱ��ִ�С�
    {

        /*begin:yangfei added 2013-08-05 for add HYDROMETER*/
        if(mf.ProtocolVer == HYDROMETER775_VER || mf.ProtocolVer == ZENNER_VER || mf.ProtocolVer == LANDISGYR_VER || mf.ProtocolVer == ENGELMANN_VER)
        {
            Err = HYDROMETER(&ProtocoalInfo, mf.ProtocolVer);
        }
        else
        {
            Err = METER_DataItem(&ProtocoalInfo);
        }
        /*end:yangfei added 2013-08-05 for add HYDROMETER*/
        if(Err == NO_ERR)
        {
            /*begin:yangfei added 2013-03-18 for meter data format standardized 188*/
            if(mf.ProtocolVer == HYDROMETER775_VER || mf.ProtocolVer == ZENNER_VER || mf.ProtocolVer == LANDISGYR_VER || mf.ProtocolVer == ENGELMANN_VER)
            {
                uint8 err = 0;
                *pTemp++ = sizeof(CJ188_Data) ;
                DataLen++;
                //memcpy(pTemp, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length-3));
                CJ188_Data = METER_Data_To_CJ188Format(mf.ProtocolVer, ProtocoalInfo.DataBuf, ProtocoalInfo.Length - 3, &err);
                if(err == 0)
                {
                    memcpy(pTemp, &CJ188_Data, sizeof(CJ188_Data));
                    ProtocoalInfo.Length = sizeof(CJ188_Data) + 3;
                }
                else
                {
                    debug("%s %d METER_Data_To_CJ188Format err=%d\r\n", __FUNCTION__, __LINE__, err );
                    memcpy(pTemp, ProtocoalInfo.DataBuf, (ProtocoalInfo.Length - 3)); /*�ϱ�ԭʼ����*/
                }

            }
            else
            {
                uint8 err = 0;
                *pTemp++ = sizeof(CJ188_Data);
                DataLen++;
                CJ188_Data = METER_Data_To_CJ188Format(mf.ProtocolVer, ProtocoalInfo.DataBuf, sizeof(CJ188_Data) , &err);
                memcpy(pTemp, &CJ188_Data, sizeof(CJ188_Data) );

            }
            /*end:yangfei added 2013-03-18 for meter data format standardized*/
            pTemp += sizeof(CJ188_Data) ;
            DataLen += sizeof(CJ188_Data) ;


            //OS_ENTER_CRITICAL();    //Ϊ�Ż�LCD��ʾ����ReadCmplNums++���ڱ����������
            //gREAD_TimingState.ReadCmplNums++;
            //OS_EXIT_CRITICAL();
            ReadMeterSuccess = 1;
            debug_info(gDebugModule[TASKDOWN_MODULE], "%s %d Read HeatMeter data  ok", __FUNCTION__, __LINE__);
        }
        else
        {
ERROR:
            debug("%s %d:read one meter fail\r\n", __FUNCTION__, __LINE__);
            *pTemp++ = 0x00;					//�ȼ��������ݳ���Ϊ0
            DataLen++;
            Err = NO_ERR;/*yangfei added 2013-11-09*/
        }

    }
    else   //������ַΪ�գ������ݳ�����Ϊ1������Ϊ00�����ճ���ɹ�������ֹ������
    {
        *pTemp++ = 1;
        DataLen++;
        *pTemp++ = 0;  //����Ϊ0������λ��Լ������������������Ϊ���ַΪ�ա�
        DataLen++;

        ReadMeterSuccess = 1;  //���ճɹ�������ֹ�ظ�������

    }


    //begin:  �����¿������ʾ����ֵ�����·�����ֵ��
    gu8ReadValveFail = 0;//��������ǰ���ûʧ�ܡ�

    if(MeterNoBcdCheck(mf.ValveAddr) == TRUE)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Valve Addr is Valid!");

        if(ReadMeterSuccess) 		//��������Ч���ݲŻὫ�����·�
        {
            //�����¿������ʾ����ֵ��

            Err = ValveContron(&mf, SETHEAT_DISPLAY, lu8DataIn, DataFrame, &DataLen_Vave);

            if(Err == NO_ERR)   //�򷧿��·�����ֵ��
            {
                if((mf.ValveProtocal == VALVE_DELU) || (mf.ValveProtocal == VALVE_ADE))
                {
                    memcpy(lu8DataIn, &CJ188_Data, sizeof(CJ188_Data));
                }
                else
                {
                    memcpy(lu8DataIn, &(ProtocoalInfo.DataBuf[5]), 5);
                }

                Err = ValveContron(&mf, SETHEAT_VALUE, lu8DataIn, DataFrame, &DataLen_Vave);

                if(Err == NO_ERR)
                {
                    debug_info(gDebugModule[TASKDOWN_MODULE], "%s %d Send HeatMeter data  to valve ok", __FUNCTION__, __LINE__);
                }
                else
                {
                    debugX(LOG_LEVEL_ERROR, "%s %d Send HeatMeter data  to valve  failed!\r\n", __FUNCTION__, __LINE__);
                }

            }
            else
            {
                debugX(LOG_LEVEL_ERROR, "%s %d set valve display heat value failed!\r\n", __FUNCTION__, __LINE__);
            }

        }
    }
    //end:  �����¿������ʾ����ֵ�����·�����ֵ��

    p_ReadVavleTime = pTemp;  //Ԥ��������ʱ��λ��ָ�룬���ش������ֵ��
    pTemp += 3;				  //ʱ��ռ��3�ֽڣ��롢�֡�ʱ��
    DataLen += 3;




    //begin: �������¶Ⱥͷ�״̬��
    //if((MeterNoBcdCheck(mf.ValveAddr) == TRUE)&&(Err == NO_ERR)){
    if(MeterNoBcdCheck(mf.ValveAddr) == TRUE)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Valve Addr is Valid!");

        Err = ValveContron(&mf, ReadVALVE_All, lu8DataIn, DataFrame, &DataLen_Vave);

        if(Err == NO_ERR)
        {
            memcpy(pTemp, DataFrame, DataLen_Vave);
            pTemp += DataLen_Vave;
            DataLen += DataLen_Vave;
        }
        else
        {
            memset(pTemp, 0xee, 6);
            pTemp += 6;
            DataLen += 6;

            gu8ReadValveFail = 1;  //��ǳ�������ʧ�ܡ�
        }

    }
    else
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Valve Addr is Not Valid!");
        debug_debug(gDebugModule[TASKDOWN_MODULE], "INFO: <METER_ReadMeterDataTiming> Valve Addr is Invalid!");
        memset(pTemp, 0xee, 6);
        pTemp += 6;
        DataLen += 6;
    }

    ReadDateTime(lu8ReadTime);
    memcpy(p_ReadVavleTime, lu8ReadTime, 3); //���볭��ʱ�䵱ǰʱ�䣬ֻ��ʱ���֡��롣


    //end: �������¶Ⱥͷ�״̬��

    if(1 == ReadMeterSuccess)   //�������ɹ�����ReadCmplNums++��
    {
        OS_ENTER_CRITICAL();
        gREAD_TimingState.ReadCmplNums++;
        gu16ReadMeterSuccessNum += 1;
        OS_EXIT_CRITICAL();
    }



    DataBuf[0]	= DataLen;						//�����򳤶�
    *pTemp = PUBLIC_CountCS(&DataBuf[1], DataLen);

    memcpy(EleBuf, DataBuf, DataLen + 2);

    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Timing Read Meter Complete!");
    debug_debug(gDebugModule[TASKDOWN_MODULE], "INFO: <METER_ReadMeterDataTiming> Timing Read Meter Complete!");
    return ReadMeterSuccess ? NO_ERR : ERR_1;
}

/****************************************************************************************************
**	�� ��  �� ��: YILINVALVE_ReadMeterDataTiming
**	�� ��  �� ��: ��������ȡ�¹����ֵ����ݼ���ȡ����
**	�� ��  �� ��: uint16 MeterSn -- �����;
**	�� ��  �� ��: uint8 *EleBuf -- ���յ�������֡��Ӧ;
**  ��   ��   ֵ: NO_ERR, ERR_1 -- û�н��յ�����
**	��		  ע: ��д���ж�ȡ���ݵ�������ݽṹ
**  SD����ÿ���������ռ��128�ֽڵĿռ䣬���е�һ���ֽ�Ϊ���ݳ���(1�ֽ�) + ����(N+11) + CS�ۼӺ�У��(1�ֽ�)
**	�������ݸ�ʽΪ: �ȼ������ַ(7�ֽ�) + �������ݳ���(1�ֽ�) + ��������(N) + �¶�����(3�ֽ�)
**	����ȡ�������ݲ���ȷ����û�ж�ȡ�����ݣ���涨�������ݳ���Ϊ0�����������ݡ�
*****************************************************************************************************/

uint8 YILINVALVE_ReadMeterDataTiming(uint16 MeterSn, uint8 *EleBuf)
{
    uint8 Err 		 	= 0x00;
    MeterFileType	mf;
    DELU_Protocol	ProtocoalInfo;
    CPU_SR		cpu_sr;
    TimeAreaArith *valve_t =	(TimeAreaArith *)ul_array_push(gpValve_Array);

    LOG_assert_param(EleBuf == NULL);
    LOG_assert_param(MeterSn > METER_NUM_MAX);

    Err = PARA_ReadMeterInfo(MeterSn, &mf);
    if(Err != NO_ERR)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <METER_ReadMeterDataTiming> Read Meter filed failed, MeterSn is %4XH, Return is %d"
                               , MeterSn, Err);
    }

    valve_t->user_id = mf.ControlPanelAddr[4];
    valve_t->address = *(uint16 *)mf.ValveAddr;
    valve_t->channel = mf.ChannelIndex;

    valve_t->area = *(uint16 *)(&mf.ControlPanelAddr[5]);

    (*METER_ComParaSetArray[gMETER_Table[12][0]])();
    gDownCommDev485 = gMETER_Table[12][3];
    ProtocoalInfo.PreSmybolNum	= 0;
    ProtocoalInfo.MeterType 	= 0xB0;
    memcpy(ProtocoalInfo.MeterAddr, mf.ValveAddr, 2);

    //��ȡ���Ŵ�ʱ������ȣ��ο�����Э��
    ProtocoalInfo.ControlCode 	= 0xA3;
    memset(ProtocoalInfo.DataBuf, 0x00, METER_FRAME_LEN_MAX);

    Err = METER_DataItem(&ProtocoalInfo);
    if(Err == NO_ERR)
    {
        valve_t->data_valid = 0xA5;
        valve_t->open_percent = getOpentime(valve_t->user_id, *(uint32 *)ProtocoalInfo.DataBuf);
        ProtocoalInfo.DataBuf[3] = HexToBcd(ProtocoalInfo.DataBuf[3]);
        valve_t->open_time = HexToBcdUint32(*(uint32 *)ProtocoalInfo.DataBuf);
        debug_info(gDebugModule[TIME_AREA], "userid=%d:read yilin valve open_time = %4x\r\n", valve_t->user_id, valve_t->open_time);
    }
    else
    {
        valve_t->data_valid = 0x0;
        debug_err(gDebugModule[TIME_AREA], "ERROR:read yilin valve fail controlcod = %x,userid = %x,meteraddr=%4x\r\n", ProtocoalInfo.ControlCode, valve_t->user_id, valve_t->address);
        return Err;
    }
    //��ȡ���ŷ����¶Ⱥͽ���ˮ�¶ȣ��ο�����Э��

    ProtocoalInfo.ControlCode 	= 0xA0;
    memset(ProtocoalInfo.DataBuf, 0x00, METER_FRAME_LEN_MAX);

    Err = METER_DataItem(&ProtocoalInfo);
    if(Err == NO_ERR)
    {
        if(ProtocoalInfo.DataBuf[4] & 0x2) //�������߹���
        {
            valve_t->data_valid = 0x0;
            valve_t->state |= 0x40;
        }
        /*begin:yangfei added  20140307 for show success number*/
        OS_ENTER_CRITICAL();
        gREAD_TimingState.ReadCmplNums++;
        OS_EXIT_CRITICAL();
        /*end:yangfei added  20140307 for show success number*/
        valve_t->room_temperature = HexToBcd4bit(ProtocoalInfo.DataBuf[5]) * 0x100; //�����ʱ�����λΪС��
        valve_t->forward_temperature = HexToBcd4bit(ProtocoalInfo.DataBuf[6]) * 0x100;
        valve_t->return_temperature = HexToBcd4bit(ProtocoalInfo.DataBuf[7]) * 0x100;

        debug_info(gDebugModule[TIME_AREA], "userid=%d:read yilin valve room_temperature = %4x,forward_temperature = %4x,return_temperature=%4x\r\n", valve_t->user_id, valve_t->room_temperature, valve_t->forward_temperature, valve_t->return_temperature);
    }
    else
    {
        debug_err(gDebugModule[TIME_AREA], "ERROR:%s %d:read yilin valve fail controlcod = %4x,userid = %x,meteraddr=%4x\r\n", __FUNCTION__, __LINE__, ProtocoalInfo.ControlCode, valve_t->user_id, valve_t->address);
    }

    return Err;
}
/****************************************************************************************************
**	�� ��  �� ��: METER_ReadMeterDataCur
**	�� ��  �� ��: ��������ȡ�¹��ȼ����������  ʵʱ��ȡ����
**	�� ��  �� ��: DELU_Protocol *pReadMeter -- ��³Э������;
                  uint16 MeterSn -- �����
**	�� ��  �� ��:
**  ��   ��   ֵ: NO_ERR, ERR_1 -- û�н��յ�����
**	��		  ע:
*****************************************************************************************************/
uint8 METER_ReadMeterDataCur(DELU_Protocol *pReadMeter, uint16 MeterSn)
{
    uint8 i				= 0x00;
    uint8 Err 		 	= 0x00;
    uint8 RetryTimes 	= 0x02;
    uint8 DataLen		= 0x00;
    uint8 DataBuf[128]	= {0x00};

    MeterFileType	mf;

    LOG_assert_param(pReadMeter == NULL);
    LOG_assert_param(MeterSn > METER_NUM_MAX);

    Err = PARA_ReadMeterInfo(MeterSn, &mf);
    if(Err != NO_ERR)
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: <METER_ReadMeterDataCur> Read Meter filed failed, MeterSn is %4XH, Return is %d"
                               , MeterSn, Err);
    }

    METER_ChangeChannel(mf.ChannelIndex);


    //���ݴ���ӡ�

    return Err;
}
uint8 Get_Current_Channel(void)
{
    return gCurrent_Channel;
}

void Reset_Current_Channel(void)
{
    gCurrent_Channel = 0;
}

/****************************************************************************************************
**	�� ��  �� ��: DisableAllMBusChannel
**	�� ��  �� ��:
**	�� ��  �� ��:
**	�� ��  �� ��:
**  ��   ��   ֵ:
**	��		  ע:
*****************************************************************************************************/
void DisableAllMBusChannel(void)
{
    DISABLE_MBUS();

}

/****************************************************************************************************
**	�� ��  �� ��: METER_ChangeChannel
**	�� ��  �� ��: �����������л�MBUSͨ�� 1--6
**	�� ��  �� ��: uint8 Channel -- ͨ����־
**	�� ��  �� ��:
**  ��   ��   ֵ: NO_ERR
**	��		  ע:
*****************************************************************************************************/

uint8 METER_ChangeChannel(uint8 Channel)
{
    uint8 lu8ChannelChangeFlag = 0; //���ͨ���Ƿ�仯��0-û�䣬1-�仯��

    LOG_assert_param(Channel < 1);
    LOG_assert_param(Channel > 7);

    switch(Channel)
    {
    case 1:
        if(gCurrent_Channel != 1)
        {
            ENABLE_MBUS_1();
            gDownCommDev485 = DOWN_COMM_DEV_MBUS;
            lu8ChannelChangeFlag = 1;
        }

        break;

    case 2:
        if(gCurrent_Channel != 2)
        {
            ENABLE_MBUS_2();
            gDownCommDev485 = DOWN_COMM_DEV_MBUS;
            lu8ChannelChangeFlag = 1;
        }

        break;

    case 3:
        if(gCurrent_Channel != 3)
        {
            ENABLE_MBUS_3();
            gDownCommDev485 = DOWN_COMM_DEV_MBUS;
            lu8ChannelChangeFlag = 1;
        }

        break;

    case 4:
        if(gCurrent_Channel != 4)
        {
            ENABLE_MBUS_4();
            gDownCommDev485 = DOWN_COMM_DEV_MBUS;
            lu8ChannelChangeFlag = 1;
        }

        break;

    case 5:
        if(gCurrent_Channel != 5)
        {
            ENABLE_MBUS_5();
            gDownCommDev485 = DOWN_COMM_DEV_MBUS;
            lu8ChannelChangeFlag = 1;

        }

        break;

    case 6:
        if(gCurrent_Channel != 6)
        {
            ENABLE_MBUS_6();
            gDownCommDev485 = DOWN_COMM_DEV_MBUS;
            lu8ChannelChangeFlag = 1;
        }

        break;

    case 7:
    {
        DISABLE_MBUS();
        gDownCommDev485 = DOWN_COMM_DEV_485;  //��7ͨ�����̶�Ϊ485���ߡ�
    }

    break;

    default:
        gCurrent_Channel = 0;
        break;



    }

    //LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ChangeChannel> Change MBUS Channel Successful! Channel is %d", Channel);
    if(lu8ChannelChangeFlag == 1)   //ͨ�������仯ʱ��ʱ�������ȶ���
    {
        OSTimeDly(OS_TICKS_PER_SEC);/* ͨ���л���ʱ̫�̵��±��޷��ɹ�*/
    }


    return NO_ERR;
}

/****************************************************************************************************
**	�� ��  �� ��: METER_DataItem
**	�� ��  �� ��: �����ȼ��������ţ��¿����Ⱦ����������
**	�� ��  �� ��: DELU_Protocol *pProtocoalInfo -- ����Ĳ�������
**	�� ��  �� ��: DELU_Protocol *pProtocoalInfo -- ������ݸ���
**  ��   ��   ֵ: NO_ERR -- ����ɹ�; 1 -- ����δ�ɹ�����ʱ
**	��		  ע:
*****************************************************************************************************/
uint8 METER_DataItem(DELU_Protocol *pProtocoalInfo)
{
    uint8 i 					= 0;
    uint8 Err					= 0;
    /*begin:yangfei modified 2013-10-14 for �������Դ���*/
    uint8 RetryTimes			= 3;
    //uint8 RetryTimes			= 2;
    /*end:yangfei modified 2013-10-14 for �������Դ���*/
    char MeterAddrString[40] 	= {0x00};
    uint8 DataTemp[128]			= {0x00};
    uint8 LenTemp				= 0;

    LOG_assert_param(pProtocoalInfo == NULL);

    if(pProtocoalInfo->MeterType == 0xB0)
        PUBLIC_MeterAddrToString(pProtocoalInfo->MeterAddr, MeterAddrString, 2);
    else
        PUBLIC_MeterAddrToString(pProtocoalInfo->MeterAddr, MeterAddrString, 7);

    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Reading Meter Addr is %s", MeterAddrString);

    for(i = 0; i < RetryTimes; i++)
    {
        Err = METER_MeterCommunicate(pProtocoalInfo, DataTemp, &LenTemp);
        if(Err == NO_ERR)
        {
            //LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Recived Valid Datas ");
            Err = METER_DELU_AnalDataFrame(pProtocoalInfo, DataTemp);
            if(Err == NO_ERR)
            {
                //LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Analysis Datas successful!");
                break;
            }
            else
            {
                LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Analysis Datas Failed!");
            }
        }
        else
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Recvied Datas Failed! RetryIndex is %d", i);
        }
    }
    if(i >= RetryTimes)										//����2��δ����������
    {
        return 1;
    }

    return NO_ERR;
}


/****************************************************************************************************
**	�� ��  �� ��: METER_DataItem
**	�� ��  �� ��: �����ȼ��������ţ��¿����Ⱦ����������
**	�� ��  �� ��: DELU_Protocol *pProtocoalInfo -- ����Ĳ�������
**	�� ��  �� ��: DELU_Protocol *pProtocoalInfo -- ������ݸ���
**  ��   ��   ֵ: NO_ERR -- ����ɹ�; 1 -- ����δ�ɹ�����ʱ
**	��		  ע:
*****************************************************************************************************/
uint8 VALVE_YILIN_DataItem(DELU_Protocol *pProtocoalInfo)
{
    uint8 i 					= 0;
    uint8 Err					= 0;
    /*begin:yangfei modified 2013-10-14 for �������Դ���*/
    //uint8 RetryTimes			= 3;
    uint8 RetryTimes			= 2;
    /*end:yangfei modified 2013-10-14 for �������Դ���*/
    char MeterAddrString[40] 	= {0x00};
    uint8 DataTemp[128]			= {0x00};
    uint8 LenTemp				= 0;

    LOG_assert_param(pProtocoalInfo == NULL);


    PUBLIC_MeterAddrToString(pProtocoalInfo->MeterAddr, MeterAddrString, 2);

    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Reading Meter Addr is %s", MeterAddrString);

    for(i = 0; i < RetryTimes; i++)
    {
        Err = METER_MeterCommunicate(pProtocoalInfo, DataTemp, &LenTemp);
        if(Err == NO_ERR)
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Recived Valid Datas ");
            Err = METER_DELU_AnalDataFrame(pProtocoalInfo, DataTemp);
            if(Err == NO_ERR)
            {
                LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Analysis Datas successful!");
                break;
            }
            else
            {
                LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Analysis Datas Failed!");
            }
        }
        else
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ReadMeterDataTiming> Recvied Datas Failed! RetryIndex is %d", i);
        }
    }
    if(i >= RetryTimes)										//����2��δ����������
    {
        return 1;
    }

    return NO_ERR;
}

/****************************************************************************************************
**	�� ��  �� ��: METER_DataItem
**	�� ��  �� ��: �����ȼ��������ţ��¿����Ⱦ����������
**	�� ��  �� ��: DELU_Protocol *pProtocoalInfo -- ����Ĳ�������
**	�� ��  �� ��: DELU_Protocol *pProtocoalInfo -- ������ݸ���
**  ��   ��   ֵ: NO_ERR -- ����ɹ�; 1 -- ����δ�ɹ�����ʱ
**	��		  ע:
*****************************************************************************************************/
uint8 METER_MeterDataForValve(DELU_Protocol *pProtocoalInfo, uint8 ProtocolVer)
{
    CJ188_Format  MeterData = {0, 0x05, 0, 0x05, 0, 0x17, 0, 0x35, 0, 0x2c};
    uint8 err;
    switch(ProtocolVer)
    {
    /*Э��Ϊ4(����4)���ϵ�ת��ΪDELU��ʽ*/
    case 0:  /*DELU*/
    {
        memcpy(&MeterData, pProtocoalInfo->DataBuf, sizeof(MeterData)); /* ��³Э����ȫ����CJ188��׼*/
        break;
    }
    case 1:  /*WANHUA*/
    {
        WANHUA_Format  Data;
        uint8 *pTemp = (uint8 *)&Data;
        uint8 i;
        memcpy(&Data, pProtocoalInfo->DataBuf, sizeof(Data));
        for(i = 0; i < sizeof(Data); i++) /*��������Ҫ��0x33*/
        {
            *pTemp -= 0x33;
            pTemp++;
        }
        MeterData.DailyHeat = Data.DailyHeat;
        MeterData.CurrentHeat = Data.CurrentHeat;
        MeterData.HeatPower = Data.HeatPower;
        MeterData.Flow = Data.Flow;
        MeterData.AccumulateFlow = Data.AccumulateFlow;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.ST = Data.ST;

        break;
    }
    case 2:  /*DANFOSS*/
    {
        Danfoss_Format  DanfossData;
        memcpy(&DanfossData, pProtocoalInfo->DataBuf, sizeof(DanfossData));
        To_Little_endian((uint8 *)DanfossData.DailyHeat, 4);
        To_Little_endian((uint8 *)DanfossData.CurrentHeat, 4);
        To_Little_endian((uint8 *)DanfossData.HeatPower, 4);
        To_Little_endian((uint8 *)DanfossData.Flow, 4);
        To_Little_endian((uint8 *)DanfossData.AccumulateFlow, 4);
        To_Little_endian(DanfossData.WaterInTemp, 3);
        To_Little_endian(DanfossData.WaterOutTemp, 3);
        To_Little_endian(DanfossData.AccumulateWorkTime, 3);
        To_Little_endian(DanfossData.RealTime, 7);
        To_Little_endian((uint8 *)DanfossData.ST, 2);
        memcpy(&MeterData, &DanfossData, sizeof(MeterData));
        break;
    }
    case 3:  /*LICHUANG*/
    {
        LICHUANG_Format  Data;
        memcpy(&Data, pProtocoalInfo->DataBuf, sizeof(Data));
        MeterData.DailyHeat = Data.DailyHeat;
        MeterData.CurrentHeat = Data.CurrentHeat;
        MeterData.HeatPower = Data.HeatPower;
        MeterData.Flow = Data.Flow;
        MeterData.AccumulateFlow = Data.AccumulateFlow;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.ST = Data.ST;
        break;
    }
    default:
    {
        MeterData = METER_Data_To_CJ188Format(ProtocolVer, pProtocoalInfo->DataBuf, pProtocoalInfo->Length - 3, &err);
        break;
    }

    }
    memcpy(pProtocoalInfo->DataBuf, &MeterData, sizeof(MeterData)); /*��ת�������ݱ���*/
    return ProtocolVer;
#if 0
    if(1 == ProtocolVer) /*wanhua DN20*/
    {
        /*begin:yangfei modified 2013-08-08 for WAN_HUA format to CJ188 */
#if 0
        for(i = 0; i < pProtocoalInfo->Length - 3; i++)
        {
            pProtocoalInfo->DataBuf[i] = pProtocoalInfo->DataBuf[i] - 0x33;
        }
#endif
        CJ188_Format  MeterData = {0, 0x05, 0, 0x05, 0, 0x17, 0, 0x35, 0, 0x2c};
        WANHUA_Format  Data;
        uint8 *pTemp = (uint8 *)&Data;
        uint8 i;
        memcpy(&Data, pProtocoalInfo->DataBuf, sizeof(Data));
        for(i = 0; i < sizeof(Data); i++) /*��������Ҫ��0x33*/
        {
            *pTemp -= 0x33;
            pTemp++;
        }
        MeterData.DailyHeat = Data.DailyHeat;
        MeterData.CurrentHeat = Data.CurrentHeat;
        MeterData.HeatPower = Data.HeatPower;
        MeterData.Flow = Data.Flow;
        MeterData.AccumulateFlow = Data.AccumulateFlow;

        MeterData.AccumulateFlowUnit = Data.AccumulateFlowUnit;/*��λת�����򻪵�λΪ29(L)*/
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.ST = Data.ST;

        memcpy(pProtocoalInfo->DataBuf, &MeterData, sizeof(MeterData)); /*��ת�������ݱ���*/
        /*end:yangfei modified 2013-08-08 for WAN_HUA format to CJ188 */
    }
    else if(5 == ProtocolVer)
    {
        /*begin:yangfei modified 2013-08-16 for WAN_HUA DN25 format to CJ188 */
        CJ188_Format  MeterData = {0, 0x05, 0, 0x05, 0, 0x17, 0, 0x35, 0, 0x2c};
        WANHUA_Format  Data;

        memcpy(&Data, pProtocoalInfo->DataBuf, sizeof(Data));

        MeterData.DailyHeat = Data.DailyHeat;
        MeterData.DailyHeatUnit = Data.DailyHeatUnit;
        MeterData.CurrentHeat = Data.CurrentHeat;
        MeterData.CurrentHeatUnit = Data.CurrentHeatUnit;
        MeterData.HeatPower = Data.HeatPower;
        MeterData.HeatPowerUnit = Data.HeatPowerUnit;
        MeterData.Flow = Data.Flow;
        MeterData.FlowUnit = Data.FlowUnit;
        MeterData.AccumulateFlow = Data.AccumulateFlow;
        MeterData.AccumulateFlowUnit = Data.AccumulateFlowUnit;/*��λת�����򻪵�λΪ29(L)*/
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.ST = Data.ST;

        memcpy(pProtocoalInfo->DataBuf, &MeterData, sizeof(MeterData)); /*��ת�������ݱ���*/
        /*end:yangfei modified 2013-08-16 for WAN_HUA format to CJ188 */
    }
    else
    {

    }
    return ProtocolVer;
#endif
}


/*
*/
CJ188_Format METER_Data_To_CJ188Format(uint8 ProtocolVer, uint8 *DataBuf, uint8 len, uint8 *err)
{
    CJ188_Format  MeterData = {0, 0x05, 0, 0x05, 0, 0x17, 0, 0x35, 0, 0x2c};


    switch(ProtocolVer)
    {
#if 0    /*Э��Ϊ4(����4)���ϵ�ת��ΪDELU��ʽ*/
    case 0:  /*DELU*/
    {
        memcpy(&MeterData, DataBuf, sizeof(MeterData)); /* ��³Э����ȫ����CJ188��׼*/
        break;
    }
    case 1:  /*WANHUA*/
    {
        WANHUA_Format  Data;
        uint8 *pTemp = (uint8 *)&Data;
        uint8 i;
        memcpy(&Data, DataBuf, sizeof(Data));
        for(i = 0; i < sizeof(Data); i++) /*��������Ҫ��0x33*/
        {
            *pTemp -= 0x33;
            pTemp++;
        }
        MeterData.DailyHeat = Data.DailyHeat;
        MeterData.CurrentHeat = Data.CurrentHeat;
        MeterData.HeatPower = Data.HeatPower;
        MeterData.Flow = Data.Flow;
        MeterData.AccumulateFlow = Data.AccumulateFlow;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.ST = Data.ST;

        break;
    }
    case 2:  /*DANFOSS*/
    {
        Danfoss_Format  DanfossData;
        memcpy(&DanfossData, DataBuf, sizeof(DanfossData));
        To_Little_endian((uint8 *)DanfossData.DailyHeat, 4);
        To_Little_endian((uint8 *)DanfossData.CurrentHeat, 4);
        To_Little_endian((uint8 *)DanfossData.HeatPower, 4);
        To_Little_endian((uint8 *)DanfossData.Flow, 4);
        To_Little_endian((uint8 *)DanfossData.AccumulateFlow, 4);
        To_Little_endian(DanfossData.WaterInTemp, 3);
        To_Little_endian(DanfossData.WaterOutTemp, 3);
        To_Little_endian(DanfossData.AccumulateWorkTime, 3);
        To_Little_endian(DanfossData.RealTime, 7);
        To_Little_endian((uint8 *)DanfossData.ST, 2);
        memcpy(&MeterData, &DanfossData, sizeof(MeterData));
        break;
    }
    case 3:  /*LICHUANG*/
    {
        LICHUANG_Format  Data;
        memcpy(&Data, DataBuf, sizeof(Data));
        MeterData.DailyHeat = Data.DailyHeat;
        MeterData.CurrentHeat = Data.CurrentHeat;
        MeterData.HeatPower = Data.HeatPower;
        MeterData.Flow = Data.Flow;
        MeterData.AccumulateFlow = Data.AccumulateFlow;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.ST = Data.ST;

        break;
    }
#endif

    case WANHUA_VER:   //��Э�鵥λ��ǰ�������ں�,WANHUA_VER�汾������λ���������ﲻ������
    {
        memcpy(&MeterData, DataBuf, sizeof(MeterData));

        break;
    }
    case 4:  /*��ҵ485 LYRB-USF*/
    {
        LEYE485_Format Data;
        memcpy(&Data, DataBuf, sizeof(MeterData));
        memcpy(&MeterData, DataBuf, sizeof(MeterData));
        MeterData.DailyHeat = Data.DailyHeat << 8;
        MeterData.HeatPower = 0;
        MeterData.HeatPowerUnit = 0x17;
        MeterData.CurrentHeat = (Data.CurrentHeat << 8) + Data.CurrentHeatDecimal[1];
        MeterData.AccumulateFlow = (Data.AccumulateFlow << 8) + Data.AccumulateFlowDecimal[2];
        MeterData.WaterInTemp[2] = 0;
        MeterData.WaterOutTemp[2] = 0;
        break;
    }
    case 5:  /*WANHUA ���*/
    {
        WANHUA_Format  Data;

        memcpy(&Data, DataBuf, sizeof(Data));

        MeterData.DailyHeat = Data.DailyHeat;
        MeterData.DailyHeatUnit = Data.DailyHeatUnit;
        MeterData.CurrentHeat = Data.CurrentHeat;
        MeterData.CurrentHeatUnit = Data.CurrentHeatUnit;
        MeterData.HeatPower = Data.HeatPower;
        MeterData.HeatPowerUnit = Data.HeatPowerUnit;
        MeterData.Flow = Data.Flow;
        MeterData.FlowUnit = Data.FlowUnit;
        MeterData.AccumulateFlow = Data.AccumulateFlow;
        MeterData.AccumulateFlowUnit = Data.AccumulateFlowUnit;/*��λת�����򻪵�λΪ29(L)*/
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.ST = Data.ST;

        break;
    }
    case HYDROMETER775_VER :
    {
        HYDROMETER_TO_CJ188(&MeterData, DataBuf, len);

        break;
    }
    case ZENNER_VER :
    {
        ZENNER_Format  ZENNER_data;
        memcpy(&ZENNER_data, DataBuf, sizeof(ZENNER_data));
        if(ZENNER_data.Vertion >= 4)
        {
            if(ZENNER_data.Status != 0x03)
            {
                MeterData.ST = (ZENNER_data.Status & 0xfc); /*yangfei added for ST*/
                LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d ZENNER_data.Status = %x", __FUNCTION__, __LINE__, ZENNER_data.Status);
            }
        }
        else    /*b0:��ص�, b1:�¶ȴ�������, b2:������������.�ϵĶ���V004�汾֮ǰ*/
        {
            if(ZENNER_data.Status & BIT0 == BIT0)
            {
                MeterData.ST |= BIT2_LOW_VOLTAGE;
            }
            if(ZENNER_data.Status & BIT1 == BIT1)
            {
                MeterData.ST |= BIT5_TEMP_SENSOR;
            }
            if(ZENNER_data.Status & BIT2 == BIT2)
            {
                MeterData.ST |= BIT6_FLOW_SENSOR;
            }
        }
        if(ZENNER_data.CurrentHeatUnit[0] != 0x04 || ZENNER_data.CurrentHeatUnit[1] != 0x06)
        {

            uint8 unit;
            unit = ZENNER_data.CurrentHeatUnit[1] & 0x07 ;
            if(ZENNER_data.CurrentHeatUnit[0] == 0x04)
            {
                MeterData.CurrentHeat = HexToBcdUint32(ZENNER_data.CurrentHeat * (pow(10, unit - 3 - 3 + 2))); /*10(nnn-3) Wh 0,001 Wh to 10 000 Wh*/ /*wh-->>kwh 2:0.01kw*/
            }
            LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d ZENNER_data.CurrentHeatUnit = %x%x", \
                                   __FUNCTION__, __LINE__, ZENNER_data.CurrentHeatUnit[0], ZENNER_data.CurrentHeatUnit[1]);
        }
        else
        {
            MeterData.CurrentHeat = HexToBcdUint32(ZENNER_data.CurrentHeat * 100);
            debug_info(gDebugModule[METER_DATA], "ZENNER_data.CurrentHeat=%d  MeterData.CurrentHeat = %x  \r\n", ZENNER_data.CurrentHeat, MeterData.CurrentHeat);
        }
        if(ZENNER_data.PowerUnit[0] != 0x04 || ZENNER_data.PowerUnit[1] != 0x2b)
        {
            uint8 unit = 0;
            unit = ZENNER_data.PowerUnit[1] & 0x07;
            if(ZENNER_data.PowerUnit[0] == 0x04)
            {
                MeterData.HeatPower = HexToBcdUint32(ZENNER_data.Power * (pow(10, unit - 4))); /**/
            }
            LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d ZENNER_data.PowerUnit = %x%x", \
                                   __FUNCTION__, __LINE__, ZENNER_data.PowerUnit[0], ZENNER_data.PowerUnit[1]);
        }
        else
        {

            MeterData.HeatPower = HexToBcdUint32(ZENNER_data.Power / 10);
            debug_info(gDebugModule[METER_DATA], "ZENNER_data.Power=%d  MeterData.HeatPower = %x  \r\n", ZENNER_data.Power, MeterData.HeatPower);
        }
        if(ZENNER_data.FlowRateUnit[0] != 0x04 || ZENNER_data.FlowRateUnit[1] != 0x3b)
        {
            uint8 unit = 0;
            unit = ZENNER_data.FlowRateUnit[1] & 0x07;
            if(ZENNER_data.FlowRateUnit[0] == 0x04)
            {
                MeterData.Flow = HexToBcdUint32(ZENNER_data.FlowRate * (pow(10, unit - 2))); /*E011 1nnn Volume Flow 10(nnn-6) m3/h*/
            }
            LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d ZENNER_data.FlowRateUnit = %x%x", \
                                   __FUNCTION__, __LINE__, ZENNER_data.FlowRateUnit[0], ZENNER_data.FlowRateUnit[1]);
        }
        else
        {
            MeterData.Flow = HexToBcdUint32(ZENNER_data.FlowRate * 10);
            debug_info(gDebugModule[METER_DATA], "ZENNER_data.FlowRate=%d  MeterData.Flow = %x  \r\n", ZENNER_data.FlowRate, MeterData.Flow);
        }
        if(ZENNER_data.VolumeUnit[0] != 0x04 || ZENNER_data.VolumeUnit[1] != 0x14)
        {
            uint8 unit = 0;
            unit = ZENNER_data.VolumeUnit[1] & 0x07;
            if(ZENNER_data.VolumeUnit[0] == 0x04)
            {
                MeterData.AccumulateFlow = HexToBcdUint32(ZENNER_data.Volume * (pow(10, unit - 4))); /*E001 0nnn Volume 10(nnn-6) m3*/
            }
            LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d ZENNER_data.VolumeUnit = %x%x", \
                                   __FUNCTION__, __LINE__, ZENNER_data.VolumeUnit[0], ZENNER_data.VolumeUnit[1]);
        }
        else
        {
            MeterData.AccumulateFlow = HexToBcdUint32(ZENNER_data.Volume);
            debug_info(gDebugModule[METER_DATA], "ZENNER_data.Volume=%d  MeterData.AccumulateFlow = %x  \r\n", ZENNER_data.Volume, MeterData.AccumulateFlow);
        }
        {
            if(ZENNER_data.FlowTemperatureUnit[0] != 0x02 || ZENNER_data.FlowTemperatureUnit[1] != 0x59)
            {
                uint8 unit = 0;
                unit = ZENNER_data.FlowTemperatureUnit[1] & 0x03;
                if(ZENNER_data.FlowTemperatureUnit[0] == 0x02)/*10(nn-3) ��C*/
                {
                    uint32 Temperature;
                    Temperature = HexToBcdUint32(ZENNER_data.FlowTemperature * (pow(10, unit - 3 + 2)));
                    memcpy(MeterData.WaterInTemp, (char *)&Temperature, 3);
                }
                LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d ZENNER_data.FlowTemperatureUnit = %x%x", \
                                       __FUNCTION__, __LINE__, ZENNER_data.FlowTemperatureUnit[0], ZENNER_data.FlowTemperatureUnit[1]);
            }
            else
            {
                uint32 Temperature;
                Temperature = HexToBcdUint32(ZENNER_data.FlowTemperature);/*0.1 ��C*/
                memcpy(MeterData.WaterInTemp, (char *)&Temperature, 3);
                debug_info(gDebugModule[METER_DATA], "ZENNER_data.FlowTemperature=%d Temperature = %x  \r\n", ZENNER_data.FlowTemperature, Temperature);
            }
            if(ZENNER_data.ReturnTemperatureUnit[0] != 0x02 || ZENNER_data.ReturnTemperatureUnit[1] != 0x5d)
            {
                uint8 unit = 0;
                unit = ZENNER_data.ReturnTemperatureUnit[1] & 0x03;
                if(ZENNER_data.ReturnTemperatureUnit[0] == 0x02)/*10(nn-3) ��C*/
                {
                    uint32 Temperature;
                    Temperature = HexToBcdUint32(ZENNER_data.ReturnTemperature * (pow(10, unit - 3 + 2)));
                    memcpy(MeterData.WaterOutTemp, (char *)&Temperature, 3);
                }
                LOG_WriteSysLog_Format(LOG_LEVEL_ERROR, "ERROR: %s %d ZENNER_data.ReturnTemperatureUnit = %x%x", \
                                       __FUNCTION__, __LINE__, ZENNER_data.ReturnTemperatureUnit[0], ZENNER_data.ReturnTemperatureUnit[1]);
            }
            else
            {
                uint32 Temperature;
                Temperature = HexToBcdUint32(ZENNER_data.ReturnTemperature);/*0.1 ��C*/
                //MeterData.WaterInTemp = HexToBcdUint32(ZENNER_data.FlowTemperature)<<8;
                memcpy(MeterData.WaterOutTemp, (char *)&Temperature, 3);
                debug_info(gDebugModule[METER_DATA], "ZENNER_data.ReturnTemperature=%d Temperature = %x  \r\n", ZENNER_data.ReturnTemperature, Temperature);
            }
        }

        {
            uint32 WorkTime = 0;
            WorkTime =  HexToBcdUint32(ZENNER_data.Operating_Time);
            memcpy(MeterData.AccumulateWorkTime, (char *)&WorkTime, 3);
            debug_info(gDebugModule[METER_DATA], "ZENNER_data.Operating_Time=%d WorkTime = %x  \r\n", ZENNER_data.Operating_Time, WorkTime);
            //MeterData.AccumulateWorkTime= HexToBcdUint32(ZENNER_data.Operating_Time)<<8;
        }
        {
            /*���ʵʱʱ��*/
            uint8 SystemTime[6] = {0};
            int i = 0;
            memcpy(SystemTime, gSystemTime, 6);

            MeterData.RealTime[6] = 0x20;
            for(i = 0; i < 6; i++)
            {
                MeterData.RealTime[i] = HexToBcd(SystemTime[i]);
                debug_info(gDebugModule[METER_DATA], " MeterData.RealTime[%d] = %x \r\n", i, MeterData.RealTime[i] );
            }

        }
        break;
    }
    case LANDISGYR_VER:
    {
        HYDROMETER_TO_CJ188(&MeterData, DataBuf, len);
    }
    break;
    case ENGELMANN_VER:
    {
        ENLEMAN_TO_CJ188(&MeterData, DataBuf, len);
    }
    break;

    case ZHENYU_VER_2:
    {
        ZHENYU2_Format  Data;

        memcpy(&Data, DataBuf, sizeof(MeterData));
        //memcpy(&MeterData,DataBuf,sizeof(MeterData));

        MeterData.DailyHeat = Data.DailyHeat;	  //����������
        MeterData.DailyHeatUnit = Data.DailyHeatUnit;
        MeterData.CurrentHeat = Data.CurrentHeat;	 //��ǰ����
        MeterData.CurrentHeatUnit = Data.CurrentHeatUnit;
        MeterData.HeatPower = Data.HeatPower;	    //�ȹ���
        MeterData.HeatPowerUnit = Data.HeatPowerUnit;
        MeterData.Flow = Data.Flow;		 //˲ʱ����	 -------����������Ĳ�ͬ��
        //MeterData.Flow =   MeterData.Flow << 4;
        MeterData.FlowUnit = Data.FlowUnit;
        MeterData.AccumulateFlow = Data.AccumulateFlow;	  //�������ۼ�����
        MeterData.AccumulateFlowUnit = Data.AccumulateFlowUnit;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);			//��ˮ�¶�
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);			 //��ˮ�¶�
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.RealTime[6]  = 0x20;

        MeterData.ST = Data.ST;


        break;

    }
    case JINGWEI_VER:	  //��γ������
    {
        JingWeiXiping_Format   Data;	  //����֡��ʽ�뺣���ĵ���֡��ʽһ������Э�鲻���ݣ�Ӳ��Э�������Э��

        memcpy(&Data, DataBuf, sizeof(Data));

        MeterData.DailyHeat = Data.DailyHeat;	//����������
        MeterData.DailyHeatUnit = Data.DailyHeatUnit;
        MeterData.CurrentHeat = Data.CurrentHeat;
        MeterData.CurrentHeatUnit = Data.CurrentHeatUnit;
        MeterData.HeatPower = Data.HeatPower;	//�ȹ���
        MeterData.HeatPowerUnit = Data.HeatPowerUnit;
        MeterData.Flow = Data.Flow;	//˲ʱ����
        MeterData.FlowUnit = Data.FlowUnit;
        MeterData.AccumulateFlow = Data.AccumulateFlow;	  //�������ۼ�����
        MeterData.AccumulateFlowUnit = Data.AccumulateFlowUnit;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);	//��ˮ�¶�
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);		 //��ˮ�¶�
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);	 //�ۼƹ���ʱ��
        memcpy(MeterData.RealTime, Data.RealTime, 7);		//ʵʱʱ��
        MeterData.ST = Data.ST;

        break;
    }
    case JINGWEIFE_VER:	  //��γ������
    {
        JingWeiFE_Format  Data;
        memcpy(&Data, DataBuf, sizeof(Data));

        MeterData.DailyHeat = Data.DailyHeat;	//����������

        MeterData.DailyHeat =	 MeterData.DailyHeat << 8;
        MeterData.CurrentHeat = Data.CurrentHeat;

        MeterData.CurrentHeat =	 MeterData.CurrentHeat << 8;

        MeterData.HeatPower = 0x00;// Data.BiaoHao;	//��� ռ�ã�ֱ�Ӹ�ֵΪ0
        // MeterData.Flow = Data.Flow;	//˲ʱ����	//��ʶ	ռ�ã�ֱ�Ӹ�ֵΪ0

        MeterData.Flow = Data.Flow;		 //˲ʱ����	 -------����������Ĳ�ͬ��
        MeterData.Flow =   MeterData.Flow << 4;

        MeterData.AccumulateFlow = Data.AccumulateFlow;	  //�������ۼ�����
        MeterData.AccumulateFlow =   MeterData.AccumulateFlow << 8;

        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);	//��ˮ�¶�
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);		 //��ˮ�¶�
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);	 //�ۼƹ���ʱ��
        memcpy(MeterData.RealTime, Data.RealTime, 7);		//ʵʱʱ��
        MeterData.ST = Data.ST;

        break;

    }

    case LI_CHUANG_2:
    {


        LICHUANG_Format  Data;


        memcpy(&Data, DataBuf, sizeof(MeterData));
        //memcpy(&MeterData,DataBuf,sizeof(MeterData));

        MeterData.DailyHeat = Data.DailyHeat;	  //����������
        MeterData.DailyHeatUnit = Data.DailyHeatUnit;
        MeterData.CurrentHeat = Data.CurrentHeat;	 //��ǰ����
        MeterData.CurrentHeatUnit = Data.CurrentHeatUnit;
        MeterData.HeatPower = Data.HeatPower;	    //�ȹ���
        MeterData.HeatPowerUnit = Data.HeatPowerUnit;
        MeterData.Flow = Data.Flow;		 //˲ʱ����	 -------����������Ĳ�ͬ��
        MeterData.Flow =   MeterData.Flow << 4;
        MeterData.AccumulateFlow = Data.AccumulateFlow;	  //�������ۼ�����
        MeterData.AccumulateFlowUnit = Data.AccumulateFlowUnit;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);			//��ˮ�¶�
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);			 //��ˮ�¶�
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.ST = Data.ST;


        break;
    }


    case JICHENG_VER:
    {

        JICHENG_Format  Data;	//�����ȱ�Э����֡
        memcpy(&Data, DataBuf, (sizeof(MeterData) - 1));	 //���ɵ�״̬λ��Ƚ�CJ188��˵����һ���ֽڣ���CJ188��2����

        //Data.DailyHeat.gp22_u8[0] =	DataBuf[17]  ;
        //Data.DailyHeat.gp22_u8[0] = 0x55  ;
        //Data.DailyHeat.gp22_u8[1] =	  0x75	;
        //Data.DailyHeat.gp22_u8[2] =	  0x0f	;
        //Data.DailyHeat.gp22_u8[3] =	0x44  ;

        Data.DailyHeat.gp22_u8[0] = DataBuf[3] ;
        Data.DailyHeat.gp22_u8[1] =  DataBuf[2]  ;
        Data.DailyHeat.gp22_u8[2] =  DataBuf[1]  ;
        Data.DailyHeat.gp22_u8[3] =  DataBuf[0]  ;

        MeterData.DailyHeat = (uint32)Data.DailyHeat.gp22_float;  //���ս�����
        MeterData.DailyHeat =	  HexToBcdUint32 (MeterData.DailyHeat);


        MeterData.CurrentHeat = Data.CurrentHeat;   //��ǰ����
        MeterData.HeatPower = Data.HeatPower;   //�ȹ���
        MeterData.Flow = Data.Flow; //˲ʱ����
        MeterData.AccumulateFlow = Data.AccumulateFlow;	//�������ۼ�����
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3); //��ˮ�¶�
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);		//��ˮ�¶�
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);	//�ۼƹ���ʱ��
        //memcpy(MeterData.RealTime,Data.RealTime,7); 	  //ʵʱʱ��
        MeterData.RealTime[0]  = Data.RealTime[6];
        MeterData.RealTime[1]  = Data.RealTime[5];
        MeterData.RealTime[2]  = Data.RealTime[4];
        MeterData.RealTime[3]  = Data.RealTime[3];
        MeterData.RealTime[4]  = Data.RealTime[2];

        MeterData.RealTime[5]  = Data.RealTime[1];
        MeterData.RealTime[6]  = Data.RealTime[0];

        MeterData.ST = 1;

        break;
    }



    case YITONGDA_VER:
    {
        YITONGDA_Format Data;
        uint32 temp = 0;
        memcpy(&Data, DataBuf, sizeof(Data));
        temp = Data.CurrentCool;  //��ͨ�ｫ��ǰ�ۼ������ŵ��˵�ǰ����λ�á�
        temp = temp << 8;  //Ѧ������ͨ�����������CJ188�иĶ�����ǰ������С��λ��
        MeterData.DailyHeat = temp;	  //����������,��ͨ�ﵱǰ����������������
        MeterData.DailyHeatUnit = 0x05;
        MeterData.CurrentHeat = temp;	 //��ǰ����
        MeterData.CurrentHeatUnit = 0x05;
        //MeterData.HeatPower = Data.HeatPower;	    //�ڴ�ͨ�ȱ�û���ȹ���
        MeterData.HeatPower = 0;
        MeterData.HeatPowerUnit = 0x17;

        temp = Data.Flow;
        temp = temp << 4; //Ѧ������ͨ�����������CJ188�иĶ�������3λС��λ��
        MeterData.Flow = temp;		 //˲ʱ��
        MeterData.FlowUnit = 0x35;
        temp = Data.AccumulateFlow;
        temp = temp << 4;//Ѧ������ͨ�����������CJ188�иĶ����ۼ�����1λС��λ��
        MeterData.AccumulateFlow = temp;	  //�������ۼ�����
        MeterData.AccumulateFlowUnit = 0x2c;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);			//��ˮ�¶�
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);			 //��ˮ�¶�
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);

        MeterData.ST = Data.ST;

        break;
    }

    case LIANQIANG_VER485:  /*��ǿ���ν��ܵ�485���Э����֡����ҵ485��֡��ʽһ������Ӳ��ͨ����ͬ*/
    {
        LEYE485_Format Data;
        memcpy(&Data, DataBuf, sizeof(MeterData));
        memcpy(&MeterData, DataBuf, sizeof(MeterData));
        MeterData.DailyHeat = Data.DailyHeat << 8;
        MeterData.HeatPower = 0;		  //��Э��û���ȹ�����һ��
        MeterData.HeatPowerUnit = 0x17;
        MeterData.CurrentHeat = (Data.CurrentHeat << 8) + Data.CurrentHeatDecimal[1];
        MeterData.AccumulateFlow = (Data.AccumulateFlow << 8) + Data.AccumulateFlowDecimal[2];
        MeterData.WaterInTemp[2] = 0;
        MeterData.WaterOutTemp[2] = 0;
        break;
    }

    case LIANQIANG_VERMBUS:  /*��ǿ���ν��ܵ�485���Э����֡����ҵ485��֡��ʽһ������Ӳ��ͨ����ͬ*/
    {
        LEYE485_Format Data;
        memcpy(&Data, DataBuf, sizeof(MeterData));
        memcpy(&MeterData, DataBuf, sizeof(MeterData));
        MeterData.DailyHeat = Data.DailyHeat << 8;
        MeterData.HeatPower = 0;
        MeterData.HeatPowerUnit = 0x17;
        MeterData.CurrentHeat = (Data.CurrentHeat << 8) + Data.CurrentHeatDecimal[1];
        MeterData.AccumulateFlow = (Data.AccumulateFlow << 8) + Data.AccumulateFlowDecimal[2];
        MeterData.WaterInTemp[2] = 0;
        MeterData.WaterOutTemp[2] = 0;
        break;
    }

    case WANHUA_TO_DELU_VER:   //��Э�鵥λ��ǰ�������ں�
    {
        WANHUA_Format  Data;
        uint8 *pTemp = (uint8 *)&Data;
        uint8 i = 0;
        memcpy(&Data, DataBuf, sizeof(Data));
        for(i = 0; i < sizeof(Data); i++) //����򻪽���ʱ��Ҫ��ȥ0x33.
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

        break;
    }

    case ZHENYU_S_VER:
    {
        ZHENYU_S_Format  Data;
        memcpy(&Data, DataBuf, sizeof(ZHENYU_S_Format));

        MeterData.DailyHeat = Data.DailyHeat;	  //����������
        MeterData.DailyHeatUnit = Data.DailyHeatUnit;
        MeterData.CurrentHeat = Data.CurrentHeat;	 //��ǰ����
        MeterData.CurrentHeatUnit = Data.CurrentHeatUnit;
        MeterData.HeatPower = Data.HeatPower;	    //�ȹ���
        MeterData.HeatPowerUnit = Data.HeatPowerUnit;
        MeterData.Flow = Data.Flow;		 //˲ʱ����	 -------����������Ĳ�ͬ��
        MeterData.FlowUnit = Data.FlowUnit;
        MeterData.AccumulateFlow = Data.AccumulateFlow1;	  //�������ۼ�����
        MeterData.AccumulateFlowUnit = Data.AccumulateFlowUnit1;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3);			//��ˮ�¶�
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);			 //��ˮ�¶�
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.RealTime[6]  = 0x20;

        MeterData.ST = Data.ST;


        break;

    }

    case MITE_VER:
    {
        YITONGDA_Format Data;   //���������ȱ����ݸ�ʽ�������ͨ��һ����
        uint32 temp = 0;
        memcpy(&Data, DataBuf, sizeof(Data));
        temp = Data.CurrentCool;	//��ǰ�ۼ������ŵ��˵�ǰ����λ�á�
        temp = temp << 8;  //���������CJ188�иĶ�����ǰ������С��λ��
        MeterData.DailyHeat = temp;	//����������,Ҳ��������������
        MeterData.DailyHeatUnit = Data.CurrentCoolUnit;
        MeterData.CurrentHeat = temp;    //��ǰ����
        MeterData.CurrentHeatUnit = Data.CurrentCoolUnit;
        //MeterData.HeatPower = Data.HeatPower;	  //�ȱ�û���ȹ���
        MeterData.HeatPower = 0;
        MeterData.HeatPowerUnit = 0x17;

        temp = Data.Flow;
        temp = temp << 4; //Ѧ������ͨ�����������CJ188�иĶ�������3λС��λ��
        MeterData.Flow = temp;	   //˲ʱ����
        MeterData.FlowUnit = 0x35;	//��λǿ��������/Сʱ��
        temp = Data.AccumulateFlow;
        temp = temp << 8;//�ۼ�����2λС��λ��
        MeterData.AccumulateFlow = temp;		//�������ۼ�����
        MeterData.AccumulateFlowUnit = Data.AccumulateFlowUnit;
        memcpy(MeterData.WaterInTemp, Data.WaterInTemp, 3); 		 //��ˮ�¶�
        memcpy(MeterData.WaterOutTemp, Data.WaterOutTemp, 3);		 //��ˮ�¶�
        memcpy(MeterData.AccumulateWorkTime, Data.AccumulateWorkTime, 3);
        memcpy(MeterData.RealTime, Data.RealTime, 7);
        MeterData.ST = Data.ST;

        break;
    }



    default:  //�����ģ���Ԫ,hx,nl��������ȫ���ϱ�Э�飬��������Э�����---������
    {
        memcpy(&MeterData, DataBuf, sizeof(MeterData)); /* ��³Э����ȫ����CJ188��׼*/

    }
    break;
    }

    return MeterData;
}
/*
**	�� ��  �� ��: To_Little_endian
**	�� ��  �� ��: ���תС�˺���
**	�� ��  �� ��: uint8* Databuff
**	�� ��  �� ��: uint8* Databuff
**  ��   ��   ֵ: NO_ERR
**	��		  ע:
*/
uint8 To_Little_endian(uint8 *Databuff, uint8 size)
{
    int i = 0;
    uint8 temp;
    for(i = 0; i < size / 2; i++)
    {
        temp = Databuff[i];
        Databuff[i] = Databuff[size - i - 1];
        Databuff[size - i - 1] = temp;
    }
    return NO_ERR;
}


uint8   HYDROMETER_TO_CJ188(CJ188_Format  *MeterData, uint8 *DataBuf, uint8 len)
{
    uint8 data[256] = {0};
    uint8 i = 0;
    ST_DIF  DIF = {0};
    ST_DIFE  DIFE = {0};
    ST_VIF  VIF = {0};
    ST_VIFE VIFE = {0};
    //uint8   VIFE_Index =0;

    memcpy(data, DataBuf, len);

    for(i = 15; i < len;)
    {
        DIF = *(ST_DIF *)(&data[i++]);
        if(DIF.Extension == 1)
        {
            do
            {
                DIFE = *(ST_DIFE *)(&data[i++]);
            }
            while(DIFE.Extension == 1);  /*DIFE:0 �� 10 (1 Byte each)*/

        }
        VIF = *(ST_VIF *)(&data[i++]);
        //VIFE_Index = 0;
        if(VIF.Extension == 1)
        {
            do
            {
                VIFE = *(ST_VIFE *)(&data[i++]);
            }
            while(VIFE.Extension == 1);  /*VIFE:0 �� 10 (1 Byte each)*/

        }
        if(DIF.Extension || DIF.LSB)
        {
            i += DIF.Data;
            continue;
        }
        if(DIF.Function == 0)/*Instantaneous value*/
        {
            if(VIF.Data >= 0 && VIF.Data <= 0X07) /*E000 0nnn Energy 10(nnn-3) Wh 0,001 Wh to 10 000 Wh*/
            {
                uint8 unit = 0;
                uint8 num = 0;
                uint32 BCD_Data = 0;
                uint32 HEX_Data = 0;
                uint8 unit_add = 0;
                unit = VIF.Data & 0x07;
                if(DIF.BCD)
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        BCD_Data += (data[i++] << 8 * num);
                    }
                }
                else
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        HEX_Data += (data[i++] << 8 * num);
                    }
                }
                if(unit == 7)
                {
                    if(HEX_Data >= 99999999)/*BCD ̫��*/
                    {
                        HEX_Data = HEX_Data / 100; /*MWH ->100MWH*/
                        unit_add = 2;
                    }
                    if(HEX_Data)
                    {
                        BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                    }
                    MeterData->CurrentHeat = BCD_Data >> 4 * abs(unit - 3 - 6 + 2); /*0.01 MWh*/
                    MeterData->CurrentHeatUnit = MWH + unit_add;
                }
                else if(unit >= 4)
                {
                    if(HEX_Data >= 99999999)/*BCD ̫��*/
                    {
                        HEX_Data = HEX_Data / 1000; /*MWH ->100MWH*/
                        unit_add = 3;
                    }
                    if(HEX_Data)
                    {
                        BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                    }
                    if((BCD_Data > 0x999999) && (unit - 3 - 3 + 2 >= 2))
                    {
                        BCD_Data >>= 3 * 4;
                        unit_add = 3;
                    }
                    else if((BCD_Data > 0x9999999) && (unit - 3 - 3 + 2 >= 1))
                    {
                        BCD_Data >>= 3 * 4;
                        unit_add = 3;
                    }
                    MeterData->CurrentHeat = BCD_Data << 4 * (unit - 3 - 3 + 2); /*0.01 kWH*/
                    MeterData->CurrentHeatUnit =  KWH + unit_add;
                }
                else
                {
                    MeterData->CurrentHeat = BCD_Data >> 4 * abs(unit - 3 - 3 + 2); /*0.01 kWH*/
                    MeterData->CurrentHeatUnit =  KWH + unit_add;
                }
                debug_info(gDebugModule[EN13757], "line=%d  CurrentHeat = %x  unit = %x \r\n", __LINE__, MeterData->CurrentHeat, MeterData->CurrentHeatUnit);
            }
            else if(VIF.Data >= 0x08 && VIF.Data <= 0x0F) /*E000 1nnn Energy 10(nnn) J 0,001 kJ to 10 000 kJ*/
            {
                uint8 unit = 0;
                uint8 num = 0;
                uint32 BCD_Data = 0;
                uint32 HEX_Data = 0;
                uint8 unit_add = 0;
                unit = VIF.Data & 0x07;
                if(DIF.BCD)
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        BCD_Data += (data[i++] << 8 * num);
                    }
                }
                else
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        HEX_Data += (data[i++] << 8 * num);
                    }
                }
                if(unit == 7)
                {
                    if(HEX_Data >= 99999999)/*BCD ̫��*/
                    {
                        HEX_Data = HEX_Data / 100; /*GJ ->100GJ*/
                        unit_add = 2;
                    }
                    if(HEX_Data)
                    {
                        BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                    }
                    MeterData->CurrentHeat = BCD_Data >> 4 * abs(unit - 3 - 6 + 2); /*0.01 GJ*/
                    MeterData->CurrentHeatUnit = GJ + unit_add;
                }
                else if(unit >= 4)
                {
                    if(HEX_Data >= 99999999)/*BCD ̫��*/
                    {
                        HEX_Data = HEX_Data / 1000;
                        unit_add = 3;
                    }
                    if(HEX_Data)
                    {
                        BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                    }
                    if((BCD_Data > 0x999999) && (unit - 3 - 3 + 2 >= 2))
                    {
                        BCD_Data >>= 3 * 4;
                        unit_add = 3;
                    }
                    else if((BCD_Data > 0x9999999) && (unit - 3 - 3 + 2 >= 1))
                    {
                        BCD_Data >>= 3 * 4;
                        unit_add = 3;
                    }
                    MeterData->CurrentHeat = BCD_Data << 4 * (unit - 3 - 3 + 2); /*0.01 MJ*/
                    MeterData->CurrentHeatUnit =  MJ + unit_add;
                }
                else
                {
                    MeterData->CurrentHeat = BCD_Data >> 4 * abs(unit - 3 - 3 + 2); /*0.01 MJ*/
                }
                debug_info(gDebugModule[EN13757], "line=%d  CurrentHeat = %x  unit = %x \r\n", __LINE__, MeterData->CurrentHeat, MeterData->CurrentHeatUnit);
            }
            else if(VIF.Data >= 0X10 && VIF.Data <= 0X17) /*E001 0nnn Volume 10(nnn-6) m3 0,001 l to 10 000 l*/
            {
                uint8 unit = 0;
                uint8 num = 0;
                uint32 BCD_Data = 0;
                unit = VIF.Data & 0x07;
                if(DIF.BCD)
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        BCD_Data += (data[i++] << 8 * num);
                    }
                }
                else
                {
                    uint32 HEX_Data = 0;
                    for(num = 0; num < DIF.Data; num++)
                    {
                        HEX_Data += (data[i++] << 8 * num);
                    }
                    BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                }
                if(unit >= 4)
                {
                    MeterData->AccumulateFlow = BCD_Data << 4 * (unit - 6 + 2); /*0.01 m^3*/
                }
                else
                {
                    MeterData->AccumulateFlow = BCD_Data >> 4 * abs(unit - 6 + 2);
                }
                debug_info(gDebugModule[EN13757], "line=%d  AccumulateFlow = %x  unit = %x \r\n", __LINE__, MeterData->AccumulateFlow, MeterData->AccumulateFlowUnit);
            }
            else if(VIF.Data >= 0X28 && VIF.Data <= 0X2F) /*E010 1nnn Power 10(nnn-3) W 0,001 W to 10 000 W*/
            {
                uint8 unit = 0;
                uint8 num = 0;
                uint32 BCD_Data = 0;
                unit = VIF.Data & 0x07;
                if(DIF.BCD)
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        BCD_Data += (data[i++] << 8 * num);
                    }
                }
                else
                {
                    uint32 HEX_Data = 0;
                    for(num = 0; num < DIF.Data; num++)
                    {
                        HEX_Data += (data[i++] << 8 * num);
                    }
                    if(HEX_Data >= 99999999)/*BCD ̫��*/
                    {
                        HEX_Data = HEX_Data / 1000;
                        MeterData->HeatPowerUnit = MW;
                    }
                    BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                }
                if(unit >= 4)
                {
                    MeterData->HeatPower = BCD_Data << 4 * (unit - 3 - 3 + 2); /*0.01 kW*/
                }
                else
                {
                    MeterData->HeatPower = BCD_Data >> 4 * abs(unit - 3 - 3 + 2);
                }
                debug_info(gDebugModule[EN13757], "line=%d  HeatPower = %x  unit = %x \r\n", __LINE__, MeterData->HeatPower, MeterData->HeatPowerUnit);
            }
            else if(VIF.Data >= 0X30 && VIF.Data <= 0X37) /*E011 0nnn Power 10(nnn) J/h 0,001 kJ/h to 10 000 kJ/h*/
            {
                uint8 unit = 0;
                uint8 num = 0;
                uint32 BCD_Data = 0;
                unit = VIF.Data & 0x07;
                if(DIF.BCD)
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        BCD_Data += (data[i++] << 8 * num);
                    }
                }
                else
                {
                    uint32 HEX_Data = 0;
                    for(num = 0; num < DIF.Data; num++)
                    {
                        HEX_Data += (data[i++] << 8 * num);
                    }
                    if(HEX_Data >= 99999999)/*BCD ̫��*/
                    {
                        HEX_Data = HEX_Data / 1000;
                        MeterData->HeatPowerUnit = MW;
                    }
                    BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                }
                if(unit >= 4)
                {
                    MeterData->HeatPower = BCD_Data << 4 * (unit - 3 - 3 + 2); /*0.01 MJ*/
                }
                else
                {
                    MeterData->HeatPower = BCD_Data >> 4 * abs(unit - 3 - 3 + 2);
                }
                MeterData->HeatPower  = MeterData->HeatPower / 3.6;
                debug_info(gDebugModule[EN13757], "line=%d  HeatPower = %x  unit = %x \r\n", __LINE__, MeterData->HeatPower, MeterData->HeatPowerUnit);
            }
            else if(VIF.Data >= 0X38 && VIF.Data <= 0X3F) /*E011 1nnn Volume Flow 10(nnn-6) m3/h 0,001 l/h to 10 000 l/h*/
            {
                uint8 unit = 0;
                uint8 num = 0;
                uint32 BCD_Data = 0;
                unit = VIF.Data & 0x07;
                if(DIF.BCD)
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        BCD_Data += (data[i++] << 8 * num);
                    }
                }
                else
                {
                    uint32 HEX_Data = 0;
                    for(num = 0; num < DIF.Data; num++)
                    {
                        HEX_Data += (data[i++] << 8 * num);
                    }
                    if(HEX_Data >= 99999999)/*BCD ̫��*/
                    {

                        //MeterData->HeatPowerUnit = MW;
                    }
                    BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                }
                if(unit >= 2)
                {
                    MeterData->Flow = BCD_Data << 4 * (unit - 6 + 4); /*0.0001 m3/h*/
                }
                else
                {
                    MeterData->Flow = BCD_Data >> 4 * abs(unit - 6 + 4);
                }
                debug_info(gDebugModule[EN13757], "line=%d  Flow = %x  unit = %x \r\n", __LINE__, MeterData->Flow, MeterData->FlowUnit);
            }
            else if(VIF.Data >= 0X58 && VIF.Data <= 0X5B) /*E101 10nn Flow Temperature 10(nn-3) ��C 0,001 ��C to 1 ��C*/
            {
                uint8 unit = 0;
                uint8 num = 0;
                uint32 BCD_Data = 0;
                unit = VIF.Data & 0x03;
                if(DIF.BCD)
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        BCD_Data += (data[i++] << 8 * num);
                    }
                }
                else
                {
                    uint32 HEX_Data = 0;
                    for(num = 0; num < DIF.Data; num++)
                    {
                        HEX_Data += (data[i++] << 8 * num);
                    }
                    BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                }
                if(unit >= 1)
                {
                    BCD_Data = BCD_Data << 4 * (unit - 3 + 2); /*0.01 ��C*/
                }
                else
                {
                    BCD_Data = BCD_Data >> 4 * abs(unit - 3 + 2);
                }
                memcpy(MeterData->WaterInTemp, (char *)&BCD_Data, 3);
                debug_info(gDebugModule[EN13757], "line=%d  WaterInTemp = %x.%x  \r\n", __LINE__, MeterData->WaterInTemp[1], MeterData->WaterInTemp[1]);
            }
            else if(VIF.Data >= 0X5C && VIF.Data <= 0X5F) /*E101 11nn Return Temperature 10(nn-3) ��C 0,001 ��C to 1 ��C*/
            {
                uint8 unit = 0;
                uint8 num = 0;
                uint32 BCD_Data = 0;
                unit = VIF.Data & 0x03;
                if(DIF.BCD)
                {
                    for(num = 0; num < DIF.Data; num++)
                    {
                        BCD_Data += (data[i++] << 8 * num);
                    }
                }
                else
                {
                    uint32 HEX_Data = 0;
                    for(num = 0; num < DIF.Data; num++)
                    {
                        HEX_Data += (data[i++] << 8 * num);
                    }
                    BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                }
                if(unit >= 1)
                {
                    BCD_Data = BCD_Data << 4 * (unit - 3 + 2); /*0.01 ��C*/
                }
                else
                {
                    BCD_Data = BCD_Data >> 4 * abs(unit - 3 + 2);
                }
                memcpy(MeterData->WaterOutTemp, (char *)&BCD_Data, 3);
                debug_info(gDebugModule[EN13757], "line=%d  WaterOutTemp = %x.%x \r\n", __LINE__, MeterData->WaterOutTemp[1], MeterData->WaterOutTemp[0]);
            }
            else if(VIF.Data >= 0X24 && VIF.Data <= 0X27)
                /*E010 01nn Operating Time   nn = 00b seconds  nn = 01b minutes nn = 10b hours nn = 11b days nn = 11 days*/
            {
                i += DIF.Data;
            }
            else if(VIF.Data == 0X7B)
            {
                if(VIFE.value == 0 || VIFE.value == 1) /*E000 000n Energy 10(n-1) MWh 0.1MWh to 1MWh*/
                {
                    uint8 unit = 0;
                    uint8 num = 0;
                    uint32 BCD_Data = 0;
                    uint32 HEX_Data = 0;
                    uint8 unit_add = 0;
                    unit = VIFE.value & 0x01;
                    if(DIF.BCD)
                    {
                        for(num = 0; num < DIF.Data; num++)
                        {
                            BCD_Data += (data[i++] << 8 * num);
                        }
                        if(BCD_Data > 0x999999)
                        {
                            BCD_Data = BCD_Data >> 8; /*MWH ->100MWH*/
                            unit_add = 2;
                        }
                    }
                    else
                    {
                        for(num = 0; num < DIF.Data; num++)
                        {
                            HEX_Data += (data[i++] << 8 * num);
                        }
                        if(HEX_Data > 999999)/*BCD ̫��*/
                        {
                            HEX_Data = HEX_Data / 100; /*MWH ->100MWH*/
                            unit_add = 2;
                        }
                        BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                    }

                    {
                        MeterData->CurrentHeat = BCD_Data << 4 * abs(unit - 1 + 2); /*0.01 MWH*/
                        MeterData->CurrentHeatUnit = MWH + unit_add;
                    }
                    debug_info(gDebugModule[EN13757], "line=%d  CurrentHeat = %x  unit = %x \r\n", __LINE__, MeterData->CurrentHeat, MeterData->CurrentHeatUnit);

                }
                else if(VIFE.value == 8 || VIFE.value == 9) /*E000 100n Energy 10(n-1) GJ 0.1GJ to 1GJ*/
                {
                    uint8 unit = 0;
                    uint8 num = 0;
                    uint32 BCD_Data = 0;
                    uint32 HEX_Data = 0;
                    uint8 unit_add = 0;
                    unit = VIFE.value & 0x01;
                    if(DIF.BCD)
                    {
                        for(num = 0; num < DIF.Data; num++)
                        {
                            BCD_Data += (data[i++] << 8 * num);
                        }
                        if(BCD_Data > 0x999999)
                        {
                            BCD_Data = BCD_Data >> 8; /*GJ ->100GJ*/
                            unit_add = 2;
                        }
                    }
                    else
                    {
                        for(num = 0; num < DIF.Data; num++)
                        {
                            HEX_Data += (data[i++] << 8 * num);
                        }
                        if(HEX_Data >= 999999)/*BCD ̫��*/
                        {
                            HEX_Data = HEX_Data / 100; /*GJ ->100GJ*/
                            unit_add = 2;
                        }
                        BCD_Data = HexToBcdUint32(HEX_Data);//HEX_Data MAX 0x5F5E0FF
                    }

                    {
                        MeterData->CurrentHeat = BCD_Data << 4 * abs(unit - 1 + 2); /*0.01 GJ*/
                        MeterData->CurrentHeatUnit = GJ + unit_add;
                    }
                    debug_info(gDebugModule[EN13757], "line=%d  CurrentHeat = %x  unit = %x \r\n", __LINE__, MeterData->CurrentHeat, MeterData->CurrentHeatUnit);
                }
                else
                {
                    i += DIF.Data;
                }
            }
            else
            {
                i += DIF.Data;
            }


        }
        else if(DIF.Function == 1)/*Maximum value*/
        {
            i += DIF.Data;
        }
        else if(DIF.Function == 2)/*Minimum value*/
        {
            i += DIF.Data;
        }
        else if(DIF.Function == 3)/*Value during error state*/
        {
            i += DIF.Data;
        }
    }

    {
        /*���ʵʱʱ��*/
        uint8 SystemTime[6] = {0};
        int i = 0;
        memcpy(SystemTime, gSystemTime, 6);
        MeterData->RealTime[6] = 0x20;
        for(i = 0; i < 6; i++)
        {
            MeterData->RealTime[i] = HexToBcd(SystemTime[i]);
        }

        if(gDebugModule[EN13757] >= 7)
        {
            debug("20%x-%x-%x %x:%x:%x\r\n", MeterData->RealTime[5], MeterData->RealTime[4], MeterData->RealTime[3], MeterData->RealTime[2], \
                  MeterData->RealTime[1], MeterData->RealTime[0]);
        }
    }
    return 0;
}





/*liuzy add ���������ϵ�����ת�� ��cj188*/

uint8  ENLEMAN_TO_CJ188(CJ188_Format  *MeterData, uint8 *DataBuf, uint8 len)
{
    uint8 err = 0;

    uint8 i = 0;

    uint8 u8LenCount = 0;

    uint8 u8LenPerVar = 0;

    uint32 u32TotalValue = 0;

    uint32 u32TotlHeatValue = 0;

    uint32 u32flow = 0;

    uint32 u32HeatPower = 0;

    uint32 u32CurrentHeat = 0;

    uint8 u8data[4] = {0};

    uint16 u16TempIn = 0;

    uint16 u16TempOut = 0;

    uint16 u16AccWorkTime = 0;


    memcpy(u8data, DataBuf + 29, 4); //�����������ݿ��� ��29��Ϊ��������


    err = To_Little_endian(u8data, 4); //���������ݰ���Э�鷴ת
    for(i = 0; i < 4; i++)
    {
        u32TotalValue |= u8data[i] << (3 - i) * 8;
    }

    switch (*(DataBuf + 28)) //��ӵ�λ������λû�м���

    {

    case 0x13:  //0.001m3

        u32TotalValue = u32TotalValue / 10;

        u32TotalValue = HexToBcdUint32(u32TotalValue);

        MeterData->AccumulateFlow = u32TotalValue;  // ������

        MeterData->AccumulateFlowUnit = M3;
        break;
    case 0x14:  //0.01m3

        u32TotalValue = u32TotalValue;

        u32TotalValue = HexToBcdUint32(u32TotalValue);

        MeterData->AccumulateFlow = u32TotalValue;  // ������

        MeterData->AccumulateFlowUnit = M3;
        break;
    case 0x15:  //0.1m3
        u32TotalValue = u32TotalValue * 10;

        u32TotalValue = HexToBcdUint32(u32TotalValue);

        MeterData->AccumulateFlow = u32TotalValue;  // ������

        MeterData->AccumulateFlowUnit = M3;
        break;
    case 0x16:  //m3
        u32TotalValue = u32TotalValue * 100;

        u32TotalValue = HexToBcdUint32(u32TotalValue);

        MeterData->AccumulateFlow = u32TotalValue;  // ������
        MeterData->AccumulateFlowUnit = M3;
        break;

    default:

        break;

    }

    u8LenCount = 29 + (*(DataBuf + 27)); //  ��ʼ���������ϳ��ȶ�λ�������ĳ����жϴ�

    u8LenPerVar = *(DataBuf + u8LenCount);

    if(*(DataBuf + u8LenCount + 1) == 0xfb) //��λ�������ֽ��ж�
    {
        memcpy(u8data, DataBuf + u8LenCount + 3, 4); //���������ݿ���������
        err = To_Little_endian(u8data, 4);  //��ת�ֽ�
        for(i = 0; i < 4; i++)
        {
            u32CurrentHeat |= u8data[i] << (3 - i) * 8;
        }
        switch (*(DataBuf + u8LenCount + 2)) //��0xfb��һ���ֽ��ж�
        {

        case 0x00:  //��λ 0.1mwh
            //u32CurrentHeat = (uint32)u8data;
            u32CurrentHeat = u32CurrentHeat * 10; //��ΪCJ188�Ǻ���λΪС��
            u32CurrentHeat = HexToBcdUint32(u32CurrentHeat);
            MeterData->CurrentHeat = u32CurrentHeat; // ������
            MeterData->CurrentHeatUnit = MWH;

            break;

        case 0x01: //��λMWH
            //u32CurrentHeat = (uint32)u8data;
            u32CurrentHeat = u32CurrentHeat * 100;
            u32CurrentHeat = HexToBcdUint32(u32CurrentHeat);
            MeterData->CurrentHeat = u32CurrentHeat; // ������
            MeterData->CurrentHeatUnit = MWH;
            break;

        default:
            break;
        }
        u8LenCount = u8LenCount + 3 + 4;
    }
    else  //��λΪһ���ֽ�
    {
        memcpy(u8data, DataBuf + u8LenCount + 2, 4); //���������ݿ���������
        err = To_Little_endian(u8data, 4);  //��ת�ֽ�
        for(i = 0; i < 4; i++)
        {
            u32CurrentHeat |= u8data[i] << (3 - i) * 8;
        }
        switch(*(DataBuf + u8LenCount + 1))
        {
        case 0x05:  //0.1kwh
            //u32CurrentHeat = (uint32)u8data;
            u32CurrentHeat = u32CurrentHeat * 10; //��ΪCJ188�Ǻ���λΪС��
            u32CurrentHeat = HexToBcdUint32(u32CurrentHeat);
            MeterData->CurrentHeat = u32CurrentHeat; // ������
            MeterData->CurrentHeatUnit = KWH;
            break;
        case 0x06: //0.001mwh
            //u32CurrentHeat = (uint32)u8data;
            u32CurrentHeat = u32CurrentHeat / 10; //��ΪCJ188�Ǻ���λΪС��
            u32CurrentHeat = HexToBcdUint32(u32CurrentHeat);
            MeterData->CurrentHeat = u32CurrentHeat; // ������
            MeterData->CurrentHeatUnit = MWH;
            break;
        case 0x07: //0.01MWH
            //u32CurrentHeat = (uint32)u8data;
            u32CurrentHeat = u32CurrentHeat;  //��ΪCJ188�Ǻ���λΪС��
            u32CurrentHeat = HexToBcdUint32(u32CurrentHeat);
            MeterData->CurrentHeat = u32CurrentHeat; // ������
            MeterData->CurrentHeatUnit = MWH;
            break;
        case 0x0e:  //0.001Gj ��284kwh 0.284MWH
            //u32CurrentHeat = (uint32)u8data;
            u32CurrentHeat = u32CurrentHeat * 284 / 10; //��ΪCJ188�Ǻ���λΪС��
            u32CurrentHeat = HexToBcdUint32(u32CurrentHeat);
            MeterData->CurrentHeat = u32CurrentHeat; // ������
            MeterData->CurrentHeatUnit = MWH;
            break;
        default:
            break;

        }
        u8LenCount = u8LenCount + 2 + 4;
    }

    u8LenCount = u8LenCount + 16 ; //�����������շѵǼ� ֱ������

    memcpy(u8data, DataBuf + u8LenCount + 2, 4); //���������ݿ���������

    err = To_Little_endian(u8data, 4);  //��ת�ֽ�

    for(i = 0; i < 4; i++)
    {
        u32flow |= u8data[i] << (3 - i) * 8;
    }

    switch(*(DataBuf + u8LenCount + 1))
    {

    case 0x39:  //0.01l/h

        //u32flow = (uint32)u8data;
        u32flow = u32flow;  //��ΪCJ188�Ǻ���λΪС��
        u32flow = HexToBcdUint32(u32flow);
        MeterData->Flow = u32flow;
        MeterData->FlowUnit = L_H;

        break;

    case 0x3a:  //0.1l/h
        //u32flow = (uint32)u8data;
        u32flow = u32flow * 10; //��ΪCJ188�Ǻ���λΪС��
        u32flow = HexToBcdUint32(u32flow);
        MeterData->Flow = u32flow;
        MeterData->FlowUnit = L_H;

        break;

    case 0x3b:   //1L/h
        //u32flow = (uint32)u8data;
        u32flow = u32flow * 100; //��ΪCJ188�Ǻ���λΪС��
        u32flow = HexToBcdUint32(u32flow);
        MeterData->Flow = u32flow;
        MeterData->FlowUnit = L_H;
        break;

    case 0x3c:  //0.01m3/h
        //u32flow = (uint32)u8data;
        u32flow = u32flow;  //��ΪCJ188�Ǻ���λΪС��
        u32flow = HexToBcdUint32(u32flow);
        MeterData->Flow = u32flow;
        MeterData->FlowUnit = M3_H;
        break;
    case 0x3d: //0.1m3/h
        //u32flow = (uint32)u8data;
        u32flow = u32flow * 10; //��ΪCJ188�Ǻ���λΪС��
        u32flow = HexToBcdUint32(u32flow);
        MeterData->Flow = u32flow;
        MeterData->FlowUnit = M3_H;
        break;
    case 0x3e: //1m3/h
        //u32flow = (uint32)u8data;
        u32flow = u32flow * 100; //��ΪCJ188�Ǻ���λΪС��
        u32flow = HexToBcdUint32(u32flow);
        MeterData->Flow = u32flow;
        MeterData->FlowUnit = M3_H;
        break;

    default:
        break;

    }
    u8LenCount = u8LenCount + 2 + 4;


    memcpy(u8data, DataBuf + u8LenCount + 2, 4);	//�ѹ������ݿ���������

    err = To_Little_endian(u8data, 4);	 //��ת�ֽ�

    for(i = 0; i < 4; i++)
    {
        u32HeatPower |= u8data[i] << (3 - i) * 8;
    }

    switch(*(DataBuf + u8LenCount + 1))
    {
    case 0x2a: //0.1W
        //u32HeatPower= (uint32)u8data;
        u32HeatPower = u32HeatPower * 10; //��ΪCJ188�Ǻ���λΪС��
        u32HeatPower = HexToBcdUint32(u32HeatPower);
        MeterData->HeatPower = u32HeatPower;
        MeterData->HeatPowerUnit = W;
        break;
    case 0x2b:  //W
        //u32HeatPower = (uint32)u8data;
        u32HeatPower = u32HeatPower * 100; //��ΪCJ188�Ǻ���λΪС��
        u32HeatPower = HexToBcdUint32(u32HeatPower);
        MeterData->HeatPower = u32HeatPower;
        MeterData->HeatPowerUnit = W;
        break;
    case 0x2c: //0.01KW
        //u32HeatPower = (uint32)u8data;
        u32HeatPower = u32HeatPower;  //��ΪCJ188�Ǻ���λΪС��
        u32HeatPower = HexToBcdUint32(u32HeatPower);
        MeterData->HeatPower = u32HeatPower;
        MeterData->HeatPowerUnit = KW;
        break;
    case 0x2d:  //0.1KW
        //u32HeatPower = (uint32)u8data;
        u32HeatPower = u32HeatPower * 10; //��ΪCJ188�Ǻ���λΪС��
        u32HeatPower = HexToBcdUint32(u32HeatPower);
        MeterData->HeatPower = u32HeatPower;
        MeterData->HeatPowerUnit = KW;
        break;
    case 0x2e:  //1KW
        //u32HeatPower = (uint32)u8data;
        u32HeatPower = u32HeatPower * 100; //��ΪCJ188�Ǻ���λΪС��
        u32HeatPower = HexToBcdUint32(u32HeatPower);
        MeterData->HeatPower = u32HeatPower;
        MeterData->HeatPowerUnit = KW;
        break;

    default:
        break;
    }



    u8LenCount = u8LenCount + 2 + 4;

    memcpy(u8data, DataBuf + u8LenCount + 2, 2);	//�ѽ�ˮ�¶ȿ���������

    err = To_Little_endian(u8data, 2);	 //��ת�ֽ�

    u16TempIn = ((u8data[0] << 8) | u8data[1]);

    u16TempIn = HexToBcd2bit(u16TempIn);

    memcpy(&(MeterData->WaterInTemp[1]), (char *)&u16TempIn, 2);

    MeterData->WaterInTemp[0] = 0;

    u8LenCount = u8LenCount + 4;



    memcpy(u8data, DataBuf + u8LenCount + 2, 2);	//�ѻ�ˮ�¶ȿ���������

    err = To_Little_endian(u8data, 2);	 //��ת�ֽ�

    u16TempOut = ((u8data[0] << 8) | u8data[1]);

    u16TempOut = HexToBcd2bit(u16TempOut);

    memcpy(&(MeterData->WaterOutTemp[1]), (char *)&u16TempOut, 2);

    MeterData->WaterOutTemp[0] = 0;

    u8LenCount = u8LenCount + 4;


    u8LenCount = u8LenCount + 8; //�����²���ۻ�ʱ��ĳ��Ⱥ͵�λֱ�ӵ�����


    memcpy(u8data, DataBuf + u8LenCount + 2, 2);	//���ۻ�����ʱ�俽��������

    err = To_Little_endian(u8data, 2);	 //��ת�ֽ�

    u16AccWorkTime = ((u8data[1] << 8) | u8data[0]); //��λ����

    u16AccWorkTime = u16AccWorkTime * 24; //ת����Сʱ

    u16AccWorkTime = HexToBcd2bit(u16AccWorkTime);

    memcpy(&(MeterData->AccumulateWorkTime[0]), (char *)&u16AccWorkTime, 2);

    //MeterData->AccumulateWorkTime[0] = 0;

    {
        /*���ʵʱʱ��*/
        uint8 SystemTime[6] = {0};
        int i = 0;
        memcpy(SystemTime, gSystemTime, 6);
        MeterData->RealTime[6] = 0x20;
        for(i = 0; i < 6; i++)
        {
            MeterData->RealTime[i] = HexToBcd(SystemTime[i]);
        }

        if(gDebugModule[EN13757] >= 7)
        {
            debug("20%x-%x-%x %x:%x:%x\r\n", MeterData->RealTime[5], MeterData->RealTime[4], MeterData->RealTime[3], MeterData->RealTime[2], \
                  MeterData->RealTime[1], MeterData->RealTime[0]);
        }
    }


    return 0;

}




//����͸���á�
uint8 Receive_Read_ParamFrame(uint8 dev, uint8 *buf, uint16 Out_Time, uint8 *datalen)
{
    uint8 data = 0;
    uint16 i = 0;
    uint8 readinfo[256] = {0}; //���֧�ֽ���100�ֽڡ�
    uint8 lu8RecDateLen = 0;


    i = 0;

    while(i < 256) 												 //��֡ͷ
    {
        if(DuGetch(dev, &data, 1 * OS_TICKS_PER_SEC))
        {
            break;
        }

        readinfo[i++] = data;
        lu8RecDateLen += 1;

    }

    *datalen = lu8RecDateLen;
    memcpy(buf, readinfo, lu8RecDateLen);

    if(lu8RecDateLen == 0)
        return 1;
    else
        return 0;


}


//����͸���á�

uint8 ReadData_Communicate(uint8 *DataFrame, uint8 *DataLen)
{
    uint8 err;

    //uint8 dev = DOWN_COMM_DEV_MBUS;
    uint8 dev = 0;
    uint8 i = 0;
    uint8 len = 0;

    uint8 DataBuf[METER_FRAME_LEN_MAX * 2];
    LOG_assert_param(DataFrame == NULL);
    LOG_assert_param(DataLen == NULL);

    dev = gDownCommDev485;   //gDownCommDev485������458���߻���MBus���ߡ�

    DuQueueFlush(dev);				 //��ջ�����
    DuSend(dev, (uint8 *)DataFrame,  *DataLen);

    OSTimeDly(OS_TICKS_PER_SEC / 2);
    //OSTimeDlyHMSM(0,0,1,0);

    err = Receive_Read_ParamFrame(dev, DataBuf, 0, &len);


    if(err == NO_ERR)
    {
        memcpy(DataFrame, &DataBuf[0], len);
        *DataLen = len;
        return NO_ERR;
    }


    return 1;
}




//ר��������Э�鷧�������ݽ��ա�
uint8 Yilin_Valve_ReceiveFrame(uint8 *buf, uint8 *datalen)
{
    uint8 data 	= 0x00;
    uint8 Cs	= 0x00;
    uint8 dev = 0;
    uint32 i, j;

    LOG_assert_param(buf == NULL);
    LOG_assert_param(datalen == NULL);

    dev = gDownCommDev485;   //gDownCommDev485������458���߻���MBus���ߡ�

    DuQueueFlush(dev);				//��ջ�����
    DuSend(dev, buf,  *datalen);

    OSTimeDly(OS_TICKS_PER_SEC / 2);

    *datalen = 0;

    i = 30;
    while(i--)														//��֡ͷ
    {
        FeedTaskDog();
        if(DuGetch(dev, &data, 2 * OS_TICKS_PER_SEC))
            return 1;
        if(data == 0x50)	break;
    }
    Cs 		= data;
    *buf++ 	= data;

    for(i = 0; i < 2; i++)												//��ַ
    {
        if(DuGetch(dev, &data, OS_TICKS_PER_SEC))
            return 2;
        Cs 		+= data;
        *buf++	 = data;
    }

    for(j = 0; j < 8; j++)											//������
    {
        if(DuGetch(dev, &data, OS_TICKS_PER_SEC))
            return 3;
        *buf++  = data;
        Cs     += data;
    }

    if(DuGetch(dev, &data, OS_TICKS_PER_SEC))
        return 4;		//У���ֽ�
    Cs = Cs ^ 0xA5;
    if(data != Cs)
        return 5;
    *buf++	= data;

    *datalen 	= 12;

    return NO_ERR;
}





/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/
