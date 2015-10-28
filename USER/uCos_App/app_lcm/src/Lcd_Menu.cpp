/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)                                                          
**  ��   ��   ��: lcd_menu.cpp
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��9��10�� 
**  ��        ��: Һ����ʾĿ¼
**	�� ��  �� ¼:   	
*****************************************************************************************************/
/********************************************** include *********************************************/
#include <includes.h>
extern "C"{
#include "lcmdrv.h"
#include "PictureCode.h"
#include "HZTable.h"
#include "ASCIICode.h"
}
#include "app_flashmem.h"
#include "app_down.h"
//#include "IniJzqPara.h"
#include "StructProtocol.h"
//#include "BaseProtocol.h"
#include "interfacefuns.h"
#include "lcd_Menu.h"
#include "ReadMeterData.h"
#include "fat_sd.h"

/********************************************** struct *********************************************/
struct TimeNode{
	uint8 Hour;
	uint8 Minute;
};

struct  StateInfo{
			TimeNode 	LastNode;
			uint16		MeterNum;				//��������
			uint16		ReadedCmplNum;			//����ɹ���
			uint16		UsedTime;				//���γ�����ʱ
};
StateInfo gREAD_MeterReadState;
/********************************************** define *********************************************/
//#define IN_LCD_MENU
#define	TotalRotatePage 5 //���Ե�����ҳ��
#define DataItemNum     6 //�ɼ������������
#define TotalMeterType
/*****************************************extern  global **********************************************/
extern uint16 gVersion;
/****************************************    global **********************************************/
uint8 GChannelType = 0; //��ǰͨ��ͨ������
uint8 GRoMode = 0;//����ģʽ һ��ֵ���߶���ֵ
uint8 FixPsw[6] = {0x30,0x30,0x30,0x30,0x30,0x30};
//METER_DATA gMeterData;//��������ȫ�ֱ������������ݷ�������
#ifdef DEMO_APP
uint8 Channel_1_SUCESS =0;
uint8 Channel_2_SUCESS =0;
uint8 Channel_3_SUCESS =0;
CJ188_Format Demo_CJ188_ch1;
CJ188_Format Demo_CJ188_ch2;
CJ188_Format Demo_CJ188_ch3;

uint8 Meter_R_End =0;
uint8 Demo_param_set =0;
uint8 Show_Data_Status=0;
uint8 Data_complete=0;
extern uint8 DemoDateIsGet ;
#endif
checkData str_check_data;

DisplayData strDisplayData; 
uint8 autotestmode = FALSE;//muxiaoqing test
uint8 manualtestmode = FALSE;
uint8 autotestRetryNum = 0;
uint8 autotestdisp3 = FALSE;
uint8 autotestIsfromExitKey =0;
uint8 autotestFirstIn =1;
uint8 testPortreadNum =0;
extern uint8 IRQ5testBuf_Counter;
extern uint8 IRQ5testBuf[32];
extern uint8 IRQ4testBuf_Counter;
extern uint8 IRQ4testBuf[32];
extern uint8 IRQ3testBuf_Counter;
extern uint8 IRQ3testBuf[32];
extern uint8 IRQ2testBuf_Counter;
extern uint8 IRQ2testBuf[32];
extern uint8 IRQ1testBuf_Counter;
extern uint8 IRQ1testBuf[32];
extern uint8 forbidsending1 ;//muxiaoqing test

extern LCDContrastPara NVLCDConraston;

//�����ʼ��
//�������ڹ��� OK �������������д��Ժ����
KeyState KeyTab[KEYBORD_MENE_SIZE] = 
{
	{0,1,1,1,1,1,0,(*MainMenu)},							//���˵�
	//һ���˵�
	//{1,2,31,1,1,4,0,(*Action_LcdMenu11)},					//"����������鿴"					
	{1,2,3,1,1,4,0,(*Action_LcdMenu11)},					//"����������鿴"	muxiaoqing test				

	{2,3,1,2,2,8,0,(*Action_LcdMenu12)},					//"����������״̬�鿴"
	{3,1,2,3,3,13,0,(*Action_LcdMenu13)},					//"������������ά��"

	
	//�����˵�
	// "����������鿴" �µĲ˵�
	{4,5,45,4,4,16,1,(*Action_LcdMenu11_21)},				//"ͨ��ͨ������"
	{5,6,4,5,5,17,1,(*Action_LcdMenu11_22)}, 				//"�ն�ʱ������"
	{6,7,5,6,6,18,1,(*Action_LcdMenu11_23)},  				//"�ն˱������"
	{7,28,6,7,7,19,1,(*Action_LcdMenu11_24)},  				//"����ʱ�������"
   

	// "����������״̬�鿴" �µĲ˵�, ��ʱֻ��һ���˵�
	{8,9,8,8,8,20,2,(*Action_LcdMenu12_21)},					//��ʾ����״̬�˵�
	{9,10,8,9,9,21,2,(*Action_LcdMenu12_22)},							//��������
	{10,11,9,10,10,22,2,(*Action_LcdMenu12_23)},							//����ͨ������
	{11,11,10,11,11,47,2,(*Action_LcdMenu12_24)},							//ʧ����Ϣ��ѯ
	{12,11,1,1,1,1,0,(*MainMenu)},							//Ԥ��


	{13,14,15,13,13,25,3,(*Action_LcdMenu13_21)},			//"�����ն�"
	{14,15,13,14,14,26,3,(*Action_LcdMenu13_22)},			//"�汾��Ϣ"
	{15,13,14,15,15,27,3,(*Action_LcdMenu13_23)},			//"�Աȶȵ���"

	//�����˵�
	{16,16,16,16,16,16,4,(*ZigbeeSet)},						//"����zigbeeͨ�ţ���վ��ַ"
	{17,17,17,17,17,17,5,(*Set_SystemTime)},				//"����ʱ����庯��"
	{18,18,18,18,18,18,6,(*Set_SystemAddr)},				//"�����ն˱�ź���"
	{19,19,19,19,19,19,7,(*Set_ReadMeterTime)},				//"���ó���ʱ��㺯��"
	
	{20,20,20,20,20,20,8,(*ReadMeterStateShow)},			//"����״̬��Ϣ���麯��"
	{21,21,21,21,21,21,9,(*ReadMeterNow)},							//��������
	{22,22,22,22,22,22,10,(*SlectReadMeterChannel)},		/*ѡ�񳭱�ͨ��*/
	{23,1,1,1,1,1,0,(*MainMenu)},							//Ԥ��
	{24,1,1,1,1,1,0,(*MainMenu)},							//Ԥ��

	{25,26,27,25,25,25,13,(*ResetSystem)},					//"�����ն�ȷ�ϻ��߷��Ϻ���"
	{26,27,25,26,26,26,14,(*VerInfoShow)},					//"�ն˰汾��Ϣ����"
	{27,25,26,27,27,27,15,(*LCDContrastSet)},				//"Һ���ԱȶȺ���"

	{28,45,7,28,28,51,1,(*Action_LcdMenu11_25)},  	        //"GPRS��������"
	{29,29,29,29,29,29,52,(*Set_IPAndPort)},  		     	//"IP���˿ں�����"

	//һ���˵�
	{30,31,3,30,30,32,0,(*Action_LcdMenu14)},					//"ʱ��ͨ�������"
//	{31,1,30,31,31,33,0,(*Action_LcdMenu15)},					//"ѡ���������"
		{31,35,30,31,31,33,0,(*Action_LcdMenu15)},					//"ѡ���������"

	//�����˵�
	{32,32,32,32,32,34,30,(*Chose_time_data)},				   //"ѡ��ʱ������"
	{33,33,33,33,33,33,31,(*Chose_metric_method)},			   //"ѡ���������"
	{34,34,34,34,34,34,32,(*Display_Data_Menu)},			   //"������ʾ����"
	//һ���˵�
	{35,1,31,35,35,36,0,(*Action_LcdMenu16)},					//muxiaoqing test "��������"
	
	//�����˵�
	#if 0
	{36,37,43,36,36,44,35,(*Action_LcdMenu16_21)},			  //��ʾ"SD ������д��Ͷ����ԱȲ���"�˵�
	{37,38,36,37,37,45,35,(*Action_LcdMenu16_22)},			//"ʱ��оƬ��д��Ͷ����ԱȲ���" �˵�
	{38,39,37,38,38,46,35,(*Action_LcdMenu16_23)},			//"GPRS ָ�����"�˵�
	{39,40,38,39,39,47,35,(*Action_LcdMenu16_24)},			//"���Դ����������"�˵�
	{40,41,39,40,40,48,35,(*Action_LcdMenu16_25)},			//"������1�������"�˵�
	{41,42,40,41,41,49,35,(*Action_LcdMenu16_26)},			//"GPRSͨѶ�������"�˵�
	{42,43,41,42,42,50,35,(*Action_LcdMenu16_27)},			 //"RS485 ͨѶ1�������"�˵�
	{43,36,42,43,43,51,35,(*Action_LcdMenu16_28)},			//"RS485 ͨѶ2�������"�˵�
	#else
	
#ifdef DEMO_APP
    
    {36,37,37,36,36,38,35,(*Action_LcdMenu16_21)},			  //��ʾ"��������"�˵�
    {37,36,36,37,37,41,35,(*Action_LcdMenu16_22)},			//��ʾ"�Զ��ɼ�" �˵�
#else
	{36,37,37,36,36,38,35,(*Action_LcdMenu16_21)},			  //��ʾ"�ֶ�����"�˵�
	{37,36,36,37,37,46,35,(*Action_LcdMenu16_22)},			//��ʾ"�Զ�����" �˵�
#endif	
	#endif
	//�����˵�
	#if 0
	{44,44,44,44,44,44,36,(*factory_test_method_sd)},			//"SD ������д��Ͷ����ԱȲ���"
	{45,45,45,45,45,45,37,(*factory_test_method_time)},			//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	{46,46,46,46,46,46,38,(*factory_test_method_GPRS)},			//"GPRS ָ�����"
	{47,47,47,47,47,47,39,(*factory_test_method_Debug_RS232_0)},			//"���Դ����������"
	{48,48,48,48,48,48,40,(*factory_test_method_ChaoBiao_RS232_1)},			//"������1�������"
	{49,49,49,49,49,49,41,(*factory_test_method_GPRS_TTL_2)},			//"GPRSͨѶ�������"
	{50,50,50,50,50,50,42,(*factory_test_method_Commu_Zigbee_3)},			//"RS485 ͨѶ1�������"
	{51,51,51,51,51,51,43,(*factory_test_method_Commu_RS485_4)},			//"RS485 ͨѶ2�������"
	#else
#ifdef DEMO_APP
//�����˵�

	{38,39,41,38,38,42,36,(*Action_LcdMenu16_21_31)},			  //"��³"
	{39,40,38,39,39,43,36,(*Action_LcdMenu16_21_32)},			//"������"
	{40,41,39,40,40,44,36,(*Action_LcdMenu16_21_33)},			//"���"
	//{41,42,40,41,41,50,36,(*Action_LcdMenu16_21_34)},			//"���Դ����������"�˵�
	
    {41,41,41,41,41,41,37,(*ReadMeter_Demo)}, 		//"�Զ��ɼ�"

//�ļ��˵�

    
	{42,42,42,42,42,42,38,(*Set_MeterParam_1)},			//��һ�Ĳ�������
	{43,43,43,43,43,43,39,(*Set_MeterParam_2)},			//����Ĳ�������
	{44,44,44,44,44,44,40,(*Set_MeterParam_3)},			//�����Ĳ�������
	//{50,50,50,50,50,50,41,(*factory_test_method_Debug_RS232_0)},			//"���Դ����������"

#else	
	//�����˵�

	{38,39,45,38,38,47,36,(*Action_LcdMenu16_21_31)},			  //��ʾ"SD ������д��Ͷ����ԱȲ���"�˵�
	{39,40,38,39,39,48,36,(*Action_LcdMenu16_21_32)},			//"ʱ��оƬ��д��Ͷ����ԱȲ���" �˵�
	{40,41,39,40,40,49,36,(*Action_LcdMenu16_21_33)},			//"GPRS ָ�����"�˵�
	{41,42,40,41,41,50,36,(*Action_LcdMenu16_21_34)},			//"���Դ����������"�˵�
	{42,43,41,42,42,51,36,(*Action_LcdMenu16_21_35)},			//"������1�������"�˵�
	{43,44,42,43,43,52,36,(*Action_LcdMenu16_21_36)},			//"GPRSͨѶ�������"�˵�
	{44,45,43,44,44,53,36,(*Action_LcdMenu16_21_37)},			 //"RS485 ͨѶ1�������"�˵�
	{45,38,44,45,45,54,36,(*Action_LcdMenu16_21_38)},			//"RS485 ͨѶ2�������"�˵�
	{46,46,46,46,46,46,37,(*factory_test_method_auto)},			//"�Զ�����"

//�ļ��˵�

    
	{47,44,44,44,44,47,38,(*factory_test_method_sd)},			//"SD ������д��Ͷ����ԱȲ���"
	{48,48,48,48,48,48,39,(*factory_test_method_time)},			//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	{49,49,49,49,49,48,40,(*factory_test_method_GPRS)},			//"GPRS ָ�����"
	{50,50,50,50,50,50,41,(*factory_test_method_Debug_RS232_0)},			//"���Դ����������"
	{51,51,51,51,51,51,42,(*factory_test_method_ChaoBiao_RS232_1)},			//"������1�������"
	{52,52,52,52,52,52,43,(*factory_test_method_GPRS_TTL_2)},			//"GPRSͨѶ�������"
	{53,53,53,53,53,53,44,(*factory_test_method_Commu_Zigbee_3)},			//"RS485 ͨѶ1�������"
	{54,54,54,54,54,54,45,(*factory_test_method_Commu_RS485_4)},			//"RS485 ͨѶ2�������"
	#endif

	
#endif	


//begin:added by zjjin.
{45,4,28,45,45,46,1,(*Action_LcdMenu11_26)}, 		//"�������������"
{46,46,46,46,46,46,45,(*Set_ReplenishPara)},		//"����������ʱ��������"
{47,48,48,47,47,49,11,(*ShowMeterFail)}, 			//ѡ�б�ʧ����Ϣ��ѯ��
{48,47,47,48,48,50,11,(*ShowValveFail)},			//ѡ�з�ʧ����Ϣ��ѯ��
{49,49,49,49,49,49,47,(*InquiryMeterFailInfo)},			//��ʧ����Ϣ��ѯ��
{50,50,50,50,50,50,48,(*InquiryValveFailInfo)}, 		//��ʧ����Ϣ��ѯ��
{51,52,52,51,51,53,28,(*SelectAPNSet)}, 			//ѡ��APN���á�IP���˿����ã�ѡ��APN���á�
{52,51,51,52,52,29,28,(*SelectIPSet)},		    	//ѡ��APN���á�IP���˿����ã�ѡ��IP���˿�����.
{53,53,53,53,53,53,51,(*APNSet)},	











//end:added by zjjin.




};

uint8 KeyFuncIndex = 0;
uint8 KeyMessageFlag = 0;


void test(void)
{
	strDisplayData.user_id = 112;
	strDisplayData.address = 0x45f0;
	strDisplayData.area = 250;
	strDisplayData.open_time = 0x40;//0x12345678;
	strDisplayData.room_temperature = 0x2345;
	strDisplayData.forward_temperature = 0x2345;
	strDisplayData.return_temperature = 0x2345;
	strDisplayData.proportion_energy = 0x234567;
	strDisplayData.total_energy = 0x12345678;
}


/****************************************************************************************************
**	�� ��  �� ��: FreshIPAPortSet
**	�� ��  �� ��: ˢ��IP�Ͷ˿�����
**	�� ��  �� ��: uint8* InputBuff
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshIPAndPortSet(uint8 * InputBuff,uint8 * InputBuff1 )
{
	uint8 i,line,column;


	display_white();
	ShowMenuTop();
	ShowMenuBottom(21);
	ShowLineChar_HZ(1,9,&MenuHZ11_29[2],8);	//IP���˿ں�����
	ShowLineChar_HZ(2,0,&MainIP[0],4);	    //��IP:
	ShowLineChar_HZ(4,0,&MainPort[0],6);   //��IP�˿�:
    ShowPicture(80,22,16,32,&MenuPic_OK[0]);//5�� OK
	line = 3;
	column = 0;
	for(i=0;i<4;i++)
	{
		ShowChar(line,column+(12*i),InputBuff[3*i]);
		ShowChar(line,column+(12*i+3),InputBuff[3*i+1]);
		ShowChar(line,column+(12*i+6),InputBuff[3*i+2]);
	}

	ShowLineChar_HZ(3,9,&Dot[0],1);	//��1����.
	ShowLineChar_HZ(3,21,&Dot[0],1);//��2����.
	ShowLineChar_HZ(3,33,&Dot[0],1);//��3����.
    
	for(i=0;i<5;i++)
	{
		ShowChar(4,24+3*i,InputBuff1[i]);
	}
}
/****************************************************************************************************
**	�� ��  �� ��: Set_IPAPort
**	�� ��  �� ��: IP���˿ں�����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Set_IPAndPort(void)  
{
  	uint8 status;
	uint8 line;
	uint8 line1;
	uint8 line2;
	uint8 column;
	uint8 column1;
	uint8 column2;
	int8 i;
	uint8 ASCIIData[12];
	uint8 ASCIIData1[5];

	uint8 TermAddr[4] = {0};
	uint8 TermAddrReverse[4] = {0x00};
	uint16 TermAddr1[4] = {0};

	uint16 TermPort = 0;
	uint32 TermPort2 = 0;


	SelectInfo TabMenu[] = {{3,0,45},{4,24,15},{5,21,12}};//�У��У�����
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;

	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;

	CPU_SR			cpu_sr;

	OS_ENTER_CRITICAL();
	memcpy(TermAddrReverse, gPARA_UpPara.HostIPAddr, 4);
	TermPort = gPARA_UpPara.HostPort ;
	OS_EXIT_CRITICAL();
	
	//���ڴ洢���ֽ���ǰ���������ڽ��ֽ�˳�����
	for(i=0; i<4; i++)
	{
	 TermAddr[i] = TermAddrReverse[3-i];
	}
	for(i=0; i<4; i++)
	{
	  TermAddr1[i] = HexToBcd2bit( TermAddr[i] ); //0XAB
	} 	
	//ip��ַ��
	for(i=0;i<4;i++)
	{
	   	ASCIIData[3*i]   = HexToAscii( TermAddr1[i]>>8 );
		ASCIIData[3*i+1] = HexToAscii((TermAddr1[i]>>4)&0x0f );
		ASCIIData[3*i+2] = HexToAscii( TermAddr1[i]&0x0f );
	}
   
   	//�˿ں�	
	 TermPort2 = HexToBcd4bit( TermPort );  //0XABCD
	 ASCIIData1[0] = HexToAscii(  TermPort2 >>16 );
	 ASCIIData1[1] = HexToAscii( (TermPort2&0x0f000)>>12 );
	 ASCIIData1[2] = HexToAscii( (TermPort2&0x00f00)>>8 );
	 ASCIIData1[3] = HexToAscii( (TermPort2&0x000f0)>>4 );
	 ASCIIData1[4] = HexToAscii( (TermPort2&0x0000f) );
   								  		           
	i = 0;
	line = TabMenu[0].startline;
	column = TabMenu[0].startcolumn;
	line1 = TabMenu[1].startline;
	column1 = TabMenu[1].startcolumn;
	line2 = TabMenu[0].startline;
	column2 = TabMenu[0].startcolumn;

	MaxTabNum = ( sizeof(TabMenu) / sizeof(SelectInfo) ) - 1;//����±�

	while(OK_Flag == FALSE)
	{
		FreshIPAndPortSet(&ASCIIData[0],&ASCIIData1[0]);
		
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len( TabMenu[TabNum].startline , TabMenu[TabNum].startcolumn , TabMenu[TabNum].columnnum ) ;
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if( TabNum == MaxTabNum )
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 4;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
				default:	
					break;
			}		
		}
		else if(TabNum == 0)
	         {
		       ReverseShowChar(line,column);
			   status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			   switch(status)
				{
				   case KEY_UP://���ϼ�
						 if( (i%3 == 0) )
						 {
							  if(ASCIIData[i] == 0x32)
							    {
								   ASCIIData[i] = 0x30;
						    	}
							    else
							    {
							       ASCIIData[i] = ASCIIData[i] + 1;
							    }
						    
						 }
						 else
						    if(ASCIIData[i] == 0x39)
						    {
							   ASCIIData[i] = 0x30;
					    	}
						    else
						    {
						       ASCIIData[i] = ASCIIData[i] + 1;
						    }
						 break;
					case KEY_DOWN://���¼�
						 if( (i%3 == 0) )
							 {
								  if(ASCIIData[i] == 0x30)
								    {
									   ASCIIData[i] = 0x32;
							    	}
								    else
								    {
								       ASCIIData[i] = ASCIIData[i] - 1;
								    }
							    
							 }
						 else
							if(ASCIIData[i] == 0x30)
							{
								ASCIIData[i] = 0x39;
							}
							else
							{
								ASCIIData[i] = ASCIIData[i] - 1;
							}	
						break;			
					case KEY_LEFT://�����
						ShowChar(line,column,ASCIIData[i]);
						if(i > 0)
						{
							if( (i%3) == 0 )
							{
								column = column - 6;
							}
							else
							{
								column = column - 3;
							}
							i = i - 1;
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line,column,ASCIIData[i]);
						if(i < 11)
						{
							if( (i%3) == 2 )
							{
								column = column + 6;
							}
							else
							{
								column = column + 3;
							}
							i = i + 1;
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
						i = 0;
						line = TabMenu[0].startline;
						column = TabMenu[0].startcolumn;
						line1 = TabMenu[1].startline;
						column1 = TabMenu[1].startcolumn;
						line2 = TabMenu[2].startline;
						column2 = TabMenu[2].startcolumn;
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 4;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					default:	
						break;
			    }
		    }//end else if(TabNum == 0)
			else if(TabNum == 1)
		         {
			       ReverseShowChar(line1,column1);
				   status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				   switch(status)
					{
					   case KEY_UP://���ϼ�
						    if( (i == 0) )
							 {
								  if(ASCIIData1[i] == 0x36)
								    {
									   ASCIIData1[i] = 0x30;
							    	}
								    else
								    {
								       ASCIIData1[i] = ASCIIData1[i] + 1;
								    }
							    
							 }
						   else
								if(ASCIIData1[i] == 0x39)
							    {
								   ASCIIData1[i] = 0x30;
						    	}
							    else
							    {
							       ASCIIData1[i] = ASCIIData1[i] + 1;
							    }
						    break;
						case KEY_DOWN://���¼�
							if( (i == 0) )
								 {
									  if(ASCIIData1[i] == 0x30)
									    {
										   ASCIIData1[i] = 0x36;
								    	}
									    else
									    {
									       ASCIIData1[i] = ASCIIData1[i] - 1;
									    }
								    
								 }
							 else
								if(ASCIIData1[i] == 0x30)
								{
									ASCIIData1[i] = 0x39;
								}
								else
								{
									ASCIIData1[i] = ASCIIData1[i] - 1;
								}	
							break;			
						case KEY_LEFT://�����
							ShowChar(line1,column1,ASCIIData1[i]);
							if(i > 0)
							{
								i = i - 1;
								column1 = column1 - 3;
							}
							break;
						case KEY_RIGHT://���Ҽ�
							ShowChar(line1,column1,ASCIIData1[i]);
							if(i < 4)
							{
								i = i + 1;
								column1 = column1 + 3;
							}
							break;
						case KEY_ENTER://�س���
							Tab_Flag = (!Tab_Flag);
							//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
							i = 0;
							line = TabMenu[0].startline;
							column = TabMenu[0].startcolumn;
							line1 = TabMenu[1].startline;
							column1 = TabMenu[1].startcolumn;
							TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
							line2 = TabMenu[2].startline;
							line2 = line2;/*��ֹ���뾯��*/
						    column2 = TabMenu[2].startcolumn;
							column2 = column2;	/*��ֹ���뾯��*/
							break;
						case KEY_EXIT://���ؼ�
							KeyFuncIndex = 4;
							(*KeyTab[KeyFuncIndex].CurrentOperate)();
							return;			
						default:	
							break;
				    }
			    }
	}//end while
	
    //�Ĵ�������
   	memset(TermAddr1, 0x00, 8);
	memset(TermAddr, 0x00, 4);
	TermPort2 =	0;
	TermPort = 0;

	for(i=0;i<4;i++)
	{		
	  TermAddr1[i] = ((ASCIIData[3*i]-0x30)<<8) + (((ASCIIData[3*i+1]-0x30)<<4)&0xf0) + ((ASCIIData[3*i+2]-0x30)&0x0F);
	}
	for(i=0; i<4; i++)
	{
	  TermAddr[i] = Uint16BCDToHex1( TermAddr1[i] ); //0XAB
	}
	for(i=0; i<4; i++)
	{
	 TermAddrReverse[i] = TermAddr[3-i];
	}

	//��Ҫ���У���������һ��
	TermPort2 =((ASCIIData1[0]-0x30)<<16)+(((ASCIIData1[1]-0x30)<<12)&0x0f000)+(((ASCIIData1[2]-0x30)<<8)&0x00f00)+(((ASCIIData1[3]-0x30)<<4)&0x000f0)+((ASCIIData1[4]-0x30)&0x0000f); 
	TermPort = BcdToHex_16bit1( TermPort2 );

    if(TermPort2 > 0x65535)  
	 {
	   ShowLineChar_HZ(6,5,&PortErr[0],7);	//�˿�����ERR
	   OSTimeDly(2000);
	   KeyFuncIndex = 29;
	   (*KeyTab[KeyFuncIndex].CurrentOperate)();
	 }

	  for(i=0;i<4;i++)
	  {		
	    if( TermAddr1[i]>0x0255 )
	     { 
		   ShowLineChar_HZ(7,5,&IPErr[0],7);	//ip����ERR
		   OSTimeDly(1500);
		   KeyFuncIndex = 29;
	       (*KeyTab[KeyFuncIndex].CurrentOperate)();
         }	  
    }
	 
	SetIP( TermAddrReverse );//IP����ɹ�	 
	SetHostPort( TermPort );//Port����ɹ�	

  	//==========================================
	ShowMenuBottom(2);//���óɹ�

	KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();

	return;

}

/****************************************************************************************************
**	�� ��  �� ��: SelectAPNSet
**	�� ��  �� ��: ѡ��APN��
**	�� ��  �� ��: 
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void SelectAPNSet(void)
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(2,3,&ZCApnSet[0],5);
	ShowLineChar_HZ(3,3,&ZCIPAndPortSet[0],7);
	ReverseShowLine(2);

}

/****************************************************************************************************
**	�� ��  �� ��: SelectIPSet
**	�� ��  �� ��: ѡ��IP���˿����á�
**	�� ��  �� ��: 
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void SelectIPSet(void)
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(2,3,&ZCApnSet[0],5);
	ShowLineChar_HZ(3,3,&ZCIPAndPortSet[0],7);
	ReverseShowLine(3);

}





/****************************************************************************************************
**	�� ��  �� ��: FreshAPNPara
**	�� ��  �� ��: ˢ��APN������ʾ��
**	�� ��  �� ��: uint8* InputBuff
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshAPNPara(uint8 *InputBuff)
{
	uint8 i,line,column;


	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(1,1,&ZCAPN1[0],7);
	ShowLineChar_HZ(2,1,&ZCAPN2[0],11);
	ShowLineChar_HZ(3,1,&ZCAPN3[0],12);
	ShowLineChar_HZ(4,1,&ZCSelectBySN[0],7);
	ShowLineChar_HZ(5,1,&ZCCueRestart[0],7);

	ShowPicture(128,22,16,32,&MenuPic_OK[0]);//8�� OK



    
	line = 7;
	column = 25;

	ShowChar(line,column,*InputBuff);

}


/****************************************************************************************************
**	�� ��  �� ��: APNSet
**	�� ��  �� ��: ѡ��APN��������ơ�
**	�� ��  �� ��: 
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: ѡ��ȷ�Ϻ�����������,������APN�������硣
*****************************************************************************************************/
void APNSet(void)
{
	uint8 err = 0;
 	uint8 line1;
	uint8 column1;
	uint8 lu8ASCIIData;
	uint8 status;

	SelectInfo TabMenu[] = {{7,25,3},{8,21,12}};//�У��У�����
	uint8 TabNum = 0;
	uint8 MaxTabNum = 1;

	GprsAPNSelect ApnSet;

	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;

	CPU_SR			cpu_sr;


	OSMutexPend (FlashMutex,0,&err);
	SDReadData("/GprsAPNSelect", &ApnSet, sizeof(GprsAPNSelect), 0);
	OSMutexPost (FlashMutex);

	line1 = TabMenu[0].startline;
	column1 = TabMenu[0].startcolumn;

	if(ApnSet.WriteFlag == 0xAA55)
		lu8ASCIIData   = HexToAscii(ApnSet.GprsAPNSet);
	else
		lu8ASCIIData   = HexToAscii(0);

   
	while(OK_Flag == FALSE)
	{
		FreshAPNPara(&lu8ASCIIData);
				
		if(Tab_Flag == TRUE){
			ReverseShowLine_len( TabMenu[TabNum].startline , TabMenu[TabNum].startcolumn , TabMenu[TabNum].columnnum ) ;
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)	{
						TabNum = MaxTabNum;
					}
					else{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)	{
						TabNum = 0;
					}
					else{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if( TabNum == MaxTabNum ){
						OK_Flag = TRUE;
					}
					else{
						Tab_Flag = FALSE;
					}
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 4;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
				default:	
					break;
			}		
		}
		else if(TabNum == 0)
	         {
		       ReverseShowChar(line1,column1);
			   status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			   switch(status)
				{
				   case KEY_UP://���ϼ�
						lu8ASCIIData = lu8ASCIIData + 1;   //����������Χ0-9.
				   		if(lu8ASCIIData > 0x39)
							lu8ASCIIData = 0x30;
						 break;
					case KEY_DOWN://���¼�
						lu8ASCIIData = lu8ASCIIData - 1;
				   		if(lu8ASCIIData < 0x30)
							lu8ASCIIData = 0x39;
						 break;		
					case KEY_LEFT://�����
		
						break;
					case KEY_RIGHT://���Ҽ�
		
						break;
					case KEY_ENTER://�س���
						Tab_Flag = TRUE;
						//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
						line1 = TabMenu[0].startline;
						column1 = TabMenu[0].startcolumn;

						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 4;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					default:	
						break;
			    }
		    }//end else if(TabNum == 0)
		}


	    //�Ĵ�������
	    ApnSet.GprsAPNSet = lu8ASCIIData - 0x30;
		

		//��Χ�޶������顣
		if(ApnSet.GprsAPNSet > SUPPORT_APN_NUM)
			ApnSet.GprsAPNSet = 0;

		ApnSet.WriteFlag = 0xAA55;


//�洢��SD���С�
	OSMutexPend (FlashMutex,0,&err);
	err = SDSaveData("/GprsAPNSelect", &ApnSet, sizeof(ApnSet), 0);
	OSMutexPost (FlashMutex);

	OSTimeDlyHMSM(0,0,2,0);


  	//=============����=============================
	NVIC_SETFAULTMASK();
	NVIC_GenerateSystemReset();




}




