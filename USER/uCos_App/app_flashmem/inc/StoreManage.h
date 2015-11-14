/*******************************************Copyright (c)*******************************************

**  文   件   名: StoreManage.h
**  创   建   人: 勾江涛
**  创 建  日 期: 2012.08.09
**  描        述: 存储分配头文件
**	备		  注: 本系统的存储介质为SD卡，容量为2GB以上，保证了绝对够的存储空间。SD卡以512字节为一块，
**				  在驱动程序中做了跨块访问处理，所以分配时不必过多考虑跨块问题。
**				  存储空间较大，每个相关参数和数据均分配 1MByte 空间
*****************************************************************************************************/
#ifndef _STOREMANGE_H
#define _STOREMANGE_H

#define TIME_NODE_NUM_MAX		24							//设置抄表时间节点的最大个数

#define PACK_MAX				900



/********************************集中器参数存储分配 -- End********************************************/

/********************************热计量表数据存储分配 -- Start****************************************/

#define METET_FILE_PER_LEN_STORE	64				//每个表档案实际占有25个字节，存储时占有64字节空间，其余预留
#define METER_NUM_MAX				600								//集中器下可接的热计量表的最大数量
#define METER_PER_CHANNEL_NUM		256								//每个通道的最大表数量
#define METER_FRAME_LEN_MAX			128								//数据帧最大长度, 该数据必须保证为2的N次方
#define STORE_DAYS_PER_MONTH		31								//每月存储31天的数据，该月若没31号等，做冗余处理
#define METERS_DATA_LEN				0x12E00							//每一个时间节点，存储的数据，占151个数据块

#define METER_CHANNEL_NUM  7

#define METER_DATA_TEMP			0xA00000							//从10M开始存储数据
//存储所有热计量表，温控面板，阀门状态等需要暂时存储的数据
//对其存储最好有一定的数据格式
//消息类型(1字节) + 指令类型(1字节) + 总字节数(2字节) + 数据 + 数据 + 数据....



#define	METERS_DATA_STORE_PUBLIC	0x1400000						//用于存储非指定月的数据, 从20M地址开始

//METER_DATA_STORE_ADDR,数据存储的基地址，以2012年1月为基准
#define METERS_DATA_STORE_ADDR		0x1E00000						//用于供暖季存储, 从30M地址开始

#pragma	pack(1)
//数据存储分配：每天最大存TIME_NODE_NUM_MAX个点的数据，该日的抄表时间点存储于抄表时间点的第一个时间节点的参数区，只有每日第一个点
//的时间节点数据 是有效的，
//本结构体为每一个抄表时间节点的数据存储的参数区，在每个抄表时间点，开始抄表前需要对其进行初始化
typedef struct
{
    uint8 	DataRecordTime[6];								//数据存储记录的时间节点 年月日时分秒，分秒一般用不上
    //uint8   ReadCmplFlag[75];								//读表数据完成标志 75*8=600
    uint16	MeterNums;										//本时间节点共存储了多少个表
    uint16  TimeNode[TIME_NODE_NUM_MAX];					//在每天起始时间点，该参数有效，存储该日的时间节点索引
    uint16 	TimeNodeNum;//共设置了几个定时点。	//若在该日内，重新设置了时间节点，需更新该参数，
    //该日以前的数据无效,以保证每日只存储TIME_NODE_NUM_MAX个点的数据
    //uint8	DataIdle[128-6-2-2*TIME_NODE_NUM_MAX - 2];
    uint8	DataIdle[118 - 2 * TIME_NODE_NUM_MAX];		//DataIdle[128-6-2-2*TIME_NODE_NUM_MAX - 2];为了数据帧以块对齐，添加此空数据
} DataStoreParaType;											//以上为一个时间节点的参数。共128字节
typedef struct
{
    uint16 WriteFlag;					//该区域是否被写过数据
    uint16 LCDContrast;				//LCD对比度
} LCDContrastPara;	//muxiaoqing add
typedef struct
{
    uint16 WriteFlag;					//该区域是否被写过数据
    uint16 MeterStoreNums;				//共存储多少块表档案信息
#ifdef DEMO_APP

    uint16 DEMOWriteFlag;					//该区域是否被写过演示版本数据
#endif
} MeterFilePara;

