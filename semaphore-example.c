// compile : gcc -pthread -o semaphore-example semaphore-example.c

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <fcntl.h>

int main () {

	// define file and path were to write
	FILE *f;
	char path[] = "./castella_mateo.txt";
		
	// create semaphore
	sem_t *sem_id = sem_open("semaphore", O_CREAT, 0600, 0);
    	
    	if (sem_id == SEM_FAILED){
        	perror("Parent  : [sem_open] Failed\n");
	}

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
				
				// write data in file
				printf("Parent : writing in file\n");
				f = fopen(path, "a");
				fprintf(f, "père (%d) : envoi de '%s'\n", getpid(), my_input);
				sleep(1);
				fclose(f);
				printf("Parent : end writing in file\n");
				// release semaphore 
				if (sem_post(sem_id) < 0) printf("Parent : sem_post error");
			}
		}
		
		// close semaphore and delete it
		if (sem_close(sem_id) != 0) printf("Parent : sem_close error");
		if (sem_unlink("semaphore") < 0) printf("Parent : sem_unlink error");
		
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
				
				// wait for semaphore to be released before writing in the file
				if (sem_wait(sem_id) < 0) printf("child : sem_wait error");
				printf("Child : writing in file\n");
				f = fopen(path, "a");
				fprintf(f, "fils (%d) : réception de '%s' (mot Nro : %d)\n", getpid(), received_input, words_count);
				fclose(f);
				printf("Child : end writing in file\n");
			}
			// else save new value to compare in the future
			else strncpy(old_input, received_input, sizeof(received_input));
		}
	}

	return 0;
}
