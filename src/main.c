#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int shell_loop()
{
        char input[1024];
        printf("> ");
        fgets(input, sizeof(input), stdin);
        char *cmds[64];
        int i = 0;
        cmds[i] = strtok(input, " \n\t");
        while(cmds[i] != NULL) cmds[++i] = strtok(NULL, " \n\t");

        pid_t pid = fork();
        if (0 == pid) {
                if (execvp(cmds[0], cmds) == -1) return -1;
        } else if (0 < pid) wait(NULL);

        return 0;
}

int main(void)
{
        while (1) shell_loop();

        return 0;
}
