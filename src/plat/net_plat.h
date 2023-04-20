/**
 * @file net_plat.h
 * @author JWWH
 * @brief 用于硬件平台移植初始化
 * @version 0.1
 * @date 2023-04-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef NET_PLAT_H
#define NET_PLAT_H

#include "net_err.h"
#include "sys_plat.h"

net_err_t net_plat_init(void);

#endif