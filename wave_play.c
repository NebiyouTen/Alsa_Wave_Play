/*
 * 
 * Wave.c
 * A simple program to parse wav and play files .
 * 
 * Adopted from 
 * http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
 * 
 * */
 
// Tell Alsa to use newer version of its API

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

#include <stdio.h>
#include "wave_play.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define READ_in_CHUNKS 1
#define HEADER_SIZE 44
#define NUM_BYTES 4

// struct for the
struct WAV_HEADER wave_header; 
// file ptr
FILE *file_ptr; 
unsigned char temp_buffer[4];
unsigned char temp_buffer_2[2];

/*
 * @Function to read a wav file and play it using
 * the PCM handle passed. 
 * 
 * Arguments: file_name: name of the wav file to be played
 *            handle   : Pointer to a PCM 
 * 			  frames   : Number of frames to be written in the buffer 
 * 						 , if chunk mode is used
 *  
 * Returns: integer:
 * 
 * */

int read_wave_and_play(char *file_name, snd_pcm_t *handle, 
			snd_pcm_uframes_t frames){
	
	int ret, data_size; 
	long number_of_samples, size_of_samples, i = 0;
	long low_limit=0l, high_limit = 0l;
	float duration_in_sec;
	char * data_temp_buffer;
	
	
	fprintf(stdout,"Wave file reading started \n");
	
	if ( (file_ptr = fopen(file_name, "rb") ) == NULL){
			printf("error in opening file");
			exit(1);
	}
	
	// read header data
	ret = fread(wave_header.riff, sizeof(wave_header.riff),1,file_ptr);
	printf("RIFF header values %s .\n",wave_header.riff);
	// read file size
	ret = fread(temp_buffer, sizeof(temp_buffer), 1, file_ptr);
	wave_header.file_size = temp_buffer[0] | (temp_buffer[1] << 8) | \
	                (temp_buffer[2] << 16) | (temp_buffer[3] << 24);
	printf("Wav file size %u KB.\n",wave_header.file_size / 1024);
	// get wav header
	ret = fread(wave_header.wave, sizeof(wave_header.wave),1, file_ptr);
	printf("Wave marker is %s .\n",wave_header.wave);
	// fmt marker
	ret = fread(wave_header.fmt_chunk_marker, \
				sizeof(wave_header.fmt_chunk_marker), 1, file_ptr);
	printf("Fmt chunk marker is %s .\n",wave_header.fmt_chunk_marker);
	// read fmt length  
	ret = fread(temp_buffer, sizeof(temp_buffer), 1, file_ptr);
	wave_header.length_of_fmt = temp_buffer[0] | (temp_buffer[1] << 8) | \
	                    (temp_buffer[2] << 16) | (temp_buffer[3] << 24);
	printf("Length of fmt header %u B.\n", wave_header.length_of_fmt);
	// get format type
	ret = fread(temp_buffer_2, sizeof(temp_buffer_2), 1, file_ptr);
	wave_header.format_type = temp_buffer_2[0] | temp_buffer_2[1] << 8 ;
	switch(wave_header.format_type){
		case (1): printf("Format type PCM \n");
				 break;
		default:  printf("Format type other \n");
				 break;
	}
	// get channels
	ret = fread(temp_buffer_2, sizeof(temp_buffer_2), 1, file_ptr);
	wave_header.channels = temp_buffer_2[0] | temp_buffer_2[1] << 8 ;
	printf("Channels %u .\n", wave_header.channels);
	// read sample rate
	ret = fread(temp_buffer, sizeof(temp_buffer), 1, file_ptr);
	wave_header.sample_rate = temp_buffer[0] | (temp_buffer[1] << 8) | \
	                (temp_buffer[2] << 16) | (temp_buffer[3] << 24);
	printf("Sample rate %u .\n",wave_header.sample_rate);
	// read byte rate
	ret = fread(temp_buffer, sizeof(temp_buffer), 1, file_ptr);
	wave_header.byterate = temp_buffer[0] | (temp_buffer[1] << 8) | \
	                  (temp_buffer[2] << 16) | (temp_buffer[3] << 24);
	printf("Byte rate %u.\n",wave_header.byterate);
	// read block allignment 
	ret = fread(temp_buffer_2, sizeof(temp_buffer_2), 1, file_ptr);
	wave_header.block_align = temp_buffer_2[0] | temp_buffer_2[1] << 8 ;
	printf("Block alignment %u .\n", wave_header.block_align);
	// get bit's per sample
	ret = fread(temp_buffer_2, sizeof(temp_buffer_2), 1, file_ptr);
	wave_header.bits_per_sample = temp_buffer_2[0] | temp_buffer_2[1] << 8 ;
	printf("Bit's per sample %u .\n", wave_header.bits_per_sample);
	// read data marker
	ret = fread(wave_header.data_chunk_header, 
				sizeof(wave_header.data_chunk_header),
				1, file_ptr);
	printf("data_chunk_header %s .\n",wave_header.data_chunk_header);
	// read data size
	ret = fread(temp_buffer, sizeof(temp_buffer), 1, file_ptr);
	wave_header.data_size = temp_buffer[0] | (temp_buffer[1] << 8) | \
	                  (temp_buffer[2] << 16) | (temp_buffer[3] << 24);
	printf("Data size %u .\n",wave_header.data_size );
	
	// calculate number of samples; 
	size_of_samples = (wave_header.channels * \
						wave_header.bits_per_sample)/8;		
	number_of_samples = ( 8 * wave_header.data_size) 
				/ (size_of_samples);
	duration_in_sec = (float) wave_header.file_size \
				/ wave_header.byterate;
    printf("%lu samples, each %ld bytes, %f seconds \n",
			number_of_samples, size_of_samples, duration_in_sec);
	
	// Reading all data at once 
	// get actual data size
	fseek(file_ptr, 0, SEEK_END);
	data_size = ftell(file_ptr) - HEADER_SIZE;
	printf("File size is %u %u %u\n", data_size, ftell(file_ptr) );
	fseek(file_ptr, HEADER_SIZE, SEEK_SET);
	// Read all data at once
	if (!READ_in_CHUNKS){
		data_temp_buffer = (char *) malloc(data_size );
		ret = fread(data_temp_buffer, NUM_BYTES , data_size/NUM_BYTES, 
					file_ptr);
		printf("Num samp is %d, read is %d \n", data_size/NUM_BYTES, 
					ret);
		snd_pcm_writei(handle, data_temp_buffer, data_size/NUM_BYTES);
		free(data_temp_buffer);
		
	} 
	else{
		// reading chunks
		// allocate temp_buffer for each chunk
		data_temp_buffer = (char *) malloc(NUM_BYTES * frames);
		// get the number of samples/chunks
		number_of_samples = data_size/ (NUM_BYTES *frames);
		
		for (i=0; i< number_of_samples; i++){
			
			ret = fread(data_temp_buffer, NUM_BYTES ,frames, file_ptr);
			printf("Reading sample %d of %d %lu samples \n",i,\
				number_of_samples , sizeof(data_temp_buffer));
			if (ret == 0) {
				fprintf(stderr, "file reading error \n");
				break;
			} 
			ret = snd_pcm_writei(handle, data_temp_buffer, frames);
			if (ret == -EPIPE) {
				///* EPIPE means underrun */
				fprintf(stderr, "underrun occurred\n");
				snd_pcm_prepare(handle);
			} else if (ret < 0) {
				fprintf(stderr,
				  "error from writei: %s\n",
				  snd_strerror(ret));
			}  else if (ret != (int)frames) {
				fprintf(stderr, 
				  "short write, write %d frames\n", ret);
			}
		}
	}
	free(data_temp_buffer);
	fclose(file_ptr);
	return 0;
}
