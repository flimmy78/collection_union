#ifndef        _METER_TABLE_H
#define		   _METER_TABLE_H

#define     COMSET_1		0
#define		COMSET_2		1
#define		COMSET_3		2    /*yangfei added for support leye 485 meter*/
#define     COMSET_4        3    /*yangfei added for support �򻪴��*/
#define     COMSET_5        4
#define     COMSET_6        5			//���������PLC
#define     COMSET_7        6


#define DE_LU			COMSET_1
#define TIAN_GANG		COMSET_1
#define	LI_CHUANG		COMSET_1
#define DAN_FUSEN		COMSET_1	
#define WAN_HUA			COMSET_2

#define WANHUA_VER			 1		//����򻪡�
#define HYDROMETER775_VER    6
#define ZENNER_VER           7    /*�����ȱ�*/
#define LANDISGYR_VER        8   /*������*/
#define ENGELMANN_VER        9   /*������*/


//���������
#define NUANLIU_VER		    17/*ů��*/
#define LIANQIANG_VER485              19   /*��ǿ485 �ν���485*/

#define HAIWEICI_VER              20   /*������*/
#define JINGWEI_VER              22   /*��γ*/
#define LIANQIANG_VERMBUS   23	   /*��ǿmbus*/
#define	LI_CHUANG_2		24 //�����ڶ��ֱ�
#define	JICHENG_VER		25 //����������
#define	ZHENYU_VER_2		26 //ZHENYU2������
#define JINGWEIFE_VER  27 //��γ����ʶFE�汾Э��

//begin: added by zjjin.
#define YITONGDA_VER			28				//�����ͨ��
#define WANHUA_TO_DELU_VER      30		//�����ת�ɵ�³Э���ϴ�
#define	ZHENYU_S_VER			31      //����������S�Ͷ�һ���ۼ�����5�ֽڡ�
#define MITE_VER				32		//��������������

//end: added by zjjin.


//#define ENGELMANN_VER              9   /*������*/



/*begin:201303141153 yangfei modified 2013-03-14 for added lichuang PROTOCOL*/
 /*yangfei added 2013-03-25 for support leye 485 meter*/
#define PROTOCOL_MAX	80  //ԭΪ8 �������Ϊ80
/*end:yangfei modified 2013-03-14 for added lichuang PROTOCOL*/
extern void (*METER_ComParaSetArray[])(void);
extern uint16 gMETER_Table[][4];

#endif

