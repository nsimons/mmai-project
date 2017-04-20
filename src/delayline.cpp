

#include "delayline.hpp"

delayline::delayline()
{
	bufidx = 0;
}

void delayline::setbuffer(float *buf, int size) 
{
	buffer = buf; 
	bufsize = size;
}

void delayline::mute()
{
	for (int i=0; i<bufsize; i++)
		buffer[i]=0;
}



//ends