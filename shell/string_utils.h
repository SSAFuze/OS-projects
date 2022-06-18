#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct token_array { // Used to store tokens and the number of tokens, allowing tokeniser to return both
    char **token_pointers;
    int token_count;
};

struct mult_cmd_token_array { // For when more than one command used
    char ***command_pointers;
    int command_count;
    int token_counts[20]; // Support up to 20 commands
};

struct token_array tokeniser(char *command) {

    struct token_array tok_struct;

    int cmd_len = strlen(command);
    
    int counter = 0; 
    int token_index = 0;

    // Constants are currently arbitrary, may wish to change and define as macros for readability
    char **tokens = malloc(20 * sizeof(char *)); // Array of 20 pointers to strings
    char *token = malloc(50 * sizeof(char)); // Each arg can only be 50 chars long    

    for (int i = 0; i < cmd_len; i++) {
        char current_char = command[i];
        if (current_char == ' ' | current_char == '\n' | current_char == '\t') { 
            if (token_index != 0) { // Allows whitespace before command and then more than one whitespace between commands
                token[token_index] = '\0';
                tokens[counter] = (char *)malloc(strlen(token)+1 * sizeof(char)); // Creating pointer to memory for this token, exact length of memory needed (+1 to give room for null terminator)
                strcpy(tokens[counter], token);
                counter++;
                token_index = 0;
            }
        }

        else {
            token[token_index] = command[i];
            token_index++;
        }
    }
    
    free(token);
    tok_struct.token_pointers = tokens;
    tok_struct.token_count = counter;
    return tok_struct;
}

struct mult_cmd_token_array mult_tokeniser(char *command) {

    struct mult_cmd_token_array tok_struct;

    int cmd_len = strlen(command);

    int command_count = 1; 
    int counter = 1;
    int token_index = 0;

    // Constants are currently arbitrary, may wish to change and define as macros for readability
    char ***commands = malloc(20 * sizeof(char **)); // Array of 20 pointers to array of pointers to strings
    char **tokens = malloc(20 * sizeof(char *));
    char *token = malloc(50 * sizeof(char)); // Each arg can only be 50 chars long    

    for (int i = 0; i < cmd_len; i++) {
        char current_char = command[i];
        if (current_char == ' ' | current_char == '\n' | current_char == '\t') { 
            if (token_index != 0) { // Allows whitespace before command and then more than one whitespace between commands
                token[token_index] = '\0';

                if (!strcmp(token, "&&")) {
                    commands[command_count-1] = (char **)malloc(counter * sizeof(char *));
                    memcpy(commands[command_count-1], tokens, counter * sizeof(char *));
                    memset(tokens, '\0', 20 * sizeof(char *)); // Zeroes tokens otherwise if previous command had more tokens, later ones may be present in subsequent commands
                    tok_struct.token_counts[command_count-1] = counter;
                    command_count++;
                    counter = 0;
                }

                tokens[counter-1] = (char *)malloc(strlen(token)+1 * sizeof(char)); // Creating pointer to memory for this token, exact length of memory needed (+1 to give room for null terminator)
                strcpy(tokens[counter-1], token);
                counter++;
                token_index = 0;
            }
        }

        else {
            token[token_index] = command[i];
            token_index++;
        }
    }

    commands[command_count-1] = (char **)malloc(counter * sizeof(char *));
    memcpy(commands[command_count-1], tokens, counter * sizeof(char *));
    tok_struct.token_counts[command_count-1] = counter;
    
    tok_struct.token_counts[command_count] = 0;

    free(token);
    tok_struct.command_pointers = commands;
    tok_struct.command_count = command_count;
    return tok_struct;

}

void token_free(int token_count, char **tokens) { // Freeing token pointers and tokens, otherwise they are stored in memory until process terminates 
    for (int i; i < token_count; i++) {
        printf("removing another token\n");
        free(tokens[i]);
    }
    free(tokens);
}

int command_count(char *command) { // Looks for instances of && etc (symbols that indicate more than one command to execute) to see how many commands must be executed

    int commands = 1;
    int length = strlen(command);

    for (int i = 0; i < length-1; i++) { // Last char will not have more characters after it so checks not worthwhile
        if (command[i] == '&' && command[i+1] == '&' && command[i+2] != '&') { // Checking for && and not &&& for example
            commands++;
        }
    }

    return commands;

}

#endif