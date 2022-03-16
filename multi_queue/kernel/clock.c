
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               clock.c
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
                           clock_handler
 *======================================================================*/
PUBLIC void clock_handler(int irq)
{
	if (k_reenter != 0) {
		return;
	}

	PROCESS* p = p_proc_ready;//p指向当前运行的进程
	int need_schedule = 0;//0表示本次中断不需要调度，1表示需要调度
	//由用户态切换为内核态时的进程只能为running和super
	if(p->state == super){
		need_schedule = 1;
	}else if(p->state == running){
		p->ticks--;//该进程的生命周期减1
		now_process_runtime++;
		if(p->ticks == 0){
			//生命周期用完，running->abort
			p->state = abort;
			need_schedule = 1;
		}else if(now_process_runtime == now_process_timep){
				//时间片用完，回到就绪态,running->ready
				p->state = ready;//running->ready
				need_schedule = 1;
		}
	}

	int i;
	for(i = 1; i < NR_TASKS; i++){
		//遍历进程表，如果已经到ticks的值已经大于了进程计划的到达的时间，就可以将进程置为到达态
		//这样做是为了尽量让进程到达的先后顺序为TestA，TestB，TestC,TestD，TestE
		if(ticks >= task_table[i].arrive_time && proc_table[i].state == unarrive){
			proc_table[i].state = arrive;//unarrive->arrive
		}
	}
	
	for(i = 1; i < NR_TASKS; i++){
		if(resource[i] == 1){//这里将所有的resource都手动置为1，所以进程不会切换为block态
			if(proc_table[i].state == arrive){
				//该进程处于到达态,切换为就绪态arrive->ready
				proc_table[i].state = ready;
				//将该进程加入队列1
				queue_1.queue[queue_1.rear] = &proc_table[i];
				queue_1.rear = (queue_1.rear + 1) % NR_TASKS;
				queue_1.len++;
			}
		}
	}

	if(need_schedule == 1){
		schedule();
	}
	ticks++;
	return;

}

/*======================================================================*
                              milli_delay
 *======================================================================*/
PUBLIC void milli_delay(int milli_sec)
{
        int t = get_ticks();

        while(((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}

