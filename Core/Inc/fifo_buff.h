#ifndef __FIFO_BUFFER_H_
#define __FIFO_BUFFER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define FULL 50

struct pkt_t {
	uint32_t data;
    struct pkt_t *next;
};
typedef struct pkt_t pkt_t;
typedef struct
{
    int count;
    pkt_t *front;
    pkt_t *rear;
} fifo_t;

void fifo_init(fifo_t * f);
bool fifo_isEmpty(fifo_t * f);
uint32_t fifo_read(fifo_t * f);
void fifo_write(fifo_t * f, uint32_t value);
void fifo_print(pkt_t *head);

#ifdef __cplusplus
}
#endif

#endif // __FIFO_BUFFER_H_
