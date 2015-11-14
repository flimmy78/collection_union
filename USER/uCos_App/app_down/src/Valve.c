/*
  ********************************************************************************************************
  * @file    Valve.c
  * @author  zjjin
  * @version V0.0.0
  * @date    04-15-2015
  * @brief
  ********************************************************************************************************
  * @attention
  *		Ϊ�ڼ�������ʵ�ֶ೧�Ҳ�ͬЭ�鷧�ŵļ��ݣ��ڱ������Ϣ�м�����
  *	����Э��汾�������ҷ�������س����ڱ��ļ��С�
  *
  ********************************************************************************************************
  */
//#include <includes.h>

#include "Valve.h"
#include "valve_elsonic.h"
#include "valve_binihi.h"
#include "valve_delu.h"
#include "valve_joyo.h"
#include "valve_lcarbo.h"
#include "valve_precise.h"
#include "valve_ADE.h"
#include "tempgather_guoneng.h"
#include "panelwired_elsonic.h"
#include "valve_guoneng.h"






/*
**************************************************************************************************
uint8 gVALVE_Table[][2],��������Э��֧�֣�
						gVALVE_Table[][0] --- ���ش��ڲ������ã�
						gVALVE_Table[][1] --- ������Э��汾�š�

**************************************************************************************************
*/
uint8 gVALVE_Table[][2] =
{
    COMSET_1, VALVE_DELU,			//0,��³����(COMSET_x����)��Э��汾Ϊ0.
    COMSET_5, VALVE_ELSONIC,		//1,���ַ��أ�Э��汾Ϊ1.
    COMSET_5, VALVE_JOYO,			//2,��Դ����,Э��汾Ϊ2.
    COMSET_7, VALVE_BINIHI,			//3,��ů�㷧�أ�Э��汾Ϊ3.
    COMSET_1, VALVE_LCARBO,			//4,�ſ�������(COMSET_x����)��Э��汾Ϊ4��
    COMSET_1, VALVE_PRECISE,    	//5����������ͨѶ��Э��汾Ϊ5.
    COMSET_1, VALVE_ADE,			//6,�����³���з�����Э��汾Ϊ6.
    COMSET_1, TEMPGATHER_GUONENG,	//7,�������ܻ����¶Ȳɼ�װ��Э��汾7.
    COMSET_5, PANELWIRED_ELSONIC,	//8,���������¿�����׼Э�飬Э��汾Ϊ8.
    COMSET_1, VALVE_GUONENG,		//9,���������¿ط�Э��汾9.
};