typedef struct
{
    uint16 MeterID;						//热表编号，唯一，但可能不连续。
    uint8 MeterAddr[7];					//热计量表地址
    uint8 Manufacturer;					//厂商代码
    uint8 ProtocolVer;					//协议版本
    uint8 EquipmentType;				//设备类型
    uint8 ChannelIndex;					//通道号
    uint8 ValveProtocal;				//阀控器协议号，用于兼容多厂家不同阀控器。
    uint8 ValveAddr[7];					//阀门地址
    uint8 ControlPanelAddr[7];			//控制面板地址
    uint8 BulidID;						//楼号
    uint8 UnitID;						//单元号
    uint16 RoomID;						//房间号
    uint16 Reserved1;					//一下预留8字节。
    uint16 Reserved2;
    uint16 Reserved3;
    uint16 Reserved4;
} MeterFileType;							//共25个字节

typedef struct
{
    uint8  user_id;/*用户编号*/
    uint16	address;  /*设备地址*/
    uint16	area;/*用户面积*/
    uint8  data_valid;/*数据是否有效  有效为0xA5，无效为0x00*/
    uint32 total_energy;/*(BCD码)用户的能量累计值,后2位为小数位。单位:kwh*/
    uint32 open_time;/*(BCD码)开阀时间，后2位为小数位。单位:h*/
    uint32 proportion_energy;/*(BCD码)当前时间段内的能量分摊值*/
    uint8  open_percent;/*当前时间段内的阀门开启比例(HEX码 0-100) */
    uint16 forward_temperature;/*(BCD码)进水温度，后2位为小数位。单位:摄氏度*/
    uint16 return_temperature;/*(BCD码)回水温度，后2位为小数位。单位:摄氏度*/
    uint16 room_temperature;/*(BCD码)房间温度，后2位为小数位。单位:摄氏度*/
    uint8  state;/*bit7 代表无线故障，1故障 0正常
								  bit6 代表欠费状态 1欠费 0正常
								 bit5 代表充值状态 1充值 0正常
								 bit4 代表开关机状态，1开机，0关机;
								 bit3 代表锁定 0不锁，1锁定;
								 bit2 代表阀门堵转故障，1故障，0正常;
								 bit1 代表NTC故障		 1故障，0正常;
								 bit0 代表阀门状态，1阀开，0阀关*/
    uint8 channel;/*通道号*/

} TimeAreaArith;
typedef struct
{
    uint16 FrameLen_Max;				//集中器每次上发数据帧的最大长度，数据包由多个数据帧组成，默认数据包最大500Byte
    uint8  FrameToFrameTime;			//帧间 时间 间隔 单位为 秒
} DebugSetPara;

typedef struct
{
    uint16 WriteFlag;						//该区域是否被写过数据
    uint8 TermAddr[6];
    uint8 HostAddr[6];
    DebugSetPara DebugPara;
    /*begin:yangfei added for add support time_on_off*/
    uint8 DeviceType;
    /*end:yangfei added for add support time_on_off*/
} TermParaSaveType;

typedef struct
{
    uint16 WriteFlag;						//该区域是否被写过数据
    uint8 HostIPAddr[4];					//主站IP地址
    uint16 HostPort;						//主站端口
    uint8 HeartCycle;						//心跳周期，单位为 Min
    uint8 RelandDelay;						//掉线重新登录延时时间，单位为 Min
    uint8 RelandTimes;						//掉线重新登录次数
} UpCommParaSaveType;

typedef struct
{
    uint16 WriteFlag;						//该区域是否被写过数据
    uint8 MeterReplenishTimes;					//补抄表次数，范围0-9.默认补抄1次。
    uint16 MeterInterval;						//补抄表时间间隔，单位ms.范围200-9999ms.
    uint8 ValveReplenishTimes;					//补抄阀次数，范围0-9.默认补抄1次。
    uint16 ValveInterval;						//补抄阀时间间隔，单位ms.范围200-9999ms.
} ReplenishReadMeterSaveType;



typedef struct
{
    uint16 MeterSn;
    uint8 HeatMeterAddr[7];				//热计量表地址
} MeterChannelType;

typedef struct
{
    uint8 HeatMeterAddr[7];				//热计量表地址
    uint8 ValveAddr[7];					//阀门地址
} ValveControlType;

typedef struct
{
    uint8 HeatMeterAddr[7];				//热计量表地址
    uint8 ControlPanelAddr[7];			//温控面板地址
} ControlPanelType;

