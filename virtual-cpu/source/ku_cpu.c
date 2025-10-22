
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ku_cpu_fcfs         337
#define ku_cpu_srtf         338
#define ku_cpu_rr           339
#define ku_cpu_priority     340
#define ku_cpu_init_queue   341

int main(int argc, char ** argv) {
    int jobTime;        //the execution duration
    int delayTime;      //the execution delay
    char name[4];       //process name
    int wait = 0;       //
    int priority;
    int response_time = -1; 

    if(argc < 4) {
        printf("\nInsufficient Arguments...\n");
        return 1;
    }

    /*
    first argument: jobTime CPU 사용 시간
    second argument: delay time before execution CPU를 할당받기 위해 대기하는 시간
    third argument: process name 프로세스명
    fourth argument: priority 우선순위
    */
    
    // 큐 초기화 과정
    if (syscall(ku_cpu_init_queue) != 0) {
        printf("Failed to initializing queue.\n");
        return 1;
    }

    jobTime = atoi(argv[1]); //char to int
    delayTime = atoi(argv[2]);
    strcpy(name, argv[3]);
    //priority = atoi(argv[4]);

    sleep(delayTime);
    printf("\nProcess %s : I will use CPU by %ds. \n", name, jobTime);
    jobTime *= 10;

    while (jobTime) {
        if (!syscall(337, name, jobTime)) {
            jobTime--;
           if (response_time == -1) {
                response_time = (wait + 5) / 10;
           }
        } else {
           wait++;
        }

        usleep(100000);
    }

    syscall(337, name, jobTime);
    //syscall(340, name, jobTime, priority);
    printf("\nProcess %s: Finish! My response time is %ds and My total wait time is %ds. ", name, response_time, (wait + 5) / 10);
    return 0;
    
}

