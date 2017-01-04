#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define number_of_pipe 8

int LogCreated = 0;
void Log(char *message)
{
	FILE *file;
	if (LogCreated==0) {
		file = fopen("LOGFILE.log", "w");//TO rewrite or create the log at init
		LogCreated = 1;
	}
	else{
		file = fopen("LOGFILE.log", "a");//to append to the log after init
    }

	if (file == NULL) {
		if (LogCreated==1){ LogCreated = 0;}
	}
	else
	{
		fputs(message, file);
		fflush(file);
		fclose(file);//close and open the file at each call
	}
}
/**
 * Handle the signal SIGINT
 */
void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("main Received SIGINT\n");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    printf("PID OF MAIN %ld\n", (long)getpid());
    fflush(stdout);

    //INIT LOG
    Log("Log INIT\n");

    pid_t mediator,subscriber1,subscriber2,subscriber3,publisher1,publisher2;
    int fd[8][2];//Will contain the file descriptors of the 8 pipes
    int status = 0;
    int res;

    char **args;//Will contain the char file descriptors. Will be used in execve
    args=(char**)malloc(17*sizeof(char*));//We need (2*8 + 1) char*

    int i = 0;
    for(i = 0 ; i < number_of_pipe; i++){
        status += pipe(fd[i]);//Create pipe
    }
    if(status != 0){
        perror("PIPE ERROR");//error create pipe
        Log("PIPE ERROR");
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
    if(mediator <0){ perror("FORK MEDIATOR ERROR");Log("FORK MEDIATOR ERROR"); return -1; }
    else if(mediator ==0){
        char * message;
        message=malloc(50*sizeof(char));
        printf("PID OF THE MEDIATOR %ld\n", (long)getpid());
        fflush(stdout);
        sprintf(message,"PID OF THE MEDIATOR %ld\n", (long)getpid());
        Log(message);
		free(message);
        res = execve("./mediator", args, NULL);
        return res;
    }
    else{
        publisher1 = fork();
        if(publisher1 <0){ perror("FORK PUBLISHER1 ERROR"); Log("FORK PUBLISHER1 ERROR"); return -1; }
        else if(publisher1 ==0){
            char * message;
            message=malloc(50*sizeof(char));
            printf("PID OF PUBLISHER1 %ld\n", (long)getpid());
            fflush(stdout);
            sprintf(message,"PID OF PUBLISHER1 %ld\n", (long)getpid());
            Log(message);
			free(message);
            res = execve("./publisher1", args, NULL);
            return res;
        }
        else{
            publisher2 = fork();
            if(publisher2 <0){ perror("FORK PUBLISHER2 ERROR"); Log("FORK PUBLISHER2 ERROR");return -1; }
            else if(publisher2 ==0){
                char * message;
                message=malloc(50*sizeof(char));
                printf("PID OF PUBLISHER2 %ld\n", (long)getpid());
                fflush(stdout);
                sprintf(message,"PID OF PUBLISHER2 %ld\n", (long)getpid());
                Log(message);
                res = execve("./publisher2", args, NULL);
				free(message);
                return res;
            }
            else{
                subscriber1 = fork();
                if(subscriber1 <0){ perror("FORK SUBSCRIBER1 ERROR"); Log("FORK SUBSCRIBER1 ERROR"); return -1; }
                else if(subscriber1 ==0){
                    char * message;
                    message=malloc(50*sizeof(char));
                    printf("PID OF SUBSCRIBER1 %ld\n", (long)getpid());
                    fflush(stdout);
                    sprintf(message,"PID OF SUBSCRIBER1 %ld\n", (long)getpid());
                    Log(message);
					free(message);
                    res = execve("./subscriber1", args, NULL);
                    return res;
                }
                else{
                    subscriber2 = fork();
                    if(subscriber2 <0){  perror("FORK SUBSCRIBER2 ERROR"); Log("FORK SUBSCRIBER2 ERROR"); return -1;}
                    else if(subscriber2 ==0){
                        char * message;
                        message=malloc(50*sizeof(char));
                        printf("PID OF SUBSCRIBER2 %ld\n", (long)getpid());
                        fflush(stdout);
                        sprintf(message,"PID OF SUBSCRIBER2 %ld\n", (long)getpid());
                        Log(message);
						free(message);
                        res = execve("./subscriber2", args, NULL);
                        return res;
                    }
                    else{
                        subscriber3 = fork();
                        if(subscriber3 <0){ perror("FORK SUBSCRIBER3 ERROR"); Log("FORK SUBSCRIBER3 ERROR"); return -1;}
                        else if(subscriber3 ==0){
                            char * message;
                            message=malloc(50*sizeof(char));
                            printf("PID OF SUBSCRIBER3 %ld\n", (long)getpid());
                            fflush(stdout);
                            sprintf(message,"PID OF SUBSCRIBER3 %ld\n", (long)getpid());
                            Log(message);
							free(message);
                            res = execve("./subscriber3", args, NULL);
                            return res;
                        }
                        else{
                            char * message;
                            message=malloc(50*sizeof(char));
                            signal(SIGINT, sig_handler);//Handle the SIGINT signal --> prevent exit
                            waitpid(subscriber1,&res,0);
                            printf("Exit status of subscriber1 : %d \n",res);
                            sprintf(message,"Exit status of subscriber1 : %d \n",res);
                            Log(message);
                            waitpid(subscriber2,&res,0);
                            printf("Exit status of subscriber2 : %d \n",res);
                            sprintf(message,"Exit status of subscriber2 : %d \n",res);
                            Log(message);
                            waitpid(subscriber3,&res,0);
                            printf("Exit status of subscriber3 : %d \n",res);
                            sprintf(message,"Exit status of subscriber3 : %d \n",res);
                            Log(message);
                            waitpid(mediator,&res,0);
                            printf("Exit status of mediator : %d \n",res);
                            sprintf(message,"Exit status of mediator : %d \n",res);
                            Log(message);
                            waitpid(publisher1,&res,0);
                            printf("Exit status of publisher1 : %d \n",res);
                            sprintf(message,"Exit status of publisher1 : %d \n",res);
                            Log(message);
                            waitpid(publisher2,&res,0);
                            printf("Exit status of publisher2 : %d \n",res);
                            sprintf(message,"Exit status of publisher2 : %d \n",res);
                            Log(message);
							free(message);
							free(args);
                            return 0;


                        }
                    }
                }
            }
        }
    }
}