/****************************************************************************************************
**	�� ��  �� ��: FreshReplenishPara
**	�� ��  �� ��: ˢ�²����������ʾ��
**	�� ��  �� ��: uint8* InputBuff
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshReplenishPara(uint8 *InputBuff,uint8 *InputBuff1,uint8 *InputBuff2,uint8 *InputBuff3)
{
	uint8 i,line,column;
//	uint8 lu8qian = 0;
//	uint8 lu8bai = 0;
//	uint8 lu8shi = 0;
//	uint8 lu8ge = 0;

	display_white();
	ShowMenuTop();
	ShowMenuBottom(21);
	ShowLineChar_HZ(1,11,&ZCBuChaoBiao[0],6);	//1�У������������á�
	ShowLineChar_HZ(2,0,ZCBiaoBuChaoCiShu,6);	    //2�У����������:
	ShowLineChar_HZ(3,0,ZCBiaoJianGe,8);   //3�У���ʱ����(ms):
	ShowLineChar_HZ(4,0,ZCFaBuChaoCiShu,6);	    //4�У�����������:
	ShowLineChar_HZ(5,0,ZCFaJianGe,8);   //5�У���ʱ����(ms):
	
    ShowPicture(96,22,16,32,&MenuPic_OK[0]);//6�� OK
    
	line = 2;
	column = 30;

	ShowChar(line,column,*InputBuff);

	line = 3;
	column = 31;
	for(i=0;i<4;i++){
		ShowChar(line,column,InputBuff1[i]);
		column += 3;
	}

	line = 4;
	column = 30;

	ShowChar(line,column,*InputBuff2);

	line = 5;
	column = 31;
	for(i=0;i<4;i++){
		ShowChar(line,column,InputBuff3[i]);
		column += 3;
	}
	

}


/****************************************************************************************************
**	�� ��  �� ��: Set_ReplenishPara
**	�� ��  �� ��: �����������ʱ�������á�
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Set_ReplenishPara(void)  
{
	uint8 err = 0;
  	uint8 status;
	uint8 line1;
	uint8 line2;
	uint8 line3;
	uint8 line4;
	uint8 column1;
	uint8 column2;
	uint8 column3;
	uint8 column4;

	int8 i = 0;
	uint8 lu8ASCIIData;
	uint8 lu8ASCIIData1[4];
	uint8 lu8ASCIIData2;
	uint8 lu8ASCIIData3[4];

	SelectInfo TabMenu[] = {{2,30,3},{3,31,12},{4,30,3},{5,31,12},{6,21,12}};//�У��У�����
	uint8 TabNum = 0;
	uint8 MaxTabNum = 4;

	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;

	CPU_SR			cpu_sr;


	ReplenishReadMeterSaveType ReplenishPara;

	OS_ENTER_CRITICAL();
	ReplenishPara = gPARA_ReplenishPara;
	OS_EXIT_CRITICAL();

	line1 = TabMenu[0].startline;
	column1 = TabMenu[0].startcolumn;
	line2 = TabMenu[1].startline;
	column2 = TabMenu[1].startcolumn;
	line3 = TabMenu[2].startline;
	column3 = TabMenu[2].startcolumn;
	line4 = TabMenu[3].startline;
	column4 = TabMenu[3].startcolumn;



	lu8ASCIIData   = HexToAscii(ReplenishPara.MeterReplenishTimes);
	lu8ASCIIData1[0] = HexToAscii(ReplenishPara.MeterInterval / 1000);
	lu8ASCIIData1[1] = HexToAscii((ReplenishPara.MeterInterval%1000) / 100);
	lu8ASCIIData1[2] = HexToAscii((ReplenishPara.MeterInterval%100) / 10);
	lu8ASCIIData1[3] = HexToAscii(ReplenishPara.MeterInterval%10);
	lu8ASCIIData2   = HexToAscii(ReplenishPara.ValveReplenishTimes);
	lu8ASCIIData3[0] = HexToAscii(ReplenishPara.ValveInterval / 1000);
	lu8ASCIIData3[1] = HexToAscii((ReplenishPara.ValveInterval%1000) / 100);
	lu8ASCIIData3[2] = HexToAscii((ReplenishPara.ValveInterval%100) / 10);
	lu8ASCIIData3[3] = HexToAscii(ReplenishPara.ValveInterval%10);
	
   
	while(OK_Flag == FALSE)
	{
		FreshReplenishPara(&lu8ASCIIData,lu8ASCIIData1,&lu8ASCIIData2,lu8ASCIIData3);
		
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len( TabMenu[TabNum].startline , TabMenu[TabNum].startcolumn , TabMenu[TabNum].columnnum ) ;
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)	{
						TabNum = MaxTabNum;
					}
					else{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)	{
						TabNum = 0;
					}
					else{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if( TabNum == MaxTabNum ){
						OK_Flag = TRUE;
					}
					else{
						//Tab_Flag = (!Tab_Flag);
						i = 0;
						Tab_Flag = FALSE;
					}
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 4;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
				default:	
					break;
			}		
		}
		else if(TabNum == 0)
	         {
		       ReverseShowChar(line1,column1);
			   status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			   switch(status)
				{
				   case KEY_UP://���ϼ�
						lu8ASCIIData = lu8ASCIIData + 1;   //����������Χ0-9.
				   		if(lu8ASCIIData > 0x39)
							lu8ASCIIData = 0x30;
						 break;
					case KEY_DOWN://���¼�
						lu8ASCIIData = lu8ASCIIData - 1;
				   		if(lu8ASCIIData < 0x30)
							lu8ASCIIData = 0x39;
						 break;		
					case KEY_LEFT://�����
		
						break;
					case KEY_RIGHT://���Ҽ�
		
						break;
					case KEY_ENTER://�س���
						Tab_Flag = TRUE;
						//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
						i = 0;
						line1 = TabMenu[0].startline;
						column1 = TabMenu[0].startcolumn;
						line2 = TabMenu[1].startline;
						column2 = TabMenu[1].startcolumn;
						line3 = TabMenu[2].startline;
						column3 = TabMenu[2].startcolumn;
						line4 = TabMenu[3].startline;
						column4 = TabMenu[3].startcolumn;


						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 4;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					default:	
						break;
			    }
		    }//end else if(TabNum == 0)
			else if(TabNum == 1)
		         {
			       ReverseShowChar(line2,column2);
				   status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				   switch(status)
					{
					   case KEY_UP://���ϼ�
							if(lu8ASCIIData1[i] == 0x39){
								   lu8ASCIIData1[i] = 0x30;
						    	}
							    else  {
							       lu8ASCIIData1[i] = lu8ASCIIData1[i] + 1;
							    }
						    break;
						case KEY_DOWN://���¼�
							if(lu8ASCIIData1[i] == 0x30){
									lu8ASCIIData1[i] = 0x39;
								}
								else{
									lu8ASCIIData1[i] = lu8ASCIIData1[i] - 1;
								}	
							break;			
						case KEY_LEFT://�����
							if(i > 0){
								i = i - 1;
								column2 = column2 - 3;
							}
							ShowChar(line2,column2,lu8ASCIIData1[i]);
							break;
						case KEY_RIGHT://���Ҽ�
							if(i < 3){
								i = i + 1;
								column2 = column2 + 3;
							}
							ShowChar(line2,column2,lu8ASCIIData1[i]);
							break;
						case KEY_ENTER://�س���
							Tab_Flag = TRUE;
							//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
							i = 0;
							line1 = TabMenu[0].startline;
							column1 = TabMenu[0].startcolumn;
							line2 = TabMenu[1].startline;
							column2 = TabMenu[1].startcolumn;
							line3 = TabMenu[2].startline;
							column3 = TabMenu[2].startcolumn;
							line4 = TabMenu[3].startline;
							column4 = TabMenu[3].startcolumn;

							TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��

							break;
						case KEY_EXIT://���ؼ�
							KeyFuncIndex = 4;
							(*KeyTab[KeyFuncIndex].CurrentOperate)();
							return;			
						default:	
							break;
				    }
			    }
			else if(TabNum == 2)   {
		       ReverseShowChar(line3,column3);
			   status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			   switch(status)
				{
				   case KEY_UP://���ϼ�
						lu8ASCIIData2 = lu8ASCIIData2 + 1;   //����������Χ0-9.
				   		if(lu8ASCIIData2 > 0x39)
							lu8ASCIIData2 = 0x30;
						 break;
					case KEY_DOWN://���¼�
						lu8ASCIIData2 = lu8ASCIIData2 - 1;
				   		if(lu8ASCIIData2 < 0x30)
							lu8ASCIIData2 = 0x39;
						 break;		
					case KEY_LEFT://�����
		
						break;
					case KEY_RIGHT://���Ҽ�
		
						break;
					case KEY_ENTER://�س���
						Tab_Flag = TRUE;
						//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
						i = 0;
						line1 = TabMenu[0].startline;
						column1 = TabMenu[0].startcolumn;
						line2 = TabMenu[1].startline;
						column2 = TabMenu[1].startcolumn;
						line3 = TabMenu[2].startline;
						column3 = TabMenu[2].startcolumn;
						line4 = TabMenu[3].startline;
						column4 = TabMenu[3].startcolumn;


						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 4;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					default:	
						break;
			    }
		    }
			else if(TabNum == 3)
		         {
			       ReverseShowChar(line4,column4);
				   status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				   switch(status)
					{
					   case KEY_UP://���ϼ�
							if(lu8ASCIIData3[i] == 0x39){
								   lu8ASCIIData3[i] = 0x30;
						    	}
							    else  {
							       lu8ASCIIData3[i] = lu8ASCIIData3[i] + 1;
							    }
						    break;
						case KEY_DOWN://���¼�
							if(lu8ASCIIData3[i] == 0x30){
									lu8ASCIIData3[i] = 0x39;
								}
								else{
									lu8ASCIIData3[i] = lu8ASCIIData3[i] - 1;
								}	
							break;			
						case KEY_LEFT://�����
							if(i > 0){
								i = i - 1;
								column4 = column4 - 3;
							}
							ShowChar(line4,column4,lu8ASCIIData3[i]);
							break;
						case KEY_RIGHT://���Ҽ�
							if(i < 3){
								i = i + 1;
								column4 = column4 + 3;
							}
							ShowChar(line4,column4,lu8ASCIIData3[i]);
							break;
						case KEY_ENTER://�س���
							Tab_Flag = TRUE;
							//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
							i = 0;
							line1 = TabMenu[0].startline;
							column1 = TabMenu[0].startcolumn;
							line2 = TabMenu[1].startline;
							column2 = TabMenu[1].startcolumn;
							line3 = TabMenu[2].startline;
							column3 = TabMenu[2].startcolumn;
							line4 = TabMenu[3].startline;
							column4 = TabMenu[3].startcolumn;

							TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��

							break;
						case KEY_EXIT://���ؼ�
							KeyFuncIndex = 4;
							(*KeyTab[KeyFuncIndex].CurrentOperate)();
							return;			
						default:	
							break;
				    }
			    }
	}//end while

	
    //�Ĵ�������
	ReplenishPara.MeterReplenishTimes = lu8ASCIIData - 0x30;
	ReplenishPara.MeterInterval = (lu8ASCIIData1[0] - 0x30) * 1000;
	ReplenishPara.MeterInterval += (lu8ASCIIData1[1] - 0x30) * 100;
	ReplenishPara.MeterInterval += (lu8ASCIIData1[2] - 0x30) * 10;
	ReplenishPara.MeterInterval += lu8ASCIIData1[3] - 0x30;

	ReplenishPara.ValveReplenishTimes = lu8ASCIIData2 - 0x30;
	ReplenishPara.ValveInterval = (lu8ASCIIData3[0] - 0x30) * 1000;
	ReplenishPara.ValveInterval += (lu8ASCIIData3[1] - 0x30) * 100;
	ReplenishPara.ValveInterval += (lu8ASCIIData3[2] - 0x30) * 10;
	ReplenishPara.ValveInterval += lu8ASCIIData3[3] - 0x30;

	//��Χ�޶������顣
	if(ReplenishPara.MeterReplenishTimes > 9)
		ReplenishPara.MeterReplenishTimes = 2;
	if((ReplenishPara.MeterInterval>9999) || (ReplenishPara.MeterInterval<100))
		ReplenishPara.MeterInterval = 200;

	if(ReplenishPara.ValveReplenishTimes > 9)
		ReplenishPara.ValveReplenishTimes = 2;
	if((ReplenishPara.ValveInterval>9999) || (ReplenishPara.ValveInterval<100))
		ReplenishPara.ValveInterval = 500;

	ReplenishPara.WriteFlag = 0xaa55;
	gPARA_ReplenishPara = ReplenishPara;  //��ֵ��ȫ�ֱ�����


//�洢��SD���С�
	OSMutexPend (FlashMutex,0,&err);
	err = SDSaveData("/REPLENISH_PARA", &ReplenishPara, sizeof(ReplenishPara), 0);
	OSMutexPost (FlashMutex);


  	//==========================================
	ShowMenuBottom(2);//���óɹ�

	KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();

	return;

}


/****************************************************************************************************
**	�� ��  �� ��: LCD_Initial
**	�� ��  �� ��: Һ����ʼ�������亯��lcd_initial()�ж�Һ���Աȶȵ�����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void LCD_Initial()
{
	lcd_initial();
    if(gStore_CommPara.LCDContrastValue < 170 || gStore_CommPara.LCDContrastValue > 220)
    {
    	gStore_CommPara.LCDContrastValue = LCD_CON_VALUE;
    }
    
    SetContrast(gStore_CommPara.LCDContrastValue);
    //Set_SelfDefinePara();
    return;
}

/****************************************************************************************************
**	�� ��  �� ��: RegisterMenu
**	�� ��  �� ��: ������ע�����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void RegisterMenu(void)
{
	char path[17]="/2014/01/01/1530";
	display_white();
	ShowMenuTop();
	ShowMenuBottom(0);
	//ShowPicture(32,0,16,160,&RegisterPic[0]);
	ShowLineChar_HZ(3, 11, RegistString, 7);
	ShowSysDate(4,12,DATETYPE_CH);
	ShowSysTime(5,15,1);

	str_check_data.user_id = 01;
	str_check_data.read_num = 01;
	memcpy(str_check_data.time_path, path, 16);
	//test();
}
/****************************************************************************************************
**	�� ��  �� ��:  NoRotate
**	�� ��  �� ��: ��ѭ��
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void NoRotate(void)
{
	display_white();
	ShowMenuTop();

	ShowPicture(64,0,16,160,&RegisterPic[0]);
	ShowSysDate(6,12,DATETYPE_CH);
	ShowSysTime(7,15,1);
}

/****************************************************************************************************
**	�� ��  �� ��: ShowMainMenu
**	�� ��  �� ��: һ���˵��Ķ�������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ShowMainMenu(void)
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(2,3,&MenuHZ11[0],7);//����������鿴
	ShowLineChar_HZ(3,3,&MenuHZ12[0],9);//����״̬�鿴������
	ShowLineChar_HZ(4,3,&MenuHZ13[0],8);//������������ά��
	//ShowLineChar_HZ(5,3,&MenuHZ14[0],7);//ʱ��ͨ�������
	//ShowLineChar_HZ(6,3,&MenuHZ15[0],6);//ѡ���������
//#ifdef DEMO_APP
//	
//	ShowLineChar_HZ(7,3,&MenuHZ16[0],4);//"�ȱ���"�ɼ�������ʾmuxiaoqing 20140718
//#else
//    ShowLineChar_HZ(7,3,&MenuHZ16[0],4);//ά�� ʵ��Ӧ��д�ɳ�������muxiaoqing 20140328
//
//#endif
}
/****************************************************************************************************
**	�� ��  �� ��: MainMenu
**	�� ��  �� ��: Һ�����˵�
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void MainMenu(void)
{
	ShowMainMenu();
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu11
**	�� ��  �� ��: 1.����������鿴
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu11(void)
{
	ShowMainMenu();
	ReverseShowLine(2);
	
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu12
**	�� ��  �� ��: 2.����״̬�鿴
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu12(void)
{
	ShowMainMenu();
	ReverseShowLine(3);
	
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu13
**	�� ��  �� ��: 3.������������ά��
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu13(void)
{
	ShowMainMenu();
	ReverseShowLine(4);
}
/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu14
**	�� ��  �� ��: 4.ʱ��ͨ�������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu14(void)
{
	ShowMainMenu();
	ReverseShowLine(5);
}
/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu15
**	�� ��  �� ��: 5.ѡ���������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu15(void)
{
	ShowMainMenu();
	ReverseShowLine(6);
}
/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu16
**	�� ��  �� ��: 5.��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/

void Action_LcdMenu16(void)
{
	ShowMainMenu();
	ReverseShowLine(7);
}

//�����˵�
/****************************************************************************************************
**	�� ��  �� ��: FreshTimeInputFun
**	�� ��  �� ��: ˢ��ʱ��
**	�� ��  �� ��: uint8* date
**				  uint8* time
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshTimeInputFun(uint8* date,uint8* time)
{
	uint8 line,column;
	line = 2;
	column = 0;	
	CleanScreen(16,0,128,54);
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(1,0,&InputTime[0],8);
	ShowPicture(64,22,16,32,&MenuPic_OK[0]);//OK 4��
		
	ShowIniDate(line,column,&date[0]);
	ShowIniTime(line + 1,column,&time[0]);
}

/****************************************************************************************************
**	�� ��  �� ��: TimeInputFun
**	�� ��  �� ��: ʱ������
**	�� ��  �� ��: uint8* DateBuff
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 TimeInputFun(uint8* DateBuff)
{
	uint8 rt;
	uint8 status,err;
	uint8 line1,line2;
	uint8 column1,column2;
	uint8 i,j;
	uint8 datebuf[6];
	uint8 ASCIIData[12];
	uint8 HexData[6];
	
	SelectInfo TabMenu[] = {{2,0,33},{3,0,33},{4,22,11}};//�У��У�����
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
		
	//����ǰʱ��
	//UserReadDateTime(&datebuf[0]);//���ʱ������
	ReadDateTime(&datebuf[0]);
	datebuf[4] = datebuf[4]&0x1F;//ȥ������
	for(i=0;i<6;i++)
	{
		ASCIIData[2*i] = HexToAscii(datebuf[5-i]>>4);
		ASCIIData[2*i+1] = HexToAscii(datebuf[5-i]&0x0f);
	}
	//��ʾ��ǰʱ��
	FreshTimeInputFun(&ASCIIData[0],&ASCIIData[6]);
	i = 0;
	j = 6;
	line1 = TabMenu[0].startline;//2
	line2 = TabMenu[1].startline;//3
	column1 = TabMenu[0].startcolumn;//0
	column2 = TabMenu[1].startcolumn;//0
	
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
		FreshTimeInputFun(&ASCIIData[0],&ASCIIData[6]);
		
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
						rt = KEY_ENTER;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
					rt = KEY_EXIT;
					return rt;			
					
				default:	
					break;
			}		
		}
		else
		{
			if(TabNum == 0)
			{
				ReverseShowChar(line1,column1);
				status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				switch(status)
				{
					case KEY_UP://���ϼ�
						if(ASCIIData[i] == 0x39)
						{
							ASCIIData[i] = 0x30;
						}
						else
						{
							ASCIIData[i] = ASCIIData[i] + 1;
						}
						break;
					case KEY_DOWN://���¼�
						if(ASCIIData[i] == 0x30)
						{
							ASCIIData[i] = 0x39;
						}
						else
						{
							ASCIIData[i] = ASCIIData[i] - 1;
						}	
						break;			
					case KEY_LEFT://�����
						ShowChar(line1,column1,ASCIIData[i]);
						if(i > 0)
						{
							if(i%2)
							{
								column1 = column1 - 3;
							}
							else
							{
								column1 = column1 - 8;
							}
							i = i - 1;
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line1,column1,ASCIIData[i]);
						if(i < 5)
						{
							if(i%2)
							{
								column1 = column1 + 8;
							}
							else
							{
								column1 = column1 + 3;
							}
							i = i + 1;
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						i = 0;
						j = 6;
						line1 = TabMenu[0].startline;//2
						line2 = TabMenu[1].startline;//3
						column1 = TabMenu[0].startcolumn;//0
						column2 = TabMenu[1].startcolumn;//0					
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						rt = KEY_EXIT;
						return rt;			
					
					default:	
						break;
				}
			}//����������
				
			else if(TabNum == 1)
			{
				ReverseShowChar(line2,column2);
				status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				switch(status)
				{
					case KEY_UP://���ϼ�
						if(ASCIIData[j] == 0x39)
						{
							ASCIIData[j] = 0x30;
						}
						else
						{
							ASCIIData[j] = ASCIIData[j] + 1;
						}
						break;
					case KEY_DOWN://���¼�
						if(ASCIIData[j] == 0x30)
						{
							ASCIIData[j] = 0x39;
						}
						else
						{
							ASCIIData[j] = ASCIIData[j] - 1;
						}	
						break;			
					case KEY_LEFT://�����
						ShowChar(line2,column2,ASCIIData[j]);
						if(j > 6)
						{
							if(j%2)
							{
								column2 = column2 - 3;
							}
							else
							{
								column2 = column2 - 8;
							}
							j = j - 1;
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line2,column2,ASCIIData[j]);
						if(j < 11)
						{
							if(j%2)
							{
								column2 = column2 + 8;
							}
							else
							{
								column2 = column2 + 3;
							}
							j = j + 1;
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						i = 0;
						j = 6;
						line1 = TabMenu[0].startline;//2
						line2 = TabMenu[1].startline;//3
						column1 = TabMenu[0].startcolumn;//0
						column2 = TabMenu[1].startcolumn;//0					
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						rt = KEY_EXIT;
						return rt;			
					
					default:	
						break;
				}				
			}//end if(TabNum == 1) ʱ���� ����
		}//end if(Tab_Flag == TRUE)
	}//end while	
	
	//==========================================
	//�˴�asciiʱ��ת��ΪHEXBCD
	for(i=0;i<6;i++)//��ASC��ת��ΪBCD����������---���˳����
	{
		HexData[5-i] = ((ASCIIData[2*i]-0x30)<<4) + (ASCIIData[2*i+1]-0x30);
	}
	//HexData[4] = HexData[4]|0x20;
	err = TimeCheck(&HexData[0]);
	if(err != 0)//����Ҫ��
	{
		if(err == 1)///���ڲ���Ҫ��
		{
			ShowMenuBottom(7);
		}
		else if(err == 2)//ʱ�䲻�Ϸ�
		{
			ShowMenuBottom(8);
		}
		OSTimeDly(OS_TICKS_PER_SEC*2);
		rt = KEY_EXIT;
	}
	else
	{
		memcpy(DateBuff,HexData,6);
	}
	return rt;
}

/****************************************************************************************************
**	�� ��  �� ��: FreshZigbeeSet
**	�� ��  �� ��: ˢ��zigbee����
**	�� ��  �� ��: uint8* InputBuff
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshZigbeeSet(uint8* InputBuff)
{
	uint8 i,line,column;

	LOG_assert_param( InputBuff == NULL );
		
	CleanScreen(16,0,128,54);
	ShowMenuTop();
	ShowMenuBottom(21);
	ShowLineChar_HZ(1,8,(const uint16 *)&Menu_ConfigType[3][0],10);//1�� zigbee��վ��ַ:
	ShowPicture(48,22,16,32,&MenuPic_OK[0]);//3�� OK
	line = 2;
	column = 20;
	for(i=0;i<4;i++)
	{
		ShowChar(line,column+3*i,InputBuff[i]);
	}	
	return;
}
/****************************************************************************************************
**	�� ��  �� ��: FreshSlectReadMeterChannel
**	�� ��  �� ��: ˢ���ȱ�ͨ������
**	�� ��  �� ��: uint8* InputBuff
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshSlectReadMeterChannel(uint8* InputBuff)
{
	uint8 line,column;

	LOG_assert_param( InputBuff == NULL );
		
	CleanScreen(16,0,128,54);
	ShowMenuTop();
	ShowMenuBottom(21);
	ShowLineChar_HZ(1,8,(const uint16 *)&MenuHZ12_23[0],6);//1�� zigbee��վ��ַ:
	ShowPicture(48,22,16,32,&MenuPic_OK[0]);//3�� OK
	line = 2;
	column = 20;
	
	ShowChar(line,column,InputBuff[0]);
	
	return;
}
/****************************************************************************************************
**	�� ��  �� ��: ZigbeeSet
**	�� ��  �� ��: zigbee��վ��ַ����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ZigbeeSet(void)
{	
	uint8 status;
	uint8 line;
	uint8 column;
	uint8 column1;
	uint8 line1;
	int8 i;
	uint8 ASCIIData[4];
	uint8 ZigbeeAddr[6] = {0};
	uint8 ZigbeeAddrReverse[6] = {0x00};
//	uint8 datalen;
	
	SelectInfo TabMenu[] = {{2,20,12},{3,22,11}};//�У��У�����
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
	CPU_SR			cpu_sr;
	
	OS_ENTER_CRITICAL();
	memcpy(ZigbeeAddrReverse, gPARA_TermPara.HostAddr, 6);
	OS_EXIT_CRITICAL();
	
	//���ڴ洢���ֽ���ǰ���������ڽ��ֽ�˳�����
	for(i=0; i<6; i++)
	{
		ZigbeeAddr[i] = ZigbeeAddrReverse[5-i];
	}
	
	//ȡ����˳���ĺ�2�ֽ���Ч
	for(i=0;i<2;i++)
	{
		ASCIIData[2*i] = HexToAscii(ZigbeeAddr[4+i]>>4);
		ASCIIData[2*i+1] = HexToAscii(ZigbeeAddr[4+i]&0x0f);
	}
	
	i = 0;
	line = TabMenu[0].startline;
	column = TabMenu[0].startcolumn;
	line1 = TabMenu[1].startline;
	column1 = TabMenu[1].startcolumn;
	
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
		FreshZigbeeSet(&ASCIIData[0]);
		
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 4;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}		
		}
		else if(TabNum == 0)
		{
			ReverseShowChar(line,column);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIData[i] == 0x39)
					{
						ASCIIData[i] = 0x30;
					}
					else
					{
						ASCIIData[i] = ASCIIData[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(ASCIIData[i] == 0x30)
					{
						ASCIIData[i] = 0x39;
					}
					else
					{
						ASCIIData[i] = ASCIIData[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line,column,ASCIIData[i]);
					if(i > 0)
					{
						i = i - 1;
						column = column - 3;
					}
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line,column,ASCIIData[i]);
					if(i < 3)
					{
						i = i + 1;
						column = column + 3;
					}
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					line1 = line1;/*��ֹ���뾯��*/
					column1 = TabMenu[1].startcolumn;
					column1 = column1;/*��ֹ���뾯��*/
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 4;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}//end else if(TabNum == 0)

	}//end while
	memset(ZigbeeAddr, 0x00, 6);
	for(i=0;i<2;i++)
	{
		ZigbeeAddr[1-i] =  ((ASCIIData[2*i]-0x30)<<4) + ((ASCIIData[2*i+1]-0x30)&0x0F);
	}
	SetHostAddr(ZigbeeAddr);
	//==========================================
	ShowMenuBottom(2);//���óɹ�

	KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();

	return;
}

// ����������鿴 �¼��˵�
/****************************************************************************************************
**	�� ��  �� ��: ShowMenu_ParaSet
**	�� ��  �� ��: Ŀ¼��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ShowMenu_ParaSet(void)
{
	ShowLineChar_HZ(1,10,&MenuHZ11[0],7);
	ShowLineChar_HZ(2,3,&MenuHZ11_22[2],6);
	ShowLineChar_HZ(3,3,&MenuHZ11_24[2],6);
	ShowLineChar_HZ(4,3,&MenuHZ11_26[2],6);
	ShowLineChar_HZ(5,3,&MenuHZ11_27[2],7);
	ShowLineChar_HZ(6,3,&MenuHZ11_28[2],8);
	ShowLineChar_HZ(7,3,&ZCBuChaoBiao[0],6);
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu11_21
**	�� ��  �� ��: 1.ͨ��ͨ������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu11_21(void) 		
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowMenu_ParaSet();
	ReverseShowLine(2);
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu11_22
**	�� ��  �� ��: 2.�ն�ʱ������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu11_22(void) 			// "2.�ն�ʱ������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowMenu_ParaSet();
	ReverseShowLine(3);
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu11_23
**	�� ��  �� ��: 3.�ն˱������
**	�� ��  �� ��:  none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu11_23(void) 			// "3.�ն˱������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowMenu_ParaSet();
	ReverseShowLine(4);
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu11_24
**	�� ��  �� ��: 4.����ʱ�������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu11_24(void) 			// "4.����ʱ�������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowMenu_ParaSet();
	ReverseShowLine(5);
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu11_24
**	�� ��  �� ��: 5.GPRS��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu11_25(void) 			// "5.GPRS��������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowMenu_ParaSet();
	ReverseShowLine(6);
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu11_24
**	�� ��  �� ��: �������������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu11_26(void) 			// "6.�������������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowMenu_ParaSet();
	ReverseShowLine(7);
}


/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu12_21
**	�� ��  �� ��: 1.�鿴��ǰʱ��ڵ㳭��״̬
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
//�ú�����Ҫ�޸�
void Action_LcdMenu12_21(void) 			// "1.�鿴��ǰʱ��ڵ㳭��״̬"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(1,5,&MenuHZ12[0],9);
	ShowLineChar_HZ(2,3,&MenuHZ12_21[0],6);
	ShowLineChar_HZ(3,3,&MenuHZ12_22[0],4);
	ShowLineChar_HZ(4,3,&MenuHZ12_23[0],6);/*yangfei added*/
	ShowLineChar_HZ(5,3,&MenuHZ12_24[0],6);/*ʧ����Ϣ��ѯ��added by zjjin */
	ReverseShowLine(2);
}
/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu12_22
**	�� ��  �� ��: ��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
//�ú�����Ҫ�޸�
void Action_LcdMenu12_22(void) 			// "1.�鿴��ǰʱ��ڵ㳭��״̬"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(1,5,&MenuHZ12[0],9);
	ShowLineChar_HZ(2,3,&MenuHZ12_21[0],6);
	ShowLineChar_HZ(3,3,&MenuHZ12_22[0],4);
	ShowLineChar_HZ(4,3,&MenuHZ12_23[0],6);/*yangfei added*/
	ShowLineChar_HZ(5,3,&MenuHZ12_24[0],6);/*ʧ����Ϣ��ѯ��added by zjjin */
	ReverseShowLine(3);
}
/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu12_23
**	�� ��  �� ��: ����ͨ������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
//�ú�����Ҫ�޸�
void Action_LcdMenu12_23(void) 			/*����ͨ������*/
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(1,5,&MenuHZ12[0],9);
	ShowLineChar_HZ(2,3,&MenuHZ12_21[0],6);
	ShowLineChar_HZ(3,3,&MenuHZ12_22[0],4);
	ShowLineChar_HZ(4,3,&MenuHZ12_23[0],6);/*yangfei added*/
	ShowLineChar_HZ(5,3,&MenuHZ12_24[0],6);/*ʧ����Ϣ��ѯ��added by zjjin */
	ReverseShowLine(4);
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu12_24
**	�� ��  �� ��: ʧ����Ϣ��ѯ
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
//�ú�����Ҫ�޸�
void Action_LcdMenu12_24(void) 			/*ʧ����Ϣ��ѯ*/
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(1,5,&MenuHZ12[0],9);
	ShowLineChar_HZ(2,3,&MenuHZ12_21[0],6);
	ShowLineChar_HZ(3,3,&MenuHZ12_22[0],4);
	ShowLineChar_HZ(4,3,&MenuHZ12_23[0],6);/*yangfei added*/
	ShowLineChar_HZ(5,3,&MenuHZ12_24[0],6);/*ʧ����Ϣ��ѯ��added by zjjin */
	ReverseShowLine(5);
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu13_21
**	�� ��  �� ��: 1.�����ն�
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
// ������������ά�� �¼��˵�
void Action_LcdMenu13_21(void)  		// "1.�����ն�"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ13[0],8);   //�ն˹�����ά��
    ShowLineChar_HZ(2,2,&MenuHZ13_21[0],4);//�����ն�
	ShowLineChar_HZ(3,2,&MenuHZ13_25[2],6);//�ն˰汾��Ϣ
	ShowLineChar_HZ(4,2,&MenuHZ13_26[2],7);//Һ���Աȶ�
	
	ReverseShowLine(2);	
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu13_22
**	�� ��  �� ��: 2.�ն˰汾��Ϣ
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu13_22(void)  		// "2.�ն˰汾��Ϣ"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ13[0],8);   //�ն˹�����ά��
    ShowLineChar_HZ(2,2,&MenuHZ13_21[0],4);//�����ն�
	ShowLineChar_HZ(3,2,&MenuHZ13_25[2],6);//�ն˰汾��Ϣ
	ShowLineChar_HZ(4,2,&MenuHZ13_26[2],7);//Һ���Աȶ�
	ReverseShowLine(3);	
}

/****************************************************************************************************
**	�� ��  �� ��: Action_LcdMenu13_23
**	�� ��  �� ��: 3.Һ���Աȶ�����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Action_LcdMenu13_23(void)  		// "3.Һ���Աȶ�����"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ13[0],8);   //�ն˹�����ά��
    ShowLineChar_HZ(2,2,&MenuHZ13_21[0],4);//�����ն�
	ShowLineChar_HZ(3,2,&MenuHZ13_25[2],6);//�ն˰汾��Ϣ
	ShowLineChar_HZ(4,2,&MenuHZ13_26[2],7);//Һ���Աȶ�
	ReverseShowLine(4);	
}



//muxiaoqing
void Action_LcdMenu16_21(void)		//"SD ������д��Ͷ����ԱȲ���"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	#if 0
 //   ShowLineChar_HZ(1,2,&MenuHZ16[0],8);   //�ն˹�����ά��
    ShowLineChar_HZ(1,2,&MenuHZ16_21[0],12);//"SD ������д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(2,2,&MenuHZ16_22[0],9);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(3,2,&MenuHZ16_23[0],12);//"GPRS ָ�����"	
    ShowLineChar_HZ(4,2,&MenuHZ16_24[0],15);//"���Դ����������"
	ShowLineChar_HZ(5,2,&MenuHZ16_25[0],14);//"������1�������"
	ShowLineChar_HZ(6,2,&MenuHZ16_26[0],14);//"GPRSͨѶ�������"
	ShowLineChar_HZ(7,2,&MenuHZ16_27[0],16);//"RS485 ͨѶ1�������"
	ShowLineChar_HZ(8,2,&MenuHZ16_28[0],15);//"RS485 ͨѶ2�������"
	#else
	
    ShowLineChar_HZ(1,2,&MenuHZ16_21[0],4);//"��������"
	ShowLineChar_HZ(2,2,&MenuHZ16_22[0],4);//"�Զ�����"
	#endif
	ReverseShowLine(1);	
}

void Action_LcdMenu16_22(void)		//"ʱ��оƬ��д��Ͷ����ԱȲ���"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
   #if 0
	//	 ShowLineChar_HZ(1,2,&MenuHZ16[0],8);	//�ն˹�����ά��
	   ShowLineChar_HZ(1,2,&MenuHZ16_21[0],12);//"SD ������д��Ͷ����ԱȲ���"
	   ShowLineChar_HZ(2,2,&MenuHZ16_22[0],9);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	   ShowLineChar_HZ(3,2,&MenuHZ16_23[0],12);//"GPRS ָ�����"   
	   ShowLineChar_HZ(4,2,&MenuHZ16_24[0],15);//"���Դ����������"
	   ShowLineChar_HZ(5,2,&MenuHZ16_25[0],14);//"������1�������"
	   ShowLineChar_HZ(6,2,&MenuHZ16_26[0],14);//"GPRSͨѶ�������"
	   ShowLineChar_HZ(7,2,&MenuHZ16_27[0],16);//"RS485 ͨѶ1�������"
	   ShowLineChar_HZ(8,2,&MenuHZ16_28[0],15);//"RS485 ͨѶ2�������"
   #else
	   
	   ShowLineChar_HZ(1,2,&MenuHZ16_21[0],4);//"�ֶ�����"
	   ShowLineChar_HZ(2,2,&MenuHZ16_22[0],4);//"�Զ�����"
   #endif
	ReverseShowLine(2); 
}
#if 0
void Action_LcdMenu16_23(void)		//"GPRS ָ�����"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
   // ShowLineChar_HZ(1,2,&MenuHZ16[0],8);   //�ն˹�����ά��
   ShowLineChar_HZ(1,2,&MenuHZ16_21[0],12);//"SD ������д��Ͷ����ԱȲ���"
   ShowLineChar_HZ(2,2,&MenuHZ16_22[0],9);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
   ShowLineChar_HZ(3,2,&MenuHZ16_23[0],12);//"GPRS ָ�����"   
   ShowLineChar_HZ(4,2,&MenuHZ16_24[0],15);//"���Դ����������"
   ShowLineChar_HZ(5,2,&MenuHZ16_25[0],14);//"������1�������"
   ShowLineChar_HZ(6,2,&MenuHZ16_26[0],14);//"GPRSͨѶ�������"
   ShowLineChar_HZ(7,2,&MenuHZ16_27[0],16);//"RS485 ͨѶ1�������"
   ShowLineChar_HZ(8,2,&MenuHZ16_28[0],15);//"RS485 ͨѶ2�������"
	ReverseShowLine(3); 
}

void Action_LcdMenu16_24(void)		//"���Դ����������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
   // ShowLineChar_HZ(1,2,&MenuHZ16[0],8);   //�ն˹�����ά��
   ShowLineChar_HZ(1,2,&MenuHZ16_21[0],12);//"SD ������д��Ͷ����ԱȲ���"
   ShowLineChar_HZ(2,2,&MenuHZ16_22[0],9);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
   ShowLineChar_HZ(3,2,&MenuHZ16_23[0],12);//"GPRS ָ�����"   
   ShowLineChar_HZ(4,2,&MenuHZ16_24[0],15);//"���Դ����������"
   ShowLineChar_HZ(5,2,&MenuHZ16_25[0],14);//"������1�������"
   ShowLineChar_HZ(6,2,&MenuHZ16_26[0],14);//"GPRSͨѶ�������"
   ShowLineChar_HZ(7,2,&MenuHZ16_27[0],16);//"RS485 ͨѶ1�������"
   ShowLineChar_HZ(8,2,&MenuHZ16_28[0],15);//"RS485 ͨѶ2�������"
	ReverseShowLine(4); 
}

void Action_LcdMenu16_25(void)		//"������1�������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
   // ShowLineChar_HZ(1,2,&MenuHZ16[0],8);   //�ն˹�����ά��
   ShowLineChar_HZ(1,2,&MenuHZ16_21[0],12);//"SD ������д��Ͷ����ԱȲ���"
   ShowLineChar_HZ(2,2,&MenuHZ16_22[0],9);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
   ShowLineChar_HZ(3,2,&MenuHZ16_23[0],12);//"GPRS ָ�����"   
   ShowLineChar_HZ(4,2,&MenuHZ16_24[0],15);//"���Դ����������"
   ShowLineChar_HZ(5,2,&MenuHZ16_25[0],14);//"������1�������"
   ShowLineChar_HZ(6,2,&MenuHZ16_26[0],14);//"GPRSͨѶ�������"
   ShowLineChar_HZ(7,2,&MenuHZ16_27[0],16);//"RS485 ͨѶ1�������"
   ShowLineChar_HZ(8,2,&MenuHZ16_28[0],15);//"RS485 ͨѶ2�������"
	ReverseShowLine(5); 
}

void Action_LcdMenu16_26(void)		//"GPRSͨѶ�������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
  //  ShowLineChar_HZ(1,2,&MenuHZ16[0],8);   //�ն˹�����ά��
  ShowLineChar_HZ(1,2,&MenuHZ16_21[0],12);//"SD ������д��Ͷ����ԱȲ���"
  ShowLineChar_HZ(2,2,&MenuHZ16_22[0],9);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
  ShowLineChar_HZ(3,2,&MenuHZ16_23[0],12);//"GPRS ָ�����"   
  ShowLineChar_HZ(4,2,&MenuHZ16_24[0],15);//"���Դ����������"
  ShowLineChar_HZ(5,2,&MenuHZ16_25[0],14);//"������1�������"
  ShowLineChar_HZ(6,2,&MenuHZ16_26[0],14);//"GPRSͨѶ�������"
  ShowLineChar_HZ(7,2,&MenuHZ16_27[0],16);//"RS485 ͨѶ1�������"
  ShowLineChar_HZ(8,2,&MenuHZ16_28[0],15);//"RS485 ͨѶ2�������"
	ReverseShowLine(6); 
}

void Action_LcdMenu16_27(void)		//"RS485 ͨѶ1�������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
   // ShowLineChar_HZ(1,2,&MenuHZ16[0],8);   //�ն˹�����ά��
   ShowLineChar_HZ(1,2,&MenuHZ16_21[0],12);//"SD ������д��Ͷ����ԱȲ���"
   ShowLineChar_HZ(2,2,&MenuHZ16_22[0],9);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
   ShowLineChar_HZ(3,2,&MenuHZ16_23[0],12);//"GPRS ָ�����"   
   ShowLineChar_HZ(4,2,&MenuHZ16_24[0],15);//"���Դ����������"
   ShowLineChar_HZ(5,2,&MenuHZ16_25[0],14);//"������1�������"
   ShowLineChar_HZ(6,2,&MenuHZ16_26[0],14);//"GPRSͨѶ�������"
   ShowLineChar_HZ(7,2,&MenuHZ16_27[0],16);//"RS485 ͨѶ1�������"
   ShowLineChar_HZ(8,2,&MenuHZ16_28[0],15);//"RS485 ͨѶ2�������"
	ReverseShowLine(7); 
}

void Action_LcdMenu16_28(void)		//"RS485 ͨѶ2�������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
  //  ShowLineChar_HZ(1,2,&MenuHZ16[0],8);   //�ն˹�����ά��
  ShowLineChar_HZ(1,2,&MenuHZ16_21[0],12);//"SD ������д��Ͷ����ԱȲ���"
  ShowLineChar_HZ(2,2,&MenuHZ16_22[0],9);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
  ShowLineChar_HZ(3,2,&MenuHZ16_23[0],12);//"GPRS ָ�����"   
  ShowLineChar_HZ(4,2,&MenuHZ16_24[0],15);//"���Դ����������"
  ShowLineChar_HZ(5,2,&MenuHZ16_25[0],14);//"������1�������"
  ShowLineChar_HZ(6,2,&MenuHZ16_26[0],14);//"GPRSͨѶ�������"
  ShowLineChar_HZ(7,2,&MenuHZ16_27[0],16);//"RS485 ͨѶ1�������"
  ShowLineChar_HZ(8,2,&MenuHZ16_28[0],15);//"RS485 ͨѶ2�������"
	ReverseShowLine(8); 
}
#else
#ifdef DEMO_APP
void Action_LcdMenu16_21_31(void)		//"��³��ɼ�"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],2);//"��³"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],3);//"������"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],2);//"���"	
//    ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ReverseShowLine(1);	
}

void Action_LcdMenu16_21_32(void)		//"��������ɼ�"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],2);////"��³"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],3);//"������"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],2);//"���"	
 //   ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ReverseShowLine(2); 
}

void Action_LcdMenu16_21_33(void)		//���ɼ�"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],2);////"��³"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],3);//"������"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],2);//"���"	
 //   ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ReverseShowLine(3); 
}

void Action_LcdMenu16_21_34(void)		//"Ԥ��"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	
	ReverseShowLine(4); 
}

#else
void Action_LcdMenu16_21_31(void)		//"SD ������д��Ͷ����ԱȲ���"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
 //   ShowLineChar_HZ(1,2,&MenuHZ16[0],8);   //�ն˹�����ά��
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],5);//"SD ������д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],4);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],8);//"GPRS ָ�����"	
    ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ShowLineChar_HZ(5,2,&MenuHZ16_21_35[0],8);//"������1�������"
	ShowLineChar_HZ(6,2,&MenuHZ16_21_36[0],10);//"GPRSͨѶ�������"
	ShowLineChar_HZ(7,2,&MenuHZ16_21_37[0],13);//"RS485 ͨѶ1�������"
	ShowLineChar_HZ(8,2,&MenuHZ16_21_38[0],12);//"RS485 ͨѶ2�������"
	ReverseShowLine(1);	
}

void Action_LcdMenu16_21_32(void)		//"ʱ��оƬ��д��Ͷ����ԱȲ���"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],5);//"SD ������д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],4);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],8);//"GPRS ָ�����"	
    ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ShowLineChar_HZ(5,2,&MenuHZ16_21_35[0],8);//"������1�������"
	ShowLineChar_HZ(6,2,&MenuHZ16_21_36[0],10);//"GPRSͨѶ�������"
	ShowLineChar_HZ(7,2,&MenuHZ16_21_37[0],13);//"RS485 ͨѶ1�������"
	ShowLineChar_HZ(8,2,&MenuHZ16_21_38[0],12);//"RS485 ͨѶ2�������"
	ReverseShowLine(2); 
}

void Action_LcdMenu16_21_33(void)		//"GPRS ָ�����"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],5);//"SD ������д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],4);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],8);//"GPRS ָ�����"	
    ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ShowLineChar_HZ(5,2,&MenuHZ16_21_35[0],8);//"������1�������"
	ShowLineChar_HZ(6,2,&MenuHZ16_21_36[0],10);//"GPRSͨѶ�������"
	ShowLineChar_HZ(7,2,&MenuHZ16_21_37[0],13);//"RS485 ͨѶ1�������"
	ShowLineChar_HZ(8,2,&MenuHZ16_21_38[0],12);//"RS485 ͨѶ2�������"
	ReverseShowLine(3); 
}

void Action_LcdMenu16_21_34(void)		//"���Դ����������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],5);//"SD ������д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],4);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],8);//"GPRS ָ�����"	
    ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ShowLineChar_HZ(5,2,&MenuHZ16_21_35[0],8);//"������1�������"
	ShowLineChar_HZ(6,2,&MenuHZ16_21_36[0],10);//"GPRSͨѶ�������"
	ShowLineChar_HZ(7,2,&MenuHZ16_21_37[0],13);//"RS485 ͨѶ1�������"
	ShowLineChar_HZ(8,2,&MenuHZ16_21_38[0],12);//"RS485 ͨѶ2�������"
	ReverseShowLine(4); 
}
void Action_LcdMenu16_21_35(void)		//"������1�������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],5);//"SD ������д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],4);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],8);//"GPRS ָ�����"	
    ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ShowLineChar_HZ(5,2,&MenuHZ16_21_35[0],8);//"������1�������"
	ShowLineChar_HZ(6,2,&MenuHZ16_21_36[0],10);//"GPRSͨѶ�������"
	ShowLineChar_HZ(7,2,&MenuHZ16_21_37[0],13);//"RS485 ͨѶ1�������"
	ShowLineChar_HZ(8,2,&MenuHZ16_21_38[0],12);//"RS485 ͨѶ2�������"
	ReverseShowLine(5); 
}

void Action_LcdMenu16_21_36(void)		//"GPRSͨѶ�������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],5);//"SD ������д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],4);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],8);//"GPRS ָ�����"	
    ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ShowLineChar_HZ(5,2,&MenuHZ16_21_35[0],8);//"������1�������"
	ShowLineChar_HZ(6,2,&MenuHZ16_21_36[0],10);//"GPRSͨѶ�������"
	ShowLineChar_HZ(7,2,&MenuHZ16_21_37[0],13);//"RS485 ͨѶ1�������"
	ShowLineChar_HZ(8,2,&MenuHZ16_21_38[0],12);//"RS485 ͨѶ2�������"
	ReverseShowLine(6); 
}

void Action_LcdMenu16_21_37(void)		//"RS485 ͨѶ1�������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],5);//"SD ������д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],4);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],8);//"GPRS ָ�����"	
    ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ShowLineChar_HZ(5,2,&MenuHZ16_21_35[0],8);//"������1�������"
	ShowLineChar_HZ(6,2,&MenuHZ16_21_36[0],10);//"GPRSͨѶ�������"
	ShowLineChar_HZ(7,2,&MenuHZ16_21_37[0],13);//"RS485 ͨѶ1�������"
	ShowLineChar_HZ(8,2,&MenuHZ16_21_38[0],12);//"RS485 ͨѶ2�������"
	ReverseShowLine(7); 
}

void Action_LcdMenu16_21_38(void)		//"RS485 ͨѶ2�������"
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
    ShowLineChar_HZ(1,2,&MenuHZ16_21_31[0],5);//"SD ������д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(2,2,&MenuHZ16_21_32[0],4);//"ʱ��оƬ��д��Ͷ����ԱȲ���"
	ShowLineChar_HZ(3,2,&MenuHZ16_21_33[0],8);//"GPRS ָ�����"	
    ShowLineChar_HZ(4,2,&MenuHZ16_21_34[0],9);//"���Դ����������"
	ShowLineChar_HZ(5,2,&MenuHZ16_21_35[0],8);//"������1�������"
	ShowLineChar_HZ(6,2,&MenuHZ16_21_36[0],10);//"GPRSͨѶ�������"
	ShowLineChar_HZ(7,2,&MenuHZ16_21_37[0],13);//"RS485 ͨѶ1�������"
	ShowLineChar_HZ(8,2,&MenuHZ16_21_38[0],12);//"RS485 ͨѶ2�������"
	ReverseShowLine(8); 
}
#endif

#endif

//�����˵�
/****************************************************************************************************
**	�� ��  �� ��: ShowMenu_CommuSet
**	�� ��  �� ��: ͨ��ͨ������ �ܲ˵�
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
//ͨ��ͨ������
void ShowMenu_CommuSet(void)	//ͨ��ͨ������ �ܲ˵�
{
	//ShowLineChar_HZ(1,10,&MenuHZ12_21[2],7);
	display_white();
	ShowMenuTop();
	//ShowMenuBottom(1);
	//ShowLineChar_HZ(1,3,&MenuHZ12_21_31[0],10);
	//ShowLineChar_HZ(2,3,&MenuHZ12_21_32[0],10);
	//ShowLineChar_HZ(3,3,&MenuHZ12_21_33[0],8);
	//ShowLineChar_HZ(5,32,&MenuHZ12_21_34[0],4);
	//ShowLineChar_HZ(5,3,&MenuHZ12_21_35[0],10);
	
	ShowLineChar_HZ(4,12,&Menu_ConfigType[0][0],10);
}

/****************************************************************************************************
**	�� ��  �� ��: FreshTimeSet
**	�� ��  �� ��: ˢ��ʱ������ҳ��
**	�� ��  �� ��: uint8* date
**                uint8* time
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
//�ն�ʱ������
void FreshTimeSet(uint8* date,uint8* time)//ˢ��ʱ������ҳ��
{
	uint8 line,column;

	LOG_assert_param( date == NULL );
	LOG_assert_param( time == NULL );

	line = 2;
	column = 0;	
	CleanScreen(16,0,128,54);
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowPicture(16,0,16,160,&MenuPic1_2_2[0]);
	ShowPicture(64,22,16,32,&MenuPic_OK[0]);//OK 4��
		
	ShowIniDate(line,column,&date[0]);
	ShowIniTime(line + 1,column,&time[0]);
}


/****************************************************************************************************
**	�� ��  �� ��: Set_SystemTime
**	�� ��  �� ��: �ն�ʱ������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Set_SystemTime(void)  //�ն�ʱ������
{
	uint8 status,err;
	uint8 line1,line2;
	uint8 column1,column2;
	uint8 i,j;
	uint8 datebuf[6];
	uint8 ASCIIData[12];
	uint8 HexData[6];
	
	SelectInfo TabMenu[] = {{2,0,33},{3,0,33},{4,22,11}};//�У��У�����
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
		
	//����ǰʱ��
	//UserReadDateTime(&datebuf[0]);//���ʱ������
	ReadDateTime(&datebuf[0]);
	/*begin:yangfei added 2013-06-04 for ʱ����Ч���ж� */
	for(i=0;i<6;i++)
		{
	debug("%d ",datebuf[i]);

		}
	if(datebuf[0]>60)
		{
		datebuf[0]=1;
		datebuf[1]=1;
		datebuf[2]=1;
		datebuf[3]=1;
		datebuf[4]=1;
		datebuf[5]=9;
		}
	/*end:yangfei added 2013-06-04 for ʱ����Ч���ж� */
	datebuf[4] = datebuf[4]&0x1F;//ȥ������
	for(i=0;i<6;i++)
	{
		ASCIIData[2*i] = HexToAscii(datebuf[5-i]>>4);
		ASCIIData[2*i+1] = HexToAscii(datebuf[5-i]&0x0f);
	}
	//��ʾ��ǰʱ��
	FreshTimeSet(&ASCIIData[0],&ASCIIData[6]);
	i = 0;
	j = 6;
	line1 = TabMenu[0].startline;//2
	line2 = TabMenu[1].startline;//3
	column1 = TabMenu[0].startcolumn;//0
	column2 = TabMenu[1].startcolumn;//0
	
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
		FreshTimeSet(&ASCIIData[0],&ASCIIData[6]);
		
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 5;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}		
		}
		else
		{
			if(TabNum == 0)
			{
				ReverseShowChar(line1,column1);
				status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				switch(status)
				{
					case KEY_UP://���ϼ�
						if(ASCIIData[i] == 0x39)
						{
							ASCIIData[i] = 0x30;
						}
						else
						{
							ASCIIData[i] = ASCIIData[i] + 1;
						}
						break;
					case KEY_DOWN://���¼�
						if(ASCIIData[i] == 0x30)
						{
							ASCIIData[i] = 0x39;
						}
						else
						{
							ASCIIData[i] = ASCIIData[i] - 1;
						}	
						break;			
					case KEY_LEFT://�����
						ShowChar(line1,column1,ASCIIData[i]);
						if(i > 0)
						{
							if(i%2)
							{
								column1 = column1 - 3;
							}
							else
							{
								column1 = column1 - 8;
							}
							i = i - 1;
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line1,column1,ASCIIData[i]);
						if(i < 5)
						{
							if(i%2)
							{
								column1 = column1 + 8;
							}
							else
							{
								column1 = column1 + 3;
							}
							i = i + 1;
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						i = 0;
						j = 6;
						line1 = TabMenu[0].startline;//2
						line2 = TabMenu[1].startline;//3
						column1 = TabMenu[0].startcolumn;//0
						column2 = TabMenu[1].startcolumn;//0					
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 5;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					
					default:	
						break;
				}
			}//����������
				
			else if(TabNum == 1)
			{
				ReverseShowChar(line2,column2);
				status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				switch(status)
				{
					case KEY_UP://���ϼ�
						if(ASCIIData[j] == 0x39)
						{
							ASCIIData[j] = 0x30;
						}
						else
						{
							ASCIIData[j] = ASCIIData[j] + 1;
						}
						break;
					case KEY_DOWN://���¼�
						if(ASCIIData[j] == 0x30)
						{
							ASCIIData[j] = 0x39;
						}
						else
						{
							ASCIIData[j] = ASCIIData[j] - 1;
						}	
						break;			
					case KEY_LEFT://�����
						ShowChar(line2,column2,ASCIIData[j]);
						if(j > 6)
						{
							if(j%2)
							{
								column2 = column2 - 3;
							}
							else
							{
								column2 = column2 - 8;
							}
							j = j - 1;
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line2,column2,ASCIIData[j]);
						if(j < 11)
						{
							if(j%2)
							{
								column2 = column2 + 8;
							}
							else
							{
								column2 = column2 + 3;
							}
							j = j + 1;
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						i = 0;
						j = 6;
						line1 = TabMenu[0].startline;//2
						line2 = TabMenu[1].startline;//3
						column1 = TabMenu[0].startcolumn;//0
						column2 = TabMenu[1].startcolumn;//0					
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 5;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					
					default:	
						break;
				}				
			}//end if(TabNum == 1) ʱ���� ����
		}//end if(Tab_Flag == TRUE)
	}//end while	
	
	//==========================================
	//�˴���Ҫ��������ʱ�亯����Ȼ����ݷ���ֵ�Ľ���ж��Ƿ����óɹ�
	for(i=0;i<6;i++)//��ASC��ת��ΪBCD����������---���˳����
	{
		HexData[5-i] = ((ASCIIData[2*i]-0x30)<<4) + (ASCIIData[2*i+1]-0x30);
	}
	HexData[4] = HexData[4]|0x20;
	err = TimeCheck(&HexData[0]);
	if(err != 0)//����Ҫ��
	{
		if(err == 1)///���ڲ���Ҫ��
		{
			ShowMenuBottom(7);
		}
		else if(err == 2)//ʱ�䲻�Ϸ�
		{
			ShowMenuBottom(8);
		}
		OSTimeDly(OS_TICKS_PER_SEC*2);
		KeyFuncIndex = 5;
		(*KeyTab[KeyFuncIndex].CurrentOperate)();
		return;
	}
	
	err = UserSetDateTime(&HexData[0]);
	ShowMenuBottom(2);//��Ҫ����ʱ�����ú����ķ���ֵ���ڵײ�״̬����ʾ�����Ƿ�ɹ�
	OSTimeDly(OS_TICKS_PER_SEC*2);
	//===========================================
	
	KeyFuncIndex = 5;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();
	return;
}

