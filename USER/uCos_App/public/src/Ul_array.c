#include "Includes.h"
#include "Ul_array.h"
#include "Publicfunc.h"

static ul_array_t arr_head;
TimeAreaArith arr_data[MAX_VALVE_NUM] = {0};
save_timearea_info savetimeinfo = {0};       //由于阀门记录打开时间为累计值，这个值保存上一次阀门上报的开启时间

/*****************************************************
根据userid 获取上次保存的阀门打开时间
 若果找到，并更新阀门打开时间
参数                 - userId  用户id
				-opentime 阀门打开时间
返回     阀门打开时间，单位为分钟

********************************************************/

uint32 getOpentime(uint8 userId, uint32 opentime)
{
    uint32 i = 0;
    uint32 oldopentime = 0;
    uint32 oldValveHot = 0;
    uint32 time;
    uint8 *dataBuf;
    save_valve_info *opentimeinfo = savetimeinfo.valve_info;
    for(i = 0; i < MAX_VALVE_NUM; i++)
    {
        if(opentimeinfo[i].user_id == userId)
        {
            oldopentime = opentimeinfo[i].oldOpentime;
            opentimeinfo[i].oldOpentime = opentime;
            debug_info(gDebugModule[TIME_AREA], "userid=%d,opentime=%x,oldopentime=%x\r\n", userId, opentime, oldopentime);
            time = opentime - oldopentime;
            dataBuf = (uint8 *)&time;
            return ((dataBuf[3] * 10000 + dataBuf[2] * 100 + dataBuf[1]) * 60 + HexToBcd(dataBuf[0]) * 100 / 2 * 60);
        }
        if(opentimeinfo[i].user_id == 0xff)
            break;
    }
    if(i == MAX_VALVE_NUM)
        return 0;
    opentimeinfo[i].user_id = userId;
    opentimeinfo[i].oldOpentime = opentime;
    opentimeinfo[i].oldValveHot = 0;
    return 0;
}


/*****************************************************
根据userid 获取上次保存的阀门热量值

参数                 - userId  用户id

返回

********************************************************/

save_valve_info *getValveHot(uint8 userId)
{
    uint32 i = 0;
    //uint32 oldopentime = 0;
    //uint32 oldValveHot = 0;
    //uint32 time;
    //uint8 *dataBuf;
    save_valve_info *opentimeinfo = savetimeinfo.valve_info;
    for(i = 0; i < MAX_VALVE_NUM; i++)
    {
        if(opentimeinfo[i].user_id == userId)
        {
            return &opentimeinfo[i];
        }

        if(opentimeinfo[i].user_id == 0xff)
        {
            break;
        }
    }
    if(i == MAX_VALVE_NUM)
        return 0;
    opentimeinfo[i].user_id = userId;
    opentimeinfo[i].oldOpentime = 0;
    opentimeinfo[i].oldValveHot = 0;
    return &opentimeinfo[i];
}


/*****************************************************
创建数组，多线程不安全
参数                 - n   数组个数
                            - size 数组元素大小
********************************************************/
ul_array_t *
ul_array_create()
{
    memset(arr_data, 0, sizeof(TimeAreaArith) * MAX_VALVE_NUM);
    memset(&savetimeinfo, 0xff, sizeof(save_timearea_info));
    savetimeinfo.totalHot = 0;
    savetimeinfo.allocHot = 0;
    arr_head.elts = arr_data;
    arr_head.nelts = 0;
    arr_head.size = sizeof(TimeAreaArith);
    arr_head.nalloc = MAX_VALVE_NUM;
    return &arr_head;
}

/*****************************************************
获取数组第一个未用的位置，多线程不安全
********************************************************/
TimeAreaArith *
ul_array_push(ul_array_t *a)
{
    TimeAreaArith        *elt = NULL;

    if (a->nelts == a->nalloc)
    {
        return NULL;
    }

    elt =  a->elts + a->nelts;
    a->nelts++;
    return elt;
}

/*****************************************************
销毁数组，多线程不安全
********************************************************/
void
ul_array_destroy(ul_array_t *a)
{

}
/*****************************************************
清空数组，多线程不安全
********************************************************/
void ul_array_clean(ul_array_t *a)
{
    a->nelts = 0;
}
