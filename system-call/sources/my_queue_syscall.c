#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/linkage.h>

int queue[100];
int hd = 0; //index of queue's head(front)
int tl = 0; //index of queue's tail

SYSCALL_DEFINE1(jiyeon_enqueue, int, a){
    bool check = true;
    int i;

    for (i = hd; i < tl; i++) {
        if (queue[i] == a) {
            check = false; //a already exits
            break;
        }
    }

    if (check) queue[tl++] = a;
    //enqueue(push) a and add one to tl (move tail index to the right
    
    //print result using printk
    printk("Queue Front---------------\n");
    for (i = hd; i < tl; i++) printk("%d\n", queue[i]);
    printk("Queue Rear---------------\n");

}

SYSCALL_DEFINE0(jiyeon_dequeue){
    int head = -1;
    int i;

    if (hd != tl) head = queue[hd++]; //pop queue's head value

    //print result using printk
    printk("Queue Front---------------\n");
    for (i = hd; i < tl; i++) printk("%d\n", queue[i]);
    printk("Queue Rear---------------\n");

    return head;
}

