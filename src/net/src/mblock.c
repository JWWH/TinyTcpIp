/**
 * @file mblock.c
 * @author JWWH
 * @brief 存储块管理器，将固定大小的存储块组织成块链表，并允许从中申请和释放块
 * @version 0.1
 * @date 2023-04-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "mblock.h"
#include "debug.h"
#include "net_cfg.h"

/**
 * @brief 初始化存储块管理器
 * 将mem开始的内存区域划分成多个相同大小的内存块， 然后用链表链接起来 
 */
net_err_t mblock_init(mblock_t * mblock, void * mem, int blk_size, int cnt, nlocker_type_t share_type) {
	uint8_t * buf = (uint8_t *)mem;

	nlist_init(&mblock->free_list);
	for(int i = 0; i < cnt; i++, buf += blk_size) {
		nlist_node_t * block = (nlist_node_t *)buf;
		nlist_node_init(block);
		nlist_insert_last(&mblock->free_list, block);
	}

	nlocker_init(&mblock->locker, share_type);

	if(share_type == NLOCKER_THREAD) {
		mblock->alloc_sem = sys_sem_create(cnt);
		if(mblock->alloc_sem == SYS_SEM_INVALID) {
			dbg_error(DBG_MBLOCK, "create sem failed.");
			nlocker_destroy(&mblock->locker);
			return NET_ERR_SYS;
		}
	}

	mblock->start = mem;

	return NET_ERR_OK;
}

/**
 * @brief 分配一个空闲的存储块
 * 
 * @param mblock
 * @param ms 如果mblock中空闲链表freelist为空，那么调用mblock_alloc的线程会阻塞等待
 *           如果不想让线程无限制的一直死等下去，就需要设置一个超时时间，如果超时时间到了，还没有分配到空闲的存储块，那么就返回一个空指针
 */
void * mblock_alloc(mblock_t * mblock, int ms) {
	// 无等待的分配， 查询后直接退出
	if ((ms < 0) ||( mblock->locker.type== NLOCKER_NONE)){
		nlocker_lock(&mblock->locker);
		int count = nlist_count(&mblock->free_list);
		if (count == 0) {
			nlocker_unlock(&mblock->locker);
			return (void *)0;
		} else {
			nlist_node_t * block = nlist_remove_first(&mblock->free_list);
			nlocker_unlock(&mblock->locker);
			return block;
		}
	} else {
		// 等待信号量
		// 如果ms等于0，那么就一直等待，直到有空闲的存储块
		// 如果ms大于0，那么就等待ms毫秒，如果ms毫秒内没有空闲的存储块，那么就返回一个空指针
		if (sys_sem_wait(mblock->alloc_sem, ms) < 0) {
			return (void *)0;
		} else {
			nlocker_lock(&mblock->locker);
			nlist_node_t * block = nlist_remove_first(&mblock->free_list);
			nlocker_unlock(&mblock->locker);
			return block;
		}
		
	}
}

// 返回mblock中空闲链表中的空闲块的个数
int mblock_free_cnt (mblock_t * mblock) {
	nlocker_lock(&mblock->locker);
	int cnt = nlist_count(&mblock->free_list);
	nlocker_unlock(&mblock->locker);
	return cnt;
}

// 释放内存块
void mblock_free (mblock_t * mblock, void * block) {
	nlocker_lock(&mblock->locker);
	nlist_insert_last(&mblock->free_list, (nlist_node_t *)block);
	nlocker_unlock(&mblock->locker);

	if(mblock->locker.type == NLOCKER_THREAD) {
		sys_sem_notify(mblock->alloc_sem);
	}
}
// 销毁内存块
void mblock_destroy (mblock_t * mblock) {
	if (mblock->locker.type != NLOCKER_NONE) {
		sys_sem_free(mblock->alloc_sem);
		nlocker_destroy(&mblock->locker);
	}
}