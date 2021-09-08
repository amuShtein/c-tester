#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

#define BUF 10000

char *name = NULL;
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
        } else {
            fprintf(stderr, "read failed\n");
            perror("read");
        }

        return (strcmp(buf, out) == 0);
    }
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
}