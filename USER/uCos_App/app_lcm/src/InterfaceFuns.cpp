/*******************************************Copyright (c)*******************************************
									ɽ������ռ似����˾(�����ֲ�)                                                          
**  ��   ��   ��: interfacefuns.cpp
**  ��   ��   ��: ������
**	��   ��   ��: 0.1
**  �� ��  �� ��: 2012��9��10�� 
**  ��        ��: ��־��¼ϵͳ������Ϣ
**	�� ��  �� ¼:   	
*****************************************************************************************************/
/********************************************** include *********************************************/
#include <includes.h>

#include "InterfaceFuns.h"
#include "app_flashmem.h" 
/****************************************************************************************************
**	�� ��  �� ��: Reversal
**	�� ��  �� ��: ��������
**	�� ��  �� ��: uint8* pStr
**				  uint16 len
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
void Reversal(uint8* pStr,uint16 len)
{
    uint8* pTemp = new uint8[len];
    uint32 i;
    for(i=0;i<len;i++) pTemp[i] = pStr[len-i-1];
    
    memcpy(pStr,pTemp,len);
    delete(pTemp);
    
 
}
/****************************************************************************************************
**	�� ��  �� ��: ReadTermAddr
**	�� ��  �� ��: ��ȡ��Ŀ��ַ
**	�� ��  �� ��: uint8* pAddr
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 ReadTermAddr(uint8* pAddr)//��ȡ�ն˵�ַ
{
	//ReadF248(pAddr);
	return 0;
}

/****************************************************************************************************
**	�� ��  �� ��: Read_CommPara
**	�� ��  �� ��: ��ͨѶ������ȡ��ȫ�ֱ�����
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Read_CommPara()//��ͨѶ������ȡ��ȫ�ֱ�����
{
    CommPara uPCommPara;
    Read_CommPara((uint8*)&uPCommPara);        
    UpdateUPCommParaInG_V((uint8*)&uPCommPara);
    return 0;  	
}

/****************************************************************************************************
**	�� ��  �� ��: Read_CommPara
**	�� ��  �� ��: ��ͨѶ������ȡ��ȫ�ֱ�����
**	�� ��  �� ��: uint8* pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Read_CommPara(uint8 *pData)
{
    uint8 result;
    //CommPara* pUPCommPara=(CommPara*)pData;
	
 	return result;
}
/****************************************************************************************************
**	�� ��  �� ��: Write_CommPara
**	�� ��  �� ��: ͨѶ������д��
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Write_CommPara()//���غ�����Ϊ�˴洢ȫ�ֱ���
{
/*
	uint8 err;
   CommPara uPCommPara;
   Read_CommPara((uint8*)&uPCommPara); //&uPCommPara.PPIP1[0]
   uint8 Ip[4]; 
   err = ConverASCIPToHex(&gStore_CommPara.PPIP[0],&Ip[0]);//��ASC���IPת��Ϊ4�ֽڵ�16����
   memcpy(uPCommPara.PPIP1,Ip,4);
   uPCommPara.PPPort = AsciiToBcd_16bit(&gStore_CommPara.PPPort[0]);
   err = ConverASCIPToHex(&gStore_CommPara.PPIP[0],&Ip[0]);//��ASC���IPת��Ϊ4�ֽڵ�16����
   memcpy(uPCommPara.PPIP1,Ip,4);
   uPCommPara.PPPort = AsciiToBcd_16bit(&gStore_CommPara.PPPort[0]);
   memcpy(uPCommPara.APNName,gStore_CommPara.APNName,gStore_CommPara.LenAPNName);
   memset(&uPCommPara.APNName[gStore_CommPara.LenAPNName],0x00,16-gStore_CommPara.LenAPNName);
   uPCommPara.SocketType = gStore_CommPara.SocketType;
   memcpy(uPCommPara.Jzqpassword,gStore_CommPara.TermPw,16);
   uPCommPara.HeartCycle =gStore_CommPara.HeartCycle ;//�õ���������  
   
   //��̫������
   //err = ConverASCIPToHex(&gStore_CommPara.TermIP[0],&Ip[0]);//��ASC���IPת��Ϊ4�ֽڵ�16����
   //memcpy(uPCommPara.TermIP,Ip,4);
   memcpy(uPCommPara.TermIP, &gStore_CommPara.TermIP[0], 4);
   
   //err = ConverASCIPToHex(&gStore_CommPara.TermSubMask[0],&Ip[0]);//��ASC���IPת��Ϊ4�ֽڵ�16����
   //memcpy(uPCommPara.TermSubMask,Ip,4);
   memcpy(uPCommPara.TermSubMask, &gStore_CommPara.TermSubMask[0], 4);
   
   //err = ConverASCIPToHex(&gStore_CommPara.TermGateWay[0],&Ip[0]);//��ASC���IPת��Ϊ4�ֽڵ�16����
   //memcpy(uPCommPara.TermGateWay,Ip,4); 
   memcpy(uPCommPara.TermGateWay, &gStore_CommPara.TermGateWay[0], 4);
   
   //uPCommPara.TermPort = AsciiToBcd_16bit(&gStore_CommPara.TermPort[0]);
   uPCommPara.TermPort = gStore_CommPara.TermPort;
     
   Write_CommPara((uint8*)&uPCommPara);
   */
   return 0;
       
}
/****************************************************************************************************
**	�� ��  �� ��: Write_CommPara
**	�� ��  �� ��: ͨѶ������д��
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Write_CommPara(uint8 *pData)
{
    uint8 result;
    CommPara* pUPCommPara=(CommPara*)pData;
    /*
    //��������
    afn04f1 f1;
    OSMutexPend(FlashMutex,0,&err);
	result=FlashReadBytes(AFN04F1,sizeof(afn04f1),(uint8*)&f1,0);
	OSMutexPost(FlashMutex);       
	f1.HeartbeatCyc=pUPCommPara->HeartCycle;
	f1.flagCon=pUPCommPara->flagCon;        
    OSMutexPend(FlashMutex,0,&err);
     result = AppFlashWriteBytes(AFN04F1,sizeof(afn04f1),(uint8*)&f1);
	 OSMutexPost(FlashMutex); 
	 
	 
     afn04f8 f8;//UDP TCP
    OSMutexPend(FlashMutex,0,&err);
	result=FlashReadBytes(AFN04F8,sizeof(afn04f8),(uint8*)&f8,0);
	OSMutexPost(FlashMutex);   
    f8.ConnectMode=pUPCommPara->SocketType;
    OSMutexPend(FlashMutex,0,&err);
     result = AppFlashWriteBytes(AFN04F8,sizeof(afn04f8),(uint8*)&f8);
	 OSMutexPost(FlashMutex); 	 

   afn04f3 f3;//��վ��ǰ�û���
    OSMutexPend(FlashMutex,0,&err);
	result=FlashReadBytes(AFN04F3,sizeof(afn04f3),(uint8*)&f3,0);
	OSMutexPost(FlashMutex);       
    memcpy(f3.Ip1,pUPCommPara->PPIP1,4);
    memcpy(f3.Ip2,pUPCommPara->PPIP2,4); 
    f3.Port1=pUPCommPara->PPPort;
    f3.Port2=pUPCommPara->PPport;
    memcpy(f3.Apn,pUPCommPara->APNName,16); 
    OSMutexPend(FlashMutex,0,&err);
     result = AppFlashWriteBytes(AFN04F3,sizeof(afn04f3),(uint8*)&f3);
	 OSMutexPost(FlashMutex); 
	 
    TermPW pw; 
     OSMutexPend(FlashMutex,0,&err);
	result=FlashReadBytes(TERMPW,sizeof(TermPW),(uint8*)&pw,0);
	OSMutexPost(FlashMutex);          
    memcpy(pw.InterfacePw,pUPCommPara->InterfacePw,6);  
    memcpy(pw.Jzqpassword,pUPCommPara->Jzqpassword,16);  
     
     OSMutexPend(FlashMutex,0,&err);
	result=AppFlashWriteBytes(TERMPW,sizeof(TermPW),(uint8*)&pw);
	OSMutexPost(FlashMutex); 
	
    //��̫������
    afn04f7 f7;
    OSMutexPend(FlashMutex,0,&err);
	result=FlashReadBytes(AFN04F7,sizeof(afn04f7),(uint8*)&f7,0);
	OSMutexPost(FlashMutex); 
    memcpy(f7.Ip,pUPCommPara->TermIP,4);
    memcpy(f7.SubnetMark,pUPCommPara->TermSubMask,4); 
    memcpy(f7.Gateway,pUPCommPara->TermGateWay,4);
    f7.Port=pUPCommPara->TermPort;
    OSMutexPend(FlashMutex,0,&err);
	result=AppFlashWriteBytes(AFN04F7,sizeof(afn04f7),(uint8*)&f7);
	OSMutexPost(FlashMutex);
	*/	
	UpdateUPCommParaInG_V((uint8*)pUPCommPara);
	         
    return result;   
}
/****************************************************************************************************
**	�� ��  �� ��: UpdateUPCommParaInG_V
**	�� ��  �� ��: ����ȫ�ֱ����е�����ͨ�Ų���
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 UpdateUPCommParaInG_V(uint8 *pData)//����ȫ�ֱ����е�����ͨ�Ų���
{
  	uint8  TempBuf[16];
	uint8  len = 0;
    CommPara* pUPCommPara=(CommPara*)pData;
   	if(pUPCommPara->SocketType == 1)
   	
	gStore_CommPara.SocketType = pUPCommPara->SocketType;// �õ�GPRS��ͨѶ��ʽ��TCP/UDP
	else
	gStore_CommPara.SocketType = 0;       //0:TCP    1:UDP


	memcpy(gStore_CommPara.TermPw,pUPCommPara->Jzqpassword,16);
	if(pUPCommPara->InterfacePw[0] == 0xff)
	memset(gStore_CommPara.InterfacePw,'6',6);
	else
	memcpy(gStore_CommPara.InterfacePw,pUPCommPara->InterfacePw,6);//��������
	gStore_CommPara.HeartCycle = pUPCommPara->HeartCycle;//�õ���������
	
	len = ConverHexToASCIP(pUPCommPara->PPIP1,TempBuf);//��IP   ת����Ϊ����
	memcpy(gStore_CommPara.PPIP,TempBuf,len);//
	gStore_CommPara.LenPPIP = len;
	
	len = Hex_BcdToAscii(pUPCommPara->PPPort,gStore_CommPara.PPPort);
	Reversal(gStore_CommPara.PPPort,len);		
	gStore_CommPara.LenPPPort = len;	
	
	for(len = 0;len<16;len++)   //����APN���Ƶĳ���Len
	{
	   if(pUPCommPara->APNName[len] == 0x00 || pUPCommPara->APNName[len] == 0x0D )
	   break;
	}
	memcpy(gStore_CommPara.APNName,pUPCommPara->APNName,len);

	gStore_CommPara.LenAPNName = len;
	
	//����ȫ�ֱ���--��̫��IP

	memcpy(gStore_CommPara.TermIP,&pUPCommPara->TermIP[0],4);

	//����ȫ�ֱ���--��������

	memcpy(gStore_CommPara.TermSubMask,&pUPCommPara->TermSubMask[0],4);
	

	memcpy(gStore_CommPara.TermGateWay,&pUPCommPara->TermGateWay[0],4);
	

	gStore_CommPara.TermPort = pUPCommPara->TermPort;
	return 0;
}

/****************************************************************************************************
**	�� ��  �� ��: Read_SelfDefinePara
**	�� ��  �� ��: �Զ�������Ķ�ȡ������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Read_SelfDefinePara()//��FLASH����ȫ�ֱ���
{
    uint8 result;
  /*  SelfDefinePara para;
    
    OSMutexPend(FlashMutex,0,&err);
	result=FlashReadBytes(SELFDEFINEPARA,sizeof(SelfDefinePara),(uint8*)&para,0);
	OSMutexPost(FlashMutex);
	
	//memcpy(gStore_CommPara.TermMACAddr, para.TermMACAddr, 6);//MAC��ַ
	gStore_CommPara.LCDContrastValue = para.LCDContrastValue;//LCD����S
	//ͨ������ѡ��
	if(para.ChannelType > 4)
	{
		para.ChannelType = 0;
	}
	gStore_CommPara.ChannelType = para.ChannelType;
	*/return result;
}
/****************************************************************************************************
**	�� ��  �� ��: Read_SelfDefinePara
**	�� ��  �� ��: �Զ�������Ķ�ȡ������
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Read_SelfDefinePara(uint8* pData)
{
	
	uint8 result;
   /* SelfDefinePara* pSelfPara=(SelfDefinePara*)pData;
    //SelfDefinePara para;
    OSMutexPend(FlashMutex,0,&err);
	result=FlashReadBytes(SELFDEFINEPARA,sizeof(SelfDefinePara),(uint8*)pSelfPara,0);
	OSMutexPost(FlashMutex);
	
	//memcpy(gStore_CommPara.TermMACAddr, pSelfPara->TermMACAddr, 6);
	//gStore_CommPara.LCDContrastValue = pSelfPara->LCDContrastValue;
	*/
	return result;
}

