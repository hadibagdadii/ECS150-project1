#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define CMDLINE_MAX 512

struct node {
    char *data;
    struct node *next;
};

struct linked_list {
  struct node *head;
  struct node *tail;
};

void linked_list_init(struct linked_list *list) {
  list->head = NULL;
  list->tail = NULL;
}

void linked_list_add(struct linked_list *list, char *data) {
  struct node *new_node = malloc(sizeof(struct node));
  new_node->data = data;
  new_node->next = NULL;

  if (list->head == NULL) {
    list->head = new_node;
    list->tail = new_node;
  } else {
    list->tail->next = new_node;
    list->tail = new_node;
  }
}

void linked_list_print(struct linked_list *list) {
  struct node *current_node = list->head;
  while (current_node != NULL) {
    printf("%s ", current_node->data);
    current_node = current_node->next;
  }
  printf("\n");
}

void parseCommandLine(char *input, char *command, char *args) {
    // Initialize variables
    int i = 0, j = 0, k = 0;
    struct linked_list flag_list;
    linked_list_init(&flag_list);

    char cmd[CMDLINE_MAX] = {0};
    // char args[CMDLINE_MAX] = {0};

    // Parse command
    while (input[i] != ' ' && input[i] != '\n' && input[i] != '\0') {
        cmd[i] = input[i];
        i++;
    }

    // Append "/bin/" to the beginning of cmd
    sprintf(command, "%s%s", "/bin/", cmd);
    printf("%s\n", command);

    // Parse arguments (including flags)
    while (input[i] != '\n' && input[i] != '\0') {
        if (input[i] == ' ') {
            // Skip spaces
            i++;
        } else if (input[i] == '-') {
            // Add characters to args as a flag
            char flag[CMDLINE_MAX] = {0};
            while (input[i] != ' ' && input[i] != '\n' && input[i] != '\0') {
                flag[k] = input[i];
                i++;
                k++;
            }
            linked_list_add(&flag_list, flag);

            // Clear the flag array for the next flag
            memset(flag, '\0', CMDLINE_MAX);
        } else {
            // Add characters to args
            while (input[i] != '\n' && input[i] != '\0') {
                args[j] = input[i];
                i++;
                j++;
            }
            args[j] = ' ';  // Add space between arguments
            j++;
        }
    }

    // Remove the trailing space from args
    if (j > 0) {
        args[j - 1] = '\0';
    }

    linked_list_print(&flag_list);
    printf("%s\n", args);
}

int cmd_w_no_args(char *command, char *args)
{
    pid_t pid;
    // char result[CMDLINE_MAX];
    // sprintf(result, "%s%s", "/bin/", input);
    // printf("%s\n", result);
    //char *cmd = "/bin/echo";
    char *arg1 = command; //command;
    char *arg2 = args;//args;
    char *arguments[] = {arg1, arg2, NULL};
    pid = fork();
    if (pid == 0) {
        /* Child */
        execv(arg1, arguments);
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
        char command[CMDLINE_MAX + 5];
        char args[CMDLINE_MAX] = {0};

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
                parseCommandLine(input, command, args); //system(cmd);
                retval = cmd_w_no_args(command, args);
                fprintf(stdout, "+ completed '%s' [%d]\n",
                        input, retval);
        }

        return EXIT_SUCCESS;
}