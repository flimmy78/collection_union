#ifndef			_HZTABLE_H
#define			_HZTABLE_H

extern uint8 LineHZ1[] ; //������̩�ƴ�Ƽ���
extern uint8 LineChar1[] ;//1 //������̩�ƴ��

//�������򣺵�һ�����ֱ�ʾ��ǰ�˵��ļ��𣬵�2�����ֱ�ʾ��ǰ�˵��µĵڼ���»��ߺ�������ֱ�ʾ��һ���˵��ļ��𣬵�2�����ֱ�ʾ��ǰ�˵�
//�µĵڼ����������

//һ���˵�
extern uint16 MenuHZ11[] ;//������������ʾ
extern uint16 MenuHZ12[] ;//����������鿴
extern uint16 MenuHZ13[] ;//������������ά��
extern uint16 MenuHZ14[] ;//ʱ��ͨ�������
extern uint16 MenuHZ15[] ;//ѡ���������
extern uint16 MenuHZ16[] ;//ά�� ʵ��Ӧ��д�ɳ�������muxiaoqing 20140328


//�����˵�
//��ʱȱ��1���˵��µ�һ��Ķ����˵�
extern uint16 TestPoint1[];//������1
extern uint16 TestPoint2[];//������2
extern uint16 TestPoint3[];//������3
extern uint16 TestPoint4[];//������4
extern uint16 TestPoint5[];//������5
extern uint16 TestPoint6[];//������6
extern uint16 TestPoint7[];//������7
extern uint16 TestPoint8[];//������8

extern uint16 MenuHZ12_21[];//1.ͨ��ͨ������
extern uint16 MenuHZ12_22[];//��������
extern uint16 MenuHZ11_22[];//2.̨������������
extern uint16 MenuHZ12_23[];//3.��������������
extern uint16 MenuHZ12_24[];  //ʧ����Ϣ��ѯ

extern uint16 MenuHZ11_24[] ;//4.�ն�ʱ������
extern uint16 MenuHZ12_25[] ;//5.������������
extern uint16 MenuHZ11_26[] ;//6.�ն˱������
extern uint16 MenuHZ11_27[] ;//7.����ʱ�������
extern uint16 MenuHZ11_28[] ;//GPRS��������
extern uint16 MenuHZ11_29[] ;//IP���˿ں�����


extern uint16 MenuHZ_TimeNum[] ;//��  ����ʱ���:
//extern uint16 MenuHZ12_27[] ;//��������
extern uint16 MenuHZ12_28[] ;//8.����������
extern uint16 MenuHZ12_29[] ;//9.������������
extern uint16 MenuHZ12_2A[];//10.Զ��ͨ�ŷ�ʽ����
extern uint16 MenuHZ12_2B[];//11.����������鿴
extern uint16 PswStar[] ;//******

extern uint16 MenuHZ13_21[];//�����ն�
extern uint16 MenuHZ13_22[] ;//���ݳ�ʼ��
extern uint16 MenuHZ13_23[] ;//������ʼ��
extern uint16 MenuHZ13_24[] ;//4.���Բ�������
extern uint16 MenuHZ13_25[] ;//5.�ն˰汾��Ϣ
extern uint16 MenuHZ13_26[] ;//6.Һ���Աȶȵ���
extern uint16 MenuHZ13_27[] ;//7.������������

extern uint16 MenuHZ15_1[];//������
extern uint16 MenuHZ15_3[];//�ȷ����
extern uint16 MenuHZ15_4[];//��ǰ��������
extern uint16 MenuHZ16_21[] ;//�ֶ�����muxiaoqing 20140328
extern uint16 MenuHZ16_22[] ;//�Զ�����muxiaoqing 20140328