/****************************************************************************************************
**	�� ��  �� ��: FreshSetReadMeterTime
**	�� ��  �� ��: ˢ�³���ʱ������ҳ��
**	�� ��  �� ��: uint8 timenum
**                uint8 *pIndex
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshSetReadMeterTime(uint8 *pIndex, uint8 *pTime)//ˢ�³���ʱ������ҳ��
{
	uint8 line,column;
	//uint8 TimeNodeIndex = 0;
	uint8 NodeTempH		= 0;
	uint8 NodeTempL		= 0;
	//uint8 time[6]={0};

	LOG_assert_param( pIndex == NULL );
	LOG_assert_param( pTime == NULL );

	line = 2;
	column = 16;	
	CleanScreen(16,0,128,54);
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(1,10,&MenuHZ11_27[2],7);
	ShowLineChar_HZ(2,9,&MenuHZ_TimeNum[0],9);
	ShowPicture(64,22,16,32,&MenuPic_OK[0]);//OK 4��
		
	NodeTempH = *pIndex++;
	NodeTempL = *pIndex;
	ShowChar(2, 14, NodeTempH);
	ShowChar(2, 17, NodeTempL);
	
	/*
	TimeNodeIndex = ((NodeTempH-0x30)<<4) + ((NodeTempL-0x30)&0x0F);
	if((TimeNodeIndex>0x09) || (TimeNodeIndex==0x00))		TimeNodeIndex = 0x01;
	memcpy(time,&gPARA_TimeNodes[TimeNodeIndex-1],2);
	*/
	ShowTimeMS(line + 1, column, pTime);
}
/****************************************************************************************************
**	�� ��  �� ��: Set_ReadMeterTime
**	�� ��  �� ��: ����ʱ������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Set_ReadMeterTime(void)  //����ʱ������
{
	uint8 status;
	uint8 line1,line2;
	uint8 column1,column2;
	uint8 i,j,k;
	//uint8 datebuf[6];
	uint8 ASCIIData[4];
	uint8 ASCIIData1[2];
	//uint8 HexData[6];
	uint8 TimeNum = 0x00;
	uint8 TimeNodeIndex = 0;
	uint8 timebuf[2] = {0x0};
	
	SelectInfo TabMenu[] = {{2,14,6},{3,16,22},{4,22,11}};//�У��У�����
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
		
	//������ʱ���
	memcpy(timebuf,&gPARA_TimeNodes[TimeNum],2);
	for(i=0;i<2;i++)
	{
		ASCIIData[2*i] = HexToAscii(timebuf[1-i]>>4);
		ASCIIData[2*i+1] = HexToAscii(timebuf[1-i]&0x0f);
	}

	//Ĭ����ʾ��һ����ʱ���
	ASCIIData1[0] = HexToAscii((TimeNum+1)>>4);
	ASCIIData1[1] = HexToAscii((TimeNum+1)&0x0f);
	FreshSetReadMeterTime(ASCIIData1, timebuf);
		
	i = 0;
	j = 0;
	line1 = TabMenu[0].startline;//2
	line2 = TabMenu[1].startline;//3
	column1 = TabMenu[0].startcolumn;//14
	column2 = TabMenu[1].startcolumn;//16
	
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
		FreshSetReadMeterTime(ASCIIData1, timebuf);
		
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 7;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}		
		}
		else
		{
			if(TabNum == 0)
			{
				ReverseShowChar(line1,column1);
				status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				switch(status)
				{
					case KEY_UP://���ϼ�
						if(ASCIIData1[i] == 0x39)
						{
							ASCIIData1[i] = 0x30;
						}
						else
						{
							ASCIIData1[i] = ASCIIData1[i] + 1;
						}
						break;
					case KEY_DOWN://���¼�
						if(ASCIIData1[i] == 0x30)
						{
							ASCIIData1[i] = 0x39;
						}
						else
						{
							ASCIIData1[i] = ASCIIData1[i] - 1;
						}	
						break;			
					case KEY_LEFT://�����
						ShowChar(line1,column1,ASCIIData1[i]);
						if(i > 0)
						{
							column1 = column1 - 3;
							i = i - 1;
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line1,column1,ASCIIData1[i]);
						if(i < 1)
						{
							column1 = column1 + 3;
							i = i + 1;
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						i = 0;
						j = 0;
						line1 = TabMenu[0].startline;//2
						line2 = TabMenu[1].startline;//3
						column1 = TabMenu[0].startcolumn;//0
						column2 = TabMenu[1].startcolumn;//0					
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 19;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					
					default:	
						break;
				}
				
				TimeNodeIndex = ((ASCIIData1[0]-0x30)<<4) + ((ASCIIData1[1]-0x30)&0x0F);
				TimeNodeIndex = BcdToHex(TimeNodeIndex);
				if((TimeNodeIndex>TIME_NODE_NUM_MAX) || (TimeNodeIndex==0x00))		TimeNodeIndex = 0x01;
				memcpy(timebuf,&gPARA_TimeNodes[TimeNodeIndex-1],2);
				
				for(k=0;k<2;k++)
				{
					ASCIIData[2*k] = HexToAscii(timebuf[1-k]>>4);
					ASCIIData[2*k+1] = HexToAscii(timebuf[1-k]&0x0f);
				}
			}
				
			else if(TabNum == 1)
			{
				ReverseShowChar(line2,column2);
				status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				switch(status)
				{
					case KEY_UP://���ϼ�
						if(ASCIIData[j] == 0x39)
						{
							ASCIIData[j] = 0x30;
						}
						else
						{
							ASCIIData[j] = ASCIIData[j] + 1;
						}
						break;
					case KEY_DOWN://���¼�
						if(ASCIIData[j] == 0x30)
						{
							ASCIIData[j] = 0x39;
						}
						else
						{
							ASCIIData[j] = ASCIIData[j] - 1;
						}	
						break;			
					case KEY_LEFT://�����
						ShowChar(line2,column2,ASCIIData[j]);
						if(j > 0)
						{
							if(j%2)
							{
								column2 = column2 - 3;
							}
							else
							{
								column2 = column2 - 8;
							}
							j = j - 1;
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line2,column2,ASCIIData[j]);
						if(j < 3)
						{
							if(j%2)
							{
								column2 = column2 + 8;
							}
							else
							{
								column2 = column2 + 3;
							}
							j = j + 1;
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						i = 0;
						j = 0;
						line1 = TabMenu[0].startline;//2
						line2 = TabMenu[1].startline;//3
						column1 = TabMenu[0].startcolumn;//0
						column2 = TabMenu[1].startcolumn;//0					
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 19;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					
					default:	
						break;
				}
				for(i=0;i<2;i++)//��ASC��ת��ΪBCD��
				{
					timebuf[1-i] = ((ASCIIData[2*i]-0x30)<<4) + (ASCIIData[2*i+1]-0x30);
				}				
			}//end if(TabNum == 1) ʱ���� ����
		}//end if(Tab_Flag == TRUE)
	}//end while	
	
	//==========================================
	//�˴���Ҫ�������ó���ʱ��㺯��
	TimeNum = ((ASCIIData1[0]-0x30)<<4) + (ASCIIData1[1]-0x30);
	TimeNum = BcdToHex(TimeNum);
	for(i=0;i<2;i++)//��ASC��ת��ΪBCD��
	{
		timebuf[1-i] = ((ASCIIData[2*i]-0x30)<<4) + (ASCIIData[2*i+1]-0x30);
	}
	
	if((BcdToHex(timebuf[1])>23)||(BcdToHex(timebuf[0])>59))
	{
	    ShowMenuBottom(8);
	    OSTimeDly(OS_TICKS_PER_SEC*2);
		KeyFuncIndex = 7;
		(*KeyTab[KeyFuncIndex].CurrentOperate)();
		return;
	}
	SetTimeNode(timebuf, TimeNum);
	//err = ����ʱ�����ú���
	ShowMenuBottom(2);//��Ҫ����ʱ�����ú����ķ���ֵ���ڵײ�״̬����ʾ�����Ƿ�ɹ�
	OSTimeDly(OS_TICKS_PER_SEC*2);
	//===========================================
	
	KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();
	return;
}


/****************************************************************************************************
**	�� ��  �� ��: FreshTermAddrSet
**	�� ��  �� ��: ˢ����Ŀ��ַ����
**	�� ��  �� ��: uint8* InputBuff
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: muxiaoqing modify protocal
*****************************************************************************************************/
void FreshTermAddrSet(uint8* InputBuffDevicecode,uint8* InputBuffBuildingcode,uint8* InputBuffAreacode)
{
	uint8 i,j,line,column;
#if 0
    LOG_assert_param( InputBuff == NULL );

#else
LOG_assert_param( InputBuffAreacode == NULL );
#endif
	CleanScreen(16,0,128,54);
	ShowMenuTop();
	ShowMenuBottom(21);
	ShowLineChar_HZ(1,8,&MenuHZ11_26[2],6);		//�ն˱������ 
	
	ShowLineChar_HZ(2,8,&AreaCode[0],3);	//¥��:	//����
	
	ShowLineChar_HZ(3,8,&BuildingCode[0],3);	//��Ԫ:	//¥��
	
	ShowLineChar_HZ(4,8,&DeviceCode[0],3);		//�豸��
	//ShowPicture(48,22,16,32,&MenuPic_OK[0]);//3�� OK
	
	ShowPicture(80,22,16,32,&MenuPic_OK[0]);//5�� OK
	#if 0
	line = 2;
	column = 20;
	for(i=0;i<4;i++)
	{
		ShowChar(line,column+3*i,InputBuff[i]);
	}	
	#endif
	
	line = 2;
	column = 25;
	for(i=0;i<3;i++)
	{ 
	    for(j=0;j<4;j++)
	    	{
	    	    if(i==0)
        		ShowChar(line+i,column+3*j,InputBuffAreacode[j]);
				else if(i==1)				
        		ShowChar(line+i,column+3*j,InputBuffBuildingcode[j]);				
				else if(i==2)				
        		ShowChar(line+i,column+3*j,InputBuffDevicecode[j]);
	    	}
	}	
	return;
}

/****************************************************************************************************
**	�� ��  �� ��: Set_SystemAddr
**	�� ��  �� ��: �ն˱�ŵ�ַ����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��					                          
**  ��   ��   ֵ: ��
**	��		  ע: ĽФ����� ���� ¥�� �豸�� �����ն˱�������Э��
*****************************************************************************************************/
void Set_SystemAddr(void)//�ն˱�ŵ�ַ����
{
	uint8 status;
#if 0	//muxiaoqing modefy	
	uint8 line;
	uint8 column;
	uint8 column1;
	uint8 line1;
	int8 i;
	uint8 ASCIIData[4];
#else
	uint8 line;
	uint8 column;
	uint8 column1;
	uint8 line1;
	
	uint8 column2;
	uint8 line2;
	
	uint8 column3;
	uint8 line3;
	int8 i;

	uint8 ASCIIDataAreacode[4];	
	uint8 ASCIIDataBuildingcode[4];	
	uint8 ASCIIDataDevicecode[4];
#endif	
	uint8 TermAddr[6] = {0};
	uint8 TermAddrReverse[6] = {0x00};
//	uint8 datalen;
#if 0	
	SelectInfo TabMenu[] = {{2,20,12},{3,22,11}};//�У��У�����
#else
    SelectInfo TabMenu[] = {{2,25,13},{3,25,13},{4,25,13},{5,22,11}};//�У��У�����
	
#endif
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
	CPU_SR			cpu_sr;
	
	OS_ENTER_CRITICAL();
	memcpy(TermAddrReverse, gPARA_TermPara.TermAddr, 6);
	OS_EXIT_CRITICAL();
	
	//���ڴ洢���ֽ���ǰ���������ڽ��ֽ�˳�����
	for(i=0; i<6; i++)
	{
		TermAddr[i] = TermAddrReverse[5-i];
	}
#if 0	
	//ȡ����˳���ĺ�2�ֽ���Ч
	for(i=0;i<2;i++)
	{
		ASCIIData[2*i] = HexToAscii(TermAddr[4+i]>>4);
		ASCIIData[2*i+1] = HexToAscii(TermAddr[4+i]&0x0f);
	}
#else
	
//ȡ����˳���ĺ�2�ֽ���Ч
for(i=0;i<2;i++)
{
	ASCIIDataDevicecode[2*i] = HexToAscii(TermAddr[4+i]>>4);
	ASCIIDataDevicecode[2*i+1] = HexToAscii(TermAddr[4+i]&0x0f);
}
for(i=0;i<2;i++)
{
	ASCIIDataBuildingcode[2*i] = HexToAscii(TermAddr[2+i]>>4);
	ASCIIDataBuildingcode[2*i+1] = HexToAscii(TermAddr[2+i]&0x0f);
}
for(i=0;i<2;i++)
{
	ASCIIDataAreacode[2*i] = HexToAscii(TermAddr[0+i]>>4);
	ASCIIDataAreacode[2*i+1] = HexToAscii(TermAddr[0+i]&0x0f);
}

#endif
	i = 0;
	line = TabMenu[0].startline;
	column = TabMenu[0].startcolumn;
	line1 = TabMenu[1].startline;
	column1 = TabMenu[1].startcolumn;
	
	line2 = TabMenu[2].startline;
	column2 = TabMenu[2].startcolumn;
	
	line3 = TabMenu[3].startline;
	column3 = TabMenu[3].startcolumn;
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
	   #if 0
		FreshTermAddrSet(&ASCIIData[0]);
	   #else	   
	   FreshTermAddrSet(&ASCIIDataDevicecode[0],&ASCIIDataBuildingcode[0],&ASCIIDataAreacode[0]);
	   #endif
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 6;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}		
		}
	#if 0
		else if(TabNum == 0)
		{
			ReverseShowChar(line,column);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIData[i] == 0x39)
					{
						ASCIIData[i] = 0x30;
					}
					else
					{
						ASCIIData[i] = ASCIIData[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(ASCIIData[i] == 0x30)
					{
						ASCIIData[i] = 0x39;
					}
					else
					{
						ASCIIData[i] = ASCIIData[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line,column,ASCIIData[i]);
					if(i > 0)
					{
						i = i - 1;
						column = column - 3;
					}
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line,column,ASCIIData[i]);
					if(i < 3)
					{
						i = i + 1;
						column = column + 3;
					}
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					line1 = line1 ;
					column1 = column1;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 6;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}//end else if(TabNum == 0)
		#else
		else if(TabNum == 0)		
		{
			ReverseShowChar(line,column);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIDataAreacode[i] >= 0x39)
					{
						ASCIIDataAreacode[i] = 0x30;
					}
					else
					{
						ASCIIDataAreacode[i] = ASCIIDataAreacode[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIDataDevicecode[i] <= 0x30)||(ASCIIDataDevicecode[i] >0x39))
					{
						ASCIIDataAreacode[i] = 0x39;
					}
					else
					{
						ASCIIDataAreacode[i] = ASCIIDataAreacode[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line,column,ASCIIDataAreacode[i]);
					if(i > 0)
					{
						i = i - 1;
						column = column - 3;
					}
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line,column,ASCIIDataAreacode[i]);
					if(i < 3)
					{
						i = i + 1;
						column = column + 3;
					}
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 6;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}
		
		else if(TabNum == 1)		
		{
			ReverseShowChar(line1,column1);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIDataBuildingcode[i] >= 0x39)
					{
						ASCIIDataBuildingcode[i] = 0x30;
					}
					else
					{
						ASCIIDataBuildingcode[i] = ASCIIDataBuildingcode[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIDataDevicecode[i] <= 0x30)||(ASCIIDataDevicecode[i] >0x39))
					{
						ASCIIDataBuildingcode[i] = 0x39;
					}
					else
					{
						ASCIIDataBuildingcode[i] = ASCIIDataBuildingcode[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line1,column1,ASCIIDataBuildingcode[i]);
					if(i > 0)
					{
						i = i - 1;
						column1 = column1 - 3;
					}
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line1,column1,ASCIIDataBuildingcode[i]);
					if(i < 3)
					{
						i = i + 1;
						column1 = column1 + 3;
					}
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 6;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}
		
		else if(TabNum == 2)		
		{
			ReverseShowChar(line2,column2);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIDataDevicecode[i] >= 0x39)
					{
						ASCIIDataDevicecode[i] = 0x30;
					}
					else
					{
						ASCIIDataDevicecode[i] = ASCIIDataDevicecode[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIDataDevicecode[i] <= 0x30)||(ASCIIDataDevicecode[i] >0x39))
					{
						ASCIIDataDevicecode[i] = 0x39;
					}
					else
					{
						ASCIIDataDevicecode[i] = ASCIIDataDevicecode[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line2,column2,ASCIIDataDevicecode[i]);
					if(i > 0)
					{
						i = i - 1;
						column2 = column2 - 3;
					}
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line2,column2,ASCIIDataDevicecode[i]);
					if(i < 3)
					{
						i = i + 1;
						column2 = column2 + 3;
					}
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 6;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}
		#endif

	}//end while

	memset(TermAddr, 0x00, 6); //
#if 0
	for(i=0;i<2;i++)
	{
		TermAddr[1-i] =  ((ASCIIData[2*i]-0x30)<<4) + ((ASCIIData[2*i+1]-0x30)&0x0F);
	}
#else
for(i=0;i<2;i++)
{
	TermAddr[1-i] =  ((ASCIIDataDevicecode[2*i]-0x30)<<4) + ((ASCIIDataDevicecode[2*i+1]-0x30)&0x0F);
}
for(i=0;i<2;i++)
{
	TermAddr[3-i] =  ((ASCIIDataBuildingcode[2*i]-0x30)<<4) + ((ASCIIDataBuildingcode[2*i+1]-0x30)&0x0F);
}
for(i=0;i<2;i++)
{
	TermAddr[5-i] =  ((ASCIIDataAreacode[2*i]-0x30)<<4) + ((ASCIIDataAreacode[2*i+1]-0x30)&0x0F);
}

#endif
	SetTermAddr(TermAddr); //
	//==========================================
	ShowMenuBottom(2);//���óɹ�

	KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();

	return;
}

