/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-04     Jackis       the first version
 */

#include <rtthread.h>
#include <board.h>

#define DBG_TAG "frame"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include <string.h>
#include "frame.h"
/*
 * 判断 len 长度的 data 数据里是否有 '{'
 */
static int has_left_brace(char *data, int len)
{
    for (int i=0; i<len; i++)
    {
        if (data[i] == '{')
            return 1;
    }
    return 0;
}
/*
 * 判断 len 长度的 data 数据里是否有 '}'
 */
static int has_right_brace(char *data, int len)
{
    for (int i=len-1; i>=0; i--)
    {
        if (data[i] == '}')
            return 1;
    }
    return 0;
}
/*
 * 获得 frame 里 '{...}' 的长度
 */
static int get_frame_length(frame_t frame_buff)
{
    int left = 0;
    int right = 0;
    for (int i=0; i<frame_buff->cur_length; i++)
    {
        if (frame_buff->buff[i] == '{')
            left = i;
        if (frame_buff->buff[i] == '}')
        {
            right = i;
            break;
        }
    }
    if (right > 0)
        return right - left + 1 - 2; //去掉 '{' '}'

    return 0;
}
/*
 * 获得 frame 里 '{' 的起始位置
 */
static char *get_frame_start(frame_t frame_buff)
{
    for (int i=0; i<frame_buff->cur_length; i++)
    {
        if (frame_buff->buff[i] == '{')
            return &(frame_buff->buff[i])+1;
    }

    return RT_NULL;
}
/*
 * 将 recv_length 长度的 recv_buff 里的数据添加到 frame_buff 里已有数据的后面
 */
static void frame_append_data(frame_t frame_buff, char*recv_buff, int recv_length)
{
    if (frame_buff->cur_length+recv_length >= frame_buff->buff_length)
    {
        frame_reset(frame_buff);
        LOG_E("一帧的数据缓存区溢出，仍未收到完整命令，请根据需要调节缓存区大小");
        return ;
    }

    memcpy(frame_buff->buff+frame_buff->cur_length, recv_buff, recv_length);
    frame_buff->cur_length += recv_length;
}


char *frame_has_complete_data(frame_t frame_buff, int *frame_length, char*recv_buff, int recv_length)
{
    // 本次收到的数据里有 '{'
    if (has_left_brace(recv_buff, recv_length))
    {
        frame_reset(frame_buff);
        frame_append_data(frame_buff, recv_buff, recv_length);
        frame_buff->left_brace_flag = 1;

        // 本次收到的数据里同时有 '{' 和 '}'
        if (has_right_brace(recv_buff, recv_length))
        {
            *frame_length = get_frame_length(frame_buff);
            return get_frame_start(frame_buff);
        }
    }
    // 本次收到的数据里没有 '{'
    else
    {
        // 帧缓存里有'{'
        if (frame_buff->left_brace_flag)
        {
            frame_append_data(frame_buff, recv_buff, recv_length);

            // 本次收到的数据里有 '}'
            if (has_right_brace(recv_buff, recv_length))
            {
                *frame_length = get_frame_length(frame_buff);
                return get_frame_start(frame_buff);
            }
        }
        // 帧缓存里没有'{'
        else
        {

        }
    }

    return RT_NULL;
}

void frame_reset(frame_t frame_buff)
{
    frame_buff->left_brace_flag = 0;
    memset(frame_buff->buff, 0, frame_buff->cur_length);
    frame_buff->cur_length = 0;
}

