#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *read_line(void)
{
        char *input= NULL;
        size_t bufsize = 0;

        if (getline(&input, &bufsize, stdin) == -1)
                exit(EXIT_FAILURE);

        return input;
}

char **tokenizer(char *line)
{
        char **cmds = malloc(64 * sizeof(char*));
        if (!cmds) exit(EXIT_FAILURE);
        int i = 0;
        cmds[i] = strtok(line, " \n\t");
        while (cmds[i] != NULL && i < 64)
                cmds[++i] = strtok(NULL, " \n\t");

        return cmds;
}

int shell_loop()
{
        while (1) {
                printf("> ");
                char *input = read_line();
                char **cmds = tokenizer(input);

                pid_t pid = fork();
                if (0 == pid) {
                        if (execvp(cmds[0], cmds) == -1) return -1;
                } else if (0 < pid) wait(NULL);

                free(input);
                free(cmds);
        }
        return 0;
}

int main(void)
{
        shell_loop();

        return 0;
}
