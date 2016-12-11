#include "circular_buffer.h"


int circular_buffer_init(circular_buffer *circ_buff,  int buffer_max_length)
{
    printf("CIRCULAR BUFFER INIT\n");
    fflush(stdout);
    circ_buff->buffer_max_length=buffer_max_length;

    circ_buff->buffer =(char *) malloc(buffer_max_length * sizeof(char));
    if(circ_buff->buffer == NULL){
        printf("ERROR BUFFER INIT MALLOC\n");
        fflush(stdout);
        return -1;
    }

    circ_buff->buffer_length = 0;
    circ_buff->head = 0;
    circ_buff->tail = 0;
}

void circular_buffer_free(circular_buffer *circ_buff)
{
    printf("CIRCULAR BUFFER FREE\n");
    fflush(stdout);
    free(circ_buff->buffer);
}

int write_buff(circular_buffer *circ_buff, const char item)
{
    printf("CIRCULAR BUFFER WRITE\n");
    fflush(stdout);
    if(circ_buff->buffer_length == circ_buff->buffer_max_length){
        printf("BUFFER IS FULL\n");
        fflush(stdout);
        return -1;
    }

    circ_buff->buffer[circ_buff->head]=item;
    circ_buff->head=(circ_buff->head+1)%circ_buff->buffer_max_length;

    circ_buff->buffer_length++;
}

int read_buff(circular_buffer *circ_buff, char *item)
{
    printf("CIRCULAR BUFFER READ\n");
    fflush(stdout);
    if(circ_buff->buffer_length == 0){
        printf("BUFFER IS EMPTY\n");
        fflush(stdout);
        return -1;
    }

    *item=circ_buff->buffer[circ_buff->tail];
    circ_buff->tail=(circ_buff->tail+1)%circ_buff->buffer_max_length;
    circ_buff->buffer_length--;
}
