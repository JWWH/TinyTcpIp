/**
 * @file netif_pcap.h
 * @author JWWH
 * @brief 使用pcap_device创建的虚拟网络接口
 * @version 0.1
 * @date 2023-04-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef NETIF_PCAP_H
#define NETIF_PCAP_H

#include "net_err.h"

net_err_t netif_pcap_open(void);

#endif