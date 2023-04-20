/**
 * @file nlocker.h
 * @author JWWH
 * @brief 资源锁
 * @version 0.1
 * @date 2023-04-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef NLOCKER_H
#define NLOCKER_H

#include "net_err.h"
#include "sys.h"

typedef enum _nlocker_type_t {
	NLOCKER_NONE,		// 不需要锁
	NLOCKER_THREAD,		// 用于线程共享的锁
}nlocker_type_t;

typedef struct _nlocker_t {
	nlocker_type_t type;

	// 根据不同的锁类型，放置不同的结构
	union {
		sys_mutex_t mutex;
	};
	
}nlocker_t;

net_err_t nlocker_init(nlocker_t* locker, nlocker_type_t type);
void nlocker_destroy(nlocker_t* locker);
void nlocker_lock(nlocker_t* locker);
void nlocker_unlock(nlocker_t* locker);


#endif