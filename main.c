#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define number_of_pipe 8


/**
 * \brief       Handle the signal SIGINT
 */
void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("main Received SIGINT\n");
    fflush(stdout);
}



int main(int argc, char *argv[])
{

    pid_t mediator,subscriber1,subscriber2,subscriber3,publisher1,publisher2;
    int fd[8][2];//Will contain the file descriptors of the 8 pipes
    int status = 0;
    int res;


    char **args;//Will contain the char file descriptors. Will be used in excve
    args=(char**)malloc(17*sizeof(char*));;//We need 2*8 + 1 char*

    int i = 0;
    for(i = 0 ; i < number_of_pipe; i++){
        status += pipe(fd[i]);//Create pipe
    }
    if(status != 0){
        perror("PIPE ERROR");
    }

    int j = 0;
    for(i = 0 ; i < number_of_pipe; i++){
        args[j]=malloc(5*sizeof(char));//Each char fd can have a size of 5 char maximum
        sprintf(args[j],"%d",fd[i][0]);
        j++;
        args[j]=malloc(5*sizeof(char));
        sprintf(args[j],"%d",fd[i][1]);
        j++;
    }
    args[16] = NULL;

    mediator = fork();//create mediator
    if(mediator <0){ perror("FORK MEDIATOR"); return -1; }
    else if(mediator ==0){
        res = execve("./mediator", args, NULL);
        return res;
    }
    else{
        publisher1 = fork();
        if(publisher1 <0){ perror("FORK PUBLISHER1"); return -1; }
        else if(publisher1 ==0){
            res = execve("./publisher1", args, NULL);
            return res;
        }
        else{
            publisher2 = fork();
            if(publisher2 <0){ perror("FORK PUBLISHER2"); return -1; }
            else if(publisher2 ==0){
                res = execve("./publisher2", args, NULL);
                return res;
            }
            else{
                subscriber1 = fork();
                if(subscriber1 <0){ perror("FORK SUBSCRIBER1"); return -1; }
                else if(subscriber1 ==0){
                    res = execve("./subscriber1", args, NULL);
                    return res;
                }
                else{
                    subscriber2 = fork();
                    if(subscriber2 <0){ perror("FORK SUBSCRIBER2"); return -1; }
                    else if(subscriber2 ==0){
                        res = execve("./subscriber2", args, NULL);
                        return res;
                    }
                    else{
                        subscriber3 = fork();
                        if(subscriber3 <0){ perror("FORK SUBSCRIBER3"); return -1; }
                        else if(subscriber3 ==0){
                            res = execve("./subscriber3", args, NULL);
                            return res;
                        }
                        else{
                            signal(SIGINT, sig_handler);//Handle the SIGINT signal --> prevent exit
                            waitpid(subscriber1,&res,0);
                            printf("Exit status of subscriber1 : %d \n",res);
                            waitpid(subscriber2,&res,0);
                            printf("Exit status of subscriber2 : %d \n",res);
                            waitpid(subscriber3,&res,0);
                            printf("Exit status of subscriber3 : %d \n",res);
                            waitpid(mediator,&res,0);
                            printf("Exit status of mediator : %d \n",res);
                            waitpid(publisher1,&res,0);
                            printf("Exit status of publisher1 : %d \n",res);
                            waitpid(publisher2,&res,0);
                            printf("Exit status of publisher2 : %d \n",res);
                            return 0;


                        }
                    }
                }
            }
        }
    }
}
