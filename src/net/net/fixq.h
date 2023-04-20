/**
 * @file fixq.h
 * @author JWWH
 * @brief 定长消息队列
 * @version 0.1
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef FIXQ_H
#define FIXQ_H

#include "nlocker.h"
#include "sys.h"

typedef struct _fixq_t {
	int size;		// 消息容量
	int in, out, cnt;	// 读写位置索引和消息数

	void ** buf;		// 消息缓存

	nlocker_t locker;	// 访问的锁
	sys_sem_t recv_sem;	// 接收消息时使用的信号量
	sys_sem_t send_sem;	// 发送消息时使用的信号量
}fixq_t;

net_err_t fixq_init(fixq_t * q, void **buf, int size, nlocker_type_t type);
net_err_t fixq_send(fixq_t * q, void * msg, int tmo);
void * fixq_recv(fixq_t * q, int tmo);
void fixq_destory(fixq_t * q);
int fixq_count(fixq_t * q);
#endif