//ΪҺ���ṩ�ӿ�

#ifndef _INTERFACEFUNS_H
#define _INTERFACEFUNS_H

extern uint8 ReadTermAddr(uint8* pAddr);
extern uint8 Read_CommPara();//��ͨѶ������ȡ��ȫ�ֱ���
extern uint8 Read_CommPara(uint8 *pData);
extern uint8 Write_CommPara();//���غ�����Ϊ�˴洢ȫ�ֱ���
extern uint8 Write_CommPara(uint8 *pData);
extern uint8 UpdateUPCommParaInG_V(uint8 *pData);//����ȫ�ֱ����е�����ͨ�Ų���

//�Զ�������ṹ�Ķ�ȡ������
extern uint8 Read_SelfDefinePara();
extern uint8 Read_SelfDefinePara(uint8* pData);
extern uint8 Set_SelfDefinePara(uint8* pData);
extern uint8 Set_SelfDefinePara();//���غ�����Ϊ�˴洢ȫ�ֱ���

extern uint8 SelfMeterIni(void);    
//��ȡ������MAC��ַ
extern uint8 Set_Mac(uint8* pData);//������̫��MAC��ַ
extern uint8 Read_Mac(uint8* pData);//��ȡ��̫��MAC��ַ
extern uint8 SetHostAddr(uint8 *pAddr);
extern uint8 SetTermAddr(uint8 *pAddr);
extern uint8 SetTimeNode(uint8 *pTimeNode, uint8 NodeIndex);
extern uint8 SetIP(uint8 *pAddr);  
extern uint8 SetHostPort(uint16 Port); 
extern uint8 SaveDeviceType(uint8 DeviceType);

 #endif
