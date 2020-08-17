/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-30     Liuzeqi       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "frame.h"
#include "protocol.h"
#include "pt_common.h"
#include "stdlib.h"
#include "string.h"
#include "uart_rcv.h"
#include "easyflash.h"
#include "dev_common.h"

#define DEBUG 0

#define SAMPLE_UART_NAME       "uart3"

#define PRINTF      rt_kprintf
#define PACK_BUFF_SIZE  64
#define MALLOC(p)   char *(p) = rt_malloc(PACK_BUFF_SIZE * sizeof(char));
#define CHECK_MALLOC(p)    \
do {                       \
    if ((p) == NULL) {       \
        PRINTF("malloc error %s, %d \n", __FILE__, __LINE__);\
        return NULL;            \
    } \
} while(0)

/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};

/* 串口设备句柄 */
rt_device_t serial;

 /* 初始化配置参数 ，用于配置波特率等*/
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

/* 消息队列控制块 */
static struct rt_messagequeue rx_mq;

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
#if DEBUG
        /* 消息队列满 */
        rt_kprintf("message queue full！\n");
#endif
    }
    return result;
}

char* Int2String(int num,char *str)//10进制
{
    int i = 0;//指示填充str
    if(num<0)//如果num为负数，将num变正
    {
        num = -num;
        str[i++] = '-';
    }
    //转换
    do
    {
        str[i++] = num%10+48;//取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0'
        num /= 10;//去掉最低位
    }while(num);//num不为0继续循环

    str[i] = '\0';

    //确定开始调整的位置
    int j = 0;
    if(str[0]=='-')//如果有负号，负号不用调整
    {
        j = 1;//从第二位开始调整
        ++i;//由于有负号，所以交换的对称轴也要后移1位
    }
    //对称交换
    for(;j<i/2;j++)
    {
        //对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b;
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    }

    return str;//返回转换后的值
}

/*cmd_args 接收指令，n为指令数据段*/
int check_dev_id(char *cmd_args, int n)
{
    char *type = UNPACK_FIELD(cmd_args, PK_DEV_TYPE_FIELD);
    char *id = UNPACK_FIELD(cmd_args, PK_DEV_ID_FIELD);

    return (atoi(type) == dev_type && atoi(id) == dev_id);
}

int parse_pkg (char * unpack_buff ,int para_num)
{
    char* res_pkg = rt_malloc(PACK_BUFF_SIZE*sizeof(char));   //返回包
    char* res_data = rt_malloc(10*sizeof(char));

    PACKAGE_TYPE pkg_type = atoi(UNPACK_FIELD(unpack_buff, 0));
    int ins = atoi(UNPACK_FIELD(unpack_buff, 3));
    int op;    //五号包修改参数ID操作类型 op=1为查询 op=2为修改

    PACKAGE_TYPE res_pkg_type;
    int res_id;  //数据类型


    if( para_num > 4)
    {
        op = atoi(UNPACK_FIELD(unpack_buff, 4));
        if ( !( (pkg_type ==  PACKAGE_PARAM_CMD) && (ins == 9) && ( op == 1)
                || ((pkg_type ==  PACKAGE_STATE_CMD) && (ins == PK1_QUERY_DEV_ID)) ) )
        {
            if(! check_dev_id(unpack_buff, para_num))
            {
                rt_kprintf("设备类型、设备ID验证失败！\n");
                return -1;
            }
        }
    }
    else
    {
        if ( !((pkg_type ==  PACKAGE_STATE_CMD) && (ins == PK1_QUERY_DEV_ID)) )
        {
            if(! check_dev_id(unpack_buff, para_num))
            {
                rt_kprintf("设备类型、设备ID验证失败！\n");
                return -1;
            }
        }
    }

    switch (pkg_type)
    {
        case (PACKAGE_STATE_CMD):   //接收到一号包
            res_pkg_type = PACKAGE_STATE_DATA;
            if ( ins == 1 )                      //查询设备状态
            {
                res_id = 1;
                res_pkg = pt_pack(res_pkg_type, res_id, &dev_stat);
                rt_device_write(serial, 0, res_pkg, strlen(res_pkg));
                rt_free(res_pkg);
            }
            if ( ins == 2 )                     //返回设备ID
            {
                res_id = 2;
                rt_snprintf(res_data, 4, "%2d%2d",dev_type,dev_id);
                res_pkg = pt_pack(res_pkg_type, res_id, res_data);
                rt_device_write(serial, 0, res_pkg, strlen(res_pkg));
                rt_free(res_data);
                rt_free(res_pkg);
            }
            if ( ins == 3)
            {
                res_pkg = pt_pack(PACKAGE_STATE_DATA, 3, "99999999999999999999999999");
                rt_device_write(serial, 0, res_pkg, strlen(res_pkg));
                rt_free(res_pkg);
            }
            if ( ins == 4)//开始测试
            {
                dev_stat = 2;
                res_id = 1;
                res_pkg = pt_pack(res_pkg_type, res_id, &dev_stat);
                rt_device_write(serial, 0, res_pkg, strlen(res_pkg));
                rt_free(res_pkg);
            }
            if ( ins == 5)//结束测试
            {
                dev_stat = 1;
                res_id = 1;
                res_pkg = pt_pack(res_pkg_type, res_id, &dev_stat);
                rt_device_write(serial, 0, res_pkg, strlen(res_pkg));
                rt_free(res_pkg);
             }

            break;
        case (PACKAGE_WORK_CMD):
            res_pkg_type = PACKAGE_WORK_DATA;
            if ( ins == 1 )
            {
                if(dev_stat==2)
                {

                }
            }
            if ( ins == 2 )
            {
                if(dev_stat == 2)
                {

                }
            }
            break;
        case (PACKAGE_PARAM_CMD):
            res_pkg_type = PACKAGE_PARAM_DATA;
            if ( ins == 9)  //设备ID操作
            {
                if ( op == 1 ) //读  设备ID
                {
                    char *devid = ef_get_env("dev_id");
                    res_pkg = pt_pack(PACKAGE_PARAM_DATA, 9, devid);
                    rt_device_write(serial, 0, res_pkg, strlen(res_pkg));
                    rt_free(res_pkg);
                }
                if ( op == 2)  //修改 设备ID
                {
                    dev_id = atoi(UNPACK_FIELD(unpack_buff, 5));
                    char devid_a[10];
                    Int2String(dev_id,devid_a);
                    ef_set_env("dev_id", devid_a);
                }
            }
            break;
    }/*end of switch*/

    return RT_EOK;
}

