/*
 * A simple progrma that records a sound.  
 * 
 * Adopted from the linux journal 
 * https://www.linuxjournal.com/article/6735
 * 
 * */
 
// Tell alsa to include the new library
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include "wave.h"

#define SAMPLE_RATE 8000
#define SECOND 1000000
#define NUMSECONDS 20

int main(){

	// PCM device handle
	snd_pcm_t *handle;
	// Structure used to configure the PCM device
	snd_pcm_hw_params_t *params;
	// name of the device, default for now
	char * device_name =  "plug:default";
	// Variable to store return values of functions
	int return_val;
	// frames used to determine size of a period
	snd_pcm_uframes_t frames;
	// Buffer to store data to be captured or played
	char *buffer;
	//char * file_name = "sample_wav.wav";
	unsigned int size, sample_rate, period_time, loop_time, num_loops;
	
	// open pcm for capturing
	if ( (return_val = snd_pcm_open(&handle, "plug:default", 
						SND_PCM_STREAM_CAPTURE,0)) < 0 ){
		fprintf(stderr, 
            "unable to open pcm device %s : %s\n", device_name \
            , snd_strerror(return_val));
		exit(1);
	
	}
	
	printf("Devie '%s' opened successfully \n", device_name);
	
	// Allocate params 
	snd_pcm_hw_params_alloca(&params);
	// Initialize with default values
	snd_pcm_hw_params_any(handle, params);
	
	
	// Set Configuration values in params
	// set access type to RW interleaved and 
	// check function callback 
	if (snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED) < 0 ){
		fprintf(stderr, 
            "unable to set access type to RW Interleaved");
		exit(1);					
	}
	// Set the data format to Signed 16-bit little-endian 
	if (snd_pcm_hw_params_set_format(handle, params,
                              SND_PCM_FORMAT_S16_LE) < 0 ){
		fprintf(stderr, 
            "unable to set format to Signed 16-bit little-endian");
		exit(1);							 	
	}
	// set setrio channels
	if (snd_pcm_hw_params_set_channels(handle, params, 2) < 0 ){
		fprintf(stderr, 
            "unable to set channel to sterio");
		exit(1);
	}
	// sample rate of sample wav file is 256000 bits/second 
	// Invalid argument error if Sample rate is higher than 200,000
	sample_rate = SAMPLE_RATE;
	if (snd_pcm_hw_params_set_rate_near(handle, params, 
                                  &sample_rate, 0) < 0){
		fprintf(stderr, 
            "unable to set sample rate to %d ",sample_rate);
		exit(1);
	}	
	
	fprintf(stdout,"sample rate is = %d \n", sample_rate);
	
							  
	// set period size to 32 frames. A frame has 2 samples, left and 
	// right samples. A sample has 2 bytes, most and least significant. 
	frames = 64;
	fprintf(stdout,"Set number of frames is %d \n", frames);
	if (snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, 0) < 0){
		fprintf(stderr, 
            "unable to set period size to %d frames",frames);
		exit(1);						  	
	}
    // Finally, write the params to the actual hardware device
	if ((return_val = snd_pcm_hw_params(handle, params)) < 0) {
		fprintf(stderr, 
            "unable to set hw parameters: %s\n",
            snd_strerror(return_val));
		exit(1);
	}
	
	
	// get buffer size for one period 
	snd_pcm_hw_params_get_period_size(params,&frames,0);
	fprintf(stdout,"Got 1 a period size of %d \n", frames);
	
	
	//size will be 4 * period size.
	size = frames * 4;
	// allocate a buffer with size of 'size'
	fprintf(stdout, "Malloc with size %d \n", size);
	
	buffer = (char *) malloc(size); 
	
	// get one period time
	snd_pcm_hw_params_get_period_time(params, &loop_time, 0);
	
	num_loops = NUMSECONDS * SECOND / loop_time;
	
	return_val = write_wave(buffer, handle,params, frames, num_loops);
		
	// Drain and close the PCM handle
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	
	if (return_val < 0){
		fprintf(stderr, "error in writing a file \n", return_val);
		exit(1);
	}

	
	return 0;

}
