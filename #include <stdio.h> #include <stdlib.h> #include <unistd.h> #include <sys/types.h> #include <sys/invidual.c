#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

void handle_sigint(int sig)
{
        printf("\n\n[Interrupt Detected] Ctrl + C (SIGNINT) caught!\n");
        printf("[System Message] Cleaning  up and terminating all process.\n");
        exit(0);
}
int main ()
{
        int pipe1[2], pipe2[2];
        pid_t pid1, pid2;
        char buffer[100];
        signal(SIGINT, handle_sigint);

        if(pipe(pipe1) == -1){
                perror("Pipe 1 creation failed");
                exit(1);
        }

        if(pipe(pipe2) == -1){
                perror("Pipe 2 creation  failed");
                exit(1);
        }

        pid1 = fork();
        if (pid1 < 0){
                perror("Fork failed for child");
                exit(1);
        }

        if (pid1== 0){
                close(pipe1[1]);
                close(pipe2[0]);
                close(pipe2[1]);
                char read_msg[100];
                while(1){
                        int n = read(pipe1[0], read_msg, sizeof(read_msg));
                        if (n>0){
                                read_msg[n] = '\0';
                                printf("[Child 1 | PID: %d] Receive from parent: \"%s\"\n", getpid(), read_msg);
                        }
                }
                close(pipe1[0]);
                exit(0);
        }

        pid2 = fork();
        if (pid2 <0){
                perror("Fork failed for child 2");
                exit(1);
        }

        if (pid2 == 0){
                close(pipe2[1]);
                close(pipe1[0]);
                close(pipe1[1]);
                char read_msg[100];

        while (1){
                int n = read(pipe2[0], read_msg, sizeof(read_msg));
                if ( n> 0){
                        read_msg[n] = '\0';
                        printf("[Child 2 | PID: %d] Received from parent \"%s\"\n", getpid(), read_msg);
                }
        }
        close(pipe2[0]);
        exit(0);
}

close(pipe1[0]);
close(pipe2[0]);

printf("Parent process started (PID: %d)\n", getpid());
printf("Status : Connected to Child 1 and Child 2 via pipe\n");
printf("Type a message and enter (Ctrl + C to interrupt)\n");

while(1){
        printf("\nInput: ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL){
                break;
        }

        buffer[strcspn(buffer, "\n")] = 0;
                continue;
        }

        write(pipe1[1], buffer, strlen(buffer) +1);
        write(pipe2[1], buffer , strlen(buffer) +1);
        usleep(100000);
        }
close(pipe1[1]);
close(pipe2[1]);
return 0;
}
