#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "parse.h"

void print_command(command *cmd) {
    int i, j;
    for(i = 0; i < cmd->var_cnt; i++) {
        for(j = 0; j < strlen(cmd->var[i]); j++) {
            printf("%c", cmd->var[i][j]);
        }
        printf(" ");
    }
}

void print_command_seq(command_seq *cmds) {
    int i;
    for(i = 0; i < cmds->cmd_cnt; i++) {
        print_command(&cmds->commands[i]);
        printf("\n");
    }

    if(cmds->is_ifile) {
        printf("there is input file\n");
        for(i = 0; cmds->ifile[i] != '\0'; i++)
            printf("%c", cmds->ifile[i]);
        printf("\n");
    } else {
        printf("no input file\n");
    }

    if(cmds->is_ofile) {
        printf("there is output file\n");
        for(i = 0; cmds->ofile[i] != '\0'; i++)
            printf("%c", cmds->ofile[i]);
        printf("\n");
    } else {
        printf("no output file\n");
    }

    if(cmds->is_bg) {
        printf("background\n");
    } else {
        printf("foreground\n");
    }
}

void parse_command(char *line, command *cmd) {
    int i = 0;
    int len = strlen(line);
    cmd->cmd = NULL;
    cmd->var[0] = NULL;
    cmd->var_cnt = 0;
    char word[MAX_VAR_NAME];
    int pos = 0;
    int adding = 0;
    for(i = 0; i < len; i++) {
/* printf("%c\n", line[i]); */
        if(line[i] == '\0') {
            break;
        } else if(!isspace(line[i])) {
            if(pos >= MAX_VAR_NAME) {
                printf("variable name should be smaller than %d\n", MAX_VAR_NAME);
                return;
            }
            word[pos] = line[i];
            adding = 1;
            pos++;
        } else {
            if(adding) {
                if(cmd->var_cnt >= MAX_VAR_CNT - 1) {
                    printf("variable count should be smaller than %d\n", MAX_VAR_CNT);
                    return;
                }
                word[pos] = '\0';
                cmd->var[cmd->var_cnt] = malloc(sizeof(char)*(strlen(word) + 1));
                if(cmd->var[cmd->var_cnt] == NULL) {
                    printf("ENOMEM\n");
                    return;
                }
                strcpy(cmd->var[cmd->var_cnt], word);
                cmd->var_cnt++;
            }
            pos = 0;
            adding = 0;
        }
    }
    cmd->cmd = malloc(sizeof(char)*(strlen(cmd->var[0]) + 1));
    strcpy(cmd->cmd, cmd->var[0]);
    cmd->var[cmd->var_cnt] = NULL;
}

command_seq *parse(char *line) {
#ifdef DEBUGGING
    printf("start parsing\n");
#endif

    int i=0;
    int len =strlen(line);
    int fpos = 0;
    int nomore = 0;
    int cpos = 0;
    char word[MAX_CMD_LEN]; 

    command_seq *cmds;
    cmds = malloc(sizeof(command_seq));
    cmds->is_ifile = 0;
    cmds->is_ofile = 0;
    cmds->is_bg = 0;
    cmds->cmd_cnt = 0;

    for(i = 0; i < len; ) {
        if(line[i] == '\0') {
            break;
        } else if(line[i] == '&') {
            cmds->is_bg = 1;
            if((i != len - 1) && (line[i+1] != '\n' && line[i+1] != '\0')) {
                printf("ignore after &\n");
            }
            break;
        } else if(line[i] == '<') {
            fpos = 0;
            i++;

            while(isspace(line[i]))
                i++;

            cmds->is_ifile = 1;

            while(!isspace(line[i]) && line[i] != '\n' && line[i] != '\0') {
                if(fpos >= MAX_FILE_LEN - 1) {
                    printf("input file name too long\n");
                    free_seq(cmds);
                    return NULL;
                }
/* printf("%d : %c\n", i, line[i]); */
                cmds->ifile[fpos] = line[i];
                i++;
                fpos++;
            }  
            cmds->ifile[fpos] = '\0';
            nomore = 1;
            i++;
        } else if(line[i] == '>') {
            fpos= 0;
            i++;

            while(isspace(line[i]))
                i++;
            
            cmds->is_ofile = 1;

            while(!isspace(line[i]) && line[i] != '\n' && line[i] != '\0') {
                if(fpos >= MAX_FILE_LEN) {
                    printf("output file name too long\n");
                    free_seq(cmds);
                    return NULL;
                }
                cmds->ofile[fpos] = line[i];
                i++;
                fpos++;
            }
            cmds->ofile[fpos] = '\0';
            nomore = 1;
            i++;
        } else if(line[i] == '|') {
            if(nomore) {
                printf("either < or > is met, no more commands\n");
                free_seq(cmds);
                return NULL;
            }
            if(cmds->cmd_cnt >= MAX_PIPE) {
                printf("maximum number of commands are %d\n", MAX_PIPE);
                free_seq(cmds);
                return NULL;
            }
            word[cpos] = '\0';
            parse_command(word, &cmds->commands[cmds->cmd_cnt]);
            cmds->cmd_cnt++;
            cpos = 0;
            i++;
        } else {
            if(cpos >= MAX_CMD_LEN - 1) {
                printf("maximum length of command is %d\n", MAX_CMD_LEN);
                free_seq(cmds);
                return NULL;
            }
            word[cpos] = line[i];
            cpos++;
            i++;
        }
    }
    word[cpos] = '\0';
    parse_command(word, &cmds->commands[cmds->cmd_cnt]);
    cmds->cmd_cnt++;
    return cmds;
}

void free_seq(command_seq *cmds) {
    int i;
    int j;
    for(i = 0; i < cmds->cmd_cnt; i++) {
        command *cmd = &cmds->commands[i];
        for(j = 0; j < cmd->var_cnt; j++) {
            free(cmd->var[j]);
        }
        free(cmd->cmd);
    }
    free(cmds);
}
