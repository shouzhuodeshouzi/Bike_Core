/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-29     Liuzeqi       the first version
 */
#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>

#include "dev_common.h"

typedef enum DEV_TYPE{
    SIMUHUMAN = 1,          //模拟人
    HEIGHT_WEIGHT = 2,      //身高体重
    RESPONSE_TIME = 3,      //反应时间
    VITAL_CAPACITY = 4,     //肺活量
    DYNAMOMETER = 5,        //握力计

    SINGLE_FOOT = 7,        //单足站立
    PUSH_UP     = 8,        //俯卧撑
    SIT_UP      = 9,        //仰卧起坐
    SIT_AND_REATH = 10,     //坐位体前屈

    BIKE        = 12,       //动感单车
    DEV_TYPE_MAX_NUM,
}DEV_TYPE;

#define PACK_BUFF_SIZE  64

#define ASSERT(p)       (void)(p)
#define UNUSED(n)       (void)(n)

#define CHECK_MALLOC(p)    \
do {                       \
    if ((p) == NULL) {       \
        PRINTF("malloc error %s, %d \n", __FILE__, __LINE__);\
        return NULL;            \
    } \
} while(0)

#if USING_RTT
#include <rtthread.h>
#define MALLOC(p)   char *(p) = rt_malloc(PACK_BUFF_SIZE * sizeof(char));
#define PRINTF      rt_kprintf
#define SNPRINTF    rt_snprintf
#else
static char pt_buff[PACK_BUFF_SIZE];
#define MALLOC(p)    char *(p) = pt_buff;
#define PRINTF      printf
#define SNPRINTF    snprintf
#endif



static char* pt_pack_empty(int id, void* data) { return NULL; }
static char* pt_pack_state_data(int id, void *data);
static char* pt_pack_work_data(int id, void *data);
static char* pt_pack_param_data(int id, void *data);

char* pt_pack(PACKAGE_TYPE type, int id, void* data) {
    switch (type) {
        case PACKAGE_STATE_DATA: return pt_pack_state_data(id, data);   break;
        case PACKAGE_WORK_DATA:  return pt_pack_work_data(id, data);    break;
        case PACKAGE_PARAM_DATA: return pt_pack_param_data(id, data);   break;
        default: break;
    }
    return NULL;
}

/*************************  state data 打包  ***********************************/
static char* pt_pack_state_data(int id, void *data) {
    ASSERT(id >= 1 && id <= 3);
    ASSERT(id != 2 && data);

    MALLOC(pack_buff);
    CHECK_MALLOC(pack_buff);

    if (id == 1) {
        int state = *((int *)data);
        SNPRINTF(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d[%d]}",
                     PACKAGE_STATE_DATA, dev_type, dev_id, id, state);
    } else if (id == 2) {
        SNPRINTF(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d[%.2d%.2d]}",
                    PACKAGE_STATE_DATA, dev_type, dev_id, id, dev_type, dev_id);
    } else {
        // ASSERT(user_id);
        char *user = (char *)data;
        SNPRINTF(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d[%s]}",
                    PACKAGE_STATE_DATA, dev_type, dev_id, id, user);
    }

    return pack_buff;
}

/*************************  work data 打包  ***********************************/
static char* pt_pack_work_data_simuhuman(int id, void *data);
static char* pt_pack_work_data_height_weight(int id, void *data);
static char* pt_pack_work_data_response_time(int id, void *data);
static char* pt_pack_work_data_vital_capacity(int id, void *data);
static char* pt_pack_work_data_dynamometer(int id, void *data);
static char* pt_pack_work_data_single_foot(int id, void *data);
static char* pt_pack_work_data_push_up(int id, void *data);
static char* pt_pack_work_data_sit_up(int id, void *data);
static char* pt_pack_work_data_sit_and_reach(int id, void *data);
static char* pt_pack_work_data_bike(int id, void *data);

static char* (*pt_pack_work_data_table[])(int, void*) = {
    &pt_pack_empty,
    &pt_pack_work_data_simuhuman,
    &pt_pack_work_data_height_weight,
    &pt_pack_work_data_response_time,
    &pt_pack_work_data_vital_capacity,
    &pt_pack_work_data_dynamometer,
    &pt_pack_empty,
    &pt_pack_work_data_single_foot,
    &pt_pack_work_data_push_up,
    &pt_pack_work_data_sit_up,
    &pt_pack_work_data_sit_and_reach,
    &pt_pack_empty,
    &pt_pack_work_data_bike,
};

static char* pt_pack_work_data(int id, void *data) {
    if (dev_type >= DEV_TYPE_MAX_NUM)
        return NULL;
    return (*pt_pack_work_data_table[dev_type])(id, data);
}


