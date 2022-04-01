#include "main.h"
#include "logging.h"
#include <stdio.h>


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
	uint8_t * src = (uint8_t *)buf;
	// HAL_StatusTypeDef com_tx_status = HAL_UART_Transmit(&huart3, src, count, 5);
	HAL_StatusTypeDef com_tx_status = HAL_UART_Transmit_DMA(&huart3, src, count);
	if(com_tx_status != HAL_OK)
	{
		Error_Handler();
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

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{

}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{

}
