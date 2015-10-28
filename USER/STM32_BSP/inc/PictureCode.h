#ifndef        _PICTURE_CODE_H
#define		   _PICTURE_CODE_H

#ifndef IN_PIC_CODE

//���������У���̰�ť���º��ͼ��
extern uint8 ProgramePic[] ;
//���������У������������־
extern uint8 ACPic[];

//GPRS�ź�ǿ��ͼ�꣬����Ϊǿ�ȴ�С
extern uint8 GPRSSignalPic[][32];//�Լ�������ź�ǿ����ʾ
extern uint8 StateSignalPic[][32];//����Ҫ����ź�ǿ����ʾ

//GPRS����CDMA�ź�ͼ�꣬0��G��ΪGPRS��1��C��ΪCDMA
extern uint8 GCPic[][32];
//�쳣���ͼ�꣬0Ϊ������1Ϊ�쳣���
extern uint8 AbnormalPic[][32];

//�˵��ײ���ʾ����ʾͼƬ��Ϊ��ͳһ���飬����Ϊ16*160��С
extern uint8 MenuBottomPic[][320];

//��������ͼƬ��RISESUN��־,������������104*104
extern uint8 StartPic[];
//��������ͼƬ2��ֻ��RISESUN��־,û���κ����� 112*85
extern uint8 RisesunLogo_Only[];
//�������� ϵͳ������...  16*96
extern uint8 SystemStart[];

//ע�����ͼƬ 16*160
extern uint8 RegisterPic[];

//ICO��־ͼƬ���������Ҽ�ͷͼ�꣩ 16*16
extern uint8  IcoPic[][32];

//���ݳ�̩����
extern uint8 Risesun[];

//����վ�����ϵ���ʾ��connect
extern uint8 CommuniReg[][70];

//�ն˵�ַ��ʾ��
extern uint8 AddrIdex[];

//�˵�ͼƬ
extern uint8 MenuPic_OK[];
extern uint8 MenuPic1[];
extern uint8 MenuPic1_1[];
extern uint8 MenuPic1_2[];
extern uint8 MenuPic1_3[];
extern uint8 MenuPic1_2_11[];
extern uint8 MenuPic1_2_12[];
extern uint8 MenuPic1_2_2[];
extern uint8 MenuPic1_2_3[];
extern uint8 MenuPic1_2_4[];
extern uint8 MenuPic1_2_5[];
extern uint8 MenuPic1_2_6[];
extern uint8 MenuPic_TestPointSet[];//MenuPic1_2_7[];

extern uint8 MenuPic1_3_1[];
extern uint8 MenuPic1_3_2[];
extern uint8 MenuPic1_3_3[];

//����ͼƬ
extern uint8 RotatePic1[];
extern uint8 RotatePic2[];
#endif

#endif
