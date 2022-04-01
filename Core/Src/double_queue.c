#include "double_queue.h"

void dq_init(dq_queue_t* pQueue, int max_size)
{
	pQueue->current = NULL;
	pQueue->last = NULL;
	pQueue->head = NULL;
	pQueue->count = 0;
	pQueue->max = max_size;
}

//is list empty
bool dq_isEmpty(dq_queue_t* pQueue) {
	return pQueue->head == NULL;
}

int dq_length(dq_queue_t* pQueue) {
	return pQueue->count;
}

void dq_insertFirst(dq_queue_t* pQueue, size_t data, size_t size)
{
	if (pQueue->count >= pQueue->max)
	{
		printf("Queue FULL\r\n");
		return;
	}

	//create a link
	struct dq_node_t* link = (struct dq_node_t*)malloc(sizeof(struct dq_node_t));

	if (link != NULL)
	{
		link->data = data;
		link->data_len = size;
		link->prev = NULL;
		link->next = NULL;

		if (dq_isEmpty(pQueue)) {
			//make it the last link
			pQueue->last = link;
		}
		else {
			//update first prev link
			pQueue->head->prev = link;
		}

		//point it to old first link
		link->next = pQueue->head;

		//point first to new first link
		pQueue->head = link;
		pQueue->count++;
	}
	else {
		printf("dq_insertFirst Error \r\n");
	}
}

void dq_insertLast(dq_queue_t* pQueue, size_t data, size_t size)
{
	if (pQueue->count >= pQueue->max)
	{
		printf("Queue FULL\r\n");
		return;
	}

	//create a link
	struct dq_node_t* link = (struct dq_node_t*)malloc(sizeof(struct dq_node_t));
	if (link != NULL)
	{
		link->data = data;
		link->data_len = size;
		link->prev = NULL;
		link->next = NULL;

		if (dq_isEmpty(pQueue)) {
			//point first to new first link
			pQueue->head = link;
		}
		else {
			//make link a new last link
			pQueue->last->next = link;

			//mark old last node as prev of new link
			link->prev = pQueue->last;

		}

		//point last to new last node
		pQueue->last = link;
		pQueue->count++;
	}
	else {
		printf("dq_insertLast Error\r\n");
	}
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
