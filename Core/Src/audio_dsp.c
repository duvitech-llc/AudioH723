#include "main.h"
#include "audio_dsp.h"

uint32_t process_adc_channel(uint32_t blanker_active, uint16_t adc_read, enum enumAlgoState *adc_state, blank_t **pADC_blanker)
{
	uint16_t adc_value = adc_read;
	uint16_t working_val = 0;
	bool bClusterTick = false;

	// blanking
	switch (*adc_state) {
	case NORMAL_OPERATION:
		if (blanker_active > 0) {
			// start blanking operation

			if(bClusterTick)
			{

			}
			else
			{
				if ((*pADC_blanker) != NULL) {
					printf("<<<<<<<<<<<< NORMAL OPERATION CURRENT Blanker POINTER NOT NULL <<<<<<<<<<<< \r\n");
				}
				(*pADC_blanker) = (blank_t*) malloc(sizeof(blank_t));
				(*pADC_blanker)->tks_val = adc_value;
				(*pADC_blanker)->cr_cnt = 0;
				(*pADC_blanker)->m1_cnt = -1;
				(*pADC_blanker)->blank_state = BLANKING_START;
				(*pADC_blanker)->correct_state = CORRECTING_START;

				// TODO: fix up previous samples M0

				(*pADC_blanker)->tks_cnt = TKS_M0;  // currently 0
			}
			*adc_state = BLANKING_OPERATION;
		} else {
			// do nothing
		}
		break;
	case BLANKING_OPERATION:
		if (blanker_active > 0) {
			// Update blanking structure
			if ((*pADC_blanker) == NULL) {
				printf("<<<<<<<<<<<< BLANKING OPERATION CURRENT Blanker IS NULL <<<<<<<<<<<< \r\n");
				*adc_state = NORMAL_OPERATION;
				break;
			}

			(*pADC_blanker)->blank_state = BLANKING_OPERATION;

			if ((*pADC_blanker)->m1_cnt >= 0) {
				// blanker signal during extended blanking reset m1 count
				(*pADC_blanker)->m1_cnt = -1;
			}

			(*pADC_blanker)->tks_cnt++;
			adc_value = (*pADC_blanker)->tks_val; // make all the starting value so we can just add the offset times count to it
		} else {
			// end blanking operation
			if ((*pADC_blanker) == NULL) {
				printf("<<<<<<<<<<<< BLANKING OPERATION CURRENT Blanker IS NULL <<<<<<<<<<<< \r\n");
				*adc_state = NORMAL_OPERATION;
				break;
			}

			if ((*pADC_blanker)->m1_cnt < 0 && TKE_M1 > 0) {
				// extend blanking
				(*pADC_blanker)->m1_cnt = TKE_M1;
				(*pADC_blanker)->tks_cnt++;
				adc_value = (*pADC_blanker)->tks_val; // make all the starting value so we can just add the offset times count to it
				(*pADC_blanker)->m1_cnt--;
				break;
			} else if ((*pADC_blanker)->m1_cnt > 0) {
				// currently in extended blanking continue blanking for m1 samples
				(*pADC_blanker)->tks_cnt++;
				adc_value = (*pADC_blanker)->tks_val; // make all the starting value so we can just add the offset times count to it
				(*pADC_blanker)->m1_cnt--;
				break;
			}

			(*pADC_blanker)->tks_cnt++;
			(*pADC_blanker)->tke_val = adc_value;
			if ((*pADC_blanker)->tks_cnt < 2) {
				(*pADC_blanker)->tks_dir = 1;
				(*pADC_blanker)->tks_step = 0;
			} else {
				// calculate step voltage

				if ((*pADC_blanker)->tke_val > (*pADC_blanker)->tks_val) {
					(*pADC_blanker)->tks_dir = 1;
					working_val = (*pADC_blanker)->tke_val
							- (*pADC_blanker)->tks_val;
				} else {
					(*pADC_blanker)->tks_dir = 0;
					working_val = (*pADC_blanker)->tks_val
							- (*pADC_blanker)->tke_val;
				}

				(*pADC_blanker)->tks_step = floor(
						(working_val / ((*pADC_blanker)->tks_cnt)));

			}

			// printf("BLANK C: %i M0: %i M1: %i D: %i S: %i\r\n", (*pADC_blanker)->tks_cnt, (*pADC_blanker)->tks_val, (*pADC_blanker)->tke_val, (*pADC_blanker)->tks_dir, (*pADC_blanker)->tks_step);

			(*pADC_blanker)->blank_state = BLANKING_COMPLETE;
			*adc_state = NORMAL_OPERATION;
		}
		break;
	default:
		printf(
				"<<<<<<<<<<<< ADC INVALID STATE BLANKER HIGH <<<<<<<<<<<< \r\n");
		break;
	}

	// write adc samples to fifo
	return (adc_value & 0xffff);
}

uint16_t process_dac_channel(enum enumAlgoState *dac_state, uint16_t dac_read, blank_t **pDAC_blanker)
{

	uint16_t dac_val = 0;

	// read next adc sample from fifo to ouput
	// decompose into DAC value and Blanker ID

	uint16_t next_step_val = 0;
	dac_val = dac_read;

	switch (*dac_state) {
	case NORMAL_OPERATION:
		if ((*pDAC_blanker) != NULL ) {
			// entering CORRECTING_OPERATION mode
			// printf("CORRECT C: %i M0: %i M1: %i D: %i S: %i\t\r\n", (*pDAC_blanker)->tks_cnt, (*pDAC_blanker)->tks_val, (*pDAC_blanker)->tke_val, (*pDAC_blanker)->tks_dir, (*pDAC_blanker)->tks_step);

			(*pDAC_blanker)->cr_cnt = 0;
			(*pDAC_blanker)->correct_state = CORRECTING_OPERATION;
			// set DAC value to M0 value
			*dac_state = CORRECTING_OPERATION;
		} else {
			// nothng to do
		}
		break;
	case CORRECTING_OPERATION:
		// CORRECTING_OPERATION dac data
		(*pDAC_blanker)->cr_cnt++;

		if ((*pDAC_blanker)->cr_cnt < (*pDAC_blanker)->tks_cnt) {
			next_step_val =
					((*pDAC_blanker)->tks_step * (*pDAC_blanker)->cr_cnt);
			if ((*pDAC_blanker)->tks_dir == 1) {
				// up direction
				dac_val += next_step_val;

			} else {
				// down direction
				dac_val -= next_step_val;

			}

		} else {
			// end correcting operation free the blanker structure memory
			(*pDAC_blanker)->correct_state = CORRECTING_COMPLETE;

			*dac_state = NORMAL_OPERATION;
		}

		break;
	default:
		printf("<<<<<<<<<<<< SHOULD NEVER SEE THIS <<<<<<<<<<<< \r\n");
		break;
	}

	return dac_val;
}