/****************************************************************************************************
**	�� ��  �� ��: MeterDataShow
**	�� ��  �� ��: ��ʾ������Ϣ
**	�� ��  �� ��: 
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void MeterDataShow(uint16 nowNum,uint16 allNum,uint8 *DataBuf)
{
	uint8 i = 0;
	uint8 lu8line = 0;
	uint8 lu8column = 0;
	uint16 lu16qian = 0;//ǧλ��
	uint16 lu16bai = 0; //��λ��
	uint16 lu16shi = 0;
	uint16 lu16ge = 0;
	uint16 lu16tmp = 0;
	uint8 lu8tmp = 0;
	uint8 lu8tmpArray[7] = {0};
	uint8 lu8DisplayFlag = 0;//���ڱ���Ƿ���ʾ0.(һ������ǰ���0������ʾ��)

	if(nowNum > 999)   //��ֹ���ޣ���������������������600. �˺�������3λ��������
		nowNum = 999;
	if(allNum > 999)
		allNum = 999;


//begin: ��ʾ��ǰ���ȡ�
	lu16bai = nowNum / 100;
	lu16shi = (nowNum - lu16bai*100) / 10;
	lu16ge = nowNum % 10;

	lu8line = 1;   //�ӵ�1�е�0�п�ʼ��
	lu8column = 0;

	if(lu16bai){
		ShowChar(lu8line,lu8column,(lu16bai+0x30));
		lu8column += 3; //ÿ��ASCII��ռ3�С�
	}
	

	if(lu16shi || lu16bai){
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3; 
	}
	

	ShowChar(lu8line,lu8column,(lu16ge+0x30));
	lu8column += 3;

	ShowChar(lu8line,lu8column,0x2f);  //��ʾб�ߡ�
	lu8column += 3;

	lu16bai = allNum / 100;
	lu16shi = (allNum - lu16bai*100) / 10;
	lu16ge = allNum % 10;

	if(lu16bai){
		ShowChar(lu8line,lu8column,(lu16bai+0x30));
		lu8column += 3; 
	}

	if(lu16shi || lu16bai){
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3; 
	}

	ShowChar(lu8line,lu8column,(lu16ge+0x30));
	lu8column += 3;

//end: ��ʾ��ǰ���ȡ�

//begin: ��ʾס����Ϣ��
	lu8line = 1;   //�ӵ�1�е�0�п�ʼ��
	lu8column = 34;
	ShowChar(lu8line,lu8column,(*(DataBuf+11)+0x30));  //��ʾ��Ԫ�š�
	lu8column += 3;
	ShowChar(lu8line,lu8column,0x2d);  //��ʾ�̺��ߡ�
	lu8column += 3;

    lu16tmp = *((uint16 *)(DataBuf+12));  //ȡ��������Ϣ��
    if(lu16tmp > 9999)   //���֧��4λ���š�
		lu16tmp = 9999;
	
    lu16qian = lu16tmp / 1000;
	lu16bai = (lu16tmp % 1000) / 100;
	lu16shi = (lu16tmp % 100) / 10;
	lu16ge = lu16tmp % 10;

	if(lu16qian){
		ShowChar(lu8line,lu8column,(lu16qian+0x30));
		lu8column += 3;
	}
	if(lu16bai || lu16qian){
		ShowChar(lu8line,lu8column,(lu16bai+0x30));
		lu8column += 3;
	}
	if(lu16shi || lu16bai || lu16qian){
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3;
	}
	ShowChar(lu8line,lu8column,(lu16ge+0x30));
	lu8column += 3;
//end: ��ʾס����Ϣ��

//begin: ��ʾ��ǰ���ID��"�������"
	lu8line = 2;   //�ӵ�2�е�0�п�ʼ��
	lu8column = 0;
	ShowLineChar_HZ(lu8line,lu8column,ZCJiLiangDian,4);
	lu8column += 18;

	memcpy(lu8tmpArray,DataBuf,2); //�ҵ��������

	lu16tmp = *((uint16 *)lu8tmpArray);

	lu16bai = lu16tmp / 100;
	lu16shi = (lu16tmp % 100) / 10;
	lu16ge = lu16tmp % 10;
	
	if(lu16bai){
		ShowChar(lu8line,lu8column,(lu16bai+0x30));
		lu8column += 3; //ÿ��ASCII��ռ3�С�
	}
	
	if(lu16shi || lu16bai){
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3; 
	}
	
	ShowChar(lu8line,lu8column,(lu16ge+0x30));
	lu8column += 3;
	
//end: ��ʾ��ǰ���ID��"�������"


//begin:��ʾ�ȱ�š�
	lu8line = 3;   //�ӵ�3�е�0�п�ʼ��
	lu8column = 0;
	ShowLineChar_HZ(lu8line,lu8column,ZCReBiaoHao,4);
	lu8column += 18;
	
	memcpy(lu8tmpArray,(DataBuf+3),7); //�ҵ��ȱ��š�
	for(i=0;i<4;i++){
		lu16tmp = lu8tmpArray[3-i] >> 4;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		
		lu16tmp = lu8tmpArray[3-i] & 0x0f;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
	}

//end:��ʾ�ȱ�š�

//begin:��ʾ�ۼ�������
	lu8DisplayFlag = 0;
	lu8line = 4;   //�ӵ�4�е�0�п�ʼ��
	lu8column = 0;
	ShowLineChar_HZ(lu8line,lu8column,ZCZongReLiang,4);
	lu8column += 18;

	lu8tmp = *(DataBuf + 17);  //�ж��ȱ����ݳ����ж��Ƿ񳭱�ɹ����粻�ɹ���ʾȫ0.
	if(lu8tmp > 40)  //��ǰ�������ɹ����ȱ����ݳ���Ϊ43���˴��ж�40.
		memcpy(lu8tmpArray,(DataBuf+23),5); //�ҵ��ۼ�������
	else
		memset(lu8tmpArray,0,7); 
		
	for(i=1;i<4;i++){   //�ۼ�����ֻ��ʾ�������֡�
		lu16tmp = lu8tmpArray[4-i] >> 4;
		if(lu8DisplayFlag == 0){
			if(lu16tmp != 0){
				lu8DisplayFlag = 1; 
				ShowChar(lu8line,lu8column,(lu16tmp+0x30));
				lu8column += 3;
			}

		}
		else{
			ShowChar(lu8line,lu8column,(lu16tmp+0x30));
			lu8column += 3;
		}

		lu16tmp = lu8tmpArray[4-i] & 0x0f;
		if(lu8DisplayFlag == 0){
			if((lu16tmp != 0) || (i==3)){  //��������һ�����֣�0ҲҪ��ʾ��
				lu8DisplayFlag = 1; 
				ShowChar(lu8line,lu8column,(lu16tmp+0x30));
				lu8column += 3;
			}

		}
		else{
			ShowChar(lu8line,lu8column,(lu16tmp+0x30));
			lu8column += 3;
		}

	}
	if(lu8tmpArray[4] == 0x08)
		ShowLineChar_HZ(lu8line,lu8column,ZCMWh,3);
	else
		ShowLineChar_HZ(lu8line,lu8column,ZCkWh,3);
//end:��ʾ�ۼ�������

//begin:��ʾ�ۼ�������
		lu8DisplayFlag = 0;
		lu8line = 5;   //�ӵ�5�е�0�п�ʼ��
		lu8column = 0;
		ShowLineChar_HZ(lu8line,lu8column,ZCZongLiuLiang,4);
		lu8column += 18;
	
		lu8tmp = *(DataBuf + 17);  //�ж��ȱ����ݳ����ж��Ƿ񳭱�ɹ����粻�ɹ���ʾȫ0.
		if(lu8tmp > 40)  //��ǰ�������ɹ����ȱ����ݳ���Ϊ43���˴��ж�40.
			memcpy(lu8tmpArray,(DataBuf+38),5); //�ҵ��ۼ�������
		else
			memset(lu8tmpArray,0,7); 
			
		for(i=1;i<4;i++){	//�ۼ�����ֻ��ʾ�������֡�
			lu16tmp = lu8tmpArray[4-i] >> 4;
			if(lu8DisplayFlag == 0){
				if(lu16tmp != 0){
					lu8DisplayFlag = 1; 
					ShowChar(lu8line,lu8column,(lu16tmp+0x30));
					lu8column += 3;
				}
			
			}
			else{
				ShowChar(lu8line,lu8column,(lu16tmp+0x30));
				lu8column += 3;
			}

			lu16tmp = lu8tmpArray[4-i] & 0x0f;
			if(lu8DisplayFlag == 0){
				if((lu16tmp != 0) || (i==3)){  //��������һ�����֣�0ҲҪ��ʾ��
					lu8DisplayFlag = 1; 
					ShowChar(lu8line,lu8column,(lu16tmp+0x30));
					lu8column += 3;
				}
			
			}
			else{
				ShowChar(lu8line,lu8column,(lu16tmp+0x30));
				lu8column += 3;
			}

		}
		ShowChar(lu8line,lu8column,0x2e);  //��ʾС����
		lu8column += 3;
		lu16tmp = lu8tmpArray[0] >> 4;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		ShowHZ( lu8line, lu8column,0x01a7 );  //��ʾm3��
//end:��ʾ�ۼ�������

//begin:��ʾ����ˮ�¶ȡ�
		lu8line = 6;   //�ӵ�6�е�0�п�ʼ��
		lu8column = 0;
		ShowHZ(lu8line,lu8column,0x013d);  //��ʾ塰�"��"
		lu8column += 5;
		
		lu8tmp = *(DataBuf + 17);  //�ж��ȱ����ݳ����ж��Ƿ񳭱�ɹ����粻�ɹ���ʾȫ0.
		if(lu8tmp > 40)  //��ǰ�������ɹ����ȱ����ݳ���Ϊ43���˴��ж�40.
			memcpy(lu8tmpArray,(DataBuf+43),6); //�ҵ�����ˮ�¶ȡ�
		else
			memset(lu8tmpArray,0,7); 
				
		lu16tmp = lu8tmpArray[1] >> 4;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		lu16tmp = lu8tmpArray[1] & 0x0f;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		ShowChar(lu8line,lu8column,0x2e);  //��ʾС����
		lu8column += 3;
		lu16tmp = lu8tmpArray[0] >> 4;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		ShowHZ(lu8line,lu8column,0x01a5);  //��ʾ��
		lu8column += 5;
		lu8column += 7;  //����һ���ֿ���ʾ��
		
		ShowHZ(lu8line,lu8column,0x0123);  //��ʾ塰�"��"
		lu8column += 5;
		lu16tmp = lu8tmpArray[4] >> 4;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		lu16tmp = lu8tmpArray[4] & 0x0f;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		ShowChar(lu8line,lu8column,0x2e);  //��ʾС����
		lu8column += 3;
		lu16tmp = lu8tmpArray[3] >> 4;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		ShowHZ(lu8line,lu8column,0x01a5);  //��ʾ��
		lu8column += 5;

//end:��ʾ����ˮ�¶ȡ�

//begin:��ʾ���١�
		lu8DisplayFlag = 0;
		lu8line = 7;   //�ӵ�7�е�0�п�ʼ��
		lu8column = 0;
		ShowLineChar_HZ(lu8line,lu8column,ZCLiuSu,3);
		lu8column += 13;
		
		lu8tmp = *(DataBuf + 17);  //�ж��ȱ����ݳ����ж��Ƿ񳭱�ɹ����粻�ɹ���ʾȫ0.
		if(lu8tmp > 40)  //��ǰ�������ɹ����ȱ����ݳ���Ϊ43���˴��ж�40.
			memcpy(lu8tmpArray,(DataBuf+33),5); //�ҵ��ۼ�������
		else
			memset(lu8tmpArray,0,7); 
				
		for(i=0;i<2;i++){	
			lu16tmp = lu8tmpArray[3-i] >> 4;
			if(lu8DisplayFlag == 0){
				if(lu16tmp != 0){
					lu8DisplayFlag = 1; 
					ShowChar(lu8line,lu8column,(lu16tmp+0x30));
					lu8column += 3;
				}
			
			}
			else{
				ShowChar(lu8line,lu8column,(lu16tmp+0x30));
				lu8column += 3;
			}

			lu16tmp = lu8tmpArray[3-i] & 0x0f;
			if(lu8DisplayFlag == 0){
				if((lu16tmp != 0) || (i==1)){  //��������һ�����֣�0ҲҪ��ʾ��
					lu8DisplayFlag = 1; 
					ShowChar(lu8line,lu8column,(lu16tmp+0x30));
					lu8column += 3;
				}
			
			}
			else{
				ShowChar(lu8line,lu8column,(lu16tmp+0x30));
				lu8column += 3;
			}

		}
		ShowChar(lu8line,lu8column,0x2e);  //��ʾС����
		lu8column += 3;

		lu16tmp = lu8tmpArray[1] >> 4;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		lu16tmp = lu8tmpArray[1] & 0x0f;
		ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		lu8column += 3;
		ShowLineChar_HZ(lu8line,lu8column,ZCLiFangMiperH,3);
//end:��ʾ���١�

//begin:��ʾ�������ݡ�
	lu8line = 8;   //�ӵ�8�е�0�п�ʼ��
	lu8column = 0;
	ShowLineChar_HZ(lu8line,lu8column,ZCShiWen,2);
	lu8column += 10;

	lu8tmp = *(DataBuf + 17);  //�ж��ȱ����ݳ����ж��Ƿ񳭱�ɹ����粻�ɹ���ʾȫ0.
	memcpy(lu8tmpArray,(DataBuf+17+lu8tmp+1+3),6); 

	if(lu8tmpArray[1] == 0xee){
		ShowChar(lu8line,lu8column,0x45);  //�������ʧ������ʾEE.
		lu8column += 3;
		ShowChar(lu8line,lu8column,0x45);
		lu8column += 3;
	}
	else{
		lu16shi = lu8tmpArray[1] >> 4;    //���´�����ʾ
		lu16ge = lu8tmpArray[1] & 0x0f;
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3;
		ShowChar(lu8line,lu8column,(lu16ge+0x30));
		lu8column += 3;
	}
	ShowHZ(lu8line,lu8column,0x01a5);  //��ʾ��
	lu8column += 5;

	lu8column += 6; //��6�С�

	ShowLineChar_HZ(lu8line,lu8column,ZCFaKai,2);
	lu8column += 10;
	if(lu8tmpArray[3] == 0xee){
		ShowChar(lu8line,lu8column,0x45);  //�������ʧ������ʾEE.
		lu8column += 3;
		ShowChar(lu8line,lu8column,0x45);
		lu8column += 3;
	}
	else{
		lu16tmp = lu8tmpArray[3] & 0x0f;  //�Է����ȵ�һ�����⴦����ʾ�ٷֱȡ�
		lu16tmp = 9 - lu16tmp;
		lu16tmp = lu16tmp * 25; //ʵ����*100/4.
		if(lu16tmp == 100)  //ֻ����ʾ2λ���֡�
			lu16tmp = 99;
		else if(lu16tmp > 100)  //����100˵��������ʾ0.
			lu16tmp = 0;

		lu16shi = lu16tmp / 10;    //���´�����ʾ
		lu16ge = lu16tmp % 10;
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3;
		ShowChar(lu8line,lu8column,(lu16ge+0x30));
		lu8column += 3;
	}
	ShowChar(lu8line,lu8column,0x25);
	lu8column += 3;

	
//end:��ʾ�������ݡ�





	

}


/****************************************************************************************************
**	�� ��  �� ��: ReadMeterStateShow
**	�� ��  �� ��: ��ȡ������״̬
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ReadMeterStateShow(void)
{
	uint8 Err = 0;
	uint8 Res = 0;
	uint8 status;
//	uint8 i;
//	uint8 j = 0;
	uint8 line, column;
//	uint8 len;
//	uint8 DateTemp[6] = {0x00};
//	uint8 ASCIIData[6] = {0x00};
	uint16 lu16ReadCmpNum = 0;
	uint16 lu16MeterNum = 0;  //��¼��������
	uint16 lu16DisplayNum = 0;  //�����ѯ��¼��ʾ�ڼ�����
	uint8 lu8UpdataFlag = 1;   //�����ڳ���ʱ��ʾ����Ϣ���˱������ڿ����Ƿ������ʾ��
	CPU_SR		cpu_sr;

	char  DataPath[]="/2012/12/24/1530";
	char  NodePath[]="/2012/12/24/timenode";
	uint8 lu8BCDTime[6] = {0};
	uint8 lu8HexTime[6] = {0};
	DataStoreParaType	History_Para;
	
	line 	= 1;
	column 	= 0;

	
  while(1)
  {
	ShowMenuTop();
	ShowMenuBottom(1);
	
	if(1 == gu8ReadAllMeterFlag){   //�������ȫ���������ԡ�
	  	display_white();
		ShowMenuTop();
		ShowMenuBottom(1);
		MeterDataShow(gu16ReadMeterSuccessNum,gu16ReadMeterNum,gu8ReadDataStore);   //������ʾ��

		lu8UpdataFlag = 1;
	}
	else{	//�������ȫ������ɲ�ѯ��
		if(lu8UpdataFlag){  //lu8UpdataFlag��ֹ�ڲ���Ҫ���µ�ʱ���SD����
			ReadDateTime(lu8BCDTime);
			BcdTimeToHexTime(lu8BCDTime,lu8HexTime);
			GetFilePath(NodePath, lu8HexTime, ARRAY_DAY);  /*��ȡ�̶���timenode path*/
			OSMutexPend (FlashMutex, 0, &Err);
			Res = SDReadData(NodePath, (uint8 *)&History_Para, sizeof(History_Para),0);
			OSMutexPost (FlashMutex);
	
			if(Res == NO_ERR){
				//GetTimeNodeFilePath(DataPath,lu8HexTime,History_Para.TimeNode);
				GetTimeNodeFilePath_Forward(DataPath,lu8HexTime,&History_Para);  //���Ҹ���ʱ���ǰһ����ʱ�����ݴ��·����
				lu16MeterNum = History_Para.MeterNums;
			
				OSMutexPend (FlashMutex, 0, &Err);
				Res = SDReadData(DataPath, gu8ReadDataStore, sizeof(gu8ReadDataStore),lu16DisplayNum*128 + 1); //+1��Ϊ�˴����ݳ����ֽڡ�
				OSMutexPost (FlashMutex);

				display_white();
				ShowMenuTop();
				ShowMenuBottom(1);
				lu8UpdataFlag = 0; //���������ʾ��ǣ���ֹƵ����Flash��
				
				if(Res == NO_ERR)	
					MeterDataShow(lu16DisplayNum+1,lu16MeterNum,gu8ReadDataStore);  //������ʾ��+1��Ϊ�˴�1��ʼ��ʾ��
				else
					ShowLineChar_HZ(5,0,ZCNoNodeData,8);//��ʾû�е�ǰʱ�����ݡ�
				

			}
			else{
				display_white();
				ShowMenuTop();
				ShowMenuBottom(1);
				lu8UpdataFlag = 0; //���������ʾ��ǣ���ֹƵ����Flash��
				ShowLineChar_HZ(4,0,ZCNoSetNode,8);//��ʾû�����ö�ʱ����ʱ��
				ShowLineChar_HZ(5,0,ZCThenRe,5);//��ʾȻ�����³���

			}
			
		}
	
		

	}


	status = GetKeyStatuschaobiao(); //��ȡ��ֵ
   	if( status == KEY_EXIT ){	
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
			//KeyFuncIndex = 2;
		(*KeyTab[KeyFuncIndex].CurrentOperate)();
			
			lu8UpdataFlag = 1;  //���ٴν���ʱʹ��ˢ����
			lu16DisplayNum = 0;
		break;
	}
	else if(status == KEY_UP){   //�����ڳ�ȫ��ʱ�����°��������л���ʾ�������ݡ�
		if(0 == gu8ReadAllMeterFlag){
			if(lu16DisplayNum == 0)
				lu16DisplayNum = lu16MeterNum - 1;
			else
				lu16DisplayNum -= 1;

			lu8UpdataFlag = 1; //��Ļˢ��ʹ�ܡ�
		}

	}
	else if(status == KEY_DOWN){
		if(0 == gu8ReadAllMeterFlag){
			if(lu16DisplayNum >= lu16MeterNum-1)
				lu16DisplayNum = 0;
			else
				lu16DisplayNum += 1;

			lu8UpdataFlag = 1; //��Ļˢ��ʹ�ܡ�

		}

	}

	
  }
	 
}
/****************************************************************************************************
**	�� ��  �� ��: ReadMeterNow
**	�� ��  �� ��: ��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ReadMeterNow(void)
{
		uint8 SendMsg[2]		= {0x00};

		if((gu8ReadAllMeterFlag!=0) && (gu8ReadAllMeterFlag!=1)){  //��ֹ����
			gu8ReadAllMeterFlag = 0;
		}
		
		if(gu8ReadAllMeterFlag == 0){//����ȫ������ִ�С�
			SendMsg[0] =  MSG_1D;
			OSQPost(CMD_Q, (void*)SendMsg);
		}
		
		ReadMeterStateShow();
}


/****************************************************************************************************
**	�� ��  �� ��: MeterFailInfoShow
**	�� ��  �� ��:��ʾ��ʧ�ܱ�ľ�����Ϣ��
**	�� ��  �� ��: 
**	�� ��  �� ��: MeterOrValve  ѡ����ʾ�ȱ�ʧ����Ϣ�򷧿�ʧ����Ϣ
							1-Meter,0-Valve.
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void MeterFailInfoShow(uint16 nowNum,uint16 allNum,MeterFileType *p_mf,uint8 MeterOrValve)
{
	uint8 i = 0;
	uint8 lu8line = 0;
	uint8 lu8column = 0;
	uint16 lu16qian = 0;//ǧλ��
	uint16 lu16bai = 0; //��λ��
	uint16 lu16shi = 0;
	uint16 lu16ge = 0;
	uint16 lu16tmp = 0;
	uint8 lu8tmp = 0;
	uint8 lu8tmpArray[7] = {0};
	
	if(nowNum > 999)   //��ֹ���ޣ���������������������600. �˺�������3λ��������
		nowNum = 999;
	if(allNum > 999)
		allNum = 999;
	
	//begin:��ʾ�ڼ���/������  ʧ�ܡ�
	lu16bai = nowNum / 100;
	lu16shi = (nowNum % 100) / 10;
	lu16ge = nowNum % 10;

	lu8line = 1;   //�ӵ�1�е�0�п�ʼ��
	lu8column = 0;

	if(lu16bai){
		ShowChar(lu8line,lu8column,(lu16bai+0x30));
		lu8column += 3; //ÿ��ASCII��ռ3�С�
	}

	if(lu16shi || lu16bai){
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3; 
	}

	ShowChar(lu8line,lu8column,(lu16ge+0x30));
	lu8column += 3;
	
	ShowChar(lu8line,lu8column,0x2f);  //��ʾб�ߡ�
	lu8column += 3;

	lu16bai = allNum / 100;
	lu16shi = (allNum - lu16bai*100) / 10;
	lu16ge = allNum % 10;

	if(lu16bai){
		ShowChar(lu8line,lu8column,(lu16bai+0x30));
		lu8column += 3; 
	}

	if(lu16shi || lu16bai){
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3; 
	}

	ShowChar(lu8line,lu8column,(lu16ge+0x30));
	lu8column += 3;
//end:��ʾ�ڼ���/������  ʧ�ܡ�
	
//begin: ��ʾ��ǰ���ID��"�������"
	lu8line = 2;   //�ӵ�2�е�0�п�ʼ��
	lu8column = 0;
	ShowLineChar_HZ(lu8line,lu8column,ZCJiLiangDian,4);
	lu8column += 18;

	lu16tmp = p_mf->MeterID;

	lu16bai = lu16tmp / 100;
	lu16shi = (lu16tmp % 100) / 10;
	lu16ge = lu16tmp % 10;
	
	if(lu16bai){
		ShowChar(lu8line,lu8column,(lu16bai+0x30));
		lu8column += 3; //ÿ��ASCII��ռ3�С�
	}
	
	if(lu16shi || lu16bai){
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3; 
	}
	
	ShowChar(lu8line,lu8column,(lu16ge+0x30));
	lu8column += 3;
	
//end: ��ʾ��ǰ���ID��"�������"


//begin:��ʾ�ȱ�Ż򷧿ر��
	lu8line = 3;   //�ӵ�3�е�0�п�ʼ��
	lu8column = 0;

	if(MeterOrValve == 1){  //��ʾ�ȱ��
		ShowLineChar_HZ(lu8line,lu8column,ZCReBiaoHao,4);
		memcpy(lu8tmpArray,&(p_mf->MeterAddr[0]),7); //�ҵ��ȱ��š�
	}
	else{	//��ʾ���غš�
		ShowLineChar_HZ(lu8line,lu8column,ZCFaKongHao,4);
		memcpy(lu8tmpArray,&(p_mf->ValveAddr[0]),7); //�ҵ��ȱ���.

	}
	
	lu8column += 18;
	for(i=0;i<4;i++){
		lu16tmp = lu8tmpArray[3-i] >> 4;
		if(lu16tmp <= 9)
			ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		else
			ShowChar(lu8line,lu8column,(lu16tmp+0x37));
		
		lu8column += 3;
		lu16tmp = lu8tmpArray[3-i] & 0x0f;
		if(lu16tmp <= 9)
			ShowChar(lu8line,lu8column,(lu16tmp+0x30));
		else
			ShowChar(lu8line,lu8column,(lu16tmp+0x37));

		lu8column += 3;
	}

//end:��ʾ�ȱ�Ż򷧿ر��

//begin:��ʾͨ����
	lu8line = 4;   //�ӵ�4�е�0�п�ʼ��
	lu8column = 0;
	ShowLineChar_HZ(lu8line,lu8column,ZCTongDaoHao,4);
	lu8column += 18;

	lu8tmp = p_mf->ChannelIndex;

	lu16tmp = lu8tmp >> 4;
	ShowChar(lu8line,lu8column,(lu16tmp+0x30));
	lu8column += 3;
	lu16tmp = lu8tmp & 0x0f;
	ShowChar(lu8line,lu8column,(lu16tmp+0x30));
	lu8column += 3;
//end:��ʾͨ����

//begin:��ʾס����Ϣ
	lu8line = 5;   //�ӵ�5�е�0�п�ʼ��
	lu8column = 0;
	ShowLineChar_HZ(lu8line,lu8column,ZCZhuHuXinXi,5);
	lu8column += 23;

	lu8tmp = p_mf->BulidID;

	lu16tmp = lu8tmp >> 4;
	ShowChar(lu8line,lu8column,(lu16tmp+0x30));
	lu8column += 3;
	lu16tmp = lu8tmp & 0x0f;
	ShowChar(lu8line,lu8column,(lu16tmp+0x30));
	lu8column += 3;

	ShowChar(lu8line,lu8column,0x2d);  //��ʾ�̺��ߡ�
	lu8column += 3;

	lu8tmp = p_mf->UnitID;

	lu16tmp = lu8tmp >> 4;
	ShowChar(lu8line,lu8column,(lu16tmp+0x30));
	lu8column += 3;
	lu16tmp = lu8tmp & 0x0f;
	ShowChar(lu8line,lu8column,(lu16tmp+0x30));
	lu8column += 3;

	ShowChar(lu8line,lu8column,0x2d);  //��ʾ�̺��ߡ�
	lu8column += 3;

	lu16tmp = p_mf->RoomID;
	if(lu16tmp > 9999)  //�������4λ������
		lu16tmp = 9999;
	
	lu16qian = lu16tmp / 1000;
	lu16bai = (lu16tmp % 1000) / 100;
	lu16shi = (lu16tmp % 100) / 10;
	lu16ge = lu16tmp % 10;
	if(lu16qian){
		ShowChar(lu8line,lu8column,(lu16qian+0x30));
		lu8column += 3;
	}
	if(lu16bai || lu16qian){
		ShowChar(lu8line,lu8column,(lu16bai+0x30));
		lu8column += 3;
	}
	if(lu16shi || lu16bai || lu16qian){
		ShowChar(lu8line,lu8column,(lu16shi+0x30));
		lu8column += 3;
	}
	ShowChar(lu8line,lu8column,(lu16ge+0x30));
	lu8column += 3;

//end:��ʾס����Ϣ





}

/****************************************************************************************************
**	�� ��  �� ��:ShowMeterFail
**	�� ��  �� ��: ��ʾѡ�в鿴 �ȱ�ʧ�ܲ�ѯ
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ShowMeterFail(void)
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(2,3,&ZCMeterFail[0],6);  //�ȱ�ʧ�ܲ�ѯ
	ShowLineChar_HZ(3,3,&ZCValveFail[0],6);  //����ʧ�ܲ�ѯ
	ReverseShowLine(2);	

}

/****************************************************************************************************
**	�� ��  �� ��:ShowValveFail
**	�� ��  �� ��: ��ʾѡ�в鿴����ʧ�ܲ�ѯ
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ShowValveFail(void)
{
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(2,3,&ZCMeterFail[0],6);  //�ȱ�ʧ�ܲ�ѯ
	ShowLineChar_HZ(3,3,&ZCValveFail[0],6);  //����ʧ�ܲ�ѯ
	ReverseShowLine(3);	

}



/****************************************************************************************************
**	�� ��  �� ��: InquiryMeterFailInfo
**	�� ��  �� ��: ����ʧ����Ϣ��ѯ
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
static uint16 gsu16FailMeterArray[METER_NUM_MAX] = {0};
static uint16 gsu16MeterChannelNum[METER_CHANNEL_NUM] = {0};
static uint16 gsu16MeterChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];
static uint8 gsu8MeterFailed[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];

void InquiryMeterFailInfo(void)
{
	uint8 Err;
	uint8 Res;
	uint8 status;
	uint8 i = 0;
	uint8 j = 0;
	uint16 lu16FailMeterNum = 0;  //��¼����ʧ�ܵı�������
	uint16 lu16DisplayNum = 0;  //�����ѯ��¼��ʾ�ڼ�����
    uint8 lu8UpdataFlag = 1;   //�˱������ڿ����Ƿ������ʾ��
	uint16 lu16MeterSn = 0;
	MeterFileType	mf;


	CPU_SR		cpu_sr;
	
		
	ShowMenuTop();
	ShowMenuBottom(1);

//begin:��ѯ����ʧ����Ϣ��

	//��SD������ʧ�������Ϣ��
	
	OSMutexPend (FlashMutex, 0, &Err);
	for(i=0;i<METER_CHANNEL_NUM;i++){
		Res = SDReadData("/METER_Failed_Info", (uint8 *)(&gsu8MeterFailed[i][0]),METER_PER_CHANNEL_NUM,i*METER_PER_CHANNEL_NUM);
	}
	OSMutexPost (FlashMutex);

	OS_ENTER_CRITICAL();
	memcpy(gsu16MeterChannelNum,gPARA_MeterChannelNum,sizeof(gsu16MeterChannelNum));
	memcpy(gsu16MeterChannel,gPARA_MeterChannel,sizeof(gsu16MeterChannel));
	OS_EXIT_CRITICAL();

	for(i=0;i<METER_CHANNEL_NUM;i++){  //���β�ѯ7��ͨ����ʧ�������

		if(gsu16MeterChannelNum[i] != 0){ //ֻ�б�ͨ���б�Ų�ѯ��
			for(j=0;j<gsu16MeterChannelNum[i];j++){
				if(gsu8MeterFailed[i][j] == 0){	//0˵�������ɹ�
					gsu16FailMeterArray[lu16FailMeterNum++] = gsu16MeterChannel[i][j];
				}

			}
		}

	}


	

//end:��ѯ����ʧ����Ϣ��

		
	while(1){
		if(1 == lu8UpdataFlag){  //ˢ����Ļ��ʾ��
			if(lu16FailMeterNum > 0){ //��ʧ�ܴ���ʱ��ִ�С�
				//ͨ��SN�Ų�ѯ�������Ϣ��
				lu16MeterSn = gsu16FailMeterArray[lu16DisplayNum];
				PARA_ReadMeterInfo(lu16MeterSn, &mf);

				display_white();
				ShowMenuTop();
				ShowMenuBottom(1);
				MeterFailInfoShow(lu16DisplayNum+1,lu16FailMeterNum,&mf,1);

			}
			else{  //û��ʧ�ܡ�
				display_white();
				ShowMenuTop();
				ShowMenuBottom(1);
				memset(&mf,0,sizeof(mf));   //û��ʧ�ܵ���Ϣ��ȫ��0.
				MeterFailInfoShow(0,0,&mf,1);

			}

			lu8UpdataFlag = 0;

		}


		status = GetKeyStatuschaobiao(); //��ȡ��ֵ
		
   		if(status == KEY_EXIT){	
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
			(*KeyTab[KeyFuncIndex].CurrentOperate)();
			lu8UpdataFlag = 1;  //���ٴν���ʱʹ��ˢ����
			lu16DisplayNum = 0;
			break;
		}
		else if(status == KEY_UP){ 
				if(lu16DisplayNum == 0)
					lu16DisplayNum = lu16FailMeterNum - 1;
				else
					lu16DisplayNum -= 1;

				lu8UpdataFlag = 1; //��Ļˢ��ʹ�ܡ�

		}
		else if(status == KEY_DOWN){
				if(lu16DisplayNum >= lu16FailMeterNum-1)
					lu16DisplayNum = 0;
				else
					lu16DisplayNum += 1;

				lu8UpdataFlag = 1; //��Ļˢ��ʹ�ܡ�

		}

		  
	}




}


/****************************************************************************************************
**	�� ��  �� ��: InquiryValveFailInfo
**	�� ��  �� ��: ����ʧ����Ϣ��ѯ
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void InquiryValveFailInfo(void)
{
	uint8 Err;
	uint8 Res;
	uint8 status;
	uint8 i = 0;
	uint8 j = 0;
	uint16 lu16FailMeterNum = 0;  //��¼����ʧ�ܵı�������
	uint16 lu16DisplayNum = 0;  //�����ѯ��¼��ʾ�ڼ�����
    uint8 lu8UpdataFlag = 1;   //�˱������ڿ����Ƿ������ʾ��
	uint16 lu16MeterSn = 0;
	MeterFileType	mf;


	CPU_SR		cpu_sr;
	
		
	ShowMenuTop();
	ShowMenuBottom(1);

//begin:��ѯ����ʧ����Ϣ��

	//��SD������ʧ�������Ϣ��
	
	OSMutexPend (FlashMutex, 0, &Err);
	for(i=0;i<METER_CHANNEL_NUM;i++){
		Res = SDReadData("/METER_Failed_Info", (uint8 *)(&gsu8MeterFailed[i][0]),METER_PER_CHANNEL_NUM,i*METER_PER_CHANNEL_NUM);
	}
	OSMutexPost (FlashMutex);

	OS_ENTER_CRITICAL();
	memcpy(gsu16MeterChannelNum,gPARA_MeterChannelNum,sizeof(gsu16MeterChannelNum));
	memcpy(gsu16MeterChannel,gPARA_MeterChannel,sizeof(gsu16MeterChannel));
	OS_EXIT_CRITICAL();

	for(i=0;i<METER_CHANNEL_NUM;i++){  //���β�ѯ7��ͨ����ʧ�������

		if(gsu16MeterChannelNum[i] != 0){ //ֻ�б�ͨ���б�Ų�ѯ��
			for(j=0;j<gsu16MeterChannelNum[i];j++){
				if(gsu8MeterFailed[i][j] == 0xbb){	//bb˵������ʧ�ܡ�
					gsu16FailMeterArray[lu16FailMeterNum++] = gsu16MeterChannel[i][j];
				}

			}
		}

	}


	

//end:��ѯ����ʧ����Ϣ��

		
	while(1){
		if(1 == lu8UpdataFlag){  //ˢ����Ļ��ʾ��
			if(lu16FailMeterNum > 0){ //��ʧ�ܴ���ʱ��ִ�С�
				//ͨ��SN�Ų�ѯ�������Ϣ��
				lu16MeterSn = gsu16FailMeterArray[lu16DisplayNum];
				PARA_ReadMeterInfo(lu16MeterSn, &mf);

				display_white();
				ShowMenuTop();
				ShowMenuBottom(1);
				MeterFailInfoShow(lu16DisplayNum+1,lu16FailMeterNum,&mf,0);

			}
			else{  //û��ʧ�ܡ�
				display_white();
				ShowMenuTop();
				ShowMenuBottom(1);
				memset(&mf,0,sizeof(mf));   //û��ʧ�ܵ���Ϣ��ȫ��0.
				MeterFailInfoShow(0,0,&mf,0);

			}

			lu8UpdataFlag = 0;

		}


		status = GetKeyStatuschaobiao(); //��ȡ��ֵ
		
   		if(status == KEY_EXIT){	
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
			(*KeyTab[KeyFuncIndex].CurrentOperate)();
			lu8UpdataFlag = 1;  //���ٴν���ʱʹ��ˢ����
			lu16DisplayNum = 0;
			break;
		}
		else if(status == KEY_UP){ 
				if(lu16DisplayNum == 0)
					lu16DisplayNum = lu16FailMeterNum - 1;
				else
					lu16DisplayNum -= 1;

				lu8UpdataFlag = 1; //��Ļˢ��ʹ�ܡ�

		}
		else if(status == KEY_DOWN){
				if(lu16DisplayNum >= lu16FailMeterNum-1)
					lu16DisplayNum = 0;
				else
					lu16DisplayNum += 1;

				lu8UpdataFlag = 1; //��Ļˢ��ʹ�ܡ�

		}

		  
	}




}




#ifdef DEMO_APP
uint8 Sort_ID(uint8 ID)//����ID �������е�λ��
{
   uint8 num=0;
   
   uint8 i=0;
	num=gPARA_MeterChannelNum[gPARA_MeterFiles[ID].ChannelIndex-1];
    for(i=0;i<num;i++)
    	{
          if(gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][i]==ID)
            
            return i;
	    }
  return 0xFF;

}
void Pull_out_OnePara( uint8 ID)
{
   uint8 Index=0;
   
   uint8 i=0;
   Index=Sort_ID(ID);
   if(Index==0xFF) return;
   if(Index==0)
   	gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][0]=0;
   else
   	{
        for(i=Index;i<gPARA_MeterChannelNum[gPARA_MeterFiles[ID].ChannelIndex-1];i++)   
        	{
     
            	
            	gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][i-1]=gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][i];


        	}
		
		    gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][gPARA_MeterChannelNum[gPARA_MeterFiles[ID].ChannelIndex-1]-1]=0;
   }
}
/****************************************************************************************************
**	�� ��  �� ��: SetMeterAddr
**	�� ��  �� ��: 
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 SetMeterAddr(uint8 *pAddr,uint8 CH, uint8 Ver ,uint8 ID)
{
	uint8 Err = NO_ERR;
	TermParaSaveType TermPara;
	CPU_SR			cpu_sr;
//    uint8 i=0;
//	uint8 diff=0;
	uint8 SendNum =3;//Ĭ�ϱ�������3��
	MeterFilePara para;
	#if 0
	for(i=0;i<7;i++)
	  {
		 if(gPARA_MeterFiles[ID].MeterAddr[i]!=pAddr[i])
		  {
		  diff=1;
		  break;
		  
		  }
		  
		  
	  }
	#endif
    if( gPARA_MeterFiles[ID].ChannelIndex!=CH)
    	{

		#if 1
		   if(gPARA_MeterChannelNum[gPARA_MeterFiles[ID].ChannelIndex-1]==3)//����Ĵ������ʵ����һ����ԭ�����г�ȡһ���Ĺ���
		   	{
              if(ID==0)
              	{
			  	gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][0]=gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][1];
			  	gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][1]=gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][2];
                gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][2]=0;
			    }
			  else if(ID==1)
              	{
			  	gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][1]=gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][2];
                gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][2]=0;
			    }
			  else if(ID==2)
			  	{
                  
				  gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][2]=0;
			    }
			  	
			  }
		   	else   if(gPARA_MeterChannelNum[gPARA_MeterFiles[ID].ChannelIndex-1]==2)
		   	{
		   	  if(ID==0)
			  	
			  {
			  gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][0]=gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][1];
			  gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][1]=0;
			  }
			  else if(ID==1)
              	{
              	  if(gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][2]>0)//˵������û��������ID ==1�ǵڶ�λ
              	  
				  gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][1]=0;
				  else
				  	{
              	  
        			  	gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][0]=gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][1];
                        gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][1]=0;
 				  	}
			    }
			  else if(ID==2)
			  	{
                  
				  gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][1]=0;
			    }
			  	
			  }
		   else if(gPARA_MeterChannelNum[gPARA_MeterFiles[ID].ChannelIndex-1]==1)
		   	  {
                 
				 gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][0]=0;
		      }
		   	#else
			Pull_out_OnePara(ID);
			#endif
			gPARA_MeterChannelNum[gPARA_MeterFiles[ID].ChannelIndex-1]--;//ԭ��ͨ��CH	   �������
		  // gPARA_MeterChannel[gPARA_MeterFiles[ID].ChannelIndex-1][ID] = 0;
		   
		   gPARA_MeterFiles[ID].ChannelIndex = CH;

		   
		   gPARA_MeterFiles[ID].ProtocolVer = Ver;	
		   
		   memcpy(gPARA_MeterFiles[ID].MeterAddr, pAddr, 7);
		   
		   gPARA_MeterChannelNum[CH-1]++;//��ͨ��CH	 �������
		   
		   gPARA_MeterChannel[CH-1][gPARA_MeterChannelNum[CH-1]-1] = ID;
	    }
	#if 0
     else if((gPARA_MeterFiles[ID].ProtocolVer!=Ver) && (diff ==0))
     	{
          
		  gPARA_MeterFiles[ID].ProtocolVer = Ver;  
	    }
	#endif 
	 else
	 	{
           
		//  if (diff)	
		
		  gPARA_MeterFiles[ID].ProtocolVer = Ver;  
		  memcpy(gPARA_MeterFiles[ID].MeterAddr, pAddr, 7);
	    }

/*   д������                          */	 
	 para.WriteFlag = 0xAA55;
	 para.DEMOWriteFlag= 0xAA55;

	 OS_ENTER_CRITICAL();	 
	 para.MeterStoreNums = SendNum;
	 /*begin:yangfei changed 2012-12-15 */
	// memcpy(SystemTime, gSystemTime, 6);
	 /*end	 :yangfei changed 2012-12-15 */
	 OS_EXIT_CRITICAL();
	 //�洢������Ϣ����
	 OSMutexPend (FlashMutex,0,&Err);
	 
	 Err = SDSaveData("/METER_FILE_ADDR", gPARA_MeterFiles, sizeof(MeterFileType)*SendNum, 0);	 
	 /*begin:yangfei changed 2012-12-15 for �����洢*/
	 Err = SDSaveData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
	// GetFilePath(NodePath, SystemTime, ARRAY_DAY);
	// Err = SDSaveData(NodePath, &gPARA_MeterNum, sizeof(gPARA_MeterNum), 6);
	 /*end:yangfei changed 2012-12-15 for �����洢*/
	
	Err = SDSaveData("/METER_FILE_DEMO", gPARA_MeterFiles, sizeof(MeterFileType)*SendNum, 0);	
	 OSMutexPost (FlashMutex);
	/*	 д������						   */	 


	Demo_param_set =1;

	

	return Err;
}


/****************************************************************************************************
**	�� ��  �� ��: FreshMeterAddrSet
**	�� ��  �� ��: ˢ����Ŀ��ַ����
**	�� ��  �� ��: uint8* InputBuff
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: muxiaoqing modify protocal
*****************************************************************************************************/
void FreshMeterAddrSet(uint8* MeterAddress,uint8* channle,uint8* Ver)
{
	uint8 i,j,line,column;
#if 0
    LOG_assert_param( InputBuff == NULL );

#else
LOG_assert_param( MeterAddress == NULL );
#endif
	CleanScreen(16,0,128,54);
	ShowMenuTop();
	ShowMenuBottom(21);
	ShowLineChar_HZ(1,20,&MenuHZ16_21_31[0],2);		//��³
	
	ShowLineChar_HZ(2,0,&Meter_ST_MeterNum[0],2);	//¥��:	//����
	
	ShowLineChar_HZ(3,0,&Meter_ST_CH[0],2);	//��Ԫ:	//¥��
	
	ShowLineChar_HZ(4,0,&IPVer[0],3);		//Э��
	//ShowPicture(48,22,16,32,&MenuPic_OK[0]);//3�� OK
	
	ShowPicture(80,22,16,32,&MenuPic_OK[0]);//5�� OK
	#if 0
	line = 2;
	column = 20;
	for(i=0;i<4;i++)
	{
		ShowChar(line,column+3*i,InputBuff[i]);
	}	
	#endif
	
	line = 2;
	column = 10;
	
	for(i=0;i<14;i++)//��ʾ14λ���ַ
	{ 
        		ShowChar(line,column+3*i,MeterAddress[i]);
	}	
	
        		ShowChar(line+1,column,channle[0]);//��ʾͨ����
        		
        		ShowChar(line+2,column,Ver[0]);//��ʾЭ���
	return;
}

/****************************************************************************************************
**	�� ��  �� ��: Set_MeterParam_1
**	�� ��  �� ��: �ȱ��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��					                          
**  ��   ��   ֵ: ��
**	��		  ע: ĽФ����� ��ʾ��
*****************************************************************************************************/
void Set_MeterParam_1(void)// �ȱ��������
{
	uint8 status;
	uint8 line;
	uint8 column;
	uint8 column1;
	uint8 line1;
	
	uint8 column2;
	uint8 line2;
	
	uint8 column3;
	uint8 line3;
	int8 i;

	uint8 ASCIIMeterAddress[14];	
	uint8 ASCIIChannle[1];	
	uint8 ASCIIVer[1];	

	uint8 TermAddr[7] = {0};
//	uint8 TermAddrReverse[6] = {0x00};
	
//	MeterFilePara	para;
//	uint8 Err	= 0;
//	uint8 datalen;
#if 0	
	SelectInfo TabMenu[] = {{2,20,12},{3,22,11}};//�У��У�����
#else
   // SelectInfo TabMenu[] = {{2,25,13},{3,25,13},{4,25,13},{5,22,11}};//�У��У�����
	SelectInfo TabMenu[] = {{2,10,43},{3,10,13},{4,10,13},{5,22,11}};//�У��У�����
	
#endif
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
	CPU_SR			cpu_sr;
				
	uint8 addrtemp_DeLu[7] = {0x09, 0x78, 0x13, 0x21, 0x00, 0x11, 0x11};
	uint8 addrtemp_LanJE[7] = {0x35, 0x25, 0x10, 0x67, 0x00, 0x00, 0x00};
	uint8 addrtemp_TG[7] = {0x22, 0x17, 0x05, 0x14, 0x00, 0x11, 0x11};

	uint8 Mertersn_ID =0;
#if 1
  if(Demo_param_set==0)
  	{

	//Err = SDReadData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
	
	//if(para.DEMOWriteFlag== 0xAA55)
	
	if(DemoDateIsGet)
		{
	
		   
		
		}
	else
     		{
     
     	
         memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
         memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
         memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));
         memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
         memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
         memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
         memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
         memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
         gPARA_MeterFiles[0].ChannelIndex = 3;
         gPARA_MeterFiles[0].ProtocolVer = 0;  
         memcpy(gPARA_MeterFiles[0].MeterAddr, addrtemp_DeLu, 7);  
      
      
         gPARA_MeterFiles[1].ChannelIndex = 3;
         gPARA_MeterFiles[1].ProtocolVer = 8;  
         memcpy(gPARA_MeterFiles[1].MeterAddr, addrtemp_LanJE, 7);	 
      
         gPARA_MeterFiles[2].ChannelIndex = 3;
         gPARA_MeterFiles[2].ProtocolVer = 0;  
         memcpy(gPARA_MeterFiles[2].MeterAddr, addrtemp_TG, 7);	 
     			
     				   
             #if 0//ÿ��ͨ��һ�����ʱ��
             				 for(i=0;i<3;i++)
             					{
             					  gPARA_MeterChannelNum[i]=1;
             					  gPARA_MeterChannel[i][0] = i;
             					  gPARA_ConPanelChannel[i][0] = i;
             					  gPARA_ConPanelChannelNum[i]=1;
             					  gPARA_ValveConChannelNum[i]=1;
             					  gPARA_ValveConChannel[i][0] = i;
             					}
             #else 
             			gPARA_MeterChannelNum[2]=3;//ͨ��3	  �������
             			for(i=0;i<3;i++)
             			{
             			  gPARA_MeterChannel[2][i] = i;
             			}
             #endif
     		
     				 gPARA_MeterNum = 3;
       	}	
  	}
#endif
	
	
	
	//���ڴ洢���ֽ���ǰ���������ڽ��ֽ�˳�����
	for(i=0; i<7; i++)
	{
		TermAddr[i] = gPARA_MeterFiles[Mertersn_ID].MeterAddr[6-i];
	}
#if 0	
	//ȡ����˳���ĺ�2�ֽ���Ч
	for(i=0;i<2;i++)
	{
		ASCIIData[2*i] = HexToAscii(TermAddr[4+i]>>4);
		ASCIIData[2*i+1] = HexToAscii(TermAddr[4+i]&0x0f);
	}
#else
	
//ȡ����˳���ĺ�2�ֽ���Ч
for(i=0;i<7;i++)
{
	ASCIIMeterAddress[2*i] = HexToAscii(TermAddr[i]>>4);
	ASCIIMeterAddress[2*i+1] = HexToAscii(TermAddr[i]&0x0f);
}
 ASCIIChannle[0] = HexToAscii(gPARA_MeterFiles[Mertersn_ID].ChannelIndex);
 ASCIIVer[0]=HexToAscii(gPARA_MeterFiles[Mertersn_ID].ProtocolVer) ;
