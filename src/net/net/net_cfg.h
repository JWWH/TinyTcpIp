/**
 * @file net_cfg.h
 * @author JWWH
 * @brief 协议栈的配置文件
 * @version 0.1
 * @date 2023-04-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef NET_CFG_H
#define NET_CFG_H

// 调试信息输出
#define DBG_MBLOCK	DBG_LEVEL_INFO
#define DBG_QUEUE	DBG_LEVEL_INFO
#define DBG_MSG		DBG_LEVEL_INFO
#define DBG_BUF		DBG_LEVEL_INFO
#define DBG_INIT	DBG_LEVEL_INFO
#define DBG_PLAT	DBG_LEVEL_INFO

#define EXMSG_MSG_CNT		10	// 消息队列的消息数
#define EXMSG_LOCKER		NLOCKER_THREAD	// 消息队列的锁类型


#define PKTBUF_BLK_SIZE		128	// 数据包块大小
#define PKTBUF_BLK_CNT		100	// 数据包块的数量
#define PKTBUF_BUF_CNT		100	// 数据包的数量

#endif