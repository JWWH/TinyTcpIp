/**
 * @file netif_pacp.c
 * @author JWWH
 * @brief 使用pcap_device创建的虚拟网络接口
 * @version 0.1
 * @date 2023-04-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "netif_pcap.h"
#include "sys_plat.h"
#include "exmsg.h"

// 网卡的接收线程
void recv_thread(void* arg){
	plat_printf("recv thread is running...\n");

	while (1)
	{
		sys_sleep(1);
		// 假设每隔一秒从网络上接受一个数据包
		// 然后调用exmsg_netif_in函数将接收到的数据包传递给工作线程
		exmsg_netif_in();
	}
	
}

// 网卡的发送线程
void xmit_thread(void* arg){
	plat_printf("xmit thread is running...\n");

	while (1)
	{
		sys_sleep(1);
	}
	
}

net_err_t netif_pcap_open(void){
	sys_thread_create(recv_thread, (void *)0);
	sys_thread_create(xmit_thread, (void *)0);
	return NET_ERR_OK;
}
