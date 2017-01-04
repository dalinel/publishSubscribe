
#ifndef H_GL_CIRCBUFF
#define H_GL_CIRCBUFF
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct circular_buffer
{
    char * buffer;     // data buffer
    int buffer_max_length; // end of data buffer
    int head;       // pointer to head
    int tail;       // pointer to tail
    int buffer_length;     // number of items in the buffer

} circular_buffer;

int circular_buffer_init(circular_buffer *circ_buff,  int buffer_max_length);
void circular_buffer_free(circular_buffer *circ_buff);
int write_buff(circular_buffer *circ_buff, const char item);
int read_buff(circular_buffer *circ_buff, char *item);


#endif
