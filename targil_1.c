#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
 
#define NUM_PROCESSES 5
 
typedef struct {
    sem_t semaphore;
    int shared_sum;
} SharedData;
 
int main() 
{
    int shmid;
    SharedData *data;
    shmid = shmget(IPC_PRIVATE, sizeof(SharedData), 0600 | IPC_CREAT);
    if (shmid == -1) 
    {
        printf("shmget failed");
        exit(1);
    }
    data = (SharedData *)shmat(shmid, NULL, 0); 
    if (data == NULL)
    {
        printf("shmat failed");
        exit(1);
    }
    data->shared_sum = 0;
    if (sem_init(&data->semaphore, 1, 1) != 0)
    {
        printf("sem_init failed");
        exit(1);
    }
    for (int i = 1; i <= NUM_PROCESSES; i++)
    {
        if (fork() == 0)
        {
            sem_wait(&data->semaphore);
            int prevSum = data->shared_sum;
            data->shared_sum = prevSum + i;
            printf("Process %d: adding %d to %d\n", getpid(), i, prevSum);
            sem_post(&data->semaphore);
            exit(0);
        }
    }
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        wait(NULL);
    }
    printf("Final result: %d\n", data->shared_sum);
    sem_destroy(&data->semaphore);
    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    
    return 0;
}