#if 0
extern uint16 MenuHZ16_21[] ;//"SD ������д��Ͷ����ԱȲ���"
extern uint16 MenuHZ16_22[] ;//"ʱ��оƬ��д��Ͷ����ԱȲ���"
extern uint16 MenuHZ16_23[] ;//"GPRS ָ�����"
extern uint16 MenuHZ16_24[] ;//"���Դ����������"
extern uint16 MenuHZ16_25[] ;//"������1�������"
extern uint16 MenuHZ16_26[] ;//"GPRSͨѶ�������"
extern uint16 MenuHZ16_27[] ;//"RS485 ͨѶ1�������"
extern uint16 MenuHZ16_28[] ;//"RS485 ͨѶ2�������"
#else
#ifdef DEMO_APP
extern uint16 MenuHZ16_21_31[] ;//"��³"
extern uint16 MenuHZ16_21_32[] ;//"������"
extern uint16 MenuHZ16_21_33[] ;//"���"
extern uint16 MenuHZ16_21_34[] ;//"Ԥ��"
extern uint16 Meter_ST_OK_1[] ;//"��³����: �ɹ�"
extern uint16 Meter_ST_OK_2[];//"����������: �ɹ�"
extern uint16 Meter_ST_OK_3[];//"�����: �ɹ�"
extern uint16 Meter_ST_FAL_1[];//"��³����:ʧ��"
extern uint16 Meter_ST_FAL_2[];//"����������:ʧ��"
extern uint16 Meter_ST_FAL_3[];//"�����:ʧ��"
extern uint16 Meter_ST_WAIT[] ;//"���Ժ�..."
extern uint16 Meter_ST_AIM[] ;//"�����¼�����������ʾҳ��"
extern uint16 Meter_ST_AIM_CHeat[] ;//"��ǰ����:"
extern uint16 Meter_ST_AIM_Power[];//"�ȹ���:"
extern uint16 Meter_ST_AIM_Flow[];//"����:"
extern uint16 Meter_ST_AIM_CFlow[];//"�ۻ�����:"
extern uint16 Meter_ST_AIM_INwater[];//"��ˮ�¶�:"
extern uint16 Meter_ST_AIM_Owater[];//"��ˮ�¶�:"
extern uint16 Meter_ST_Building[] ;//"¥��:"
extern uint16 Meter_ST_Unit[];//"��Ԫ:"
extern uint16 Meter_ST_MeterNum[];//"���:"
extern uint16 Meter_ST_CH[];//"ͨ��:"
extern uint16 IPVer[]  ;//Э���

#else
extern uint16 MenuHZ16_21_31[] ;//"SD ������д��Ͷ����ԱȲ���"
extern uint16 MenuHZ16_21_32[] ;//"ʱ��оƬ��д��Ͷ����ԱȲ���"
extern uint16 MenuHZ16_21_33[] ;//"GPRS ָ�����"
extern uint16 MenuHZ16_21_34[] ;//"���Դ����������"
extern uint16 MenuHZ16_21_35[] ;//"������1�������"
extern uint16 MenuHZ16_21_36[] ;//"GPRSͨѶ�������"
extern uint16 MenuHZ16_21_37[] ;//"RS485 ͨѶ1�������"
extern uint16 MenuHZ16_21_38[] ;//"RS485 ͨѶ2�������"
#endif
#endif



//�����˵�

extern uint16 MenuHZ11_21_31[] ;//��ǰ����
extern uint16 MenuHZ11_21_32[] ;//�����:
/*
extern uint16 MenuHZ12_21_31[] ;//1.��ǰͨ����������
extern uint16 MenuHZ12_21_32[] ;//2.��IP�Ͷ˿�����
extern uint16 MenuHZ12_21_33[] ;//3.��IP�Ͷ˿�����
extern uint16 MenuHZ12_21_34[] ;//4.APN����
extern uint16 MenuHZ12_21_35[] ;//5.Զ��ͨ�ŷ�ʽ����
*/
extern uint16 MenuHZ12_21_33[] ;
extern uint16 MenuHZ12_21_31[] ;//1.Զ��ͨ�ŷ�ʽ����
extern uint16 MenuHZ12_21_32[] ;//2.��ǰͨ����������
extern uint16 MenuHZ12_21_33[] ;//3.��������ѡ��
extern uint16 MenuHZ12_21_34[] ;//��ϸ����

