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
    printf("publisher2 Received SIGINT\n");
    _exit(0);
}
void Log(char *message)
{
    FILE * file;
	file = fopen("LOGFILE.log", "a");//to append to the log after init
    if(file==NULL){
        printf("ERROR CANNOT OPEN LOG");
    }
    else{
    	fputs(message, file);
        fflush(file);
    	fclose(file);//close and open the file at each call
	}
}
int main(int argc, char *argv[]){
    char * message;
    message=malloc(50*sizeof(char));
    struct timespec t;//Used in nanosleep
    t.tv_sec = 5;
    t.tv_nsec = 0;

    struct timeval trand;//Will be used to renew the seed

    //File descriptors are reiceived as string we need to convert them to int
    char *ptr;
    int fd[2];
    fd[0]=strtol(argv[2], &ptr, 10);
    fd[1]=strtol(argv[3], &ptr, 10);

    //publisher2 close the read (input) side of the pipe;
    close(fd[0]);

    //publisher2 send upper case char
    char randomletter;

    for(;;){
        gettimeofday(&trand, NULL);
        srand(trand.tv_usec * trand.tv_sec);//renew the seed
        randomletter = 'A' + (rand() % 26);//random upper case char
        write(fd[1], &randomletter, sizeof(char));
        printf("publisher2 has sent the char : %c\n", randomletter);
        fflush(stdout);
        sprintf(message,"publisher2 has sent the char : %c\n", randomletter);
        Log(message);
        signal(SIGINT, sig_handler);//Enable publisher2 to catch and handle SIGTERM
        nanosleep(&t, NULL);

    }
    return 0;
}
