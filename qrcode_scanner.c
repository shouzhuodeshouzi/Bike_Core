/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-09     Jackis       the first version
 */

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>
#include <string.h>

//#include "sit-and-reach.h"
//#include "serial_port_of_host_computer.h"
#include "pt_common.h"
#include "protocol.h"
#include "uart_rcv.h"
#define DBG_TAG "qrcode_scanner"

#define DBG_LVL DBG_LOG
#include <rtdbg.h>

char *user_id = RT_NULL;


const char* pre_post_fix = "####";
#define USER_ID_LEN     26
/*
 * PA2-- TX,   PA3 -- RX
 */
#define UART_QRCODE      "uart2"
static rt_device_t uart_qrcode = RT_NULL;

static rt_mailbox_t mb_qrcode = RT_NULL;

static rt_thread_t tid = RT_NULL;       // �����߳�
#define THREAD_PRIORITY     25
#define THREAD_STACK_SIZE   1024
#define THREAD_TIMESLICE    10


static int is_legal(char* str);

#define QRCODE_BUFF_LEN 64
static char qrcode_buff[QRCODE_BUFF_LEN];
static rt_err_t qrcode_uart_input(rt_device_t dev, rt_size_t size)
{
    static uint8_t pos = 0;
    char c;
    if (rt_device_read(uart_qrcode, 0, (void *)&c, sizeof(char)) > 0)
    {
        if (c == '\r')
        {
            return RT_EOK;
        }
        else if (c == '\n')
        {
            qrcode_buff[pos] = '\0';
            rt_mb_send(mb_qrcode, pos);
            pos = 0;
            return RT_EOK;
        }
        else
            qrcode_buff[pos] = c;

        pos = (pos >= QRCODE_BUFF_LEN-1) ? 0 : pos+1;
    }

    return RT_EOK;
}

void thread_qrcode(void *args)
{
    rt_uint32_t rx_length = 0;
    static char rx_buffer[QRCODE_BUFF_LEN];
    static char user_qrcode_id[USER_ID_LEN+1];

    while (1)
    {
        if (rt_mb_recv(mb_qrcode, (rt_ubase_t*)&rx_length, RT_WAITING_FOREVER) == RT_EOK)
        {
            memcpy(&rx_buffer, &qrcode_buff, rx_length);
            LOG_D("%s receive: %s", uart_qrcode->parent.name, rx_buffer);

            size_t pre_post_len = strlen(pre_post_fix);
            if(strncmp(pre_post_fix, (char*)rx_buffer, pre_post_len)==0 &&
                strncmp(pre_post_fix, (char*)(rx_buffer+rx_length - pre_post_len), pre_post_len)==0)
            {
                rx_buffer[rx_length - pre_post_len] = '\0';
                char *id = (char*)rx_buffer + pre_post_len;
                if(strlen(id) == USER_ID_LEN && is_legal(id))
                {
                    int i = 0;
                    while(i < USER_ID_LEN && id[i] != '\0')
                    {
                        user_qrcode_id[i] = id[i];
                        i++;
                    }
                    user_qrcode_id[i] = '\0';

                    rx_length = 0;

                    //�ڴ˴�������������Ĵ���
                    user_id = user_qrcode_id;
                    pcUart_send_msg(PACKAGE_STATE_DATA, PK2_USER_ID, user_id);
                }
            }
        }
    }
}

int init_qrcode_scanner(void)
{
    uart_qrcode = rt_device_find(UART_QRCODE);
    if (uart_qrcode == RT_NULL)
    {
        LOG_E("fine uart2 ERROR");
        return RT_ERROR;
    }

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.bufsz = 64;
    if (rt_device_control(uart_qrcode, RT_DEVICE_CTRL_CONFIG, &config) != RT_EOK)
    {
        LOG_E("configure uart_qrcode ERROR");
        return RT_ERROR;
    }

    if (rt_device_open(uart_qrcode, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        LOG_E("open uart_qrcode ERROR");
        return RT_ERROR;
    }

    rt_device_set_rx_indicate(uart_qrcode, qrcode_uart_input);

    /***************************  ��������  **************************************/
    mb_qrcode = rt_mb_create("mb_qrcode", 5, RT_IPC_FLAG_FIFO);
   // ASSERT(mb_qrcode);


    /***************************  �����߳�  ******************************************/
    tid = rt_thread_create("thread_qrcode_scanner",
                            thread_qrcode,
                            RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY,
                            THREAD_TIMESLICE);
    if (tid == RT_NULL)
    {
        LOG_E("thread_qrcode_scanner thread create ERROR");
        return RT_ERROR;
    }
    rt_thread_startup(tid);

    return RT_EOK;
}
INIT_APP_EXPORT(init_qrcode_scanner);


/*
�ж������Ƿ�Ϸ�
*/
static int is_legal(char* str)
{
    int i = 0;
    char s = str[0];
    while(s != '\0')
    {
        if(!((s>='0')&&(s<='9'))||((s>='a')&&(s<='z'))||((s>='A')&&(s<='Z')))
            return 0;
        s = str[++i];
    }
    return 1;
}