/****************************************************************************************************
**	�� ��  �� ��: Read_SelfDefinePara
**	�� ��  �� ��: �Զ�������Ķ�ȡ������
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Set_SelfDefinePara(uint8* pData)
{
	
	uint8 result;
  /*  OSMutexPend(FlashMutex,0,&err);
	result=AppFlashWriteBytes(SELFDEFINEPARA,sizeof(SelfDefinePara),pData);
	OSMutexPost(FlashMutex);
	*/
	return result;
}

/****************************************************************************************************
**	�� ��  �� ��: Read_SelfDefinePara
**	�� ��  �� ��: �Զ�������Ķ�ȡ������
**	�� ��  �� ��: none
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Set_SelfDefinePara()//�洢ȫ�ֱ���
{
    uint8 result;
	/*
    SelfDefinePara para;
    result = Read_SelfDefinePara((uint8*)&para);
    if(result == FALSE)
    {
    	return result;
    }
	para.LCDContrastValue = gStore_CommPara.LCDContrastValue;
	para.ChannelType = gStore_CommPara.ChannelType;
	result = Set_SelfDefinePara((uint8*)&para);
	*/
	return result;
}	

/****************************************************************************************************
**	�� ��  �� ��: Set_Mac
**	�� ��  �� ��: ������̫��MAC��ַ
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Set_Mac(uint8* pData)//������̫��MAC��ַ
{
	
	uint8 result;
/*	IOandCommCfg para;
    OSMutexPend(FlashMutex,0,&err);
	result = FlashReadBytes(IOANDCOMMCFG,sizeof(IOandCommCfg),(uint8*)&para,0);
	OSMutexPost(FlashMutex);
	if(result == FALSE)
    {
    	return result;
    }
	memcpy(&para.MAC[0],pData,6);
    OSMutexPend(FlashMutex,0,&err);
	result = AppFlashWriteBytes(IOANDCOMMCFG,sizeof(IOandCommCfg),(uint8*)&para);
	OSMutexPost(FlashMutex);
	*/
	return result;
}
/****************************************************************************************************
**	�� ��  �� ��: Read_Mac
**	�� ��  �� ��: ��ȡ��̫��MAC��ַ
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 Read_Mac(uint8* pData)//��ȡ��̫��MAC��ַ
{
	
    uint8 result;
/*	IOandCommCfg para;
    OSMutexPend(FlashMutex,0,&err);
	result = FlashReadBytes(IOANDCOMMCFG,sizeof(IOandCommCfg),(uint8*)&para,0);
	OSMutexPost(FlashMutex);
	
    if(result == FALSE)
    {
    	return result;
    }
	memcpy(pData, &para.MAC[0], 6);
	*/

	return result;
}

