#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
        char input[1024];
        printf("> ");
        fgets(input, sizeof(input), stdin);
        char *cmds[64];
        int i = 0;
        cmds[i] = strtok(input, " \n");
        while (cmds[i] != NULL) cmds[++i] = strtok(NULL, " \n");

        pid_t pid = fork();
        if (pid == 0) {
                if (execvp(cmds[0], cmds) == -1) return -1;
        } else if (pid > 0) {
                wait(NULL);
        }

        return 0;
}
