/*
 * A simple code to read from a wav file 
 * and play it. 
 * 
 * Adopted from the linux journal 
 * https://www.linuxjournal.com/article/6735
 * 
 * */
 
// Tell Alsa to use newer version of its API
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include "wave_play.h"

#define SECOND 1000000
#define file_name "sample_wav.wav"


/*
 * @main program. It opens a PCM devices, initializes paramters and 
 *  makes a call to the wave_play program by passing a filename, a PCM    
 *  handle and number of frames. 
 *
 * */
int main(){
	
	// PCM device handle
	snd_pcm_t *handle;
	// Structure used to configure the PCM device
	snd_pcm_hw_params_t *params;
	// name of the device, default for now
	char * device_name =  "default";
	// Variable to store return values of functions
	int return_val;
	// frames used to determine size of a period
	snd_pcm_uframes_t frames;
	// Buffer to store data to be captured or played
	char *buffer;
	//char * file_name = "sample_wav.wav";
	unsigned int size, sample_rate, period_time, loop_time, num_loops;
	
	// open PCM device 
	if ( (return_val = snd_pcm_open(&handle, device_name, 
                    SND_PCM_STREAM_PLAYBACK, 0)) < 0){
		fprintf(stderr, 
            "unable to open pcm device %s : %s\n", device_name \
            , snd_strerror(return_val));
		exit(1);
	}
	fprintf(stdout, "PCM Device \"%s\" successfully opened\n" \
			, device_name );

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
	sample_rate = 8000;
	if (snd_pcm_hw_params_set_rate_near(handle, params, 
                                  &sample_rate, 0) < 0){
		fprintf(stderr, 
            "unable to set sample rate to %d ",sample_rate);
		exit(1);
	}							  
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
	// get actual period size. It could be different from 
	// the previously set value. 
	snd_pcm_hw_params_get_period_size(params, &frames,0);
    fprintf(stdout,"Got a period size of %d \n", frames);
	
	// This function will open a wav file and play it using
	// the handle. 
	read_wave_and_play(file_name, handle, frames);
	// Drain and close the PCM handle
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	
	return 0;
	
}
