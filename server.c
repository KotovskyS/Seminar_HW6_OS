#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

#define SHMSZ 1024

int shmid;
char *shm;

void cleanup(int sig)
{
    printf("Server terminated by signal %d\n", sig);

    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(1);
    }

    printf("Shared memory segment removed.\n");

    exit(0);
}

int main()
{
    key_t key;
    int *data;
    int num;

    signal(SIGINT, cleanup);

    key = ftok(".", 'x');
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    data = (int *) shm;

    while (1) {
        printf("Waiting for data...\n");
        while (*data == 0)
            sleep(1);

        printf("Received data: %d\n", *data);
        *data = 0;
    }

    return 0;
}

