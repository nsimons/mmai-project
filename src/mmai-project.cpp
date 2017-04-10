//============================================================================
// Name        : mmai-project.cpp
//============================================================================

#include <iostream>
#include "revmodel.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

using namespace std;

int main() {
	revmodel model;
	
	SNDFILE *infile, *outfile;;
	SF_INFO info;
	int num, num_items;
	float *inbuf, *outbuf;
	int f, sr, c;
	
	
	/* Open the WAV file */
	info.format = 0;
	infile = sf_open("infile.wav",SFM_READ, &info);
	if(infile == NULL)
	{
		printf("Failed to open the file.\n");
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
	sf_close(infile);
	
	
	/* Apply reverb filter */
	model.processreplace(inbuf, inbuf, outbuf, outbuf, num_items, 1);
	
	
	/* Write to wav file*/
	outfile = sf_open("outfile.wav",SFM_WRITE, &info);
	sf_count_t count = sf_write_float(outfile, outbuf, num_items);
	sf_write_sync(outfile);
	sf_close(outfile);
	
	free(inbuf);
	free(outbuf);

	cout << "File processed" << endl;
	return 0;
}
