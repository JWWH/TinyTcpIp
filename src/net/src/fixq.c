#include "fixq.h"
# include "debug.h"

net_err_t fixq_init(fixq_t * q, void **buf, int size, nlocker_type_t type) {
	q->size = size;
	q->in = q->out = q->cnt = 0;
	q->buf = buf;
	q->send_sem = SYS_SEM_INVALID;
	q->recv_sem = SYS_SEM_INVALID;

	net_err_t err = nlocker_init(&q->locker, type);
	if (err < 0) {
		dbg_error(DBG_QUEUE, "init locker failed");
		return err;
	}

	q->send_sem = sys_sem_create(size);
	if (q->send_sem == SYS_SEM_INVALID) {
		dbg_error(DBG_QUEUE, "create sem failed");
		err = NET_ERR_SYS;
		goto init_failed;
	}

	q->recv_sem = sys_sem_create(0);
	if (q->recv_sem == SYS_SEM_INVALID) {
		dbg_error(DBG_QUEUE, "create sem failed");
		err = NET_ERR_SYS;
		goto init_failed;
	}

	return NET_ERR_OK;

init_failed:
	if (q->send_sem != SYS_SEM_INVALID) {
		sys_sem_free(q->send_sem);
	}
	if (q->recv_sem != SYS_SEM_INVALID) {
		sys_sem_free(q->recv_sem);
	}
	nlocker_destroy(&q->locker);
	return err;
}
/**
 * @brief 向消息队列中发送一个消息
 * 
 * @param q 消息队列
 * @param msg 消息指针
 * @param tmo 超时时间
 * @return net_err_t 
 */
net_err_t fixq_send(fixq_t * q, void * msg, int tmo) {
	nlocker_lock(&q->locker);
	if ((tmo < 0) && (q->cnt >= q->size)) {
		nlocker_unlock(&q->locker);
		return NET_ERR_FULL;
	}
	nlocker_unlock(&q->locker);

	// 两种情况：
	// 1、如果消息队列中的消息数量小于消息队列的大小，那么就直接将消息放入消息队列中
	// 2、如果消息队列中的消息数量等于消息队列的大小，但是超时时间设置为大于等于0，那么就等待信号量，直到有空闲的位置
	if (sys_sem_wait(q->send_sem, tmo) < 0) {
		// 等待一段时间之后还没有等到空闲位置
		// 返回超时错误
		return NET_ERR_TMO;
	}
	nlocker_lock(&q->locker);
	q->buf[q->in++] = msg;
	if (q->in >= q->size) {
		q->in = 0;
	}
	q->cnt++;
	nlocker_unlock(&q->locker);

	// 通知其它进程有消息可用
	sys_sem_notify(q->recv_sem);

	return NET_ERR_OK;
}

/**
 * @brief 从消息队列中取一个消息， 如果没有，等待
 * 
 * @param q 
 * @param tmo 
 * @return void* 
 */
void * fixq_recv(fixq_t * q, int tmo) {
	nlocker_lock(&q->locker);
	// 如果此时消息队列中没有消息，而且tmo小于0，那么不需要等
	if ((!q->cnt) && (tmo < 0)) {
		nlocker_unlock(&q->locker);
		return (void *)0;
	}
	nlocker_unlock(&q->locker);

	if (sys_sem_wait(q->recv_sem, tmo) < 0) {
		return (void *)0;
	}

	nlocker_lock(&q->locker);
	void * msg = q->buf[q->out++];
	if (q->out >= q->size) {
		q->out = 0;
	}
	q->cnt--;
	nlocker_unlock(&q->locker);

	sys_sem_notify(q->send_sem);

	return msg;
}

void fixq_destory(fixq_t * q) {
	nlocker_destroy(&q->locker);
	sys_sem_free(&q->send_sem);
	sys_sem_free(&q->recv_sem);
}
int fixq_count(fixq_t * q) {
	nlocker_lock(&q->locker);
	int cnt = q->cnt;
	nlocker_unlock(&q->locker);

	return cnt;
}