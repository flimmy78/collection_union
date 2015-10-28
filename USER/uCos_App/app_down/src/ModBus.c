 /*
 �ļ�����ModBus.c
 �����ˣ�������
 �������ڣ�2014��2��27
 �汾�ţ�01
 ��ҪӦ�ã�����485ͬPLC��ͨ��
 �޸ļ�¼��
 */
 //modbus

#include <includes.h>
#include "ModBus.h"


//MODBUS������

#define READ_COIL     01          //���̵���
#define READ_DI       02          //������״̬ READ INPUT STATUS
#define READ_HLD_REG  03          //��������
#define READ_AI       04          //������Ĵ��� READ INPUT REGISTER 
								 
								 //05 WRITE SINGLE COIL
								 //06 WRITE SINGLE REGISTER
								 //15 WRITE MULTIPLE COIL
								 // 16 WRITE MULTIPLE REGISTER


#define PROTOCOL_EXCEPTION 0x81   //Э�鳬��
#define PROTOCOL_ERR  1           //Э�����
#define FRM_ERR       2           //��ʽ����  

//PLCͨ�Ŵ���
//������9600,8λ����У�飬 1��ֹͣλ

//��ʱ��ȫ�ֱ�����485���ڵķ��ͻ��� ���� com_out_aa
//unsigned char 	com_out_aa[8];	// 

		

//CRC�ߵ��ֽ�У�����   
const unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;
const unsigned  char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};


											 
//CRCУ���Ӻ�������ڣ�����ָ��+���ݶγ���
/*
�������
*puchMsg   ��Ҫ�������ݵ�ָ��
usDataLen  �� Ҫ�������ݵ��ֽ���
*/
unsigned short crc(unsigned char *puchMsg , unsigned short usDataLen)
   {
    unsigned char uchCRCHi = 0xFF ; /* ��CRC��ʼ��*/
    unsigned char uchCRCLo = 0xFF ; /* ��CRC��ʼ��*/
    unsigned uIndex ;  /* ������� */
    
    while (usDataLen--) /* �������ݻ��������������ݵ�CRC�����ֽ� */
    {
     uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
     uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
     uchCRCLo = auchCRCLo[uIndex] ;
    }
    return (uchCRCHi << 8 | uchCRCLo) ;
}

