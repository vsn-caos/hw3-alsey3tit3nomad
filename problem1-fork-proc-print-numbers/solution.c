#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Программе передается аргумент — целое число N > 0.
// Необходимо создать N-1 дополнительных процессов таким образом,
// чтобы у каждого процесса было не более одного дочернего процесса.
// Каждый из процессов должен вывести ровно одно число так,
// чтобы на выходе получилась строка: 1 2 3 4 ... N
// Между числами — ровно один пробел, строка завершается символом '\n'.

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        return 0;
    }

    int k = 1;
    while (1) {
        if (k == 1) {
            printf("%d", k);
        } else {
            printf(" %d", k);
        }
        if (k == n) {
            printf("\n");
        }
        fflush(stdout);

        if (k == n) {
            break;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            k++;
            continue;
        } else {
            wait(NULL);
            break;
        }
    }

    return 0;
}
