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
#include "nlocker.h"

net_err_t nlocker_init(nlocker_t* locker, nlocker_type_t type) {
	if(type == NLOCKER_THREAD){
		sys_mutex_t mutex = sys_mutex_create();
		if(mutex == SYS_MUTEx_INVALID) {
			return NET_ERR_SYS;
		}
		locker->mutex = mutex;
	}
	locker->type = type;
	return NET_ERR_OK;
}

void nlocker_destroy(nlocker_t* locker) {
	if (locker->type == NLOCKER_THREAD) {
		sys_mutex_free(locker->mutex);
	}
}
void nlocker_lock(nlocker_t* locker) {
	if (locker->type == NLOCKER_THREAD) {
		sys_mutex_lock(locker->mutex);
	}
}
void nlocker_unlock(nlocker_t* locker) {
	if (locker->type == NLOCKER_THREAD) {
		sys_mutex_unlock(locker->mutex);
	}
}