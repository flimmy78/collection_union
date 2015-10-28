
#ifndef				_DATETIME_H
#define				_DATETIME_H

#define ARRAY_SECOND	0
#define ARRAY_MINUTE	1
#define ARRAY_HOUR		2
#define ARRAY_DAY		3
#define ARRAY_MONTH		4
#define ARRAY_YEAR		5


extern uint8 TIME_AddSec(uint8* pDate,uint8 sec); //������
extern uint8 TIME_AddMin(uint8* pDate,uint8 min); //��������

extern uint8 TIME_AddHour(uint8* pDate,uint8 hour); //Сʱ����

extern uint8 TIME_AddDate(uint8* pDate,uint8 date); //������

extern uint8 TIME_AddMon(uint8* pDate,uint8 mon); //������
extern uint8 TIME_AddTime(uint8* pDate,uint8 time,uint8 flag);

extern int8 TIME_CmpTime(uint8* pTime2,uint8* pTime1);//��ȷ������ 

extern uint8 TIME_SubMin(uint8* pDate,uint8 min); //���Ӽ���

extern uint8 TIME_SubHour(uint8* pDate,uint8 hour); //Сʱ����

extern void TIME_BspSystemTimeInit(void);		//ϵͳӲ����ʼ���׶ε�ϵͳʱ�����

extern void TIME_UpdateSystemTime(void);		//����ϵͳʱ��

extern uint8 ReadDateTime(uint8* retbuf);		//��ȡϵͳʱ��

#endif
