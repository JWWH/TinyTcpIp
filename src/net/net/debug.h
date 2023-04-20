/**
 * @file debug.h
 * @author JWWH
 * @brief 调试信息输出
 * @version 0.1
 * @date 2023-04-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef DEBUG_H
#define DEBUG_H

#include "net_cfg.h"

// 调试信息的显示样式设计
#define DBG_STYLE_RESET 	"\033[0m"   	// 复位显示
#define DBG_STYLE_ERROR 	"\033[31m"	// 红色显示
#define DBG_STYLE_WARNING 	"\033[33m"	// 黄色显示

// 调试等级
#define DBG_LEVEL_NONE		0
#define DBG_LEVEL_ERROR		1
#define DBG_LEVEL_WARNING	2
#define DBG_LEVEL_INFO		3


void dbg_printf(int m_level, int s_level, const char* file, const char* func, const int line, const char* fmt, ...);
#define dbg_info(module, fmt, ...) dbg_printf(module, DBG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define dbg_error(module, fmt, ...) dbg_printf(module, DBG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define dbg_waring(module, fmt, ...) dbg_printf(module, DBG_LEVEL_WARNING, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

// 断言
#define dbg_assert(expr, msg) {\
	if(!(expr)){\
		dbg_printf(DBG_LEVEL_ERROR, DBG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "assert failed:"#expr","msg);\
		while(1);\
	}\
}

#endif 