typedef struct 								//抄表状态信息
{
    uint8 TimingStartTime[6];
    uint8 TimingEndTime[6];
    uint16 TimingMeterNums;
    uint16 ReadCmplNums;
} TimingState;

typedef struct 					//GPRS APN接入点选择设置。
{
    uint16 WriteFlag;
    uint8 GprsAPNSet;
} GprsAPNSelect;



#pragma pack()


/********************************热计量表数据存储分配 -- End****************************************/

extern uint16 gPARA_MeterNum;										//当前表档案总数量
extern MeterFileType gPARA_MeterFiles[METER_NUM_MAX];				//内存中存储当前表档案信息
extern uint8 gPARA_SendIndex_Prevous;								//备份上一包数据包的序列号
extern uint16 gPARA_TimeNodes[TIME_NODE_NUM_MAX];
extern uint16 gPARA_TimeNodesNum;


extern uint16 gPARA_MeterChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];			//6个通道热计量表信息, MeterSn
extern uint8   gPARA_Meter_Failed[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];	/*7个通道失败记录*/
extern uint16 gPARA_MeterChannelNum[METER_CHANNEL_NUM];								//每个通道表数量

extern uint16 gPARA_ConPanelChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];		//6个通道每个通道的温控面板信息, MeterSn
extern uint16 gPARA_ConPanelChannelNum[METER_CHANNEL_NUM];							//每个通道加载的温控面板数量

extern uint16 gPARA_ValveConChannel[METER_CHANNEL_NUM][METER_PER_CHANNEL_NUM];		//6个通道每个通道的阀门信息, MeterSn
extern uint16 gPARA_ValveConChannelNum[METER_CHANNEL_NUM];							//每个通道加载的阀门数量

extern TermParaSaveType gPARA_TermPara;								//集中器基本参数
extern UpCommParaSaveType	gPARA_UpPara;							//GPRS上行通讯参数
extern CommPara uPCommPara;//muxiaoqing
extern ReplenishReadMeterSaveType gPARA_ReplenishPara;

extern GlobalPara gStore_CommPara;
extern uint8 gLOG_SystemLog[1024];									//存储系统日志
extern OS_EVENT *LogQueSem; 										//产生新日志的信号量
extern OS_EVENT *LogWriteSem;										//请求写入日志的信号量


/********************抄表信息查看与轮显用到的变量****************************/
extern uint8 gu8ReadAllMeterFlag;
extern uint8 gu8ReadDataStore[METER_FRAME_LEN_MAX];

/********************MBUS短路保护用到的全局变量*********************************/
extern uint8 gu8MBusShortRising; //MBUS短路指示引脚是否出现上升沿，0-没有，1-出现上升沿。
extern uint8 gu8MBusShortFlag[METER_CHANNEL_NUM];  //标记对应MBUS通道通道短路,1-短路，0-不短路。
extern uint8 gu8NowMBUSChannel;    //记录当前MBUS通道?

extern uint8 gu8MBusShortTime;

extern uint8 gu8ReadValveFail;


extern uint8 gu8ErrFlag;
extern uint16 gu16ErrOverTime;

extern char  gcYesterdayPath[];




extern uint8 STORE_GetAddrOfMeterData(uint8 *pTime, uint16 MeterSn, uint32 *AddrOut, uint8 Flag);
extern uint8 PARA_ReadMeterInfo(uint16 MeterSn, MeterFileType *pMeterFile);
extern uint8 STORE_CalcTime(uint8 *pReadTime, uint32 *pTimeOffSet, uint8 Flag);
extern uint8 StoreYesterday(void);
extern uint8 STORE_GetTimeNodeInfo(uint16 InTime, uint8 *pNodeIndex, int8 *pNodeOffset);
extern uint8 STORE_FindTimeNodeIndex(uint16 InTime, uint16 *pTimeNode, uint8 *pNodeIndexOut);
extern 	uint8 FindTimeNodeIndex_Forward(uint16 InTime, DataStoreParaType *datastorePara, uint8 *pNodeIndexOut, uint8 *pYesterday);
extern uint8 STORE_InitTimeNodePara(uint8 *pDateTime, uint8 NodeIndex);
extern uint8 STORE_FindDayStartAddr(uint8 *pReadTime, uint32 *pAddrOut);

extern void PARA_InitGlobalVar(void);
extern void GetVipDataFormSD(void);


#endif
