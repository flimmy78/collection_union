/*******************************************Copyright (c)*******************************************
**									ɽ������ռ似����˾(�����ֲ�)                                **
**  ��   ��   ��: ReadMeterdata.cpp																  **
**  ��   ��   ��: ������																		  **
**	��   ��   ��: 0.1																		      **
**  �� ��  �� ��: 2012��9��20�� 													     	      **
**  ��        ��: zigbeeЭ��									      						  **
**	�� ��  �� ¼:   							  												  **
****************************************************************************************************/
/********************************************** include *********************************************/
#include <includes.h>
#include "app_flashmem.h"
#include "app_up.h"
/********************************************** static *********************************************/
extern OS_EVENT *UpLandAnswMbox;
extern TermParaSaveType gPARA_TermPara;

static uint8 gPRO_MsgIndex	= 0;							//�ϴ����ݵ���Ϣ���
static uint8 gPRO_ReceiveData[1024];						//���н����������֡�ֽ�
static uint8 gPRO_SendData[1024];							//���з����������֡�ֽ�
char gPrintData[1024] = {0};

uint8 gDebugModule[20] = {0};
uint8 gDebugBuffer[250] = {0};
extern uint16 gDebugLength;
/********************************************************************************************************
**  �� ��  �� ��: PRO_InitGlobalVar
**	�� ��  �� ��: ��ʼ��ȫ�ֱ���
**	�� ��  �� ��: none
**	�� ��  �� ��: none
**  ��   ��   ֵ: none
**	��		  ע:
*********************************************************************************************************/
void PRO_InitGlobalVar(void)
{
    gPRO_MsgIndex = 0x00;
    memset(gPRO_ReceiveData, 0x00, sizeof(gPRO_ReceiveData));
    memset(gPRO_SendData, 0x00, sizeof(gPRO_SendData));
    memset(gDebugModule, 0x00, sizeof(gDebugModule));

    memset(gu8MBusShortFlag, 0, METER_CHANNEL_NUM);
    gu8ReadAllMeterFlag = 0;

}
/********************************************************************************************************
**  �� ��  �� ��: IfFrameHead
**	�� ��  �� ��: ͷָ��
**	�� ��  �� ��: uint8 bHead
**	�� ��  �� ��: none
**  ��   ��   ֵ: none
**	��		  ע:
*********************************************************************************************************/
uint8 _Protocol::IfFrameHead(uint8 bHead)
{
    if (bHead != frameStart) return FALSE;
    return TRUE;
}
/********************************************************************************************************
**  �� ��  �� ��: CreateFrame
**	�� ��  �� ��: ����֡
**	�� ��  �� ��: uint8 *_data
**                uint8 *_sendBuf
**                uint16 &lenFrame
**	�� ��  �� ��: none
**  ��   ��   ֵ: none
**	��		  ע:
*********************************************************************************************************/
void _Protocol::CreateFrame(uint8 *_data, uint8 *_sendBuf, uint16 &lenFrame)
{
    uint8 *pTemp			= _sendBuf;
    uint8 *pHeadCheck		= NULL;
    uint8 *pContentCheck	= NULL;

    LOG_assert_param( _data == NULL );
    LOG_assert_param(_sendBuf == NULL );

    _ProtocolType *p_ProData = NULL;
    p_ProData = (_ProtocolType *)_data;

    lenFrame = 0;
    *pTemp++ = 0xFB;							//ǰ����������Ĭ��Ϊ2��
    *pTemp++ = 0xFB;

    *pTemp++ = 0x7B;							//��ʼ���� 0x7B
    pHeadCheck = pTemp;							//��ϢͷУ��Ԥ��
    *pTemp++ = PROTOCOL_VER;					//�汾��
    lenFrame = 4;

    memcpy(pTemp, p_ProData->DestAddr, 6);		//Դ��ַΪ��������ַ������վ��������Ŀ���ַ
    pTemp += 6;
    memcpy(pTemp, p_ProData->SourceAddr, 6);	//Ŀ���ַΪ��վ��ַ������վ������Դ��ַ
    pTemp += 6;
    lenFrame += 12;

    *pTemp++ = gPRO_MsgIndex++;					//MID ��Ϣ����

    *pTemp++ = p_ProData->MsgLength;			//��Ϣ�峤��
    *pTemp++ = p_ProData->MsgLength >> 8;

    *pTemp++ = p_ProData->MsgType;				//��Ϣ����
    lenFrame += 4;

    memcpy(pTemp, p_ProData->TimeSmybol, 6);	//ʱ���ǩ
    pTemp += 6;

    *pTemp++ = CountCheck(pHeadCheck, 23);		//��ϢͷУ���ֽ�
    pContentCheck = pTemp;						//��Ϣ��У���ֽ�Ԥ��
    lenFrame += 7;

    memcpy(pTemp, p_ProData->Data.pDataBack, p_ProData->MsgLength);
    pTemp += p_ProData->MsgLength;
    lenFrame += p_ProData->MsgLength;
    //��Ϣ��У���ֽ�
    *pTemp++ = CountCheck(pContentCheck, p_ProData->MsgLength);

    *pTemp++ = 0xFD;
    *pTemp++ = 0xFD;

    lenFrame += 3;

    //����Ϊ�����վ������
    //*pTemp++ = 0x7D;

    //lenFrame += 4;
}
/********************************************************************************************************
**  �� ��  �� ��: ReceiveFrame
**	�� ��  �� ��: ����֡
**	�� ��  �� ��: uint8* _receBuf
**                uint16 &lenFrame
**	�� ��  �� ��: none
**  ��   ��   ֵ: none
**	��		  ע:
*********************************************************************************************************/
uint8 _Protocol::ReceiveFrame(uint8 *_receBuf, uint16 &lenFrame)
{
    int8 CheckSource 	= 0;
    int8 CheckDest 		= 0;
    uint8 b1, b2;
    uint8 HeadCheck 	= 0x00;
    uint8 ContentCheck 	= 0x00;
    uint8 *pTemp		= _receBuf;
    uint8 *pHeadTemp;
    uint8 *pContentTemp;
    uint8 SourceAddr[6] = {0x00};		    //Դ��ַ�����ͷ���ַ
    uint8 DestAddr[6]	= {0x00};			//Ŀ�ĵ�ַ�����շ���ַ
    uint8 TimeSmybol[6] = {0x00};			//TS ʱ���ǩ
    uint16 DataLen = 0, lenTemp = 0, lenTemp2 = 0;

    LOG_assert_param( _receBuf == NULL );

    CPU_SR		cpu_sr;

    pTemp++;								//����֡ͷ
    pHeadTemp = pTemp;						//Ԥ��ָ�룬���ڶ���Ϣͷ����Ч��
    pTemp++;								//Э��汾��

    memcpy(SourceAddr, pTemp, sizeof(SourceAddr));
    pTemp += sizeof(SourceAddr);			//����Դ��ַ
    memcpy(DestAddr, pTemp, sizeof(DestAddr));
    pTemp += sizeof(DestAddr);				//����Ŀ�ĵ�ַ
    OS_ENTER_CRITICAL();
    CheckSource = CmpMeterNo(SourceAddr, gPARA_TermPara.HostAddr);
    CheckDest	= CmpMeterNo(DestAddr, gPARA_TermPara.TermAddr);
    OS_EXIT_CRITICAL();

    if((CheckSource != 0) || (CheckDest != 0))
    {

        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <_Protocol::ReceiveFrame> Source or Dest Addr is not correct!");
        return 2;
    }
    pTemp++;								//MID��Ϣ���

    b1 = *pTemp++;
    b2 = *pTemp++;
    DataLen =  (b2 << 8) + b1;				//�����򳤶�2���ֽ�

    pTemp++;								//MT��Ϣ����

    memcpy(TimeSmybol, pTemp, sizeof(TimeSmybol));
    pTemp += sizeof(TimeSmybol);			//����ʱ���ǩ
    /*yangfei deleted 2013-01-22 for debug need changed*/
#if 0
    if(TimeCheck(TimeSmybol))
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <_Protocol::ReceiveFrame> TimeSmybol is not correct!");
        return 3;
    }
