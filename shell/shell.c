#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "string_utils.h"
#include "mult_cmd.h"

struct node { // Used for linked list in checking PIDs
    int pid;
    struct node *next; // Next element in linked list
};

// Currently show ^C in awkward places, will look to tidy up
// Either don't show or print a new line?

void parent_signal_handler(int signum) { // Parent does not do anything in response to signal

}

void child_signal_handler(int signum) { // Child exits when receiving SIGINT
    if (signum == SIGINT) {
        exit(0);
    }
}

int main() {
    signal(SIGINT, parent_signal_handler); // Lets parent_signal_handler deal with CTRL+C signal (ie do nothing)

    char command[200]; // Commands cannot exceed 200 chars currently
    struct token_array tokens;
    int background; // Number used to determine how to treat a process

    struct node *root; // Constant first element of linked list
    root = (struct node *) malloc(sizeof(struct node));
    root->next = NULL;
    int wstatus;

    struct node *traversing_node;
    struct node *tmp; // Used for when swapping pointers

    while (1) { 

        traversing_node = root;

        while (traversing_node->next != NULL) {
            // if statement checking on waitpid, passes if process has terminated
            if (waitpid(traversing_node->next->pid, &wstatus, WNOHANG) > 0) { // WNOHANG returns immediately if process not terminated
                printf("Background process finished\n");
                // Terminating list if the one being removed is final element
                if (traversing_node->next->next == NULL) {
                    traversing_node->next = NULL;
                }
                else {
                    /* 
                    Getting address of subsequent node and changing current next to that 
                    Before: (assuming node being checked next address isn't NULL)

                        ----------------    ----------------------    -------------------
                        | Current node | -> | Node being checked | -> | Subsequent node |
                        ----------------    ----------------------    -------------------


                    After:

                              --------------------------->----------------------
                              ^                                                |
                              |                                                |
                        ----------------    ----------------------    -------------------
                        | Current node |    | Node being checked |    | Subsequent node |
                        ----------------    ----------------------    -------------------

                    */

                    tmp = traversing_node->next;
                    traversing_node->next = traversing_node->next->next;
                    // Deleting node that has been unlinked
                    free(tmp);
                }
            }
            else {
                traversing_node = traversing_node->next;
            }
        }

        printf("$ ");
        fgets(command, 200, stdin); 

        int cmd_count = command_count(command);

        if (cmd_count > 1) { // Not currently supporting backgrounding of chained commands
            multiple_commands(command);
            continue;
        }

        tokens = tokeniser(command);

        if (tokens.token_count == 0) { // Deals with whitespace only input, returns to prompt
            token_free(tokens.token_count, tokens.token_pointers);
            continue;
        }
 
        if (!strcmp("&", tokens.token_pointers[tokens.token_count-1])) { // Requires space between & and previous token to work, should also check last character of final token?
            background = 1;
        }

        else {
            background = 0;
        }

        if (!strcmp("cd", tokens.token_pointers[0])) {
            if (tokens.token_pointers[2] != NULL ) { // Assuming simple cd <dir>, no flags
                printf("Too many arguments to \"cd\"\n");
            }

            else {
                chdir(tokens.token_pointers[1]);
            }
        }

        else if (!strcmp("exit", tokens.token_pointers[0])) {
            exit(0);
        }

        else {

            if (background == 0) { // Standard foreground execution
                int pid = fork();

                if (pid == 0) {
                    signal(SIGINT, child_signal_handler);
                    if (execvp(tokens.token_pointers[0], tokens.token_pointers) == -1) { // execvp will only return on error
                        printf("Command \"%s\" not found\n", tokens.token_pointers[0]);
                        exit(1); // If execvp fails, it fails in child process so needs to exit
                    } 
                }

                else {
                    waitpid(pid, &wstatus, 0); // PID must be specified or may reap any terminated background process
                }
            }

            else if (background == 1) { // Use of &

                int pid = fork();

                if (pid == 0) {
                    tokens.token_pointers[tokens.token_count-1] = NULL;

                    if (execvp(tokens.token_pointers[0], tokens.token_pointers) == -1) { // execvp will only return on error
                        printf("Command \"%s\" not found\n", tokens.token_pointers[0]);
                        exit(1); // If execvp fails, it fails in child process so needs to exit
                    } 
                }

                else {

                    // Appending PID of new background process to linked list
                    traversing_node = root;

                    if (traversing_node->next != NULL) {
                        while (traversing_node-> next != NULL) {
                            traversing_node = traversing_node->next;
                        }
                    }

                    traversing_node->next = malloc(sizeof(struct node));

                    traversing_node->next->pid = pid;
                    traversing_node->next->next = NULL;
                }

            }
        }

        token_free(tokens.token_count, tokens.token_pointers);
    }
}