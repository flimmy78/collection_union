/*
�ü���ɨ���������ã�ʵ�������֡��ַ����ĵ�������������ϵͳ��PB2��PB3��ֻ�ڰ���
ʱ���Ͱ�����Ϣ��
*/
#include <includes.h>

#include "task_uart_shell.h"


/*�û��ĳ��������������*/
#define UartCharMaxCount 30		//�����ַ����ַ�������ֵ
#define UartCmdMaxCount 17		//����֧��10���ڲ�����
#define UartMaxPara 10
													
/* Ϊ�˷��㶨�壬�������typedef����һЩ����ָ�롢�ṹ�塢�������ö�ٱ���*/

/* �û��������ⲿ�����������������������*/

/* �û��Լ���ȫ�ֱ��������ﶨ�壬������������*/
UartCommandState UartCmdState=UartStateInput; /*״̬����������*/
u8 UartCmdBuf[UartCharMaxCount+1]; /*��������飬�ַ������޶�*/

UartCmdDef UartCmdTable[UartCmdMaxCount];  //��������ṹ����
u8* UartCmdStrTable[UartCmdMaxCount]={	   //���������ַ���
"help","cls","ls","rm","mv","pwd","cd","cat","mkdir","sdwr","new","time","set","reset","cp","test","channel"};

WordTable CmdArg;
void *UartMsg_Tbl[30];          /*������Ϣָ������*/
OS_EVENT *UartMsgOSQ;	/*������Ϣ����*/

/* �û��������ⲿ������������*/

/* �û�ʵ�ֵĺ�������������*/

void UartCmdTable_Init(void)  /*�����ĳ�ʼ����*/
{
  u8 i;
  for (i=0; i<UartCmdMaxCount; i++ )
  { 
     UartCmdTable[i].Used=1; 
     UartCmdTable[i].UartCmdStr=UartCmdStrTable[i];
  }
  UartCmdTable[0].UartCmdFunc=UartCmdHelp;
  UartCmdTable[1].UartCmdFunc=UartCmdCls;
  UartCmdTable[2].UartCmdFunc=UartCmdls;
  UartCmdTable[3].UartCmdFunc=UartCmdrm;
  UartCmdTable[4].UartCmdFunc=UartCmdmv;
  UartCmdTable[5].UartCmdFunc=UartCmdpwd;
  UartCmdTable[6].UartCmdFunc=UartCmdcd;
  UartCmdTable[7].UartCmdFunc=UartCmdcat;
  UartCmdTable[8].UartCmdFunc=UartCmdmkdir;
  UartCmdTable[9].UartCmdFunc=UartCmdSDWrite;
  UartCmdTable[10].UartCmdFunc=UartCmdnew;
  UartCmdTable[11].UartCmdFunc=UartCmdtime;
  UartCmdTable[12].UartCmdFunc=UartCmdSetDebugLevel;
  UartCmdTable[13].UartCmdFunc=UartCmdReset;
  UartCmdTable[14].UartCmdFunc=UartCmdCp;
  UartCmdTable[15].UartCmdFunc=UartCmdTest;
  UartCmdTable[16].UartCmdFunc=UartSetChannel;
}

u8 UartParseCmdBuf(u8* UartCmdBuf,WordTable* CmdArg)
{ 
  u16 CmdBufIndex=0;
  u8 CharIndex=0;
  u8 WordIndex=0;
  u8 CmdEndFlag=0;
  u8 ch;

  ch=UartCmdBuf[0];
  for ( ; !CmdEndFlag && CmdBufIndex<UartCharMaxCount; CmdBufIndex++,ch=UartCmdBuf[CmdBufIndex] )
      {
	        if ( ch==0 ) 
		     { CmdEndFlag=1;
			   CmdArg->Argv[WordIndex][CharIndex]=0;
		     }
		else if ( ch==' ' ) 
		     { 
		                   CmdArg->Argv[WordIndex][CharIndex]=0;
				   WordIndex++; 
				   if ( WordIndex>=MaxArgv )return 1;
				   CharIndex=0;
		     }
		else
		     { 
			   CmdArg->Argv[WordIndex][CharIndex]=ch;
			   CharIndex++; 
			   if ( CharIndex>MaxChar )return 1;
		     }
      }
  if ( 	CmdBufIndex>=UartCharMaxCount )return 1;
  CmdArg->Argc=WordIndex+1;
  return 0;
}

