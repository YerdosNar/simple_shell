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

int execute_pipe(char **cmd1, char **cmd2)
{
        int fd[2];
        if (pipe(fd) == -1) {
                fprintf(stderr, "ERROR: pipe() failed\n");
                exit(EXIT_FAILURE);
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {
                if (dup2(fd[1], STDOUT_FILENO) == -1) {
                        fprintf(stderr, "ERROR: dup2(fd[1]) failed\n");
                        exit(EXIT_FAILURE);
                }
                close(fd[0]);
                close(fd[1]);
                execvp(cmd1[0], cmd1);
                fprintf(stderr, "ERROR: execvp(%s) failed\n", cmd1[0]);
                exit(EXIT_FAILURE);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
                if (dup2(fd[0], STDIN_FILENO) == -1) {
                        fprintf(stderr, "ERROR: dup2(fd[0]) failed\n");
                        exit(EXIT_FAILURE);
                }
                close(fd[0]);
                close(fd[1]);
                execvp(cmd2[0], cmd2);
                fprintf(stderr, "ERROR: execvp(%s) failed\n", cmd2[0]);
                exit(EXIT_FAILURE);
        }

        close(fd[0]);
        close(fd[1]);

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);

        return 0;
}

int run()
{
        while (1) {
                printf("> ");
                int pipe_count = 0;
                int num_cmds  = 0;
                char *input = input_read_line(&pipe_count);
                char **chunks = input_tokenizer(input, "|", &num_cmds);
                char ***cmds = malloc(num_cmds * sizeof(char**));
                for (int i = 0; i < num_cmds; i++)
                        cmds[i] = input_tokenizer(chunks[i], " \n", NULL);

                execute_pipeline(cmds, num_cmds);

                free(input);
                free(chunks);
                free(cmds);
        }

        return 0;
}

int main(void)
{
        signal(SIGINT, handler);
        run();

        return 0;
}