#endif
    //����Ϣͷ���м���
    HeadCheck = *pTemp++;
    if(HeadCheck != CountCheck(pHeadTemp, 23) )
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <_Protocol::ReceiveFrame> HeadCheck is not correct!");
        return 5;
    }

    pContentTemp = pTemp;					//Ԥ��ָ�����Ϣ�����Ч��

    lenTemp2 = lenFrame;
    lenTemp = DataLen + 26 + 2;				// �����򳤶� + ��Ϣͷ + 2���ֽڵĽ�����
    lenFrame = lenTemp;
    /*yangfei deleted 2013-01-22 for debug need changed*/
#if 1
    if (lenTemp2 < lenTemp)
    {
        //���Ȳ�����δ���յ�����֡
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <_Protocol::ReceiveFrame> ReceivedFrame Lenght is not correct!");
        return 4;
    }
#endif

    pTemp = _receBuf + lenFrame - 3;
    ContentCheck = *pTemp++;
    if(ContentCheck != CountCheck(pContentTemp, DataLen) )
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <_Protocol::ReceiveFrame> BodyCheck is not correct!");
        return 5;
    }

    if ( (*pTemp++ != 0xFD) || (*pTemp++ != 0xFD))
    {
        LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <_Protocol::ReceiveFrame> EndSmybol is not correct!");
        return 6;
    }
    /*begin:yangfei added 2013-03-12 for ֡ת��*/
