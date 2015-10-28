//�ļ�����ModBus.h

#ifndef        _ModBus_H
#define		   _ModBus_H


//��������ʵ������ȡ���������ݣ�ʵ����PLCֻ����������32λ�������Լ�16λ�޷����ͣ������������λ���޶�����ֻ��00 02���Լ�00 01��	 
// �豸ID	������	 ��ʼ��ַ413   ��ȡ������ַ�����ݣ�4���ֽڣ�										  CRCУ�� ��crc 16-2��
// 01       03       01 9B          00 02 ������16���ƣ���ȡ��  ��Ӧ 40413��ʮ���ƣ���ַ��413����Ч��ַ�� B4 18 
//��������
// 	�豸ID	 ������	 ��ȡ�ֽ���������˵���ص��ֽ�����  �����ֽ��� ��ǰ�����2����ַ�򷵻�4���ֽڣ�	  CRCУ��
//  01        03     04                                3F 19 99 9A                                     CD DB 
//�������ݸ�ʽ	   ModBus�Ǵ�˽ṹ����λ�ڵ͵�ַλ���ȷ��͸ߵ�ַλ
//Modbus�����ݽṹ
#pragma pack(1) 
typedef struct{
        //3���ֽڵ�ʱ�仺��	���봮�ڲ��������
		uint8 SlaveAddr;					//������PLC��ַ����ַ�򣩣�0�ǹ㲥-----�����ڴ���͵�ַ  1
		uint8 FucCode;					//���ܴ��루������	2
		unsigned short 	Adrr;			 //��ַ
		//uint8 HAdrr;					//���ĸߵ�ַλ	 ����	 3
		//uint8 LAdrr;				//���ĵ͵�ַλ
		unsigned short 	DataLength;
		//uint8 HDataLength;					//��ַ������λ	00	(0x0022)
		//uint8 LDataLength;					//��ַ������λ	02
		//uint8 HData;							//�������λ
		//uint8 LData;							//�������λ
		unsigned short CRCData;
		//uint8 HCRCData;									//crcУ���λ
		//uint8 LCRCData;									//crcУ���λ	----- �����ڴ���ߵ�ַ
	     //3���ֽڵ�ʱ�仺�� ���봮�ڲ��������
	}ModBusMRData;	//д����

//Modbus�����豸��������
typedef struct{
        //3���ֽڵ�ʱ�仺��	���봮�ڲ��������
		uint8 SlaveAddr;					//������PLC��ַ����ַ�򣩣�0�ǹ㲥-----�����ڴ���͵�ַ  1
		uint8 FucCode;					//���ܴ��루������	2
		uint8 ByteNUM;					//������Ч���ݵ��ֽ���	 ����	 3
		uint8 Data[8];					//��ַ������λ	00	(0x0022),���4���Լ�������Ļ�������һЩ

		unsigned short CRCData;
		//uint8 HCRCata;									//crcУ���λ
		//uint8 LCRCata;									//crcУ���λ	----- �����ڴ���ߵ�ַ
	     //3���ֽڵ�ʱ�仺�� ���봮�ڲ��������
	}ModBusSRData;  //������

//OLC����λ���·���������ṹ��
typedef struct
{
	//uint8 ForAllOrOne;
	//uint8 Device_type;
	uint8 Channel;  //ͨ����   ��λ��
	uint8 ProtocolVer;		//Э��汾��   ��λ��
	uint8 ControlCode[6];	  //��λ���·���PLC������Ϣ
	uint8 Lenth;		  //��λ���·��������򳤶�
}FrameMsg_31;

#pragma pack()

extern unsigned short crc(unsigned char *puchMsg , unsigned short usDataLen);
extern void construct_rtu_frm ( unsigned char *dst_buf,unsigned char *src_buf,unsigned char lenth);
//void construct_rtu_frm ( unsigned char *dst_buf,unsigned char *src_buf,unsigned char lenth);
//extern int rtu_data_anlys(unsigned char *source_p,unsigned char rtu_number_long);
extern  void  rtu_read_coil_status ( ModBusMRData *PLCCMD,unsigned char n); 
#endif