#endif
	i = 0;
	line = TabMenu[0].startline;
	column = TabMenu[0].startcolumn;
	line1 = TabMenu[1].startline;
	column1 = TabMenu[1].startcolumn;
	
	line2 = TabMenu[2].startline;
	column2 = TabMenu[2].startcolumn;
	
	line3 = TabMenu[3].startline;
	column3 = TabMenu[3].startcolumn;
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
	   #if 0
		FreshTermAddrSet(&ASCIIData[0]);
	   #else	   
	   FreshMeterAddrSet(&ASCIIMeterAddress[0],&ASCIIChannle[0],&ASCIIVer[0]);
	   #endif
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
						KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
						//KeyFuncIndex = 2;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}		
		}
		else if(TabNum == 0)		
		{
			ReverseShowChar(line,column);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIMeterAddress[i] >= 0x39)
					{
						ASCIIMeterAddress[i] = 0x30;
					}
					else
					{
						ASCIIMeterAddress[i] = ASCIIMeterAddress[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIMeterAddress[i] <= 0x30)||(ASCIIMeterAddress[i] >0x39))
					{
						ASCIIMeterAddress[i] = 0x39;
					}
					else
					{
						ASCIIMeterAddress[i] = ASCIIMeterAddress[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line,column,ASCIIMeterAddress[i]);
					if((i > 0)&&(i<14))
					{
						i = i - 1;
						column = column - 3;
					}
					else
						{
                          i =0;
						  column = TabMenu[0].startcolumn;
					    }
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line,column,ASCIIMeterAddress[i]);
					if(i < 13)
					{
						i = i + 1;
						column = column + 3;
					}
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}
		
		else if(TabNum == 1)		
		{
			ReverseShowChar(line1,column1);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIChannle[i] >= 0x39)
					{
						ASCIIChannle[i] = 0x30;
					}
					else
					{
						ASCIIChannle[i] = ASCIIChannle[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIChannle[i] <= 0x30)||(ASCIIChannle[i] >0x39))
					{
						ASCIIChannle[i] = 0x39;
					}
					else
					{
						ASCIIChannle[i] = ASCIIChannle[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line1,column1,ASCIIChannle[0]);
					#if 0
					if(i > 0)
					{
						i = i - 1;
						column1 = column1 - 3;
					}
					#endif
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line1,column1,ASCIIChannle[0]);
					#if 0
					if(i < 3)
					{
						i = i + 1;
						column1 = column1 + 3;
					}
					#endif
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}
		else if(TabNum ==2)
		{
			ReverseShowChar(line2,column2);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIVer[i] >= 0x39)
					{
						ASCIIVer[i] = 0x30;
					}
					else
					{
						ASCIIVer[i] = ASCIIVer[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIVer[i] <= 0x30)||(ASCIIVer[i] >0x39))
					{
						ASCIIVer[i] = 0x39;
					}
					else
					{
						ASCIIVer[i] = ASCIIVer[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line2,column2,ASCIIVer[0]);
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line2,column2,ASCIIVer[0]);
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}

	}//end while

	memset(TermAddr, 0x00, 7); //

for(i=0;i<7;i++)
{
	TermAddr[6-i] =  ((ASCIIMeterAddress[2*i]-0x30)<<4) + ((ASCIIMeterAddress[2*i+1]-0x30)&0x0F);
}


	SetMeterAddr(TermAddr,(ASCIIChannle[0]-0x30),(ASCIIVer[0]-0x30),Mertersn_ID); //
	//==========================================
	ShowMenuBottom(2);//���óɹ�

	KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();

	return;
}


/****************************************************************************************************
**	�� ��  �� ��: Set_MeterParam_2
**	�� ��  �� ��: �ȱ��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��					                          
**  ��   ��   ֵ: ��
**	��		  ע: ĽФ����� ��ʾ��
*****************************************************************************************************/
void Set_MeterParam_2(void)// �ȱ��������
{
	uint8 status;
	uint8 line;
	uint8 column;
	uint8 column1;
	uint8 line1;
	
	uint8 column2;
	uint8 line2;
	
	uint8 column3;
	uint8 line3;
	int8 i;

	uint8 ASCIIMeterAddress[14];	
	uint8 ASCIIChannle[1];	
	uint8 ASCIIVer[1];	

	uint8 TermAddr[7] = {0};
//	uint8 TermAddrReverse[6] = {0x00};
	
//	MeterFilePara	para;
//	uint8 Err	= 0;
//	uint8 datalen;
#if 0	
	SelectInfo TabMenu[] = {{2,20,12},{3,22,11}};//�У��У�����
#else
   // SelectInfo TabMenu[] = {{2,25,13},{3,25,13},{4,25,13},{5,22,11}};//�У��У�����
	SelectInfo TabMenu[] = {{2,10,43},{3,10,13},{4,10,13},{5,22,11}};//�У��У�����
	
#endif
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
	CPU_SR			cpu_sr;
				
	uint8 addrtemp_DeLu[7] = {0x09, 0x78, 0x13, 0x21, 0x00, 0x11, 0x11};
	uint8 addrtemp_LanJE[7] = {0x35, 0x25, 0x10, 0x67, 0x00, 0x00, 0x00};
	uint8 addrtemp_TG[7] = {0x22, 0x17, 0x05, 0x14, 0x00, 0x11, 0x11};

	uint8 Mertersn_ID =1;
#if 1
	  if(Demo_param_set==0)
		{

	//	Err = SDReadData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
		
	//	if(para.DEMOWriteFlag== 0xAA55)
	
	if(DemoDateIsGet)
			{
		
			   
			
			}
		else
     			{
     
     		
     		memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
     		memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
     		memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));
     		memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
     		memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
     		memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
     		memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
     		memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
     	 
     		gPARA_MeterFiles[0].ChannelIndex = 3;
     		gPARA_MeterFiles[0].ProtocolVer = 0;  
     		memcpy(gPARA_MeterFiles[0].MeterAddr, addrtemp_DeLu, 7);  
     	 
     	 
     		gPARA_MeterFiles[1].ChannelIndex = 3;
     		gPARA_MeterFiles[1].ProtocolVer = 8;  
     		memcpy(gPARA_MeterFiles[1].MeterAddr, addrtemp_LanJE, 7);	 
     	 
     		gPARA_MeterFiles[2].ChannelIndex = 3;
     		gPARA_MeterFiles[2].ProtocolVer = 0;  
     		memcpy(gPARA_MeterFiles[2].MeterAddr, addrtemp_TG, 7);	 
     				
     					   
             #if 0//ÿ��ͨ��һ�����ʱ��
     							 for(i=0;i<3;i++)
     								{
     								  gPARA_MeterChannelNum[i]=1;
     								  gPARA_MeterChannel[i][0] = i;
     								  gPARA_ConPanelChannel[i][0] = i;
     								  gPARA_ConPanelChannelNum[i]=1;
     								  gPARA_ValveConChannelNum[i]=1;
     								  gPARA_ValveConChannel[i][0] = i;
     								}
             #else 
     						gPARA_MeterChannelNum[2]=3;//ͨ��1	  �������
     						for(i=0;i<3;i++)
     						{
     						  gPARA_MeterChannel[2][i] = i;
     						}
             #endif
     			
     					 gPARA_MeterNum = 3;
     		}	
	  	}
#endif
	
	
	
	//���ڴ洢���ֽ���ǰ���������ڽ��ֽ�˳�����
	for(i=0; i<7; i++)
	{
		TermAddr[i] = gPARA_MeterFiles[Mertersn_ID].MeterAddr[6-i];
	}
#if 0	
	//ȡ����˳���ĺ�2�ֽ���Ч
	for(i=0;i<2;i++)
	{
		ASCIIData[2*i] = HexToAscii(TermAddr[4+i]>>4);
		ASCIIData[2*i+1] = HexToAscii(TermAddr[4+i]&0x0f);
	}
#else
	
//ȡ����˳���ĺ�2�ֽ���Ч
for(i=0;i<7;i++)
{
	ASCIIMeterAddress[2*i] = HexToAscii(TermAddr[i]>>4);
	ASCIIMeterAddress[2*i+1] = HexToAscii(TermAddr[i]&0x0f);
}
 ASCIIChannle[0] = HexToAscii(gPARA_MeterFiles[Mertersn_ID].ChannelIndex);
 ASCIIVer[0]=HexToAscii(gPARA_MeterFiles[Mertersn_ID].ProtocolVer) ;
#endif
	i = 0;
	line = TabMenu[0].startline;
	column = TabMenu[0].startcolumn;
	line1 = TabMenu[1].startline;
	column1 = TabMenu[1].startcolumn;
	
	line2 = TabMenu[2].startline;
	column2 = TabMenu[2].startcolumn;
	
	line3 = TabMenu[3].startline;
	column3 = TabMenu[3].startcolumn;
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
	   #if 0
		FreshTermAddrSet(&ASCIIData[0]);
	   #else	   
	   FreshMeterAddrSet(&ASCIIMeterAddress[0],&ASCIIChannle[0],&ASCIIVer[0]);
	   #endif
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
						KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
						//KeyFuncIndex = 2;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}		
		}
		else if(TabNum == 0)		
		{
			ReverseShowChar(line,column);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIMeterAddress[i] >= 0x39)
					{
						ASCIIMeterAddress[i] = 0x30;
					}
					else
					{
						ASCIIMeterAddress[i] = ASCIIMeterAddress[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIMeterAddress[i] <= 0x30)||(ASCIIMeterAddress[i] >0x39))
					{
						ASCIIMeterAddress[i] = 0x39;
					}
					else
					{
						ASCIIMeterAddress[i] = ASCIIMeterAddress[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line,column,ASCIIMeterAddress[i]);
					if((i > 0)&&(i<14))
					{
						i = i - 1;
						column = column - 3;
					}
					else
						{
                          i =0;
						  column = TabMenu[0].startcolumn;
					    }
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line,column,ASCIIMeterAddress[i]);
					if(i < 13)
					{
						i = i + 1;
						column = column + 3;
					}
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}
		
		else if(TabNum == 1)		
		{
			ReverseShowChar(line1,column1);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIChannle[i] >= 0x39)
					{
						ASCIIChannle[i] = 0x30;
					}
					else
					{
						ASCIIChannle[i] = ASCIIChannle[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIChannle[i] <= 0x30)||(ASCIIChannle[i] >0x39))
					{
						ASCIIChannle[i] = 0x39;
					}
					else
					{
						ASCIIChannle[i] = ASCIIChannle[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line1,column1,ASCIIChannle[0]);
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line1,column1,ASCIIChannle[0]);
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}
		else if(TabNum ==2)
		{
			ReverseShowChar(line2,column2);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIVer[i] >= 0x39)
					{
						ASCIIVer[i] = 0x30;
					}
					else
					{
						ASCIIVer[i] = ASCIIVer[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIVer[i] <= 0x30)||(ASCIIVer[i] >0x39))
					{
						ASCIIVer[i] = 0x39;
					}
					else
					{
						ASCIIVer[i] = ASCIIVer[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line2,column2,ASCIIVer[0]);
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line2,column2,ASCIIVer[0]);
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}

	}//end while

	memset(TermAddr, 0x00, 7); //

	
	for(i=0;i<7;i++)
	{
		TermAddr[6-i] =  ((ASCIIMeterAddress[2*i]-0x30)<<4) + ((ASCIIMeterAddress[2*i+1]-0x30)&0x0F);
	}
	
	
		SetMeterAddr(TermAddr,(ASCIIChannle[0]-0x30),(ASCIIVer[0]-0x30),Mertersn_ID); //
	//==========================================
	ShowMenuBottom(2);//���óɹ�

	KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();

	return;
}


/****************************************************************************************************
**	�� ��  �� ��: Set_MeterParam_3
**	�� ��  �� ��: �ȱ��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��					                          
**  ��   ��   ֵ: ��
**	��		  ע: ĽФ����� ��ʾ��
*****************************************************************************************************/
void Set_MeterParam_3(void)// �ȱ��������
{
	uint8 status;
	uint8 line;
	uint8 column;
	uint8 column1;
	uint8 line1;
	
	uint8 column2;
	uint8 line2;
	
	uint8 column3;
	uint8 line3;
	int8 i;

	uint8 ASCIIMeterAddress[14];	
	uint8 ASCIIChannle[1];	
	uint8 ASCIIVer[1];	

	uint8 TermAddr[7] = {0};
//	uint8 TermAddrReverse[6] = {0x00};
	
//	MeterFilePara	para;
//	uint8 Err	= 0;
//	uint8 datalen;
#if 0	
	SelectInfo TabMenu[] = {{2,20,12},{3,22,11}};//�У��У�����
#else
   // SelectInfo TabMenu[] = {{2,25,13},{3,25,13},{4,25,13},{5,22,11}};//�У��У�����
	SelectInfo TabMenu[] = {{2,10,43},{3,10,13},{4,10,13},{5,22,11}};//�У��У�����
	
#endif
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
	CPU_SR			cpu_sr;
				
	uint8 addrtemp_DeLu[7] = {0x09, 0x78, 0x13, 0x21, 0x00, 0x11, 0x11};
	uint8 addrtemp_LanJE[7] = {0x35, 0x25, 0x10, 0x67, 0x00, 0x00, 0x00};
	uint8 addrtemp_TG[7] = {0x22, 0x17, 0x05, 0x14, 0x00, 0x11, 0x11};

	uint8 Mertersn_ID =2;
#if 1
	  if(Demo_param_set==0)
		{
		 // Err = SDReadData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
		  
		//  if(para.DEMOWriteFlag== 0xAA55)
		
		 if(DemoDateIsGet)
			  {
		  
				 
			  
			  }
		  else
     			  {
     		
     		memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
     		memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
     		memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));
     		memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
     		memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
     		memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
     		memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
     		memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
     	 
     		gPARA_MeterFiles[0].ChannelIndex = 3;
     		gPARA_MeterFiles[0].ProtocolVer = 0;  
     		memcpy(gPARA_MeterFiles[0].MeterAddr, addrtemp_DeLu, 7);  
     	 
     	 
     		gPARA_MeterFiles[1].ChannelIndex = 3;
     		gPARA_MeterFiles[1].ProtocolVer = 8;  
     		memcpy(gPARA_MeterFiles[1].MeterAddr, addrtemp_LanJE, 7);	 
     	 
     		gPARA_MeterFiles[2].ChannelIndex = 3;
     		gPARA_MeterFiles[2].ProtocolVer = 0;  
     		memcpy(gPARA_MeterFiles[2].MeterAddr, addrtemp_TG, 7);	 
     				
     					   
             #if 0//ÿ��ͨ��һ�����ʱ��
     							 for(i=0;i<3;i++)
     								{
     								  gPARA_MeterChannelNum[i]=1;
     								  gPARA_MeterChannel[i][0] = i;
     								  gPARA_ConPanelChannel[i][0] = i;
     								  gPARA_ConPanelChannelNum[i]=1;
     								  gPARA_ValveConChannelNum[i]=1;
     								  gPARA_ValveConChannel[i][0] = i;
     								}
             #else 
     						gPARA_MeterChannelNum[2]=3;//ͨ��1	  �������
     						for(i=0;i<3;i++)
     						{
     						  gPARA_MeterChannel[2][i] = i;
     						}
             #endif
     			
     					 gPARA_MeterNum = 3;
     		}	
	  	}
#endif
	
	
	
	//���ڴ洢���ֽ���ǰ���������ڽ��ֽ�˳�����
	for(i=0; i<7; i++)
	{
		TermAddr[i] = gPARA_MeterFiles[Mertersn_ID].MeterAddr[6-i];
	}
#if 0	
	//ȡ����˳���ĺ�2�ֽ���Ч
	for(i=0;i<2;i++)
	{
		ASCIIData[2*i] = HexToAscii(TermAddr[4+i]>>4);
		ASCIIData[2*i+1] = HexToAscii(TermAddr[4+i]&0x0f);
	}
#else
	
//ȡ����˳���ĺ�2�ֽ���Ч
for(i=0;i<7;i++)
{
	ASCIIMeterAddress[2*i] = HexToAscii(TermAddr[i]>>4);
	ASCIIMeterAddress[2*i+1] = HexToAscii(TermAddr[i]&0x0f);
}
 ASCIIChannle[0] = HexToAscii(gPARA_MeterFiles[Mertersn_ID].ChannelIndex);
 ASCIIVer[0]=HexToAscii(gPARA_MeterFiles[Mertersn_ID].ProtocolVer) ;
#endif
	i = 0;
	line = TabMenu[0].startline;
	column = TabMenu[0].startcolumn;
	line1 = TabMenu[1].startline;
	column1 = TabMenu[1].startcolumn;
	
	line2 = TabMenu[2].startline;
	column2 = TabMenu[2].startcolumn;
	
	line3 = TabMenu[3].startline;
	column3 = TabMenu[3].startcolumn;
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
	   #if 0
		FreshTermAddrSet(&ASCIIData[0]);
	   #else	   
	   FreshMeterAddrSet(&ASCIIMeterAddress[0],&ASCIIChannle[0],&ASCIIVer[0]);
	   #endif
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
						KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
						//KeyFuncIndex = 2;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}		
		}
		else if(TabNum == 0)		
		{
			ReverseShowChar(line,column);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIMeterAddress[i] >= 0x39)
					{
						ASCIIMeterAddress[i] = 0x30;
					}
					else
					{
						ASCIIMeterAddress[i] = ASCIIMeterAddress[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIMeterAddress[i] <= 0x30)||(ASCIIMeterAddress[i] >0x39))
					{
						ASCIIMeterAddress[i] = 0x39;
					}
					else
					{
						ASCIIMeterAddress[i] = ASCIIMeterAddress[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line,column,ASCIIMeterAddress[i]);
					if((i > 0)&&(i<14))
					{
						i = i - 1;
						column = column - 3;
					}
					else
						{
                          i =0;
						  column = TabMenu[0].startcolumn;
					    }
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line,column,ASCIIMeterAddress[i]);
					if(i < 13)
					{
						i = i + 1;
						column = column + 3;
					}
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}
		
		else if(TabNum == 1)		
		{
			ReverseShowChar(line1,column1);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIChannle[i] >= 0x39)
					{
						ASCIIChannle[i] = 0x30;
					}
					else
					{
						ASCIIChannle[i] = ASCIIChannle[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIChannle[i] <= 0x30)||(ASCIIChannle[i] >0x39))
					{
						ASCIIChannle[i] = 0x39;
					}
					else
					{
						ASCIIChannle[i] = ASCIIChannle[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line1,column1,ASCIIChannle[0]);
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line1,column1,ASCIIChannle[0]);
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;

					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}
		else if(TabNum ==2)
		{
			ReverseShowChar(line2,column2);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIVer[i] >= 0x39)
					{
						ASCIIVer[i] = 0x30;
					}
					else
					{
						ASCIIVer[i] = ASCIIVer[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if((ASCIIVer[i] <= 0x30)||(ASCIIVer[i] >0x39))
					{
						ASCIIVer[i] = 0x39;
					}
					else
					{
						ASCIIVer[i] = ASCIIVer[i] - 1;
					}	
					break;			
				case KEY_LEFT://�����
					ShowChar(line2,column2,ASCIIVer[i]);
					if(i > 0)
					{
						i = i - 1;
						column2 = column2 - 3;
					}
					break;
				case KEY_RIGHT://���Ҽ�
					ShowChar(line2,column2,ASCIIVer[i]);
					if(i < 3)
					{
						i = i + 1;
						column2 = column2 + 3;
					}
					break;
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					column1 = TabMenu[1].startcolumn;
					
					line2 = TabMenu[2].startline;
					column2 = TabMenu[2].startcolumn;
					
					line3 = TabMenu[3].startline;
					column3 = TabMenu[3].startcolumn;
					line1 = line1 ;
					column1 = column1;
					line2 = line2 ;
					column2 = column2;
					line3 = line3 ;
					column3 = column3;
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}

	}//end while

	memset(TermAddr, 0x00, 7); //

for(i=0;i<7;i++)
{
	TermAddr[6-i] =  ((ASCIIMeterAddress[2*i]-0x30)<<4) + ((ASCIIMeterAddress[2*i+1]-0x30)&0x0F);
}


	SetMeterAddr(TermAddr,(ASCIIChannle[0]-0x30),(ASCIIVer[0]-0x30),Mertersn_ID); //
	//==========================================
	ShowMenuBottom(2);//���óɹ�

	KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();

	return;
}



void CJ188_BCD2Hex(CJ188_Format *MeterFormatCJ188Data)
{
	MeterFormatCJ188Data->DailyHeat =BcdToHex_16bit1(MeterFormatCJ188Data->DailyHeat);
	MeterFormatCJ188Data->CurrentHeat=BcdToHex_16bit1(MeterFormatCJ188Data->CurrentHeat);
	MeterFormatCJ188Data->HeatPower=BcdToHex_16bit1(MeterFormatCJ188Data->HeatPower);
	MeterFormatCJ188Data->Flow=BcdToHex_16bit1(MeterFormatCJ188Data->Flow);
	MeterFormatCJ188Data->AccumulateFlow=BcdToHex_16bit1(MeterFormatCJ188Data->AccumulateFlow);
	MeterFormatCJ188Data->WaterInTemp[0]=BcdToHex(MeterFormatCJ188Data->WaterInTemp[0]);
	
	MeterFormatCJ188Data->WaterInTemp[1]=BcdToHex(MeterFormatCJ188Data->WaterInTemp[1]);
	
	MeterFormatCJ188Data->WaterInTemp[2]=BcdToHex(MeterFormatCJ188Data->WaterInTemp[2]);
	MeterFormatCJ188Data->WaterOutTemp[0]=BcdToHex(MeterFormatCJ188Data->WaterOutTemp[0]);
	MeterFormatCJ188Data->WaterOutTemp[1]=BcdToHex(MeterFormatCJ188Data->WaterOutTemp[1]);
	MeterFormatCJ188Data->WaterOutTemp[2]=BcdToHex(MeterFormatCJ188Data->WaterOutTemp[2]);

}
/****************************************************************************************************
**	�� ��  �� ��: ReadMeterStateShowData
**	�� ��  �� ��: 
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ReadMeterStateShowData(void)

{
	uint8 status;
//	uint8 i  ;
//	uint8 j = 0  ;
	uint8 line, column;
	uint8 len,len1,len2,len3;
//	uint8 DateTemp[6] = {0x00};
	uint8 ASCIIData[6] = {0x00};
	uint8 ASCIIZero[1] = {0x30};

//	TimingState	lTimingState;  //����״̬��Ϣ�ṹ��
	CPU_SR		cpu_sr;
	
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	uint32 tempData=0;
	line 	= 1;
	column 	= 0;
	uint8 index_keydown=0;
	//Demo_CJ188;
	CJ188_BCD2Hex(&Demo_CJ188_ch1);
	
	CJ188_BCD2Hex(&Demo_CJ188_ch2);
	
	CJ188_BCD2Hex(&Demo_CJ188_ch3);
	Show_Data_Status =1;
 while(1)
  {
  #if 0
	OS_ENTER_CRITICAL();
	lTimingState = gREAD_TimingState;
	OS_EXIT_CRITICAL();
	
	//����ʼʱ�䣺
	for(i=0;i<3;i++)
	{
		ASCIIData[2*i] = HexToAscii(lTimingState.TimingStartTime[2-i]>>4);
		ASCIIData[2*i+1] = HexToAscii(lTimingState.TimingStartTime[2-i]&0x0f);
	}
	
	ShowLineChar_HZ(line, column, &MeterState[0][0], 7); //��ʾ���֣�������ʼʱ�䡱
	ShowIniTime(line + 1, column+10, &ASCIIData[0]);  //��ʾ��ǰʱ��
   #endif 
 //if(Channel_1_SUCESS)
 	//{
	// ShowLineChar_HZ(line, column, &Meter_ST_OK_1[0], 7); //��ʾ���֣�"��³����: �ɹ�"
	// ShowIniTime(line + 1, column+10, &ASCIIData[0]);  //��ʾ��ǰʱ��

  //  }
  
 if( index_keydown==0)
 	{
   ShowLineChar_HZ(line, column, &MenuHZ16_21_31[0],2);//��³
   
	ShowLineChar_HZ(line, column+12, &Meter_ST_Building[0],3);//¥��:
	len = sprintf((char*)&ASCIIData[0],"%d",10);
	ShowLineChar(line, column+25, &ASCIIData[0], len); 

	
	ShowLineChar_HZ(line, column+32, &Meter_ST_Unit[0],3);//��Ԫ:
	
	len = sprintf((char*)&ASCIIData[0],"%d",1);
	ShowLineChar(line, column+45, &ASCIIData[0], len); 
	
   ShowLineChar_HZ(line+1, column, &Meter_ST_AIM_CHeat[0],5);
   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch1.CurrentHeat);
   if(len>2)//ȥ��С��
   ShowLineChar(line+1, column+22, &ASCIIData[0], len-2); 
   else
   	
   ShowLineChar(line+1, column+22, &ASCIIZero[0], 1); 
   
   ShowLineChar_HZ(line+2, column, &Meter_ST_AIM_Power[0],4);
   
   tempData=BcdToHex_16bit1(Demo_CJ188_ch1.HeatPower);
   len = sprintf((char*)&ASCIIData[0],"%d",(uint16)Demo_CJ188_ch1.HeatPower);
   ShowLineChar(line+2, column+22, &ASCIIData[0], len); 

   ShowLineChar_HZ(line+3, column, &Meter_ST_AIM_Flow[0],3);
   
   tempData=BcdToHex_16bit1(Demo_CJ188_ch1.Flow);
   len = sprintf((char*)&ASCIIData[0],"%d",(uint16)Demo_CJ188_ch1.Flow);
   ShowLineChar(line+3, column+22, &ASCIIData[0], len); 

   ShowLineChar_HZ(line+4, column, &Meter_ST_AIM_CFlow[0],5);
   
 //  tempData=BcdToHex_16bit1(Demo_CJ188_ch1.AccumulateFlow);
 //  len = sprintf((char*)&ASCIIData[0],"%d",(uint16)Demo_CJ188_ch1.AccumulateFlow);
 
 len = sprintf((char*)&ASCIIData[0],"%d",0);
 
 len1 = sprintf((char*)&ASCIIData[1],"%s",".");
 
 len2 = sprintf((char*)&ASCIIData[2],"%d",Demo_CJ188_ch1.AccumulateFlow);
   ShowLineChar(line+4, column+22, &ASCIIData[0], len+len1+len2); 
//��ˮ�¶�
   ShowLineChar_HZ(line+5, column, &Meter_ST_AIM_INwater[0],5);

   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch1.WaterInTemp[2]);
   len1 = sprintf((char*)&ASCIIData[len],"%d",Demo_CJ188_ch1.WaterInTemp[1]);
    len2 = sprintf((char*)&ASCIIData[len+len1],"%s",".");
	
	len3 = sprintf((char*)&ASCIIData[len+len1+len2],"%d",Demo_CJ188_ch1.WaterInTemp[0]);
	
   ShowLineChar(line+5, column+22, &ASCIIData[0], (len+len1+len2+len3)); 
   //��ˮ�¶�

   //��ˮ�¶�
   ShowLineChar_HZ(line+6, column, &Meter_ST_AIM_Owater[0],5);
   
      len=len1=len2=len3=0;
	  len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch1.WaterOutTemp[2]);
	  len1 = sprintf((char*)&ASCIIData[len],"%d",Demo_CJ188_ch1.WaterOutTemp[1]);
	   len2 = sprintf((char*)&ASCIIData[len+len1],"%s",".");
	   
	   len3 = sprintf((char*)&ASCIIData[len+len1+len2],"%d",Demo_CJ188_ch1.WaterOutTemp[0]);
	   
	  ShowLineChar(line+6, column+22, &ASCIIData[0], (len+len1+len2+len3)); 
   //��ˮ�¶�

 	}
 else if ( index_keydown==1)
 	{
   ShowLineChar_HZ(line, column, &MenuHZ16_21_32[0],3);//������

   ShowLineChar_HZ(line, column+17, &Meter_ST_Building[0],3);//¥��:
   len = sprintf((char*)&ASCIIData[0],"%d",11);
   ShowLineChar(line, column+30, &ASCIIData[0], len); 
   
   
   ShowLineChar_HZ(line, column+37, &Meter_ST_Unit[0],3);//��Ԫ:
   
   len = sprintf((char*)&ASCIIData[0],"%d",1);
   ShowLineChar(line, column+50, &ASCIIData[0], len); 


   
   ShowLineChar_HZ(line+1, column, &Meter_ST_AIM_CHeat[0],5);
   
   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch2.CurrentHeat);
   
   if(len>2)//ȥ��С��
   ShowLineChar(line+1, column+22, &ASCIIZero[0], 1); 
  else
  	
  ShowLineChar(line+1, column+22, &ASCIIZero[0], 1); 
   ShowLineChar_HZ(line+2, column, &Meter_ST_AIM_Power[0],4);
   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch2.HeatPower);
   ShowLineChar(line+2, column+22, &ASCIIData[0], len); 

   ShowLineChar_HZ(line+3, column, &Meter_ST_AIM_Flow[0],3);
   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch2.Flow);
   ShowLineChar(line+3, column+22, &ASCIIData[0], len); 

   ShowLineChar_HZ(line+4, column, &Meter_ST_AIM_CFlow[0],5);
  // len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch2.AccumulateFlow);
    len = sprintf((char*)&ASCIIData[0],"%d",0);
    
    len1 = sprintf((char*)&ASCIIData[1],"%s",".");

	len2 = sprintf((char*)&ASCIIData[2],"%d",Demo_CJ188_ch2.AccumulateFlow);
   ShowLineChar(line+4, column+22, &ASCIIData[0], len+len1+len2); 
//��ˮ�¶�
   ShowLineChar_HZ(line+5, column, &Meter_ST_AIM_INwater[0],5);

   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch2.WaterInTemp[2]);
   len1 = sprintf((char*)&ASCIIData[len],"%d",Demo_CJ188_ch2.WaterInTemp[1]);
    len2 = sprintf((char*)&ASCIIData[len+len1],"%s",".");
	
	len3 = sprintf((char*)&ASCIIData[len+len1+len2],"%d",Demo_CJ188_ch2.WaterInTemp[0]);
	
   ShowLineChar(line+5, column+22, &ASCIIData[0], (len+len1+len2+len3)); 
   //��ˮ�¶�

   //��ˮ�¶�
   ShowLineChar_HZ(line+6, column, &Meter_ST_AIM_Owater[0],5);
   
      len=len1=len2=len3=0;
	  len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch2.WaterOutTemp[2]);
	  len1 = sprintf((char*)&ASCIIData[len],"%d",Demo_CJ188_ch2.WaterOutTemp[1]);
	   len2 = sprintf((char*)&ASCIIData[len+len1],"%s",".");
	   
	   len3 = sprintf((char*)&ASCIIData[len+len1+len2],"%d",Demo_CJ188_ch2.WaterOutTemp[0]);
	   
	  ShowLineChar(line+6, column+22, &ASCIIData[0], (len+len1+len2+len3)); 
   //��ˮ�¶�

 	}
 else if(index_keydown ==2)
 	{
   ShowLineChar_HZ(line, column, &MenuHZ16_21_33[0],2);


   ShowLineChar_HZ(line, column+12, &Meter_ST_Building[0],3);//¥��:
   len = sprintf((char*)&ASCIIData[0],"%d",12);
   ShowLineChar(line, column+25, &ASCIIData[0], len); 
   
   
   ShowLineChar_HZ(line, column+32, &Meter_ST_Unit[0],3);//��Ԫ:
   
   len = sprintf((char*)&ASCIIData[0],"%d",1);
   ShowLineChar(line, column+45, &ASCIIData[0], len); 

   
   ShowLineChar_HZ(line+1, column, &Meter_ST_AIM_CHeat[0],5);
   
   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch3.CurrentHeat);
	if(len>2)//ȥ��С��
	ShowLineChar(line+1, column+22, &ASCIIData[0], len-2); 
   else
	 
   ShowLineChar(line+1, column+22, &ASCIIZero[0], 1); 
   
   ShowLineChar_HZ(line+2, column, &Meter_ST_AIM_Power[0],4);
   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch3.HeatPower);
   ShowLineChar(line+2, column+22, &ASCIIData[0], len); 

   ShowLineChar_HZ(line+3, column, &Meter_ST_AIM_Flow[0],3);
   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch3.Flow);
   ShowLineChar(line+3, column+22, &ASCIIData[0], len); 

   ShowLineChar_HZ(line+4, column, &Meter_ST_AIM_CFlow[0],5);
//   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch3.AccumulateFlow);
   len = sprintf((char*)&ASCIIData[0],"%d",0);
   
   len1 = sprintf((char*)&ASCIIData[1],"%s",".");
   
   len2 = sprintf((char*)&ASCIIData[2],"%d",Demo_CJ188_ch3.AccumulateFlow);

   ShowLineChar(line+4, column+22, &ASCIIData[0], len+len1+len2); 
//��ˮ�¶�
   ShowLineChar_HZ(line+5, column, &Meter_ST_AIM_INwater[0],5);

   len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch3.WaterInTemp[2]);
   len1 = sprintf((char*)&ASCIIData[len],"%d",Demo_CJ188_ch3.WaterInTemp[1]);
    len2 = sprintf((char*)&ASCIIData[len+len1],"%s",".");
	
	len3 = sprintf((char*)&ASCIIData[len+len1+len2],"%d",Demo_CJ188_ch3.WaterInTemp[0]);
	
   ShowLineChar(line+5, column+22, &ASCIIData[0], (len+len1+len2+len3)); 
   //��ˮ�¶�

   //��ˮ�¶�
   ShowLineChar_HZ(line+6, column, &Meter_ST_AIM_Owater[0],5);
   
      len=len1=len2=len3=0;
	  len = sprintf((char*)&ASCIIData[0],"%d",Demo_CJ188_ch3.WaterOutTemp[2]);
	  len1 = sprintf((char*)&ASCIIData[len],"%d",Demo_CJ188_ch3.WaterOutTemp[1]);
	   len2 = sprintf((char*)&ASCIIData[len+len1],"%s",".");
	   
	   len3 = sprintf((char*)&ASCIIData[len+len1+len2],"%d",Demo_CJ188_ch3.WaterOutTemp[0]);
	   
	  ShowLineChar(line+6, column+22, &ASCIIData[0], (len+len1+len2+len3)); 
   //��ˮ�¶�

 	}
#if 0
    //�������ʱ�䣺
	for(i=0;i<3;i++)
	{
		ASCIIData[2*i] = HexToAscii(lTimingState.TimingEndTime[2-i]>>4);
		ASCIIData[2*i+1] = HexToAscii(lTimingState.TimingEndTime[2-i]&0x0f);
	}
	
	ShowLineChar_HZ(line+2, column, &MeterState[3][0], 7);
	ShowIniTime(line + 3, column+10, &ASCIIData[0]);
	
 
 
	//��������:
	len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
    ShowLineChar_HZ(line+5, column, &MeterState[1][0], 7); //��ʾ ��������������
	ShowLineChar(line+5, column+30, &ASCIIData[0], len); //��ʾ��������ܸ�����

	
	//����ɹ���:
   	len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.ReadCmplNums);
	ShowLineChar_HZ(line+6, column, &MeterState[2][0], 7);  //��ʾ ������ɹ�������
	ShowLineChar(line+6, column+35, &ASCIIData[0], len);  //��ʾ������ĳɹ�������
	
	status = GetKeyStatuschaobiao(); //��ȡ��ֵ
#endif	
	status = GetKeyStatuschaobiao(); //��ȡ��ֵ
    
   	if( status == KEY_DOWN )
	{	
	index_keydown++;
	if(index_keydown>2)
		index_keydown=0;
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	}
	else if( status == KEY_UP )
	{	
	index_keydown--;
	if(index_keydown<0||index_keydown>2)
		index_keydown=2;
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	}
   	else if( status == KEY_EXIT )
	{	
	       
		   Show_Data_Status =0;
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
			//KeyFuncIndex = 2;
		(*KeyTab[KeyFuncIndex].CurrentOperate)();
		break;
	}	
  }
	 
}

/****************************************************************************************************
**	�� ��  �� ��: ReadMeterStateShow_Demo
**	�� ��  �� ��: ��ȡ������״̬
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ReadMeterStateShow_Demo(void)
{
	uint8 status;
//	uint8 i  ;
//	uint8 j = 0  ;
	uint8 line, column;
	uint8 len;
//	uint8 DateTemp[6] = {0x00};
	uint8 ASCIIData[6] = {0x00};
	TimingState	lTimingState;  //����״̬��Ϣ�ṹ��
	CPU_SR		cpu_sr;
	
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	
	line 	= 1;
	column 	= 0;
	Meter_R_End =0;
 while(1)
  {
  #if 0
	OS_ENTER_CRITICAL();
	lTimingState = gREAD_TimingState;
	OS_EXIT_CRITICAL();
	
	//����ʼʱ�䣺
	for(i=0;i<3;i++)
	{
		ASCIIData[2*i] = HexToAscii(lTimingState.TimingStartTime[2-i]>>4);
		ASCIIData[2*i+1] = HexToAscii(lTimingState.TimingStartTime[2-i]&0x0f);
	}
	
	ShowLineChar_HZ(line, column, &MeterState[0][0], 7); //��ʾ���֣�������ʼʱ�䡱
	ShowIniTime(line + 1, column+10, &ASCIIData[0]);  //��ʾ��ǰʱ��
   #endif 
 //if(Channel_1_SUCESS)
 	//{
	// ShowLineChar_HZ(line, column, &Meter_ST_OK_1[0], 7); //��ʾ���֣�"��³����: �ɹ�"
	// ShowIniTime(line + 1, column+10, &ASCIIData[0]);  //��ʾ��ǰʱ��

  //  }
  
   if(Meter_R_End ==0)
   	{
   	OS_ENTER_CRITICAL();
	lTimingState = gREAD_TimingState;
	OS_EXIT_CRITICAL();
      ShowLineChar_HZ(line, column, &Meter_ST_WAIT[0], 6);
	  
	  len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
	  ShowLineChar_HZ(line+5, column, &MeterState[1][0], 7); //��ʾ ��������������
	  ShowLineChar(line+5, column+30, &ASCIIData[0], len); //��ʾ��������ܸ�����
	  
	  
	  //����ɹ���:
	  len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.ReadCmplNums);
	  ShowLineChar_HZ(line+6, column, &MeterState[2][0], 7);  //��ʾ ������ɹ�������
	  ShowLineChar(line+6, column+35, &ASCIIData[0], len);	//��ʾ������ĳɹ�������
    }
  else if(Meter_R_End ==1)
  	{
  	
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	(Channel_1_SUCESS ==1)?(ShowLineChar_HZ(line, column, &Meter_ST_OK_1[0], 7)):(ShowLineChar_HZ(line, column, &Meter_ST_FAL_1[0], 7));
	 (Channel_2_SUCESS ==1)?(ShowLineChar_HZ(line+1, column, &Meter_ST_OK_2[0], 8)):(ShowLineChar_HZ(line+1, column, &Meter_ST_FAL_2[0], 8));
	 (Channel_3_SUCESS ==1)?(ShowLineChar_HZ(line+2, column, &Meter_ST_OK_3[0], 7)):(ShowLineChar_HZ(line+2, column, &Meter_ST_FAL_3[0], 7));
     
	 ShowLineChar_HZ(line+5, column, &Meter_ST_AIM[0],11);
    }
#if 0
    //�������ʱ�䣺
	for(i=0;i<3;i++)
	{
		ASCIIData[2*i] = HexToAscii(lTimingState.TimingEndTime[2-i]>>4);
		ASCIIData[2*i+1] = HexToAscii(lTimingState.TimingEndTime[2-i]&0x0f);
	}
	
	ShowLineChar_HZ(line+2, column, &MeterState[3][0], 7);
	ShowIniTime(line + 3, column+10, &ASCIIData[0]);
	
 
 
	//��������:
	len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
    ShowLineChar_HZ(line+5, column, &MeterState[1][0], 7); //��ʾ ��������������
	ShowLineChar(line+5, column+30, &ASCIIData[0], len); //��ʾ��������ܸ�����

	
	//����ɹ���:
   	len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.ReadCmplNums);
	ShowLineChar_HZ(line+6, column, &MeterState[2][0], 7);  //��ʾ ������ɹ�������
	ShowLineChar(line+6, column+35, &ASCIIData[0], len);  //��ʾ������ĳɹ�������
	
	status = GetKeyStatuschaobiao(); //��ȡ��ֵ
#endif	
	status = GetKeyStatuschaobiao(); //��ȡ��ֵ
    
   	if( status == KEY_DOWN )
	{	
	    if(Meter_R_End ==1)//���ݲɼ����ǰ������������
	    	{
        		ReadMeterStateShowData();
        		break;
	    	}
	}	
   	else if( status == KEY_EXIT )
	{	
	    if(Meter_R_End ==1)//���ݲɼ����ǰ������������
	    	{
        			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
        			//KeyFuncIndex = 2;
        		(*KeyTab[KeyFuncIndex].CurrentOperate)();
        		break;
	    	}
	}	
  }
	 
}

/****************************************************************************************************
**	�� ��  �� ��: ReadMeter_Demo
**	�� ��  �� ��: ��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ReadMeter_Demo(void)
{
		uint8 SendMsg[2]		= {0x00};
		SendMsg[0] =  MSG_1D;
		uint8 i=0;
#ifdef DEMO_APP
//			MeterFilePara	para;
//			uint8 Err	= 0;
			
			uint8 addrtemp_DeLu[7] = {0x09, 0x78, 0x13, 0x21, 0x00, 0x11, 0x11};
			uint8 addrtemp_LanJE[7] = {0x35, 0x25, 0x10, 0x67, 0x00, 0x00, 0x00};
			uint8 addrtemp_TG[7] = {0x22, 0x17, 0x05, 0x14, 0x00, 0x11, 0x11};
#endif			
		
		memset((uint8 *)&Demo_CJ188_ch1, 0, sizeof(Demo_CJ188_ch1));
		
		memset((uint8 *)&Demo_CJ188_ch2, 0, sizeof(Demo_CJ188_ch2));
		memset((uint8 *)&Demo_CJ188_ch3, 0, sizeof(Demo_CJ188_ch3));
#if 1
		  if(Demo_param_set==0)
			{
			
			//Err = SDReadData("/METER_FILE_PARA", &para, sizeof(MeterFilePara), 0);
			
		//	if(para.DEMOWriteFlag== 0xAA55)
		    if(DemoDateIsGet)
				{

                   
				
				}
			else
				{
           			memset((uint8 *)gPARA_MeterFiles, 0x00, sizeof(gPARA_MeterFiles));
           			memset((uint8 *)gPARA_MeterChannel, 0x00, sizeof(gPARA_MeterChannel));
           			memset((uint8 *)gPARA_Meter_Failed, 0x00, sizeof(gPARA_Meter_Failed));
           			memset((uint8 *)gPARA_MeterChannelNum, 0x00, sizeof(gPARA_MeterChannelNum));
           			memset((uint8 *)gPARA_ConPanelChannel, 0x00, sizeof(gPARA_ConPanelChannel));
           			memset((uint8 *)gPARA_ConPanelChannelNum, 0x00, sizeof(gPARA_ConPanelChannelNum));
           			memset((uint8 *)gPARA_ValveConChannel, 0x00, sizeof(gPARA_ValveConChannel));
           			memset((uint8 *)gPARA_ValveConChannelNum, 0x00, sizeof(gPARA_ValveConChannelNum));
           		 
           			gPARA_MeterFiles[0].ChannelIndex = 3;
           			gPARA_MeterFiles[0].ProtocolVer = 0;  
           			memcpy(gPARA_MeterFiles[0].MeterAddr, addrtemp_DeLu, 7);  
           		 
           		 
           			gPARA_MeterFiles[1].ChannelIndex = 3;
           			gPARA_MeterFiles[1].ProtocolVer = 8;  
           			memcpy(gPARA_MeterFiles[1].MeterAddr, addrtemp_LanJE, 7);	 
           		 
           			gPARA_MeterFiles[2].ChannelIndex = 3;
           			gPARA_MeterFiles[2].ProtocolVer = 0;  
           			memcpy(gPARA_MeterFiles[2].MeterAddr, addrtemp_TG, 7);	 
           					
           						   
                   #if 0//ÿ��ͨ��һ�����ʱ��
           								 for(i=0;i<3;i++)
           									{
           									  gPARA_MeterChannelNum[i]=1;
           									  gPARA_MeterChannel[i][0] = i;
           									  gPARA_ConPanelChannel[i][0] = i;
           									  gPARA_ConPanelChannelNum[i]=1;
           									  gPARA_ValveConChannelNum[i]=1;
           									  gPARA_ValveConChannel[i][0] = i;
           									}
                   #else 
           							gPARA_MeterChannelNum[2]=3;//ͨ��1	  �������
           							for(i=0;i<3;i++)
           							{
           							  gPARA_MeterChannel[2][i] = i;
           							}
                   #endif
				   
				   gPARA_MeterNum = 3;
				}
				
			}	
#endif
     
		OSQPost(CMD_Q, (void*)SendMsg);
		ReadMeterStateShow_Demo();
}
#endif
/****************************************************************************************************
**	�� ��  �� ��: SlectReadMeterChannel
**	�� ��  �� ��: ѡ�񳭱�ͨ��
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void SlectReadMeterChannel(void)
{	
	uint8 status;
	uint8 line;
	uint8 column;
	uint8 column1;
	uint8 line1;
	int8 i;
	uint8 ASCIIData[4] = {0x31,0,0,0};
	
//	uint8 datalen;
	
	SelectInfo TabMenu[] = {{2,20,12},{3,22,11}};//�У��У�����
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
	
	i = 0;
	line = TabMenu[0].startline;
	column = TabMenu[0].startcolumn;
	line1 = TabMenu[1].startline;
	column1 = TabMenu[1].startcolumn;
	
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
		FreshSlectReadMeterChannel(&ASCIIData[0]);
		
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 10;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}		
		}
		else if(TabNum == 0)
		{
			ReverseShowChar(line,column);
			status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
			
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(ASCIIData[i] == 0x36)
					{
						ASCIIData[i] = 0x31;
					}
					else
					{
						ASCIIData[i] = ASCIIData[i] + 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(ASCIIData[i] == 0x31)
					{
						ASCIIData[i] = 0x36;
					}
					else
					{
						ASCIIData[i] = ASCIIData[i] - 1;
					}	
					break;			
				
				case KEY_ENTER://�س���
					Tab_Flag = (!Tab_Flag);
					//����ԭʼ�У��´ν���ʱ���Դ�ͷ��ʼ
					i = 0;
					line = TabMenu[0].startline;
					column = TabMenu[0].startcolumn;
					line1 = TabMenu[1].startline;
					line1 = line1;/*��ֹ���뾯��*/
					column1 = TabMenu[1].startcolumn;
					column1 = column1;/*��ֹ���뾯��*/
					TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 4;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}
		}//end else if(TabNum == 0)

	}//end while
	
	{
          uint8 channel=0;
	   channel = ASCIIData[0]-0x30;
	    if(channel>=1&&channel<=6)
          {
           //�л�Mbusͨ��
	     METER_ChangeChannel(channel);		
            Uart_Printf("METER_ChangeChannel  to %d\r\n",channel);
          }
        else
          {
            Uart_Printf("Error:METER_ChangeChannel = %d \r\n",channel);
          }
	}
	//==========================================
	ShowMenuBottom(2);//���óɹ�

	KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();

	return;
}