#if  0
    {
        uint8 PreSmybol[2] = {0xFB, 0xFB};
        if(CmpMeterNo(DestAddr, gPARA_TermPara.HostAddr) == 0 && CmpMeterNo(SourceAddr, gPARA_TermPara.TermAddr) != 0) /*ת����������������*/
        {
            UpDevSend(UP_COMMU_DEV_GPRS, PreSmybol, 2);
            UpDevSend(UP_COMMU_DEV_GPRS,  _receBuf, lenFrame);
            return 7;
        }
        if(CheckSource == 0 && CheckDest != 0) /*ת����������������*/
        {
            UpDevSend(UP_COMMU_DEV_ZIGBEE,  PreSmybol, 2);
            UpDevSend(UP_COMMU_DEV_ZIGBEE,  _receBuf, lenFrame);
            return 8;
        }
    }
#endif
    /*end:yangfei added 2013-03-12 for ֡ת��*/
    return 0;
}


/********************************************************************************************************
**  �� ��  �� ��: AnalysisFrame
**	�� ��  �� ��: ֡����
**	�� ��  �� ��: uint8* _receBuf
**                uint16 &lenFrame
**	�� ��  �� ��: none
**  ��   ��   ֵ: none
**	��		  ע:
*********************************************************************************************************/
uint8 _Protocol::AnalysisFrame(uint8 *_receBuf, uint16 &lenBuf)
{
    uint8 *pTemp 	= NULL;
    uint8 b1, b2;
    uint8 err = 0;

    //_receBufΪ���յ����б��ĵ�ָ�룬ָ��ͷһ��ΪFRAME_START
    LOG_assert_param( _receBuf == NULL );

    _ProtocolType _ProData;
    _ProData.Data.pData = gPRO_ReceiveData;
    _ProData.Data.pDataBack = gPRO_SendData;

    pTemp = _receBuf;
    pTemp++;									//��Ϣͷ
    _ProData._Ver = *pTemp++;					//Э��汾��
    memcpy(_ProData.SourceAddr, pTemp, 6);		//Դ��ַ
    pTemp += 6;
    memcpy(_ProData.DestAddr, pTemp, 6);		//Ŀ���ַ
    pTemp += 6;

    _ProData.MsgID = *pTemp++;					//��Ϣ���

    b1 = *pTemp++;
    b2 = *pTemp++;
    _ProData.MsgLength = (b2 << 8) + b1;			//��Ϣ�峤��

    _ProData.MsgType = *pTemp++;				//��Ϣ����

    memcpy(_ProData.TimeSmybol, pTemp, 6);		//ʱ���ǩ
    pTemp += 6;

    pTemp++;									//��ϢͷЧ��

    _ProData.Data.pData 	= pTemp;			//��Ϣ��
    _ProData.Data.lenData 	= _ProData.MsgLength;
    /*begin:yangfei added 2013-02-18 */
    if(gDebugModule[TASKUP_MODULE] >= KERN_DEBUG)
    {
        PUBLIC_HexStreamToString(_receBuf, 28 + _ProData.MsgLength, gPrintData);
        debug_debug(gDebugModule[TASKUP_MODULE], "recevie data:%s\r\n", gPrintData);
    }
    /*end:yangfei added 2013-02-18*/
    if( (_ProData.MsgType == 0x2D) )
    {
        if( (_ProData.Data.lenData == 0x01) && (_ProData.Data.pData[0] == 0x01) )
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <AnalysisFrame> Recvie GPRS UpLand Confirm Frame OK! Then Post UpLandAnswMbox!");
            OSMboxPost(UpLandAnswMbox, (void *)1);
        }
        return 1;
    }
    if( (_ProData.MsgType == 0x2F) )
    {
        if( (_ProData.Data.lenData == 0x01) && (_ProData.Data.pData[0] == 0x01) )
        {
            LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <AnalysisFrame> Recvie GPRS Heart Confirm Frame OK!");
            return 2;
        }

    }

    UpdGprsRunSta_FeedRecDog();
    UpdGprsRunSta_FeedSndDog();
    LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <AnalysisFrame> Recvie GPRS Data OK! Clear GPRS Send and Recive Dog!"
                           "The MsgType is %2XH", _ProData.MsgType);
    /*begin:yangfei added 2013-03-12 for ֡ת��needed deleted*/
