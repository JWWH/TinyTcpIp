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
#include "debug.h"

/**
 * @brief 数据包接收线程，不断的使用pcap库从网卡接收数据包
 * 
 * @param arg 
 */
void recv_thread(void* arg){
	plat_printf("recv thread is running...\n");

	netif_t* netif = (netif_t *)arg;
	pcap_t* pcap = (pcap_t *)netif->ops_data;

	while (1)
	{
		// 从网卡中获取一个数据包

		/*
		这里必须加上struct，否则会报错 error: must use 'struct' tag to refer to type 'pcap_pkthdr'
		原因详见https://www.cnblogs.com/vincentqliu/p/typedef_struct.html
		
		*/
		struct pcap_pkthdr* pkthdr;
		const uint8_t* pkt_data;
		// 返回值： 1 - 成功读取数据包， 0 - 没有数据包， 其他值 - 出错
		if (pcap_next_ex(pcap, &pkthdr, &pkt_data) != 1) {
			continue;
		}

		// 将data转变为缓存结构
		pktbuf_t* buf = pktbuf_alloc(pkthdr->len);
		if (buf == (pktbuf_t *)0) {
			dbg_error(DBG_NETIF, "buf == NULL");
			continue;
		}

		// 将数据包的内容写入到数据链表中
		pktbuf_write(buf, (uint8_t*)pkt_data, pkthdr->len);

		if (netif_put_in(netif, buf, 0) < 0) {
			dbg_error(DBG_NETIF, "netif %s in_q full", netif->name);
			pktbuf_free(buf); // 将刚才分配的数据包缓存释放掉
			continue;
		}
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

/**
 * @brief pcap设备打开
 * 
 * @param netif 打开的接口
 * @param data 传入的驱动数据
 * @return net_err_t 
 */
static net_err_t netif_pcap_open(struct _netif_t* netif, void * data) {
	// 借助pcap库打开底层网卡设备
	pcap_data_t * dev_data = (pcap_data_t *)data;
	pcap_t * pcap = pcap_device_open(dev_data->ip, dev_data->hwaddr);
	if (pcap == (pcap_t *)0) {
		dbg_error(DBG_NETIF, "pcap open failed! name: %s\n", netif->name);
		return NET_ERR_IO;
	}

	// 打开网卡之后对网络接口结构进行设置
	netif->type = NETIF_TYPE_ETHER;
	netif->mtu = 1500;
	// netif的ops_data字段保存与网络接口操作相关的数据
	// 在之后进行网络接口的关闭和数据收发时都需要借助pcap这个指针
	netif->ops_data = pcap;
	netif_set_hwaddr(netif, dev_data->hwaddr, 6); // 以太网硬件地址的长度为6个字节

	sys_thread_create(recv_thread, netif);
	sys_thread_create(xmit_thread, netif);


	return NET_ERR_OK;
}
void netif_pcap_close (struct _netif_t* netif) {
	pcap_t * pcap = (pcap_t *)netif->ops_data;
	pcap_close(pcap);
}

static net_err_t netif_pcap_xmit (struct _netif_t* netif) {
	return NET_ERR_OK;
}

/**
 * @brief pcap驱动结构
 * 
 */
const netif_ops_t netdev_ops = {
	.open = netif_pcap_open,
	.close = netif_pcap_close,
	.xmit = netif_pcap_xmit,
};