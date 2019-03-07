CC = gcc
CFLAGS = -Wall
DEPS = wave.h
OBJ = PCM_Wav_Play.o wave.o

%.o %.c $(DEPS)
        $(cc) $(CFLAGS) -c -o $@ $<

PCM_Wav_Play : $(OBJ)
        gcc $(CFLAGS) -o $@ $^