static char* pt_pack_work_data_simuhuman(int id, void *data) {
    return NULL;
}
static char* pt_pack_work_data_height_weight(int id, void *data) {
    return NULL;
}
static char* pt_pack_work_data_response_time(int id, void *data) {
    ASSERT(id == 1);
    ASSERT(data);

    MALLOC(pack_buff);
    CHECK_MALLOC(pack_buff);

    int n = *(int *)data;
    SNPRINTF(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d[%d]}",
                PACKAGE_WORK_DATA, dev_type, dev_id, id, n);

    return pack_buff;
}
static char* pt_pack_work_data_vital_capacity(int id, void *data) {
    return NULL;
}
static char* pt_pack_work_data_dynamometer(int id, void *data) {
    return NULL;
}
static char* pt_pack_work_data_single_foot(int id, void *data) {
    return NULL;
}
static char* pt_pack_work_data_push_up(int id, void *data) {
    return NULL;
}
static char* pt_pack_work_data_sit_up(int id, void *data) {
    return NULL;
}
static char* pt_pack_work_data_sit_and_reach(int id, void *data) {
    ASSERT(id >= 1 && id <= 2);
    ASSERT(data);

    MALLOC(pack_buff);
    CHECK_MALLOC(pack_buff);

    switch(id) {
        case 1: {
            int n = *(int *)data;
            SNPRINTF(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d[%d]}",
                        PACKAGE_WORK_DATA, dev_type, dev_id, id, n);
            break;
        }
        case 2: {
            float dis = *(float *)data;
            SNPRINTF(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d[%.2f]}",
                        PACKAGE_WORK_DATA, dev_type, dev_id, id, dis);
            break;
        }
    }
    return pack_buff;
}
static char* pt_pack_work_data_bike(int id, void *data) {
    ASSERT(id >= 1 && id <= 4);
    ASSERT(data);

    MALLOC(pack_buff);
    CHECK_MALLOC(pack_buff);

    //int n = *(int *)data;
    SNPRINTF(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d}",
                PACKAGE_WORK_DATA, dev_type, dev_id, id);

    return pack_buff;
}

/*************************  param data 打包  ***********************************/
static char* pt_pack_param_data_simuhuman(int id, void *data);
static char* pt_pack_param_data_height_weight(int id, void *data);
static char* pt_pack_param_data_response_time(int id, void *data);
static char* pt_pack_param_data_vital_capacity(int id, void *data);
static char* pt_pack_param_data_dynamometer(int id, void *data);
static char* pt_pack_param_data_single_foot(int id, void *data);
static char* pt_pack_param_data_push_up(int id, void *data);
static char* pt_pack_param_data_sit_up(int id, void *data);
static char* pt_pack_param_data_sit_and_reach(int id, void *data);
static char* pt_pack_param_data_bike(int id, void *data);

static char* (*pt_pack_param_data_table[])(int, void*) = {
    &pt_pack_empty,
    &pt_pack_param_data_simuhuman,
    &pt_pack_param_data_height_weight,
    &pt_pack_param_data_response_time,
    &pt_pack_param_data_vital_capacity,
    &pt_pack_param_data_dynamometer,
    &pt_pack_empty,
    &pt_pack_param_data_single_foot,
    &pt_pack_param_data_push_up,
    &pt_pack_param_data_sit_up,
    &pt_pack_param_data_sit_and_reach,
    &pt_pack_empty,
    &pt_pack_param_data_bike,
};

static char* pt_pack_param_data(int id, void *data) {
    if (dev_type >= DEV_TYPE_MAX_NUM)
        return NULL;
    return (*pt_pack_param_data_table[dev_type])(id, data);
}

static char* pt_pack_param_data_simuhuman(int id, void *data){
    return NULL;
}
static char* pt_pack_param_data_height_weight(int id, void *data){
    return NULL;
}
static char* pt_pack_param_data_response_time(int id, void *data){
    ASSERT(data);

    MALLOC(pack_buff);
    CHECK_MALLOC(pack_buff);

    //int n = *(int *)data;
    SNPRINTF(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d[%s]}",
            PACKAGE_PARAM_DATA,dev_type, dev_id, id, data);

    return pack_buff;
}
static char* pt_pack_param_data_vital_capacity(int id, void *data){
    return NULL;
}
static char* pt_pack_param_data_dynamometer(int id, void *data){
    return NULL;
}
static char* pt_pack_param_data_single_foot(int id, void *data){
    return NULL;
}
static char* pt_pack_param_data_push_up(int id, void *data){
    return NULL;
}
static char* pt_pack_param_data_sit_up(int id, void *data){
    return NULL;
}
static char* pt_pack_param_data_sit_and_reach(int id, void *data){
    UNUSED(id);
    UNUSED(data);

    return NULL;
}
static char* pt_pack_param_data_bike(int id, void *data){
    ASSERT(data);

    MALLOC(pack_buff);
    CHECK_MALLOC(pack_buff);

    //int n = *(int *)data;
    SNPRINTF(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d[%s]}",
            PACKAGE_PARAM_DATA,dev_type, dev_id, id, data);

    return pack_buff;
}
