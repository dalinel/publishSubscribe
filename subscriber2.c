#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

//Sig handler for SIGINT
void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("subscriber2 Received SIGINT\n");
    fflush(stdout);
    _exit(0);
}

int main(int argc, char *argv[]){
    struct timespec t;//Used in nanosleep
    t.tv_sec = 8;
    t.tv_nsec = 0;

    const int request_info = 1;//int send when the subscriber request data to the mediator
    char char_received;
    char *ptr1;
    char *ptr2;
    int fd[2];
    int fd1[2];
    //File descriptors are reiceived as string we need to convert them to int
    fd[0]=strtol(argv[8], &ptr1, 10);
    fd[1]=strtol(argv[9], &ptr2, 10);
    fd1[0]=strtol(argv[10], &ptr1, 10);
    fd1[1]=strtol(argv[11], &ptr2, 10);
    //subscriber close the read (input) side of the pipe;
    close(fd[0]);
    //subscriber close the write (output) side of the pipe;
    close(fd1[1]);

    for(;;){
        write(fd[1], &request_info, sizeof(int));
        printf("subscriber2 has requested information\n");
        fflush(stdout);
        signal(SIGINT, sig_handler);//Enable subscriber2 to catch and handle SIGINT
        read(fd1[0],&char_received,sizeof(char_received));
        printf("subscriber2 has reiceived string : %c\n",char_received);
        fflush(stdout);
        nanosleep(&t,NULL);
    }
    return 0;
}