void Task_UartCmd(void *pdata)
{
  u8 UartCharIn;	/*�Ӵ��ڻ�õ��ַ�*/
  u8 UartCharCount; /*�����ĵ�ǰ����*/
  u8* UartCmdStr;    /*�����ַ���ָ��*/
  char UartCompResult=1;/*�����ַ����ȽϵĽ����������ַ��������ҵ�������0.*/
  u8 UartCmdIndex;          /*�����ַ����ڱ����������*/
  void* argv[UartMaxPara];
  u8 j,argc;

  pdata=pdata;

  UartCharCount=0;
  UartCmdState=UartStateInput;
  //������Ϣ���У����ڴ������ 
   UartMsgOSQ=OSQCreate(&UartMsg_Tbl[0],30);
  UartCmdTable_Init();   /*�����ĳ�ʼ����*/
  //mountSd();                    /* ����SD  ��*/
  Uart_Printf("\r\n********** ������ӿ�!*********\r\n");
  Uart_Printf("sh>");      /*���������ʾ��*/
  for(;;)
  {
  
  switch(UartCmdState)			   /*����״̬������ɢת*/
  {
    
    case UartStateInput:			//�����������״̬
	  { 
	  	//UartCharIn=Uart_GetChar();		//��ȡ���밴��
	  	USART1Getch(&UartCharIn, 0);
		UartCharIn &= 0x7F;

	    if(UartCharIn=='\r')	    // ���������Enter��,���Ǵ������״̬��������̬
	      { 					// �����������ִ��̬�Ĺؼ���
		    UartCmdBuf[UartCharCount]='\0'; /*CharCount=�����ַ������֣����������0����*/
			debug( "\r\n" );      //��ʾ���ϻ��Ի���
			if ( UartCharCount==0 )	 	  /*֮ǰû�������ַ�������ƽ�װ���һ��Enter*/
			   { Uart_Printf("sh>");   /*������ʾ������ʾ������������̬��ѭ��*/
			   }
			else
			   { UartCmdState=UartStateExe;	  //���������ַ��������������̬
			     UartCharCount=0;
			   }
	      }
		else if (UartCharIn=='\n')	   //���»س�ʱ�����ն��͹���Ϊ\r\n.
		  {
		  	//debug("'\n'");
		  }
		else if ( UartCharIn=='\b') 		  //��������˸��
		  {
			if ( UartCharCount==0 )		/*֮ǰû�������ַ������˸�������*/
			   { 
			   	//Speaker_Beep();  //���棬�Ѿ��˻���ʾ���ˣ����˾�:>�ˡ�
			   }
			else
			   {
				 
				 UartCharCount -=1;		      /*��Ч�ַ�����ȥ1.*/
				 debug( "%c",UartCharIn ); /*���ַ������Լ������˸��*/
				 debug( "%c",' '); 
				 debug( "%c",UartCharIn );
			   }
		  }
		else  		  //�����������
		  {
		    UartCmdBuf[UartCharCount]=UartCharIn;  //���ַ����뻺������ǰλ��
			UartCharCount +=1;			   //������λ����ǰ�ƶ�
			if (UartCharCount>=UartCharMaxCount+1)		//��������ˣ�����ַ�����ռ�����һ��λ��
			   { 
			   	UartCharCount=UartCharMaxCount;  //ʹ��ǰλ�ñ���Ϊ���һ�����ȴ�Enter��
			       //Speaker_Beep();			  //����ַ�Ҳ�����ַ��������,���ǲ�������
			   }						  
			else
			   {
				 debug( "%c",UartCharIn );//����Ч�ַ������
			   }
		  }

	   }
	   break;

    case UartStateExe:			//�����������ִ��̬
	  { 
	  	UartParseCmdBuf(UartCmdBuf,&CmdArg);
	        UartCmdStr=CmdArg.Argv[0];  
		//��������������õ�����ṹ��Ϣ�������ﻹֻ֧�ּ�����
	  	//���Դ���ܼ򵥣�ֱ�ӻ�������ַ������Ժ������չ��					 		 
	    for(UartCmdIndex=0; UartCmdIndex<UartCmdMaxCount; UartCmdIndex++ )
	      {  // ���������������Ӧ�����ַ���
		    UartCompResult=strcmp( (char*)UartCmdStr, (char*)(UartCmdStrTable[UartCmdIndex]) );
			//�����ַ����ȽϵĽ��
			if (UartCompResult==0) //������ַ��������ҵ�������0.
			    break; //��ʱCmdIndex��Ӧ��ֵΪ��������������������
	      }
		if ( UartCmdIndex<UartCmdMaxCount)
		   {  /*�ҵ���Ӧ���������Ӧ��������*/
		      argc=CmdArg.Argc;
			  for ( j=0; j<argc; j++ )
			  {
			  	argv[j]=(void*)CmdArg.Argv[j];
			  }
			  UartCmdTable[UartCmdIndex].UartCmdFunc(argc,argv);
		   } 
		else   /*û���ҵ���Ӧ��������Ϣ��*/
		   {  
		   	debug("Bad Command!\r\n"); 
		   }  
		UartCmdState=UartStateInput;
		Uart_Printf("Sh>");
		UartCharCount=0;
	  }
      break;

	default:
	  break;
  }
 }    
}

