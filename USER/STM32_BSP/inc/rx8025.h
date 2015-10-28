#ifndef				_RX8025_H
#define				_RX8025_H				
#ifdef   _MBUS_DETACH_
#define		RX8025_ADDR 			0x64
#endif
#define		I2C_MSG_GRP_SIZE		32

#define I2C_MSG_CMD_RD			0x01 //�������RX8025�ӵ�ַ0��15������ַ13��������ֽ�
#define I2C_MSG_CMD_SD			0x02 //������������ʱ��
#define I2C_MSG_CMD_INIT_SD		0x03 //������������ʱ�䲢��ʼ��8025

typedef struct {
	uint8 cmd;
	uint8 datbuf[I2C_MSG_GRP_SIZE];
} I2C_MSG;

extern	OS_EVENT *I2cReqSem; //������I2cReqMbox����Ϣǰ�����Ȼ�ô��ź�����ֱ���յ�I2cAnswMbox���ͷŸ��ź���
extern	OS_EVENT *I2cReqMbox; //����ʹ��I2c������
extern	OS_EVENT *I2cAnswMbox; //I2c���ظ���������ʹ�õ�����

//=============ֻ����TaskI2cManagement���õĺ���
extern void Rd8025(uint8* retbuf);
extern void Init8025(void);
extern void RX8025_ClearXST(void);
extern void WrDt8025(uint8* datebuf);
//=============ֻ����TaskI2cManagement���õĺ���


//����ֵ �� 0��ʾ�ɹ�����0��ʾʧ��
//retbuf�� ���ص�����ʱ�䣬��-��
extern	uint8 UserReadDateTime(uint8* retbuf);
//RX8025_EXT	uint8 UserReadDateTime1(uint8* retbuf);

//datebuf�� ��-��
//����ֵ �� 0��ʾ�ɹ�����0��ʾʧ��
extern	uint8 UserSetDateTime(uint8* datebuf);
extern uint8 UserSetDateTime(uint8* datebuf);


extern u8 clock_state ;
//RX8025_EXT	uint8 UserSetDateTime_1(uint8* datebuf); //for test


//=================ncq828
//RX8025_EXT void Recover8025_PostPend(uint8* DateTime_Rec,uint8* DateTime_Bak); 
//////RX8025_EXT void Recover8025(void);
//=================ncq828


//RX8025_EXT uint8 CheckDateTime(uint8* DateTime);//ncq1006


#endif
