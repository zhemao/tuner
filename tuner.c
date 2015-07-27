#include "playtone.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static int parse_note(char *str)
{
	char base = str[0];
	char mod = str[1];
	enum note note;

	switch (base) {
	case 'a':
	case 'A':
		note = NOTE_A;
		break;
	case 'b':
	case 'B':
		note = NOTE_B;
		break;
	case 'c':
	case 'C':
		note = NOTE_C;
		break;
	case 'd':
	case 'D':
		note = NOTE_D;
		break;
	case 'e':
	case 'E':
		note = NOTE_E;
		break;
	case 'f':
	case 'F':
		note = NOTE_F;
		break;
	case 'g':
	case 'G':
		note = NOTE_G;
		break;
	default:
		return -1;
	}

	if (mod == '-')
		note--;
	else if (mod == '+')
		note++;

	return note;
}

static void print_usage(FILE *stream, const char *progname)
{
	fprintf(stream, "Usage: %s [-n note] [-d time] [-o octave]\n", progname);
}

int main(int argc, char *argv[])
{
	struct playback_handle handle;
	int err, opt;
	enum note note = NOTE_A;
	double duration = 0.0;
	int octave = 0;
	sigset_t sigset;
	siginfo_t siginfo;

	while ((opt = getopt(argc, argv, "n:d:o:h")) != -1) {
		switch (opt) {
		case 'n':
			note = parse_note(optarg);
			break;
		case 'd':
			duration = atof(optarg);
			break;
		case 'o':
			octave = atoi(optarg);
			break;
		case 'h':
			print_usage(stdin, argv[0]);
			return 0;
		default:
			print_usage(stderr, argv[0]);
			return EXIT_FAILURE;
		}
	}

	if (octave < -1) {
		fprintf(stderr, "Note is too low\n");
		return -1;
	}
	if (octave > 1) {
		fprintf(stderr, "Note is too high\n");
		return -1;
	}

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGINT);

	err = init_playback(&handle);
	if (err)
		goto error;

	err = play_tone(&handle, note, octave);
	if (err)
		goto error;

	if (duration == 0) {
		err = sigwaitinfo(&sigset, &siginfo);
		if (err) {
			perror("sigwaitinfo");
			stop_tone(&handle);
			terminate_playback(&handle);
			return err;
		}
	} else {
		Pa_Sleep(duration * 1000);
	}

	err = stop_tone(&handle);
	if (err)
		goto error;

	terminate_playback(&handle);

	return 0;

error:
	terminate_playback(&handle);
	fprintf(stderr, "PortAudio error: (%d) %s\n",
			err, Pa_GetErrorText(err));
	return err;
}
