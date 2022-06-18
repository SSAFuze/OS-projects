#ifndef MULT_CMD_H
#define MULT_CMD_H

int multiple_commands(char *command) {
    // There is still a memory leak when handling multiple commands (verified with valgrind) - investigate
    struct mult_cmd_token_array command_tokens;

    command_tokens = mult_tokeniser(command);
    int status;
    int pid;
    int return_value = 0; // Default is command chain returns 0

    for (int i = 0; i < command_tokens.command_count; i++) {
        // Need to support backgrounding command
        // Implement cd and exit?

        if (!strcmp("cd", command_tokens.command_pointers[i][0])) {
            if (command_tokens.command_pointers[i][2] != NULL ) { // Assuming simple cd <dir>, no flags
                printf("Too many arguments to \"cd\"\n");
                break;
            }

            else {
                chdir(command_tokens.command_pointers[i][1]);
            }
        }

        else if (!strcmp("exit", command_tokens.command_pointers[i][0])) {
            exit(0);
        }

        else {
        
            pid = fork();

            if (pid == 0) {
                if (execvp(command_tokens.command_pointers[i][0], command_tokens.command_pointers[i]) == -1) { // execvp will only return on error
                        printf("Command \"%s\" not found\n", command_tokens.command_pointers[i][0]);
                        exit(1); // If execvp fails, it fails in child process so needs to exit
                } 
            }

            else {
                waitpid(pid, &status, 0);
                if (WEXITSTATUS(status) != 0) { // Child returns non-zero value
                    return_value = 1;
                    break;
                }
            }
        }
    }

    for (int j = 0; j < command_tokens.command_count; j++) {
        token_free(command_tokens.token_counts[j], command_tokens.command_pointers[j]);
    }

    free(command_tokens.command_pointers);

    return return_value;
}

#endif