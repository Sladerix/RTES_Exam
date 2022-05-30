#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

FILE *fptr;

/*void error_old(const char* message, const int code){
    printf("%s\n", message);

    // Close the file
    if (fptr != NULL) {
        fclose(fptr);
    }

    exit(code);
}*/

void error(const char* message){
    printf("%s\n", message);

    // Close the file
    if (fptr != NULL) {
        fclose(fptr);
    }

    // exit(code);
    exit(EXIT_FAILURE);
}

void syntax(){
    printf("ADDING AN ELEMENT TO A QUEUE\n\tqueue add <element> <queue name> (if the queue does not exists it will be created\n\nSHOW A QUEUE\n\tqueue show <queue name>\n\nDELETE A QUEUE\n\tqueue delete <queue name>\n\nLIST ALL QUEUE\n\tqueue list\n\n");
}

char* queue_name(const char* name){
    //char* qn_ = malloc(strlen(name) * 2);  //  Allocate memory
    char* qn_ = (char*)name;
    strcat(qn_, ".queue");
    return qn_;
}

int main(int argc, char const *argv[]) {
    // vector add <element> <file>
    // vector show <file>
    // vector delete <file>

    if (argc < 2) {
        // error("Insufficient arguments.", 1);
        syntax();
        return EXIT_FAILURE;
    }

    const char* command = argv[1];
    if (strcmp(command, "add") == 0) {
        // Check if all arguments are present
        if (argc < 4) error("Insufficient arguments");

        // Create the file if does not exists
        fptr = fopen(queue_name(argv[3]), "a");

        // Add the element
        fprintf(fptr, "%s\n", argv[2]);

    } else if (strcmp(command, "show") == 0) {
        // Check if all arguments are present
        if (argc < 3) error("Insufficient arguments");

        // Check if the file exists
        fptr = fopen(queue_name(argv[2]), "r");
        if (fptr != NULL) {
            // TODO Print all elements
            char currentline[100];
            while (fgets(currentline, sizeof(currentline), fptr) != NULL) {
                printf("%s", currentline);
            }

        } else {
            error("Queue does not exists");
        }
    
    } else if (strcmp(command, "delete") == 0) {
        // Check if all arguments are present
        if (argc < 3) error("Insufficient arguments");

        // Delete the file
        if (remove(queue_name(argv[2])) != 0) error("Error while deleting");

    } else if (strcmp(command, "list") == 0) {
        // Check if all arguments are present
        if (argc < 2) error("Insufficient arguments");

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

    } else if (strcmp(command, "help") == 0) {
        syntax();

    } else {
        error("Invalid command");
    }
    
    // Close the file
    if (fptr != NULL) {
        fclose(fptr);
    }

    return EXIT_SUCCESS;
}