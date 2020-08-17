/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-16     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "easyflash.h"
#include "fal.h"
#include "dev_common.h"
#include "stdlib.h"

int main(void)
{
    fal_init(); //抽象层初始化
    easyflash_init();
    dev_id = atoi(ef_get_env("dev_id"));
/*    int count = 1;

    while (count++)
    {
        LOG_D("Hello RT-Thread!");
        rt_thread_mdelay(1000);
    }*/

    return RT_EOK;
}
