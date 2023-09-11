#ifndef AUDIO_DSP_H
#define AUDIO_DSP_H

#ifdef __cplusplus
extern "C" {
#endif

// Include necessary libraries and header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Define your signal processing functions, buffers, and any necessary data structures

// Define the sample rate and bit depth
#define AUDIO_SAMPLE_RATE 96000
#define AUDIO_BIT_DEPTH 16

// Define a macro to enable or disable the passthrough
// #define AUDIO_DSP_PASSTHROUGH_ENABLED 1  // Set to 1 to enable, 0 to disable
// Define the buffer duration in milliseconds max is 50ms
#define DAC_SEPARATION_MS 10
#define  TKS_M0 5U				// pre start samples
#define  TKE_K 20U				// samples in between blanker pulses that constitute a cluster correction
#define  B_DEL 12U				// blanker signal delay in samples

#define DAC_SEPARATION ((AUDIO_SAMPLE_RATE / 1000) * DAC_SEPARATION_MS)

enum enumAlgoState {
	NORMAL_OPERATION, BLANKING_OPERATION, CORRECTING_OPERATION, BLANKING_START, BLANKING_COMPLETE, CORRECTING_START, CORRECTING_COMPLETE, BLANKER_UNUSED
};

enum enumSampleState {
	SAMPLE_VALID, SAMPLE_ERROR, SAMPLE_UNUSED
};

void audio_dsp_init();
void enable_dac();

#ifdef __cplusplus
}
#endif

#endif // __AUDIO_DSP_H_

