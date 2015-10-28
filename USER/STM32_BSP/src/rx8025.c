

#include <includes.h>

u8 clock_state = TRUE;

#define		RX8025_ADDR 			0x64
#define		TRAN_MODE				0
#define		CVT_8025_RA(reg_a)		((reg_a << 4) | TRAN_MODE)

OS_EVENT *I2cReqSem; //������I2cReqMbox����Ϣǰ�����Ȼ�ô��ź�����ֱ���յ�I2cAnswMbox���ͷŸ��ź���
OS_EVENT *I2cReqMbox; //����ʹ��I2c������
OS_EVENT *I2cAnswMbox; //I2c���ظ���������ʹ�õ�����

extern uint8 I2C_ReadByte(uint8* pBuffer, uint8 length, uint8 ReadAddress, uint8 DeviceAddress);
extern uint8 I2C_Write(uint8 *pSendByte, uint8 SendNums, uint8 DeviceAddress);

//extern uint32 gRx8025tstn[];

//����8025��ַ0x0e��ʼ��2���ֽ�  
void __Rd8025CR(uint8* retbuf)
{
	uint8 tmp,num;
	
	num=2;
	while(1){
	
		retbuf[0]=CVT_8025_RA(0x0e);//�ֵ�ַ
		tmp=I2C_ReadByte(retbuf, num, retbuf[0], RX8025_ADDR);//1:�ֵ�ַ��1���ֽڣ�Ҫ��num���ֽ�����

    	if(tmp==1){
    		break;
    	}
    	else{
    		//gRx8025tstn[0]++;
    		//OSTimeDly(OS_TICKS_PER_SEC/10); 
    	}
    }
	//OSTimeDly(2);
}

//����8025�ӵ�ַ0��15������ַ13��������ֽ�
void Rd8025(uint8* retbuf)
{
	uint8 tmp,num;
	uint8 datbuf[17];  //���һ�������á�
	num = 17;
	//num = 7;			//ֻ��7���ֽ�
	while(1){
	
		datbuf[0]=CVT_8025_RA(0);//�ֵ�ַ
		tmp = I2C_ReadByte(datbuf, num, datbuf[0], RX8025_ADDR);//1:�ֵ�ַ��1���ֽڣ�Ҫ��num���ֽ�����

    	if(tmp==1){
				memcpy(retbuf,datbuf,17);
	
    		break;
    	}
    	else{
				clock_state = FALSE;
				break;
    		//gRx8025tstn[1]++;
    		//OSTimeDly(OS_TICKS_PER_SEC/10); 
    	}
    }
	
	//OSTimeDly(2);
	 		//__Rd8025CR(&datbuf[13]);
			//memcpy(retbuf,datbuf,15);
}

void Wr8025(uint8* buf,uint8 n)
 {
	uint8 tmp;
	
	while(1){
			tmp=I2C_Write(buf, n, RX8025_ADDR);
    	
    	if(tmp==1){
    		
    		break;
    	}
    	else{
				clock_state = FALSE;
				break;
    		//gRx8025tstn[2]++;
    		//OSTimeDly(OS_TICKS_PER_SEC/10); 
    
    	}
    }
	//OSTimeDly(2);
}
void Init8025(void)
{
	uint8 iicbuf[10];
		
	iicbuf[0] = CVT_8025_RA(0x07);//�ֵ�ַ
	iicbuf[1] = 0x00; //Digital Offset bit7= 0("���д0��λ"); F6-F0=0
	iicbuf[2] = 0x00; //Alarm_W:Minute
	iicbuf[3] = 0x00; //Alarm_W:Hour
	iicbuf[4] = 0x00; //Alarm_W:Weekday
	iicbuf[5] = 0x00; //Alarm_D:Minute
	iicbuf[6] = 0x00; //Alarm_D:Hour

	Wr8025(iicbuf,7);//д7���ֽڵ�����(�����ֵ�ַ����)
	
	iicbuf[0] = CVT_8025_RA(0x0e);//�ֵ�ַ
	iicbuf[1] = 0x23; //control1: WALE=0;DALE=0;/12,24 = 1,BIT4=0;TEST=0;CT2CT1CT0=3(/INTA���1HZ���������������)
	iicbuf[2] = 0x20; //control2: VDSL =0 (��Դ���ͼ�⹦�ܵı�׼��ѹֵ�趨Ϊ2.1V); VDET=0;/XST=1;
	                         //PON = 0; BIT3=0;CTFG=0;WAFG=0;DAFG=0;
	Wr8025(iicbuf,3);//д3���ֽڵ�����(�����ֵ�ַ����)
}

//��⵽��RX8025����ֹͣ�������λ��added by zjjin.
void RX8025_ClearXST(void)
{
	uint8 iicbuf[10];

	iicbuf[0] = CVT_8025_RA(0x0e);//�ֵ�ַ
	iicbuf[1] = 0x23; //control1: WALE=0;DALE=0;/12,24 = 1,BIT4=0;TEST=0;CT2CT1CT0=3(/INTA���1HZ���������������)
	iicbuf[2] = 0x20; //control2: VDSL =0 (��Դ���ͼ�⹦�ܵı�׼��ѹֵ�趨Ϊ2.1V); VDET=0;/XST=1;
						 //PON = 0; BIT3=0;CTFG=0;WAFG=0;DAFG=0;
	Wr8025(iicbuf,3);//д3���ֽڵ�����(�����ֵ�ַ����)

}