extern uint16 MenuHZ12_22_31[] ;//������ţ�
extern uint16 MenuHZ12_22_32[] ;//����ַ��
extern uint16 MenuHZ12_22_33[] ;//������ͣ�
extern uint16 MenuHZ12_22_34[] ;//�ɼ����ţ�
extern uint16 MenuHZ12_22_35[] ;//ͨ�ŷ�ʽ��
extern uint16 MenuHZ12_22_36[] ;//�๦���ܱ�
extern uint16 MenuHZ12_25_31[] ;//�����룺
extern uint16 MenuHZ12_25_32[] ;//�����룺

extern uint16 MenuHZ12_27_31[] ;//1.����ز���ʱ����
extern uint16 MenuHZ12_27_32[] ;//2.��������鿴
extern uint16 MenuHZ12_27_33[] ;//3.����״̬�鿴
extern uint16 MenuHZ12_27_34[] ;//4.�ɼ�����������

extern uint16 MenuHz_DeviceNum[] ;//�豸���:
extern uint16 MenuHz_ProtocalType[] ;//��Լ:
extern uint16 MenuHz_BigTypeNo[] ;//�����:
extern uint16 Menu_BigTypeNo[][3];//�����ѡ�����ݣ�1--6
extern uint16 MenuHz_CommPort[] ;//ͨ�Ŷ˿ں�:
extern uint16 MenuHz_BaudRate[] ;//�����ʣ�
extern uint16 Menu_BaudRate[][5] ;//������ѡ������

extern uint16 MenuHZTesting[];//muxiaoqing test
extern uint16 MenuHZTestingPleaseWaite[];//muxiaoqing test

extern uint16 SdErr[];
extern uint16 ClockErr[];
extern uint16 ClockErrr[];
extern uint16 ClockErrrr[];
extern uint16 ClockErrrrr[];
extern uint16 ClockErrrrrr[];

extern uint16 chose_user_display[];//ѡ��鿴����
extern uint16 MenuHZ_user[];//��  �û�

extern uint16 MenuHZ_user_num[];//�û����
extern uint16 MenuHZ_device_addr[];//�豸��ַ
extern uint16 MenuHZ_user_area[];//�û����
extern uint16 MenuHZ_open_time[];//����ʱ��
extern uint16 MenuHZ_indoor_temp[];//�����¶�
extern uint16 MenuHZ_water_temp_in[];//��ˮ�¶�
extern uint16 MenuHZ_water_temp_out[];//��ˮ�¶�
extern uint16 MenuHZ_proportion_energy[];//��̯����
extern uint16 MenuHZ_total_energy[];//������

extern uint16 Sdtest[];//muxiaoqing test

//600
//1200
//2400
//4800
//7200
//9600
//19200
extern uint16 MenuHz_RateNum[] ;//������:
extern uint16 Menu_RateNum[][3] ;//������ѡ�����ݣ�1--4

extern const uint16 MeterStateInfo[][9];
extern const uint16 MeterState[][7];

extern uint16 InputPsw[] ;//���������룺
extern uint16 MenuHZ12_21_32[];//2.��ǰͨ����������
extern uint16 MenuHZ13_2_31[] ;//��ȷ�ϼ�����
extern uint16 MenuHZ13_2_32[] ;//��ȷ�ϼ���ʼ��
extern uint16 MenuHZ13_2_33[] ;//�����ؼ�ȡ��
extern uint16 IniSelect[] ;//��ʼ������ѡ��
extern uint16 Menu_IniSelect[][4] ;//��ʼ��ѡ�������,���� 1.��ʷ���� 2.����� 3.ȫ������

extern uint16 FirmRotateParaSet[] ;//��ȷ�ϼ�����
extern uint16 QuitRotateParaSet[] ;//�����ؼ��˳�

