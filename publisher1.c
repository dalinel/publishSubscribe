#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

//Sig handler for SIGINT
void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("publisher1 Received SIGINT\n");
    _exit(0);
}

int main(int argc, char *argv[]){
    struct timespec t;//Used in nanosleep
    t.tv_sec = 3;//Will sleep for 3 seconds
    t.tv_nsec = 0;

    struct timeval trand;//Will be used to renew the seed

    //File descriptors are reiceived as string we need to convert them to int
    char *ptr;
    int fd[2];
    fd[0]=strtol(argv[0], &ptr, 10);
    fd[1]=strtol(argv[1], &ptr, 10);

    //publisher1 close the read (input) side of the pipe;
    close(fd[0]);

    //publisher1 send lower case char
    char randomletter;

    for(;;){
        gettimeofday(&trand, NULL);
        srand(trand.tv_usec * trand.tv_sec);//renew the seed
        randomletter = 'a' + (rand() % 26);//random lower case char
        write(fd[1], &randomletter, sizeof(char));
        printf("publisher1 has sent the char : %c\n", randomletter);
        fflush(stdout);
        signal(SIGINT, sig_handler);//Enable publisher 1 to catch and handle SIGINT
        nanosleep(&t, NULL);
    }
    return 0;
}