void WrDt8025(uint8* datebuf)
{
	uint8 iicbuf[10];
	
	iicbuf[0] = CVT_8025_RA(0);//�ֵ�ַ
	iicbuf[1] = datebuf[0]; //��
	iicbuf[2] = datebuf[1]; //��
	iicbuf[3] = datebuf[2]; //ʱ
	iicbuf[4] = datebuf[4]>>5;			//����
	iicbuf[5] = datebuf[3]; //��
	iicbuf[6] = datebuf[4]&0x1f; //��
	iicbuf[7] = datebuf[5]; //��
	
	Wr8025(iicbuf,8); //д8���ֽڵ�����(�����ֵ�ַ����)
}


//�������RX8025�ӵ�ַ0��15������ַ13��������ֽ�
void Rd8025_PostPend(uint8* retbuf) //������
{
	uint8 err;
	I2C_MSG* cp;
	I2C_MSG I2cMsg;
	
	OSSemPend(I2cReqSem, 0, &err);
	
	I2cMsg.cmd = I2C_MSG_CMD_RD;//��ʾ������;

	OSMboxPost(I2cReqMbox,(void *)(&I2cMsg));
	cp = (I2C_MSG *)OSMboxPend(I2cAnswMbox, 0, &err); //����
	if(cp!=&I2cMsg){
		while(1);
	}
	memcpy(retbuf,cp->datbuf,17);
	
	OSSemPost(I2cReqSem);
}

uint8 SetDateTime_PostPend(uint8* datebuf,uint8 init) //������
{
	uint8 err;
	I2C_MSG* cp;
	I2C_MSG I2cMsg;
	
	OSSemPend(I2cReqSem, 0, &err);

	if(init==TRUE){
		I2cMsg.cmd = I2C_MSG_CMD_INIT_SD;
	}
	else{
		I2cMsg.cmd = I2C_MSG_CMD_SD;
	}
	
	memcpy(I2cMsg.datbuf,datebuf,6);
	
	OSMboxPost(I2cReqMbox,(void *)(&I2cMsg));
	cp = (I2C_MSG *)OSMboxPend(I2cAnswMbox, 0, &err); //����
	//˵��������һ�жϵ�Ļ��ᵼ�� һ��IIC���� ��Ϊ���е�����������IIC��д������PENDIIC�жϷ�����򷢵��ź���������IIC��������Ҳ����˵����ϵ�պô����IIC���߽���
	if(cp!=&I2cMsg){
		while(1);
	}
	
	OSSemPost(I2cReqSem);
	return (0);
}

uint8 UserSetDateTime(uint8* datebuf)  //������
{
	//����ʱ��Ϸ����ж�
	if(CheckDT(datebuf)){
		return 1;
	}
	
	SetDateTime_PostPend(datebuf,TRUE);
	
	TIME_UpdateSystemTime();
	
	return (0);
} 
/*
uint8 UserSetDateTime_1(uint8* datebuf)  //������
{
	//����ʱ��Ϸ����ж�
	if(CheckDT(datebuf)){
		return 1;
	}
	
	SetDateTime_PostPend(datebuf,FALSE);
	return (0);
}*/

uint8 UserReadDateTime(uint8* retbuf)  //������
{
//	uint8 err = 0;
	uint8  datebuf[17] = {0};

	Rd8025_PostPend(datebuf);

	memcpy(retbuf,datebuf,3);
	memcpy(&retbuf[3],&datebuf[4],3);
	return 0;
}
/*
//���ڣ��µĸ���λ��Ϊ 1-7 ��7Ϊ�����գ�
uint8 UserReadDateTime1(uint8* retbuf)  //������
{
	uint8  datebuf[16];

	Rd8025_PostPend(datebuf);

	memcpy(retbuf,datebuf,3);
	memcpy(&retbuf[3],&datebuf[4],3);
		
	retbuf[4]&=0x1f;
	if(datebuf[3]==0){ //����
		datebuf[3]=7;
	}
	retbuf[4]|=(datebuf[3]<<5);
	return 0;
}

//=================ncq828
void Recover8025_PostPend(uint8* DateTime_Rec,uint8* DateTime_Bak) 
{
	
	//ncq1006 ����ԭ��if(DateTime_Rec[5]==0x07||DateTime_Rec[5]==0x08){
	if(CheckDateTime(DateTime_Rec)==TRUE){//ncq1006
		UserSetDateTime(DateTime_Rec);
	}
	else{
		UserSetDateTime(DateTime_Bak);
		
	}

	
}

void Recover8025(void) 
{
	uint8 iicbuf[5];
	iicbuf[0] = 0x00;//�ֵ�ַ  
	iicbuf[1] = 0x00;  //TEST1 TESTC  STOP  SET TO 0   
	Wr8025(iicbuf,2);//д2���ֽڵ�����(�����ֵ�ַ����)  
	OSTimeDly(2);
}*/
//=================ncq828


//========ncq1006
/*
uint8 CheckDateTime(uint8* DateTime)
{
	if(DateTime[5]<0x07||DateTime[5]>0x19){
		return FALSE;	
	}
	else{
		return TRUE;
	}
} */
//========ncq1006

