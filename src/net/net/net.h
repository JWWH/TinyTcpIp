/**
 * @file net.h
 * @author JWWH
 * @brief 协议栈初始化和启动
 * @version 0.1
 * @date 2023-04-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef NET_H
#define NET_H

#include "net_err.h"

net_err_t net_init(void);
net_err_t net_start(void);

#endif