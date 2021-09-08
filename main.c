#include "main.h"

char *testfile = NULL;
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
        full_name_str = cwd;
    }

    return full_name_str;
}

int get_arg(char* arg) {
    if(strcmp(arg, CREATE_TESTFILE_OPT) == 0) {
        return CREATE_TESTFILE;
    } else {
        return EXISTING_TESTFILE;
    }
}

char* create_testfile() {
    FILE *fd = fopen(DEFAULT_TESTFILE_NAME, "w");
    fputs(default_testfile, fd);
    fclose(fd);

    return DEFAULT_TESTFILE_NAME;
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

int get_cmd(const char *cmd) {
    int incorrect = 0;
    char *new_cmd = (char *) cmd;

    if(cmd[0] != '#') {
        new_cmd = malloc(CMD_LEN);
        new_cmd[0] = '#';
        new_cmd[1] = '\0';
        strcat(new_cmd, cmd);
        incorrect = 1;
    }

    int res;

    if(strcmp(new_cmd, NAME_TF) == 0) {
        res = NAME_TF_CMD;
    } else if(strcmp(new_cmd, TEST_TF) == 0) {
        res = TEST_TF_CMD;
    } else if(strcmp(new_cmd, INPUT_TF) == 0) {
        res = INPUT_TF_CMD;
    } else if(strcmp(new_cmd, OUTPUT_TF) == 0) {
        res = OUTPUT_TF_CMD;
    } else if(strcmp(new_cmd, TEST_END_TF) == 0) {
        res = TEST_END_TF_CMD;
    } else {
        res = -1;
    }

    if(incorrect) {
        free(new_cmd);
    }

    return res;
}

char* scan_data(FILE *fd) {
    int c;
    while ((c = fgetc(fd)) == ' ' || c == '\n');

    int j = 0;
    int buf_len = INPUT_LEN;
    char *buf = malloc(buf_len);

    do {
        if(j + 2 >= buf_len) {
            buf_len *= 2;
            buf = realloc(buf, buf_len);
        }

        buf[j] = c;
        j++;
    } while ((c = fgetc(fd)) != '#' && c > 0);

    buf[j] = 0;
    buf = realloc(buf, j+1);

    return buf;
}

struct test** parse_testfile() {
    FILE *fd = fopen(testfile, "r");
    if(fd == 0) {
        printf("cannot open testfile '%s'", testfile);
    }

    int i = 0;
    int array_buf = 32;
    struct test **tests = malloc(sizeof(struct test*) * array_buf);
    struct test *cur;

    char *cmd = malloc(CMD_LEN);
    char *buf;
    while (fscanf(fd, "%s", cmd) > 0) {
        switch (get_cmd(cmd)) {
            case NAME_TF_CMD:
                buf = malloc(NAME_LEN);
                fscanf(fd,"%s\n", buf);
                buf = realloc(buf, strlen(buf) + 1);
                if(name) {
                    free(name);
                }
                name = buf;
                break;
            case TEST_TF_CMD:
                cur = malloc(sizeof (struct test));

                buf = malloc(TEST_NAME_LEN);
                fscanf(fd,"%s\n", buf);
                buf = realloc(buf, strlen(buf) + 1);
                cur->name = buf;
                buf = NULL;
                break;
            case INPUT_TF_CMD:
                cur->input = scan_data(fd);
                break;
            case OUTPUT_TF_CMD:
                cur->output = scan_data(fd);
                break;
            case TEST_END_TF_CMD:
                if(i + 2 >= array_buf) {
                    array_buf *= 2;
                    tests = realloc(tests, sizeof(struct test*) * array_buf);
                }

                tests[i] = cur;
                i++;
        }
    }

    fclose(fd);

    tests[i] = 0;
    tests = realloc(tests, sizeof(struct test*) * (i+2));

    return tests;
}

int main(int argc, char *argv[]) {
    for(int i = 1; i < argc; i++) {
        switch (get_arg(argv[i])) {
            case CREATE_TESTFILE:
                testfile = malloc(MAX_TESTFILE_NAME_LEN);
                strcpy(testfile, create_testfile());
                testfile = realloc(testfile, strlen(testfile) + 1);
                break;
            case EXISTING_TESTFILE:
                testfile = malloc(MAX_TESTFILE_NAME_LEN);
                strcpy(testfile, argv[i]);
                testfile = realloc(testfile, strlen(testfile) + 1);
                break;
        }
    }

    struct test **tests = parse_testfile();

    for(int i = 0; tests[i] != 0; i++) {
        tests[i]->status = test(tests[i]->input, tests[i]->output) == 1 ? PASSED : FAILED;
        printf("test[%s]: %s\n", tests[i]->name, tests[i]->status == PASSED? "passed" : "failed");
    }

    return 0;
}