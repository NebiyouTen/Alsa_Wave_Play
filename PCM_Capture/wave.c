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
#include "wave.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#define TRUE 1
#define FALSE 0
#define READ_in_CHUNKS 1
#define HEADER_SIZE 44
#define NUM_BYTES 4

// struct for the
struct WAV_HEADER wave_header; 
// file ptr
FILE *file_ptr; 
#define FILENAME "test.wav";

/*
 * @Function to write a wav file and play it using
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
int write_wave(char *buffer, snd_pcm_t *handle, 
					snd_pcm_hw_params_t *params, 
					int frames, int length){
		
	int return_val, sample_rate, data_length; 
	char *file_name = FILENAME;
	
	fprintf(stdout,"Wave file writing started %lu \n"
					, sizeof(wave_header));
	
	file_ptr = fopen(file_name , "w+");
	
	/*
	 * Write header first
	 * 
	 * */
	
	fprintf(stdout, "Allocated wave header %lu \n", sizeof(wave_header));
	
	// riff header: hex encoded string for riff
	wave_header.riff = htonl(0X52494646); // riff header 
	// write chunk size, zero for now will be updated once file written
	wave_header.file_size = 0;
	// write WAVE to format, encoded string
	wave_header.wave = htonl(0X57415645);
	// fmt chunk marker encoded string "fmt"
	wave_header.fmt_chunk_marker = htonl(0X666d7420);
	wave_header.length_of_fmt = 16;
	wave_header.format_type = 1; // PCM
	wave_header.channels = 2;
	wave_header.sample_rate = 8000;
	wave_header.byterate = 4 * 8000;
	wave_header.block_align = 4;
	wave_header.bits_per_sample = 16;
	wave_header.data_chunk_header = htonl(0x64617461); // "data"
	wave_header.data_size = 0; // fill it at the end
	
	fwrite(&wave_header, sizeof(wave_header), 1, file_ptr);
	
	
	/*
	 * Write data now 
	 * 
	 * */
	data_length = length;
	while (length > 0 ){
		
		length -= 1; 
		if ( (return_val = snd_pcm_readi(handle, buffer, frames)) 
								== -EPIPE ){
			// overrun occurs
			fprintf(stderr, "overrun occured \n");
			snd_pcm_prepare(handle);
		}else if(return_val < 0){
			fprintf(stderr, "error from read: %s \n", snd_strerror(return_val));
		}else if( return_val != (int)frames ){
			fprintf(stderr, "short write %d != %d bytes \n. ",return_val,(int)frames);
		}
		
		return_val = fwrite(buffer, frames , 4, file_ptr);
		
		if (return_val != 4){
		
			printf(" Write to file error %d != %d \n", return_val, frames);
			fclose(file_ptr);
			return -1;
			
		}
	
	}
	
	// move pointer to beggning of file with offset of 4
	// meaning points to file size
	fseek(file_ptr, NUM_BYTES , SEEK_SET);
	// set file size
	data_length = data_length * frames * NUM_BYTES 
			+ (HEADER_SIZE - 2 * NUM_BYTES);
	fwrite(&data_length, 1, sizeof(uint32_t), file_ptr);
	
	// move pointer to point to data size header
	fseek(file_ptr, HEADER_SIZE -  NUM_BYTES, SEEK_SET);
	data_length -= + (HEADER_SIZE - 2 * NUM_BYTES);
	fwrite(&data_length, 1, sizeof(uint32_t), file_ptr);
	
	fprintf(stdout, "Final datasize is %d \n",data_length);
	
	fclose(file_ptr);
	return 0;
}
