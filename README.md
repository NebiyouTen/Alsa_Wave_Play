# Alsa_Wave_Playback and Capture

A simple C program that uses the ALSA audio library to play and record wav files. [Here](https://www.alsa-project.org/wiki/Download) is the link to the ALSA linux library. 

## PCM_Playback
The program parses wav files and writes the data buffer to a sound card. Parsing wav files and headers was based on [this](http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/) tutorial. 

## PCM_Capture
The program opens an audio stream form an UAC222 audio device and writes it to a wav file. Wav file header is generated and 
data will be buffered to the file. The output of this capture can be played back with the above program. 

To run use the following commands
```
./gcc_compile_script && ./PCM_Wav_Play
```

```
./gcc_compile_script && ./PCM_Capture
```


