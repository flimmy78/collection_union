#ifndef __COMMAND_H
#define __COMMAND_H

typedef enum{UartStateInput,UartStateExe}UartCommandState; //����״̬�����Ͷ���
typedef void ( *UartCmdFunction) (u8 argc,void **argv); //����ָ�붨��
typedef struct{
u8 Used;   //���������Ƿ�ռ�ã��������Ժ����չ���Ѿ�ʹ��Ϊ1��δ��Ϊ0.
u8* UartCmdStr;	   //�����ַ���
UartCmdFunction UartCmdFunc;  //��Ӧ�ĺ���ָ�룬���Կ���һ���ص�����
}UartCmdDef;			  //һ�������

#define MaxArgv 5
#define MaxChar 20
typedef struct{
u8 Argc;                                         /*ָ����������*/
u8 Argv[MaxArgv][MaxChar+1];  /*�����ַ����Ͳ����ַ�������*/
}WordTable;	
/*begin:yangfei added 2013-01-23*/
extern void Task_UartCmd(void *pdata);
/*end:yangfei added 2013-01-23*/
#endif /* __COMMAND_H */
