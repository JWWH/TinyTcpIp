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
#ifndef MBLOCK_H
#define MBLOCK_H

#include "nlist.h"
#include "nlocker.h"

typedef struct _mblock_t {
	nlist_t free_list;	// 存储块链表
	void* start;		// 将一个大数组构造成空闲链表free_list，start就用来记录大数组的起始地址
	nlocker_t locker;	// 1、处理线程之间的互斥 2、处理线程与中断之间共享资源的互斥（用于将协议栈移植到嵌入式系统上比如arm中处理硬件中断）
	sys_sem_t alloc_sem;	// 信号量，用于线程同步

} mblock_t;

/**
 * @brief 初始化存储块管理器
 * 
 * @param mblock 
 * @param mem 建立链表的那块内存空间（大数组）的起始地址
 * @param blk_size 数组是由很多块相同内存大小的内存组成的，blk_size就是每个块的大小
 * @param cnt 数组中有多少块内存块
 * @param share_type 分配释放用在哪种场合：1、线程和线程间内存分配释放 2、线程和中断间内存分配释放
 * @return net_err_t 
 */
net_err_t mblock_init(mblock_t * mblock, void * mem, int blk_size, int cnt, nlocker_type_t share_type);

/**
 * @brief 分配一个空闲的存储块
 * 
 * @param mblock
 * @param ms 如果mblock中空闲链表freelist为空，那么调用mblock_alloc的线程会阻塞等待
 *           如果不想让线程无限制的一直死等下去，就需要设置一个超时时间，如果超时时间到了，还没有分配到空闲的存储块，那么就返回一个空指针
 */
void * mblock_alloc(mblock_t * mblock, int ms);
// 返回mblock中空闲链表中的空闲块的个数
int mblock_free_cnt(mblock_t * mblock);
// 释放内存块
void mblock_free(mblock_t * mblock, void * block);
// 销毁内存块
void mblock_destroy(mblock_t * mblock);
#endif