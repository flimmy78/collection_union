#ifndef _UL_ARRAY_H
#define  _UL_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_VALVE_NUM    200


typedef struct
{
    TimeAreaArith        *elts;  //数组中具体的数据区域的指针
    int           nelts; /*数组中实际包含的元素数量*/
    size_t      size;  //数组中单个元素的大小（字节）
    int           nalloc; //数组容量，即数组预先分配的内存大小
} ul_array_t;

typedef struct
{
    uint8  user_id;
    uint8  rev[3];
    uint32 oldOpentime;
    uint32 oldValveHot;
} save_valve_info;


typedef struct
{

    uint32 totalHot; //总的热量
    uint32 allocHot; //待分配的热量
    save_valve_info valve_info[MAX_VALVE_NUM];

} save_timearea_info;

ul_array_t *ul_array_create( );
TimeAreaArith *ul_array_push(ul_array_t *a);
void ul_array_destroy(ul_array_t *a);
void ul_array_clean(ul_array_t *a);
uint32 getOpentime(uint8 userId, uint32 opentime);
save_valve_info *getValveHot(uint8 userId);

#ifdef __cplusplus
}
#endif

#endif
