#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "fifo_buff.h"

struct pkt_t fifo_pool[MAX_FIFO_POOL_SIZE];
struct pkt_t *pfifo_pool[MAX_FIFO_POOL_SIZE];
static int fifo_top = -1;
static bool fifoIsInitialized = false;

static struct pkt_t * _fifo_pop();
static void _fifo_push(struct pkt_t *ele);

//This initializes the FIFO structure with the given buffer and size
void fifo_init(fifo_t * f){
    f->count = 0;
    f->front = NULL;
    f->rear = NULL;

    if(!fifoIsInitialized)
    {
        for(int i = 0; i<MAX_FIFO_POOL_SIZE; i++ )
        {
            fifo_pool[i].data = 0;
            fifo_pool[i].next = 0;
            pfifo_pool[i] = &fifo_pool[i];
        }
        fifo_top = MAX_FIFO_POOL_SIZE - 1;
        fifoIsInitialized = true;
    }
}

static struct pkt_t * _fifo_pop()
{
	struct pkt_t * ele = NULL;
    if (fifo_top == -1)
    {
        printf("FIFO Underflow!!\r\n");
    }
    else
    {
    	ele = pfifo_pool[fifo_top];
    	pfifo_pool[fifo_top] = NULL;
    	fifo_top = fifo_top - 1;
    }

    return ele;
}

static void _fifo_push(struct pkt_t *ele)
{
	if (fifo_top == MAX_FIFO_POOL_SIZE - 1)
    {
        printf("FIFO Overflow!!\r\n");
    }
    else
    {
    	ele->data = 0;
    	ele->next = 0;
    	fifo_top = fifo_top + 1;
        pfifo_pool[fifo_top] = ele;
    }
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
    _fifo_push(tmp);
    return(n);
}

void fifo_write(fifo_t * f, uint32_t value)
{
    if (f->count < FIFO_FULL)
    {
    	pkt_t *tmp;
        tmp = _fifo_pop();
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

struct pkt_t* fifo_createEle()
{
	struct pkt_t* ele = _fifo_pop();
	return ele;
}

void fifo_deleteEle(struct pkt_t* ele)
{
	_fifo_push(ele);
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
