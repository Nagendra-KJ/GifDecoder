#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>
#include "gifdec.h"

typedef struct YUV_Buffer
{
	int yuvwidth;
	int yuvheight;
	int nFrames;
	int currentFrame;
	uint8_t *buffer;
	gd_GIF *gif;
}YUV_Buffer;

YUV_Buffer* Initialise(const char *fileName);
uint8_t* FetchFrame(YUV_Buffer *buffer);
void Deinitialise(YUV_Buffer *buffer);

#endif
