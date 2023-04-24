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
#include "nlist.h"
#include "mblock.h"
#include "pktbuf.h"

net_err_t netdev_init(void){
    netif_pcap_open();

    return NET_ERR_OK;
}
typedef struct _tnode_t {
    int id;
    nlist_node_t node;
} tnode_t;

// C语言中函数声明对函数的检测最主要的标准是函数名，C语言中函数参数列表中的参数名可以缺省
// C语言当中参数列表中的数据类型也可以缺省，默认是int。
// add()并不等价于add(void);  add(void)明确指明add函数不接收任何参数，若对其传参会报错，提示是error；
// 而add()表示接收参数，他的类型是int，只是他是一种没有意义的接收，并不会用这个参数,提示是wring；

void nlist_test(void) {
    #define NODE_CNT   4

    tnode_t node[NODE_CNT];
    nlist_t list;

    nlist_init(&list);
    for(int i=0; i<NODE_CNT; i++) {
        node[i].id = i;
        nlist_insert_first(&list, &node[i].node);
    }

    plat_printf("insert first\n");

    nlist_node_t * p;
    nlist_for_each(p, &list){
        tnode_t * t = nlist_entry(p, tnode_t, node);
        plat_printf("id: %d\n", t->id);
    }

    plat_printf("remove first\n");
    for(int i=0; i<NODE_CNT; i++) {
        p = nlist_remove_first(&list);
        plat_printf("id:%d\n", nlist_entry(p, tnode_t, node)->id);
    }

    plat_printf("insert last\n");
    nlist_init(&list);
    for(int i=0; i<NODE_CNT; i++) {
        node[i].id = i;
        nlist_insert_last(&list, &node[i].node);
    }

    nlist_for_each(p, &list){
        tnode_t * t = nlist_entry(p, tnode_t, node);
        plat_printf("id: %d\n", t->id);
    }

    plat_printf("remove last\n");
    for(int i=0; i<NODE_CNT; i++) {
        p = nlist_remove_last(&list);
        plat_printf("id:%d\n", nlist_entry(p, tnode_t, node)->id);
    }
    
    plat_printf("remove after\n");
    for(int i=0; i<NODE_CNT; i++) {
        nlist_insert_after(&list, nlist_first(&list), &node[i].node);
    }

    nlist_for_each(p, &list){
        tnode_t * t = nlist_entry(p, tnode_t, node);
        plat_printf("id: %d\n", t->id);
    }

}

void mblock_test (void) {
    mblock_t blist;
    static uint8_t buffer[100][10]; // C语言中二维数组的内存布局？

    mblock_init(&blist, buffer, 100, 10, NLOCKER_THREAD);

    void * temp[10];
    for (int i = 0; i < 10; i++) {
        temp[i] = mblock_alloc(&blist, 0);
        plat_printf("block: %p, free_count: %d\n", temp[i], mblock_free_cnt(&blist));
    }

    for (int i = 0; i < 10; i++) {
        mblock_free(&blist, temp[i]);
        plat_printf("free count: %d\n", mblock_free_cnt(&blist));
    }
}

void pktbuf_test(void) {
    pktbuf_t * buf = pktbuf_alloc(2000);
    pktbuf_free(buf);

    buf = pktbuf_alloc(2000);
    for (int i = 0; i < 16; i++) {
        pktbuf_add_header(buf, 33, 1);
    }

    for (int i = 0; i < 16; i++) {
        pktbuf_remove_header(buf, 33);
    }

    for (int i = 0; i < 16; i++) {
        pktbuf_add_header(buf, 33, 0);
    }

    for (int i = 0; i < 16; i++) {
        pktbuf_remove_header(buf, 33);
    }

    pktbuf_free(buf);

    buf = pktbuf_alloc(8);
    pktbuf_resize(buf, 32);
    pktbuf_resize(buf, 288);
    pktbuf_resize(buf, 4922);
    pktbuf_resize(buf, 1921);
    pktbuf_resize(buf, 288);
    pktbuf_resize(buf, 32);
    pktbuf_resize(buf, 0);
    pktbuf_free(buf);

    buf = pktbuf_alloc(689);
    pktbuf_t * sbuf = pktbuf_alloc(892);
    pktbuf_join(buf, sbuf);
    pktbuf_free(buf);

    // 32 - 4 - 16 - 54 - 32 - 38
    buf = pktbuf_alloc(32);
    pktbuf_join(buf, pktbuf_alloc(4));
    pktbuf_join(buf, pktbuf_alloc(16));
    pktbuf_join(buf, pktbuf_alloc(54));
    pktbuf_join(buf, pktbuf_alloc(32));
    pktbuf_join(buf, pktbuf_alloc(38));

    pktbuf_set_cont(buf, 44);
    pktbuf_set_cont(buf, 60);
    pktbuf_set_cont(buf, 44);
    pktbuf_set_cont(buf, 128);
    pktbuf_set_cont(buf, 135);  // 会出错
    pktbuf_free(buf);

    buf = pktbuf_alloc(32);
    pktbuf_join(buf, pktbuf_alloc(4));
    pktbuf_join(buf, pktbuf_alloc(16));
    pktbuf_join(buf, pktbuf_alloc(54));
    pktbuf_join(buf, pktbuf_alloc(32));
    pktbuf_join(buf, pktbuf_alloc(38));
    pktbuf_join(buf, pktbuf_alloc(512));
    pktbuf_join(buf, pktbuf_alloc(1000));

    pktbuf_reset_acc(buf);

    static uint16_t temp[1000];
    for (int i = 0; i < 1000; i++) {
        temp[i] = i;
    }
    pktbuf_write(buf, (uint8_t *)temp, pktbuf_total(buf));

    static uint16_t read_temp[1000];
    plat_memset(read_temp, 0, sizeof(read_temp));

    pktbuf_reset_acc(buf);
    pktbuf_read(buf, (uint8_t *)read_temp, pktbuf_total(buf));
    if (plat_memcmp(temp, read_temp, pktbuf_total(buf)) != 0) {
        plat_printf("not equal");
        return;
    }

    plat_memset(read_temp, 0, sizeof(read_temp));
    pktbuf_seek(buf, 18 * 2);
    pktbuf_read(buf, (uint8_t *)read_temp, 56);
    if (plat_memcmp(temp + 18, read_temp, 56) != 0) {
        plat_printf("not equal");
        return;
    }

    plat_memset(read_temp, 0, sizeof(read_temp));
    pktbuf_seek(buf, 85 * 2);
    pktbuf_read(buf, (uint8_t *)read_temp, 256);
    if (plat_memcmp(temp + 85, read_temp, 256) != 0) {
        plat_printf("not equal");
        return;
    }
}

void basic_test(void) {
    nlist_test();
    mblock_test();
    pktbuf_test();
}

#define DBG_TEST    DBG_LEVEL_INFO
int main(int argc, char const *argv[]){
    dbg_info(DBG_TEST, "info");
    dbg_error(DBG_TEST, "error");
    dbg_waring(DBG_TEST, "warning");
    dbg_assert(1==1, "failed");
    // dbg_assert(1==0, "failed");

    net_init();

    basic_test();

    net_start();

    netdev_init();

    while(1){
        sys_sleep(100);
    }
    return 0;
}