/*
//�������ܣ�����Ҫ���͵���Ч���ݵ�CRCֵ������������Э�黺�����Ƶ�485��Ӧ�ķ��Ͷ��л�����
//���������	*src_buf Ҫ�������ݵ�ĿԴ������---��Ҫ���͵�MBUSָ�����ã�CRCֵ�������������ʱ����ģ�
//���������	*dst_buf Ҫ�������ݵķ��ͻ�����----��������ָ���ڵķ��Ͷ���

*/
void construct_rtu_frm ( unsigned char *dst_buf,unsigned char *src_buf,unsigned char lenth)
{
    unsigned short  crc_tmp;
    crc_tmp = crc(src_buf, lenth);   //ԭ��ַ�����ݶ���CRC����  ����Ҫ���͵���Ч���ݵ�CRCֵ��16bit�����ݣ�
     
    *(src_buf+lenth) = crc_tmp >> 8 ;  //��CRC��λ���������ĵ�ַ	�ɴ�Ҫ��src_buf��ΪҪ�������ݵĻ��������䳤��Ҫ����Ҫ�������ݵ���󳤶�
    *(src_buf+lenth+1) = crc_tmp & 0xff;  //��CRC��λ
    lenth++;                             //������ָ��ָ�����
    lenth++;							 //����������CRC�ֽ� ����������������ݳ���
 
    while ( lenth--)                   //�ᶯЭ�黺���������ڷ��ͻ�����  
    {
       *dst_buf = *src_buf;
       dst_buf++;
       src_buf++;	
     }
}
//���̵���״̬��Ӧ   ���� ������PLC��д��һ��16bit�����ݣ�����ֵ�ļ�������λ������һ�ζ�����
//���ĸ�ʽΪ��������ַ    ��������    ���ĸ�λ��ַ   ���ĵ�λ��ַ    �ֽڳ��ȸ�    �ֽڳ��ȵ�   CRCУ�飬��8���ֽ� ����ַ��16λ��
//��Ӧ��ʽΪ���ӻ���ַ      ������      ��Ч�����ֽ���   n����Ч�ֽ���     CRCУ����
/*
//���������	board_adr PLC��ʶ��ַ
				unsigned short start_address  ������ʼ��ַ
				unsigned short lenth:��ȡ���ݵ��ֽ�������ȡ�ķ�������ֻ�����֣�32λ����������16λ�޷�����
				�����Ϊ����ṹ��ָ��		ModBusMRData ,Ϊ����λ�����յ�ָ��

�������������Ҫ�޸�һ�£��޸ĳɣ���ΪPLC ��д����ʹ�ã���������дһ����
*/
void  rtu_read_coil_status ( ModBusMRData *PLCCMD,unsigned char n) 
   {
    unsigned char tmp[16], tmp_lenth;		 //tmp[100] �Ƿ������ݵĻ�����
	unsigned char com_out_aa[16];//��ʱ������
    //unsigned int temp_start;
    //unsigned char temp1,temp2;
    //ModBusMRData *PLCCMD
	//PLCCMD  ��λ���������� 0x01030000100002��pData->MeterAddr[6]={0x02��0x00��0x01, 0x00, 0x03, 0x01, 0x00};	λ����Ҫ����
	/*			//	�������λ����ô����
	tmp[0] = datain[5];	  //ID
	tmp[1] =datain[4];	   //������
	tmp[2] = datain[3];  //��λ��ַ
	tmp[3] = datain[2];//��λ��ַ
    tmp[4] = datain[1]; //��ַ�� ��λ
	tmp[5] = datain[0];	  //��ַ�� ��λ
	*/


	/*			//	�������λ����ô����
	tmp[0] = datain[5];	  //ID
	tmp[1] =datain[4];	   //������
	tmp[2] = datain[3];  //��λ��ַ
	tmp[3] = datain[2];//��λ��ַ
    tmp[4] = datain[1]; //��ַ�� ��λ
	tmp[5] = datain[0];	  //��ַ�� ��λ
	*/

	
	tmp[0] = PLCCMD->SlaveAddr;
	tmp[1] = PLCCMD->FucCode;
	tmp[2] = PLCCMD->Adrr >> 8;  //��λ��ַ
	tmp[3] = PLCCMD->Adrr;//��λ��ַ
    tmp[4] = PLCCMD->DataLength >> 8; //��ַ�� ��λ
	tmp[5] = PLCCMD->DataLength;	  //��ַ�� ��λ
	//PLCCMD->CRCData;  ����Ҫ
	
	tmp_lenth = n;   
    

    
    construct_rtu_frm (com_out_aa,tmp, tmp_lenth);	 //com_out_aa  �Ǵ��ڵķ��ͻ�����  ������Ҫ�滻

	memcpy((unsigned char*)PLCCMD,com_out_aa,8);
    //return (tmp_lenth+2);
	//������
    }


/*
ModBus ����Է��ص����ݽ���CRCУ�飬����������ȡ�����еķ���ֵ������CRCֵ�����뷵�ص�CRCֵ���бȽϣ������ͨ�ųɹ�
//�Է��ص�������RTU���ݷ���
//��ڣ����ջ������Ĵ�С������
//��������� unsigned char *source_p  �������ݻ�����
             rtu_number_long������һ�����ݵ����ݳ��ȣ���������Э�飩
*/			 
int rtu_data_anlys(unsigned char *source_p,unsigned char rtu_number_long)
   {
    unsigned short crc_result; 
	unsigned short crc_tmp; //�������������ݵ�CRCУ������ȡ����
    //unsigned char tmp1;
    //unsigned int tmp2,tmp3,tmp4;
    //unsigned char *source_p;
    //unsigned char temp;
    
    //source_p = uart0_bb;
    //temp = *source_p;
    //if(board_address!=temp) return;
    
    crc_tmp = *(source_p + rtu_number_long-2);                // �ɽ��ջ���������� crc  ��һ�ֽ�  (���CRCֵ���Ǵ�˽ṹ) 
    crc_tmp = crc_tmp * 256 + *( source_p+rtu_number_long-1); // �õ�ʵ��  CRC ֵ	 ��С�˽ṹ��
    crc_result = crc(source_p, rtu_number_long-2);            // ���� CRC ֵ
    
   
    if ( crc_tmp != crc_result ) // �Ա� CRC У����ȷ
      {
       return -1;
       }
	/*�����Ȳ����*/
	/*
    //��modubusЭ�����ݣ���˽ṹ��ת���ɷ�����λ����С�˽ṹ  
    //source_p=uart1_aa;
    tmp1=*source_p;
    tmp3=(*(source_p+2)*256)+*(source_p+3);
    tmp4=(*(source_p+4)*256)+*(source_p+5);
    switch ( *(source_p+1) ) // ������   ���ݹ����룬���ж�д������������λ����ָ�
        {
        case READ_COIL:                   //��ȡ�̵���״̬ 
            tmp2=rtu_read_coil_status (tmp1,tmp3,tmp4);
            rtu_out_active(tmp2);	
            break;

        case READ_DI: //��ȡ����������
            tmp2=rtu_read_input_status(tmp1,tmp3,tmp4);
            rtu_out_active(tmp2);	
            break;

        case READ_HLD_REG:  //��ȡ���ּĴ���
            tmp2=rtu_read_hldreg(tmp1,tmp3,tmp4);
            rtu_out_active(tmp2);	
            break ;

        case READ_AI:      //��ȡģ��������
            tmp2=rtu_read_anloginput (tmp1,tmp3,tmp4);
            rtu_out_active(tmp2);	
            break;

        case PROTOCOL_EXCEPTION:
        return -1*PROTOCOL_ERR;   
       
        default:
        return -1*PROTOCOL_ERR;
       }
	   */
    return 0;
}


