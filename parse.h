#ifndef _PARSE_H_
#define _PARSE_H_

#define MAX_FILE_NAME 30
#define MAX_PIPE 10
#define MAX_CMD_LEN 50
#define MAX_FILE_LEN 10

#define MAX_VAR_NAME 20
#define MAX_VAR_CNT 10

/* #define DEBUGGING */

typedef struct _command {
    char *cmd;
    char *var[MAX_VAR_CNT];
    int var_cnt;
} command;

typedef struct _command_seq {
    int is_ifile;
    int is_ofile;
    int is_bg;
    char ifile[MAX_FILE_LEN];
    char ofile[MAX_FILE_LEN];
    int cmd_cnt;
    command commands[MAX_PIPE];
} command_seq;

command_seq *parse(char *);
void free_seq(command_seq *);
void print_command(command *);
void parse_command(char *, command *);

#endif


