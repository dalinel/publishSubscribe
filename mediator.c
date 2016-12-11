#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "circular_buffer.h"

#define pipe_number 8

circular_buffer c_s[3]; //3 circular buffer, one for each subscriber
//Prevents blockind if one subscriber is very slow

//Sig handler for SIGINT
void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("mediator Received SIGINT\n");
    fflush(stdout);
    circular_buffer_free(&c_s[0]);
    circular_buffer_free(&c_s[1]);
    circular_buffer_free(&c_s[2]);
    _exit(0);
}

//Shuffle an array, will be use to not read the pipes in a deterministic order
void shuffle(int *array, size_t n) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);//renew the seed each time
    if (n > 1) {
        size_t i;
        for (i = n - 1; i > 0; i--) {
            size_t j = (unsigned int) (drand48()*(i+1));
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int main(int argc, char *argv[]){
    struct timespec t;//Used in nanosleep
    t.tv_sec = 1;//Will sleep for one second
    t.tv_nsec = 0;

    circular_buffer_init(&c_s[0],50);
    circular_buffer_init(&c_s[1],50);
    circular_buffer_init(&c_s[2],50);

    int pipe_array[8]={0,1,2,3,4,5,6,7};
    int i_shuffle = 0;

    char *ptr;
    int fd[pipe_number][2];

    int j=0;
    int i=0;

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
        signal(SIGINT, sig_handler);//handle signal
        FD_ZERO(&read_set);//nitializes the file descriptor set of pipes to read to have zero bits for all file descriptors.
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
                        write_buff(&c_s[0], char_received);//copy it in the three buffers
                        write_buff(&c_s[1], char_received);
                        write_buff(&c_s[2], char_received);

                    }
                    else{
                        read(fd[i_shuffle][0],&int_received,sizeof(int_received));//Read the request int from the subscriber
                        read_buff(&c_s[(i_shuffle/2)-1], &char_to_send);//read the buffer corresponding to the subscriber
                        write(fd[i_shuffle+1][1], &char_to_send ,sizeof(char_to_send));//send the char to the subscriber
                    }
                }
            }
        }
        nanosleep(&t, NULL);//sleep
    }

    return 0;
}