#if  0
    if(_ProData.MsgType == 0x82 || _ProData.MsgType == 0x83 || _ProData.MsgType == 0x84 || _ProData.MsgType == 0x85)
    {
        uint8 PreSmybol[2] = {0xFB, 0xFB};
        if(CmpMeterNo(_ProData.DestAddr, gPARA_TermPara.HostAddr) == 0 && CmpMeterNo(_ProData.SourceAddr, gPARA_TermPara.TermAddr) == 0) /*ת����������������*/
        {
            uint16 BufNum = 0xaa55; /*send PreSmybo 0xFB0xFB*/
            debug_debug(gDebugModule[TASKUP_MODULE], "GPRS retransmit\r\n");
            GprsIPSEND_xmz(_receBuf, _ProData.MsgLength + 28, &BufNum);
            return 7;
        }
        if(CmpMeterNo(_ProData.DestAddr, gPARA_TermPara.TermAddr) == 0 && CmpMeterNo(_ProData.SourceAddr, gPARA_TermPara.HostAddr) == 0) /*ת����������������*/
        {
            debug_debug(gDebugModule[TASKUP_MODULE], "ZIGBEE retransmit\r\n");
            UpDevSend(UP_COMMU_DEV_ZIGBEE,  PreSmybol, 2);
            UpDevSend(UP_COMMU_DEV_ZIGBEE,  _receBuf,  _ProData.MsgLength + 28);
            return 8;
        }
    }
