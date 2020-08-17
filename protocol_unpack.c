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
#include <string.h>

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
#define MALLOC(p)   char *(p) = rt_malloc(sizeof(char)*UNPACK_BUFF_SIZE*UNPACK_BUFF_SIZE);
#define PRINTF      rt_kprintf
#define SNPRINTF    rt_snprintf
#else
static char pt_buff[UNPACK_BUFF_SIZE][UNPACK_BUFF_SIZE];
#define MALLOC(p)   char *(p) = (char *)pt_buff;
#define PRINTF      printf
#define SNPRINTF    snprintf
#endif


#define UNPACK_PACK_DEVTYPE_DEVID(unpack_buff, package) do {\
    SNPRINTF(UNPACK_FIELD((unpack_buff), 0), UNPACK_BUFF_SIZE, "%c", (package)[0]);                 \
    SNPRINTF(UNPACK_FIELD((unpack_buff), 1), UNPACK_BUFF_SIZE, "%c%c", (package)[1], (package)[2]);   \
    SNPRINTF(UNPACK_FIELD((unpack_buff), 2), UNPACK_BUFF_SIZE, "%c%c", (package)[3], (package)[4]);   \
    SNPRINTF(UNPACK_FIELD((unpack_buff), 3), UNPACK_BUFF_SIZE, "%c", (package)[5]); \
} while (0)


static void pt_unpack_empty(char *unpack_buff, char *package, int *n) { ; }
static void pt_unpack_state_cmd(char *unpack_buff, char *package, int *n);
static void pt_unpack_work_cmd(char *unpack_buff, char *package, int *n);
static void pt_unpack_param_cmd(char *unpack_buff, char *package, int *n);


char* pt_unpack(char *package, int* n) {
    ASSERT(package);
    ASSERT(n);

    int type = package[0] - '0';
    ASSERT(type == 1 || type == 3 || type == 5);

    MALLOC(unpack_buff);
    CHECK_MALLOC(unpack_buff);
    memset(unpack_buff,'\0',UNPACK_BUFF_SIZE*UNPACK_BUFF_SIZE);
    switch (type) {
        case PACKAGE_STATE_CMD: pt_unpack_state_cmd(unpack_buff, package, n);   break;
        case PACKAGE_WORK_CMD:  pt_unpack_work_cmd(unpack_buff, package,  n);    break;
        case PACKAGE_PARAM_CMD: pt_unpack_param_cmd(unpack_buff, package, n);   break;
        default:    break;
    }
    return unpack_buff;
}

/*************************  state cmd 解包  ***********************************/
static void pt_unpack_state_cmd(char *unpack_buff, char *package, int *n) {
    int len = strlen(package);
    ASSERT(len == 6);

    UNPACK_PACK_DEVTYPE_DEVID(unpack_buff, package);
    *n = 4;
}

/*************************  work cmd 解包  ***********************************/
static void pt_unpack_work_cmd(char *unpack_buff, char *package, int *n) {
    int len = strlen(package);
    ASSERT(len >= 6);

    UNPACK_PACK_DEVTYPE_DEVID(unpack_buff, package);
    *n = 4;

    if (len > 6) {
        char *i = strrchr(package, '[');
        char *j = strrchr(package, ']');
        ASSERT(i && j && i < j);

        int k = 0;
        while (++i < j) {
            *(UNPACK_FIELD(unpack_buff, 4) + k++) = *i;
        }

        *n = *n + 1;
    }
}

/*************************  param cmd 解包  ***********************************/
static void pt_unpack_param_cmd(char *unpack_buff, char *package, int *n) {
    int len = strlen(package);
    ASSERT(len >= 7);

    UNPACK_PACK_DEVTYPE_DEVID(unpack_buff, package);
    SNPRINTF(UNPACK_FIELD(unpack_buff, 4), UNPACK_BUFF_SIZE, "%c", package[6]);
    *n = 5;

    if (len > 7) {
        char *i = strrchr(package, '[');
        char *j = strrchr(package, ']');
        ASSERT(i && j && i < j);

        int k = 0;
        while (++i < j) {
            *(UNPACK_FIELD(unpack_buff, 5) + k++) = *i;
        }

        *n = *n + 1;
    }
}
