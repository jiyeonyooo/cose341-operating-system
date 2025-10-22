#include <unistd.h>
#include <stdio.h>

#define jiyeon_enqueue_syscall 335
#define jiyeon_dequeue_syscall 336

int main(){
    int r;
    
    r = syscall(jiyeon_enqueue_syscall, 1);
    printf("Enqueue: %d\n", 1);
    r = syscall(jiyeon_enqueue_syscall, 2);
    printf("Enqueue : %d\n", 2);
    r = syscall(jiyeon_enqueue_syscall, 3);
    printf("Enqueue : %d\n", 3);
    r = syscall(jiyeon_enqueue_syscall, 3);
    printf("Enqueue : %d\n", 3);

    r = syscall(jiyeon_dequeue_syscall);
    printf("Dequeue : %d\n", r);
    r = syscall(jiyeon_dequeue_syscall);
    printf("Dequeue : %d\n", r);
    r = syscall(jiyeon_dequeue_syscall);
    printf("Dequeue : %d\n", r);

    return 0;
}
