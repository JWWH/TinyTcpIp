/**
 * @file net.c
 * @author JWWH
 * @brief TCP/IP协议栈的初始化
 * @version 0.1
 * @date 2023-04-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "net.h"
#include "exmsg.h"
#include "net_plat.h"
#include "pktbuf.h"
#include "debug.h"
#include "netif.h"

net_err_t net_init(void){
	dbg_info(DBG_INIT, "init net");
	net_plat_init();
	
	exmsg_init();
	pktbuf_init();
	netif_init();
	return NET_ERR_OK;
}
net_err_t net_start(void){
	exmsg_start();
	dbg_info(DBG_INIT, "net is running");
	return NET_ERR_OK;
}