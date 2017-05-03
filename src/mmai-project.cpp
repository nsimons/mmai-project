//============================================================================
// Name        : mmai-project.cpp
//============================================================================

#include <iostream>
#include <cassert>

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <portaudio.h>

#include "revmodel.hpp"

using namespace std;

#define INFILE "infile.wav"
#define OUTFILE "outfile.wav"
#define SAMPLE_RATE (44100)

revmodel model;

typedef struct
{
	float *left_in;
	float *right_in;
	float *left_out;
	float *right_out;
	int idx;
	int maxidx;
}
paTestData;

void show_help(string name) {
	cout << "Usage: " << name << " [options [..]]\n"
		 << "Options:\n"
		 << "-h, --help\t Show help\n"
		 << "--wet WET\t Set the wet coefficient (float) (default: " << initialwet << ")\n"
		 << "--dry DRY\t Set the dry coefficient (float) (default: " << initialdry << ")\n"
		 << "--damp DAMP\t Set the damp coefficient (float) (default: " << initialdamp << ")\n"
		 << "--size SIZE\t Set the late reverberation size coefficient (float) (default: " << initialroom << ")\n"
		 << endl;
}

static int patestCallback( const void *inputBuffer, void *outputBuffer,
						   unsigned long framesPerBuffer,
						   const PaStreamCallbackTimeInfo* timeInfo,
						   PaStreamCallbackFlags statusFlags,
						   void *userData )
{    
	/* Cast data passed through stream to our structure. */
	paTestData *data = (paTestData*)userData;
	float *lptrIn = &data->left_in[data->idx];
	float *rptrIn = &data->right_in[data->idx];
	float *lptrOut = &data->left_out[data->idx];
	float *rptrOut = &data->right_out[data->idx];
	float *out = (float*)outputBuffer;
	unsigned int i;
	int finished;
	unsigned int framesLeft = data->maxidx - data->idx;
	
	(void) inputBuffer; /* Prevent unused variable warning. */
	(void) timeInfo;
	(void) statusFlags;
	
	model.processreplace(lptrIn, rptrIn, lptrOut, rptrOut, framesPerBuffer, 1);				 
	
	if(framesLeft < framesPerBuffer)
	{
		/* final buffer... */
		for(i=0; i<framesLeft; i++)
		{
			*out++ = *lptrOut++; // left
			*out++ = *rptrOut++; // right
		}
		for(; i<framesPerBuffer; i++)
		{
			*out++ = 0; // left
			*out++ = 0; // right
		}
		data->idx += framesLeft;
		finished = paComplete;
	}
	else
	{
		for(i=0; i<framesPerBuffer; i++)
		{
			*out++ = *lptrOut++; // left
			*out++ = *rptrOut++; // right
		}
		data->idx += framesPerBuffer;
		finished = paContinue;
	}
		
		return finished;
}

int main(int argc, char *argv[]) {
	
	SNDFILE *infile, *outfile;
	SF_INFO info;
	int num, num_items;
	float *inbuf, *outbuf;
	int f, sr, c;
	
	PaStream *stream;
	PaError err = paNoError;
	paTestData *data = (paTestData *)malloc(sizeof(paTestData));
	
	/* Parse arguments */
	for (int i = 1; i < argc; i++) {
		string arg(argv[i]);
		if (arg == "-h" || arg == "--help") {
			show_help(argv[0]);
			return 0;
		}
		else if (arg == "--wet") {
			assert(++i != argc);
			float val = strtof(argv[i], NULL);
			cout << "Setting wet to " << val << endl;
			model.setwet(val);
		}
		else if (arg == "--dry") {
			assert(++i != argc);
			float val = strtof(argv[i], NULL);
			cout << "Setting dry to " << val << endl;
			model.setdry(val);
		}
		else if (arg == "--damp") {
			assert(++i != argc);
			float val = strtof(argv[i], NULL);
			cout << "Setting damp to " << val << endl;
			model.setdamp(val);
		}
		else if (arg == "--size") {
			assert(++i != argc);
			float val = strtof(argv[i], NULL);
			cout << "Setting late reverberation size to " << val << endl;
			model.setroomsize(val);
		}
		else {
			cout << "Invalid argument: " << arg << endl;
			show_help(argv[0]);
			return 0;
		}
	}
	
	/* Open the WAV file */
	info.format = 0;
	infile = sf_open(INFILE, SFM_READ, &info);
	if(infile == NULL)
	{
		printf("Failed to open the file: %s\n", INFILE);
		exit(-1);
	}
	/* Read info, and figure out how much data to read. */
	f = info.frames;
	sr = info.samplerate;
	c = info.channels;
	num_items = f*c;
	
	printf("channels: %d\n",c);

	/* Allocate space for the data to be read, then read it. */
	inbuf = (float *)malloc(num_items*sizeof(float));
	outbuf = (float *)malloc(num_items*sizeof(float));
	num = sf_read_float(infile,inbuf,num_items);
	assert(num == num_items);
	sf_close(infile);
	
	/* Separate left and right audio channels */
	data->left_in = new float[f];
	data->right_in = new float[f];
	data->left_out = new float[f];
	data->right_out = new float[f];
	data->idx = 0;
	data->maxidx = f;
	for(int i=0; i<f; i++){
		data->left_in[i] = inbuf[data->idx];
		if(c == 2){data->idx++;}
		data->right_in[i] = inbuf[data->idx++];
	}
	
	
	/* Playback */
	
	/* Initialize */
	data->idx = 0;
	err = Pa_Initialize();
	if(err != paNoError) goto error;
	
	/* Open audio I/O stream */
	err = Pa_OpenDefaultStream(&stream,
							   0,		// no input channels
							   2,		// stereo outputBuffer
							   paFloat32,	// 32 bit floating point output
							   SAMPLE_RATE,
							   1024,//paFramesPerBufferUnspecified,		// frames per buffer
							   patestCallback,	// callback function
							   data );	// pointer that will be passed to callback
	if(err != paNoError) goto error;
	
	if(stream)
	{
		err = Pa_StartStream(stream);
		if(err != paNoError) goto error;
		
		printf("Waiting for playback to finish\n"); fflush(stdout);
		
		while((err = Pa_IsStreamActive(stream)) == 1) Pa_Sleep(100);
		if(err < 0) goto error;
		
		err = Pa_CloseStream(stream);
		if(err != paNoError) goto error;
		
		printf("Done\n"); fflush(stdout);
	}
	
error:	
	err = Pa_Terminate();
	if(err != paNoError)
	{
		printf("PortAudio error: %s\n", Pa_GetErrorText(err));
	}
	
	
	/* Apply reverb filter */
/*	model.processreplace(data->left_in, data->right_in, data->left_out, data->right_out, f, 1);
	data->idx = 0;
	for(int i=0; i<f; i++){	
		outbuf[data->idx++] = data->left_out[i];	// left 
		outbuf[data->idx++] = data->right_out[i];	// right 
	}
*/	
	
	/* Write to wav file*/
/*	outfile = sf_open(OUTFILE, SFM_WRITE, &info);
	if (outfile) {
		sf_count_t count = sf_write_float(outfile, outbuf, num_items);
		assert(count == num_items);
		sf_write_sync(outfile);
		sf_close(outfile);
	}
*/
	free(inbuf);
	free(outbuf);
	free(data);

	cout << "File processed" << endl;
	return 0;
}