/*
  ********************************************************************************************************
  * �������ƣ�uint8 ValveContron(MeterFileType *p_mf,uint8 functype,uint8 *p_databuf,uint8 p_datalenback)
  * ˵    ���� ���������ƺ������ڴ˺�����ʵ�ֲ�ͬЭ�鷧�صļ��ݣ�
  *					ͬʱ�ڴ˺�����ʵ����Է��Ų�ͬ���ܵĿ��ơ�
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

uint8 ValveContron(MeterFileType *p_mf, uint8 functype, uint8 *p_DataIn, uint8 *p_databuf, uint8 *p_datalenback)
{
    uint8 Err = 0;
    uint8 i = 0;
    uint8 lu8ReadTimes = 0;
    uint16 lu16Second = 0;
    uint16 lu16ms = 0;

    lu8ReadTimes = gPARA_ReplenishPara.ValveReplenishTimes + 1;  //����������
    lu16Second = gPARA_ReplenishPara.ValveInterval / 1000;
    lu16ms = gPARA_ReplenishPara.ValveInterval % 1000;

    switch(gVALVE_Table[p_mf->ValveProtocal][1])     //���ݷ���Э��ţ�ִ����Ӧ����Э�顣
    {

    case VALVE_DELU:
    {
        (*METER_ComParaSetArray[gVALVE_Table[p_mf->ValveProtocal][0]])();  //���õ�³���Ŷ�Ӧ���ڲ�����
        OSTimeDlyHMSM(0, 0, 0, 200);
        if(lu8ReadTimes > 2)  //��Ϊ��³Э��ÿ�������ظ�3�Σ��޶�Ϊ2�����6�Ρ�
            lu8ReadTimes = 2;

        for(i = 0; i < lu8ReadTimes; i++)
        {
            Err = ValveContron_Delu(p_mf, functype, p_DataIn, p_databuf, p_datalenback);
            if(Err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, lu16Second, lu16ms);
            }
        }

        break;
    }

    case VALVE_ELSONIC:
    {
        (*METER_ComParaSetArray[gVALVE_Table[p_mf->ValveProtocal][0]])();  //�������ַ��Ŷ�Ӧ���ڲ�����
        OSTimeDlyHMSM(0, 0, 0, 100);
        for(i = 0; i < lu8ReadTimes; i++)
        {
            Err = ValveContron_Elsonic(p_mf, functype, p_DataIn, p_databuf, p_datalenback);
            if(Err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, lu16Second, lu16ms);
            }
        }

        break;
    }

    case VALVE_JOYO:
    {
        (*METER_ComParaSetArray[gVALVE_Table[p_mf->ValveProtocal][0]])();  //���þ�Դ���Ŷ�Ӧ���ڲ�����
        OSTimeDlyHMSM(0, 0, 0, 100);
        for(i = 0; i < lu8ReadTimes; i++)
        {
            Err = ValveContron_Joyo(p_mf, functype, p_DataIn, p_databuf, p_datalenback);
            if(Err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, lu16Second, lu16ms);
            }
        }

        break;
    }

    case VALVE_BINIHI:
    {
        (*METER_ComParaSetArray[gVALVE_Table[p_mf->ValveProtocal][0]])();  //���ð�ů�㷧�Ŷ�Ӧ���ڲ�����
        OSTimeDlyHMSM(0, 0, 0, 100);
        for(i = 0; i < lu8ReadTimes; i++)
        {
            Err = ValveContron_Binihi(p_mf, functype, p_DataIn, p_databuf, p_datalenback);
            if(Err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, lu16Second, lu16ms);
            }
        }


        break;
    }

    case VALVE_LCARBO:
    {

        break;
    }

    case VALVE_PRECISE:
    {
        (*METER_ComParaSetArray[gVALVE_Table[p_mf->ValveProtocal][0]])();  //��������ͨѶ���Ŷ�Ӧ���ڲ�����
        OSTimeDlyHMSM(0, 0, 0, 100);
        if(lu8ReadTimes > 2)  //��ΪЭ��ÿ�������ظ�3�Σ��޶�Ϊ2�����6�Ρ�
            lu8ReadTimes = 2;

        for(i = 0; i < lu8ReadTimes; i++)
        {
            Err = ValveContron_Precise(p_mf, functype, p_DataIn, p_databuf, p_datalenback);
            if(Err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, lu16Second, lu16ms);
            }
        }

        break;
    }

    case VALVE_ADE:
    {
        (*METER_ComParaSetArray[gVALVE_Table[p_mf->ValveProtocal][0]])();  //���ô��ڲ�����
        OSTimeDlyHMSM(0, 0, 0, 100);
        if(lu8ReadTimes > 2)  //��ΪЭ��ÿ�������ظ�3�Σ��޶�Ϊ2�����6�Ρ�
            lu8ReadTimes = 2;

        for(i = 0; i < lu8ReadTimes; i++)
        {
            Err = ValveContron_ADE(p_mf, functype, p_DataIn, p_databuf, p_datalenback);
            if(Err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, lu16Second, lu16ms);
            }
        }

        break;

    }

    case TEMPGATHER_GUONENG:
    {
        (*METER_ComParaSetArray[gVALVE_Table[p_mf->ValveProtocal][0]])();  //���ô��ڲ�����
        OSTimeDlyHMSM(0, 0, 0, 100);

        for(i = 0; i < lu8ReadTimes; i++)
        {
            Err = TempGather_GuoNeng(p_mf, functype, p_DataIn, p_databuf, p_datalenback);
            if(Err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, lu16Second, lu16ms);
            }
        }

        break;

    }

    case PANELWIRED_ELSONIC:
    {
        (*METER_ComParaSetArray[gVALVE_Table[p_mf->ValveProtocal][0]])();  //���ô��ڲ�����
        OSTimeDlyHMSM(0, 0, 0, 100);

        for(i = 0; i < lu8ReadTimes; i++)
        {
            Err = PanelWiredControl_Elsonic(p_mf, functype, p_DataIn, p_databuf, p_datalenback);
            if(Err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, lu16Second, lu16ms);
            }
        }

        break;
    }

    case VALVE_GUONENG:
    {
        (*METER_ComParaSetArray[gVALVE_Table[p_mf->ValveProtocal][0]])();  //���ô��ڲ�����
        OSTimeDlyHMSM(0, 0, 0, 100);

        for(i = 0; i < lu8ReadTimes; i++)
        {
            Err = ValveContron_GuoNeng(p_mf, functype, p_DataIn, p_databuf, p_datalenback);
            if(Err == NO_ERR)
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0, 0, lu16Second, lu16ms);
            }
        }


        break;
    }

    default:
        Err = ERR_1;
        break;

    }




    return Err;
}

