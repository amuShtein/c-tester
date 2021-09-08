#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/wait.h>
#include <limits.h>

#define BUF 10000

void sleepMs(uint32_t ms) {
    struct timespec ts;
    ts.tv_sec = 0 + (ms / 1000);
    ts.tv_nsec = 1000 * 1000 * (ms % 1000);
    nanosleep(&ts, NULL);
}

char *name = "testing";
char *full_name_str = NULL;

char* full_name() {
    if(full_name_str == NULL) {
        char* cwd = malloc(PATH_MAX);

        if (!getcwd(cwd, PATH_MAX)) {
            printf("cannot get cwd");
            return NULL;
        }

        strcat(cwd, "/");
        strcat(cwd, name);
        printf("program: %s\n", cwd);
        full_name_str = cwd;
    }

    return full_name_str;
}


int test(char* in, char* out) {
    int status = 1;

    int pipe_in[2], pipe_out[2];
    pipe(pipe_in);
    pipe(pipe_out);

    char *cmd[] = {full_name(), NULL};
    pid_t pid;

    if ((pid = fork()) == 0) { /* child */
        dup2(pipe_in[0], 0); // set stdin of the process to write end of the pipe
        dup2(pipe_out[1], 1); // set stdout of the process to write end of the pipe

        write(pipe_in[1], in, BUF);
        execvp(cmd[0], cmd); // execute the program.

        fflush(stdout);
        perror(cmd[0]); // only reached in case of error
        exit(0);
    } else if (pid == -1) { /* failed */
        perror("fork");
        exit(1);
    } else { /* parent */
        waitpid(pid, &status, 0);
        int n;
        char buf[BUF];

        if ((n = read(pipe_out[0], buf, BUF)) >= 0) { // Try to read stdout of the child process from the read end of the pipe
            buf[n] = 0; /* terminate the string */
//            printf("Got: [%s]\n", buf);
        } else {
            fprintf(stderr, "read failed\n");
            perror("read");
        }

        int res = strcmp(buf, out);
        if(res == 0) {
//            printf("test passed!\n");
            return 1;
        } else {
//            printf("test failed!\n");
            return 0;
        }
    }

    return status;
}


int main() {
    char *inp[] = {
            "1 2",
            "0 5",
            "10 -5",
            "10000 10000",
            "-1 -1",
            "0 0"
    };

    char *out[] = {
            "3\n",
            "5\n",
            "5\n",
            "20001\n",
            "-2\n",
            "0\n"
    };

    int size = sizeof(inp)/sizeof(inp[0]);
    printf("size = %d\n", size);

    for(int i = 0; i < size; i++) {
        int status = test(inp[i], out[i]);
        printf("test[%d]: %s\n", i, status? "passed" : "failed");
    }

    return 0;
    int pipe_fds[2];
    int pipe_in_fds[2];
    int n;
    char buf[BUF] = {0};
    int status;
    pid_t pid;

    pipe(pipe_fds);
    pipe(pipe_in_fds);

    char *cmd[] = {"/home/xnpst/CLionProjects/cmc_prac/a", NULL};

    if ((pid = fork()) == 0) { /* child */
        dup2(pipe_fds[1], 1); // set stdout of the process to the write end of the pipe
        dup2(pipe_in_fds[0], 0); // set stdin of the process to the write end of the pipe

        write(pipe_in_fds[1], "he11llo_3wor22ld\n", BUF);
        execvp(cmd[0], cmd); // execute the program.
        fprintf(stderr, "print1");
        fprintf(stderr, "print2");
        fflush(stdout);
        perror(cmd[0]); // only reached in case of error
        exit(0);
    } else if (pid == -1) { /* failed */
        perror("fork");
        exit(1);
    } else { /* parent */

        while (1) {
            sleepMs(500); // Wait a bit to let the child program run a little
            waitpid(pid, &status, 0);
            fprintf(stderr, "child exit status = %d\n", status);

            printf("Trying to read\n");
            if ((n = read(pipe_fds[0], buf, BUF)) >= 0) { // Try to read stdout of the child process from the read end of the pipe
                buf[n] = 0; /* terminate the string */
                fprintf(stderr, "Got: %s", buf); // this should print "1 2 3 4 5 6 7 8 9 10 ..."
            } else {
                fprintf(stderr, "read failed\n");
                perror("read");
            }
        }
    }
}