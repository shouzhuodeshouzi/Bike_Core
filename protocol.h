/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-29     Liuzeqi       the first version
 */
#ifndef APPLICATIONS_PROTOCOL_H_
#define APPLICATIONS_PROTOCOL_H_

#include "pt_common.h"

typedef enum PACKAGE_TYPE{
    PACKAGE_STATE_CMD  = 1,
    PACKAGE_STATE_DATA = 2,
    PACKAGE_WORK_CMD   = 3,
    PACKAGE_WORK_DATA  = 4,
    PACKAGE_PARAM_CMD  = 5,
    PACKAGE_PARAM_DATA = 6
}PACKAGE_TYPE;

#define USING_RTT   1       // 裸机环境为0，RT-Thread 操作系统环境为 1

#define UNPACK_BUFF_SIZE  10        // 目前满足需求，可能需要根据实际情况更改
#define UNPACK_FIELD(p, n)  ((p) + (n)*UNPACK_BUFF_SIZE)

/**
 * 功能：从参数中组成发给PC的数据包，
 * 参数：
 *      type 为包的类型
 *      id 为该包的特征id，如反馈类型、项目id、参数id
 *      data 为该包的数据，data为NULL则代表没有数据。如反馈数据、数据字段、参数值
 * 返回值：
 *      返回数据包的字符串表现形式，注意使用完后释放空间。
 *      返回 NULL 代表当前 PACKAGE 不支持
 *
 * 例子：假如当前设备类型为01，设备id为01，则
 *      pt_pack(PACKAGE_STATE_DATA, 1, 1)    --->      201011[1]
 *      pt_pack(PACKAGE_STATE_DATA, 3, "00000000000000000000000001")
 *                          --->             201013[00000000000000000000000001]
 *      pt_pack(PACKAGE_WORK_DATA, 1, 3.5)    --->      401011[3.5]
 *      pt_pack(PACKAGE_PARAM_DATA, 1, 2000)    --->      601011[2000]
 *
 * 发送 PACKAGE_STATE_DATA '2'数据类型时，data可以为NULL。
 * ********** 注意释放返回的字符串空间 *********************
*/
extern char* pt_pack(PACKAGE_TYPE type, int id, void* data);

/**
 * 功能：从字符串中把字段分割出来
 * 参数：
 *      package 为收到的字符串包
 *      *n 为分割的字段的个数
 * 返回值：
 *      返回参数列表，(*n)代表参数列表的大小
 * 例子：
 *      pt_unpack("1031")         --->        {"1", "03", "1"},  n = 3
 *      pt_unpack("3031[11]")     --->        {"3", "03", "1", "11"},  n = 4
 *      pt_unpack("50222[235]")   --->        {"5", "02", "2", "2", "235"},  n = 5
 *
 * ********** 注意释放返回的字符串空间 *********************
*/
extern char* pt_unpack(char *package, int* n);

/********************* 该宏用于释放空间 **************************/
#if USING_RTT
#define PT_FREE(buf) do {  rt_free((buf)); } while(0)
#else
#define PT_FREE(buf) do { ; }while(0)
#endif  /* USING_RTT */

#endif /* APPLICATIONS_PROTOCOL_H_ */
