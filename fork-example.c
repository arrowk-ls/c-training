// compile : gcc -o fork-example fork-example.c

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void sigkill();

int main () {
    pid_t child_pid;
        child_pid = fork ();

        // parent
        if (child_pid != 0) {
                printf ("I'm parent, child PID : %d\n", (int) child_pid);
                printf("I'm parent, parent PID : %d\n", (int) getpid());
                // ask a char
                char c;
                while (c = getchar() != 'b') {
                        getchar();
                }
                // send sigkill signal to child
                kill(child_pid, SIGKILL);
                printf("Waiting for child die\n");
                // wait for child to die
                waitpid(child_pid, &child_pid, WUNTRACED);
                printf("Child died\n");
        // child
        } else {
                printf("I'm child, child PPID : %d\n", (int) getppid());
                printf ("I'm child, child ID : %d\n", (int) getpid ());
                // handle sigkill signal
                signal(SIGKILL, sigkill);
                for (;;)
                        ;
        }

    return 0;
}

void sigkill() {
        // reinitialise signal
        signal(SIGKILL, sigkill);
}
