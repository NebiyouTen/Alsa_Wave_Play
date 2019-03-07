
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
	
	unsigned char riff[4];             //Bytes for RIFF file format
	unsigned int file_size;            // An int (4 bytes) for file size
	unsigned char wave[4];             // Chars for file headers
	unsigned char fmt_chunk_marker[4]; // fmt string data
	unsigned int length_of_fmt;        // length of format data
	unsigned int format_type;          // format type
	unsigned int channels; 
	unsigned int sample_rate;
	unsigned int byterate;
	unsigned int block_align;
	unsigned int bits_per_sample; 
	unsigned char data_chunk_header [4];
	unsigned int data_size; 
	
};


int read_wave_and_play(char *file_name, snd_pcm_t *handle, 
			snd_pcm_uframes_t frames);
