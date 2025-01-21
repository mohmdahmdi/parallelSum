#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    int n, m;
    scanf("%d", &n);
    scanf("%d", &m);

    int range = n / m;
    int remainder = n % m;

    int shmid = shmget(IPC_PRIVATE, m * sizeof(int), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit();
    }

    int* results = (int*) shmat(shmid, NULL, 0);
    if (results == (int*) -1) {
        perror("shmat");
        exit();
    }

    for (int i = 0; i < m; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            int start = i * range + 1;
            int end = (i + 1) * range;
            if (i == m - 1) {
                end += remainder;
            }

            int sum = 0;
            for (int j = start; j <= end; j++) {
                sum += j;
            }

            results[i] = sum;
            shmdt(results);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < m; i++) {
        wait(NULL);
    }

    int totalSum = 0;
    for (int i = 0; i < m; i++) {
        totalSum += results[i];
    }

    printf("%d\n", totalSum);

    shmdt(results);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}