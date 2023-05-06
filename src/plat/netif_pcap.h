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
#include "netif.h"

typedef struct _pcap_data_t {
	const char * ip;
	const uint8_t * hwaddr;
}pcap_data_t;

extern const netif_ops_t netdev_ops;

#endif