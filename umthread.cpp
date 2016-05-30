//
// Created by hackeris on 16-5-30.
//

#include <ucontext.h>
#include <memory.h>
#include <sys/time.h>
#include <cstdlib>
#include <assert.h>
#include "umthread.h"

constexpr int STACK_SIZE = 4096 * 2;

constexpr int STOPED = 0, RUNNING = 1, IDLE = 2;

constexpr int TID_IDLE = 0;

struct umthread_t {

    ucontext_t ctx;

    void(*pfun)(void *);

    void *param;

    char stack[STACK_SIZE];
    int status;
};

constexpr int number_of_threads = 100;
int curr = 0;
int previous = 0;

umthread_t threads[number_of_threads];

void idle_fun(void *) {
    assert(false);
}

void switch_to(int tid) {
    if (!(previous == tid && tid == 0)) {
        swapcontext(&threads[previous].ctx, &threads[tid].ctx);
    }
}

int find_runnable_thread() {

    int i;
    for (i = (curr + 1) % number_of_threads; i != curr;
         i = (i + 1) % number_of_threads) {
        if (threads[i].status == RUNNING) {
            return i;
        }
    }
    return 0;
}

int find_empty_thread() {

    int i;
    for (i = 0; i < number_of_threads; i++) {
        if (threads[i].status == STOPED) {
            return i;
        }
    }
    return -1;
}

void schedule() {

    int new_thread = find_runnable_thread();
    previous = curr;
    curr = new_thread;

    switch_to(curr);
}


void umthread_entry(umthread_t *thread) {

    thread->pfun(thread->param);
    thread->status = STOPED;
    schedule();
}

int umthread_create(void(*pfun)(void *), void *param) {

    int new_thread = find_empty_thread();
    if (new_thread < 0) {
        return -1;
    }
    getcontext(&threads[new_thread].ctx);
    threads[new_thread].ctx.uc_stack.ss_sp = threads[new_thread].stack;
    threads[new_thread].ctx.uc_stack.ss_size = sizeof(threads[new_thread].stack);
    threads[new_thread].ctx.uc_link = &threads[0].ctx;
    threads[new_thread].status = RUNNING;
    threads[new_thread].pfun = pfun;
    threads[new_thread].param = param;
    makecontext(&threads[new_thread].ctx, (void (*)()) umthread_entry, 1, &threads[new_thread]);

    return new_thread;
}

void timer_signal(int signo) {

    schedule();
}


void umthread_wait_all() {
    while (find_runnable_thread() != 0);
}

int init() {

    memset(&threads, 0, sizeof(threads));
    threads[TID_IDLE].ctx.uc_stack.ss_sp = threads[TID_IDLE].stack;
    threads[TID_IDLE].ctx.uc_stack.ss_size = sizeof(threads[TID_IDLE].stack);
    threads[TID_IDLE].ctx.uc_link = &threads[TID_IDLE].ctx;
    threads[TID_IDLE].status = IDLE;
    threads[TID_IDLE].pfun = idle_fun;
    threads[TID_IDLE].param = NULL;
    makecontext(&threads[TID_IDLE].ctx, (void (*)()) umthread_entry, 1, &threads[TID_IDLE]);

    signal(SIGALRM, timer_signal);
    struct itimerval ticks_timer;
    ticks_timer.it_interval.tv_sec = 0;
    ticks_timer.it_interval.tv_usec = 10000;
    ticks_timer.it_value.tv_sec = 0;
    ticks_timer.it_value.tv_usec = 10000;
    setitimer(ITIMER_REAL, &ticks_timer, NULL);

    atexit(umthread_wait_all);
    return 0;
}

int x = init();