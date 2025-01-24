#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void calc(int start, int end, int* result) {
    int sum = 0;
    for (int i = start; i <= end; i++) {
        sum += i;
    }
    *result = sum;
}

int main() {
    int n, m;
    scanf("%d", &n);
    scanf("%d", &m);

    int segment = n / m;
    int remainder = n % m;

    int shm_id = shmget(IPC_PRIVATE, m * sizeof(int), 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int* results = (int*) shmat(shm_id, NULL, 0);
    if (results == (int*) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < m; i++) {
        int start = i * segment + 1;
        int end = (i + 1) * segment;
        if (i == m - 1) {
            end += remainder;
        }

        pid_t pid = fork();
        if (pid == 0) {
            calc(start, end, &results[i]);
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

    int total = 0;
    for (int i = 0; i < m; i++) {
        total += results[i];
    }

    printf("%d\n", total);


    return 0;
}