#endif
    /*end:yangfei added 2013-03-12 for ֡ת��*/
    err = ProcessFrame(_ProData);
    if(err == NO_ERR)
    {
        CreateFrame((uint8 *)&_ProData, _receBuf, lenBuf);
        /*begin:yangfei added 2013-02-18 */
        if(gDebugModule[TASKUP_MODULE] >= KERN_DEBUG)
        {
            PUBLIC_HexStreamToString(_receBuf, lenBuf, gPrintData);
            debug_debug(gDebugModule[TASKUP_MODULE], "send data:%s\r\n", gPrintData);
        }
        /*end:yangfei added 2013-02-18*/
    }
    return err;
}

/********************************************************************************************************
**  �� ��  �� ��: CreateHeartFrm
**	�� ��  �� ��: ��������֡
**	�� ��  �� ��: uint8* _receBuf
**                uint16 &lenFrame
**	�� ��  �� ��: none
**  ��   ��   ֵ: none
**	��		  ע:
*********************************************************************************************************/
uint8 _Protocol::CreateHeartFrm(uint8 *HeartFrm, uint8 flag)
{
    uint16 len = 0x00;
    _ProtocolType _ProData;
    CPU_SR	cpu_sr;

    _ProData.Data.pDataBack = gPRO_SendData;
    _ProData._Ver = PROTOCOL_VER;				//Э��汾��

    OS_ENTER_CRITICAL();
    memcpy(_ProData.SourceAddr, gPARA_TermPara.HostAddr, 6);	//Դ��ַ
    memcpy(_ProData.DestAddr, gPARA_TermPara.TermAddr, 6);		//Ŀ���ַ
    OS_EXIT_CRITICAL();

    if(flag == 0x01) /*GPRS��½֡*/
    {
        _ProData.MsgLength = 6;						//��Ϣ�峤��
        _ProData.MsgType = 0x2C;					//��Ϣ����
        memcpy(_ProData.Data.pDataBack, gPARA_TermPara.TermAddr, 6);
        _ProData.Data.lenDataBack 	= 6;
        /*begin:yangfei added 2013-02-20*/
        debug_debug(gDebugModule[GPRS_MODULE], "GPRS log in frame !\r\n");
        /*end:yangfei added 2013-02-20*/
    }
    if(flag == 0x03)  /*GPRS ����֡*/
    {
        _ProData.MsgLength = 1;						//��Ϣ�峤��
        _ProData.MsgType = 0x2E;					//��Ϣ����
        _ProData.Data.pDataBack[0] = 0x01;
        _ProData.Data.lenDataBack 	= 1;
        /*begin:yangfei added 2013-02-20*/
        debug_debug(gDebugModule[GPRS_MODULE], "GPRS heart frame !\r\n");
        /*end:yangfei added 2013-02-20*/
    }
    /*begin:yangfei added 2013-08-03 for add debug message */
    if(flag == 0x10)  /*������Ϣ֡�����͸���λ��*/
    {
        uint16 len = gDebugLength;
        _ProData.MsgLength = len;						//��Ϣ�峤��
        _ProData.MsgType = 0x10;					//��Ϣ����
        //_ProData.Data.pDataBack[0] = 0x01;
        memcpy(_ProData.Data.pDataBack, gDebugBuffer, _ProData.MsgLength);
        _ProData.Data.lenDataBack 	= len;
        /*begin:yangfei added 2013-02-20*/
        debug_debug(gDebugModule[GPRS_MODULE], "GPRS debug message  frame ! len=%d \r\n", _ProData.MsgLength);
        /*end:yangfei added 2013-02-20*/
    }
    /*end:yangfei added 2013-08-03 for add debug message */
    ReadDateTime(_ProData.TimeSmybol);			//ʱ���ǩ

    CreateFrame((uint8 *)&_ProData, HeartFrm, len);

    return len;
}
/********************************************************************************************************
**  �� ��  �� ��: ProcessFrame
**	�� ��  �� ��: ����֡
**	�� ��  �� ��:
**	�� ��  �� ��: none
**  ��   ��   ֵ: none
**	��		  ע: ��֡������֡����Ŀ�ʼ���øú���
*********************************************************************************************************/
uint8 _Protocol::ProcessFrame(_ProtocolType &_ProData)
{
    uint8 err = 0;
    if(_ProData.MsgType != 0x11)			//���ݴ洢�������ͣ�����0x11���ܴ洢
    {
        memcpy((uint8 *)&MultiBackup, (uint8 *)&_ProData, sizeof(_ProtocolType));
        //�˴������򱸷���Ҫע�⣬ProData.Data.lenData���ܴ���100.
        memcpy(DataFiledBack, _ProData.Data.pData, _ProData.Data.lenData);
        memset(ControlSave, 0x00, sizeof(ControlSave));
    }

    switch(_ProData.MsgType)
    {
    case 0x02:
        PARA_ProcessMsg_02(_ProData);
        break;
    case 0x04:
        test_proMsg_04(_ProData);
        break;
    case 0x06:
        DEAL_ProcessMsg_06(_ProData);  //���ݼ����㵥����
        break;
    case 0x0C:
        PARA_ProcessMsg_0C(_ProData);
        break;
    case 0x13:
        PARA_ProcessMsg_13(_ProData); //�޸ĵ������ַ��Ϣ��
        break;

    case 0x0E:
        DEAL_ProcessMsg_0E(_ProData, ControlSave);
        break;
    case 0x11:								//��֡����£��ſ����������´������һ������
        /*begin:2013102501 yangfei modified 2013-10-25 for ���һ����ʧʱ����λ��������ʱ�������ProcessMsg_11����ΪControlSave[0] =0*/
        ProcessMsg_11(_ProData, ControlSave);
#if 0
        if(ControlSave[0] == 1)
        {
            ProcessMsg_11(_ProData, ControlSave);
        }
        else
        {
            debug("%s ControlSave[0]=%d \r\n", __FUNCTION__, ControlSave[0] );
        }
#endif
        /*end:yangfei modified 2013-10-25 for ���һ����ʧʱ����λ��������ʱ�������ProcessMsg_11����ΪControlSave[0] =0*/
        break;
    case 0x18:
        DEAL_ProcessMsg_18(_ProData);
        break;
    case 0x1A:
        DEAL_ProcessMsg_1A(_ProData, ControlSave);
        break;
    case 0x1C:
        DEAL_ProcessMsg_1C(_ProData);
        break;
    case 0x1E:
        DEAL_ProcessMsg_1E(_ProData, ControlSave);
        break;
    case 0x20:
        PARA_ProcessMsg_20(_ProData);
        break;
    case 0x22:
        DEAL_ProcessMsg_22(_ProData, ControlSave);
        break;
    case 0x24:
        PARA_ProcessMsg_24(_ProData);
        break;
    case 0x26:
        PARA_ProcessMsg_26(_ProData);
        break;
    case 0x34:
        PARA_ProcessMsg_34(_ProData);  //ָ������������
        break;
    case 0x3A:
        PARA_ProcessMsg_3A(_ProData);  //����ָ�����ò�������ز�����
        break;
    case 0x3C:
        DEAL_ProcessMsg_3C(_ProData);  //��ָ��ʱ��ָ����ŵ���ʷ���ݡ�
        break;
    case 0x3E:
        DEAL_ProcessMsg_3E(_ProData);  //���ü�����塰�"��������"
        break;

    case 0x40:
        DEAL_ProcessMsg_40(_ProData, ControlSave);
        break;
    case 0x42:
        DEAL_ProcessMsg_42(_ProData);  //����͸��
        break;
#ifdef PARA_READ_ALLOW
    case 0x50:				/*��ȡ�ն˻�����Ϣ*/
        PARA_ProcessMsg_50(_ProData);
        break;
    case 0x51:				/*��ȡ�ն˱�����Ϣ*/
        err = PARA_ProcessMsg_51(_ProData); //Todo:need to be modified
        //PARA_ProcessMsg_83(_ProData);
        break;
    case 0x52:				/*��ȡ����ͳ����Ϣ*/
        PARA_ProcessMsg_52(_ProData);
        break;
    case 0x53:				/*��ȡ�ն˳���ʱ���*/
        PARA_ProcessMsg_53(_ProData);
        break;
    case 0x54:				/*��ȡ��վIP�Ͷ˿�ͨѶ����*/
        PARA_ProcessMsg_54(_ProData);
        break;
    case 0x55:				/*��ȡ�ն�IP�Ͷ˿ڲ���*/
        err = PARA_ProcessMsg_55(_ProData); //Todo:need to be modified
        //PARA_ProcessMsg_83(_ProData);
        break;
    case 0x56:				/*��ȡ�����ñ�����Ϣ*/
        PARA_ProcessMsg_56(_ProData);
        break;
#endif

    /*begin:yangfei added 2013-01-16 for update*/
    case 0x82:              /*��վ����������ʼ��־*/
        PARA_ProcessMsg_82(_ProData);
        break;
    case 0x83:				/*��վ������������*/
        err = PARA_ProcessMsg_83(_ProData); //Todo:need to be modified
        //PARA_ProcessMsg_83(_ProData);
        break;
    case 0x84:				/*��վ�·�Զ������������־*/
        PARA_ProcessMsg_84(_ProData);
        break;
    case 0x85:				/*��վѯ���ļ�ȱ�����*/
        PARA_ProcessMsg_85(_ProData);
        break;
    /*end:yangfei added 2013-01-16 for update*/

    case 0x90:
        PARA_ProcessMsg_90(_ProData);  //��������IP/�˿ں�/��Ӳ���汾����Ϣ��
        break;

    case 0x92:
        DEAL_ProcessMsg_92(_ProData, ControlSave);  //���������еı������Ϣ��
        break;

    case 0x94:
        PARA_ProcessMsg_94(_ProData);  //ָ�����IP���˿ںš�
        break;

    default:
        err = ERR_1;
        debug_debug(gDebugModule[TASKUP_MODULE], "Message Type err!\r\n");
        break;
    }
    return err;
}

