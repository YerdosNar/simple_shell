#ifndef INPUT_H
#define INPUT_H

/*
 * Allocates space for a string, and writes user input there. Caller must free.
 * If there is a pipe, then sets 'pipe_exists' to 1.
 *
 * On success: returns pointer to the string on heap, caller must free
 * On failure: exits with EXIT_FAILURE
 */
char *input_read_line(int *pipe_exists);

/*
 * Takes argument 'line', and tokenizes with 'delim'. Caller must free.
 *
 * On success: returns array of pointers to tokens
 * On failure: exits with EXIT_FAILURE
 */
char **input_tokenizer(char *line, char *delim);

/*
 * Takes a line, finds index of pipe.
 *
 * On success: returns index of pipe '|'
 * On failure: exits with EXIT_FAILURE
 */
int input_pipe_idx(char *line);

/*
 * Tokenizes input on given index. Caller must free.
 * On success: returns array of two pointers
 * On failure: exits with EXIT_FAILURE
 */
char **input_tokenize_idx(char *line, int index);

#endif
