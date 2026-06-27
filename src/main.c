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
                int pipe_exists = 0;
                char *input = input_read_line(&pipe_exists);
                if (pipe_exists) {
                        char **halves = input_tokenizer(input, "|");
                        printf("cmd1: %s\n", halves[0]);
                        printf("cmd2: %s\n", halves[1]);
                        char **cmd1 = input_tokenizer(halves[0], " \n");
                        char **cmd2 = input_tokenizer(halves[1], " \n");
                        execute_pipe(cmd1, cmd2);
                        free(cmd1);
                        free(cmd2);
                        free(halves);
                } else {
                        char **cmds = input_tokenizer(input, " \n");
                        int return_val = execute_cmd(cmds);
                        if (return_val) {
                                fprintf(stderr, "ERROR: Return value is not 0\n");
                                exit(EXIT_FAILURE);
                        }
                        free(cmds);
                }

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
