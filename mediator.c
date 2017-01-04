#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "circular_buffer.h"
#define pipe_number 8
char * message;
circular_buffer c_s[3]; //3 circular buffer, one for each subscriber
//Prevents blocking if one subscriber is very slow

//Sig handler for SIGINT
void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("mediator Received SIGINT\n");
    fflush(stdout);
    free(message);
    circular_buffer_free(&c_s[0]);
    circular_buffer_free(&c_s[1]);
    circular_buffer_free(&c_s[2]);
    _exit(0);
}

//Shuffle an array, will be use to not read the pipes in a deterministic order
//The size the array to shuffle is n
void shuffle(int *array, size_t n) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);//renew the seed each time
    if (n > 1) {
        size_t i;
        for (i = n - 1; i > 0; i--) {
            size_t j = (unsigned int) (drand48()*(i+1));//random index pointing to the data to shuffle
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
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
    int buffer_error = 0;
    int j=0;
    int i=0;
    int k=0;
    int h=0;


    message=malloc(100*sizeof(char));

    struct timespec t;//Used in nanosleep
    t.tv_sec = 1;//Will sleep for one second
    t.tv_nsec = 0;

    for(i=0;i<3;i++){
        buffer_error = circular_buffer_init(&c_s[i],50);
        if(buffer_error != 0){
            printf("TRYING TO INIT BUFFER ANOTHER TIME\n");
            fflush(stdout);
            Log("TRYING TO INIT BUFFER ANOTHER TIME\n");
            buffer_error = circular_buffer_init(&c_s[i],50);
        }
    }

    int pipe_array[8]={0,1,2,3,4,5,6,7};
    int i_shuffle = 0;

    char *ptr;
    int fd[pipe_number][2];

    //convert char fd to int fd
    for(i=0;i<pipe_number;i++){
        fd[i][0]=strtol(argv[j], &ptr, 10);
        j++;
        fd[i][1]=strtol(argv[j], &ptr, 10);
        j++;
    }

    //Close the unused sides of the pipes
    for (i=0; i < pipe_number ; ++i)
    {
        if(i==3 || i==5 || i==7){
            close(fd[i][0]);//close the read side
        }
        else{
            close(fd[i][1]);//close the write side
        }
    }

    char char_received, char_to_send;
    int int_received;

    struct timeval wait_time;//Will be used in select
    wait_time.tv_sec = 0;
    wait_time.tv_usec = 500;//select will wait 500ms to see if a pipe is ready

    fd_set read_set,read_set_copy;

    for(;;){
        FD_ZERO(&read_set);//initializes the file descriptor set of pipes to read to have zero bits for all file descriptors.
        for (i=0; i < pipe_number ; ++i)
        {
            if(i!=3 && i!=5 && i!=7){
                FD_SET(fd[i][0], &read_set);//Add to the set of pipes to read the fd fd[i][0]
            }
        }
        read_set_copy=read_set;
        if (select(pipe_number-2+1,  &read_set, NULL, NULL, &wait_time) > 0)//if one set is ready
        {
            shuffle(pipe_array,8);//shuffle the array containing indexes
            for (i=0; i < pipe_number ; ++i)
            {
                i_shuffle=pipe_array[i];//random index
                if (FD_ISSET(fd[i_shuffle][0], &read_set_copy)){//Check if the fd was the one belonging to the ready pipe
                    if(i_shuffle==0 || i_shuffle==1){//If the pipe is the pipe from one of the publishers
                        read(fd[i_shuffle][0],&char_received,sizeof(char_received));//read the char sent
                        h=0;
                        for(k=0; k<3; k++){
                            buffer_error= write_buff(&c_s[k], char_received);//copy it in the three buffers
                            if(buffer_error != 0){
                                h++;
                                printf("BUFFER IS FULL CANNOT WRITE ITEM %c IN THE BUFFER\n", char_received);
                                fflush(stdout);
                                sprintf(message, "BUFFER IS FULL CANNOT WRITE ITEM %c IN THE BUFFER\n", char_received);
                                Log("TRYING TO INIT BUFFER ANOTHER TIME\n");
                            }
                        }
                    }
                    else{
                        read(fd[i_shuffle][0],&int_received,sizeof(int_received));//Read the request int from the subscriber
                        buffer_error=read_buff(&c_s[(i_shuffle/2)-1], &char_to_send);//read the buffer corresponding to the subscriber
                        if(buffer_error != 0){
                            printf("BUFFER IS STILL EMPTY SUBSCRIBER %d CANNOT READ ANYTHING YET\n", (i_shuffle/2));
                            fflush(stdout);
                            sprintf(message,"BUFFER IS STILL EMPTY SUBSCRIBER %d CANNOT READ ANYTHING YET\n", (i_shuffle/2));
                            Log(message);
                            char_to_send=' ';
                        }
                        write(fd[i_shuffle+1][1], &char_to_send ,sizeof(char_to_send));//send the char to the subscriber
                    }
                }
            }
        }
        signal(SIGINT, sig_handler);//handle signal
        nanosleep(&t, NULL);//sleep
    }

    return 0;
}
