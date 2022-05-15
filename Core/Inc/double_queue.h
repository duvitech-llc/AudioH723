#pragma once
#ifndef __DOUBLE_QUEUE_H_
#define __DOUBLE_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

    struct dq_node_t {
    	uint16_t left_adc_Val;
    	uint16_t right_adc_val;
        size_t pLeft_blanker;
        size_t pRight_blanker;
        struct dq_node_t* next;
        struct dq_node_t* prev;
    };

    typedef struct {
        struct dq_node_t* head;
        struct dq_node_t* last;
        struct dq_node_t* current;
        int count;
        int max;
    } dq_queue_t;

    void dq_init(dq_queue_t* queue, int max_size);
    bool dq_isEmpty(dq_queue_t* pQueue);
    int dq_length(dq_queue_t* pQueue);
    struct dq_node_t* dq_createNode();
    void dq_insertFirst(dq_queue_t* pQueue, struct dq_node_t* node);
    void dq_insertLast(dq_queue_t* pQueue, struct dq_node_t* node);
    struct dq_node_t* dq_peekFirst(dq_queue_t* pQueue);
    struct dq_node_t* dq_peekLast(dq_queue_t* pQueue);
    struct dq_node_t* dq_deleteFirst(dq_queue_t* pQueue);
    struct dq_node_t* dq_deleteLast(dq_queue_t* pQueue);

#ifdef __cplusplus
}
#endif

#endif // __DOUBLE_QUEUE_H_
