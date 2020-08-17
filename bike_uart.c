/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-16     Liuzeqi       the first version
 */
#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#define DBG_TAG "bike_uart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "uart_rcv.h"
#include "protocol.h"
#include <stdlib.h>
#include "string.h"
#include "dev_common.h"

/*
 * PC10 -- TX,   PC11 -- RX
 */
#define BIKE_USART_NAME      "uart4"
static rt_device_t bike_uart = RT_NULL;

// bike_uart_thread 阻塞在该信号量。
static rt_sem_t bike_uart_sem = RT_NULL;

static rt_thread_t bike_uart_tid = RT_NULL;       // uart thread

// 本次测量 HALL 触发的次数，即自行车转动的圈数。
//static rt_uint32_t hall_count = 0;
// HALL 触发一次串口发过来的字节。
const rt_uint8_t HALL_FLAG = 0x55;

#define THREAD_PRIORITY     15
#define THREAD_STACK_SIZE   1024
#define THREAD_TIMESLICE    10

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    rt_err_t res;

    res = rt_sem_release(bike_uart_sem);

    return res;
}

void bike_uart_thread(void *args)
{
    size_t recv;
    rt_uint8_t recv_byte;
    char *res_pkg = malloc(64*sizeof(char));
    while (1)
    {
        rt_sem_take(bike_uart_sem, RT_WAITING_FOREVER);

        recv = rt_device_read(bike_uart, 0, &recv_byte, sizeof(recv_byte));
        if (recv == sizeof(recv_byte) && recv_byte == HALL_FLAG)
        {
           /* rt_enter_critical();
            hall_count++;
            rt_exit_critical();*/
            if ( dev_stat == 2)
            {
                res_pkg = pt_pack(PACKAGE_WORK_DATA, 1 , 0);
                rt_device_write(serial, 0, res_pkg, strlen(res_pkg));
                rt_free(res_pkg);
            }
            //rt_kprintf("hall_count = %d\n", hall_count);
        }
    }
}

int bike_uart_init(void)
{
    /************************  配置串口  *********************************************/
    bike_uart = rt_device_find(BIKE_USART_NAME);
    if (bike_uart == RT_NULL)
    {
        LOG_E("find bike_uart ERROR");
        return RT_ERROR;
    }

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.bufsz = 128;
    if (rt_device_control(bike_uart, RT_DEVICE_CTRL_CONFIG, &config) != RT_EOK)
    {
        LOG_E("configure bike_uart ERROR");
        return RT_ERROR;
    }

    if (rt_device_open(bike_uart, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_DMA_RX) != RT_EOK)
    {
        LOG_E("open bike_uart ERROR");
        return RT_ERROR;
    }

    rt_device_set_rx_indicate(bike_uart, uart_input);

    /***************************  配置停止检测定时器  ******************************/
/*    stop_check_timer = rt_timer_create("stop_check_timer", stop_check_timer_callback, RT_NULL,
            check_times*RT_TICK_PER_SECOND, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);*/

    /***************************  配置信号量  **************************************/
    bike_uart_sem = rt_sem_create("bike_uart_sem", 0, RT_IPC_FLAG_FIFO);
    if (bike_uart == RT_NULL)
    {
        LOG_E("create bike_uart_sem ERROR");
        return RT_ERROR;
    }


    /***************************  配置线程  ******************************************/
    bike_uart_tid = rt_thread_create("bike_uart",
                            bike_uart_thread,
                            RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY,
                            THREAD_TIMESLICE);
    if (bike_uart == RT_NULL)
    {
        LOG_E("bike_uart thread create ERROR");
        return RT_ERROR;
    }
    rt_thread_startup(bike_uart_tid);

    return RT_EOK;
}
INIT_APP_EXPORT(bike_uart_init);
