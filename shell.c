#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parse.h"
#include "exec.h"


void srun() {
    int status = 1;
    char *line = NULL;
    size_t size;
    int err;
    command_seq *cmds;
    
    while(status) {
        sleep(1);
        printf(" => ");

        err = getline(&line, &size, stdin);

        if(err == -1) {
            printf("input error\n");
        } else {
            cmds = parse(line);
            status = execute(cmds);
        }
    }
} 

int main(int argc, char ** argv) {
   printf("===== shell start ======\n");
   srun();
   printf("===== shell end ======\n");
   return 0;
}
