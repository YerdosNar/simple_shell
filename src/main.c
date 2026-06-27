#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *read_line(void)
{
        int input_len = 128;
        char *input = malloc(input_len);
        if (!input) {
                fprintf(stderr, "ERROR: malloc(%d) failed\n",
                                input_len);
                exit(EXIT_FAILURE);
        }

        int c, i = 0;
        while ((c = getchar()) != '\n') {
                if (i >= input_len) {
                        input_len += input_len / 2;
                        input = realloc(input, input_len);
                        if (!input) {
                                fprintf(stderr,
                                        "ERROR: realoc(%d) failed\n",
                                        input_len);
                                exit(EXIT_FAILURE);
                        }
                }
                input[i++] = c;
        }
        input[i] = '\0';

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

        char *delim = " \n";
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
        int status;

        if (0 == pid) {
                if (execvp(cmds[0], cmds) == -1) {
                        fprintf(stderr, "ERROR: execvp(%s) failed\n", cmds[0]);
                }
                exit(EXIT_FAILURE);
        } else if (0 < pid) {
                do { waitpid(pid, &status, WUNTRACED);
                } while(!WIFEXITED(status) && !WIFSIGNALED(status));
        } else {
                fprintf(stderr, "ERROR: fork() failed, pid=%d\n", pid);
                exit(EXIT_FAILURE);
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
