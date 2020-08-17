/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-01     Liuzeqi       the first version
 */
#ifndef APPLICATIONS_UART_RCV_H_
#define APPLICATIONS_UART_RCV_H_
#include "protocol.h"

extern rt_device_t serial;
int pcUart_send_msg(PACKAGE_TYPE pkg_state_data,int pk2_user_id , char * user_id);

#endif /* APPLICATIONS_UART_RCV_H_ */
