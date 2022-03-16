
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS *p = p_proc_ready;
    //先将该进程从原来的队列中出队
		if(now_process_timep == queue_1.timep){
			//该进程处于队列1
			queue_1.front = (queue_1.front + 1) % NR_TASKS;//出队
			queue_1.len--;
			if(p->state == ready){
				queue_2.queue[queue_2.rear] = p;
				queue_2.rear = (queue_2.rear + 1) % NR_TASKS;
				queue_2.len++;
			}
		}
		if(now_process_timep == queue_2.timep){
			//该进程处于队列2
			queue_2.front = (queue_2.front + 1) % NR_TASKS;//出队
			queue_2.len--;
			if(p->state == ready){
				queue_4.queue[queue_4.rear] = p;
				queue_4.rear = (queue_4.rear + 1) % NR_TASKS;
				queue_4.len++;
			}
		}
		if(now_process_timep == queue_4.timep){
			//该进程处于队列3
			queue_4.front = (queue_4.front + 1) % NR_TASKS;//出队
			queue_4.len--;
			if(p->state == ready){
				queue_4.queue[queue_4.rear] = p;
				queue_4.rear = (queue_4.rear + 1) % NR_TASKS;
				queue_4.len++;
			}
		}
		
		int flag = 0;
		if(queue_1.len > 0) {//接下来从一级队列开始挑选处在队列头部的进程
			p = queue_1.queue[queue_1.front];
            p->state = running;
			now_process_timep = queue_1.timep;
			now_process_runtime = 0;
			flag = 1;
		}else if(queue_2.len > 0) {//在第一队列未找到，找第二队列
			p = queue_2.queue[queue_2.front];
            p->state = running;
			now_process_timep = queue_2.timep;
			now_process_runtime = 0;
			flag = 1;
		}else if(queue_4.len > 0){//在第一队列和第二队列都未找到，在第三队列开始找
			p = queue_4.queue[queue_4.front];
            p->state = running;
			now_process_timep = queue_4.timep;
			now_process_runtime = 0;
			flag = 1;
		}
		if(flag == 0){
			p = &proc_table[0];
			p->state = super;
			now_process_timep = -1;
			now_process_runtime = -1;
		}
		p_proc_ready = p;
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

