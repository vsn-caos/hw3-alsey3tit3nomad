#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Программе на стандартный поток ввода задается арифметическое выражение
// в синтаксисе языка python3. Необходимо вычислить это выражение и вывести результат.
// Использовать дополнительные процессы запрещено — нужно использовать exec.

int main(void) {
    char expr[65536];
    size_t total = 0;

    ssize_t r;
    while (total < sizeof(expr) - 1 &&
           (r = read(STDIN_FILENO, expr + total, sizeof(expr) - 1 - total)) > 0) {
        total += (size_t)r;
    }
    expr[total] = '\0';

    while (total > 0 &&
           (expr[total - 1] == '\n' || expr[total - 1] == '\r' ||
            expr[total - 1] == ' '  || expr[total - 1] == '\t')) {
        expr[--total] = '\0';
    }

    char code[65600];
    snprintf(code, sizeof(code), "print(%s)", expr);

    execlp("python3", "python3", "-c", code, (char *)NULL);

    perror("execlp");
    return 1;
}
