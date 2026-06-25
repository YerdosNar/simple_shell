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

        if (getline(&input, &bufsize, stdin) == -1) {
                fprintf(stderr, "ERROR: getline() failed\n");
                exit(EXIT_FAILURE);
        }

        return input;
}

char **tokenizer(char *line)
{
        if (!line) {
                fprintf(stderr, "ERROR: Tokenizer received NULL\n");
                exit(EXIT_FAILURE);
        }

        int token_num = 64, i = 0;
        char **tokens= malloc(token_num * sizeof(char*));
        if (!tokens) {
                fprintf(stderr, "ERROR: malloc(tokens) failed\n");
                exit(EXIT_FAILURE);
        }

        char *delim = " \n\t";
        char *token = strtok(line, delim);
        while (token != NULL) {
                tokens[i++] = token;

                if (i >= token_num) {
                        token_num += (token_num / 2);
                        tokens = realloc(tokens, token_num * sizeof(char*));
                        if (!tokens) {
                                fprintf(stderr, "ERROR: realloc(tokens, %d) failed\n",
                                                token_num);
                                exit(EXIT_FAILURE);
                        }
                }
                token = strtok(NULL, delim);
        }

        return tokens;
}

int execute_cmd(char **cmds)
{
        pid_t pid = fork();
        if (0 == pid) {
                if (execvp(cmds[0], cmds) == -1) {
                        fprintf(stderr, "ERROR: execvp(%s) failed\n", cmds[0]);
                        exit(EXIT_FAILURE);
                }
        } else if (0 > pid) {
                wait(NULL);
        }

        return 0;
}

int run()
{
        while (1) {
                printf("> ");
                char *input = read_line();
                char **cmds = tokenizer(input);
                int return_val = execute_cmd(cmds);
                if (return_val) {
                        fprintf(stderr, "ERROR: Return value is not 0\n");
                        exit(EXIT_FAILURE);
                }

                free(input);
                free(cmds);
        }

        return 0;
}

int main(void)
{
        run();

        return 0;
}
