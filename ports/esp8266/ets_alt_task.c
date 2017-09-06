#include <stdio.h>
#include "osapi.h"
#include "os_type.h"
#include "ets_sys.h"
#include <esp_sdk_ver.h>
#include "etshal.h"
#include "user_interface.h"
#include "ets_alt_task.h"

// Use standard ets_task or alternative impl
#define USE_ETS_TASK 0

#define MP_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

struct task_entry {
    os_event_t *queue;
    os_task_t task;
    uint8_t qlen;
    uint8_t prio;
    int8_t i_get;
    int8_t i_put;
};

static void (*idle_cb)(void *);
static void *idle_arg;

#if ESP_SDK_VERSION >= 010500
# define FIRST_PRIO 0
#else
# define FIRST_PRIO 0x14
#endif
#define LAST_PRIO 0x20
#define PRIO2ID(prio) ((prio) - FIRST_PRIO)

volatile struct task_entry emu_tasks[PRIO2ID(LAST_PRIO) + 1];

static inline int prio2id(uint8_t prio) {
    int id = PRIO2ID(prio);
    if (id < 0 || id >= MP_ARRAY_SIZE(emu_tasks)) {
        printf("task prio out of range: %d\n", prio);
        while (1);
    }
    return id;
}

#if DEBUG
void dump_task(int prio, volatile struct task_entry *t) {
    printf("q for task %d: queue: %p, get ptr: %d, put ptr: %d, qlen: %d\n",
        prio, t->queue, t->i_get, t->i_put, t->qlen);
}

void dump_tasks(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(emu_tasks); i++) {
        if (emu_tasks[i].qlen) {
            dump_task(i + FIRST_PRIO, &emu_tasks[i]);
        }
    }
    printf("====\n");
}
#endif

bool ets_task(os_task_t task, uint8 prio, os_event_t *queue, uint8 qlen) {
    static unsigned cnt;
    printf("#%d ets_task(%p, %d, %p, %d)\n", cnt++, task, prio, queue, qlen);
#if USE_ETS_TASK
    return _ets_task(task, prio, queue, qlen);
#else
    int id = prio2id(prio);
    emu_tasks[id].task = task;
    emu_tasks[id].queue = queue;
    emu_tasks[id].qlen = qlen;
    emu_tasks[id].i_get = 0;
    emu_tasks[id].i_put = 0;
    return true;
#endif
}

bool ets_post(uint8 prio, os_signal_t sig, os_param_t param) {
//    static unsigned cnt; printf("#%d ets_post(%d, %x, %x)\n", cnt++, prio, sig, param);
#if USE_ETS_TASK
    return _ets_post(prio, sig, param);
#else
    ets_intr_lock();

    const int id = prio2id(prio);
    os_event_t *q = emu_tasks[id].queue;
    if (emu_tasks[id].i_put == -1) {
        // queue is full
        printf("ets_post: task %d queue full\n", prio);
        return 1;
    }
    q = &q[emu_tasks[id].i_put++];
    q->sig = sig;
    q->par = param;
    if (emu_tasks[id].i_put == emu_tasks[id].qlen) {
        emu_tasks[id].i_put = 0;
    }
    if (emu_tasks[id].i_put == emu_tasks[id].i_get) {
        // queue got full
        emu_tasks[id].i_put = -1;
    }
    //printf("after ets_post: "); dump_task(prio, &emu_tasks[id]);
    //dump_tasks();

    ets_intr_unlock();

    return 0;
#endif
}

int ets_loop_iter_disable = 0;

// to implement a 64-bit wide microsecond counter
static uint32_t system_time_prev = 0;
uint32_t system_time_high_word = 0;

bool ets_loop_iter(void) {
    if (ets_loop_iter_disable) {
        return false;
    }

    // handle overflow of system microsecond counter
    ets_intr_lock();
    uint32_t system_time_cur = system_get_time();
    if (system_time_cur < system_time_prev) {
        system_time_high_word += 1; // record overflow of low 32-bits
    }
    system_time_prev = system_time_cur;
    ets_intr_unlock();

    //static unsigned cnt;
    bool progress = false;
    for (volatile struct task_entry *t = emu_tasks; t < &emu_tasks[MP_ARRAY_SIZE(emu_tasks)]; t++) {
        system_soft_wdt_feed();
        ets_intr_lock();
        //printf("etc_loop_iter: "); dump_task(t - emu_tasks + FIRST_PRIO, t);
        if (t->i_get != t->i_put) {
            progress = true;
            //printf("#%d Calling task %d(%p) (%x, %x)\n", cnt++,
            //    t - emu_tasks + FIRST_PRIO, t->task, t->queue[t->i_get].sig, t->queue[t->i_get].par);
            int idx = t->i_get;
            if (t->i_put == -1) {
                t->i_put = t->i_get;
            }
            if (++t->i_get == t->qlen) {
                t->i_get = 0;
            }
            //ets_intr_unlock();
            t->task(&t->queue[idx]);
            //ets_intr_lock();
            //printf("Done calling task %d\n", t - emu_tasks + FIRST_PRIO);
        }
        ets_intr_unlock();
    }
    return progress;
}

#if SDK_BELOW_1_1_1
void my_timer_isr(void *arg) {
//    uart0_write_char('+');
    ets_post(0x1f, 0, 0);
}

// Timer init func is in ROM, and calls ets_task by relative addr directly in ROM
// so, we have to re-init task using our handler
void ets_timer_init() {
    printf("ets_timer_init\n");
//    _ets_timer_init();
    ets_isr_attach(10, my_timer_isr, NULL);
    SET_PERI_REG_MASK(0x3FF00004, 4);
    ETS_INTR_ENABLE(10);
    ets_task((os_task_t)0x40002E3C, 0x1f, (os_event_t*)0x3FFFDDC0, 4);

    WRITE_PERI_REG(PERIPHS_TIMER_BASEDDR + 0x30, 0);
    WRITE_PERI_REG(PERIPHS_TIMER_BASEDDR + 0x28, 0x88);
    WRITE_PERI_REG(PERIPHS_TIMER_BASEDDR + 0x30, 0);
    printf("Installed timer ISR\n");
}
#endif

bool ets_run(void) {
#if USE_ETS_TASK
    #if SDK_BELOW_1_1_1
    ets_isr_attach(10, my_timer_isr, NULL);
    #endif
    _ets_run();
#else
//    ets_timer_init();
    *(char*)0x3FFFC6FC = 0;
    ets_intr_lock();
    printf("ets_alt_task: ets_run\n");
#if DEBUG
    dump_tasks();
#endif
    ets_intr_unlock();
    while (1) {
        if (!ets_loop_iter()) {
            //printf("idle\n");
            ets_intr_lock();
            if (idle_cb) {
                idle_cb(idle_arg);
            }
            asm("waiti 0");
            ets_intr_unlock();
        }
    }
#endif
}

void ets_set_idle_cb(void (*handler)(void *), void *arg) {
    //printf("ets_set_idle_cb(%p, %p)\n", handler, arg);
    idle_cb = handler;
    idle_arg = arg;
}
