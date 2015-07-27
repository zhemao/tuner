#ifndef __PLAYTONE_H__
#define __PLAYTONE_H__

#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 64
#define MAX_TABLE_SIZE 410
#define BASE_FREQ 440.0

enum note {
	NOTE_A,
	NOTE_AS,
	NOTE_B,
	NOTE_C,
	NOTE_CS,
	NOTE_D,
	NOTE_DS,
	NOTE_E,
	NOTE_F,
	NOTE_FS,
	NOTE_G,
	NOTE_GS,
};

struct playback_userdata {
	float table[MAX_TABLE_SIZE];
	unsigned int length;
	int phase;
};

struct playback_handle {
	PaStreamParameters outParams;
	PaStream *stream;
	struct playback_userdata userdata;
};

int init_playback(struct playback_handle *handle);
int play_tone(struct playback_handle *handle, enum note note, int octave);
int stop_tone(struct playback_handle *handle);
void terminate_playback(struct playback_handle *handle);

#endif
