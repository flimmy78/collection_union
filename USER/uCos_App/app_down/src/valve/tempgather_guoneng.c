/*
  ********************************************************************************************************
  * @file    tempgather_guoneng.c
  * @author  zjjin
  * @version V0.0.0
  * @date    09-17-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		���������ԿؿƼ�"�����¶Ȳɼ�װ��"�á�
  		���豸ֻ��Ҫ�ɼ��û������¶ȼ��ɡ�
  *
  ********************************************************************************************************
  */

//#include <includes.h>

#include "Valve.h"
#include "tempgather_guoneng.h"



/*
  ********************************************************************************************************
  * ��������: uint8 ValveContron_Elsonic(MeterFileType *p_mf,uint8 functype,uint8 *p_DataIn,uint8 *p_databuf,uint8 *p_datalenback)

  * ˵    �������� ���������ƺ������ڴ˺�����ʵ�����ַ��ز�ͬ���ơ�
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

uint8 TempGather_GuoNeng(MeterFileType *p_mf, uint8 functype, uint8 *p_DataIn, uint8 *p_databuf, uint8 *p_datalenback)
{
    uint8 Err = 0;
    uint8 lu8dataframe[100] = {0};
    uint8 lu8datalen = 0;
    uint8 lu8databuf[20] = {0xee};
    uint8 lu8datalenback = 0;
    uint8 i = 0;
    uint8 lu8tmp = 0;



    switch(functype)
    {
    case ReadVALVE_All:
    {
        Create_GuoNengTemp_ReadFrame(p_mf, lu8dataframe, &lu8datalen); //�齨��ȡ�¶Ȳɼ�����������֡
        Err = GuonengTemp_ReadData_Communicate(lu8dataframe, &lu8datalen);

        if(Err == NO_ERR)
        {
            lu8databuf[i++] = lu8dataframe[14] & 0x0f;
            lu8datalenback++;
            lu8tmp = lu8dataframe[14] >> 4;
            lu8tmp += lu8dataframe[15] << 4;
            lu8databuf[i++] = lu8tmp;
            lu8datalenback++;
            lu8databuf[i++] = 0x00;  //����λ
            lu8datalenback++;

            lu8databuf[i++] = 0x00;   //������д0.
            lu8datalenback++;
            lu8databuf[i++] = lu8dataframe[16];   //�������,���ܲɼ�������ȷʾ�⡣
            lu8datalenback++;
            lu8databuf[i++] = 0x00;
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



    default:
        break;
    }



    return Err;
}



/*
 ******************************************************************************
 * �������ƣ�Create_ElsonicVave_SetInfo_Frame(DELU_Protocol *pData, uint8 *pSendFrame, uint8 *plenFrame)
 * ˵	 �����˺������ڶ�ȡ���ַ�����״̬��Ϣ��
 * ��	 ����
 ******************************************************************************
 */
void Create_GuoNengTemp_ReadFrame(MeterFileType *p_mf, uint8 *pSendFrame, uint8 *plenFrame)
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



uint8 GuonengTemp_ReadData_Communicate(uint8 *DataFrame, uint8 *DataLen)
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


