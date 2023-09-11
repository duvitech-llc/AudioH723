#include "audio_dsp.h"
#include "main.h"
#include "double_queue.h"


// Define your signal processing functions, buffers, and any necessary data structures

// Implement your signal processing functions

// Include the extern declarations for tx_buf and rx_buf
extern uint16_t rx_buf[4];
extern uint16_t tx_buf[4];
extern TIM_HandleTypeDef htim23;
extern volatile uint32_t processing_time_us;

volatile unsigned int sample_count = 0;
volatile bool dac_enabled = false;

dq_queue_t audio_queue;

void audio_dsp_init(){
	dq_init(&audio_queue, DAC_SEPARATION + 10);
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai) {

}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai) {

}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
#ifdef AUDIO_DSP_PASSTHROUGH_ENABLED
    // Passthrough is enabled, so copy data from rx_buf to tx_buf
    tx_buf[0] = rx_buf[0];
    tx_buf[1] = rx_buf[1];
    ((GPIO_TypeDef*) CORRECT_L_GPIO_Port)->BSRR = (uint32_t) CORRECT_L_Pin << 16U; // reset pin
#else
    // Create an AudioSample from rx_buf and add it to the list
    struct dq_node_t *sample = dq_createNode();
    sample->left_adc_val = rx_buf[0];
    sample->right_adc_val = rx_buf[1];

	dq_insertFirst(&audio_queue, sample);
    // If DAC is enabled, output data
    if (dac_enabled) {
        // Get the first sample from the list
		struct dq_node_t* temp = dq_deleteLast(&audio_queue);

		tx_buf[0] = temp->left_adc_val;
		tx_buf[1] = temp->right_adc_val;
		dq_deleteNode(temp);
    }
    else {
        if (sample_count >= DAC_SEPARATION-1)
        {
            dac_enabled = true;
            ((GPIO_TypeDef*) sample_timing_GPIO_Port)->BSRR = (uint32_t) sample_timing_Pin << 16U; // reset pin
        }
    }

#endif
    sample_count++;
}

void enable_dac(){
	dac_enabled = true;
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) {
((GPIO_TypeDef*) CORRECT_L_GPIO_Port)->BSRR = CORRECT_L_Pin;  // timing start

#ifdef AUDIO_DSP_PASSTHROUGH_ENABLED
    // Passthrough is enabled, so copy data from rx_buf to tx_buf
    tx_buf[2] = rx_buf[2];
    tx_buf[3] = rx_buf[3];
    ((GPIO_TypeDef*) CORRECT_L_GPIO_Port)->BSRR = (uint32_t) CORRECT_L_Pin << 16U; // reset pin
#else
    // Create an AudioSample from rx_buf and add it to the list
    struct dq_node_t *sample = dq_createNode();
    sample->left_adc_val = rx_buf[2];
    sample->right_adc_val = rx_buf[3];

	dq_insertFirst(&audio_queue, sample);
    // If DAC is enabled, output data
    if (dac_enabled) {
        // Get the first sample from the list
		struct dq_node_t* temp = dq_deleteLast(&audio_queue);

		tx_buf[2] = temp->left_adc_val;
		tx_buf[3] = temp->right_adc_val;
		dq_deleteNode(temp);
    }
    else {
        if (sample_count >= DAC_SEPARATION-1)
        {
            dac_enabled = true;
            ((GPIO_TypeDef*) sample_timing_GPIO_Port)->BSRR = (uint32_t) sample_timing_Pin << 16U; // reset pin
        }
    }

#endif
    sample_count++;
    ((GPIO_TypeDef*) CORRECT_L_GPIO_Port)->BSRR = (uint32_t) CORRECT_L_Pin << 16U; // reset pin
}
