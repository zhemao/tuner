#include "playtone.h"

#include <math.h>
#include <stdlib.h>

int init_playback(struct playback_handle *handle)
{
	int err;

	err = Pa_Initialize();
	if (err != paNoError)
		return err;

	handle->outParams.device = Pa_GetDefaultOutputDevice();
	if (handle->outParams.device == paNoDevice)
		return -1;

	handle->outParams.channelCount = 2;
	handle->outParams.sampleFormat = paFloat32;
	handle->outParams.suggestedLatency = Pa_GetDeviceInfo(
			handle->outParams.device)->defaultLowOutputLatency;
	handle->outParams.hostApiSpecificStreamInfo = NULL;

	return 0;
}

static int play_tone_callback(const void *inBuffer, void *outBuffer,
		unsigned long frames_per_buffer,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flags, void *data)
{
	struct playback_userdata *userdata = (struct playback_userdata *) data;
	float *out = (float *) outBuffer;
	float value;
	int i;

	for (i = 0; i < frames_per_buffer; i++) {
		value = userdata->table[userdata->phase];
		out[2 * i] = out[2 * i + 1] = value;
		userdata->phase++;
		if (userdata->phase >= userdata->length)
			userdata->phase -= userdata->length;
	}

	return paContinue;
}

int play_tone(struct playback_handle *handle, enum note note, int octave)
{
	double t, freq = BASE_FREQ * pow(2, octave + note / 12.0);
	int i, err;
	struct playback_userdata *userdata = &handle->userdata;

	userdata->length = SAMPLE_RATE / freq;

	for (i = 0; i < userdata->length; i++) {
		t = (double) i / SAMPLE_RATE;
		userdata->table[i] = sinf(2 * M_PI * freq * t);
	}
	userdata->phase = 0;

	err = Pa_OpenStream(
			&handle->stream,
			NULL,
			&handle->outParams,
			SAMPLE_RATE,
			FRAMES_PER_BUFFER,
			paClipOff,
			play_tone_callback,
			&handle->userdata);

	if (err != paNoError)
		return err;

	err = Pa_StartStream(handle->stream);
	if (err != paNoError)
		return err;

	return 0;
}

int stop_tone(struct playback_handle *handle)
{
	int err;

	err = Pa_StopStream(handle->stream);
	if (err != paNoError)
		return err;

	err = Pa_CloseStream(handle->stream);
	if (err != paNoError)
		return err;

	return 0;
}

void terminate_playback(struct playback_handle *handle)
{
	Pa_Terminate();
}
