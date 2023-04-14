/**
 * @file main.c
 * @author JWWH
 * @brief 测试主程序
 * @version 0.1
 * @date 2023-04-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include "net.h"
#include "sys_plat.h"

#include "netif_pcap.h"
#include "debug.h"

net_err_t netdev_init(void){
    netif_pcap_open();

    return NET_ERR_OK;
}

#define DBG_TEST    DBG_LEVEL_INFO
int main(int argc, char const *argv[]){
    dbg_info(DBG_TEST, "info");
    dbg_error(DBG_TEST, "error");
    dbg_waring(DBG_TEST, "warning");
    dbg_assert(1==1, "failed");
    dbg_assert(1==0, "failed");

    net_init();
    net_start();

    netdev_init();

    while(1){
        sys_sleep(100);
    }
    return 0;
}