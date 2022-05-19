#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "fifo_buff.h"


//This initializes the FIFO structure with the given buffer and size
void fifo_init(fifo_t * f){
    f->count = 0;
    f->front = NULL;
    f->rear = NULL;
}

bool fifo_isEmpty(fifo_t * f)
{
	return (f->rear == NULL);
}

uint32_t fifo_read(fifo_t * f)
{
	pkt_t *tmp;
    int n = f->front->data;
    tmp = f->front;
    f->front = f->front->next;
    f->count--;
    free(tmp);
    return(n);
}

void fifo_write(fifo_t * f, uint32_t value)
{
    if (f->count < FULL)
    {
    	pkt_t *tmp;
        tmp = malloc(sizeof(pkt_t));
        tmp->data = value;
        tmp->next = NULL;
        if(!fifo_isEmpty(f))
        {
            f->rear->next = tmp;
            f->rear = tmp;
        }
        else
        {
            f->front = f->rear = tmp;
        }
        f->count++;
    }
    else
    {
        printf("List is full\n");
    }
}

void fifo_print(pkt_t *head)
{
    if(head == NULL)
    {
        printf("NULL\n");
    }
    else
    {
        printf("%ld\n", head -> data);
        fifo_print(head->next);
    }
}
