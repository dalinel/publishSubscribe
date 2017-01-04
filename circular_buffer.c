#include "circular_buffer.h"

void LogB(char *message)
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

// init a circular buffer of max length buffer_max_length
int circular_buffer_init(circular_buffer *circ_buff,  int buffer_max_length)
{
    printf("CIRCULAR BUFFER INIT\n");
    fflush(stdout); //print immediatly
    LogB("CIRCULAR BUFFER INIT\n");
    circ_buff->buffer_max_length=buffer_max_length;

    circ_buff->buffer =(char *) malloc(buffer_max_length * sizeof(char));
    if(circ_buff->buffer == NULL){
        printf("ERROR BUFFER INIT MALLOC\n");
        fflush(stdout);
        LogB("ERROR BUFFER INIT MALLOC\n");
        return -1;
    }

    circ_buff->buffer_length = 0;
    circ_buff->head = 0;
    circ_buff->tail = 0;
    return 0;
}

// Free the allocated memory
void circular_buffer_free(circular_buffer *circ_buff)
{
    printf("CIRCULAR BUFFER FREE\n");
    fflush(stdout);
    free(circ_buff->buffer);
    LogB("CIRCULAR BUFFER FREE\n");

}


int write_buff(circular_buffer *circ_buff, const char item)
{
    printf("CIRCULAR BUFFER WRITE\n");
    fflush(stdout);
    LogB("CIRCULAR BUFFER WRITE\n");
    if(circ_buff->buffer_length == circ_buff->buffer_max_length){
        printf("BUFFER IS FULL\n");
        fflush(stdout);
        LogB("BUFFER IS FULL\n");
        return -1;
    }

    circ_buff->buffer[circ_buff->head]=item;
    circ_buff->head=(circ_buff->head+1)%circ_buff->buffer_max_length;

    circ_buff->buffer_length++;

    return 0;
}

int read_buff(circular_buffer *circ_buff, char *item)
{
    printf("CIRCULAR BUFFER READ\n");
    fflush(stdout);
    LogB("CIRCULAR BUFFER READ\n");
    if(circ_buff->buffer_length == 0){
        printf("BUFFER IS EMPTY\n");
        fflush(stdout);
        LogB("BUFFER IS EMPTY\n");
        return -1;
    }

    *item=circ_buff->buffer[circ_buff->tail];
    circ_buff->tail=(circ_buff->tail+1)%circ_buff->buffer_max_length;
    circ_buff->buffer_length--;

    return 0;
}
