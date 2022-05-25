#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

FILE *fptr;

void error(const char* message, const int code){
    printf("%s\n", message);
    exit(code);
}

char* queue_name(const char* name){
    char* qn_ = malloc(strlen(name) + 1);  //  Allocate memory
    strcpy(qn_, name);

    strcat(qn_, ".queue");
    return qn_;
}

int main(int argc, char const *argv[]) {   
    // vector add <element> <file>
    // vector show <file>
    // vector delete <file>

    if (argc < 2) error("Insufficient arguments.", 1);

    const char* command = argv[1];
    if (strcmp(command, "add") == 0) {
        // Check if all arguments are present
        if (argc < 4) error("Insufficient arguments", 1);

        // Create the file if does not exists
        fptr = fopen(queue_name(argv[3]), "a");

        // Add the element
        fprintf(fptr, "%s\n", argv[2]);

    } else if (strcmp(command, "show") == 0) {
        // Check if all arguments are present
        if (argc < 3) error("Insufficient arguments", 1);

        // Check if the file exists
        fptr = fopen(queue_name(argv[2]), "r");
        if (fptr != NULL) {
            // TODO Print all elements
            char currentline[100];
            while (fgets(currentline, sizeof(currentline), fptr) != NULL) {
                printf("%s", currentline);
            }

        } else {
            error("File does not exists", 2);
        }
    
    } else if (strcmp(command, "delete") == 0) {
        // Check if all arguments are present
        if (argc < 3) error("Insufficient arguments", 1);

        // Delete the file
        if (remove(queue_name(argv[2])) != 0) error("Error while deleting", 4);

    } else if (strcmp(command, "list") == 0) {
        // Check if all arguments are present
        if (argc < 2) error("Insufficient arguments", 1);

        // Retrive all .queue files
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                //printf("%s\n", dir->d_name);

                char* qne[2];
                char* token = strtok(dir->d_name, ".");
                for (size_t i = 0; i < 2; i++)  {
                    qne[i] = token;
                    token = strtok(NULL, " ");
                }
                //printf("%s.%s\n", qne[0], qne[1]);

                if (qne[1] != NULL) {
                    // printf("%s\n", qne[1]);
                    if (strcmp(qne[1], "queue") == 0) {
                        printf("%s\n", qne[0]);
                    }
                }
            }
            closedir(d);
        }

    } else {
        error("Invalid command", 3);
    }
    
    // Close the file
    fclose(fptr);

    return 0;
}