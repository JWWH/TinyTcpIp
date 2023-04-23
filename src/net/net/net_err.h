/**
 * @file net_err.h
 * @author JWWH
 * @brief 错误类型
 * @version 0.1
 * @date 2023-04-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef NET_ERR_H
#define NET_ERR_H

typedef enum _net_err_t {
	NET_ERR_OK = 0,
	NET_ERR_SYS = -1,
	NET_ERR_MEM = -2,   	// 内存分配相关的问题
	NET_ERR_FULL = -3,	// 缓存满
	NET_ERR_TMO = -4,	// 超时
	NET_ERR_SIZE = -5,
	NET_ERR_NONE = -6,
} net_err_t;

#endif