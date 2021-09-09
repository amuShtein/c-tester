#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

#define BUF 10000

#define CREATE_TESTFILE_OPT "-c"
#define MAX_TESTFILE_NAME_LEN 16
#define DEFAULT_TESTFILE_NAME "tf"

#define CMD_LEN 10
#define INPUT_LEN 1024
#define OUTPUT_LEN 1024
#define NAME_LEN 64
#define TEST_NAME_LEN 64
#define NAME_TF "#name"
#define TEST_TF "#test"
#define INPUT_TF "#input"
#define OUTPUT_TF "#output"
#define TEST_END_TF "#end"

enum {
    NAME_TF_CMD,
    TEST_TF_CMD,
    INPUT_TF_CMD,
    OUTPUT_TF_CMD,
    TEST_END_TF_CMD,
};

enum {
    FIRST_FAILED,
    ALL_TESTS,
    SHOW_PASSED,
    PRINT_FAILED,
    PRINT_PASSED,
};

enum {
    EXISTING_TESTFILE,
    CREATE_TESTFILE,
};

char *default_testfile =
"#name a\n\n"
"#test test1\n"
"#input\n"
"100 204\n"
"#output\n"
"304\n"
"#end\n\n"
"#test test2\n"
"#input\n"
"100 -200\n"
"#output\n"
"-100\n"
"#end\0";

enum {
    PASSED,
    FAILED
};

struct test {
    char *name, *input, *output;
    int status;
};