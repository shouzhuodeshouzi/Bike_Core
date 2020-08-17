/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-30     Liuzeqi       the first version
 */
#ifndef APPLICATIONS_PT_COMMON_H_
#define APPLICATIONS_PT_COMMON_H_

/*************** package 1 ********************/
#define  PK1_QUERY_DEV_STATE     1
#define  PK1_QUERY_DEV_ID        2
#define  PK1_PC_USER_LOGIN       3
#define  PK1_START_MEASURE       4
#define  PK1_STOP_MEASURE        5

/*************** package 2 ********************/
#define PK2_DEV_STATE           1
#define PK2_DEV_ID              2
#define PK2_USER_ID             3

#define PK2_DEV_TYPE_READY      1
#define PK2_DEV_TYPE_WORKING    2

/*************** package 3 ********************/
#define PK3_SIMUHUMAN_DIE        1
#define PK3_SIMUHUMAN_LIVE       2

#define PK3_HEIGHT_WEIGHT_START_ROTATE  1
#define PK3_HEIGHT_WEIGHT_STOP_ROTATE   2

#define PK3_RESPONSE_TIME_UP_LED      1
#define PK3_RESPONSE_TIME_DOWN_LED    2

#define PK3_SIT_AND_REACH_BAFFLE      1


/*************** package 4 ********************/
//模拟人
#define PK4_SIMUHUMAN_HEIGTH        1
#define PK4_SIMUHUMAN_POS           2
#define PK4_SIMUHUMAN_BREATH        3
//身高体重
#define PK4_HEIGHT_WEIGHT_W         1
#define PK4_HEIGHT_WEIGHT_H         2
#define PK4_HEIGHT_WEIGHT_ROTATED   3
#define PK4_HEIGHT_WEIGHT_EMPTY     4
//反应时间
#define PK4_RESPONSE_TIME_LED       1
//肺活量
#define PK4_VITAL_CAPACITY_DATA     1
//握力计
#define PK4_DYNAMOMETER_DATA        1
//单足站立
#define PK4_SINGLE_FOOT_STATE       1
//俯卧撑
#define PK4_PUSH_UP_HAVE_USER       1
#define PK4_PUSH_UP_ONE_ACTION      2
//仰卧起坐
#define PK4_SIT_UP_HAVE_USER        1
#define PK4_SIT_UP_ONE_ACTION       2
//坐位体前屈
#define PK4_SIT_AND_REATH_BAFFLE    1
#define PK4_SIT_AND_REATH_DISTANCE  2
//动感单车
#define PK4_BIKE_FORWARD            1
#define PK4_BIKE_BACK               2
#define PK4_BIKE_LEFT               3
#define PK4_BIKE_RIGHT              4

/*************** package 5 ********************/
#define PK5_SIMUHUMAN_MODE          1
#define PK5_SIMUHUMAN_TIME          2

#define PK5_HEIGHT_WEIGHT_W         1
#define PK5_HEIGHT_WEIGHT_H         2
#define PK5_HEIGHT_WEIGHT_SPEED     3

#define PK5_VITAL_CAPACITY_FACTOR   1

#define PK5_DYNAMOMETER_FACTOR      1

/*************** package 6 ********************/
#define PK6_SIMUHUMAN_MODE          1
#define PK6_SIMUHUMAN_TIME          2

#define PK6_HEIGHT_WEIGHT_W         1
#define PK6_HEIGHT_WEIGHT_H         2
#define PK6_HEIGHT_WEIGHT_SPEED     3

#define PK6_VITAL_CAPACITY_FACTOR   1

#define PK6_DYNAMOMETER_FACTOR      1


/*************** for all packages ********************/
/***  is for UNPACK_FIELD macro ***/
#define PK_TYPE_FIELD                 0
#define PK_DEV_TYPE_FIELD             1
#define PK_DEV_ID_FIELD               2

#define PK1_COMMAND_FIELD             3

#define PK3_FUNCTION_FIELD            3
#define PK3_DATA_FIELD                4

#define PK5_PARAM_ID_FIELD            3
#define PK5_READ_WRITE_FIELD          4
#define PK5_DATA_FIELD                5


#endif /* APPLICATIONS_PT_COMMON_H_ */
