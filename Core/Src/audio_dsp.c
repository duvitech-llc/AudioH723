#include "main.h"
#include "audio_dsp.h"
#include "double_queue.h"

uint32_t static left_seperation_samples = TKE_K;
uint32_t static right_seperation_samples = TKE_K;

uint32_t process_adc_channel(int ch_id, uint32_t blanker_active, uint16_t adc_read, enum enumAlgoState *adc_state, blank_t **pADC_blanker, void* pAudqueue)
{
	uint16_t adc_value = adc_read;
	uint16_t working_val = 0;
	bool bClusterTick = false;

	// blanking
	switch (*adc_state) {
	case NORMAL_OPERATION:
		if (blanker_active > 0) {

			// start blanking operation
			if ((*pADC_blanker) != NULL) {
				printf("<<<<<<<<<<<< NORMAL OPERATION CURRENT Blanker POINTER NOT NULL <<<<<<<<<<<< \r\n");
			}

			if(ch_id == 0)
			{
				// printf("TKE: %i CNT: %i\r\n", TKE_K, left_seperation_samples);
				if(left_seperation_samples < TKE_K){
					bClusterTick = true;
				}
			}
			else
			{
				// printf("TKE: %i CNT: %i\r\n", TKE_K, right_seperation_samples);
				if(right_seperation_samples < TKE_K){
					bClusterTick = true;
				}

			}

			// Check M0 and fix up samples
			if(!bClusterTick && TKS_M0 > 0){
				// get blanker
				struct dq_node_t* temp_blnk = dq_peekFirst((dq_queue_t*)pAudqueue);
				if (temp_blnk) {
					int mv_cnt = 0;
					// walk the samples back
					for (mv_cnt = 1; mv_cnt < TKS_M0; mv_cnt++)
					{
						if (temp_blnk->next)
							temp_blnk = temp_blnk->next;
						else
							break;
					}

					(*pADC_blanker) = ch_id == 0? (blank_t*)temp_blnk->d2:(blank_t*)temp_blnk->d3;

					if(*pADC_blanker == NULL){
						// create blanker
						(*pADC_blanker) = (blank_t*) malloc(sizeof(blank_t));
						(*pADC_blanker)->tks_val = ch_id == 0?(uint16_t)(temp_blnk->d0): (uint16_t)(temp_blnk->d1);
						(*pADC_blanker)->tks_cnt = mv_cnt;
						(*pADC_blanker)->cr_cnt = 0;
						(*pADC_blanker)->blank_state = BLANKING_START;
						(*pADC_blanker)->correct_state = CORRECTING_START;
					}else{
						(*pADC_blanker)->tks_cnt += mv_cnt;
						(*pADC_blanker)->cr_cnt = 0;
					}

					// fix up M0 samples
					while (temp_blnk)
					{
						if(ch_id == 0)
						{
							temp_blnk->d0 = (*pADC_blanker)->tks_val;
							temp_blnk->d2 = (size_t)(*pADC_blanker);
						}
						else
						{
							temp_blnk->d1 = (*pADC_blanker)->tks_val;
							temp_blnk->d3 = (size_t)(*pADC_blanker);
						}

						temp_blnk = temp_blnk->prev;
					}

				}
			}else if(bClusterTick){
				// handle cluster tick
				int mv_cnt = 0;
				struct dq_node_t* temp_blnk = dq_peekFirst((dq_queue_t*)pAudqueue);
				do{
					(*pADC_blanker) = ch_id == 0? (blank_t*)temp_blnk->d2:(blank_t*)temp_blnk->d3;
					mv_cnt++;

					if(*pADC_blanker != NULL)
						break; // found the blanker

					if(temp_blnk->next)
						temp_blnk = temp_blnk->next;
					else
						break; // no more to process

				}while(*pADC_blanker == NULL);

				if(*pADC_blanker == NULL){
					// no blanker found error
					printf("No Blanker found\r\n");
				}else{
					(*pADC_blanker)->blank_state = BLANKING_START;
					(*pADC_blanker)->correct_state = CORRECTING_START;
					(*pADC_blanker)->tks_cnt += mv_cnt;
					(*pADC_blanker)->cr_cnt = 0;

					// fix up samples in between
					while (temp_blnk)
					{
						if(ch_id == 0)
						{
							temp_blnk->d0 = (*pADC_blanker)->tks_val;
							temp_blnk->d2 = (size_t)(*pADC_blanker);
						}
						else
						{
							temp_blnk->d1 = (*pADC_blanker)->tks_val;
							temp_blnk->d3 = (size_t)(*pADC_blanker);
						}

						temp_blnk = temp_blnk->prev;
					}

					adc_value = (*pADC_blanker)->tks_val;
				}

				*adc_state = BLANKING_OPERATION;

			}else{

				// create blanker
				(*pADC_blanker) = (blank_t*) malloc(sizeof(blank_t));
				(*pADC_blanker)->tks_val = adc_value;
				(*pADC_blanker)->cr_cnt = 0;
				(*pADC_blanker)->tks_cnt = 0;
				(*pADC_blanker)->blank_state = BLANKING_START;
				(*pADC_blanker)->correct_state = CORRECTING_START;

			}

			adc_value = (*pADC_blanker)->tks_val;
			*adc_state = BLANKING_OPERATION;
		} else {
			// do nothing
			if(ch_id == 0)
			{
				left_seperation_samples++;
			}
			else
			{
				right_seperation_samples++;
			}
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
			(*pADC_blanker)->tks_cnt++;

			adc_value = (*pADC_blanker)->tks_val; // make all the starting value so we can just add the offset times count to it
		} else {
			// end blanking operation
			if ((*pADC_blanker) == NULL) {
				printf("<<<<<<<<<<<< BLANKING OPERATION CURRENT Blanker IS NULL <<<<<<<<<<<< \r\n");
				*adc_state = NORMAL_OPERATION;
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

			//printf("BLANK C: %i M0: %i M1: %i D: %i S: %i\r\n", (*pADC_blanker)->tks_cnt, (*pADC_blanker)->tks_val, (*pADC_blanker)->tke_val, (*pADC_blanker)->tks_dir, (*pADC_blanker)->tks_step);

			if(ch_id == 0)
			{
				left_seperation_samples = 0;
			}
			else
			{
				right_seperation_samples = 0;
			}

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


			//printf("CR CURR: %i TOT: %i O: %i N: %i M0: %i M1: %i D: %i S: %i\t\r\n", (*pDAC_blanker)->cr_cnt, (*pDAC_blanker)->tks_cnt, dac_read, dac_val, (*pDAC_blanker)->tks_val, (*pDAC_blanker)->tke_val, (*pDAC_blanker)->tks_dir, (*pDAC_blanker)->tks_step);

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