extern uint16 MenuHZ13_24_31[] ;//1.��ʾ��������
extern uint16 MenuHZ13_24_32[] ;//2.��ʾ��ʽ����
extern uint16 MenuHZ13_24_33[] ;//3.���Բ���������

extern uint16 MenuHZ13_27_31[] ;//1.����ز���ʱ����

//zengxl20101027���������Ϣ�鿴
//������:
//RS485��:
//�ز���:
extern uint16 Menu_MeterNum[][7];//��������鿴

extern uint16 InputTime[] ;//�������ѯʱ�䣺

//zengxl20101022�����㳭����������ѡ��ʵʱ���ݻ�����ʷ����
extern uint16 RealOrHisType[] ;//��ѯ����:
//ʵʱ����
//���ն������
extern uint16 Menu_RealOrHis[][6] ;//������������ѡ��


//zengxl20101020����ն˰汾��Ϣ
//�汾��Ϣ:
//����ʡ��:
//��������:
//������Ϣ��
extern const uint16 Menu_VerInfo[][5] ;//�ն˰汾��Ϣ
extern const uint16 Menu_VerInfo1[][8];//������Ϣ
extern const uint8 date[];             //shijian 

//zengxl20101012���ͨ��ͨ����������
//��վip����
//APN����
//������������
//��̫��IP����
extern const uint16 Menu_ConfigType[][10];//�������ͣ�������ʾ

//zengxl20100819����ն˱������ͼ���ֵ
extern uint16 AddrCodeType[];//�������ͣ�

//BCD��
//HEX��
extern const uint16 Menu_AddrCodeType[][4];//�������ͣ�������ʾ

//��ͨ485��
//�ز���
//���׶๦��
//�๦���ܱ�
//�м���
extern uint16 Menu_MeterType[][5];//������ͣ�������ʾ
extern uint16 Menu_ProtocalType[][12];//��Լ���ͣ�DL/T645-2007��DL/T645-1997����
extern uint16 Menu_CommType[][5];//ͨ�ŷ�ʽ��RS485 �� �ز� ����

//1.�ն˵�ַ
//2.�ն�ʱ��
//3.��̫������
//4.����ͨ�Ų���
//5.��վ����
//6.�ն˰汾
extern const uint16 Menu_TermParaShow[][8] ; //�ն˲����鿴 �˵� ������ʾ



//�ļ��˵�

//��������:
//�ն˱��:
//10����:
//16����:
extern uint16 Menu_TermAddrShow[][5] ; //�ն˵�ַ�鿴 ������ʾ

//ͨ������:
//ͨ�ŷ�ʽ:
// APN:
//��������:
extern uint16 Menu_UpCommuParaShow[][5] ; //����ͨ�Ų����鿴 ������ʾ

//�ƶ�����
//��ͨ����
//��̫��  
//����    
extern uint16 Menu_ChannelType[][6];//��ǰͨ�����ͣ�������ʾ

extern const uint16 Menu_RemoteCommType[][5];//Զ��ͨ�ŷ�ʽ��UDP��TCP����

extern uint16 MainIP[] ;//��IP:
extern uint16 SecondIP[] ;//��IP:
extern uint16 MainPort[] ;//��IP�˿�:
extern uint16 SecondPort[] ;//��IP�˿�:
extern uint16 Minute[] ;//����
extern uint16 EthernetIP[] ;//IP��ַ:(��̫��IP)
extern uint16 SubNetMask[] ;//��������:
extern uint16 DefaultGateway[] ;//Ĭ������:
extern uint16 MACAddr[] ;//MAC��ַ
extern uint16 AreaCode[] ;//����
extern uint16 BuildingCode[];//¥��
extern uint16 DeviceCode[];//�豸��

//����״̬��Ϣ,����
////�ѳ��������:
////δ���������:
////��󳭱�ʱ��:
////���ڳ������:
extern const uint16 MeterStateInfo[][9] ; //����״̬��Ϣ�������ѳ����δ�������󳭱�ʱ�䡢���ڳ������
extern uint16 SelectMeterType[] ;//ѡ�������ͣ�
extern uint16 Menu_MeterType_DataItem[][3] ;//��Ҫ���òɼ�������ĵ�����ͣ���Ϊ��ͨ����ص��

