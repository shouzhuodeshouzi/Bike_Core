/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-04     Jackis       the first version
 */
#ifndef APPLICATIONS_FRAME_H_
#define APPLICATIONS_FRAME_H_

#include <rtthread.h>

#define FRAME_BUFF_LEN 128

struct frame {
    uint8_t  left_brace_flag;  // 当前帧缓存区是否有 '{' 的标志位
    uint16_t cur_length;        // 当前帧缓存的长度
    uint16_t buff_length;        // 缓存区大小
    char *buff;          // 帧缓存区
};
typedef struct frame* frame_t;

#define FRAME_CONFIG_DEFAULT  \
    {   \
        0,  \
        0,  \
        FRAME_BUFF_LEN,     \
        rt_malloc(sizeof(FRAME_BUFF_LEN)) \
    }

/*
 * 判断当前帧缓存里是否有一帧完整数据
 * 若有一帧完整数据，则返回该帧的起始位置，而且 frame_length 返回该帧的长度。
 * 否则返回 RT_NULL，且 frame_length 无用。
 * 注意：调用该函数得到一帧数据并执行完命令后，需调用 frame_reset() 函数来清除一帧数据。
 */
char *frame_has_complete_data(frame_t frame_buff, int *frame_length, char*recv_buff, int recv_length);
/*
 * 清除一帧数据
 */
void frame_reset(frame_t frame_buff);



#endif /* APPLICATIONS_FRAME_H_ */
