#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#define CMDLINE_MAX 1000  // Adjust this based on your maximum expected input size

char** parseCommandLine(const char* input) {
    char* inputCopy = strdup(input);  // Create a copy of the input string
    if (inputCopy == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    char** tokens = NULL;
    char* token = strtok(inputCopy, " \n");  // Tokenize based on space and newline

    int count = 0;
    while (token != NULL) {
        tokens = realloc(tokens, sizeof(char*) * (count + 1));
        tokens[count] = malloc(strlen(token) + 1);
        strcpy(tokens[count], token);

        token = strtok(NULL, " \n");
        count++;
    }

    tokens = realloc(tokens, sizeof(char*) * (count + 1));
    tokens[count] = NULL;  // Null-terminate the array

    free(inputCopy);  // Free the copied string

    return tokens;
}


int getCommands(char *input) {
    // Tokenize the input to get the first word
    char** parsedArgs = parseCommandLine(input);

    // Allocate memory for currDir
    char *currDir = (char *)malloc(CMDLINE_MAX);
    if (currDir == NULL) {
        perror("Error Allocating Memory");
        return EXIT_FAILURE;
    }

    // Initialize currentDir once at the start of the program
    if (getcwd(currDir, CMDLINE_MAX) == NULL) {
        perror("Error Accessing Current Directory");
        free(currDir);
        return EXIT_FAILURE;
    }
    
    // Check if the parsedArgs array is not empty
    if (parsedArgs[0] == NULL) {
        free(parsedArgs);
        return 0;
    }

    // Check if the first word is "exit"
    if (strcmp(parsedArgs[0], "exit") == 0) {
        printf("Bye!\n");
        // Clean up memory
        for (int i = 0; parsedArgs[i] != NULL; i++) {
            free(parsedArgs[i]);
        }
        free(parsedArgs);
        exit(EXIT_SUCCESS);
    }
    // Check if the first word is "pwd"
    else if (strcmp(parsedArgs[0], "pwd") == 0) {
        printf("%s\n", currDir);
    }


    //Check if first word is sls
    else if (strcmp(parsedArgs[0], "sls") == 0) {
        DIR *dp;
        struct dirent *ep;
        FILE *fp; 
        int size = 0;

        dp = opendir ("./");
        if (dp != NULL)
        {
                //Reads entire directory 
                while ((ep = readdir(dp)) != NULL)
                { 
                        //Prints the file name and the file size with the expection of the "." and ".." root directories
                        if ( strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0)
                        { 
                             fp = fopen(ep->d_name, "r"); 

                             fseek(fp, 0, 2);

                             size = ftell(fp);

                             printf("%s (%d bytes)\n", ep->d_name, size);

                        }
                       
                }
                 
                (void) closedir (dp);
                printf("+ completed 'sls' [0]\n");
         }
    //If Directory cannot be opened     
    else
    { 
        perror ("Error: cannot open directory\n");
        printf("+ completed 'sls' [1]\n");
    }
         
    } 
    // Check if the first word is "cd"
    else if (strcmp(parsedArgs[0], "cd") == 0) {
        // If there are no arguments after cd, change to the root directory
        if (parsedArgs[1] == NULL) {
            chdir("/");
            printf("Changed to root directory\n");
        }
        // If there is a ".." after cd, go back one directory to the parent of the current
        else if (strcmp(parsedArgs[1], "..") == 0) {
            chdir("..");
            printf("Changed to parent directory\n");
        }
        // If there is an argument after cd, go to that argument
        else {
            if (chdir(parsedArgs[1]) == 0) {
                printf("Changed to given directory\n");
            }
            else {
                perror("ERROR: Unable to change directory");
            }
        }
    }
    // If none of the above conditions match, return 1
    else {
        // Clean up memory
        for (int i = 0; parsedArgs[i] != NULL; i++) {
            free(parsedArgs[i]);
        }
        free(parsedArgs);
        return 0;
    }
    
    // Clean up memory
    for (int i = 0; parsedArgs[i] != NULL; i++) {
        free(parsedArgs[i]);
    }
    free(parsedArgs);

    // Return 0 to indicate successful execution of a built-in command
    return 1;
}


int main() {
    // Initialize variables
    char input[CMDLINE_MAX];
    //char currDir[CMDLINE_MAX];

    // Initialize currentDir once at the start of the program
    // if (getcwd(currDir, sizeof(currDir)) == NULL) {
    //     perror("Error Accessing Current Directory");
    //     return EXIT_FAILURE;
    // }

    while (1)
    {
        // Print out sshell$ to let user know the shell is ready for input
        printf("sshell$ ");
        fflush(stdout);

        // Get input and check if there was an error collecting input
        if (fgets(input, CMDLINE_MAX, stdin) == NULL) {
            perror("Command Input Error");
            exit(1);
        }

        char** parsedArgs = parseCommandLine(input);

        // Print the parsed arguments
        for (int i = 0; parsedArgs[i] != NULL; i++) {
            printf("Argument %d: %s\n", i + 1, parsedArgs[i]);
        }

        // Check if the inputted command is either exit, pwd, or cd
        if (getCommands(input)) {
            continue; 
        }

        // Fork and execute the command
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            execvp(parsedArgs[0], parsedArgs);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);

            // Clean up memory
            for (int i = 0; parsedArgs[i] != NULL; i++) {
                free(parsedArgs[i]);
            }
            free(parsedArgs);
        }
        //return 0;
    }
}
