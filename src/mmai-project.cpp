//============================================================================
// Name        : mmai-project.cpp
//============================================================================

#include <iostream>
#include <cassert>

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

#include "revmodel.hpp"

using namespace std;

#define INFILE "infile.wav"
#define OUTFILE "outfile.wav"

void show_help(string name) {
	cout << "Usage: " << name << endl;
}

int main(int argc, char *argv[]) {
	revmodel model;
	SNDFILE *infile, *outfile;
	SF_INFO info;
	int num, num_items;
	float *inbuf, *outbuf;
	int f, sr, c;
	
	/* Parse arguments */
	for (int i = 1; i < argc; i++) {
		string arg(argv[i]);
		if (arg == "-h" || arg == "--help") {
			show_help(argv[0]);
			return 0;
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

	/* Allocate space for the data to be read, then read it. */
	inbuf = (float *)malloc(num_items*sizeof(float));
	outbuf = (float *)malloc(num_items*sizeof(float));
	num = sf_read_float(infile,inbuf,num_items);
	assert(num == num_items);
	sf_close(infile);
	
	
	/* Apply reverb filter */
	model.processreplace(inbuf, inbuf, outbuf, outbuf, num_items, 1);
	
	
	/* Write to wav file*/
	outfile = sf_open(OUTFILE, SFM_WRITE, &info);
	if (outfile) {
		sf_count_t count = sf_write_float(outfile, outbuf, num_items);
		assert(count == num_items);
		sf_write_sync(outfile);
		sf_close(outfile);
	}

	free(inbuf);
	free(outbuf);

	cout << "File processed" << endl;
	return 0;
}