/*
//��ȡ��������״̬״̬
int rtu_read_input_status ( unsigned char board_adr,int start_address,int lenth) 
   {
    unsigned char tmp[100], tmp_lenth;
    unsigned int temp_start;
    unsigned char temp1,temp2;
    
    tmp_lenth = lenth;
    temp_start = start_address;
    
    if(tmp_lenth<=8)  
        {
         tmp[2]=1;
         }
       else 
         {
          tmp[2]=2;
          }
          
    alert_value_mid=alert_value_mid>>temp_start; 
         
    tmp[0] = board_adr;
    tmp[1] = READ_COIL;
    temp1= alert_value_mid/256;
    temp2= alert_value_mid;
    
    if(tmp[2]==1)
         {
         tmp[3] = temp2;         //����ʵ�ʵ�����1���ֽڣ��ɴ���8���̵���     
         tmp_lenth = 4;
         } 
     else 
         {
         tmp[3] = temp1;         //����ʵ�ʵ�����1���ֽڣ��ɴ���8���̵���     
         tmp[4] = temp2;
         tmp_lenth = 5;
         }
    
    construct_rtu_frm (com_out_aa, tmp, tmp_lenth);
    return (tmp_lenth+2);
    }
//��ȡ����������
//����ʽ��������ַ+������+����λ��ַ+����λ��ַ+���ݶγ��ȸ�λ+���ݶγ��ȵ�λ+CRCУ����
//���ظ�ʽ��������ַ+������+��Ч���ݳ��� +����1+����2+++++++
int rtu_read_hldreg ( unsigned char board_adr,int start_address,int lenth) 
{
    unsigned char tmp[100], tmp_lenth;
    unsigned char i,j,k;
    unsigned int temp;
    
    tmp_lenth=lenth;
    
    tmp[0] = board_adr;
    tmp[1] = READ_HLD_REG;
    tmp[2] = tmp_lenth;
    
    j=start_address;
    k=2;
    for(i=j;i<tmp_lenth;i++)
      {
       temp=uart1_value[i];
       k++;
       tmp[k] = temp >> 8 ;
       k++;
       tmp[k] = uart1_value[i];
       }
    tmp_lenth = k+1;
    construct_rtu_frm (com_out_aa, tmp, tmp_lenth);
    return (tmp_lenth+2);
}
//��ȡ����������
//����ʽ��������ַ+������+����λ��ַ+����λ��ַ+���ݶγ��ȸ�λ+���ݶγ��ȵ�λ+CRCУ����
//���ظ�ʽ��������ַ+������++��Ч���ݳ���+����1+����2+++++++
int rtu_read_anloginput( unsigned char board_adr,int start_address,int lenth) 
{
    unsigned char tmp[100], tmp_lenth;
    unsigned char i,j,k;
    unsigned int temp;
    
    tmp_lenth=lenth;
    
    tmp[0] = board_adr;
    tmp[1] = READ_HLD_REG;
    tmp[2] = tmp_lenth;
    
    j=start_address;
    k=2;
    for(i=j;i<tmp_lenth;i++)
      {
       temp=uart1_value[i];
       k++;
       tmp[k] = temp >> 8 ;
       k++;
       tmp[k] = uart1_value[i];
       }
    tmp_lenth = k+1;
    construct_rtu_frm (com_out_aa, tmp, tmp_lenth);
    return (tmp_lenth+2);
}
*/

