#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>
#include <linux/slab.h>

#define IDLE 0
#define MAX 50

typedef struct job {
    pid_t pid;      //process ID
    int job;        //수행해야하는 jobTime
    int priority;   //priority
} process;

//준비 큐를 관리하기 위한 front, rear index
int front = 0;
int rear = 0;
process waiting_queue[MAX] = {{0,0,0}, };

pid_t now_pid = IDLE;   //현재 CPU를 사용하고 있는 process의 pid
process now_p;          //현재 CPU를 사용하고 있는 process 구조체
int timeslice = 30;     //RR 스케줄링 시에 사용되는 타임 퀀텀

void init_queue(void);
bool is_empty(void);
bool is_existed(pid_t new_pid);
process pop_queue(void);
void push_queue(process p);
void push_srtf_queue(process p);
void push_priority_queue(process p);

SYSCALL_DEFINE0(ku_cpu_init_queue) {
    init_queue();
    printk("ku_cpu_init_queue called\n");
    return 0;
}

//---------------------------FCFS---------------------------
SYSCALL_DEFINE2 (ku_cpu_fcfs, char *, name, int, jobTime) {
    process new_p = {current -> pid, jobTime, 0};

    if (now_pid == IDLE) now_pid = new_p.pid;
    
    if (now_pid == new_p.pid) {
        if (jobTime == 0) {
            printk("Process Finished: %s\n", name);
            if (is_empty()) now_pid = IDLE;
            else {
                process next_p = pop_queue();
                now_pid = next_p.pid;
            }
        }
        else {
            printk("Working: %s\n", name);
        }
        return 0; //CPU 할당 성공
    }
    else {
        if (!is_existed(new_p.pid)) {
            push_queue(new_p);
        }
        printk("Working Denied: %s\n", name);
        return 1; //CPU 할당 실패
    }
}

//---------------------------SRTF---------------------------
SYSCALL_DEFINE2(ku_cpu_srtf, char *, name, int, jobTime) {
    process new_p = {current -> pid, jobTime, 0};

    if (now_pid == IDLE) {
        now_p = new_p;
        now_pid = new_p.pid;
    }
    
    if (now_pid == new_p.pid) {
        if (jobTime == 0) {
            printk("Process Finished: %s\n", name);
            if (is_empty()) {
                now_pid = IDLE;
            }
            else {
                process next_p = pop_queue();
                now_p = next_p;
                now_pid = next_p.pid;
            }
        }
        else {
            printk("Working: %s\n", name);
        }
        return 0; //CPU 할당 성공
    }
    else {
        if (!is_existed(new_p.pid)) {
            //새로 들어오는 프로세스의 jobTime이 더 짧은 경우
            if (new_p.job < now_p.job) {
                push_srtf_queue(now_p); //준비 큐에 수행되던 작업 push
                now_p = new_p;
                now_pid = new_p.pid;
                printk("Working: %s\n", name);
                return 0; //CPU 할당 성공
            }
            else {
                push_srtf_queue(now_p);
            }
        }
        printk("Working Denied: %s\n", name);
        return 1; //CPU 할당 실패
    }
}

//---------------------------RR---------------------------
SYSCALL_DEFINE2(ku_cpu_rr, char *, name, int, jobTime) {
    process new_p = {current -> pid, jobTime, 0};

    if (now_pid == IDLE) {
        now_p = new_p;
        now_pid = new_p.pid;
        timeslice = 20;
    }
    
    if (now_pid == new_p.pid) {
        if (jobTime == 0) {
            printk("Process Finished: %s\n", name);
            if (is_empty()) {
                now_pid = IDLE;
            }
            else {
                process next_p = pop_queue();
                now_p = next_p;
                now_pid = next_p.pid;
            }
            timeslice = 20; //타임퀀텀 초기화
            return 0; 
        }
        else if (timeslice == 0) {
            printk("Turn Over ----> %s\n", name);
            push_queue(new_p);
            now_p = pop_queue(); //준비큐에서 새로운 프로세스 pop
            now_pid = now_p.pid;
            timeslice = 20;
            return 1;
        }
        else {
            printk("Working: %s\n", name);
            timeslice--;
            return 0;
        }
    }
    else {
        if (!is_existed(new_p.pid)) push_queue(new_p)
       printk("Working Denied: %s\n", name);
        return 1; //CPU 할당 실패
    }
}

//---------------------------Priority---------------------------
SYSCALL_DEFINE3(ku_cpu_priority, char *, name, int, jobTime, int, priority) {
    process new_p = {current -> pid, jobTime, priority};

    if (now_pid == IDLE) {
        now_p = new_p;
        now_pid = new_p.pid;
    }
    
    if (now_pid == new_p.pid) {
        if (jobTime == 0) {
            printk("Process Finished: %s\n", name);
            if (is_empty()) {
                now_pid = IDLE;
            }
            else {
                process next_p = pop_queue();
                now_p = next_p;
                now_pid = next_p.pid;
            }
        }
        else {
            printk("Working: %s\n", name);
        }
        return 0; //CPU 할당 성공
    }
    else {
        if (!is_existed(new_p.pid)) {
            //새로 들어오는 프로세스의 jobTime이 더 짧은 경우
            if (new_p.priority < now_p.priority) {
                push_priority_queue(now_p); //준비 큐에 수행되던 작업 push
                now_p = new_p;
                now_pid = new_p.pid;
                printk("Working: %s\n", name);
                return 0; //CPU 할당 성공
            }
            else {
                push_priority_queue(new_p);
            }
        }
        printk("Working Denied: %s\n", name);
        return 1; //CPU 할당 실패
    }
}

void init_queue(void) {
    front = 0;
    rear = 0;
}

bool is_empty(void) {
    if (front == rear) return true;
    else return false;
}

//waiting queue에 추가하려는 프로세스가 이미 존재하는지 확인하는 함수
//존재하면 true, 존재하지 않으면 false return
bool is_existed(pid_t new_pid) {
    int i;
    for (i = front; i != rear; i = (i+1) % MAX) {
        if (waiting_queue[i].pid == new_pid) return true;
    }
    return false;
}   

process pop_queue(void) {
    if (front == rear) {
        process empty = {IDLE, 0, 0};
        return empty;
    }
    process p;
    p = waiting_queue[front];
    front = (front + 1) % MAX;
    return p;
}

void push_queue(process p) {
    waiting_queue[rear] = p;
    rear = (rear + 1) % MAX;
}
void push_srtf_queue(process p) {
    int i;
    waiting_queue[rear] = p;
    rear = (rear + 1) % MAX;

    //준비 큐를 오름차순 정렬
    for (i = rear - 1; i != front; i = (i - 1) % MAX) {
        if (waiting_queue[i].job < waiting_queue[(i-1) % MAX].job) {
            process temp = waiting_queue[i];
            waiting_queue[i] = waiting_queue[(i-1) % MAX];
            waiting_queue[(i-1) % MAX] = temp;
        }
    }
}

void push_priority_queue(process p) {
    int i;
    waiting_queue[rear] = p;
    rear = (rear + 1) % MAX;

    //준비 큐를 오름차순 정렬
    for (i = rear - 1; i != front; i = (i - 1) % MAX) {
        if (waiting_queue[i].priority < waiting_queue[(i-1) % MAX].priority) {
            process temp = waiting_queue[i];
            waiting_queue[i] = waiting_queue[(i-1) % MAX];
            waiting_queue[(i-1) % MAX] = temp;
        }
    }
}



