#include "../include/input.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void handler(int sig)
{
        printf("\rCaught signal %d\n", sig);
        printf("Exiting...\n");
        exit(sig);
}

int execute_pipeline(char ***cmds, int num_cmds)
{
        pid_t *pids = malloc(num_cmds * sizeof(pid_t));
        if (!pids) {
                fprintf(stderr, "ERROR: malloc(pids) failed\n");
                exit(EXIT_FAILURE);
        }

        int prev_fd = STDIN_FILENO;

        for (int i = 0; i < num_cmds; i++) {
                int new_fds[2];
                int is_last = (i == num_cmds - 1);

                if (!is_last) {
                        if (pipe(new_fds) == -1) {
                                fprintf(stderr, "ERROR: pipe(%d) failed\n", i);
                                exit(EXIT_FAILURE);
                        }
                }

                pid_t pid = fork();
                if (pid == -1) {
                        fprintf(stderr, "ERROR: fork(%d) failed\n", i);
                        exit(EXIT_FAILURE);
                }
                if (pid == 0) {
                        if (prev_fd != STDIN_FILENO) {
                                if (dup2(prev_fd, STDIN_FILENO) == -1) {
                                        fprintf(stderr, "ERROR: dup2(%d) failed\n",
                                                        i);
                                        exit(EXIT_FAILURE);
                                }
                                close(prev_fd);
                        }
                        if (!is_last) {
                                if (dup2(new_fds[1], STDOUT_FILENO) == -1) {
                                        fprintf(stderr, "ERROR: dup2(%d) failed\n",
                                                        i);
                                        exit(EXIT_FAILURE);
                                }
                                close(new_fds[0]);
                                close(new_fds[1]);
                        }
                        execvp(cmds[i][0], cmds[i]);
                        fprintf(stderr, "ERROR: execvp(%s) failed\n",
                                        cmds[i][0]);
                        exit(EXIT_FAILURE);
                }

                pids[i] = pid;

                if (prev_fd != STDIN_FILENO)
                        close(prev_fd);

                if(!is_last) {
                        close(new_fds[1]);
                        prev_fd = new_fds[0];
                }
        }

        for (int i = 0; i < num_cmds; i++)
                waitpid(pids[i], NULL, 0);

        free(pids);
        return 0;
}

int run()
{
        while (1) {
                printf("> ");
                int num_cmds  = 0;
                char *input = input_read_line();
                if (!input) {
                        printf("\n");
                        break;
                }
                char **chunks = input_tokenizer(input, "|", &num_cmds);
                char ***cmds = malloc(num_cmds * sizeof(char**));
                for (int i = 0; i < num_cmds; i++)
                        cmds[i] = input_tokenizer(chunks[i], " \n", NULL);

                execute_pipeline(cmds, num_cmds);

                free(cmds);
                free(chunks);
                free(input);
        }

        return 0;
}

int main(void)
{
        signal(SIGINT, handler);
        run();

        return 0;
}