/****************************************************************************************************
**	�� ��  �� ��: setHostAddr
**	�� ��  �� ��: ������վ��ַ
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 SetHostAddr(uint8 *pAddr)
{
	uint8 Err = NO_ERR;
	TermParaSaveType TermPara;
	CPU_SR			cpu_sr;
	
	OS_ENTER_CRITICAL();
	TermPara = gPARA_TermPara;
	OS_EXIT_CRITICAL();
	
	TermPara.WriteFlag = 0xAA55;
	memcpy(TermPara.HostAddr, pAddr, 6);
	
	OSMutexPend (FlashMutex,0,&Err);
	#if 0
    Err =AppSdWrRdBytes(TERMINAL_PARA, sizeof(TermParaSaveType), (uint8 *)&TermPara.WriteFlag, SD_WR);
	#else
	Err = SDSaveData("/TERMINAL_PARA", &TermPara, sizeof(TermParaSaveType), 0);
	#endif
    OSMutexPost (FlashMutex);
    
    OS_ENTER_CRITICAL();
	gPARA_TermPara = TermPara;
	OS_EXIT_CRITICAL();

	return Err;
}

/****************************************************************************************************
**	�� ��  �� ��: SetTermAddr
**	�� ��  �� ��: 
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 SetTermAddr(uint8 *pAddr)
{
	uint8 Err = NO_ERR;
	TermParaSaveType TermPara;
	CPU_SR			cpu_sr;
	
	OS_ENTER_CRITICAL();
	TermPara = gPARA_TermPara;
	OS_EXIT_CRITICAL();
	
	TermPara.WriteFlag = 0xAA55;
	memcpy(TermPara.TermAddr, pAddr, 6);
	
	OSMutexPend (FlashMutex,0,&Err);
	#if 0
    Err =AppSdWrRdBytes(TERMINAL_PARA, sizeof(TermParaSaveType), (uint8 *)&TermPara.WriteFlag, SD_WR);
	#else
	Err = SDSaveData("/TERMINAL_PARA", &TermPara, sizeof(TermParaSaveType), 0);
	#endif
    OSMutexPost (FlashMutex);
    
    OS_ENTER_CRITICAL();
	gPARA_TermPara = TermPara;
	OS_EXIT_CRITICAL();

	return Err;
}
/****************************************************************************************************
**	�� ��  �� ��: SetTimeNode
**	�� ��  �� ��: 
**	�� ��  �� ��: uint8 *pData
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 SetTimeNode(uint8 *pTimeNode, uint8 NodeIndex)
{
	uint8 Err = 0x00;
	//uint8 ReadTime[6] = {0x00};
	uint16 TimeNodeArry[TIME_NODE_NUM_MAX+1] = {0x00};		//��һ������Ԫ�أ������洢��SD����ʾ�ò�����д���
	//uint32 LogicAddr	= 0x00;
	//DataStoreParaType  DataPara;
	/*begin:yangfei modified 2012-12-24 find path*/
	char  NodePath[] = "/2012/12/24/timenode";
	uint8 SystemTime[6] = {0};
	/*end   :yangfei modified 2012-12-24*/
	CPU_SR		cpu_sr;
	
	NodeIndex--;
	memset((uint8 *)TimeNodeArry, 0xFF, sizeof(TimeNodeArry));
	TimeNodeArry[0] = 0xAA55;
	
	OS_ENTER_CRITICAL();
	memcpy(&gPARA_TimeNodes[NodeIndex], pTimeNode, 2);
	#if  0
	memcpy((uint8 *)&TimeNodeArry[1], (uint8 *)gPARA_TimeNodes, sizeof(TimeNodeArry));
	#endif
	memcpy((uint8 *)&TimeNodeArry[1], (uint8 *)gPARA_TimeNodes, sizeof(gPARA_TimeNodes));
	memcpy(SystemTime, gSystemTime, 6);
	OS_EXIT_CRITICAL();

	//�洢��SD����
	OSMutexPend (FlashMutex,0,&Err);
	#if 0
    Err =AppSdWrRdBytes(TIME_NODE_ADDR, (TIME_NODE_NUM_MAX+1)*sizeof(uint16), (uint8 *)TimeNodeArry, SD_WR);
	#else
	Err = SDSaveData("/TIME_NODE_ADDR", TimeNodeArry, sizeof(TimeNodeArry), 0);
	#endif
    OSMutexPost (FlashMutex);
	if(Err!=NO_ERR)
    	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING:Err =%d %s %d SDSaveData Error!",Err,__FUNCTION__,__LINE__ );
    	}
	/*begin:yangfei modified 2012-12-24 find path*/	
	#if 0
    Err =AppSdWrRdBytes(LogicAddr, sizeof(DataStoreParaType), (uint8 *)&DataPara, SD_RD);
    memcpy((uint8 *)DataPara.TimeNode, (uint8 *)&TimeNodeArry[1], TIME_NODE_NUM_MAX*sizeof(uint16));
    Err =AppSdWrRdBytes(LogicAddr, sizeof(DataStoreParaType), (uint8 *)&DataPara, SD_WR);
	#else
	GetFilePath(NodePath, SystemTime, ARRAY_DAY);
	OSMutexPend (FlashMutex, 0, &Err);
	Err = SDSaveData(NodePath, &TimeNodeArry[1], (TIME_NODE_NUM_MAX)*sizeof(uint16),8);
	OSMutexPost (FlashMutex);
	if(Err!=NO_ERR)
    	{
    		LOG_WriteSysLog_Format(LOG_LEVEL_WARN, "WARNING: %s %d SDSaveData Error!",__FUNCTION__,__LINE__ );
    	}
	/*end:yangfei changed 2012-12-28*/
	#endif
 
	if(Err != NO_ERR)		Err = NO_ERR;
	
	return Err;
}
/****************************************************************************************************
**	�� ��  �� ��: SetIP
**	�� ��  �� ��: 
**	�� ��  �� ��: uint8 *pAddr  
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 SetIP(uint8 *pAddr)
{
	uint8 Err = NO_ERR;
	UpCommParaSaveType TermPara;
	CPU_SR			cpu_sr;
	
	OS_ENTER_CRITICAL();
	TermPara = gPARA_UpPara;
	OS_EXIT_CRITICAL();
	
	TermPara.WriteFlag = 0xAA55; 
	memcpy(TermPara.HostIPAddr, pAddr, 4);
	
	OSMutexPend (FlashMutex,0,&Err);	
	Err = SDSaveData("/UP_COMM_PARA", &TermPara, sizeof(UpCommParaSaveType), 0);
    OSMutexPost (FlashMutex);
    
    OS_ENTER_CRITICAL();
	gPARA_UpPara = TermPara;
	OS_EXIT_CRITICAL();

	return Err;
}
/****************************************************************************************************
**	�� ��  �� ��: SetPort
**	�� ��  �� ��: 
**	�� ��  �� ��: uint8 *pAddr  
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 SetHostPort(uint16 Port)
{
	uint8 Err = NO_ERR;
	UpCommParaSaveType TermPara;
	CPU_SR			cpu_sr;
	
	OS_ENTER_CRITICAL();
	TermPara = gPARA_UpPara;
	OS_EXIT_CRITICAL();
	
	TermPara.WriteFlag = 0xAA55; 
//	memcpy(TermPara.HostPort, *Port, 16);
	TermPara.HostPort =  Port;
	OSMutexPend (FlashMutex,0,&Err);
    #if 0
    Err =AppSdWrRdBytes(UP_COMM_PARA, sizeof(UpCommParaSaveType), (uint8 *)&TermPara, SD_WR);
	#else
	Err = SDSaveData("/UP_COMM_PARA", &TermPara, sizeof(UpCommParaSaveType), 0);
	#endif
    OSMutexPost (FlashMutex);
    
    OS_ENTER_CRITICAL();
	gPARA_UpPara = TermPara;
	OS_EXIT_CRITICAL();

	return Err;
}

/****************************************************************************************************
**	�� ��  �� ��: SaveDeviceType
**	�� ��  �� ��: �����豸����
**	�� ��  �� ��: uint8 DeviceType
**	�� ��  �� ��: ��
**  ��   ��   ֵ: ��
**	��		  ע: 
*****************************************************************************************************/
uint8 SaveDeviceType(uint8 DeviceType)
{
	uint8 Err = NO_ERR;
	TermParaSaveType TermPara;
	CPU_SR			cpu_sr;
	
	OS_ENTER_CRITICAL();
	TermPara = gPARA_TermPara;
	OS_EXIT_CRITICAL();
	
	TermPara.WriteFlag = 0xAA55;
	TermPara.DeviceType = DeviceType;
	
	OSMutexPend (FlashMutex,0,&Err);
	Err = SDSaveData("/TERMINAL_PARA", &TermPara, sizeof(TermParaSaveType), 0);
    OSMutexPost (FlashMutex);
    
    OS_ENTER_CRITICAL();
	gPARA_TermPara = TermPara;
	OS_EXIT_CRITICAL();

	return Err;
}

/********************************************************************************************************
**                                               End Of File										   **
********************************************************************************************************/

