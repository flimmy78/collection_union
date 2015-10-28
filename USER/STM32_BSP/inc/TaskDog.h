				
#ifdef   TASKDOG_GLOBALS
#define  TASKDOG_EXT
#else
#define  TASKDOG_EXT  extern
#endif

//�Զ����������==========================================================start
#define  SD_ERR   98
//�Զ����������==========================================================end

//=========================�����Ź� 
typedef struct {
	uint32 CounterPre;
	uint32 CounterCur;
	uint8  Monitor;//TRUE OR FALSE
} WATCH_DOG_TASK;

TASKDOG_EXT WATCH_DOG_TASK gTasksWatchDog[OS_LOWEST_PRIO+1];

TASKDOG_EXT void InitFeedDogFuns(void);
TASKDOG_EXT void SetTaskDogMon(uint8 prio,uint8 Monitor);
TASKDOG_EXT void Dog_OSTaskSuspend(uint8 prio);
TASKDOG_EXT void FeedTaskDog(void);
TASKDOG_EXT void FeedTaskDog_Int(void);
