#include "../include/input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *input_read_line(int *pipe_exists)
{
        int input_len = 128;
        char *input = malloc(input_len);
        if (!input) {
                fprintf(stderr, "ERROR: malloc(%d) failed\n",
                                input_len);
                exit(EXIT_FAILURE);
        }

        int c, i = 0;
        while ((c = getchar()) != '\n' && c != EOF) {
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
                if (c == '|') *pipe_exists = 1;
                input[i++] = c;
        }
        if (c == EOF && i == 0) {
                free(input);
                return NULL;
        }
        input[i] = '\0';

        return input;
}

char **input_tokenizer(char *line, char *delim)
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
        tokens[i] = NULL;

        return tokens;
}

int input_pipe_idx(char *line)
{
        if (!line) {
                fprintf(stderr, "ERROR: pipe_idx(NULL)\n");
                exit(EXIT_FAILURE);
        }

        int idx = -1;
        while (line[++idx] != '\0') {
                if (line[idx] == '|') break;
        }

        if (idx == 0) {
                fprintf(stderr, "ERROR: pipe cannot be first\n");
                exit(EXIT_FAILURE);
        }

        return idx;
}
