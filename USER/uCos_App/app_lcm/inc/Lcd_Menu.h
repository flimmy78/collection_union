#ifndef        	 _LCD_MENU_H
#define		 _LCD_MENU_H

#define		KEY_UP		0x01
#define 	KEY_DOWN	0x02
#define		KEY_LEFT	0x03
#define		KEY_RIGHT	0x04
#define		KEY_ENTER	0x05
#define		KEY_EXIT	0x06
#define 	NO_KEY		0xff

//#ifndef IN_LCD_MENU
void LCD_Initial(void);//Һ����ʼ�����Ժ���lcd_initial()�еĶԱȶ����ý��в���
void RegisterMenu(void);//ע�����˵�
//void IniPara(void);//��ʼ������  ��ʱ�������λ��

//1��
void MainMenu(void);//Һ�����˵�							==0

void Action_LcdMenu11(void); // "1.����������鿴"			==1
void Action_LcdMenu12(void); // "2.����״̬��ѯ				==2
void Action_LcdMenu13(void); // "3.������������ά��"		==3
void Action_LcdMenu14(void);
void Action_LcdMenu15(void);
void Action_LcdMenu16(void);//muxiaoqing test


//2��
void Action_LcdMenu11_21(void); //"1.ͨ��ͨ������"			==4
void Action_LcdMenu11_22(void); //"2.�ն�ʱ������"			==5
void Action_LcdMenu11_23(void); //"3.�ն˱������"			==6
void Action_LcdMenu11_24(void); //"4.����ʱ�������"		==7
void Action_LcdMenu11_25(void);	//"5.GPRS��������"		    ==28
void Set_IPAndPort(void);	  	       //"IP���˿ں�����"			==29
void Action_LcdMenu11_26(void);
void Set_ReplenishPara(void);


//"1.��ǰ����ʱ��ڵ㳭��״̬ͳ����ʾ"
void Action_LcdMenu12_21(void);			//					==8
void Action_LcdMenu12_22(void); 			// "1.�鿴��ǰʱ��ڵ㳭��״̬"
void Action_LcdMenu12_23(void);
void Action_LcdMenu12_24(void);

//�˴�Ԥ��8-12����ʾ����״̬��

void Action_LcdMenu13_21(void);	//"1.�����ն�"				==13
void Action_LcdMenu13_22(void); //"2.�ն˰汾��Ϣ"			==14
void Action_LcdMenu13_23(void); //"3.Һ���Աȶ�����"		==15

//muxiaoqing
#if 0
void Action_LcdMenu16_21(void);		//"SD ������д��Ͷ����ԱȲ���"
void Action_LcdMenu16_22(void);		//"ʱ��оƬ��д��Ͷ����ԱȲ���"
void Action_LcdMenu16_23(void);		//"GPRS ָ�����"
void Action_LcdMenu16_24(void);		//"���Դ����������"
void Action_LcdMenu16_25(void);		//"������1�������"
void Action_LcdMenu16_26(void);		//"GPRSͨѶ�������"
void Action_LcdMenu16_27(void);		//"RS485 ͨѶ1�������"
void Action_LcdMenu16_28(void);		//"RS485 ͨѶ2�������"
#else
void Action_LcdMenu16_21(void);		//"�ֶ�����"
void Action_LcdMenu16_22(void);		//"�Զ�����"

#endif

//3��
void ZigbeeSet(void);				//����zigbee��վ��ַ	==16
void Set_SystemTime(void);			//�����ն�ʱ�亯��		==17
void Set_SystemAddr(void);			//�ն˱�����ú���		==18
void Set_ReadMeterTime(void);		//����ʱ������ú���	==19





void ReadMeterStateShow(void);		//����״̬��Ϣ���麯��	==20
void ReadMeterNow(void);//�������� ==21
void ShowMeterFail(void);
void ShowValveFail(void);
void InquiryMeterFailInfo(void);
void InquiryValveFailInfo(void);
void SlectReadMeterChannel(void);
//muxiaoqing test menu manual
void Action_LcdMenu16_21_31(void); 	//"SD ������д��Ͷ����ԱȲ���"
void Action_LcdMenu16_21_32(void); 	//"ʱ��оƬ��д��Ͷ����ԱȲ���"
void Action_LcdMenu16_21_33(void); 	//"GPRS ָ�����"
void Action_LcdMenu16_21_34(void); 	//"���Դ����������"
void Action_LcdMenu16_21_35(void); 	//"������1�������"
void Action_LcdMenu16_21_36(void); 	//"GPRSͨѶ�������"
void Action_LcdMenu16_21_37(void); 	//"RS485 ͨѶ1�������"
void Action_LcdMenu16_21_38(void); 	//"RS485 ͨѶ2�������"

#ifdef DEMO_APP
void ReadMeter_Demo(void);//
void ReadMeterStateShow_Demo(void);		//����״̬��Ϣ���麯��	==20
void Set_MeterParam_1(void);		//
void Set_MeterParam_2(void);		//
void Set_MeterParam_3(void);		//

#endif

//�˴�Ԥ��20-24����ʾ����״̬��

