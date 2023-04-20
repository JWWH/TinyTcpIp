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
#include <stdarg.h> // 处理可变参数的头文件
#include "debug.h"
#include "sys_plat.h"

void dbg_printf(int m_level, int s_level, const char* file, const char* func, const int line, const char* fmt, ...){
	static const char * title[] = {
		[DBG_LEVEL_NONE] = "none",
		[DBG_LEVEL_ERROR] = DBG_STYLE_ERROR"error",
		[DBG_LEVEL_WARNING] = DBG_STYLE_WARNING"warning",
		[DBG_LEVEL_INFO] = "info",
	};
	if (m_level >= s_level){
		const char * end = file + plat_strlen(file);
		while(end >= file){
			if((*end == '\\') || (*end == '/')){
				break;
			}
			end--;
		}
		end++;

		plat_printf("%s(%s-%s-%d):", title[s_level], end, func, line);

		char str_buf[128];

		// C库函数，处理可变参数
		// 可变参数最终保存在args中
		va_list args;
		va_start(args, fmt);
		plat_vsprintf(str_buf, fmt, args);

		plat_printf("%s\n"DBG_STYLE_RESET, str_buf);

		va_end(args);
	}
}