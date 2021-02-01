#ifndef _EXEC_H_
#define _EXEC_H_

#include <string.h>
#include "parse.h"

#define BUILTIN_CNT 3
#define DEBUGGING

static const char builtin_commands[BUILTIN_CNT][10] = {
                                            "cd",
                                            "exit",
                                            "help"
                                        };

int is_builtin(command *);
int execute_cd(command *);
int execute_exit(command *);
int execute_help(command *);
int execute_builtin(command *, int);
int execute_nonbuiltin(command *, int);
int execute(command_seq *);

#endif


