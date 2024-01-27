#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define CMDLINE_MAX 512

void parseCommandLine(char *input, char *command, char *args) {
    // Initialize variables
    int i = 0;

    // Parse command
    while (input[i] != ' ' && input[i] != '\n' && input[i] != '\0') {
        command[i+5] = input[i];
        i++;
    }
    command[i+5] = '\0';

    // Parse arguments (including flags)
    while (input[i] != '\n' && input[i] != '\0') {
        if (input[i] == ' ') {
            // Skip spaces
            i++;
        } else {
            // Add characters to args
            while (input[i] != ' ' && input[i] != '\n' && input[i] != '\0') {
                strncat(args, &input[i], 1);
                i++;
            }
            strcat(args, " ");
        }
    }

    // Remove the trailing space from args
    // if (args[0] != '\0') {
    //     args[strlen(args) - 1] = '\0';
    // }
}

int cmd_w_no_args(char *command, char *args)
{
    pid_t pid;
    // char result[CMDLINE_MAX];
    // sprintf(result, "%s%s", "/bin/", input);
    // printf("%s\n", result);
    //char *cmd = "/bin/echo";
    char *arg[] = {command, args, NULL};
    pid = fork();
    if (pid == 0) {
        /* Child */
        execv(command, args);
        perror("execv"); // print when there is error in execv
        exit(1);
    } else if (pid > 0) {
        /* Parent */
        int status;
        waitpid(pid, &status, 0); // wait for child process to execute
    } else {
        perror("fork"); // print when there is error in forking
        exit(1);
    }
    return 0;
}


int main(void)
{
        char input[CMDLINE_MAX];
        char command[] = "/bin/";
        char args[0] = '\0';

        while (1) {
                char *nl;
                int retval;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                fgets(input, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", input);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(input, '\n');
                if (nl)
                        *nl = '\0';

                /* Builtin command */
                if (!strcmp(input, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        break;
                }

                /* Regular command */
                printf("%s\n", input);
                retval = parseCommandLine(input,); //system(cmd);
                fprintf(stdout, "+ completed '%s' [%d]\n",
                        input, retval);
        }

        return EXIT_SUCCESS;
}