/****************************************************************************************************
**	�� ��  �� ��: restart
**	�� ��  �� ��: ��������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void restart(void)
{
	;
}

/****************************************************************************************************
**	�� ��  �� ��: ResetSystem
**	�� ��  �� ��: �����նˣ�ȷ�ϻ��߷���
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void ResetSystem(void)  //�����նˣ�ȷ�ϻ��߷���
{
	uint8 status;
	
	display_white();
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowPicture(16,0,48,160,&MenuPic1_3_1[0]);
	
	status = GetKeyStatus();
	switch(status)
	{
		case KEY_ENTER://�س���
			//====================
			//���������ն˵ĺ���
			    //RestartHardWare();//��λ����
			     //gRestart = 1;
				 /*begin:yangfei added 2013-01-29*/
				 	NVIC_SETFAULTMASK();
					NVIC_GenerateSystemReset();
				 /*end:yangfei added 2013-01-29*/
				ShowMenuBottom(4);
				OSTimeDly(OS_TICKS_PER_SEC*5);
			//====================
			KeyFuncIndex = 11;
			(*KeyTab[KeyFuncIndex].CurrentOperate)();
			break;
		case KEY_EXIT://���ؼ�
			KeyFuncIndex = 11;
			(*KeyTab[KeyFuncIndex].CurrentOperate)();
			break;	
		default:
			KeyFuncIndex = 24;
			(*KeyTab[KeyFuncIndex].CurrentOperate)();		
			break;
	}
	return;
}

/****************************************************************************************************
**	�� ��  �� ��: FindKey
**	�� ��  �� ��: ���ҹؼ���key,����ֵΪ�ؼ���λ��
**	�� ��  �� ��: uint8* StrBuff
**                uint8 Key
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
int FindKey(uint8* StrBuff, uint8 Key)//���ҹؼ���key,����ֵΪ�ؼ���λ��
{
	int i = 0;
	uint8 flag = 0;
	while(*StrBuff != 0x00)
	{
		if(*StrBuff != Key)
		{
			StrBuff++;
			i++;		
		}
		else
		{
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		i = 0xffff;
	}
	return i;
}

/****************************************************************************************************
**	�� ��  �� ��: VerInfoShow
**	�� ��  �� ��: �ն˰汾��Ϣ��ʾ
**	�� ��  �� ��: uint8* StrBuff
**                uint8 Key
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void VerInfoShow(void)  //�ն˰汾��Ϣ��ʾ===
{
    uint16 version = gVersion;
	uint16 version1[4];
	int i= 0;	
	uint8 lu8VersionH = 0;
	uint8 lu8VersionL = 0;

	
	display_white(); //��ʾ����
	ShowMenuTop();	//��ʾҺ��������Ϣ
	ShowMenuBottom(1);//��ʾҺ���ײ���Ϣ
	//��ʾ����
	
    version = (((((SW_VERSION)/100)<<8)&0x0F00)+((((SW_VERSION%100)/10)<<4)&0x00F0)+((SW_VERSION%10)&0x000F));
	ShowLineChar_HZ(1,0,&Menu_VerInfo[0][0],5);	//��ʾһ��ASCII���ַ��ͺ��ֵĻ��
	ShowLineChar_HZ(3,0,&Menu_VerInfo[1][0],5);	//��ʾһ��ASCII���ַ��ͺ��ֵĻ��

	//��ʾ�汾��Ϣ
	ShowLineChar1(4,0, __DATE__,11);
	ShowLineChar1(5,0, __TIME__,8);
	ShowLineChar1(2,0, &date[0],1);	//�汾��Ϣ�����"V"
	ShowLineChar_HZ(2,9,&Dot[0],1);//�汾��Ϣ�����"."

	lu8VersionH = version >> 8;
	lu8VersionH = lu8VersionH % 100;  //ֻ��ʾ2λ.
	lu8VersionH = HexToBcd(lu8VersionH);
	lu8VersionL = version & 0x00ff;
	lu8VersionL = lu8VersionL % 100;  //ֻ��ʾ2λ.
	lu8VersionL = HexToBcd(lu8VersionL);
	

	version1[0] = HexToAscii( lu8VersionH >> 4 );
	version1[1] = HexToAscii( lu8VersionH & 0x0f );
	version1[2] = HexToAscii( lu8VersionL >> 4 );
	version1[3] = HexToAscii( lu8VersionL & 0x0f ); 

	for( i=0; i<4; i++ )
	{
        if( i<2 )
		  {
			ShowChar1(2,3+i*3,version1[i]);
		  }
		else
		  {
			ShowChar1(2,6+i*3,version1[i]);
		  } 
	}
	return;
}

/****************************************************************************************************
**	�� ��  �� ��: LCDContrastSet
**	�� ��  �� ��: Һ���Աȶȵ���
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void LCDContrastSet(void) //Һ���Աȶȵ���
{
	uint8 status;
	uint8 line = 4;
	uint8 column = 1;
	uint8 ContrastValue = LCD_CON_VALUE;//195
	uint8 BlackNum;
	
	bool flag = TRUE;
	uint8 len;
	uint8 ContrastBuff[5] = {0};
	uint8 Err 	= 0;
	LCDContrastPara NVLCDConrast;
    uint8 counter=0;
    ContrastValue =(uint8)NVLCDConraston.LCDContrast;


//	SelfDefinePara pSelfPara;
	
//	ContrastValue = gStore_CommPara.LCDContrastValue;
	/*
	Read_SelfDefinePara((uint8*)&pSelfPara);
	ContrastValue = pSelfPara.LCDContrastValue;
	if(ContrastValue == 0 || ContrastValue == 0xff)
	{
		ContrastValue = LCD_CON_VALUE;
	}
	*/
	while(flag)
	{
		display_white();
		ShowMenuTop();
		ShowMenuBottom(25);
			
		ShowLineChar_HZ(1,10,&MenuHZ13_26[2],7);//��ʾ Һ���Աȶȵ���
		ShowHZ(5,0,0x015a);//��ʾ ��
		ShowHZ(5,48,0x0159);//��ʾ ��
		
		//��ʾ����ͼ��
		ShowRectangle((16*line-1), column, 17, 153, TRUE);
		BlackNum = (153 - ((ContrastValue/5)*3)) / 3;
		BlackScreen(16*line, column, 16, BlackNum);
		
		len = sprintf((char*)&ContrastBuff[0], "%d", ContrastValue);
		ShowLineChar(line+1, 27, &ContrastBuff[0], len);
		SetContrast(ContrastValue);//���öԱ�ֵ
		//�洢��FLASH��
//		Read_SelfDefinePara((uint8*)&pSelfPara);
//		pSelfPara.LCDContrastValue = ContrastValue;
//		Set_SelfDefinePara((uint8*)&pSelfPara);
//		SelfDefinePara.LCDContrastValue = ContrastValue;//����ContrastValueȫ�ֱ���,��lcmdrv.c��Һ����ʼ����ʱ���õ�ȫ�ֱ���
		
		status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
		switch(status)
		{
			case KEY_UP://���ϼ�
				break;
			case KEY_DOWN://���¼�
				break;
			case KEY_LEFT://�����
				if(ContrastValue <= 250)
				{
					ContrastValue = ContrastValue + 5;
				}
				break;
			case KEY_RIGHT://���Ҽ�
				if(ContrastValue >= 5 )
				{
					ContrastValue = ContrastValue - 5;
				}
				break;			
			case KEY_ENTER://�س���
				KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
				(*KeyTab[KeyFuncIndex].CurrentOperate)();
				memset(&NVLCDConrast,0,sizeof(NVLCDConrast));
				NVLCDConrast.WriteFlag = 0xAA55;
				NVLCDConrast.LCDContrast = ContrastValue;
				Err=MakeFile("/LCD_CONTRAST_PARA");
				   if(Err)
					   {
					   
					   }
				   else
					   {
				   
						   do{
							   Err = SDReadData("/LCD_CONTRAST_PARA", &NVLCDConraston, sizeof(LCDContrastPara), 0);
							   
							   if(Err!=NO_ERR) 
								 {
								  counter++;
								  
								 }
							   if (counter ==5 )
								   {
									 counter =0;
									 break;
								   }
						   }while(Err!=NO_ERR);
						//   OSMutexPend (FlashMutex, 0, &Err);
						   Err = SDSaveData("/LCD_CONTRAST_PARA", &NVLCDConrast, sizeof(LCDContrastPara), 0);
						   
						 //  OSMutexPost (FlashMutex);
						   
					   }
				
				return;
			case KEY_EXIT://���ؼ�
				KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
				(*KeyTab[KeyFuncIndex].CurrentOperate)();
				return;
		
			default:
				break;
		}
	}
	return;
}

///////////=====������������Ӧ�÷���publicfunc.c�У���Ϊ�˸��µķ��㣬��ʱ��������========
////������Ϊ16���Ƶ�IPת��Ϊ15λ��ASCII�룬��ConverHexToASCIP���س��Ȳ���
/****************************************************************************************************
**	�� ��  �� ��: ConverHexToASCIP_15
**	�� ��  �� ��: 16���Ƶ�IPת��Ϊ15λ��ASCII��
**	�� ��  �� ��: uint8* pTemp1
**  			  uint8* pTemp2
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 ConverHexToASCIP_15(uint8* pTemp1,uint8* pTemp2)
{
	uint8 a,i;
	uint8 TempBuf[3];

	LOG_assert_param(  pTemp1 == NULL );
	LOG_assert_param(  pTemp2 == NULL );

	for(i=0;i<=3;i++)
	{
	    a = pTemp1[i];
	    memset(&TempBuf[0],0x30,3);
	    Hex_BcdToAscii((uint16)a,TempBuf);
	    Reversalmemcpy(pTemp2,TempBuf,3);
	    pTemp2 += 3;
	    if(i==3)  *pTemp2++ = 0x00;
	    else  *pTemp2++ = '.';//0x2E
	}
	return 15;
}
/****************************************************************************************************
**	�� ��  �� ��: Hex_BcdToAscii_5
**	�� ��  �� ��: 16λ��HEX����BCD��ת��ΪASCII�룬Ȼ�󷴷�������AscBuf�У�����Ϊ5
**	�� ��  �� ��: uint16 Hex_bcd_code
**  			  uint8* AscBuf
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
/////������Ϊ16���Ƶ�portת��Ϊ5λ��ASCII�룬��Hex_BcdToAscii���ز�����
uint8 Hex_BcdToAscii_5(uint16 Hex_bcd_code,uint8* AscBuf)//16λ��HEX����BCD��ת��ΪASCII�룬Ȼ�󷴷�������AscBuf�У�����Ϊ5
{
	uint8  num = 0;
	//uint8  TempBuf;
	uint16 b1,b2;

	LOG_assert_param(  AscBuf == NULL );

	b2 = Hex_bcd_code;
	memset(&AscBuf[0],0x30,5);
	//if(b2==0)//Hex_bcd_codeΪ0�����
	//{
	//	memset(&AscBuf[0],0x30,5);
	//}
	while(b2)
	{
		b1 = b2%10;
		AscBuf[num++] = HexToAscii(b1);
		b2 = b2/10;
	}
	return 5;
}
////////////������������Ϊ�˴���ʾԭ��FLASH�е�IP��PORT�����===================================

/****************************************************************************************************
**	�� ��  �� ��: KeyCheck
**	�� ��  �� ��: �������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void KeyCheck(void)
{
	//void  *KeyFuncPtr;//���ܺ���ָ��
//	uint8 KeyFuncIndex;
	uint8 status;
	status = GetKeyStatus();
	switch(status)//�жϺ����д��޸�Ϊ�ʵ����������д�����
	{
		case KEY_DOWN://���¼�
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyDnState;
			break;
		case KEY_UP://���ϼ�
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyUpState;
			break;
		case KEY_LEFT://�����
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyLeftState;
			break;
		case KEY_RIGHT://���Ҽ�
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyRightState;
			break;			
		case KEY_ENTER: //�س���
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyCrState;
			break;
		case KEY_EXIT://���ؼ�
			KeyFuncIndex = KeyTab[KeyFuncIndex].KeyBackState;
			break;
		default:
			break;
	} 
	//(*KeyTab[KeyFuncIndex].CurrentOperate)();
	//KeyFuncPtr =
	//(*KeyFuncPtr)();
}


/****************************************************************************************************
**	�� ��  �� ��: MenuTest
**	�� ��  �� ��: Ŀ¼����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void MenuTest(void)
{
//	uint8 i;
//	KeyFuncIndex = 1;
//	for(i = 0;i < 50;i++)
//	{	
		(*KeyTab[KeyFuncIndex].CurrentOperate)();

		KeyCheck();
		
//	}
	
}
/****************************************************************************************************
**	�� ��  �� ��: FreshMethodshow
**	�� ��  �� ��: ˢ��ѡ�񷽷�״̬
**	�� ��  �� ��: 
**	�� ��  �� ��: ��
**	��	 ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshMethodshow(uint8 line)
{
	display_white();
	ShowLineChar_HZ(1,4,&MenuHZ15[0],6);   //�ն˹�����ά��
	ShowLineChar_HZ(2,1,&MenuHZ15_1[0],4);//�ȼ�����
	ShowLineChar_HZ(3,1,&MenuHZ14[0],7);//ʱ��ͨ�������
	ShowLineChar_HZ(4,1,&MenuHZ15_3[0],5);//�ȷ����
	
	ShowLineChar_HZ(7,1,&MenuHZ15_4[0],6);//��ǰ��������
	ShowChar(7, 30, ':');
	if(gPARA_TermPara.DeviceType == HEAT_METER_TYPE)
	{
		ShowLineChar_HZ(8,1,&MenuHZ15_1[0],4);//�ȼ�����
	}
	else if(gPARA_TermPara.DeviceType == TIME_ON_OFF_AREA_TYPE)
	{
		ShowLineChar_HZ(8,1,&MenuHZ14[0],7);//ʱ��ͨ�������
	}
	else if(gPARA_TermPara.DeviceType == HEATCOST_METER_TYPE)
	{
		ShowLineChar_HZ(8,1,&MenuHZ15_3[0],5);//�ȷ����
	}
	ShowMenuTop();
	ShowMenuBottom(25);
	ReverseShowLine(line);
}

/****************************************************************************************************
**	�� ��  �� ��: FreshSetReadMeterTime
**	�� ��  �� ��: ˢ�³���ʱ������ҳ��
**	�� ��  �� ��: uint8 timenum
**                uint8 *pIndex
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshSetUserData(uint8 *pIndex,uint8 *puser,uint8 *pData)//ˢ�³���ʱ������ҳ��
{
	LOG_assert_param( pIndex == NULL );
	LOG_assert_param( puser == NULL );

	CleanScreen(16,0,128,54);
	ShowMenuTop();
	ShowMenuBottom(1);
	ShowLineChar_HZ(1,10,&chose_user_display[0],6);
	ShowLineChar_HZ(2,9,&MenuHZ_user[0],6);
	ShowLineChar_HZ(3,9,&MenuHZ_TimeNum[0],9);
	ShowIniDate(4,9,&pData[0]);//��ʾ����
	ShowPicture(80,22,16,32,&MenuPic_OK[0]);//OK 4��
		

	ShowChar(2, 14, *puser++);
	ShowChar(2, 17, *puser++);
	ShowChar(2, 20, *puser);
	
	ShowChar(3, 14, *pIndex++);
	ShowChar(3, 17, *pIndex);

}
/****************************************************************************************************
**	�� ��  �� ��: get_sd_display_data
**	�� ��  �� ��: ��sd��ѡ��Ҫ��ʾ������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void get_sd_display_data(void)
{
	uint8 Res;
	uint8 buf[200];
	uint8 asiibuf[10];
	uint16  TimeNode;
	uint8 *pbuf;
	uint16 max_usr_num;
	SDSave_Data *pSaveData;
	char  Datapath[]="/2014/02/25/1500";
	char  NodePath[]="/2014/02/25/timenode";
	//�˴�����Ӹ��ݳ������ҳ���ʱ��㺯��

	NodePath[3]=str_check_data.time_path[3];
	NodePath[4]=str_check_data.time_path[4];
	NodePath[6]=str_check_data.time_path[6];
	NodePath[7]=str_check_data.time_path[7];
	NodePath[9]=str_check_data.time_path[9];
	NodePath[10]=str_check_data.time_path[10];
	//��ȡ��ʱ������û���Ŀ
	//Res = SDReadData((const char*)NodePath, (uint8 *)&max_usr_num, 2,(str_check_data.read_num-1)*2+6);
	Res = SDReadData((const char*)NodePath, (uint8 *)&max_usr_num, 2,6);
	if(Res!=NO_ERR)
	{
		;
	}
	if(str_check_data.user_id>(max_usr_num-1))
	{
		str_check_data.user_id = 1;
	}
	else if(str_check_data.user_id==0)
	{
		str_check_data.user_id = max_usr_num-1;
	}	
	//��ȡ��ʱ����·��						 
	Res = SDReadData((const char*)NodePath, (uint8 *)&TimeNode, 2,(str_check_data.read_num-1)*2+8);
	if(Res!=NO_ERR)
	{
		;
	}
	memcpy(Datapath, str_check_data.time_path, 17);
	HexToAscii_1(TimeNode,asiibuf);
	Datapath[12] = HexToAscii((TimeNode>>12)%16); 
	Datapath[13] = HexToAscii((TimeNode>>8)%16); 
	Datapath[14] = HexToAscii((TimeNode>>4)%16); 
	Datapath[15] = HexToAscii(TimeNode%16); 
	pbuf = buf;
	if(str_check_data.user_id <=1)
	{
		str_check_data.user_id = 1;
	}
	/*��0��洢�ȱ����ݣ���1���Ժ�洢ʱ��ͨ�����������*/
	Res = SDReadData((const char*)Datapath, pbuf, sizeof(SDSave_Data),(str_check_data.user_id)*128); 
	if(Res!=NO_ERR)
	{
		;
	}
	pSaveData = (SDSave_Data*)pbuf;
	strDisplayData.user_id = pSaveData->user_id;
	strDisplayData.address = pSaveData->address;
	strDisplayData.area = pSaveData->area;
	strDisplayData.open_time = pSaveData->open_time;
	strDisplayData.room_temperature = pSaveData->room_temperature;
	strDisplayData.forward_temperature = pSaveData->forward_temperature;
	strDisplayData.return_temperature = pSaveData->return_temperature;
	strDisplayData.proportion_energy = pSaveData->proportion_energy;
	strDisplayData.total_energy = pSaveData->total_energy;

}

/****************************************************************************************************
**	�� ��  �� ��: Chose_time_data
**	�� ��  �� ��: ѡ��ʱ��
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Chose_time_data(void)
{
	uint8 status;
	uint8 line1,line2,line3;
	uint8 column1,column2,column3;
	uint8 i;
	uint8 ASCIIData1[2];
	uint8 User_Num[3];
	static uint8 TimeNum = 0x01;
	static uint16 UserNum=1;
	
	uint8 ASCIIData[12];
	uint8 datebuf[6];
	
	SelectInfo TabMenu[] = {{2,14,9},{3,14,6},{4,9,34},{5,22,11}};//�У��У�����
	uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	bool OK_Flag = FALSE;
	bool Tab_Flag = TRUE;
	
	//Ĭ����ʾ��һ����ʱ���
	ASCIIData1[0] = HexToAscii((TimeNum)>>4);
	ASCIIData1[1] = HexToAscii((TimeNum)&0x0f);

	User_Num[0]= HexToAscii(UserNum/100);
	User_Num[1]= HexToAscii((UserNum - (UserNum/100)*100)/10);
	User_Num[2]= HexToAscii(UserNum%10);

	
	//����ǰʱ��
	ReadDateTime(&datebuf[0]);
	
	datebuf[4] = datebuf[4]&0x1F;//ȥ������
	for(i=0;i<6;i++)
	{
		ASCIIData[2*i] = HexToAscii(datebuf[5-i]>>4);
		ASCIIData[2*i+1] = HexToAscii(datebuf[5-i]&0x0f);
	}
	str_check_data.user_id = UserNum;
	str_check_data.read_num = TimeNum;
	str_check_data.time_path[3]=ASCIIData[0];
	str_check_data.time_path[4]=ASCIIData[1];
	str_check_data.time_path[6]=ASCIIData[2];
	str_check_data.time_path[7]=ASCIIData[3];
	str_check_data.time_path[9]=ASCIIData[4];
	str_check_data.time_path[10]=ASCIIData[5];		
	FreshSetUserData(ASCIIData1,User_Num,ASCIIData);
	
	i = 0;
	line1 = TabMenu[0].startline;
	line2 = TabMenu[1].startline;//3
	line3 = TabMenu[2].startline;//3
	
	column1 = TabMenu[0].startcolumn;//14
	column2 = TabMenu[1].startcolumn;//16
	column3 = TabMenu[2].startcolumn;//16
	
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	while(OK_Flag == FALSE)
	{
		FreshSetUserData(ASCIIData1,User_Num,ASCIIData);
		
		if(Tab_Flag == TRUE)
		{
			ReverseShowLine_len(TabMenu[TabNum].startline,TabMenu[TabNum].startcolumn,TabMenu[TabNum].columnnum);
			status = GetKeyStatus();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
					if(TabNum == 0)
					{
						TabNum = MaxTabNum;
					}
					else
					{
						TabNum = TabNum - 1;
					}
					break;
				case KEY_DOWN://���¼�
					if(TabNum == MaxTabNum)
					{
						TabNum = 0;
					}
					else
					{
						TabNum = TabNum + 1;
					}
					break;			
				case KEY_LEFT://�����	
					break;
				case KEY_RIGHT://���Ҽ�	
					break;
				case KEY_ENTER://�س���
					if(TabNum == MaxTabNum)
					{
						OK_Flag = TRUE;
					}
					else
					{
						Tab_Flag = (!Tab_Flag);
					}
					break;
				case KEY_EXIT://���ؼ�
					KeyFuncIndex = 30;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					return;			
					
				default:	
					break;
			}		
		}
		else
		{
			if(TabNum == 0)
			{
				ReverseShowChar(line1,column1);
				status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				switch(status)
				{
					case KEY_UP://���ϼ�
						if(User_Num[i] == 0x39)
						{
							User_Num[i] = 0x30;
						}
						else
						{
							User_Num[i] = User_Num[i] + 1;
						}
						break;
					case KEY_DOWN://���¼�
						if(User_Num[i] == 0x30)
						{
							User_Num[i] = 0x39;
						}
						else
						{
							User_Num[i] = User_Num[i] - 1;
						}	
						break;			
					case KEY_LEFT://�����
						ShowChar(line1,column1,User_Num[i]);
						if(i > 0)
						{
							column1 = column1 - 3;
							i = i - 1;
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line1,column1,User_Num[i]);
						if(i < 2)
						{
							column1 = column1 + 3;
							i = i + 1;
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						i = 0;
						column1 = TabMenu[0].startcolumn;//0
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 32;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					
					default:	
						break;
				}

				UserNum = (User_Num[0]-0x30)*100+ (User_Num[1]-0x30)*10 + (User_Num[2]-0x30);
				str_check_data.user_id=UserNum;
			}
			else if(TabNum == 1)
			{
				ReverseShowChar(line2,column2);
				status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				switch(status)
				{
					case KEY_UP://���ϼ�
						if(ASCIIData1[i] == 0x39)
						{
							ASCIIData1[i] = 0x30;
						}
						else
						{
							ASCIIData1[i] = ASCIIData1[i] + 1;
						}
						break;
					case KEY_DOWN://���¼�
						if(ASCIIData1[i] == 0x30)
						{
							ASCIIData1[i] = 0x39;
						}
						else
						{
							ASCIIData1[i] = ASCIIData1[i] - 1;
						}	
						break;			
					case KEY_LEFT://�����
						ShowChar(line2,column2,ASCIIData1[i]);
						if(i > 0)
						{
							column2 = column2 - 3;
							i = i - 1;
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line2,column2,ASCIIData1[i]);
						if(i < 1)
						{
							column2 = column2 + 3;
							i = i + 1;
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						i = 0;
						column2 = TabMenu[1].startcolumn;//0					
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 32;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					
					default:	
						break;
				}

				TimeNum = (ASCIIData1[0]-0x30)*10+ (ASCIIData1[1]-0x30);
				str_check_data.read_num = TimeNum;
			}
			else if(TabNum == 2)
			{
				ReverseShowChar(line3,column3);
				status = GetKeyStatus();//�д��޸�Ϊ�õ����̵�״̬����
				switch(status)
				{
					case KEY_UP://���ϼ�
						if(i == 2)//�ж��·����ֻ�ܵ�1
						{
							if(ASCIIData[i] == 0x31)
							{
								ASCIIData[i] = 0x30;
							}
							else
							{
								ASCIIData[i] = ASCIIData[i] + 1;
							}
						}
						else if(i == 3)
						{
							if(ASCIIData[2] == 0x31)
							{
								if(ASCIIData[i] == 0x32)
								{
									ASCIIData[i] = 0x30;
								}
								else
								{
									ASCIIData[i] = ASCIIData[i] + 1;
								}
							}
							else
							{
								if(ASCIIData[i] == 0x39)
								{
									ASCIIData[i] = 0x30;
								}
								else
								{
									ASCIIData[i] = ASCIIData[i] + 1;
								}
							}
						}
						else if(i == 4 )
						{
							if(ASCIIData[i] == 0x33)
							{
								ASCIIData[i] = 0x30;
							}
							else
							{
								ASCIIData[i] = ASCIIData[i] + 1;
							}
						}
						else if(i == 5)
						{
							if(ASCIIData[4] == 0x33)
							{
								if(ASCIIData[i] == 0x31)
								{
									ASCIIData[i] = 0x30;
								}
								else
								{
									ASCIIData[i] = ASCIIData[i] + 1;
								}
							}
							else
							{
								if(ASCIIData[i] == 0x39)
								{
									ASCIIData[i] = 0x30;
								}
								else
								{
									ASCIIData[i] = ASCIIData[i] + 1;
								}
							}
						}
						else
						{
							if(ASCIIData[i] == 0x39)
							{
								ASCIIData[i] = 0x30;
							}
							else
							{
								ASCIIData[i] = ASCIIData[i] + 1;
							}
						}
						break;
					case KEY_DOWN://���¼�
						if(i == 2)//�ж��·����ֻ�ܵ�1
						{
							if(ASCIIData[i] == 0x30)
							{
								ASCIIData[i] = 0x31;
							}
							else
							{
								ASCIIData[i] = ASCIIData[i] - 1;
							}
						}
						else if(i == 3)
						{
							if(ASCIIData[2] == 0x31)
							{
								if(ASCIIData[i] == 0x30)
								{
									ASCIIData[i] = 0x32;
								}
								else
								{
									ASCIIData[i] = ASCIIData[i] - 1;
								}
							}
							else
							{
								if(ASCIIData[i] == 0x30)
								{
									ASCIIData[i] = 0x39;
								}
								else
								{
									ASCIIData[i] = ASCIIData[i] - 1;
								}
							}
						}
						else if(i == 4 )
						{
							if(ASCIIData[i] == 0x30)
							{
								ASCIIData[i] = 0x33;
							}
							else
							{
								ASCIIData[i] = ASCIIData[i] - 1;
							}
						}
						else if(i == 5)
						{
							if(ASCIIData[4] == 0x33)
							{
								if(ASCIIData[i] == 0x30)
								{
									ASCIIData[i] = 0x31;
								}
								else
								{
									ASCIIData[i] = ASCIIData[i] - 1;
								}
							}
							else
							{
								if(ASCIIData[i] == 0x30)
								{
									ASCIIData[i] = 0x39;
								}
								else
								{
									ASCIIData[i] = ASCIIData[i] - 1;
								}
							}
						}
						else
						{
							if(ASCIIData[i] == 0x30)
							{
								ASCIIData[i] = 0x39;
							}
							else
							{
								ASCIIData[i] = ASCIIData[i] - 1;
							}
						}
						break;			
					case KEY_LEFT://�����
						ShowChar(line3,column3,ASCIIData[i]);
						if(i > 0)
						{
							if(i%2)
							{
								column3 = column3 - 3;
							}
							else
							{
								column3 = column3 - 8;
							}
							i = i - 1;
							if((ASCIIData[2]==0x31)&&(ASCIIData[3]>0x32))
							{
								ASCIIData[3] = 0x30;
							}
							if((ASCIIData[4]==0x33)&&(ASCIIData[5]>0x31))
							{	
								ASCIIData[5] = 0x30;
							}
						}
						break;
					case KEY_RIGHT://���Ҽ�
						ShowChar(line1,column3,ASCIIData[i]);
						if(i < 5)
						{
							if(i%2)
							{
								column3= column3 + 8;
							}
							else
							{
								column3 = column3 + 3;
							}
							i = i + 1;
							if((ASCIIData[2]==0x31)&&(ASCIIData[3]>0x32))
							{
								ASCIIData[3] = 0x30;
							}
							if((ASCIIData[4]==0x33)&&(ASCIIData[5]>0x31))
							{	
								ASCIIData[5] = 0x30;
							}
						}
						break;
					case KEY_ENTER://�س���
						Tab_Flag = (!Tab_Flag);
						i = 0;
						column3 = TabMenu[0].startcolumn;//0	
						TabNum = TabNum + 1;//ȷ��ʱ�Զ�ת����һ��Ҫ���õ�λ��
						break;
					case KEY_EXIT://���ؼ�
						KeyFuncIndex = 32;
						(*KeyTab[KeyFuncIndex].CurrentOperate)();
						return;			
					
					default:	
						break;
				}
				//�˴���Ҫ����ʱ��ת��������ȥ�����ļ�
				str_check_data.time_path[3]=ASCIIData[0];
				str_check_data.time_path[4]=ASCIIData[1];
				str_check_data.time_path[6]=ASCIIData[2];
				str_check_data.time_path[7]=ASCIIData[3];
				str_check_data.time_path[9]=ASCIIData[4];
				str_check_data.time_path[10]=ASCIIData[5];			
			}
		}//end if(Tab_Flag == TRUE)
	}//end while	

    ShowMenuBottom(8);
	get_sd_display_data();
	KeyFuncIndex = 34;
	(*KeyTab[KeyFuncIndex].CurrentOperate)();
	return;

}
uint8  HexToAscii_1(uint16 Hex_bcd_code,uint8* AscBuf)//16λ��HEX��ת��ΪASCII�룬Ȼ�󷴷�������AscBuf�У�����Ϊnum
{
	uint8  num = 0;
	//uint8  TempBuf;
	uint16 b1,b2;
	b2 = Hex_bcd_code;
	if(b2==0)//Hex_bcd_codeΪ0�����
	{
		AscBuf[num] = 0x30;
		num = 1;
	}
	while(b2)
	{
		b1 = b2%16;
		AscBuf[num] = HexToAscii(b1);
		b2 = b2/16;
		num++;
	}
	return num;
}

void  DeciToAscii(uint16 code,uint8* AscBuf)//10���Ƶ�HEX��ת��ΪASCII�룬Ȼ�󷴷�������AscBuf�У�����Ϊnum
{
	AscBuf[0] = HexToAscii(code/100);
	AscBuf[1] = HexToAscii((code - (code/100)*100)/10);
	AscBuf[2] = HexToAscii(code%10);
}

uint8	Hex_BcdToAscii_32(uint32 Hex_bcd_code,uint8* AscBuf)//16λ��HEX����BCD��ת��ΪASCII�룬Ȼ�󷴷�������AscBuf�У�����Ϊnum
{
	uint8  num = 0;
	//uint8  TempBuf;
	uint32 b1,b2;
	b2 = Hex_bcd_code;
	if(b2==0)//Hex_bcd_codeΪ0�����
	{
		AscBuf[num] = 0x30;
		num = 1;
	}
	while(b2)
	{
		b1 = b2%16;
		AscBuf[num] = HexToAscii(b1);
		b2 = b2/16;
		num++;
	}
	return num;
}
/****************************************************************************************************
**	�� ��  �� ��: FreshDisplayData
**	�� ��  �� ��: ˢ����ʾ����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void FreshDisplayData(void)
{
	uint8 ASCIIData[20];
	uint8 i,num;

	display_white();
	ShowMenuTop();
	ShowMenuBottom(25);

	memset(ASCIIData,'0', 20);
	ShowLineChar_HZ(1,0,&MenuHZ_user_num[0],4); //�û����
	DeciToAscii(strDisplayData.user_id,ASCIIData);
	for(i=0;i<3;i++)
	{	
		ShowChar(1, 19+i*3, ASCIIData[i]);
	}
	
	ShowLineChar_HZ(1,28,&MenuHZ_user_area[0],2);//�û����
	//DeciToAscii(strDisplayData.area,ASCIIData);
	num = Hex_BcdToAscii(strDisplayData.area,ASCIIData);
	for(i=0;i<num;i++)
	{	
		ShowChar(1, 38+i*3, ASCIIData[num-1-i]);
	}
	ShowHZ(1,38+i*3,0x0185);//�O
	
	ShowLineChar_HZ(2,0,&MenuHZ_device_addr[0],4);//�豸��ַ
	HexToAscii_1(strDisplayData.address, (uint8 *)ASCIIData);
	for(i=0;i<4;i++)
	{	
		ShowChar(2, 30+i*3, ASCIIData[3-i]);
	}
	
	ShowLineChar_HZ(3,0,&MenuHZ_open_time[0],4);//����ʱ��
	num = Hex_BcdToAscii_32(strDisplayData.open_time,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(3, 22+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(3, 22+i*3, ASCIIData[num-1-i]);
		}
	}
	
	ShowChar(3, 22+i*3, 0x2E);
	ShowChar(3, 22+(i+1)*3, ASCIIData[1]);
	ShowChar(3, 22+(i+2)*3, ASCIIData[0]);
	ShowChar(3, 22+(i+3)*3, 'h');
	ShowLineChar_HZ(4,0,&MenuHZ_indoor_temp[0],4);//�����¶�
	memset(ASCIIData,'0',sizeof(ASCIIData));
	num = Hex_BcdToAscii_32(strDisplayData.room_temperature,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(3, 22+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(4, 22+i*3, ASCIIData[num-1-i]);
		}
	}
	ShowChar(4, 22+2*3, 0x2E);
	ShowChar(4, 22+3*3, ASCIIData[1]);
	ShowChar(4, 22+4*3, ASCIIData[0]);
	ShowHZ(4,38,0x0184);//��
	ShowLineChar_HZ(5,0,&MenuHZ_water_temp_in[0],4);//��ˮ�¶�
	/*yangfei added for*/
	memset(ASCIIData,'0',sizeof(ASCIIData));
	/**/
	num = Hex_BcdToAscii_32(strDisplayData.forward_temperature,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(3, 22+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(5, 22+i*3, ASCIIData[num-1-i]);
		}
	}
	ShowChar(5, 22+2*3, 0x2E);
	ShowChar(5, 22+3*3, ASCIIData[1]);
	ShowChar(5, 22+4*3, ASCIIData[0]);
	ShowHZ(5,38,0x0184);//��

	ShowLineChar_HZ(6,0,&MenuHZ_water_temp_out[0],4);//��ˮ�¶�
	memset(ASCIIData,'0',sizeof(ASCIIData));
	num = Hex_BcdToAscii_32(strDisplayData.return_temperature,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(3, 22+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(6, 22+i*3, ASCIIData[num-1-i]);
		}
	}
	ShowChar(6, 22+2*3, 0x2E);
	ShowChar(6, 22+3*3, ASCIIData[1]);
	ShowChar(6, 22+4*3, ASCIIData[0]);
	ShowHZ(6,38,0x0184);//��

	memset(ASCIIData,'0',sizeof(ASCIIData));
	ShowLineChar_HZ(7,0,&MenuHZ_proportion_energy[0],4);//��̯���� 
	num =Hex_BcdToAscii_32(strDisplayData.proportion_energy,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(7, 20+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(7, 20+i*3, ASCIIData[num-1-i]);
		}
	}
	
	ShowChar(7, 20+i*3, 0x2E);
	ShowChar(7, 20+(i+1)*3-2, ASCIIData[1]);
	ShowChar(7, 20+(i+2)*3-2, ASCIIData[0]);
	ShowChar(7, 20+(i+3)*3-2, 'k');
	ShowChar(7, 20+(i+4)*3-2, 'w');
	ShowChar(7, 20+(i+5)*3-2, 'h');
	ShowLineChar_HZ(8,0,&MenuHZ_total_energy[0],3);//������
	num =Hex_BcdToAscii_32(strDisplayData.total_energy,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(8, 15+i*3, 0x30);
		i++;
	}
	for(i=0;i<num-2;i++)
	{	
		ShowChar(8, 15+i*3, ASCIIData[num-1-i]);
	}
	ShowChar(8, 15+i*3, 0x2E);
	ShowChar(8, 15+(i+1)*3, ASCIIData[1]);
	ShowChar(8, 15+(i+2)*3, ASCIIData[0]);
	ShowChar(8, 15+(i+3)*3, 'k');
	ShowChar(8, 15+(i+4)*3, 'w');
	ShowChar(8, 15+(i+5)*3, 'h');
	
}


