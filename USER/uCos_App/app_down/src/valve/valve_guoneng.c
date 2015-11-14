/*
  ********************************************************************************************************
  * @file    valve_guoneng.c
  * @author  zjjin
  * @version V0.0.0
  * @date    10-15-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		���������ԿؿƼ�"�����¿ط�"�á�
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
  * ��������: uint8 ValveContron_GuoNeng(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)

  * ˵    �����������ܷ��������ƺ������ڴ˺�����ʵ���������ܷ��ز�ͬ���ơ�
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
        Create_GuoNengVave_ReadInfo_Frame(p_mf, lu8dataframe, &lu8datalen); //�齨��ȡ������������֡
        Err = GuonengValve_ReadData_Communicate(lu8dataframe, &lu8datalen);

        if(Err == NO_ERR)
        {
            lu8databuf[i++] = lu8dataframe[22] & 0x0f;
            lu8datalenback++;
            lu8tmp = lu8dataframe[22] >> 4;
            lu8tmp += lu8dataframe[23] << 4;
            lu8databuf[i++] = lu8tmp;
            lu8datalenback++;
            lu8databuf[i++] = 0x00;  //����λ
            lu8datalenback++;

            lu8tmp = BCD_TO_HEX(lu8dataframe[25]);  //���ܷ������ǰٷֱȣ�BCD��2�ֽڡ�
            lu8tmp = lu8tmp * 100;
            lu8tmp += BCD_TO_HEX(lu8dataframe[24]);
            if(lu8tmp > 90)
                lu8databuf[i++] = 0x55;  // ȫ��
            else if(lu8tmp > 75)
                lu8databuf[i++] = 0x66;  // 3/4����
            else if(lu8tmp > 40)
                lu8databuf[i++] = 0x77;  // 2/4��
            else if(lu8tmp > 20)
                lu8databuf[i++] = 0x88;  // 1/4
            else
                lu8databuf[i++] = 0x99;//ȫ�ء�

            lu8datalenback++;


            //begin:����״̬λ�ֽڴ���
            lu8databuf[4] = 0; //�Ƚ�״̬�ֽڳ�ʼ��Ϊ0 ��
            lu8tmp = lu8dataframe[29];
            if(lu8tmp & 0x04)  //�����쳣��
                lu8databuf[4] |= 0x01;

            //lu8databuf[4] |= 0x02;  //���ܷ���û����忪�ر�־�����Բ����жϡ�

            lu8tmp = lu8dataframe[20];
            if(lu8tmp & 0x02)  //��������Ƿ�
                lu8databuf[4] |= 0x04;

            //if(lu8tmp & 0x02)  //�����Ƿ�������
            //	lu8databuf[4] |= 0x08;
            //if(lu8tmp & 0x80)  //�����Ƿ�������
            //	lu8databuf[4] |= 0x08;
            //end:����״̬λ�ֽڴ���?

            i++;
            lu8datalenback++;
            lu8databuf[i++] = 0x00;  //Ԥ����
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


    case SETHEAT_VALUE:   //����������ʾ���ܲ��ӡ�
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

    case SETTEMP_RANGE:   //���ܲ����趨�¶ȷ�Χ��ֻ�������35�棬�����������35���϶ȡ�
    {


        break;
    }

    case SETVALVE_STATUS:   //���������¿�������ǿ�ƿ������ڷ��ŷ�ǿ�ƹر�ʱ�����������ء�
    {
        //������û��дǿ�ƹط������Դ˴�Ϊ�ա�
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
 * �������ƣ�Create_GuoNengVave_ReadInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * ˵	 �����˺������ڶ�ȡ���ܷ�����״̬��Ϣ��
 * ��	 ����
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
 * �������ƣ�Create_GuoNengVave_SetHeat_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * ˵	 �����˺������ڽ�����ֵд�뷧������
 * ��	 ����
 ******************************************************************************
 */
void Create_GuoNengVave_SetHeat_Frame(MeterFileType *p_mf, uint8 *p_DataIn, uint8 *pSendFrame, uint8 *plenFrame)
{



}



/*
 ******************************************************************************
 * �������ƣ�Create_ElsonicVave_SetRoomTemp_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * ˵	 �����˺����������������¶ȡ�
 * ��	 ����
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

    DuQueueFlush(dev);				 //��ջ�����
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