extern uint16 Menu_RotateMode[][5];//���Է�ʽ��������ʾһ��ֵ����ʾ����ֵ
extern uint16 RotatePoint[] ;//���������Բ����㣺
extern uint16 MaxCarrierDelay_1[] ;//����������ز���ʱ��
extern uint16 MaxCarrierDelay_2[] ;//(15<=MAX<=60)

//����
extern uint16 Rotate1[] ;//�����й�����
extern uint16 Rotate2[] ;//�����й�����
extern uint16 RotateZXWG[] ;     //�����޹�����  yb add 100519
extern uint16 RotateFXWG[] ;//�����޹�����
extern uint16 TotalEle[] ;//��:
extern uint16 Ele1[] ;//��:
extern uint16 Ele2[] ;//��:
extern uint16 Ele3[] ;//ƽ:
extern uint16 Ele4[] ;//��:
extern uint16 PreMonth[] ;//(����)
extern uint16 Current[] ;//(��ǰ)

extern uint16 Rotate3[] ;//���ʼ���������
extern uint16 Rotate3_1[] ;//�й�����(Kw):
extern uint16 Rotate3_2[] ;////�޹�����(Kvar):
extern uint16 Rotate3_3[] ;//��������

extern uint16 Rotate4[] ;//�������������ʱ��
extern uint16 Rotate4_1[] ;//�����й�(Kw):
extern uint16 Rotate4_2[] ;//�����й�(Kw):
extern uint16 Rotate4_3[] ;//����ʱ��:
extern uint16 Rotate4_4[] ;//�����޹�(Kw):    ybrepair  100519
extern uint16 Rotate4_5[] ;//�����޹�(Kw):

extern uint16 Rotate5[];//��ѹ�͵���

extern uint16 Rotate5_1[] ;//��ѹ(V)
extern uint16 Ua[] ;//Ua= 
extern uint16 Ub[] ;//Ub= 
extern uint16 Uc[] ;//Uc= 
extern uint16 Rotate5_2[] ;//����(A)
extern uint16 Ia[] ;//Ia= 
extern uint16 Ib[] ;//Ib= 
extern uint16 Ic[] ;//Ic= 

extern uint8 NoValue[] ;//--------
extern uint16 RotateWGDL[] ;//���޹�����
extern uint16 ZX[] ;//����:
extern uint16 FX[] ;//����:
extern uint16 MenuHz_DeviceAddr[] ;//�豸��ַ:
extern uint16 MenuHz_MeterNum[] ;//�����:
extern uint16 MenuHz_SysTime[] ;//�ն�ʱ��:
extern uint16 RotateYFFItem[] ;//Ԥ����������

extern uint16 MenuHz_QuadrantWGDL[] ;//�������޹��ܵ���
extern uint16 Quadrant_1[] ;//��1����:
extern uint16 Quadrant_2[] ;//��2����:
extern uint16 Quadrant_3[] ;//��3����:
extern uint16 Quadrant_4[] ;//��4����:

//������ʼֵ
extern uint8  IniTestAddr[];//��ʼ���Ե����ַ 000000000001
extern uint8  IniDate[] ;//08��09��01��
extern uint8  IniTime[] ; //00ʱ00��00��
extern uint8  IniIp[]  ;//192.168.000.001
extern uint16  Dot[]	;  //. . . 
extern uint8  IniPort[] ;//08001
extern uint8  IniSecIp[] ;
extern uint8  IniSecPort[] ;
extern uint8  IniEthenetIP[] ;//192.168.1.2��ʼ��̫��IP��ַ
extern uint8  IniSubNetMask[] ;//255.255.255.255��ʼ��������
extern uint8  IniGateWay[] ;//192.168.1.1��ʼ����
extern uint8  IniMACAddr[] ;//00:00:00:00:00:01 ��ʼMAC��ַ

