#ifndef INPUT_H
#define INPUT_H

/*
 * Allocates space for a string, and writes user input there. Caller must free.
 *
 * On success: returns pointer to the string on heap, caller must free
 * On failure: exits with EXIT_FAILURE
 */
char *input_read_line();

/*
 * Takes argument 'line', and tokenizes with 'delim'. Caller must free.
 *
 * On success: returns array of pointers to tokens
 * On failure: exits with EXIT_FAILURE
 */
char **input_tokenizer(char *line, char *delim, int *length);

#endif