/****************************************************************************************************
**	�� ��  �� ��: Display_Data_Menu
**	�� ��  �� ��: ����չʾ����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Display_Data_Menu(void)
{
	uint8 status;
	bool OK_Flag = FALSE;
	
	while(OK_Flag == FALSE)
	{
		FreshDisplayData();
		status = GetKeyStatuschaobiao();//�õ����̵�״̬
		switch(status)
		{
			case KEY_UP://���ϼ�
			case KEY_RIGHT://���Ҽ�	
			{
				str_check_data.user_id++;
				get_sd_display_data();
				break;
			}
			case KEY_DOWN://���¼�
			case KEY_LEFT://�����
			{
				str_check_data.user_id--;
				get_sd_display_data();
				break;
			}			
			case KEY_ENTER://�س���
			{
				break;
			}
			case KEY_EXIT://���ؼ�
			{
				KeyFuncIndex = 32;
				(*KeyTab[KeyFuncIndex].CurrentOperate)();
				OK_Flag = TRUE;
				break;
			}		
			default:	
				break;
		}
	}	
}


/****************************************************************************************************
**	�� ��  �� ��: Chose_metric_method
**	�� ��  �� ��: ѡ���������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Chose_metric_method(void)
{
	uint8 status;
	uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
	bool OK_Flag = FALSE;
	uint8 DeviceType;
	while(OK_Flag == FALSE)
	{
		FreshMethodshow(now_line);
		status = GetKeyStatuschaobiao();//�õ����̵�״̬
		switch(status)
		{
			case KEY_UP://���ϼ�
			{
				if(now_line>2)
				{
					now_line--;
				}
				else
				{
					now_line = 4;
				}
				break;
			}
			case KEY_DOWN://���¼�
			{
				if(now_line<4)
				{
					now_line++;
				}
				else
				{
					now_line = 2;
				}
				break;
			}			
			case KEY_LEFT://�����	
			case KEY_RIGHT://���Ҽ�	
			{
				break;
			}
			case KEY_ENTER://�س���
			{
				OK_Flag = TRUE;
				if(now_line ==2)
				{
					DeviceType = HEAT_METER_TYPE;
				}
				else if(now_line ==3)/*ʱ��ͨ�������*/
				{
					DeviceType = TIME_ON_OFF_AREA_TYPE;
				}
				else if(now_line ==4)/*����ʽ������*/
				{
					DeviceType = HEATCOST_METER_TYPE;
				}
				else
				{
					DeviceType = HEAT_METER_TYPE;
				}
				/*begin:yangfei added 20140226 for ����sd����������*/
				SaveDeviceType(DeviceType);
				/*end:yangfei added 20140226 */
				now_line= 8;
				FreshMethodshow(now_line);
				break;
				
			}
			case KEY_EXIT://���ؼ�
			{
				KeyFuncIndex = 31;
				(*KeyTab[KeyFuncIndex].CurrentOperate)();
				OK_Flag = TRUE;
				break;
			}		
			default:	
				break;
		}
	}
	
}
/****************************************************************************************************
**	�� ��  �� ��: FactroytestSDCardData
**	�� ��  �� ��: ˢ�³���������ʾ����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
	typedef struct{
			uint16 WriteFlag;					//�������Ƿ�д������
			uint16 MeterStoreNums;				//���洢���ٿ������Ϣ
		}MeterFilePara1;

uint8  FactroytestSDCardData(void)//muxiaoqing test

{
//	uint8 ASCIIData[20];
//	uint8 i,num;
	uint8 Err 	= 0;
//	char  NodePath[] = "/2012/12/24/test.txt";;
	//uint16 testNum			= 16;							//test
	
	MeterFilePara1	para;
	
	TermParaSaveType TermPara;
	if(!autotestmode)
		{
        	display_white();
        	ShowMenuTop();
        	ShowMenuBottom(25);
        	ShowLineChar_HZ(2,2,&MenuHZTesting[0],6);//
        //	memset(ASCIIData,'0', 20);
        	ShowLineChar_HZ(1,0,&Sdtest[0],3); //sd ��
        	
			OSTimeDly(OS_TICKS_PER_SEC);
		}
//	DeciToAscii(strDisplayData.user_id,ASCIIData);
	manualtestmode = TRUE;

	para.WriteFlag = 0xAA55;
	para.MeterStoreNums = 0x0016;

	Err = SDSaveData("/TEST_FILE_ADDR", &para, sizeof(MeterFilePara1),0); 
	if(Err!=NO_ERR)	
	  {
	   // debug("%s %d read TIME_NODE_ADDR err=%d!\r\n",__FUNCTION__,__LINE__,Err);
		
		//len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
	//	len = sprintf((char*)&sdtestBuff[0], "%s", sdtestwriterr);
	//	ShowLineChar(3, 7, &sdtestBuff[0], len);
		return Err=1;
	  }
	else
		{
			   // debug("%s %d read TIME_NODE_ADDR err=%d!\r\n",__FUNCTION__,__LINE__,Err);
				
				//len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
		//		len = sprintf((char*)&sdtestBuff[0], "%s", sdtestwritOK);
		//		ShowLineChar(3, 7, &sdtestBuff[0], len);
        	//memset(sdtestBuff,0,sizeof(sdtestBuff));
			
			para.WriteFlag = 0x00;
			para.MeterStoreNums = 0x00;
        	//testNum = 0;
			MakeFile("/test");
			do{
				OSMutexPend (FlashMutex,0,&Err);
				Err = SDReadData("/test", &TermPara, sizeof(TermParaSaveType), 0);
				OSMutexPost (FlashMutex);
				if(Err!=NO_ERR) 
				  {
					debug("%s %d read test err=%d!\r\n",__FUNCTION__,__LINE__,Err);
				  }
			}while(Err!=NO_ERR);
			/*end:yangfei modified 2013-4-3 */
			OSMutexPend (FlashMutex,0,&Err);
			Err = SDReadData("/TEST_FILE_ADDR", &para, sizeof(MeterFilePara1), 0);
			OSMutexPost (FlashMutex);
        	
			
        	if(Err!=NO_ERR)
        	{
        		//len = sprintf((char*)&sdtestBuff[0], "%s", sdtestreaderr);
        		//ShowLineChar(5, 7, &sdtestBuff[0], len);
        		return Err=2;
        	}
			
			else if(para.WriteFlag == 0xAA55)
			
			{
        		//len = sprintf((char*)&sdtestBuff[0], "%s", sdtestOK);
        		//ShowLineChar(7, 7, &sdtestBuff[0], len);
        		return 0;

			}
			else
				{
				return Err =3;
				}
		}
#if 0
	for(i=0;i<3;i++)
	{	
		ShowChar(1, 19+i*3, ASCIIData[i]);
	}
	
	ShowLineChar_HZ(1,28,&MenuHZ_user_area[0],2);//�û����
	//DeciToAscii(strDisplayData.area,ASCIIData);
	num = Hex_BcdToAscii(strDisplayData.area,ASCIIData);
	for(i=0;i<num;i++)
	{	
		ShowChar(1, 38+i*3, ASCIIData[num-1-i]);
	}
	ShowHZ(1,38+i*3,0x0185);//�O
	
	ShowLineChar_HZ(2,0,&MenuHZ_device_addr[0],4);//�豸��ַ
	HexToAscii_1(strDisplayData.address, (uint8 *)ASCIIData);
	for(i=0;i<4;i++)
	{	
		ShowChar(2, 30+i*3, ASCIIData[3-i]);
	}
	
	ShowLineChar_HZ(3,0,&MenuHZ_open_time[0],4);//����ʱ��
	num = Hex_BcdToAscii_32(strDisplayData.open_time,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(3, 22+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(3, 22+i*3, ASCIIData[num-1-i]);
		}
	}
	
	ShowChar(3, 22+i*3, 0x2E);
	ShowChar(3, 22+(i+1)*3, ASCIIData[1]);
	ShowChar(3, 22+(i+2)*3, ASCIIData[0]);
	ShowChar(3, 22+(i+3)*3, 'h');
	ShowLineChar_HZ(4,0,&MenuHZ_indoor_temp[0],4);//�����¶�
	memset(ASCIIData,'0',sizeof(ASCIIData));
	num = Hex_BcdToAscii_32(strDisplayData.room_temperature,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(3, 22+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(4, 22+i*3, ASCIIData[num-1-i]);
		}
	}
	ShowChar(4, 22+2*3, 0x2E);
	ShowChar(4, 22+3*3, ASCIIData[1]);
	ShowChar(4, 22+4*3, ASCIIData[0]);
	ShowHZ(4,38,0x0184);//��
	ShowLineChar_HZ(5,0,&MenuHZ_water_temp_in[0],4);//��ˮ�¶�
	/*yangfei added for*/
	memset(ASCIIData,'0',sizeof(ASCIIData));
	/**/
	num = Hex_BcdToAscii_32(strDisplayData.forward_temperature,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(3, 22+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(5, 22+i*3, ASCIIData[num-1-i]);
		}
	}
	ShowChar(5, 22+2*3, 0x2E);
	ShowChar(5, 22+3*3, ASCIIData[1]);
	ShowChar(5, 22+4*3, ASCIIData[0]);
	ShowHZ(5,38,0x0184);//��

	ShowLineChar_HZ(6,0,&MenuHZ_water_temp_out[0],4);//��ˮ�¶�
	memset(ASCIIData,'0',sizeof(ASCIIData));
	num = Hex_BcdToAscii_32(strDisplayData.return_temperature,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(3, 22+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(6, 22+i*3, ASCIIData[num-1-i]);
		}
	}
	ShowChar(6, 22+2*3, 0x2E);
	ShowChar(6, 22+3*3, ASCIIData[1]);
	ShowChar(6, 22+4*3, ASCIIData[0]);
	ShowHZ(6,38,0x0184);//��

	memset(ASCIIData,'0',sizeof(ASCIIData));
	ShowLineChar_HZ(7,0,&MenuHZ_proportion_energy[0],4);//��̯���� 
	num =Hex_BcdToAscii_32(strDisplayData.proportion_energy,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(7, 20+i*3, 0x30);
		i++;
	}
	else
	{
		for(i=0;i<num-2;i++)
		{	
			ShowChar(7, 20+i*3, ASCIIData[num-1-i]);
		}
	}
	
	ShowChar(7, 20+i*3, 0x2E);
	ShowChar(7, 20+(i+1)*3-2, ASCIIData[1]);
	ShowChar(7, 20+(i+2)*3-2, ASCIIData[0]);
	ShowChar(7, 20+(i+3)*3-2, 'k');
	ShowChar(7, 20+(i+4)*3-2, 'w');
	ShowChar(7, 20+(i+5)*3-2, 'h');
	ShowLineChar_HZ(8,0,&MenuHZ_total_energy[0],3);//������
	num =Hex_BcdToAscii_32(strDisplayData.total_energy,ASCIIData);
	if(num<=2)//�ж��Ƿ�ֻ��С��
	{
		i=0;
		ShowChar(8, 15+i*3, 0x30);
		i++;
	}
	for(i=0;i<num-2;i++)
	{	
		ShowChar(8, 15+i*3, ASCIIData[num-1-i]);
	}
	ShowChar(8, 15+i*3, 0x2E);
	ShowChar(8, 15+(i+1)*3, ASCIIData[1]);
	ShowChar(8, 15+(i+2)*3, ASCIIData[0]);
	ShowChar(8, 15+(i+3)*3, 'k');
	ShowChar(8, 15+(i+4)*3, 'w');
	ShowChar(8, 15+(i+5)*3, 'h');
	#endif
	
	manualtestmode = FALSE;
}

/****************************************************************************************************
**	�� ��  �� ��:factory_test_method_sd
**	�� ��  �� ��:
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void factory_test_method_sd(void)
{
	uint8 status;
	uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
	bool OK_Flag = FALSE;
	//uint8 DeviceType;
	uint8 err;	
	uint8 len;
    char  sdtestwriterr[] = "SD write erro ";	
    char  sdtestwritOK[] = "SD write OK ";
    char  sdtestreaderr[] = "SD read erro ";	
    char  sdtestOK[] = "SD test OK ";	
	uint8 sdtestBuff[15] = {0};
	err = FactroytestSDCardData();
	while(OK_Flag == FALSE)
	{
	    
		display_white();
		ShowMenuTop();
		ShowMenuBottom(25);
		//FreshMethodshow(now_line);
		
		if(err == 1) 
		  {
		   // debug("%s %d read TIME_NODE_ADDR err=%d!\r\n",__FUNCTION__,__LINE__,Err);
			
			//len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
			len = sprintf((char*)&sdtestBuff[0], "%s", sdtestwriterr);
			ShowLineChar(3, 7, &sdtestBuff[0], len);
		  }
		else if(err == 2)
		  {
        	 len = sprintf((char*)&sdtestBuff[0], "%s", sdtestreaderr);
        	 ShowLineChar(3, 7, &sdtestBuff[0], len);
          }
		else if (err ==0)
		  {
        	 len = sprintf((char*)&sdtestBuff[0], "%s", sdtestOK);
        	 ShowLineChar(3, 7, &sdtestBuff[0], len);

			}	
		status = GetKeyStatuschaobiao();//�õ����̵�״̬
		switch(status)
		{
			case KEY_UP://���ϼ�
			{
				if(now_line>2)
				{
					now_line--;
				}
				else
				{
					now_line = 4;
				}
				break;
			}
			case KEY_DOWN://���¼�
			{
				if(now_line<4)
				{
					now_line++;
				}
				else
				{
					now_line = 2;
				}
				break;
			}			
			case KEY_LEFT://�����	
			case KEY_RIGHT://���Ҽ�	
			{
				break;
			}
			case KEY_ENTER://�س���
			{
				#if 0
				OK_Flag = TRUE;
				if(now_line ==2)
				{
					DeviceType = HEAT_METER_TYPE;
				}
				else if(now_line ==3)/*ʱ��ͨ�������*/
				{
					DeviceType = TIME_ON_OFF_AREA_TYPE;
				}
				else if(now_line ==4)/*����ʽ������*/
				{
					DeviceType = HEATCOST_METER_TYPE;
				}
				else
				{
					DeviceType = HEAT_METER_TYPE;
				}
				/*begin:yangfei added 20140226 for ����sd����������*/
				SaveDeviceType(DeviceType);
				/*end:yangfei added 20140226 */
				now_line= 8;
				FreshMethodshow(now_line);
				#endif
				break;
				
			}
			case KEY_EXIT://���ؼ�
			{
				KeyFuncIndex = 38;
				(*KeyTab[KeyFuncIndex].CurrentOperate)();
				OK_Flag = TRUE;
				break;
			}		
			default:	
				break;
		}
	}
	
}
/****************************************************************************************************
**	�� ��  �� ��: Set_SystemTime
**	�� ��  �� ��: �ն�ʱ������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Factory_test_Set_SystemTime(void)  //�ն�ʱ������

{
	uint8 err;
	uint8 line1,line2;
	uint8 column1,column2;
	uint8 i,j;
	uint8 datebuf[6];
	uint8 ASCIIData[12];
	uint8 HexData[6];
	
	SelectInfo TabMenu[] = {{2,0,33},{3,0,33},{4,22,11}};//�У��У�����
	//uint8 TabNum = 0;
	uint8 MaxTabNum = 0;
	//bool OK_Flag = FALSE;
	//bool Tab_Flag = TRUE;
	
	if(!autotestmode)
		{
        	display_white();
        	ShowMenuTop();
        	ShowMenuBottom(25);
        	ShowLineChar_HZ(2,2,&MenuHZTesting[0],6);//
		}
		
	//����ǰʱ��
	//UserReadDateTime(&datebuf[0]);//���ʱ������
	ReadDateTime(&datebuf[0]);
	/*begin:yangfei added 2013-06-04 for ʱ����Ч���ж� */
	#if 0
	for(i=0;i<6;i++)
		{
	debug("%d ",datebuf[i]);

		}
	#endif
	if(datebuf[0]>60)
		{
		datebuf[0]=1;
		datebuf[1]=1;
		datebuf[2]=1;
		datebuf[3]=1;
		datebuf[4]=1;
		datebuf[5]=9;
		}
	/*end:yangfei added 2013-06-04 for ʱ����Ч���ж� */
	datebuf[4] = datebuf[4]&0x1F;//ȥ������
	for(i=0;i<6;i++)
	{
		ASCIIData[2*i] = HexToAscii(datebuf[5-i]>>4);
		ASCIIData[2*i+1] = HexToAscii(datebuf[5-i]&0x0f);
	}
	i = 0;
	j = 6;
	line1 = TabMenu[0].startline;//2
	line2 = TabMenu[1].startline;//3
	column1 = TabMenu[0].startcolumn;//0
	column2 = TabMenu[1].startcolumn;//0
	
	MaxTabNum = (sizeof(TabMenu)/sizeof(SelectInfo)) - 1;//����±�
	//end while	
	
	//==========================================
	//�˴���Ҫ��������ʱ�亯����Ȼ����ݷ���ֵ�Ľ���ж��Ƿ����óɹ�
	for(i=0;i<6;i++)//��ASC��ת��ΪBCD����������---���˳����
	{
		HexData[5-i] = ((ASCIIData[2*i]-0x30)<<4) + (ASCIIData[2*i+1]-0x30);
	}
	HexData[4] = HexData[4]|0x20;
	err = TimeCheck(&HexData[0]);
	if(err != 0)//����Ҫ��
	{
		if(err == 1)///���ڲ���Ҫ��
		{
			ShowMenuBottom(7);
		}
		else if(err == 2)//ʱ�䲻�Ϸ�
		{
			ShowMenuBottom(8);
		}
//		OSTimeDly(OS_TICKS_PER_SEC*2);
//		KeyFuncIndex = 37;
//		(*KeyTab[KeyFuncIndex].CurrentOperate)();
//		return;
	}
	
	err = UserSetDateTime(&HexData[0]);
	ShowMenuBottom(2);//��Ҫ����ʱ�����ú����ķ���ֵ���ڵײ�״̬����ʾ�����Ƿ�ɹ�
//	OSTimeDly(OS_TICKS_PER_SEC*2);
	return err;
}

/****************************************************************************************************
**	�� ��  �� ��:factory_test_method_time
**	�� ��  �� ��:
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void factory_test_method_time(void)
{
	uint8 status;
	uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
	bool OK_Flag = FALSE;
	//uint8 DeviceType;	
	uint8 Err 	= 0;	
	uint8 len;
    char  timetesterr[] = "time test erro ";	
    char  timetestOK[] = "time test OK ";	
	uint8 timetestBuff[15] = {0};
	
	Err = Factory_test_Set_SystemTime();
	while(OK_Flag == FALSE)
	{
	
	display_white();
	ShowMenuTop();
	ShowMenuBottom(25);
		if(Err!=NO_ERR) 
		  {
		   // debug("%s %d read TIME_NODE_ADDR err=%d!\r\n",__FUNCTION__,__LINE__,Err);
			
			//len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
			len = sprintf((char*)&timetestBuff[0], "%s", timetesterr);
			ShowLineChar(3, 7, &timetestBuff[0], len);
			
		  }
		else
			{
				   // debug("%s %d read TIME_NODE_ADDR err=%d!\r\n",__FUNCTION__,__LINE__,Err);
					
					//len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
					len = sprintf((char*)&timetestBuff[0], "%s", timetestOK);
					ShowLineChar(3, 7, &timetestBuff[0], len);
					
			}
		
		status = GetKeyStatuschaobiao();//�õ����̵�״̬
		switch(status)
		{
			case KEY_UP://���ϼ�
			{
				if(now_line>2)
				{
					now_line--;
				}
				else
				{
					now_line = 4;
				}
				break;
			}
			case KEY_DOWN://���¼�
			{
				if(now_line<4)
				{
					now_line++;
				}
				else
				{
					now_line = 2;
				}
				break;
			}			
			case KEY_LEFT://�����	
			case KEY_RIGHT://���Ҽ�	
			{
				break;
			}
			case KEY_ENTER://�س���
			{
				#if 0
				OK_Flag = TRUE;
				if(now_line ==2)
				{
					DeviceType = HEAT_METER_TYPE;
				}
				else if(now_line ==3)/*ʱ��ͨ�������*/
				{
					DeviceType = TIME_ON_OFF_AREA_TYPE;
				}
				else if(now_line ==4)/*����ʽ������*/
				{
					DeviceType = HEATCOST_METER_TYPE;
				}
				else
				{
					DeviceType = HEAT_METER_TYPE;
				}
				/*begin:yangfei added 20140226 for ����sd����������*/
				SaveDeviceType(DeviceType);
				/*end:yangfei added 20140226 */
				now_line= 8;
				FreshMethodshow(now_line);
				#endif
				break;

			}
			case KEY_EXIT://���ؼ�
			{
				KeyFuncIndex = 39;
				(*KeyTab[KeyFuncIndex].CurrentOperate)();
				OK_Flag = TRUE;
				break;
			}		
			default:	
				break;
		}
	}
	
}
/****************************************************************************************************
**	�� ��  �� ��:factory_test_method_GPRS
**	�� ��  �� ��:
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void factory_test_method_GPRS(void)
{

	uint8 status;
	uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
	bool OK_Flag = FALSE;
	//uint8 DeviceType;
	
//	uint8 tmpmid;
	uint8 err;
	
	char Ats_AT[]="AT\r";
	char Ata_OK[]="\r\nOK\r\n";
	uint8 len;
    char  GPRStesterr[] = "GPRS test erro ";	
    char  GPRStestOK[] = "GPRS test OK ";	
	uint8 GPRStestBuff[15] = {0};
	
	if(!autotestmode)
    {
         display_white();
     	ShowMenuTop();
     	ShowMenuBottom(25);
     	
     	ShowLineChar_HZ(2,2,&MenuHZTesting[0],6);//
	}
	err = CMD_AT_RP(Ats_AT,Ata_OK,NULL,OS_TICKS_PER_SEC,1,TRUE);
	while(OK_Flag == FALSE)
	{
		//FreshMethodshow(now_line);
		//FactroytestSDCardData();
		
		
		display_white();
		ShowMenuTop();
		ShowMenuBottom(25);

	
		if(err!=NO_ERR) 
		{
			//len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
			len = sprintf((char*)&GPRStestBuff[0], "%s", GPRStesterr);
			ShowLineChar(3, 7, &GPRStestBuff[0], len);
		}		
		else
			{
				   // debug("%s %d read TIME_NODE_ADDR err=%d!\r\n",__FUNCTION__,__LINE__,Err);
					
					//len = sprintf((char*)&ASCIIData[0],"%d",lTimingState.TimingMeterNums);
					len = sprintf((char*)&GPRStestBuff[0], "%s", GPRStestOK);
					ShowLineChar(3, 7, &GPRStestBuff[0], len);
					
			}
		
		status = GetKeyStatuschaobiao();//�õ����̵�״̬
		switch(status)
		{
			case KEY_UP://���ϼ�
			{
				if(now_line>2)
				{
					now_line--;
				}
				else
				{
					now_line = 4;
				}
				break;
			}
			case KEY_DOWN://���¼�
			{
				if(now_line<4)
				{
					now_line++;
				}
				else
				{
					now_line = 2;
				}
				break;
			}			
			case KEY_LEFT://�����	
			case KEY_RIGHT://���Ҽ�	
			{
				break;
			}
			case KEY_ENTER://�س���
			{
				#if 0
				OK_Flag = TRUE;
				if(now_line ==2)
				{
					DeviceType = HEAT_METER_TYPE;
				}
				else if(now_line ==3)/*ʱ��ͨ�������*/
				{
					DeviceType = TIME_ON_OFF_AREA_TYPE;
				}
				else if(now_line ==4)/*����ʽ������*/
				{
					DeviceType = HEATCOST_METER_TYPE;
				}
				else
				{
					DeviceType = HEAT_METER_TYPE;
				}
				/*begin:yangfei added 20140226 for ����sd����������*/
				SaveDeviceType(DeviceType);
				/*end:yangfei added 20140226 */
				now_line= 8;
				FreshMethodshow(now_line);
				#endif
				break;
				
			}
			case KEY_EXIT://���ؼ�
			{
				KeyFuncIndex = 40;
				(*KeyTab[KeyFuncIndex].CurrentOperate)();
				OK_Flag = TRUE;
				break;
			}		
			default:	
				break;
		}
	}
	
}
uint8 IfconfirmByte(uint8 data,char* confirm,uint8 p)
{
	if(data==(uint8)(confirm[p])){
		return 0;//ƥ����ѡ��Ӧ�ɹ�
	}
	else{
		return 1;//ƥ��ʧ��
	}
	
}
        uint16 QueuetestNData(void *Buf)//�� ������
{
    uint16 temp;
//	OS_CPU_SR cpu_sr;
    
    temp = 0;                                                   /* ������Ч����0 */
    if (Buf != NULL)
    {
      //  OS_ENTER_CRITICAL();
        temp = ((DataQueue *)Buf)->NData;
       // OS_EXIT_CRITICAL();
    }
    return temp;
}

uint8 PorttestGetHead(uint8 device,char* confirm,uint16 OutTime,uint8 Find, uint8 upORdown)

{
	uint8 err =0;
	uint8 headlen;
	char  TmpBuf[64];//���Թ۲���
	uint8 test_buff[512];
  #if 1		
	headlen=strlen(confirm);
	if(headlen==0||headlen>64){//headlen �������0 ����HeadStr����ǿ�;HeadStr�������HEAD_MAX_LEN���ַ�
		return 0xff;
	}
	memset(test_buff, 0x00, 512);	
	memset(TmpBuf, 0x00, 64);
	#if 1
	if(upORdown)
		{
        	switch(device)
        	{
        		case UP_COMMU_DEV_ZIGBEE:						//�����Zigbee�������ݣ�ÿ��100���ֽڣ�ÿ�η��ͼ��333MS
        				{
        			//		QueueFlush((void*)UART4RecQueue); //��ս��ն���
        		//			QueueNData((void*)UART4RecQueue); //��ս��ն���
        		while((IRQ4testBuf[strlen((char*)IRQ4testBuf)-1])!=0x0a)//���ж�
        			{
        			
					OSTimeDly(OS_TICKS_PER_SEC/4);
                     testPortreadNum++;
					 if(testPortreadNum==2)	
					 	break;
				    }
				testPortreadNum = 0;
				    if(strlen((char*)IRQ4testBuf)==0)
						err = 1;
					     //   err = strcmp(confirm, (char*)IRQ4testBuf);
        					
        					break;
        				}
				
					case UP_COMMU_DEV_485:
        				{
					#if 0		
							//count = QueueNData(UART5RecQueue);
							count = 32;
							
							for(i=0; i<count; i++)
							{
								err=UpGetch(UP_COMMU_DEV_485,buf,OS_TICKS_PER_SEC/5);
							   if(!err)
									IRQ5testBuf[i] =  *buf;
								else
								{
									break;
								}
							}
					#endif	
					
					while((IRQ5testBuf[strlen((char*)IRQ5testBuf)-1])!=0x0a)//���ж�
						{
						
						OSTimeDly(OS_TICKS_PER_SEC/4);
						 testPortreadNum++;
						 if(testPortreadNum==2) 
							break;
						}
					testPortreadNum = 0;
							//	err = strcmp(confirm, (char*)IRQ5testBuf);
							
							if(strlen((char*)IRQ5testBuf)==0)
								err = 1;
			//		if(err == NO_ERR)		
        			//		QueueFlush((void*)UART4RecQueue); //��ս��ն���
        	//				QueueNData((void*)UART5RecQueue); //��ս��ն���
        	
        					
        					break;
        				}
					case UP_COMMU_DEV_232:
        				{
        			//		QueueFlush((void*)UART4RecQueue); //��ս��ն���
        			//		QueueNData((void*)USART1RecQueue); //��ս��ն���
        			
					while((IRQ1testBuf[strlen((char*)IRQ1testBuf)-1])!=0x0a)//���ж�
						{
						OSTimeDly(OS_TICKS_PER_SEC/4);
						 testPortreadNum++;
						 if(testPortreadNum==2) 
							break;
						}
					testPortreadNum = 0;
        				//	err = strcmp(confirm, (char*)IRQ1testBuf);
						if(strlen((char*)IRQ1testBuf)==0)
							err = 1;
        					
        					break;
        				}
					case UP_COMMU_DEV_GPRS:
        				{
        			//		QueueFlush((void*)UART4RecQueue); //��ս��ն���
        			//		QueueNData((void*)USART3RecQueue_At); //��ս��ն���
        			
					while((IRQ3testBuf[strlen((char*)IRQ3testBuf)-1])!=0x0a)//���ж�
						{
						
						OSTimeDly(OS_TICKS_PER_SEC/4);
						 testPortreadNum++;
						 if(testPortreadNum==2) 
							break;
						}
					testPortreadNum = 0;
        				//	err = strcmp(confirm, (char*)IRQ3testBuf);
						if(strlen((char*)IRQ3testBuf)==0)
							err = 1;
        					
        					break;
        				}					
        	}
		}
	else
		{
		
		switch(device)
			{
			case DOWN_COMM_DEV_MBUS:						//�����Zigbee�������ݣ�ÿ��100���ֽڣ�ÿ�η��ͼ��333MS
					{
				//		QueueFlush((void*)UART4RecQueue); //��ս��ն���
				//		QueueNData((void*)USART2RecQueue); //��ս��ն���
				
				while((IRQ1testBuf[strlen((char*)IRQ2testBuf)-1])!=0x0a)//���ж�
        			{
        			
					OSTimeDly(OS_TICKS_PER_SEC/4);
                     testPortreadNum++;
					 if(testPortreadNum==2)	
					 	break;
				    }
				testPortreadNum = 0;
        				//err = strcmp((char*)confirm, (char*)IRQ2testBuf);
						
						if(strlen((char*)IRQ2testBuf)==0)
							err = 1;
						break;
					}

		    }
		}

	
//	err = strcmp(gDebugRcv, data_buf);
	#else
	if(upORdown)
	err=UpGetch(device,&data,OutTime);
	else
	err=DuGetch(device,&data,OutTime);		
	if(err){//��ʱ
	//	gtmpnum++;//Ϊ���ڴ˴������öϵ�
		return err;	
	}
  /*begin:yangfei added 2013-02-26 for test */
    test_buff[i++] = data;
	/*end:yangfei added 2013-02-26 for test */
	n=500;///��������������յ��ַ���
	flag=0;
	p=0;
	while(n>1){
		n--;
		//if(data!=(uint8)(HeadStr[p])){
		if(IfconfirmByte(data,confirm,p)>=2){
	 		if(Find==TRUE){
	 			if(p>0){//����ƥ��HeadStr�ĵ�1���ֽ��Ժ���ֽڣ���ƥ��p���ֽ� ��ƥ���p+1���ֽڳ���
	 				p=0;     //
	 				continue;//���¿�ʼƥ��HeadStr�ĵ�1���ֽ�
	 			}
	 		}
	 		else{
	 			flag=0xfe;//��ʾʧ�ܣ�������Ҫ������headlen���ַ������Բ�������
	 			TmpBuf[p]=data;//���Թ۲���
	 			TmpBuf[p+1]='\0';//���ڵ��Թ۲�
	 			p++;
	 			
	 		}
	 	}
	 	else{
	 		TmpBuf[p]=data;//���Թ۲���
	 		TmpBuf[p+1]='\0';//���ڵ��Թ۲�
	 		p++;
			
	 	}
					
		if(p==headlen){//ƥ�����
		//	gtmpnum++;//Ϊ���ڴ˴������öϵ�
			break;
		}
	//	err=UpGetch(device,&data,OutTime);//OS_TICKS_PER_SEC/5
	
	if(upORdown)
	err=UpGetch(device,&data,OutTime);
	else
	err=DuGetch(device,&data,OutTime);		
		if(err){//
			return 0xfe;	//�����м��ַ�ʱ��ʱ  //ncq080819 ����Ҳ����������ƥ���1���ַ����������泬ʱʱ��Ӧ��ô�������ð죬���ڻ�������
		}
         /*begin:yangfei added 2013-02-26 for test */
         test_buff[i++] = data;
         if(i>=100)
          {
          //debug("\r\n%s\r\n",test_buff);
          #if  0
          OSMutexPend (FlashMutex,0,&Err);
		  SDSaveData("test.txt",test_buff, 512, 512*count);
          OSMutexPost (FlashMutex);
          #endif
          i = 0;
          count++;
          }
         /*end:yangfei added 2013-02-26 for test */
	}
	if(n==1){//���Ҵ��������ƣ�Ӧ����ʧ��
		flag=0xfd;
	}
	#endif
	#endif
	//return flag;
	
	return  err;
}
void Test_SuspendGprsRecTask(void)
{
	uint8 err;
	//�ڹ���GPRS��������ǰ��ȡ����������ռ�õ��ź���GprsXmzSem,���ź������з�������Ҳ��������,
	//��������˴���,���ܵ������з�����������
	OSSemPend(GprsXmzSem, 0, &err);
	OSTaskSuspend(PRIO_TASK_GPRS_IPD);//����GPRS��������
	OSSemPost(GprsXmzSem);
}
uint8  Randtest8(void)
{
	uint8 Randtest8;
//	uint8 Time[6];

//	uint32 seed,randu32;
	CPU_SR	cpu_sr;
	
	OS_ENTER_CRITICAL();
	//seed=*((uint32*)Time);//���ʱ�����������
 //   srand(time(NULL));
    
   // Randtest8=1+(int)(10.0*rand()/(RAND_MAX+1.0));
   
  Randtest8= 1+(rand()%50);
   
   
	OS_EXIT_CRITICAL();
	
	//Randtest8=randu32%1000000;
	return Randtest8;
	
}

