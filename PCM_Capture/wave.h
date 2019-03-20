
/*
 * 
 * Wave.h 
 * A header file for a simple wav file parser
 * Adopted from 
 * http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
 * 
 * */


// A structure to hold header of a wave file
struct WAV_HEADER{
	
	uint32_t riff;             //Bytes for RIFF file format
	uint32_t file_size;            // An int (4 bytes) for file size
	uint32_t wave;             // Chars for file headers
	uint32_t fmt_chunk_marker;       // fmt string data
	uint32_t length_of_fmt;        // length of format data
	uint16_t format_type;          // format type
	uint16_t channels; 
	uint32_t sample_rate;
	uint32_t byterate;
	uint16_t block_align;
	uint16_t bits_per_sample; 
	uint32_t data_chunk_header ;
	uint32_t data_size; 
	
};


int read_wave_and_play(char *file_name, snd_pcm_t *handle, 
			snd_pcm_uframes_t frames);
		


int write_wave(char *buffer, snd_pcm_t *handle, 
			snd_pcm_hw_params_t *params, int frames, int length);
			