/********************************************************************************************************
**  �� ��  �� ��: ProcessMsg_11
**	�� ��  �� ��: ��Ϣ����
**	�� ��  �� ��:
**	�� ��  �� ��: none
**  ��   ��   ֵ: none
**	��		  ע: ������ֻ�������ж�֡��������ʱ�Ż����
*********************************************************************************************************/
uint8 _Protocol::ProcessMsg_11(_ProtocolType &_ProData, uint8 *pControlSave)
{
    uint8 MultiControlData;

    LOG_assert_param( pControlSave == NULL );

    MultiControlData = *(_ProData.Data.pData);			//ȡ��Ҫ���͵ڼ���

    memcpy((uint8 *)&_ProData, (uint8 *)&MultiBackup, sizeof(_ProtocolType));
    memcpy(_ProData.Data.pData, DataFiledBack, _ProData.Data.lenData);

    switch(_ProData.MsgType)
    {
    case 0x0E:
        MultiFrameMsg_0E *pMultiPack_0E;
        pMultiPack_0E = (MultiFrameMsg_0E *)pControlSave;

        if(MultiControlData == pMultiPack_0E->SendPackIndex)
        {
            //�����ط���һ��
            DEAL_ProcessMsg_0E(_ProData, pControlSave);
        }
        if(MultiControlData == (pMultiPack_0E->SendPackIndex + 1))
        {
            //������һ������
            pMultiPack_0E->StartMeterSn += pMultiPack_0E->SendMeterNums;
            pMultiPack_0E->SendPackIndex++;
            DEAL_ProcessMsg_0E(_ProData, pControlSave);
        }
        //�������������ݣ�������Ӧ
        break;
    case 0x18:
        DEAL_ProcessMsg_18(_ProData);
        break;
    case 0x1A:
        DEAL_ProcessMsg_1A(_ProData, ControlSave);
        break;
    case 0x1C:
        DEAL_ProcessMsg_1C(_ProData);
        break;
    case 0x1E:
        MultiFrameMsg_1E *pMultiPack_1E;
        pMultiPack_1E = (MultiFrameMsg_1E *)pControlSave;

        if(MultiControlData == pMultiPack_1E->SendPackIndex)
        {
            //�����ط���һ��
            DEAL_ProcessMsg_1E(_ProData, pControlSave);
        }
        if(MultiControlData == (pMultiPack_1E->SendPackIndex + 1))
        {
            //������һ������
            pMultiPack_1E->StartReadAddr += pMultiPack_1E->SendByteNums;
            pMultiPack_1E->SendPackIndex++;
            DEAL_ProcessMsg_1E(_ProData, pControlSave);
        }
        break;

    case 0x22:
    {
        MultiFrameMsg_22 *pMultiPack_22;
        pMultiPack_22 = (MultiFrameMsg_22 *)pControlSave;

        if(MultiControlData == pMultiPack_22->SendPackIndex)
        {
            //�����ط���һ��
            DEAL_ProcessMsg_22(_ProData, pControlSave);
        }
        if(MultiControlData == (pMultiPack_22->SendPackIndex + 1))
        {
            //������һ������
            pMultiPack_22->StartMeterSn += pMultiPack_22->SendMeterNums;
            pMultiPack_22->SendPackIndex++;
            DEAL_ProcessMsg_22(_ProData, pControlSave);
        }
        break;
    }
    case 0x40:
    {
        MultiFrameMsg_40 *pMultiPack_40;
        pMultiPack_40 = (MultiFrameMsg_40 *)pControlSave;

        if(MultiControlData == pMultiPack_40->SendPackIndex)
        {
            //�����ط���һ��
            DEAL_ProcessMsg_40(_ProData, pControlSave);
        }
        if(MultiControlData == (pMultiPack_40->SendPackIndex + 1))
        {
            //������һ������
            pMultiPack_40->StartMeterSn += pMultiPack_40->SendMeterNums;
            pMultiPack_40->SendPackIndex++;
            DEAL_ProcessMsg_40(_ProData, pControlSave);
        }
        break;
    }

    case 0x92:  //�ϴ����ַ��Ϣ��
    {
        MultiFrameMsg_92 *pMultiPack_92;
        pMultiPack_92 = (MultiFrameMsg_92 *)pControlSave;

        if(MultiControlData == pMultiPack_92->SendPackIndex)
        {
            //�����ط���һ��
            DEAL_ProcessMsg_92(_ProData, pControlSave);
        }
        if(MultiControlData == (pMultiPack_92->SendPackIndex + 1))
        {
            //������һ������
            pMultiPack_92->StartMeterSn += pMultiPack_92->SendMeterNums;
            pMultiPack_92->SendPackIndex++;
            DEAL_ProcessMsg_92(_ProData, pControlSave);
        }
        break;
    }

    default:
    {
        debug("ERROR:%s  _ProData.MsgType=%d multi frame transfer insert other frame\r\n", __FUNCTION__, _ProData.MsgType);
    }
    break;
    }
    return 0;
}


/********************************************************************************************************
**  �� ��  �� ��: CountCheck
**	�� ��  �� ��: ͳ�Ƽ��
**	�� ��  �� ��: uint8 *_data
**                uint16 _len
**	�� ��  �� ��: none
**  ��   ��   ֵ: none
**	��		  ע: ������ֻ�������ж�֡��������ʱ�Ż����
*********************************************************************************************************/
uint8 _Protocol::CountCheck(uint8 *_data, uint16 _len)
{
    uint8 cs = 0;
    uint16 i;

    LOG_assert_param( _data == NULL );

    for(i = 0; i < _len; i++)
    {
        cs += *_data++;
    }

    cs = (~cs) + 1;

    return cs;
}


/*****************************************************************************************
**								End of File
*****************************************************************************************/