void ResetSystem(void);  			//�����նˣ�ȷ�ϻ��߷���==25
void VerInfoShow(void);				//��ʾ �ն˰汾��Ϣ		==26
void LCDContrastSet(void);			//��ʾ Һ���Աȶ�		==27

void KeyCheck(void);
void MenuTest(void);
void Meter_RotateDisplay(void);
void RotateDisplay(void);

void Chose_metric_method(void);
void Chose_time_data(void);
void Display_Data_Menu(void);
uint8  HexToAscii_1(uint16 Hex_bcd_code, uint8 *AscBuf);
void factory_test_method_sd(void);//muxiaoqing test
void factory_test_method_time(void);
void factory_test_method_GPRS(void);
void factory_test_method_Debug_RS232_0(void);
void factory_test_method_ChaoBiao_RS232_1(void);
void factory_test_method_GPRS_TTL_2(void);
void factory_test_method_Commu_Zigbee_3(void);
void factory_test_method_Commu_RS485_4(void);
void factory_test_method_auto(void);
void SelectAPNSet(void);
void SelectIPSet(void);
void APNSet(void);






//���ļ����ú���
uint8 ConverHexToASCIP_15(uint8 *pTemp1, uint8 *pTemp2);
uint8 Hex_BcdToAscii_5(uint16 Hex_bcd_code, uint8 *AscBuf);

typedef	struct	SelectInfoStruct
{
    uint8 startline;
    uint8 startcolumn;
    uint8 columnnum;
} SelectInfo;

#pragma pack(1)

typedef struct DisplayDataStruct
{
    uint8   user_id;/*�û����*/
    uint16	address;  /*�豸��ַ*/
    uint16	area;/*�û����*/
    uint32  open_time;/*(BCD��)����ʱ�䣬��2λΪС��λ����λ:h*/
    uint16  room_temperature;/*(BCD��)�����¶ȣ���2λΪС��λ����λ:���϶�*/
    uint16  forward_temperature;/*(BCD��)��ˮ�¶ȣ���2λΪС��λ����λ:���϶�*/
    uint16  return_temperature;/*(BCD��)��ˮ�¶ȣ���2λΪС��λ����λ:���϶�*/
    uint32  proportion_energy;/*(BCD��)��ǰʱ����ڵ�������ֵ̯*/
    uint32  total_energy;/*(BCD��)�û��������ۼ�ֵ,��2λΪС��λ����λ:kwh*/
} DisplayData;


typedef struct SDSaveDataStruct
{
    uint8   data_len;
    uint8   user_id;/*�û����*/
    uint16  address;  /*�豸��ַ*/
    uint16  area;/*�û����*/
    uint8   data_valid;/*�����Ƿ���Ч  ��ЧΪ0xA5����ЧΪ0x00*/
    uint32  total_energy;/*(BCD��)�û��������ۼ�ֵ,��2λΪС��λ����λ:kwh*/
    uint32  open_time;/*(BCD��)����ʱ�䣬��2λΪС��λ����λ:h*/
    uint32  proportion_energy;/*(BCD��)��ǰʱ����ڵ�������ֵ̯*/
    uint8   open_percent;/*��ǰʱ����ڵķ��ſ�������(HEX�� 0-100) */
    uint16  forward_temperature;/*(BCD��)��ˮ�¶ȣ���2λΪС��λ����λ:���϶�*/
    uint16  return_temperature;/*(BCD��)��ˮ�¶ȣ���2λΪС��λ����λ:���϶�*/
    uint16  room_temperature;/*(BCD��)�����¶ȣ���2λΪС��λ����λ:���϶�*/
    uint8   state;/*bit7 �������߹��ϣ�1���� 0����
				      bit6 ����Ƿ��״̬ 1Ƿ�� 0����
				     bit5 �����ֵ״̬ 1��ֵ 0����
				     bit4 �����ػ�״̬��1������0�ػ�;
				     bit3 �������� 0������1����;
				     bit2 �����Ŷ�ת���ϣ�1���ϣ�0����;
				     bit1 ����NTC����        1���ϣ�0����;
				     bit0 ������״̬��1������0����*/
} SDSave_Data;

typedef struct CheckDataStruct
{
    uint8 user_id;//�û�id
    uint8 read_num;//�����
    uint8 time_path[17];
} checkData;

#pragma pack()

typedef struct	KeyStateStruct
{
    uint8 KeyStateIndex;//��ǰ����
    uint8 KeyDnState;//�����¼�ʱת�������
    uint8 KeyUpState;//�����ϼ�ʱת�������
    uint8 KeyLeftState;//�������ʱת�������
    uint8 KeyRightState;//�����Ҽ�ʱת�������
    uint8 KeyCrState;//��ȷ�ϼ�ʱת�������
    uint8 KeyBackState;//��ȡ����ʱת�������
    void  (*CurrentOperate)(void);//��ǰ����ִ�еĶ���
} KeyState;

//void *pMainMenu;
//pMainMenu  = &MainMenu;
#define	KEYBORD_MENE_SIZE	60 //muxiaoqing 50->60

extern KeyState KeyTab[KEYBORD_MENE_SIZE];
//#endif

#endif
