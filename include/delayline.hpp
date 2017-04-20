

#ifndef _delayline_
#define _delayline_
#include "denormals.h"

class delayline
{
public:
					delayline();
			void	setbuffer(float *buf, int size);
	inline  float	process(float inp);
			void	mute();
			
// private:
	float	*buffer;
	int		bufsize;
	int		bufidx;
};


// Big to inline - but crucial for speed

inline float delayline::process(float input)
{
	float output;
	
	output = buffer[bufidx];	// read operation
	undenormalise(output);
	
	buffer[bufidx++] = input;	// write operation
	
	if(bufidx >= bufsize){bufidx -= bufsize;}	// wrap bufidx if needed
	
	return output;
}

#endif//_delayline

//ends