uint8 CMD_Port_test_Check(uint8 device,char *str,char *confirm,uint8 upORdown)// 1 ->up 0->down
{
	uint8 err =0;
	//UGprsWriteStr(str);
    char  Debugtesting[] = "������...";	
    uint8  Debugtestingbuf[24]={0} ;	
	//uint8 status;
	uint8 len;
//	uint8 i =0;
//	uint8 PreSmybol[30]={0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
	uint8 PreSmybol[512];

    len = sprintf((char*)&Debugtestingbuf[0], "%s", Debugtesting);
//	ShowLineChar(3, 7, &Debugtestingbuf[0], len);
	if(!autotestmode)
		{
        	ShowLineChar_HZ(2,2,&MenuHZTesting[0],6);//
		}
	memset(PreSmybol, Randtest8(), 512);
	//memset(PreSmybol, 0xaa, 512);
	#if 0
	for(i = 0; i<512; i ++)
		{
          PreSmybol[i] = Randtest8();
	    }
	#endif
	
#if 1	
		OSTaskSuspend(PRIO_TASK_UART_SHELL);	
		OSTaskSuspend(PRIO_FLASH_MUTEX_PIP);	
		OSTaskSuspend(PRIO_TASK_MANAGE_DEBUG_INFO); 
		OSTaskSuspend(PRIO_TASK_GPRS_IPD);	
		//Test_SuspendGprsRecTask();
		OSTaskSuspend(PRIO_TASK_UP_SEND);	
		OSTaskSuspend(PRIO_TASK_UP_ANAL_FR);	
		OSTaskSuspend(PRIO_TASK_UP_REC_GPRS);	
		OSTaskSuspend(PRIO_TASK_UP_REC_ZIGBEE); 
		OSTaskSuspend(PRIO_TASK_UP_REC_RS485);	
		OSTaskSuspend(PRIO_TASK_UP_REC_RS232);	
		OSTaskSuspend(PRIO_TASK_GPRS_MANA); 
		OSTaskSuspend(PRIO_TASK_CLOCK); 
		OSTaskSuspend(PRIO_TASK_READ_ALL_CUR);	
		OSTaskSuspend(PRIO_TASK_LOG);	
#endif	
	if(upORdown)
		{
	#if 1	
	UpDevSend(device,  (uint8*)str,	strlen(str)); 
	#else
        	//for(i=30;i>0;i--)
        		{
                	UpDevSend(device,  PreSmybol,	512); 
	//				OSTimeDly(OS_TICKS_PER_SEC*2);
        		}
	#endif		
		}
	else
	DuSend(device,  (uint8*)str,	strlen(str));	
	
	//forbidsending1 = 0;
//	OSTimeDly(OS_TICKS_PER_SEC);
//	OSTimeDly(OS_TICKS_PER_SEC/2);
#if 0
	if(upORdown)
		{
        	switch(device)
        	{
        		case UP_COMMU_DEV_ZIGBEE:						//�����Zigbee�������ݣ�ÿ��100���ֽڣ�ÿ�η��ͼ��333MS
        				{
        					QueueFlush((void*)UART4RecQueue); //��ս��ն���
        					QueueNData((void*)UART4RecQueue); //��ս��ն���
        					
        					break;
        				}
				
					case UP_COMMU_DEV_485:
        				{
        					QueueFlush((void*)UART5RecQueue); //��ս��ն���
        					QueueNData((void*)UART5RecQueue); //��ս��ն���
        					
        					break;
        				}
					case UP_COMMU_DEV_232:
        				{
        					QueueFlush((void*)USART1RecQueue); //��ս��ն���
        					QueueNData((void*)USART1RecQueue); //��ս��ն���
        					
        					break;
        				}
					case UP_COMMU_DEV_GPRS:
        				{
        					QueueFlush((void*)USART3RecQueue_At); //��ս��ն���
        					QueueNData((void*)USART3RecQueue_At); //��ս��ն���
        					
        					break;
        				}					
        	}
		}
	else
		{
		
		switch(device)
			{
			case DOWN_COMM_DEV_MBUS:						//�����Zigbee�������ݣ�ÿ��100���ֽڣ�ÿ�η��ͼ��333MS
					{
						QueueFlush((void*)USART2RecQueue); //��ս��ն���
						QueueNData((void*)USART2RecQueue); //��ս��ն���
						
						break;
					}

		    }
		}
#endif	
   // testmode = TRUE;

	err=PorttestGetHead(device,confirm,OS_TICKS_PER_SEC,TRUE,upORdown);
#if 0	
	OSTaskResume(PRIO_TASK_UART_SHELL);	
	OSTaskResume(PRIO_FLASH_MUTEX_PIP);	
	OSTaskResume(PRIO_TASK_MANAGE_DEBUG_INFO);	
	OSTaskResume(PRIO_TASK_GPRS_IPD);	
	OSTaskResume(PRIO_TASK_UP_SEND);	
	OSTaskResume(PRIO_TASK_UP_ANAL_FR);	
	OSTaskResume(PRIO_TASK_UP_REC_GPRS);	
	OSTaskResume(PRIO_TASK_UP_REC_ZIGBEE);	
	OSTaskResume(PRIO_TASK_UP_REC_RS485);	
	OSTaskResume(PRIO_TASK_UP_REC_RS232);	
	OSTaskResume(PRIO_TASK_GPRS_MANA);	
	OSTaskResume(PRIO_TASK_CLOCK);	
	OSTaskResume(PRIO_TASK_READ_ALL_CUR);	
	OSTaskResume(PRIO_TASK_LOG);
#endif	
#if 1
	if((err!=NO_ERR)&&(autotestRetryNum<2)) 
	{
      autotestRetryNum ++;
	  if((upORdown)&&(device==UP_COMMU_DEV_232))
	  	{
	  	 forbidsending1 =0;
	  	}
	  err = CMD_Port_test_Check(device,str,confirm,upORdown);
	}
	#if 0
	if((autotestRetryNum ==2)&& err!=0)
		{
		 if(autotestmode)
		 	{
     		 err= 0;
     		 autotestdisp3 = TRUE;
		 	}
		}
	#endif
	return err;
#endif	

}

/****************************************************************************************************
**	�� ��  �� ��:factory_test_method_Debug_RS232_0
**	�� ��  �� ��:
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/

void factory_test_method_Debug_RS232_0(void)
{

	uint8 status;
	uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
	bool OK_Flag = FALSE;
//	uint8 DeviceType;
	
//	uint8 tmpmid;
	uint8 err;
	
	uint8 len;
    char  Debugtestforport0Command[] = "Debug Test Command";	
    char  DebugtestforportOK[] = "Debug Test OK\n";	
	
    char  DebugtestforportErro[] = "Debug test Erro ";	
	uint8 DebugtestforportBuff[24] = {0};
   	   IRQ1testBuf_Counter = 0x00;
	  //ueueFlush((void*)UART5RecQueue); //��ս��ն���
	   memset(IRQ1testBuf, 0x00, 32);
   display_white();
   ShowMenuTop();
   ShowMenuBottom(25);
   autotestRetryNum = 0;
  // UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
  err = CMD_Port_test_Check(UP_COMMU_DEV_232,Debugtestforport0Command,DebugtestforportOK,1);
   
  // ShowLineChar(3, 7, &GPRStestBuff[0], len);
	while(OK_Flag == FALSE)
	{
		//FreshMethodshow(now_line);
		//FactroytestSDCardData();
		display_white();
		ShowMenuTop();
		ShowMenuBottom(25);
		
//		UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
		 if(err!=NO_ERR) 
		 {		 
	    	 len = sprintf((char*)&DebugtestforportBuff[0], "%s", DebugtestforportErro);
			 ShowLineChar(3, 7, &DebugtestforportBuff[0], len);
		 }		 
		 else
		 {			 
        	 len = sprintf((char*)&DebugtestforportBuff[0], "%s", DebugtestforportOK);
    		 ShowLineChar(3, 7, &DebugtestforportBuff[0], len);
				 
		 }
		

	
		
		status = GetKeyStatuschaobiao();//�õ����̵�״̬
		switch(status)
		{
			case KEY_UP://���ϼ�
			{
				if(now_line>2)
				{
					now_line--;
				}
				else
				{
					now_line = 4;
				}
				break;
			}
			case KEY_DOWN://���¼�
			{
				if(now_line<4)
				{
					now_line++;
				}
				else
				{
					now_line = 2;
				}
				break;
			}			
			case KEY_LEFT://�����	
			case KEY_RIGHT://���Ҽ�	
			{
				break;
			}
			case KEY_ENTER://�س���
			{
				#if 0
				OK_Flag = TRUE;
				if(now_line ==2)
				{
					DeviceType = HEAT_METER_TYPE;
				}
				else if(now_line ==3)/*ʱ��ͨ�������*/
				{
					DeviceType = TIME_ON_OFF_AREA_TYPE;
				}
				else if(now_line ==4)/*����ʽ������*/
				{
					DeviceType = HEATCOST_METER_TYPE;
				}
				else
				{
					DeviceType = HEAT_METER_TYPE;
				}
				/*begin:yangfei added 20140226 for ����sd����������*/
				SaveDeviceType(DeviceType);
				/*end:yangfei added 20140226 */
				now_line= 8;
				FreshMethodshow(now_line);
				#endif
				break;
				
			}
			case KEY_EXIT://���ؼ�
			{
				KeyFuncIndex = 41;
				(*KeyTab[KeyFuncIndex].CurrentOperate)();
				OK_Flag = TRUE;
				break;
			}		
			default:	
				break;
		}
	}
	
}
/****************************************************************************************************
**	�� ��  �� ��:factory_test_method_ChaoBiao_RS232_1
**	�� ��  �� ��:
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/

void factory_test_method_ChaoBiao_RS232_1(void)
	{
	
		uint8 status;
		uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
		bool OK_Flag = FALSE;
	//	uint8 DeviceType;

	//	uint8 tmpmid;
		uint8 err;
		
		uint8 len;
		char  MBUStestforport0Command[] = "MBUS Test Command";	
		char  MBUStestforportOK[] = "MBUS Test OK\n";	
		
		char  MBUStestforportErro[] = "MBUS Test Erro ";	
		uint8 MBUStestforportBuff[24] = {0};
	   	   IRQ2testBuf_Counter = 0x00;
	  //ueueFlush((void*)UART5RecQueue); //��ս��ն���
	   memset(IRQ2testBuf, 0x00, 32);
	   display_white();
	   ShowMenuTop();
	   ShowMenuBottom(25);
	   len = sprintf((char*)&MBUStestforportBuff[0], "%s", MBUStestforport0Command);
	   
	//   DuSend(DOWN_COMM_DEV_MBUS,(uint8 *)MBUStestforport0Command,len);
	
	autotestRetryNum = 0;
	err = CMD_Port_test_Check(DOWN_COMM_DEV_MBUS,MBUStestforport0Command,MBUStestforportOK,0);
	  // UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
//	  err = CMD_Port_test_Check(UP_COMMU_DEV_232,MBUStestforport0Command,MBUStestforportOK);
	   
	  // ShowLineChar(3, 7, &GPRStestBuff[0], len);
		while(OK_Flag == FALSE)
		{
			//FreshMethodshow(now_line);
			//FactroytestSDCardData();
			display_white();
			ShowMenuTop();
			ShowMenuBottom(25);
			
	//		UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
			 if(err!=NO_ERR) 
			 {		 
				 len = sprintf((char*)&MBUStestforportBuff[0], "%s", MBUStestforportErro);
				 ShowLineChar(3, 7, &MBUStestforportBuff[0], len);
			 }		 
			 else
			 {			 
				 len = sprintf((char*)&MBUStestforportBuff[0], "%s", MBUStestforportOK);
				 ShowLineChar(3, 7, &MBUStestforportBuff[0], len);
					 
			 }
			
	
		
			
			status = GetKeyStatuschaobiao();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
				{
					if(now_line>2)
					{
						now_line--;
					}
					else
					{
						now_line = 4;
					}
					break;
				}
				case KEY_DOWN://���¼�
				{
					if(now_line<4)
					{
						now_line++;
					}
					else
					{
						now_line = 2;
					}
					break;
				}			
				case KEY_LEFT://�����	
				case KEY_RIGHT://���Ҽ� 
				{
					break;
				}
				case KEY_ENTER://�س���
				{
				#if 0
					OK_Flag = TRUE;
					if(now_line ==2)
					{
						DeviceType = HEAT_METER_TYPE;
					}
					else if(now_line ==3)/*ʱ��ͨ�������*/
					{
						DeviceType = TIME_ON_OFF_AREA_TYPE;
					}
					else if(now_line ==4)/*����ʽ������*/
					{
						DeviceType = HEATCOST_METER_TYPE;
					}
					else
					{
						DeviceType = HEAT_METER_TYPE;
					}
					/*begin:yangfei added 20140226 for ����sd����������*/
					SaveDeviceType(DeviceType);
					/*end:yangfei added 20140226 */
					now_line= 8;
					FreshMethodshow(now_line);
				#endif
					break;
					
				}
				case KEY_EXIT://���ؼ�
				{
					KeyFuncIndex = 42;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					OK_Flag = TRUE;
					break;
				}		
				default:	
					break;
			}
		}
		
	}

/****************************************************************************************************
**	�� ��  �� ��:factory_test_method_GPRS_TTL_2
**	�� ��  �� ��:
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/

void factory_test_method_GPRS_TTL_2(void)
	{
	
		uint8 status;
		uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
		bool OK_Flag = FALSE;
	//	uint8 DeviceType;
		
	//	uint8 tmpmid;
		uint8 err;
		
		uint8 len;
		char  GPRStestforport0Command[] = "GPRS Test Command";	
		char  GPRStestforportOK[] = "GPRS Test OK\n";	
		
		char  GPRStestforportErro[] = "GPRS Test Erro ";	
		uint8 GPRStestforportBuff[15] = {0};
	   	   IRQ3testBuf_Counter = 0x00;
	  //ueueFlush((void*)UART5RecQueue); //��ս��ն���
	   memset(IRQ3testBuf, 0x00, 32);
	   display_white();
	   ShowMenuTop();
	   ShowMenuBottom(25);
	   
	   autotestRetryNum = 0;
	  // UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
	  err = CMD_Port_test_Check(UP_COMMU_DEV_GPRS,GPRStestforport0Command,GPRStestforportOK,1);
	   
	  // ShowLineChar(3, 7, &GPRStestBuff[0], len);
		while(OK_Flag == FALSE)
		{
			//FreshMethodshow(now_line);
			//FactroytestSDCardData();
			display_white();
			ShowMenuTop();
			ShowMenuBottom(25);
			
	//		UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
			 if(err!=NO_ERR) 
			 {		 
				 len = sprintf((char*)&GPRStestforportBuff[0], "%s", GPRStestforportErro);
				 ShowLineChar(3, 7, &GPRStestforportBuff[0], len);
			 }		 
			 else
			 {			 
				 len = sprintf((char*)&GPRStestforportBuff[0], "%s", GPRStestforportOK);
				 ShowLineChar(3, 7, &GPRStestforportBuff[0], len);
					 
			 }
			
	
		
			
			status = GetKeyStatuschaobiao();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
				{
					if(now_line>2)
					{
						now_line--;
					}
					else
					{
						now_line = 4;
					}
					break;
				}
				case KEY_DOWN://���¼�
				{
					if(now_line<4)
					{
						now_line++;
					}
					else
					{
						now_line = 2;
					}
					break;
				}			
				case KEY_LEFT://�����	
				case KEY_RIGHT://���Ҽ� 
				{
					break;
				}
				case KEY_ENTER://�س���
				{
				#if 0
					OK_Flag = TRUE;
					if(now_line ==2)
					{
						DeviceType = HEAT_METER_TYPE;
					}
					else if(now_line ==3)/*ʱ��ͨ�������*/
					{
						DeviceType = TIME_ON_OFF_AREA_TYPE;
					}
					else if(now_line ==4)/*����ʽ������*/
					{
						DeviceType = HEATCOST_METER_TYPE;
					}
					else
					{
						DeviceType = HEAT_METER_TYPE;
					}
					/*begin:yangfei added 20140226 for ����sd����������*/
					SaveDeviceType(DeviceType);
					/*end:yangfei added 20140226 */
					now_line= 8;
					FreshMethodshow(now_line);
				#endif
					break;
					
				}
				case KEY_EXIT://���ؼ�
				{
					KeyFuncIndex = 43;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					OK_Flag = TRUE;
					break;
				}		
				default:	
					break;
			}
		}
		
	}

/****************************************************************************************************
**	�� ��  �� ��:factory_test_method_Commu_RS485_3
**	�� ��  �� ��:
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/

void factory_test_method_Commu_Zigbee_3(void)
	{
	
		uint8 status;
		uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
		bool OK_Flag = FALSE;
	//	uint8 DeviceType;
		
	//	uint8 tmpmid;
		uint8 err;
		
		uint8 len;
		char  Zigbeetestforport0Command[] = "Zigbee Test Command";	
		char  ZigbeetestforportOK[] = "Zigbee Test OK\n";	
		
		char  ZigbeetestforportErro[] = "Zigbee Test Erro ";	
		uint8 ZigbeetestforportBuff[15] = {0};
	   	   IRQ4testBuf_Counter = 0x00;
	  //ueueFlush((void*)UART5RecQueue); //��ս��ն���
	   memset(IRQ4testBuf, 0x00, 32);
	   display_white();
	   ShowMenuTop();
	   ShowMenuBottom(25);
	   
	   autotestRetryNum = 0;
	  // UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
	  err = CMD_Port_test_Check(UP_COMMU_DEV_ZIGBEE,Zigbeetestforport0Command,ZigbeetestforportOK,1);
	   
	  // ShowLineChar(3, 7, &GPRStestBuff[0], len);
		while(OK_Flag == FALSE)
		{
			//FreshMethodshow(now_line);
			//FactroytestSDCardData();
			display_white();
			ShowMenuTop();
			ShowMenuBottom(25);
			
	//		UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
			 if(err!=NO_ERR) 
			 {		 
				 len = sprintf((char*)&ZigbeetestforportBuff[0], "%s", ZigbeetestforportErro);
				 ShowLineChar(3, 7, &ZigbeetestforportBuff[0], len);
			 }		 
			 else
			 {			 
				 len = sprintf((char*)&ZigbeetestforportBuff[0], "%s", ZigbeetestforportOK);
				 ShowLineChar(3, 7, &ZigbeetestforportBuff[0], len);
					 
			 }
			
	
		
			
			status = GetKeyStatuschaobiao();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
				{
					if(now_line>2)
					{
						now_line--;
					}
					else
					{
						now_line = 4;
					}
					break;
				}
				case KEY_DOWN://���¼�
				{
					if(now_line<4)
					{
						now_line++;
					}
					else
					{
						now_line = 2;
					}
					break;
				}			
				case KEY_LEFT://�����	
				case KEY_RIGHT://���Ҽ� 
				{
					break;
				}
				case KEY_ENTER://�س���
				{
				#if 0
					OK_Flag = TRUE;
					if(now_line ==2)
					{
						DeviceType = HEAT_METER_TYPE;
					}
					else if(now_line ==3)/*ʱ��ͨ�������*/
					{
						DeviceType = TIME_ON_OFF_AREA_TYPE;
					}
					else if(now_line ==4)/*����ʽ������*/
					{
						DeviceType = HEATCOST_METER_TYPE;
					}
					else
					{
						DeviceType = HEAT_METER_TYPE;
					}
					/*begin:yangfei added 20140226 for ����sd����������*/
					SaveDeviceType(DeviceType);
					/*end:yangfei added 20140226 */
					now_line= 8;
					FreshMethodshow(now_line);
				#endif
					break;
					
				}
				case KEY_EXIT://���ؼ�
				{
					KeyFuncIndex = 44;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					OK_Flag = TRUE;
					break;
				}		
				default:	
					break;
			}
		}
		
	}

/****************************************************************************************************
**	�� ��  �� ��:factory_test_method_Commu_RS485_4
**	�� ��  �� ��:
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/

void factory_test_method_Commu_RS485_4(void)
	{
	
		uint8 status;
		uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
		bool OK_Flag = FALSE;
	//	uint8 DeviceType;
		
	//	uint8 tmpmid;
		uint8 err;
		
		uint8 len;
		char  RS485testforport0Command[] = "RS485 Test Command";	
		char  RS485testforportOK[] = "RS485 Test OK\n";	
		
		char  RS485testforportErro[] = "RS485 Test Erro ";	
		uint8 RS485testforportBuff[15] = {0};
	   display_white();
	   ShowMenuTop();
	   ShowMenuBottom(25);
	   IRQ5testBuf_Counter = 0x00;
	  //ueueFlush((void*)UART5RecQueue); //��ս��ն���
	   memset(IRQ5testBuf, 0x00, 32);
	//  QueueFlush((void*)UART5RecQueue);
	   
	autotestRetryNum = 0;
	  // UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
	  err = CMD_Port_test_Check(UP_COMMU_DEV_485,RS485testforport0Command,RS485testforportOK,1);
	 //  testmode= FALSE;
	  // ShowLineChar(3, 7, &GPRStestBuff[0], len);
		while(OK_Flag == FALSE)
		{
			//FreshMethodshow(now_line);
			//FactroytestSDCardData();
			display_white();
			ShowMenuTop();
			ShowMenuBottom(25);
			
	//		UpDevSend(UP_COMMU_DEV_232,DebugtestforportBuff,len);
			 if(err!=NO_ERR) 
			 {		 
				 len = sprintf((char*)&RS485testforportBuff[0], "%s", RS485testforportErro);
				 ShowLineChar(3, 7, &RS485testforportBuff[0], len);
			 }		 
			 else
			 {			 
				 len = sprintf((char*)&RS485testforportBuff[0], "%s", RS485testforportOK);
				 ShowLineChar(3, 7, &RS485testforportBuff[0], len);
					 
			 }
			
	
		
			
			status = GetKeyStatuschaobiao();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
				{
					if(now_line>2)
					{
						now_line--;
					}
					else
					{
						now_line = 4;
					}
					break;
				}
				case KEY_DOWN://���¼�
				{
					if(now_line<4)
					{
						now_line++;
					}
					else
					{
						now_line = 2;
					}
					break;
				}			
				case KEY_LEFT://�����	
				case KEY_RIGHT://���Ҽ� 
				{
					break;
				}
				case KEY_ENTER://�س���
				{
				#if 0
					OK_Flag = TRUE;
					if(now_line ==2)
					{
						DeviceType = HEAT_METER_TYPE;
					}
					else if(now_line ==3)/*ʱ��ͨ�������*/
					{
						DeviceType = TIME_ON_OFF_AREA_TYPE;
					}
					else if(now_line ==4)/*����ʽ������*/
					{
						DeviceType = HEATCOST_METER_TYPE;
					}
					else
					{
						DeviceType = HEAT_METER_TYPE;
					}
					/*begin:yangfei added 20140226 for ����sd����������*/
					SaveDeviceType(DeviceType);
					/*end:yangfei added 20140226 */
					now_line= 8;
					FreshMethodshow(now_line);
				#endif
					break;
					
				}
				case KEY_EXIT://���ؼ�
				{
					KeyFuncIndex = 45;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					OK_Flag = TRUE;
					break;
				}		
				default:	
					break;
			}
		}
		
	}

uint8 factory_test_map(uint8 index)
{
	char Ats_AT[]="AT\r";
	char Ata_OK[]="\r\nOK\r\n";

    uint8 err=0;
	
	 char  sdtestwriterr[] = "SD write erro ";	 
	 char  timetesterr[] = "time test erro ";	 
	 char  timetestOK[] = "time test OK ";	 
	 char  sdtestreaderr[] = "SD read erro ";	 
	 char  sdtestOK[] = "SD test OK ";	 
	 char  GPRStesterr[] = "GPRS test erro ";	 
	 char  GPRStestOK[] = "GPRS test OK ";	 
	 char  Debugtestforport0Command[] = "Debug Test Command";	 
	 char  DebugtestforportOK[] = "Debug Test OK\n"; 
	 
	 char  DebugtestforportErro[] = "Debug test Erro ";  
	 
	 char	 MBUStestforport0Command[] = "MBUS Test Command";  
	 char	 MBUStestforportOK[] = "MBUS Test OK\n"; 
	 
	 char	 MBUStestforportErro[] = "MBUS Test Erro ";    
	 uint8 MBUStestforportBuff[24] = {0};
	 char  GPRStestforport0Command[] = "GPRS Test Command";  
	 char  GPRStestforportOK[] = "GPRS Test OK\n";	 
	 
	 char  GPRStestforportErro[] = "GPRS Test Erro ";	 
	 
	char Zigbeetestforport0Command[] = "Zigbee Test Command";  
	char ZigbeetestforportOK[] = "Zigbee Test OK\n"; 
	
	char ZigbeetestforportErro[] = "Zigbee Test Erro ";    
	
	  char	RS485testforport0Command[] = "RS485 Test Command";	  
	  char	RS485testforportOK[] = "RS485 Test OK\n";	  
	switch(index)
	{
		case 0:
		{
			err=FactroytestSDCardData();  
			break;
		}
		case 1:
		{
			
			err=Factory_test_Set_SystemTime();
			break;
		}			
		case 2:
		{
			err=CMD_AT_RP(Ats_AT,Ata_OK,NULL,OS_TICKS_PER_SEC,1,TRUE);
			break;
		}
		case 3:
		{
			
			err=CMD_Port_test_Check(UP_COMMU_DEV_232,Debugtestforport0Command,DebugtestforportOK,1);
			break;
		}
		case 4:
		{
			
			err =CMD_Port_test_Check(DOWN_COMM_DEV_MBUS,MBUStestforport0Command,MBUStestforportOK,0);
			break;
			
		}
		case 5:
		{
			err =CMD_Port_test_Check(UP_COMMU_DEV_GPRS,GPRStestforport0Command,GPRStestforportOK,1);
			break;
		}
		case 6:
		{
			
			err =CMD_Port_test_Check(UP_COMMU_DEV_ZIGBEE,Zigbeetestforport0Command,ZigbeetestforportOK,1);
		}
		case 7:
		{
			
			err = CMD_Port_test_Check(UP_COMMU_DEV_485,RS485testforport0Command,RS485testforportOK,1);
		}
		default:	
			break;
	}
    return err;
}
void factory_test_method_auto(void)
{ 
//   uint8 err; 
//   char Ats_AT[]="AT\r";
//   char Ata_OK[]="\r\nOK\r\n";
   autotestmode = TRUE;   
   uint8 len;
   uint8 testresult[8] ={0};
   
   uint8 status;   
   uint8 now_line =2;//��ʾ��ǰ��귴�Ե���
   bool OK_Flag = FALSE;
   uint8 testresultBuff[24] = {0};
    char  sdtestwriterr[] = "SD write erro ";	
    char  timetesterr[] = "time test erro ";	
    char  timetestOK[] = "time test OK ";	
    char  sdtestreaderr[] = "SD read erro ";	
    char  sdtestOK[] = "SD test OK ";	
    char  GPRStesterr[] = "GPRS test erro ";	
    char  GPRStestOK[] = "GPRS test OK ";	
    char  Debugtestforport0Command[] = "Debug Test Command";	
    char  DebugtestforportOK[] = "Debug Test OK\n";	
	
    char  DebugtestforportErro[] = "Debug test Erro ";	
	
    char	MBUStestforport0Command[] = "MBUS Test Command";  
    char	MBUStestforportOK[] = "MBUS Test OK\n"; 
    
    char	MBUStestforportErro[] = "MBUS Test Erro ";	  
    uint8 MBUStestforportBuff[24] = {0};
	char  GPRStestforport0Command[] = "GPRS Test Command";	
	char  GPRStestforportOK[] = "GPRS Test OK\n";	
	
	char  GPRStestforportErro[] = "GPRS Test Erro ";	
	
   char	Zigbeetestforport0Command[] = "Zigbee Test Command";  
   char	ZigbeetestforportOK[] = "Zigbee Test OK\n"; 
   
   char	ZigbeetestforportErro[] = "Zigbee Test Erro ";	  
   
	 char  RS485testforport0Command[] = "RS485 Test Command";	 
	 char  RS485testforportOK[] = "RS485 Test OK\n";	 
	 
	 char  RS485testforportErro[] = "RS485 Test Erro ";  
	//const char resultOKArr[8][24] = {sdtestOK[0],timetestOK[0],GPRStestOK[0],DebugtestforportOK[0],MBUStestforportOK[0],GPRStestforportOK[0],ZigbeetestforportOK[0],RS485testforportOK[0]};
	//const char resultERRArr[8][24] = {sdtestreaderr[0],timetesterr[0],GPRStesterr[0],DebugtestforportErro[0],MBUStestforportErro[0],GPRStestforportErro[0],ZigbeetestforportErro[0],RS485testforportErro[0]};
	const char resultOKArr[8][24] = {{"SD test OK "},{"time test OK "},{"GPRS test OK "},{"Debug Test OK"},{"MBUS Test OK"},{"GPRS Test OK"},{"Zigbee Test OK"},{"RS485 Test OK"}};
	const char resultOKArr3[8][24] = {{"SD test OK. "},{"time test OK. "},{"GPRS test OK. "},{"Debug Test OK."},{"MBUS Test OK."},{"GPRS Test OK."},{"Zigbee Test OK."},{"RS485 Test OK."}};
	const char resultERRArr[8][24] = {{"SD read erro "},{"time test erro"},{"GPRS test erro "},{"Debug test Erro "},{"MBUS Test Erro "},{"GPRS Test Erro "},{"Zigbee Test Erro "},{"RS485 Test Erro "}};

//char resultOKArr[][8]={"a","a","a","a","a","a","a","a"};
//	char resultERRArr[][8]={"b","b","b","b","b","b","b","b"};
	 uint8 RS485testforportBuff[15] = {0};
	if(autotestIsfromExitKey== 1)//��ֹ�Զ����Է��ؼ��˳����ٰ��������Ҽ��ظ����к���
	return;
	autotestIsfromExitKey= 0;
    autotestFirstIn ++;
	IRQ5testBuf_Counter = 0x00;
   //ueueFlush((void*)UART5RecQueue); //��ս��ն���
	memset(IRQ5testBuf, 0x00, 32);
 	 IRQ4testBuf_Counter = 0x00;
 //ueueFlush((void*)UART5RecQueue); //��ս��ն���
  memset(IRQ4testBuf, 0x00, 32);
   	   IRQ3testBuf_Counter = 0x00;
  //ueueFlush((void*)UART5RecQueue); //��ս��ն���
   memset(IRQ3testBuf, 0x00, 32);
	  
		 IRQ2testBuf_Counter = 0x00;
	//ueueFlush((void*)UART5RecQueue); //��ս��ն���
	 memset(IRQ2testBuf, 0x00, 32);
   	   IRQ1testBuf_Counter = 0x00;
	  //ueueFlush((void*)UART5RecQueue); //��ս��ն���
	   memset(IRQ1testBuf, 0x00, 32);
	  display_white();
	  ShowMenuTop();
	  ShowMenuBottom(25);
	  
   ShowLineChar_HZ(2,2,&MenuHZTestingPleaseWaite[0],9);//
   
#if 1	
	   OSTaskSuspend(PRIO_TASK_UART_SHELL);    
	   OSTaskSuspend(PRIO_FLASH_MUTEX_PIP);    
	   OSTaskSuspend(PRIO_TASK_MANAGE_DEBUG_INFO); 
	   OSTaskSuspend(PRIO_TASK_GPRS_IPD);  
	   //Test_SuspendGprsRecTask();
	   OSTaskSuspend(PRIO_TASK_UP_SEND);   
	   OSTaskSuspend(PRIO_TASK_UP_ANAL_FR);    
	   OSTaskSuspend(PRIO_TASK_UP_REC_GPRS);   
	   OSTaskSuspend(PRIO_TASK_UP_REC_ZIGBEE); 
	   OSTaskSuspend(PRIO_TASK_UP_REC_RS485);  
	   OSTaskSuspend(PRIO_TASK_UP_REC_RS232);  
	   OSTaskSuspend(PRIO_TASK_GPRS_MANA); 
	   OSTaskSuspend(PRIO_TASK_CLOCK); 
	   OSTaskSuspend(PRIO_TASK_READ_ALL_CUR);  
	   OSTaskSuspend(PRIO_TASK_LOG);   
#endif	
   autotestRetryNum = 0;
      
	  OSTimeDly(OS_TICKS_PER_SEC/2);
	  
	  display_white();
	  ShowMenuTop();
	  ShowMenuBottom(25);
       for (int i =0; i<8 ;i++)
       {
         testresult[i] =factory_test_map(i);
         
		 if(testresult[i])
			   {
			   
			   len = sprintf((char*)&testresultBuff[0], "%s", (char*)&resultERRArr[i][0]);
			   ShowLineChar(i+1, 2, &testresultBuff[0], len);
			   
			   }
			 else
			   {
				if(autotestdisp3)
					{					
					len = sprintf((char*)&testresultBuff[0], "%s", (char*)&resultOKArr3[i]);
					ShowLineChar(i+1, 2, &testresultBuff[0], len);
					autotestdisp3 =FALSE;
					}
				else
					{
        				len = sprintf((char*)&testresultBuff[0], "%s", (char*)&resultOKArr[i]);
        				ShowLineChar(i+1, 2, &testresultBuff[0], len);
					}
			   }
				 status = GetKeyStatuschaobiao();//�õ����̵�״̬
				 switch(status)
				 {
					 case KEY_UP://���ϼ�
					 {
						 break;
					 }
					 case KEY_DOWN://���¼�
					 {
						 break;
					 }			 
					 case KEY_LEFT://�����  
					 case KEY_RIGHT://���Ҽ� 
					 {
						 break;
					 }
					 case KEY_ENTER://�س���
					 {
						 break;
						 
					 }
					 case KEY_EXIT://���ؼ�
					 {
					 	#if 0
						 display_white();
						 ShowMenuTop();
						 ShowMenuBottom(25);
						 KeyFuncIndex = 37;
						 (*KeyTab[KeyFuncIndex].CurrentOperate)();
						 #endif
						// OK_Flag = TRUE;
						autotestIsfromExitKey = 1;
						 autotestmode = FALSE;
						 return;
					 }		 
					 default:	 
						 break;
				 }


	   }
#if 0	   
   testresult[0] = FactroytestSDCardData();  
   testresult[1] =0;//Factory_test_Set_SystemTime();
   testresult[2] =CMD_AT_RP(Ats_AT,Ata_OK,NULL,OS_TICKS_PER_SEC,1,TRUE);;
   testresult[3] =CMD_Port_test_Check(UP_COMMU_DEV_232,Debugtestforport0Command,DebugtestforportOK,1);
   testresult[4] =CMD_Port_test_Check(DOWN_COMM_DEV_MBUS,MBUStestforport0Command,MBUStestforportOK,0);
   testresult[5] =CMD_Port_test_Check(UP_COMMU_DEV_GPRS,GPRStestforport0Command,GPRStestforportOK,1);
   testresult[6] =CMD_Port_test_Check(UP_COMMU_DEV_ZIGBEE,Zigbeetestforport0Command,ZigbeetestforportOK,1);
   testresult[7] = CMD_Port_test_Check(UP_COMMU_DEV_485,RS485testforport0Command,RS485testforportOK,1);
   display_white();
   ShowMenuTop();
   ShowMenuBottom(25);

   for(int i =0; i<8; i++)
   	{
      if(testresult[i])
      	{
      	
		len = sprintf((char*)&testresultBuff[0], "%s", (char*)&resultERRArr[i][0]);
		ShowLineChar(i+1, 2, &testresultBuff[0], len);
		
		OSTimeDly(OS_TICKS_PER_SEC/2);
      	}
	  else
	  	{
         
		 len = sprintf((char*)&testresultBuff[0], "%s", (char*)&resultOKArr[i]);
		 ShowLineChar(i+1, 2, &testresultBuff[0], len);		 
		 OSTimeDly(OS_TICKS_PER_SEC/2);
	    }
	  
    }
  #endif 
   autotestmode = FALSE;   
  
  forbidsending1 = 0;
  while(OK_Flag == FALSE)
   {
			
	
		
			
			status = GetKeyStatuschaobiao();//�õ����̵�״̬
			switch(status)
			{
				case KEY_UP://���ϼ�
				{
					if(now_line>2)
					{
						now_line--;
					}
					else
					{
						now_line = 4;
					}
					break;
				}
				case KEY_DOWN://���¼�
				{
					if(now_line<4)
					{
						now_line++;
					}
					else
					{
						now_line = 2;
					}
					break;
				}			
				case KEY_LEFT://�����	
				case KEY_RIGHT://���Ҽ� 
				{
					break;
				}
				case KEY_ENTER://�س���
				{
				#if 0
					OK_Flag = TRUE;
					if(now_line ==2)
					{
						DeviceType = HEAT_METER_TYPE;
					}
					else if(now_line ==3)/*ʱ��ͨ�������*/
					{
						DeviceType = TIME_ON_OFF_AREA_TYPE;
					}
					else if(now_line ==4)/*����ʽ������*/
					{
						DeviceType = HEATCOST_METER_TYPE;
					}
					else
					{
						DeviceType = HEAT_METER_TYPE;
					}
					/*begin:yangfei added 20140226 for ����sd����������*/
					SaveDeviceType(DeviceType);
					/*end:yangfei added 20140226 */
					now_line= 8;
					FreshMethodshow(now_line);
				#endif
					break;
					
				}
				case KEY_EXIT://���ؼ�
				{
					KeyFuncIndex = 37;
					(*KeyTab[KeyFuncIndex].CurrentOperate)();
					OK_Flag = TRUE;
					autotestmode = FALSE;
					
					forbidsending1 = 0;
					break;
				}		
				default:	
					break;
			}
		}
   	
}

/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/

