#include "main.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

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

    // Copy the data from buf to tx_buffer
    HAL_UART_Transmit(&huart3, buf, count, 50);

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
	bPrintfTransferComplete = true;
}

bool isInitialized(){
	return bInit;
}

void print_stats(){

}

void processUartTxData(){
}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{

}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	bPrintfTransferComplete = true;
}


