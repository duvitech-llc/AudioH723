#include "main.h"
#include "double_queue.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

static dq_queue_t logging_queue;
static bool bInit = false;
volatile bool bPrintfTransferComplete = false;
uint8_t tx_buffer[256];

#ifdef __GNUC__
	/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
		 set to 'Yes') calls __io_putchar() */
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


#ifdef __GNUC__
int _write(int fd, const void *buf, size_t count){
	UNUSED(fd);
	if(bInit){
		char *msg = (char*) malloc(count);
		memcpy(msg, buf, count);
		struct dq_node_t* msg_node = dq_createNode();
		msg_node->pLeft_blanker = (uint32_t)msg;
		msg_node->pRight_blanker = count;
		dq_insertLast(&logging_queue, msg_node);
	}else{
		uint8_t * src = (uint8_t *)buf;
		HAL_StatusTypeDef com_tx_status = HAL_UART_Transmit(&huart3, src, count, 10);
		if(com_tx_status != HAL_OK)
		{
			Error_Handler();
		}
	}
	return count;
}
#else
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
	//HAL_UART_Transmit(&huart5, (uint8_t *)&ch, 1,10);
	UART_putChar( (uint8_t) ch);
  return ch;
}
#endif

void init_dma_logging()
{
	dq_init(&logging_queue, 100);
	bInit = true;
	bPrintfTransferComplete = true;
}

bool isInitialized(){
	return bInit;
}

void print_stats(){
	printf("Print LOG SIZE: %i \r\n", dq_length(&logging_queue));
}

void processUartTxData(){
	if(bPrintfTransferComplete && !dq_isEmpty(&logging_queue)){
		struct dq_node_t* temp = dq_deleteFirst(&logging_queue);
		if(temp != NULL){
			bPrintfTransferComplete = false;
			memcpy(tx_buffer,(uint8_t*)temp->pLeft_blanker, temp->pRight_blanker);
			uint16_t size = (uint16_t)temp->pRight_blanker;
			HAL_StatusTypeDef com_tx_status = HAL_UART_Transmit_DMA(&huart3, tx_buffer, size);
			if(com_tx_status != HAL_OK)
			{
				Error_Handler();
			}
			free((uint8_t*)temp->pLeft_blanker);
			free(temp);
		}
	}
}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{

}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	bPrintfTransferComplete = true;
}


