#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#include "exec.h"
#include "parse.h"

int is_builtin(command *cmd) {
    int i;
    for(i = 0; i < BUILTIN_CNT; i++) {
        if(!strcmp(cmd->cmd, builtin_commands[i]))
            return i;
    }
    return -1;
}

int execute_cd(command *cmd) {
   if(cmd->var_cnt < 2) {
      printf("usage : cd [destination path]\n");
      return 0;
   } else {
       if(!chdir(cmd->var[1])) { //success
           return 1;
       } else {
           printf("cannot move to directory\n");
           return 0;
       }
   }
}

int execute_help(command *cmd) {
    printf("jshell\n");
    printf("builtins : help, cd, exit\n");
    printf("non builtins : ls ...\n");
    return 1;
}

int execute_exit(command *cmd) {
    return 0;
}

int execute_builtin(command *cmd, int id) {
    int err;
    if(id == 0) { // cd
        err = execute_cd(cmd);
    } else if(id == 1) { //exit
        err = execute_exit(cmd);
    } else if(id == 2) { //help
        err = execute_help(cmd);
    }
    return err;
}

int execute_nonbuiltin(command *cmd, int id) {
    pid_t child_pid;
    child_pid = fork();

    if(child_pid == 0) {
        execvp(cmd->cmd, cmd->var);
        perror("execvp");
        _exit(1);
    } else if(child_pid < 0) {
        perror("fork");
        return 0;
    }
}

int execute(command_seq *cmds) {
    int i;
    int id;
    int err;
    pid_t child_pid;

    int fd_p[2];
    int fd_i;
    int fd_o;

    id = is_builtin(&cmds->commands[0]);
    if(id >= 0) {
        err = execute_builtin(&cmds->commands[0], id);
        return err;
    } else {
        for(i = 0; i < cmds->cmd_cnt; i++) {
/* assert(i < MAX_PIPE); */
            pipe(fd_p);

            child_pid = fork();
            if(child_pid == 0) {
                if(i == 0 && cmds->is_ifile) {
                    int fd_t = open(cmds->ifile, O_RDONLY);
                    dup2(fd_t, 0);
                } else {
                    dup2(fd_i, 0);
                }
                if(i < cmds->cmd_cnt - 1) {
                    dup2(fd_p[1], 1);
                } else if (i == cmds->cmd_cnt - 1 && cmds->is_ofile) {
                    int fd_t = open(cmds->ofile, O_WRONLY);
                    dup2(fd_t, 1);
                }
                close(fd_p[0]);

                execvp(cmds->commands[i].cmd, cmds->commands[i].var);
                close(fd_p[0]);

                perror("execvp");
                _exit(1);
            } else if(child_pid > 0) {
                close(fd_p[1]);
                fd_i = fd_p[0];
            } else {
                perror("fork");
                return 0;
            }
        }

        int status;
        if(!cmds->is_bg) {
            waitpid(child_pid, NULL);
        }
        return 1;
    }
}