int ext_commond(char * del_frame_buff , int frame_length)
{
    char *unpack_buff ;
    char p_del_frame_buff[PACK_BUFF_SIZE] = {'\0'};
    int para_num;

//    strncpy((char*)p_del_frame_buff, del_frame_buff, (size_t)frame_length);
    memcpy(p_del_frame_buff, del_frame_buff, frame_length);
    p_del_frame_buff[frame_length] = '\0';
    unpack_buff = pt_unpack(p_del_frame_buff, &para_num);

    parse_pkg (unpack_buff ,para_num);
#if DEBUG
    rt_kprintf("del_frame_buff : %s , len = %d \n" ,p_del_frame_buff ,frame_length);
    for (int i = 0; i<para_num; i++)
    {
        rt_kprintf("unpack_buff[%d] : %s  \n" , i, unpack_buff+i*10);
    }
#endif

    PT_FREE(unpack_buff);
    return RT_EOK;
}

int pcUart_send_msg(PACKAGE_TYPE pkg_state_data,int pk2_user_id , char * user_id)
{
    char *pack_buff = rt_malloc(PACK_BUFF_SIZE * sizeof(char));
    if (pack_buff == RT_NULL)
    {
        return RT_ERROR;
    }
    char *user = user_id;
    rt_snprintf(pack_buff, PACK_BUFF_SIZE, "{%d%.2d%.2d%d[%s]}",
            pkg_state_data, dev_type, dev_id, pk2_user_id, user);
    rt_device_write(serial, 0, pack_buff, strlen(pack_buff));
    rt_free(pack_buff);
    return RT_EOK;
}

static void serial_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;
    static char rx_buffer[RT_SERIAL_RB_BUFSZ + 1];

    struct frame frame_buff  = FRAME_CONFIG_DEFAULT;
    int frame_length;

    char * del_frame_buff;

    char *res_pkg;
    int res_id = 1;
    res_pkg = pt_pack(PACKAGE_STATE_DATA, res_id, &dev_stat);
    rt_device_write(serial, 0, res_pkg, strlen(res_pkg));
    rt_free(res_pkg);

    while (1)
    {
        rt_memset(&msg, 0, sizeof(msg));

        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            /* 从串口读取数据*/
            rx_length = rt_device_read(msg.dev, 0, rx_buffer, msg.size);
            rx_buffer[rx_length] = '\0';

            del_frame_buff = frame_has_complete_data(&frame_buff, &frame_length, rx_buffer, rx_length);
            if (del_frame_buff != RT_NULL && frame_length > 0)
            {
                ext_commond(del_frame_buff, frame_length);
                frame_reset(&frame_buff);
            }

#if DEBUG
            rt_kprintf("get uart buff : %s , len = %d \n" ,rx_buffer ,rx_length);
#endif


        }
    }
}



static int uart_rcv_main(void)
{
    static char msg_pool[256];
#if DEBUG
    char str[] = "hello RT-Thread!\r\n";
#endif

    /* 查找串口设备 */
    serial = rt_device_find(SAMPLE_UART_NAME);
    if (!serial)
    {
#if DEBUG
        rt_kprintf("find %s failed!\n", SAMPLE_UART_NAME);
#endif
    }

    /* step：修改串口配置参数 */
    config.baud_rate = BAUD_RATE_115200;        //修改波特率为 9600
    config.data_bits = DATA_BITS_8;           //数据位 8
    config.stop_bits = STOP_BITS_1;           //停止位 1
    config.bufsz     = 128;                   //修改缓冲区 buff size 为 128
    config.parity    = PARITY_NONE;           //无奇偶校验位

    /* step：控制串口设备。通过控制接口传入命令控制字，与控制参数 */
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

    /* 初始化消息队列 */
    rt_mq_init(&rx_mq,
                "rx_mq",
                msg_pool,                 /* 存放消息的缓冲区 */
                sizeof(struct rx_msg),    /* 一条消息的最大长度 */
                sizeof(msg_pool),         /* 存放消息的缓冲区大小 */
                RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    /* 以 DMA 接收及轮询发送方式打开串口设备 */
    rt_err_t res = rt_device_open(serial,  RT_DEVICE_FLAG_DMA_RX);
    if ( res != RT_EOK )
    {
#if DEBUG
           rt_kprintf("error %d \n", res);
#endif
    }

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);

#if DEBUG
    /* 发送字符串 */
    rt_device_write(serial, 0, str, (sizeof(str) - 1));
#endif

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 25, 10);

    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
#if DEBUG
        rt_kprintf("Create %s Entry failed!\n", SAMPLE_UART_NAME);
#endif
    }

    return RT_EOK;
}

INIT_APP_EXPORT(uart_rcv_main);


