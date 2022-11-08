// compile : gcc -o pipe-example pipe-example.c

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

int main () {
    int my_pipe[2];
    
    if (pipe(my_pipe) == -1) {
        printf("Pipe creation failed");
        return 1;
    }
    
    char my_input[100];
    
    pid_t child_pid;
    child_pid = fork();
    
    int words_count = 0;

    // parent
    if (child_pid != 0) {
        
        int status;
        int childAlive = 1;
        while (childAlive) {
            // wait for child to display new value before asking a new value
            sleep(0.5);
            // check if child is still alive
            if (waitpid(child_pid, &status, WNOHANG) != 0) {
                childAlive = 0;
            // else ask and send data to child
            } else {
                // ask for data
                printf("Parent, type value : ");
                scanf("%s", my_input);
                // write data in pipe
                write(my_pipe[1], my_input, strlen(my_input) + 1);
            }
        }
        
        printf("Child died\n");

    // child
    } else {

        char old_input[100];
        char received_input[100];
        
        // while the received value is not equal to lastname
        while (strcmp(received_input, "castella") != 0 && strcmp(received_input, "mateo_castella") != 0) {
            // read data from parent
            read(my_pipe[0], received_input, sizeof(received_input));
            
            // if we received a new value, increment word count and display new value
            if (strcmp(old_input, received_input) != 0) {
                words_count++;
                printf("Words count : %d\nChild, new value : %s\n", words_count, received_input);
            }
            // else save new value to compare in the future
            else strncpy(old_input, received_input, sizeof(received_input));
        }
    }

    return 0;
}