extern uint8  IniApn[]  ;//cmnet
extern uint8  IniAddr[];//FF FF FF FF
extern uint8  IniInterfacePsw[] ;//000000 ��ֵ��Ϊ��������ʱ,�ڴ�ĳ�ʼֵ
extern uint8  IniInterfacePsw_1[] ;//111111//��ʼ����Ľ�������ֵ

extern uint8  IniTestPointNum[];//00001�������
extern uint8  IniCollectAdd[];//0000������ɼ�����ַ
extern uint8  IniTestPointNo[];//0001�������
extern uint8  IniDeviceNum[];//0001 ��ʼ���豸��
extern uint8  IniCommPort[];// 001 ��ʼ��ͨ�Ŷ˿ں�
//extern uint8  IniCarrierDelay[];//15 ��ʼ������ز���ʱʱ��

extern uint16 IPErr[];//IP����ERR
extern uint16 PortErr[];//�˿�����ERR

extern const uint16 RegistString[7];


//begin:Ϊ���ӳ����������ѯ���ӡ�zjjin,20150423.
extern uint16 ZCReBiaoHao[4];    //�ִ�  "�ȱ��:"
extern uint16 ZCFaKongHao[4];
extern uint16 ZCZongReLiang[4];   //�ִ�"�ۼ�����:"
extern uint16 ZCZongLiuLiang[4];   //�ִ�"�ۼ�����:"
extern uint16 ZCLiuSu[3]; //�ִ�"����:"
extern uint16 ZCkWh[3]; //�ִ�"kWh"
extern uint16 ZCMWh[3];
extern uint16 ZCLiFangMiperH[3];//�ִ�"m3/h"
extern uint16 ZCTongDaoHao[4];
extern uint16 ZCZhuHuXinXi[5];
extern uint16 ZCChaoFaKong[4]; //�ִ�"������:"
extern uint16 ZCChengGong[2];  //�ִ�"�ɹ�"
extern uint16 ZCShiBai[2];	 //�ִ�"ʧ��"
extern uint16 ZCShiWen[2]; //�ִ�"����"
extern uint16 ZCFaKai[2];  //�ִ�"����"
extern uint16 ZCTimeErr[7];
extern uint16 ZCBuChaoBiao[6];  //������������á�
extern uint16 ZCBiaoBuChaoCiShu[6];  //����������
extern uint16 ZCBiaoJianGe[8];  //��ʱ����(ms).
extern uint16 ZCFaBuChaoCiShu[6];  //������������
extern uint16 ZCFaJianGe[8];  //��ʱ����(ms).

extern uint16 ZCMeterFail[6];
extern uint16 ZCValveFail[6];

extern uint16 ZCJiLiangDian[4];
extern uint16 ZCNoNodeData[8];
extern uint16 ZCNoSetNode[8];
extern uint16 ZCThenRe[5];

extern uint16 ZCApnSet[5];  //APN���������
extern uint16 ZCIPAndPortSet[7];
extern uint16 ZCAPN1[7];  //0-cmnet.
extern uint16 ZCAPN2[11];  //1-unicomM2M.
extern uint16 ZCAPN3[12];  //2-whrd.wh.sd.
extern uint16 ZCSelectBySN[7];  //��������Ӧ��š�
extern uint16 ZCCueRestart[7];






//end:Ϊ���ӳ����������ѯ���ӡ�




/*�ֿ⺺�����£�
��������̩�ƴ�Ƽ��з��������ι�˾ע��ִ�е�ǰ���ʺ͵���ѹ�������������ܼ��ƽ����������ݲ��Ե���ʾ������鿴����������ά����Ϣ�ն�ʱ������������վ���˿ں��������ʼ��������Ҫѯ�ı����ַ����汾�����������ñ��ÿ������·�������ȷ�ϼ�����ȡ����
*/
extern const uint8 HZTable[][32];




#endif
