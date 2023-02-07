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

#define DAC_SEPARATION 1042U		// 10ms 9.6us
#define  TKS_M0 5U				// pre start samples
#define  TKE_K 20U				// samples in between ticks that constitute a cluster tick
#define  B_DEL 12U				// blanker signal delay in samples

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

uint16_t process_left_adc_channel(uint32_t blanker_active, uint16_t adc_read, enum enumAlgoState *adc_state, blank_t **pADC_blanker, void* pAudqueue);
uint16_t process_right_adc_channel(uint32_t blanker_active, uint16_t adc_read, enum enumAlgoState *adc_state, blank_t **pADC_blanker, void* pAudqueue);

uint16_t process_dac_channel(enum enumAlgoState *dac_state, uint16_t dac_read, blank_t **pDAC_blanker);

#ifdef __cplusplus
}
#endif

#endif // __AUDIO_DSP_H_
