#include "double_queue.h"

struct dq_node_t node_pool[MAX_NODE_POOL_SIZE];
struct dq_node_t *ptr_pool[MAX_NODE_POOL_SIZE];
static int pool_top = -1;
static struct dq_node_t * _pool_pop();
static void _pool_push(struct dq_node_t *ele);

void dq_init(dq_queue_t* pQueue, int max_size)
{
	pQueue->current = NULL;
	pQueue->last = NULL;
	pQueue->head = NULL;
	pQueue->count = 0;
	pQueue->max = max_size;

	for(int i = 0; i<MAX_NODE_POOL_SIZE; i++ )
	{
		node_pool[i].left_adc_val = 0;
		node_pool[i].right_adc_val = 0;
		node_pool[i].next = 0;
		node_pool[i].prev = 0;
		ptr_pool[i] = &node_pool[i];
	}
	pool_top = MAX_NODE_POOL_SIZE - 1;
}

static struct dq_node_t * _pool_pop()
{
	struct dq_node_t * ele = NULL;
    if (pool_top == -1)
    {
        printf("Underflow!!\r\n");
    }
    else
    {
    	ele = ptr_pool[pool_top];
    	ptr_pool[pool_top] = NULL;
    	pool_top = pool_top - 1;
    }

    return ele;
}

static void _pool_push(struct dq_node_t *ele)
{
	if (pool_top == MAX_NODE_POOL_SIZE - 1)
    {
        printf("Overflow!!\r\n");
    }
    else
    {
    	ele->left_adc_val = 0;
    	ele->right_adc_val = 0;
    	ele->next = 0;
    	ele->prev = 0;
    	pool_top = pool_top + 1;
        ptr_pool[pool_top] = ele;
    }
}

//is list empty
bool dq_isEmpty(dq_queue_t* pQueue) {
	return pQueue->head == NULL;
}

int dq_length(dq_queue_t* pQueue) {
	return pQueue->count;
}


struct dq_node_t* dq_createNode()
{
	struct dq_node_t* node = _pool_pop();
	return node;
}

void dq_deleteNode(struct dq_node_t* node)
{
	_pool_push(node);
}

void dq_insertFirst(dq_queue_t* pQueue, struct dq_node_t* node)
{
	if (pQueue->count >= pQueue->max)
	{
		printf("Queue FULL\r\n");
		return;
	}

	if (node != NULL)
	{
		node->prev = NULL;
		node->next = NULL;

		if (dq_isEmpty(pQueue)) {
			//make it the last link
			pQueue->last = node;
		}
		else {
			//update first prev link
			pQueue->head->prev = node;
		}

		//point it to old first link
		node->next = pQueue->head;

		//point first to new first link
		pQueue->head = node;
		pQueue->count++;
	}
	else {
		printf("dq_insertFirst Error \r\n");
	}
}

void dq_insertLast(dq_queue_t* pQueue, struct dq_node_t* node)
{
	if (pQueue->count >= pQueue->max)
	{
		printf("Queue FULL\r\n");
		return;
	}

	if (node != NULL)
	{
		node->prev = NULL;
		node->next = NULL;

		if (dq_isEmpty(pQueue)) {
			//point first to new first link
			pQueue->head = node;
		}
		else {
			//make link a new last link
			pQueue->last->next = node;

			//mark old last node as prev of new link
			node->prev = pQueue->last;

		}

		//point last to new last node
		pQueue->last = node;
		pQueue->count++;
	}
	else {
		printf("dq_insertLast Error\r\n");
	}
}

struct dq_node_t* dq_peekFirst(dq_queue_t* pQueue)
{
	//save reference to first link
	struct dq_node_t* tempLink = pQueue->head;
	return tempLink;
}

struct dq_node_t* dq_deleteFirst(dq_queue_t* pQueue)
{
	//save reference to first link
	struct dq_node_t* tempLink = pQueue->head;

	//if only one link
	if (pQueue->head->next == NULL) {
		pQueue->last = NULL;
	}
	else {
		pQueue->head->next->prev = NULL;
	}

	pQueue->head = pQueue->head->next;
	pQueue->count--;
	//return the deleted link
	return tempLink;
}

struct dq_node_t* dq_peekLast(dq_queue_t* pQueue)
{
	//save reference to last link
	struct dq_node_t* tempLink = pQueue->last;
	return tempLink;
}

struct dq_node_t* dq_deleteLast(dq_queue_t* pQueue)
{
	//save reference to last link
	struct dq_node_t* tempLink = pQueue->last;

	//if only one link
	if (pQueue->head->next == NULL) {
		pQueue->head = NULL;
	}
	else {
		pQueue->last->prev->next = NULL;
	}

	pQueue->last = pQueue->last->prev;
	pQueue->count--;

	//return the deleted link
	return tempLink;
}
