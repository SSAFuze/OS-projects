#ifndef MULT_CMD_H
#define MULT_CMD_H

int multiple_commands(char *command) {
    struct mult_cmd_token_array command_tokens;

    command_tokens = mult_tokeniser(command);
    int status;
    int pid;

    for (int i = 0; i < command_tokens.command_count; i++) {
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
                return 1;
            }
        }
    }

    return 0;
}

#endif