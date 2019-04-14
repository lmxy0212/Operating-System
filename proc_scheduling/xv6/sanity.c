#include "types.h"
#include "user.h"

int i, j, wpid;
int proc[20];
int wtime, rtime, iotime, status, sumw, sumr, sumta;

int main(){
    for(i = 0; i < 20; ++i){
        proc[i] = fork();
        if(proc[i] == -1){
            printf(1, "unable to fork(), exiting...");
            exit();
        }
        if(proc[i] > 0) continue;

        for(j = 0; j < 9999999; j++){}//do some time-consuming computation
        exit();
    }
    while((wpid = wait_stat(&wtime, &rtime, &iotime, &status)) != -1){
        for(i = 0; i < 20; ++i){
            if(status == proc[i]){
                printf(1, "Child process %d with pid %d:\n", i, proc[i]);
                printf(1, "waiting time: %d, running time: %d,turnaround time: %d\n", wtime, rtime, wtime + rtime+ iotime);
                break;
            }
        }
        sumw += wtime;
        sumr += rtime;
        sumta += (wtime + rtime + iotime);
    }
    printf(1, "On Average:\n");
    printf(1, "waiting time: %d, running time: %d, turnaround time: %d\n", sumw/20, sumr/20, sumta/20);
    exit();
}
