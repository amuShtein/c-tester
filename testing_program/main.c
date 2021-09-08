#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/wait.h>
#define BUF 10000

void sleepMs(uint32_t ms) {
    struct timespec ts;
    ts.tv_sec = 0 + (ms / 1000);
    ts.tv_nsec = 1000 * 1000 * (ms % 1000);
    nanosleep(&ts, NULL);
}

int main() {
    int a, b;
    scanf("%d %d", &a, &b);

    printf("%d\n", a+b);
}
