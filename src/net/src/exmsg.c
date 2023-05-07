/**
 * @file exmsg.c
 * @author JWWH
 * @brief TCP/IP核心线程通信模块,运行了一个核心线程，所有TCP/IP中相关的事件都交由该线程处理
 * @version 0.1
 * @date 2023-04-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "exmsg.h"
#include "net_cfg.h"
#include "fixq.h"
#include "debug.h"
#include "mblock.h"
#include "sys.h"

static void * msg_tbl[EXMSG_MSG_CNT];   	// 消息缓冲区
static fixq_t msg_queue;			// 消息队列
static exmsg_t msg_buffer[EXMSG_MSG_CNT];	// 消息块
static mblock_t msg_block;			// 消息分配器


net_err_t exmsg_init(void){
	dbg_info(DBG_MSG, " exmsg init.");

	net_err_t err = fixq_init(&msg_queue, msg_tbl, EXMSG_MSG_CNT, EXMSG_LOCKER);
	if (err < 0) {
		dbg_error(DBG_MSG, "fixq init failed");
		return err;
	}

	err = mblock_init(&msg_block, msg_buffer, sizeof(exmsg_t), EXMSG_MSG_CNT, EXMSG_LOCKER);
	if ( err < 0) {
		dbg_error(DBG_MSG, "mblock init failed");
		return err;
	}

	dbg_info(DBG_MSG, "init done");
	return NET_ERR_OK;
}
/**
 * @brief 收到来自网卡的消息
 * 
 * @param netif 
 * @return net_err_t 
 */
net_err_t exmsg_netif_in(netif_t * netif) {
	// 从mblock中分配一个消息结构
	// 超时时间设置为-1，表示不等待，原因是将协议栈移植到arm或者其他嵌入式平台时，中断在请求时不可能一直等待
	exmsg_t* msg = mblock_alloc(&msg_block, -1);
	// 如果分配失败了，网卡上接收的网络数据包会被丢弃，但是网络中丢失数据包是正常的，不会影响网络的正常运行
	if (!msg) {
		dbg_waring(DBG_MSG, "no free exmsg");
		return NET_ERR_MEM;
	}

	msg->type = NET_EXMSG_NETIF_IN;
	msg->netif.netif = netif;

	// 将构造完的消息发送给消息队列
	net_err_t err = fixq_send(&msg_queue, msg, -1);
	if (err < 0) {
		dbg_waring(DBG_MSG, "fixq full");
		mblock_free(&msg_block, msg);
		return err;
	}

	return err;
}

static net_err_t do_netif_in(exmsg_t * msg) {
	netif_t * netif = msg->netif.netif;

	pktbuf_t * buf;
	while ((buf = netif_get_in(netif, -1)))
	{
		dbg_info(DBG_MSG, "recv a packet");

		// 这里进行简单测试，把收到的数据包的前六个字节改写为0x11，然后再发送出去
		pktbuf_fill(buf, 0x11, 6);
		net_err_t err = netif_out(netif, (ipaddr_t *)0, buf);
		if (err < 0) {
			pktbuf_free(buf);
		}
	}

	return NET_ERR_OK;
}

static void work_thread(void *arg){
	dbg_info(DBG_MSG, "exmsg is running...\n");

	while(1){
		sys_sleep(1);
		// 从消息队列中取出一个消息
		// 超时时间设置为0，表明消息队列中没有消息时，工作线程会一直等待
		exmsg_t * msg = (exmsg_t *)fixq_recv(&msg_queue, 0);

		dbg_info(DBG_MSG, "recv a msg %p: %d\n", msg, msg->type);

		switch (msg->type)
		{
		case NET_EXMSG_NETIF_IN:
			do_netif_in(msg);
			break;
		
		default:
			break;
		}
		
		// 释放消息
		mblock_free(&msg_block, msg);
	}
}
net_err_t exmsg_start(void){
	sys_thread_t thread = sys_thread_create(work_thread, (void *)0);
	if(thread == SYS_THREAD_INVALID){
		return NET_ERR_SYS;
	}
}