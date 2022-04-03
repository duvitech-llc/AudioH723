#pragma once
#ifndef __AUDIO_DSP_H_
#define __AUDIO_DSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


#define DAC_SEPARATION 256U		// 10ms = 256
#define BLANKER_QUEUE_SIZE 64U	// blanker queue size shouldn't be smaller then 1/4 separation
#define  TKS_M0 0U				// pre start samples
#define  TKE_K 12U				// samples in between ticks that constitute a cluster tick

enum enumAlgoState {
	NORMAL_OPERATION, BLANKING_OPERATION, CORRECTING_OPERATION, BLANKING_START, BLANKING_COMPLETE, CORRECTING_START, CORRECTING_COMPLETE
};

typedef struct {
        uint16_t tks_step;
        int16_t tks_val;
        int16_t tke_val;
        uint16_t tks_cnt;
        uint16_t tks_dir;
        uint16_t cr_cnt;
        int16_t unused;
        enum enumAlgoState blank_state;
        enum enumAlgoState correct_state;
    } blank_t;

uint32_t process_adc_channel(int ch_id, uint32_t blanker_active, uint16_t adc_read, enum enumAlgoState *adc_state, blank_t **pADC_blanker, void* pAudqueue);
uint16_t process_dac_channel(enum enumAlgoState *dac_state, uint16_t dac_read, blank_t **pDAC_blanker);

#ifdef __cplusplus
}
#endif

#endif // __AUDIO_DSP_H_
