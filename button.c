/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-31     Liuzeqi       the first version
 */
#include <agile_button.h>
#include <drv_common.h>

#ifdef RT_USING_FINSH
#include <finsh.h>
#endif

#include "dev_common.h"


#include "protocol.h"
#include "uart_rcv.h"

#define DEBUG 1

#define BTN_MAX    2

#define VIP_PIN         GET_PIN(C, 1)

static agile_btn_t *vip = RT_NULL;

static void btn_click_event_cb(agile_btn_t *btn)
{
    char *res_pkg;

    if ( btn->button_id == BTN_MAX )
    {
        res_pkg = pt_pack(PACKAGE_STATE_DATA, 3, "00000000000000000000000000");
        rt_device_write(serial, 0, res_pkg, strlen(res_pkg));
        PT_FREE(res_pkg);
    }

#if DEBUG
    rt_kprintf("[button click event] pin:%d   repeat:%d, hold_time:%d\r\n", btn->pin, btn->repeat_cnt, btn->hold_time);
    return ;
#endif

}


static int key_create(void)
{
    if(vip == RT_NULL)
    {
        vip = agile_btn_create(VIP_PIN, PIN_LOW, PIN_MODE_INPUT_PULLUP,BTN_MAX);
        agile_btn_set_event_cb(vip, BTN_CLICK_EVENT, btn_click_event_cb);
        agile_btn_start(vip);
    }
    return RT_EOK;
}


static int key_delete(void)
{
    if(vip)
    {
        agile_btn_delete(vip);
        vip= RT_NULL;
    }
    return RT_EOK;
}

#ifdef RT_USING_FINSH
MSH_CMD_EXPORT(key_create, create key);
MSH_CMD_EXPORT(key_delete, delete key);
#endif

INIT_APP_EXPORT(key_create);


