#ifndef _UL_ARRAY_H
#define  _UL_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_VALVE_NUM    200


typedef struct
{
    TimeAreaArith        *elts;  //�����о�������������ָ��
    int           nelts; /*������ʵ�ʰ�����Ԫ������*/
    size_t      size;  //�����е���Ԫ�صĴ�С���ֽڣ�
    int           nalloc; //����������������Ԥ�ȷ�����ڴ��С
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

    uint32 totalHot; //�ܵ�����
    uint32 allocHot; //�����������
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
